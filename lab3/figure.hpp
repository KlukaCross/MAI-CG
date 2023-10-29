#ifndef MAI_CG_FIGURE_HPP
#define MAI_CG_FIGURE_HPP

#include <QVector4D>
#include <QMatrix4x4>

class Figure {
public:
    QVector<QVector4D> baseVertexes;
    QVector<QVector4D> sideVertexes;
    QVector4D topVertex;

public:
    Figure(size_t baseRadius, size_t height, size_t baseVertexesNumber, size_t sideVertexesNumber);
};

#endif //MAI_CG_FIGURE_HPP
