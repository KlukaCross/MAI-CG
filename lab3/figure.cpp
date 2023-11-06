#include <cmath>
#include "figure.hpp"
#define EPS 0.00000001

int sign(double a) {
    return (a >= 0)? 1 : -1;
}

Figure::Figure(size_t baseRadius, size_t height, size_t baseVertexesNumber, size_t sideVertexesNumber) {
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

    // fill base vertexes
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        double angle = 2*M_PI*double(i)/double(baseVertexesNumber);
        double k = std::tan(angle);
        double x = std::sqrt(double(baseRadius*baseRadius)/(k*k+1));
        double y = std::abs(k)*x;
        if (angle > M_PI_2 && angle <= M_PI) {
            x = -x;
        } else if (angle > M_PI && angle <= 3*M_PI_2) {
            x = -x;
            y = -y;
        } else if (angle > 3*M_PI_2) {
            y = -y;
        }
        baseVertexes.emplace_back(x, y, 0, 1);
    }

    // fill side vertexes
    for (size_t i = 0; i < baseVertexesNumber; ++i) {
        if (sideVertexesNumber == 1) { // then figure is prism
            sideVertexes.emplace_back(baseVertexes[i].x(), baseVertexes[i].y(), height, 1);
        }
        for (size_t j = 0; j < sideVertexesNumber-1; ++j) {
            double k = std::tan(double(j+1)*M_PI/double(2*(sideVertexesNumber+1)))*double(height)/double(baseRadius);
            double xy = std::sqrt(double(baseRadius*baseRadius) * double(height*height) / (double(baseRadius*baseRadius) * k*k + double(height*height)));
            double z = std::abs(k) * xy;
            double x, y;
            if (abs(baseVertexes[i].y()) < EPS) {
                y = 0;
                x = xy;
            } else if (abs(baseVertexes[i].x()) < EPS) {
                y = xy;
                x = 0;
            } else {
                y = std::sqrt(baseVertexes[i].y()*baseVertexes[i].y() * xy*xy / (baseVertexes[i].x()*baseVertexes[i].x() + baseVertexes[i].y()*baseVertexes[i].y()));
                x = y * abs(baseVertexes[i].x() / baseVertexes[i].y());
            }
            sideVertexes.emplace_back(sign(baseVertexes[i].x()) * x, sign(baseVertexes[i].y()) * y, z, 1);
        }
    }

    topVertex = QVector4D(0, 0, height, 1);
}
