#include "prism.hpp"

Prism::Prism(QWidget *parent)
        : QWidget(parent)
{/*
                      z
                      |
                      |
               4------|-----------5
              /       |          /|
             7------------------6 |
             |        |         | |
             |        |         | |
             |        |         | |
             | (0,0,0).---------|--------------x
             |       /          | |
             | 0(-w/2,-w/2,-h/2)| | 1 (w/2,-w/2,-h/2)
             |     /            |/
             3----/-------------2
(-w/2,w/2,-h/2)  /             (w/2,w/2,-h/2)
                /
               y

 */
    vertexes = QVector<QVector4D> {
            QVector4D{-w/2,-w/2,-h/2,1},
            QVector4D{w/2, -w/2, -h/2, 1},
            QVector4D{w/2, w/2, -h/2, 1},
            QVector4D{-w/2, w/2, -h/2, 1},

            QVector4D{-w/2, -w/2, h/2, 1},
            QVector4D{w/2, -w/2, h/2, 1},
            QVector4D{w/2, w/2, h/2, 1},
            QVector4D{-w/2, w/2, h/2, 1}
    };

    planes = QVector<QVector<size_t>> {
            {0,1,2,3},
            {7,6,5,4},
            {0,3,7,4},
            {4,5,1,0},
            {5,6,2,1},
            {6,7,3,2}
    };
}
