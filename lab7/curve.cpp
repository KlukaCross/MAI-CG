#include "curve.hpp"
#include <QPainter>


Curve::Curve(size_t w, size_t h) : h(h), w(w) {}

void Curve::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {

}

QRectF Curve::boundingRect() const {
    return QRectF(QPoint(-w/2, -h/2), QPoint(w/2, h/2));
}
