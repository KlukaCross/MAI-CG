#include "geometry_engine.hpp"

#include <QtMath>
#include <QVector2D>
#include <QVector3D>

#define EPS 0.00000001

int sign(double a) {
    return (a >= 0)? 1 : -1;
}

struct VertexData {
    QVector3D position;
    QVector3D normal;
};

GeometryEngine::GeometryEngine() : facesBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    vertexesBuf.create();
    facesBuf.create();

    // Initializes cube geometry and transfers it to VBOs
    initFigureGeometry();
}

GeometryEngine::~GeometryEngine() {
    vertexesBuf.destroy();
    facesBuf.destroy();
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
     *   |\_
     *   |  \_
     *   |    \_
     *   |______\
     *(0,0,0)
     *
     *   TOP VERTEX:
     *   . (0, 0, height)
     */

    QVector<VertexData> baseVertexes, sideVertexes;

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
        baseVertexes.push_back({QVector3D(x, y, 0), baseNormal});
    }

    // fill side vertexes
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        if (sideVertexesNumber == 1) { // then figure is prism
            sideVertexes.push_back({QVector3D(baseVertexes[i].position.x(), baseVertexes[i].position.y(), height), QVector3D(1, 1, 1)});
        }
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
            sideVertexes.push_back({QVector3D(sign(baseVertexes[i].position.x()) * x, sign(baseVertexes[i].position.y()) * y, z), QVector3D(1, 1, 1)});
        }
        // заполнять сразу гранями
    }

    // fill side normals



    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
            // Vertex data for face 0
            {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
            {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

            // Vertex data for face 1
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
            {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
            {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
            {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

            // Vertex data for face 2
            {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
            {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
            {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
            {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

            // Vertex data for face 3
            {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
            {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
            {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

            // Vertex data for face 4
            {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
            {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
            {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

            // Vertex data for face 5
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
            {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
            {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
            {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
            0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
            4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
            8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
            12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
            16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
            20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    // Transfer vertex data to VBO 0
    vertexesBuf.bind();
    vertexesBuf.allocate(vertices, 24 * sizeof(VertexData));

    // Transfer index data to VBO 1
    facesBuf.bind();
    facesBuf.allocate(indices, 34 * sizeof(GLushort));
}

void GeometryEngine::drawFigureGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    vertexesBuf.bind();
    facesBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, nullptr);
}
