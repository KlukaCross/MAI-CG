#ifndef LAB7_GRAPHICS_VIEW_HPP
#define LAB7_GRAPHICS_VIEW_HPP
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "curve.hpp"

class GraphicsView : public QGraphicsView {
Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);
    void draw();

private:
    QGraphicsScene *scene;
    Curve *curve;
    // currentDot - точка для перемешения
    // dots -- точки (6 штук)

private:
    void resizeEvent(QResizeEvent *event) override;
    void pushEvent();  // выбор точки для перемещения
    void moveEvent();  // перемещение текущей точки и rebuild
    void releaseEvent();  // currentDot = None
};


#endif //LAB7_GRAPHICS_VIEW_HPP
