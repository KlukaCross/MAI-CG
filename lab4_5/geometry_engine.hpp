#ifndef GEOMETRY_ENGINE_H
#define GEOMETRY_ENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


struct VertexData {
    QVector3D position;
    QVector3D normal;
};

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawFigureGeometry(QOpenGLShaderProgram *program);

private:
    void initFigureGeometry(size_t baseRadius, size_t height, size_t baseVertexesNumber, size_t sideVertexesNumber);
    void fillSideVertexes(size_t height, size_t baseRadius, VertexData *baseVertexes, VertexData *sideVertexes, size_t i) const;

    size_t baseVertexesNumber, sideVertexesNumber;

    QOpenGLBuffer baseVertexesBuf, sideVertexesBuf, topVertexesBuf;
    QOpenGLBuffer baseIndexesBuf, sideIndexesBuf, topIndexesBuf;
};

#endif // GEOMETRY_ENGINE_H