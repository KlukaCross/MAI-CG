#include "main_window.hpp"
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setMinimumHeight(100);
    setMinimumWidth(100);

    prism = new Prism();
}


void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(this->rect(),Qt::white);
    QPen pen(Qt::black);
    painter.setPen(pen);

    const float scale=(width()>height()?height():width())/4;

    QMatrix4x4 perspectiveMatrix {
        1,0,0,0,
        0,1,0,0,
        0,0,0,0,
        0,0,0,1
    };
    QMatrix4x4 scaleMatrix {
        scale, 0, 0, 0,
        0, scale, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 1
    };
    QList<QVector4D> transformedVertexes;
    for(auto vertex: prism->vertexes) {
        transformedVertexes.push_back(vertex*rotateMatrix*perspectiveMatrix*scaleMatrix);
    }

    painter.save();
    painter.translate(width()/2,height()/2);

    QList<QPainterPath> backEdges;
    QList<QPainterPath> frontEdges;
    for (auto plane: prism->planes) {
        QPainterPath path;
        path.moveTo(QPointF{transformedVertexes[plane[0]].x(), transformedVertexes[plane[0]].y()});
        path.lineTo(QPointF{transformedVertexes[plane[1]].x(), transformedVertexes[plane[1]].y()});
        path.lineTo(QPointF{transformedVertexes[plane[2]].x(), transformedVertexes[plane[2]].y()});
        path.lineTo(QPointF{transformedVertexes[plane[3]].x(), transformedVertexes[plane[3]].y()});
        path.closeSubpath();

        const QVector3D v1 = (transformedVertexes[plane[1]] - transformedVertexes[plane[0]]).toVector3D();
        const QVector3D v2 = (transformedVertexes[plane[2]] - transformedVertexes[plane[1]]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() < 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }

    painter.setRenderHint(QPainter::Antialiasing,true);
    //painter.setPen(QPen(Qt::gray,1, Qt::DashLine));
    //for(auto path: backEdges) {
    //    painter.drawPath(path);
    //}
    painter.setPen(QPen(Qt::black,4));
    for(auto path: frontEdges) {
        painter.drawPath(path);
    }
    painter.restore();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    mousePressed=true;
    mousePos=event->pos();
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if(mousePressed){
        const QPoint posOffset=event->pos()-mousePos;
        mousePos=event->pos();
        rotateMatrix.rotate(2.,QVector3D(0.5*posOffset.y(),-0.5*posOffset.x(),0.));
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    mousePressed=false;
    QWidget::mouseReleaseEvent(event);
}
