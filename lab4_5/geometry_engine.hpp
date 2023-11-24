#ifndef GEOMETRY_ENGINE_H
#define GEOMETRY_ENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawFigureGeometry(QOpenGLShaderProgram *program);

private:
    void initFigureGeometry(size_t baseRadius, size_t height, size_t baseVertexesNumber, size_t sideVertexesNumber);

    QOpenGLBuffer vertexesBuf;
    QOpenGLBuffer facesBuf;
};

#endif // GEOMETRY_ENGINE_H