#include "main_window.hpp"
#include <QDateTime>
#include <QRandomGenerator>
#include <QPainter>
#include <QPainterPath>

const size_t POINT_RADIUS = 4;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    resize(300, 300);
    generatePoints(6);
}

void MainWindow::generatePoints(size_t count) {
    QRandomGenerator random(QDateTime::currentMSecsSinceEpoch());
    points.push_back(new QPointF(5, 5));
    for (int i = 0; i < count-2; ++i) {
        points.push_back(new QPointF(random.bounded(5, this->width()), random.bounded(5, this->height())));
    }
    points.push_back(new QPointF(this->width(), this->height()));
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);


    painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
    QPainterPath pointPath = GetPath();
    pointPath.moveTo(*points[0]);
    for (size_t i = 1; i < points.size(); ++i) {
        pointPath.lineTo(*points[i]);
    }
    painter.drawPath(pointPath);

    painter.setPen(QPen(Qt::red, 5));
    for (auto p: points) {
        painter.drawEllipse(*p, POINT_RADIUS, POINT_RADIUS);
    }

    painter.setPen(QPen(Qt::white, 2));
    QPainterPath BezierPath = GetPath();
    painter.drawPath(BezierPath);
}

QPointF MainWindow::GetLineStart(QPointF* pt1, QPointF* pt2) const {
    QPointF pt;
    float fRat = 0.5f;
    pt.setX((1.0f - fRat)*pt1->x() + fRat*pt2->x());
    pt.setY((1.0f - fRat)*pt1->y() + fRat*pt2->y());
    return pt;
}

QPointF MainWindow::GetLineEnd(QPointF* pt1, QPointF* pt2) const {
    QPointF pt;
    float fRat = 0.5f;
    pt.setX(fRat*pt1->x() + (1.0f - fRat)*pt2->x());
    pt.setY(fRat*pt1->y() + (1.0f - fRat)*pt2->y());
    return pt;
}

QPainterPath MainWindow::GetPath() {
    QPainterPath path;

    path.moveTo(*points[0]);
    QPointF pt1;
    QPointF pt2;
    for (size_t i = 0; i < points.size() - 1; i++) {
        pt1 = GetLineStart(points[i], points[i+1]);

        path.quadTo(*points[i], pt1);

        pt2 = GetLineEnd(points[i], points[i+1]);
        path.lineTo(pt2);
    }

    path.lineTo(*points[points.size()-1]);
    return path;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    for (auto p: points) {
        QRectF rect(QPoint(p->x()-POINT_RADIUS, p->y()-POINT_RADIUS), QPoint(p->x()+POINT_RADIUS, p->y()+POINT_RADIUS));
        if (rect.contains(pos)) {
            currentPoint = p;
            break;
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (currentPoint)
        currentPoint = nullptr;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (!currentPoint)
        return;
    QPoint pos = event->pos();
    currentPoint->setX(pos.x());
    currentPoint->setY(pos.y());
    update();
}
