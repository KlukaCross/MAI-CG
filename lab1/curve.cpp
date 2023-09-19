#include "curve.hpp"
#include <QPainter>

const size_t ITERATIONS = 1000;
const double SMOOTH_COEFFICIENT = 0.01;
const double SCALE_COEFFICIENT = 1.5;
const Qt::GlobalColor CURVE_COLOR = Qt::black;
const int CURVE_WIDTH = 2;

Curve::Curve(size_t w, size_t h) : h(h), w(w) {}

void Curve::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPainterPath path(f(0));
    for (int i = 0; i < ITERATIONS; ++i) {
        path.lineTo(f(SMOOTH_COEFFICIENT*i));
    }
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(CURVE_COLOR, CURVE_WIDTH));
    painter->drawPath(path);
}

QRectF Curve::boundingRect() const {
    return QRectF(QPoint(-w/2, -h/2), QPoint(w/2, h/2));
}

QPointF Curve::f(double t) const {
    return QPointF(QPointF(w/2 + w / 2 / SCALE_COEFFICIENT * sin(t), h/2 + h / 2 / SCALE_COEFFICIENT * cos(t)));
}
