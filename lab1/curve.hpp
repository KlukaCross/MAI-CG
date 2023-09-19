#ifndef LAB1_CURVE_HPP
#define LAB1_CURVE_HPP
#include <QGraphicsItem>

class Curve : public QGraphicsItem {
public:
    Curve(size_t w, size_t h);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QRectF boundingRect() const override;
private:
    size_t h, w;
private:
    QPointF f(double t) const;
};


#endif //LAB1_CURVE_HPP
