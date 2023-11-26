#include "geometry_engine.hpp"

#include <QtMath>
#include <QVector3D>

#define EPS 0.00000001

int sign(double a) {
    return (a >= 0)? 1 : -1;
}


void
GeometryEngine::fillSideVertexes(size_t height, size_t baseRadius, const QVector<VertexData> &baseVertexes, QVector<VertexData> &sideVertexes,
                                 size_t baseIndex, size_t sideIndex) const {
    for (size_t j = 0; j < sideVertexesNumber; ++j) {
        double k = std::tan(double(j)*M_PI/double(2*(sideVertexesNumber+1)))*double(height)/double(baseRadius);
        double xy = std::sqrt(double(baseRadius*baseRadius) * double(height*height) / (double(baseRadius*baseRadius) * k*k + double(height*height)));
        double z = std::abs(k) * xy;
        double x, y;
        if (abs(baseVertexes[baseIndex].position.y()) < EPS) {
            y = 0;
            x = xy;
        } else if (abs(baseVertexes[baseIndex].position.x()) < EPS) {
            y = xy;
            x = 0;
        } else {
            y = std::sqrt(baseVertexes[baseIndex].position.y() * baseVertexes[baseIndex].position.y() * xy * xy / (baseVertexes[baseIndex].position.x() * baseVertexes[baseIndex].position.x() + baseVertexes[baseIndex].position.y() * baseVertexes[baseIndex].position.y()));
            x = y * abs(baseVertexes[baseIndex].position.x() / baseVertexes[baseIndex].position.y());
        }
        QVector3D v = QVector3D(sign(baseVertexes[baseIndex].position.x()) * x, sign(baseVertexes[baseIndex].position.y()) * y, z);
        if (j == 0)
            sideVertexes[sideIndex * 2*(sideVertexesNumber-1)].position = v;
        else if (j == sideVertexesNumber - 1)
            sideVertexes[sideIndex * 2*(sideVertexesNumber-1) + 2*j - 1].position = v;
        else {
            sideVertexes[sideIndex * 2*(sideVertexesNumber-1) + 2*j - 1].position = v;
            sideVertexes[sideIndex * 2*(sideVertexesNumber-1) + 2*j].position = v;
        }
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

    baseVAO.create();
    sideVAO.create();
    topVAO.create();
}

GeometryEngine::~GeometryEngine() {
    baseVertexesBuf.destroy();
    sideVertexesBuf.destroy();
    topVertexesBuf.destroy();

    baseIndexesBuf.destroy();
    sideIndexesBuf.destroy();
    topIndexesBuf.destroy();

    baseVAO.destroy();
    sideVAO.destroy();
    topVAO.destroy();
}

void GeometryEngine::initFigureGeometry(QOpenGLShaderProgram *program) {
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
    baseSize = preBaseSize;
    sideSize = preSideSize;
    topSize = preTopSize;

    QVector<VertexData> baseVertexes(baseSize);
    QVector<GLuint> baseIndexes(baseSize);

    QVector<VertexData> sideVertexes(sideSize);
    QVector<GLuint> sideIndexes(sideSize);

    QVector<VertexData> topVertexes(topSize);
    QVector<GLuint> topIndexes(topSize);

    // fill base vertexes
    QVector3D baseNormal{0, 0, -1};
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
        baseIndexes[i] = baseVertexesNumber-i-1;
    }

    // fill side vertexes, normals and indexes
    size_t indexesCounter = 0;
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        fillSideVertexes(height, baseRadius, baseVertexes, sideVertexes, i, 2*i);
        if (i < baseVertexesNumber - 1)
            fillSideVertexes(height, baseRadius, baseVertexes, sideVertexes, i + 1, 2*i + 1);
        else
            fillSideVertexes(height, baseRadius, baseVertexes, sideVertexes, 0, 2*i + 1);
        for (size_t j = 0; j < sideVertexesNumber-1; ++j) {
            QVector<size_t> tempIndexes = {4*i*(sideVertexesNumber-1)+2*j, 4*i*(sideVertexesNumber-1)+2*j+1, 2*(2*i+1)*(sideVertexesNumber-1)+2*j+1, 2*(2*i+1)*(sideVertexesNumber-1)+2*j};
            QVector3D v1 = sideVertexes[tempIndexes[3]].position - sideVertexes[tempIndexes[0]].position;
            QVector3D v2 = sideVertexes[tempIndexes[2]].position - sideVertexes[tempIndexes[3]].position;
            const QVector3D normal = QVector3D::crossProduct(v1, v2).normalized();
            for (auto ind: tempIndexes) {
                sideVertexes[ind].normal = normal;
                sideIndexes[indexesCounter++] = ind;
            }
        }
    }

    indexesCounter = 0;
    // fill top vertexes
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        QVector<size_t> tempIndexes = {3*i, 3*i+1, 3*i+2};
        topVertexes[tempIndexes[0]] = sideVertexes[2*(2*i+2)*(sideVertexesNumber-1)-1];
        topVertexes[tempIndexes[1]] = sideVertexes[2*(2*i+1)*(sideVertexesNumber-1)-1];
        topVertexes[tempIndexes[2]] = {QVector3D(0, 0, height)};
        QVector3D v1 = topVertexes[tempIndexes[0]].position - topVertexes[tempIndexes[1]].position;
        QVector3D v2 = topVertexes[tempIndexes[2]].position - topVertexes[tempIndexes[0]].position;
        const QVector3D normal = QVector3D::crossProduct(v1, v2).normalized();
        for (auto ind: tempIndexes) {
            topVertexes[ind].normal = normal;
            topIndexes[indexesCounter++] = ind;
        }
    }

    int positionLocation = program->attributeLocation("position");
    int normalLocation = program->attributeLocation("normal");
    // bind baseVAO
    baseVAO.bind();
    baseVertexesBuf.bind();
    baseVertexesBuf.allocate(baseVertexes.data(), baseSize * sizeof(VertexData));
    baseIndexesBuf.bind();
    baseIndexesBuf.allocate(baseIndexes.data(), baseSize * sizeof(GLuint));
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(VertexData));
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, sizeof(QVector3D), 3, sizeof(VertexData));
    program->enableAttributeArray(normalLocation);
    baseVAO.release();

    // bind sideVAO
    sideVAO.bind();
    sideVertexesBuf.bind();
    sideVertexesBuf.allocate(sideVertexes.data(), sideSize * sizeof(VertexData));
    sideIndexesBuf.bind();
    sideIndexesBuf.allocate(sideIndexes.data(), sideSize * sizeof(GLuint));
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(VertexData));
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, sizeof(QVector3D), 3, sizeof(VertexData));
    program->enableAttributeArray(normalLocation);
    sideVAO.release();

    // bind topVAO
    topVAO.bind();
    topVertexesBuf.bind();
    topVertexesBuf.allocate(topVertexes.data(), topSize * sizeof(VertexData));
    topIndexesBuf.bind();
    topIndexesBuf.allocate(topIndexes.data(), topSize * sizeof(GLuint));
    program->setAttributeBuffer(positionLocation, GL_FLOAT, 0, 3, sizeof(VertexData));
    program->enableAttributeArray(positionLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, sizeof(QVector3D), 3, sizeof(VertexData));
    program->enableAttributeArray(normalLocation);
    topVAO.release();
}

void GeometryEngine::drawFigureGeometry()
{
    baseVAO.bind();
    glDrawElements(GL_POLYGON, baseSize, GL_UNSIGNED_INT, nullptr);
    baseVAO.release();

    sideVAO.bind();
    glDrawElements(GL_QUADS, sideSize, GL_UNSIGNED_INT, nullptr);
    sideVAO.release();

    topVAO.bind();
    glDrawElements(GL_TRIANGLES, topSize, GL_UNSIGNED_INT, nullptr);
    topVAO.release();
}

void GeometryEngine::setVertexesNumber(size_t baseVertexesNumber, size_t sideVertexesNumber) {
    this->baseVertexesNumber = baseVertexesNumber;
    this->sideVertexesNumber = sideVertexesNumber;
    preBaseSize = baseVertexesNumber;
    preSideSize = 4*baseVertexesNumber*(sideVertexesNumber-1);
    preTopSize = 3*baseVertexesNumber;
}

size_t GeometryEngine::calculateVertexesNumber() {
    return preBaseSize + preSideSize + preTopSize;
}

