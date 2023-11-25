#include "geometry_engine.hpp"

#include <QtMath>
#include <QVector2D>
#include <QVector3D>

#define EPS 0.00000001

int sign(double a) {
    return (a >= 0)? 1 : -1;
}


void
GeometryEngine::fillSideVertexes(size_t height, size_t baseRadius, VertexData *baseVertexes, VertexData *sideVertexes,
                                 size_t i) const {
    for (size_t j = 0; j < sideVertexesNumber; ++j) {
        double k = std::tan(double(j)*M_PI/double(2*(sideVertexesNumber+1)))*double(height)/double(baseRadius);
        double xy = std::sqrt(double(baseRadius*baseRadius) * double(height*height) / (double(baseRadius*baseRadius) * k*k + double(height*height)));
        double z = std::abs(k) * xy;
        double x, y;
        if (abs(baseVertexes[i].position.y()) < EPS) {
            y = 0;
            x = xy;
        } else if (abs(baseVertexes[i].position.x()) < EPS) {
            y = xy;
            x = 0;
        } else {
            y = std::sqrt(baseVertexes[i].position.y()*baseVertexes[i].position.y() * xy*xy / (baseVertexes[i].position.x()*baseVertexes[i].position.x() + baseVertexes[i].position.y()*baseVertexes[i].position.y()));
            x = y * abs(baseVertexes[i].position.x() / baseVertexes[i].position.y());
        }
        sideVertexes[i*baseVertexesNumber+j] = {QVector3D(sign(baseVertexes[i].position.x()) * x, sign(baseVertexes[i].position.y()) * y, z)};
    }
}

GeometryEngine::GeometryEngine() : baseIndexesBuf(QOpenGLBuffer::IndexBuffer), sideIndexesBuf(QOpenGLBuffer::IndexBuffer), topIndexesBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    baseVertexesBuf.create();
    sideVertexesBuf.create();
    topVertexesBuf.create();
    baseIndexesBuf.create();
    sideIndexesBuf.create();
    topIndexesBuf.create();

    initFigureGeometry(1, 1, 10, 10);
}

GeometryEngine::~GeometryEngine() {
    baseVertexesBuf.destroy();
    sideVertexesBuf.destroy();
    topVertexesBuf.destroy();
    baseIndexesBuf.destroy();
    sideIndexesBuf.destroy();
    topIndexesBuf.destroy();
}

