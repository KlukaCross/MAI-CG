#include "main_window.hpp"
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    surface = new Surface(this);

    setMinimumHeight(100);
    setMinimumWidth(100);

    figureRadius = 1;
    figureHeight = 1;
    accuracy = 1;

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignRight);

    QLabel *radiusLabel = new QLabel("radius: ");
    radiusSpinBox = new QSpinBox();
    radiusSpinBox->setMinimum(1);
    radiusSpinBox->setMaximum(1000);
    radiusSpinBox->setValue(figureRadius);

    QLabel *heightLabel = new QLabel("height: ");
    heightSpinBox = new QSpinBox();
    heightSpinBox->setMinimum(1);
    heightSpinBox->setMaximum(1000);
    heightSpinBox->setValue(figureHeight);

    QLabel *accuracyLabel = new QLabel("accuracy: ");
    accuracySpinBox = new QSpinBox();
    accuracySpinBox->setMinimum(1);
    accuracySpinBox->setMaximum(100);
    accuracySpinBox->setValue(accuracy);

    connect(radiusSpinBox, SIGNAL(valueChanged(int)), this, SLOT(radiusChanged(int)));
    connect(heightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));
    connect(accuracySpinBox, SIGNAL(valueChanged(int)), this, SLOT(accuracyChanged(int)));

    vbox->addWidget(surface);
    hbox->addWidget(radiusLabel, 0, Qt::AlignLeft);
    hbox->addWidget(radiusSpinBox, 0, Qt::AlignLeft);
    hbox->addWidget(heightLabel, 0, Qt::AlignLeft);
    hbox->addWidget(heightSpinBox, 0, Qt::AlignLeft);
    hbox->addWidget(accuracyLabel, 0, Qt::AlignLeft);
    hbox->addWidget(accuracySpinBox, 0, Qt::AlignLeft);
    vbox->addLayout(hbox);

    setLayout(vbox);

    build();
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

void MainWindow::build() {
    figure = new Figure(figureRadius, figureHeight, accuracy+3, accuracy);
}

void MainWindow::radiusChanged(int newRadius) {
    figureRadius = newRadius;
    build();
}

void MainWindow::heightChanged(int newHeight) {
    figureHeight = newHeight;
    build();
}

void MainWindow::accuracyChanged(int newAccuracy) {
    accuracy = newAccuracy;
    build();
}

Surface::Surface(MainWindow *parent) : QWidget(parent) {
    this->parent = parent;
}

void Surface::paintEvent(QPaintEvent *event) {
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
    for(auto vertex: parent->figure->baseVertexes) {
        transformedVertexes.push_back(vertex*parent->rotateMatrix*perspectiveMatrix*scaleMatrix);
    }
    for(auto vertex: parent->figure->sideVertexes) {
        transformedVertexes.push_back(vertex*parent->rotateMatrix*perspectiveMatrix*scaleMatrix);
    }
    transformedVertexes.push_back(parent->figure->topVertex*perspectiveMatrix*scaleMatrix);

    painter.save();
    painter.translate(width()/2,height()/2);

    QList<QPainterPath> backEdges;
    QList<QPainterPath> frontEdges;

    QPainterPath path;
    size_t baseVertexesSize = parent->figure->baseVertexes.size();
    size_t sideVertexesSize = parent->figure->sideVertexes.size();
    path.moveTo(QPointF{transformedVertexes[0].x(), transformedVertexes[0].y()});
    for (size_t i = 1; i < baseVertexesSize; ++i) {
        path.lineTo(QPointF{transformedVertexes[i].x(), transformedVertexes[i].y()});
    }
    painter.fillPath(path, Qt::blue);
    path.closeSubpath();
    QVector3D v1 = (transformedVertexes[1] - transformedVertexes[0]).toVector3D();
    QVector3D v2 = (transformedVertexes[2] - transformedVertexes[1]).toVector3D();
    QVector3D normal = QVector3D::crossProduct(v1, v2);
    if (normal.z() < 0)
        frontEdges.push_back(path);
    else
        backEdges.push_back(path);

    path.clear();
    for (size_t i = 0; i < baseVertexesSize-1; ++i) {
        path.moveTo(QPointF{transformedVertexes[i].x(), transformedVertexes[i].y()});
        path.lineTo(QPointF{transformedVertexes[i+1].x(), transformedVertexes[i+1].y()});
        path.lineTo(QPointF{transformedVertexes[i+baseVertexesSize+1].x(), transformedVertexes[i+baseVertexesSize+1].y()});
        path.lineTo(QPointF{transformedVertexes[i+baseVertexesSize].x(), transformedVertexes[i+baseVertexesSize].y()});
        path.closeSubpath();
        painter.fillPath(path, Qt::green);
        v1 = (transformedVertexes[i+1] - transformedVertexes[i]).toVector3D();
        v2 = (transformedVertexes[i+baseVertexesSize+1] - transformedVertexes[i+1]).toVector3D();
        normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() < 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }
    path.moveTo(QPointF{transformedVertexes[baseVertexesSize-1].x(), transformedVertexes[baseVertexesSize-1].y()});
    path.lineTo(QPointF{transformedVertexes[0].x(), transformedVertexes[0].y()});
    path.lineTo(QPointF{transformedVertexes[baseVertexesSize].x(), transformedVertexes[baseVertexesSize].y()});
    path.lineTo(QPointF{transformedVertexes[2*baseVertexesSize-1].x(), transformedVertexes[2*baseVertexesSize-1].y()});
    path.closeSubpath();
    painter.fillPath(path, Qt::green);
    v1 = (transformedVertexes[0] - transformedVertexes[baseVertexesSize-1]).toVector3D();
    v2 = (transformedVertexes[baseVertexesSize] - transformedVertexes[0]).toVector3D();
    normal = QVector3D::crossProduct(v1, v2);
    if (normal.z() < 0)
        frontEdges.push_back(path);
    else
        backEdges.push_back(path);


    size_t vertexesStep = sideVertexesSize/baseVertexesSize;
    for (size_t i = baseVertexesSize; i < transformedVertexes.size()-baseVertexesSize-1; ++i) {
        path.moveTo(QPointF{transformedVertexes[i].x(), transformedVertexes[i].y()});
        path.lineTo(QPointF{transformedVertexes[i+vertexesStep].x(), transformedVertexes[i+vertexesStep].y()});
        path.lineTo(QPointF{transformedVertexes[i+vertexesStep+1].x(), transformedVertexes[i+vertexesStep+1].y()});
        path.lineTo(QPointF{transformedVertexes[i+1].x(), transformedVertexes[i+1].y()});
        path.closeSubpath();
        painter.fillPath(path, Qt::yellow);
        const QVector3D v1 = (transformedVertexes[i+vertexesStep] - transformedVertexes[i]).toVector3D();
        const QVector3D v2 = (transformedVertexes[i+vertexesStep+1] - transformedVertexes[i+vertexesStep]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() < 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }


    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(QPen(Qt::gray,1, Qt::DashLine));
    for(auto path: backEdges) {
        painter.drawPath(path);
    }
    painter.setPen(QPen(Qt::black,4));
    for(auto path: frontEdges) {
        painter.drawPath(path);
    }
    painter.restore();
}
