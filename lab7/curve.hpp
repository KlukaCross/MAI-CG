#ifndef LAB7_CURVE_HPP
#define LAB7_CURVE_HPP
#include <QGraphicsItem>

class Curve : public QGraphicsItem {
public:
    Curve(size_t w, size_t h);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QRectF boundingRect() const override;
private:
    size_t h, w;
};


#endif //LAB7_CURVE_HPP
