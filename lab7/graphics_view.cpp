#include "graphics_view.hpp"


GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setAlignment(Qt::AlignCenter);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setMinimumHeight(100);
    this->setMinimumWidth(100);

    scene = new QGraphicsScene();
    this->setScene(scene);

    curve = new Curve(this->width(), this->height());

    scene->addItem(curve);
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    this->draw();
    QGraphicsView::resizeEvent(event);
}

void GraphicsView::draw() {
    scene->removeItem(curve);
    delete curve;

    int width = this->width();
    int height = this->height();

    scene->setSceneRect(0,0,width,height);

    scene->addItem(curve = new Curve(width, height));
}
