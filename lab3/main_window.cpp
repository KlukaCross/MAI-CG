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
    QPen pen(Qt::black, 20);
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
    transformedVertexes.push_back(parent->figure->topVertex*parent->rotateMatrix*perspectiveMatrix*scaleMatrix);

    painter.save();
    painter.translate(width()/2,height()/2);

    QList<QPainterPath> backEdges;
    QList<QPainterPath> frontEdges;
    QList<std::pair<QVector3D, QVector3D>> normals;

    size_t baseVertexesSize = parent->figure->baseVertexes.size();
    size_t sideVertexesSize = parent->figure->sideVertexes.size();
    size_t vertexesStep = sideVertexesSize/baseVertexesSize;

    {
        QPainterPath path;
        path.moveTo(QPointF{transformedVertexes[0].x(), transformedVertexes[0].y()});
        for (size_t i = 1; i < baseVertexesSize; ++i) {
            path.lineTo(QPointF{transformedVertexes[i].x(), transformedVertexes[i].y()});
        }
        path.closeSubpath();
        const QVector3D v1 = (transformedVertexes[1] - transformedVertexes[0]).toVector3D();
        const QVector3D v2 = (transformedVertexes[2] - transformedVertexes[1]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() < 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }


    if (transformedVertexes.size() >= 2*baseVertexesSize+vertexesStep) {
        for (size_t i = 0; i < baseVertexesSize-1; ++i) {
            QPainterPath path;
            path.moveTo(QPointF{transformedVertexes[i].x(), transformedVertexes[i].y()});
            path.lineTo(QPointF{transformedVertexes[i+1].x(), transformedVertexes[i+1].y()});
            path.lineTo(QPointF{transformedVertexes[baseVertexesSize+(i+1)*vertexesStep].x(), transformedVertexes[baseVertexesSize+(i+1)*vertexesStep].y()});
            path.lineTo(QPointF{transformedVertexes[baseVertexesSize+i*vertexesStep].x(), transformedVertexes[baseVertexesSize+i*vertexesStep].y()});
            path.closeSubpath();
            const QVector3D v1 = (transformedVertexes[i+1] - transformedVertexes[i]).toVector3D();
            const QVector3D v2 = (transformedVertexes[baseVertexesSize+(i+1)*vertexesStep] - transformedVertexes[i+1]).toVector3D();
            const QVector3D normal = QVector3D::crossProduct(v1, v2);
            normals.emplace_back(transformedVertexes[i].toVector3D(), normal);
            if (normal.z() > 0)
                frontEdges.push_back(path);
            else
                backEdges.push_back(path);
        }
        QPainterPath path;
        path.moveTo(QPointF{transformedVertexes[baseVertexesSize-1].x(), transformedVertexes[baseVertexesSize-1].y()});
        path.lineTo(QPointF{transformedVertexes[0].x(), transformedVertexes[0].y()});
        path.lineTo(QPointF{transformedVertexes[baseVertexesSize].x(), transformedVertexes[baseVertexesSize].y()});
        path.lineTo(QPointF{transformedVertexes[baseVertexesSize+(baseVertexesSize-1)*vertexesStep].x(), transformedVertexes[baseVertexesSize+(baseVertexesSize-1)*vertexesStep].y()});
        path.closeSubpath();
        const QVector3D v1 = (transformedVertexes[0] - transformedVertexes[baseVertexesSize-1]).toVector3D();
        const QVector3D v2 = (transformedVertexes[baseVertexesSize] - transformedVertexes[0]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() > 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }

    for (size_t i = baseVertexesSize; i < transformedVertexes.size()-vertexesStep-1; ++i) {
        QPainterPath path;
        if ((i-baseVertexesSize+1) % vertexesStep == 0)
            continue;
        path.moveTo(QPointF{transformedVertexes[i].x(), transformedVertexes[i].y()});
        path.lineTo(QPointF{transformedVertexes[i+vertexesStep].x(), transformedVertexes[i+vertexesStep].y()});
        path.lineTo(QPointF{transformedVertexes[i+vertexesStep+1].x(), transformedVertexes[i+vertexesStep+1].y()});
        path.lineTo(QPointF{transformedVertexes[i+1].x(), transformedVertexes[i+1].y()});
        path.closeSubpath();
        const QVector3D v1 = (transformedVertexes[i+vertexesStep] - transformedVertexes[i]).toVector3D();
        const QVector3D v2 = (transformedVertexes[i+vertexesStep+1] - transformedVertexes[i+vertexesStep]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() > 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }
    for (size_t i = 0; i < vertexesStep-1; ++i) {
        QPainterPath path;
        path.moveTo(QPointF{transformedVertexes[transformedVertexes.size()-vertexesStep-1+i].x(), transformedVertexes[transformedVertexes.size()-vertexesStep-1+i].y()});
        path.lineTo(QPointF{transformedVertexes[baseVertexesSize+i].x(), transformedVertexes[baseVertexesSize+i].y()});
        path.lineTo(QPointF{transformedVertexes[baseVertexesSize+i+1].x(), transformedVertexes[baseVertexesSize+i+1].y()});
        path.lineTo(QPointF{transformedVertexes[transformedVertexes.size()-vertexesStep+i].x(), transformedVertexes[transformedVertexes.size()-vertexesStep+i].y()});
        path.closeSubpath();
        const QVector3D v1 = (transformedVertexes[baseVertexesSize+i] - transformedVertexes[transformedVertexes.size()-vertexesStep-1+i]).toVector3D();
        const QVector3D v2 = (transformedVertexes[baseVertexesSize+i+1] - transformedVertexes[baseVertexesSize+i]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() > 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }

    for (size_t i = 0; i < baseVertexesSize-1; ++i) {
        QPainterPath path;
        path.moveTo(QPointF{transformedVertexes[baseVertexesSize+vertexesStep*(i+1)-1].x(), transformedVertexes[baseVertexesSize+vertexesStep*(i+1)-1].y()});
        path.lineTo(QPointF{transformedVertexes[baseVertexesSize+vertexesStep*(i+2)-1].x(), transformedVertexes[baseVertexesSize+vertexesStep*(i+2)-1].y()});
        path.lineTo(QPointF{transformedVertexes[transformedVertexes.size()-1].x(), transformedVertexes[transformedVertexes.size()-1].y()});
        path.closeSubpath();
        const QVector3D v1 = (transformedVertexes[baseVertexesSize+vertexesStep*(i+2)-1] - transformedVertexes[baseVertexesSize+vertexesStep*(i+1)-1]).toVector3D();
        const QVector3D v2 = (transformedVertexes[transformedVertexes.size()-1] - transformedVertexes[baseVertexesSize+vertexesStep*(i+2)-1]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() > 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }
    {
        QPainterPath path;
        path.clear();
        path.moveTo(QPointF{transformedVertexes[transformedVertexes.size()-2].x(), transformedVertexes[transformedVertexes.size()-2].y()});
        path.lineTo(QPointF{transformedVertexes[baseVertexesSize+vertexesStep-1].x(), transformedVertexes[baseVertexesSize+vertexesStep-1].y()});
        path.lineTo(QPointF{transformedVertexes[transformedVertexes.size()-1].x(), transformedVertexes[transformedVertexes.size()-1].y()});
        path.closeSubpath();
        const QVector3D v1 = (transformedVertexes[baseVertexesSize+vertexesStep-1] - transformedVertexes[transformedVertexes.size()-2]).toVector3D();
        const QVector3D v2 = (transformedVertexes[transformedVertexes.size()-1] - transformedVertexes[baseVertexesSize+vertexesStep-1]).toVector3D();
        const QVector3D normal = QVector3D::crossProduct(v1, v2);
        if (normal.z() > 0)
            frontEdges.push_back(path);
        else
            backEdges.push_back(path);
    }

    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(QPen(Qt::black,2));
    for(auto path: frontEdges) {
        painter.fillPath(path, Qt::green);
        painter.drawPath(path);
    }
    painter.restore();
}
