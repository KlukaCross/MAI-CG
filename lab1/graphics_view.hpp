#ifndef LAB1_GRAPHICS_VIEW_HPP
#define LAB1_GRAPHICS_VIEW_HPP
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "curve.hpp"

class GraphicsView : public QGraphicsView {
Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);
    void set_constants(size_t a, size_t b);
    void draw();

private:
    size_t a, b;
    QGraphicsScene *scene;
    Curve *curve;
    QGraphicsItemGroup *plot_items;

private:
    void resizeEvent(QResizeEvent *event) override;
    void draw_plot();
};


#endif //LAB1_GRAPHICS_VIEW_HPP
