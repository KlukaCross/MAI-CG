#ifndef MAI_CG_PRISM_HPP
#define MAI_CG_PRISM_HPP

#include <QWidget>

#include <QVector4D>
#include <QMatrix4x4>

class Prism : public QWidget
{
Q_OBJECT

public:
    QVector<QVector4D> vertexes;
    QVector<QVector<size_t>> planes;
    const float h = 2, w = 1;

public:
    explicit Prism(QWidget *parent = nullptr);

};

#endif //MAI_CG_PRISM_HPP
