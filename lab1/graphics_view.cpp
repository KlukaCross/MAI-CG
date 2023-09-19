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

    plot_items = new QGraphicsItemGroup();
    curve = new Curve(this->width(), this->height());

    scene->addItem(plot_items);
    scene->addItem(curve);
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    this->draw();
    QGraphicsView::resizeEvent(event);
}

void GraphicsView::draw() {
    foreach(QGraphicsItem *item, scene->items(plot_items->boundingRect())) {
         if(item->group() == plot_items) {
             delete item;
         }
    }
    scene->removeItem(curve);
    delete curve;

    int width = this->width();
    int height = this->height();

    scene->setSceneRect(0,0,width,height);

    draw_plot();

    scene->addItem(curve = new Curve(width, height));
}

void GraphicsView::set_constants(size_t a, size_t b) {
    this->a = a;
    this->b = b;
}

void GraphicsView::draw_plot() {
    QPen penAxes(Qt::black, 2);
    QPen penStrokes(Qt::black, 1);
    QFont textFont("Times", 10, QFont::Bold);

    int width = this->width();
    int height = this->height();
    int centerOfPlotX = width / 2;
    int centerOfPlotY = height / 2;
    int arrowsSize = 10;
    int indent = 1;
    int strokeOnWidth = 40;
    int strokeOnHeight = 40;
    int strokeLength = 4;
    double maxValueX = a*1.5;
    double maxValueY = b*1.5;
    double numberStepX = 2*maxValueX/(double(width)/strokeOnWidth);
    double numberStepY = 2*maxValueY/(double(height)/strokeOnHeight);

    // axes and arrows
    // x
    plot_items->addToGroup(scene->addLine(indent, centerOfPlotY, width-indent, centerOfPlotY, penAxes));
    plot_items->addToGroup(scene->addLine(width-indent, centerOfPlotY, width - indent - arrowsSize, centerOfPlotY + arrowsSize, penAxes));
    plot_items->addToGroup(scene->addLine(width-indent, centerOfPlotY, width - indent - arrowsSize, centerOfPlotY - arrowsSize, penAxes));
    QGraphicsSimpleTextItem *text = scene->addSimpleText(QString("X"), textFont);
    text->setPos(width-arrowsSize, centerOfPlotY+arrowsSize);
    plot_items->addToGroup(text);
    // y
    plot_items->addToGroup(scene->addLine(centerOfPlotX, height-indent, centerOfPlotX, indent, penAxes));
    plot_items->addToGroup(scene->addLine(centerOfPlotX, indent, centerOfPlotX + arrowsSize, indent + arrowsSize, penAxes));
    plot_items->addToGroup(scene->addLine(centerOfPlotX, indent, centerOfPlotX - arrowsSize, indent + arrowsSize, penAxes));
    text = scene->addSimpleText(QString("Y"), textFont);
    text->setPos(centerOfPlotX+arrowsSize, 0+arrowsSize);
    plot_items->addToGroup(text);

    // strokes and numbers
    QGraphicsSimpleTextItem *text_number = scene->addSimpleText(QString::number(0), textFont);
    text_number->setPos(centerOfPlotX+strokeLength/2, centerOfPlotY+strokeLength/2);
    plot_items->addToGroup(text_number);

    double number = numberStepX;
    for (int i = 1; i < width/strokeOnWidth/2; ++i) {
        QGraphicsSimpleTextItem *text_number = scene->addSimpleText(QString::number(number, 'f', 1), textFont);
        text_number->setPos(centerOfPlotX+strokeOnWidth*i, centerOfPlotY+strokeLength/2);
        plot_items->addToGroup(text_number);
        plot_items->addToGroup(scene->addLine(centerOfPlotX+strokeOnWidth*i, centerOfPlotY-strokeLength/2, centerOfPlotX+strokeOnWidth*i, centerOfPlotY+strokeLength/2, penStrokes));
        number += numberStepX;
    }
    number = -numberStepX;
    for (int i = 1; i < width/strokeOnWidth/2; ++i) {
        QGraphicsSimpleTextItem *text_number = scene->addSimpleText(QString::number(number, 'f', 1), textFont);
        text_number->setPos(centerOfPlotX-strokeOnWidth*i, centerOfPlotY+strokeLength/2);
        plot_items->addToGroup(text_number);
        plot_items->addToGroup(scene->addLine(centerOfPlotX-strokeOnWidth*i, centerOfPlotY-strokeLength/2, centerOfPlotX-strokeOnWidth*i, centerOfPlotY+strokeLength/2, penStrokes));
        number -= numberStepX;
    }
    number = -numberStepY;
    for (int i = 1; i < height/strokeOnHeight/2; ++i) {
        QGraphicsSimpleTextItem *text_number = scene->addSimpleText(QString::number(number, 'f', 1), textFont);
        text_number->setPos(centerOfPlotX+strokeLength/2,  centerOfPlotY+strokeOnHeight*i);
        plot_items->addToGroup(text_number);
        plot_items->addToGroup(scene->addLine(centerOfPlotX-strokeLength/2, centerOfPlotY+strokeOnHeight*i, centerOfPlotX+strokeLength/2, centerOfPlotY+strokeOnHeight*i, penStrokes));
        number -= numberStepY;
    }
    number = numberStepY;
    for (int i = 1; i < height/strokeOnHeight/2; ++i) {
        QGraphicsSimpleTextItem *text_number = scene->addSimpleText(QString::number(number, 'f', 1), textFont);
        text_number->setPos(centerOfPlotX+strokeLength/2,  centerOfPlotY-strokeOnHeight*i);
        plot_items->addToGroup(text_number);
        plot_items->addToGroup(scene->addLine(centerOfPlotX-strokeLength/2, centerOfPlotY-strokeOnHeight*i, centerOfPlotX+strokeLength/2, centerOfPlotY-strokeOnHeight*i, penStrokes));
        number += numberStepY;
    }
}
