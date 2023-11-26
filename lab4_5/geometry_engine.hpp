#ifndef GEOMETRY_ENGINE_H
#define GEOMETRY_ENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>


struct VertexData {
    QVector3D position;
    QVector3D normal;
};

class GeometryEngine : protected QOpenGLFunctions {
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawFigureGeometry();
    void initFigureGeometry(QOpenGLShaderProgram *program);
    void setVertexesNumber(size_t baseVertexesNumber, size_t sideVertexesNumber);
    size_t calculateVertexesNumber();

private:
    void fillSideVertexes(size_t height, size_t baseRadius, const QVector<VertexData> &baseVertexes, QVector<VertexData> &sideVertexes, size_t baseIndex, size_t sideIndex) const;

    size_t baseVertexesNumber, sideVertexesNumber;
    size_t preBaseSize, preSideSize, preTopSize;
    size_t baseSize, sideSize, topSize;

    QOpenGLBuffer baseVertexesBuf, sideVertexesBuf, topVertexesBuf;
    QOpenGLBuffer baseIndexesBuf, sideIndexesBuf, topIndexesBuf;
    QOpenGLVertexArrayObject baseVAO, sideVAO, topVAO;

    const size_t baseRadius = 1, height = 1;
};

#endif // GEOMETRY_ENGINE_H