void GeometryEngine::initFigureGeometry(size_t baseRadius, size_t height, size_t baseVertexesNumber, size_t sideVertexesNumber) {
    /*   BASE:
     *   _______
     *  /       \
     * | (0,0,0)-|------> x
     *  \___|___/
     *      |
     *     \|/
     *      y
     *
     *   ONE SIDE:
     *   z
     *  /|\
     *   |
     *   |
     *   |  \_
     *   |    \_
     *   |______\
     *(0,0,0)
     *
     *   TOP VERTEXES:
     *   . (0, 0, height)
     *   |\
     */
    this->baseVertexesNumber = baseVertexesNumber;
    this->sideVertexesNumber = sideVertexesNumber;

    VertexData baseVertexes[baseVertexesNumber];
    GLushort baseIndexes[baseVertexesNumber];

    VertexData sideVertexes[2*baseVertexesNumber*sideVertexesNumber];
    GLushort sideIndexes[2*baseVertexesNumber*sideVertexesNumber];

    VertexData topVertexes[3*baseVertexesNumber];
    GLushort topIndexes[3*baseVertexesNumber];

    // fill base vertexes
    QVector3D baseNormal{0, 0, 1};
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        double angle = 2*M_PI*double(i)/double(baseVertexesNumber);
        double k = qTan(angle);
        float x = qSqrt(double(baseRadius*baseRadius)/(k*k+1));
        float y = qAbs(k)*x;
        if (angle > M_PI_2 && angle <= M_PI) {
            x = -x;
        } else if (angle > M_PI && angle <= 3*M_PI_2) {
            x = -x;
            y = -y;
        } else if (angle > 3*M_PI_2) {
            y = -y;
        }
        baseVertexes[i] = {QVector3D(x, y, 0), baseNormal};
        baseIndexes[i] = i;
    }

    // fill side vertexes, normals and indexes
    size_t indexesCounter = 0;
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        fillSideVertexes(height, baseRadius, baseVertexes, sideVertexes, i);
        if (i < baseVertexesNumber - 1)
            fillSideVertexes(height, baseRadius, baseVertexes, sideVertexes, i + 1);
        else
            fillSideVertexes(height, baseRadius, baseVertexes, sideVertexes, 0);
        // fill normals
        for (size_t j = 0; j < sideVertexesNumber-1; ++j) {
            QVector3D v1 = sideVertexes[2*i*sideVertexesNumber+1].position - sideVertexes[2*i*sideVertexesNumber].position;
            QVector3D v2 = sideVertexes[(2*i+1)*sideVertexesNumber].position - sideVertexes[2*i*sideVertexesNumber].position;
            const QVector3D normal = QVector3D::crossProduct(v1, v2).normalized();
            sideVertexes[2*i*sideVertexesNumber].normal = normal;
            sideVertexes[2*i*sideVertexesNumber+1].normal = normal;
            sideVertexes[(2*i+1)*sideVertexesNumber].normal = normal;
            sideVertexes[(2*i+1)*sideVertexesNumber+1].normal = normal;
        }
        // fill indexes
        sideIndexes[indexesCounter] = 2*i*sideVertexesNumber;
        sideIndexes[indexesCounter+1] = (2*i+1)*sideVertexesNumber;
        sideIndexes[indexesCounter+2] = (2*i+1)*sideVertexesNumber+1;
        sideIndexes[indexesCounter+3] = 2*i*sideVertexesNumber+1;
        indexesCounter += 4;
    }

    indexesCounter = 0;
    // fill top vertexes
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        topVertexes[3*i] = sideVertexes[(2*i+1)+sideVertexesNumber-1];
        topVertexes[3*i+1] = {QVector3D(0, 0, height)};
        topVertexes[3*i+2] = sideVertexes[(2*i+2)+sideVertexesNumber-1];
        QVector3D v1 = topVertexes[3*i+2].position - topVertexes[3*i].position;
        QVector3D v2 = topVertexes[3*i+1].position - topVertexes[3*i+2].position;
        const QVector3D normal = QVector3D::crossProduct(v1, v2).normalized();
        topVertexes[3*i].normal = normal;
        topVertexes[3*i+1].normal = normal;
        topVertexes[3*i+2].normal = normal;
    }

    baseVertexesBuf.bind();
    baseVertexesBuf.allocate(baseVertexes, baseVertexesNumber * sizeof(VertexData));

    sideVertexesBuf.bind();
    sideVertexesBuf.allocate(sideVertexes, 2*baseVertexesNumber*sideVertexesNumber * sizeof(VertexData));

    topVertexesBuf.bind();
    topVertexesBuf.allocate(topVertexes, 3*baseVertexesNumber * sizeof(VertexData));

    baseIndexesBuf.bind();
    baseIndexesBuf.allocate(baseIndexes, baseVertexesNumber * sizeof(GLushort));

    sideIndexesBuf.bind();
    sideIndexesBuf.allocate(sideIndexes, 2*baseVertexesNumber*sideVertexesNumber * sizeof(GLushort));

    topIndexesBuf.bind();
    topIndexesBuf.allocate(topIndexes, 3*baseVertexesNumber * sizeof(GLushort));
}

void GeometryEngine::drawFigureGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    baseVertexesBuf.bind();
    //sideVertexesBuf.bind();
    //topVertexesBuf.bind();

    baseIndexesBuf.bind();
    //sideIndexesBuf.bind();
    //topIndexesBuf.bind();

    quintptr offset = 0;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    int vertexLocation = program->attributeLocation("position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    //size_t *offset = 0;
    glDrawElements(GL_POLYGON, baseVertexesNumber, GL_UNSIGNED_SHORT, nullptr);
    //glDrawElements(GL_QUADS, 2*baseVertexesNumber*sideVertexesNumber, GL_UNSIGNED_SHORT, offset);
    //glDrawElements(GL_TRIANGLES, 3*baseVertexesNumber, GL_UNSIGNED_SHORT, offset);
}

