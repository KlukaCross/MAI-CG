#include "main_window.hpp"
#include <QDateTime>
#include <QRandomGenerator>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>


const size_t POINT_RADIUS = 4;
const size_t CURVE_ITERATIONS = 2000;

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

QPainterPath MainWindow::GetPath() {
    QPainterPath path;

    path.moveTo(*points[0]);
    double step = 1./double(CURVE_ITERATIONS);
    for (size_t i = 0; i < CURVE_ITERATIONS; ++i) {
        path.lineTo(B(i*step));
    }

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

QPointF MainWindow::B(double t) {
    QPointF p(0, 0);
    size_t n = points.size() - 1;
    for (size_t i = 0; i <= n; ++i) {
        p += BIteration(n, i, t);
    }
    return p;
}

QPointF MainWindow::BIteration(size_t n, size_t k, double t) {
    size_t fact_n = 1;
    for (size_t i = 2; i < n; ++i)
        fact_n *= i;
    size_t fact_k = 1;
    for (size_t i = 2; i < k; ++i)
        fact_k *= i;
    size_t fact_n_k = 1;
    for (size_t i = 2; i < n-k; ++i)
        fact_n_k *= i;
    double b = fact_n/(fact_k*fact_n_k)*qPow(t, k)*qPow(1-t, n-k);
    return {b*points[k]->x(), b*points[k]->y()};
}
