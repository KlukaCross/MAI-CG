#include "main_window.hpp"
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

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
    parent->backEdges.clear();
    parent->frontEdges.clear();
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

    size_t baseVertexesSize = parent->figure->baseVertexes.size();
    size_t sideVertexesSize = parent->figure->sideVertexes.size();
    size_t vertexesStep = sideVertexesSize/baseVertexesSize;

    QList<QVector4D> baseVertexes;
    for (size_t i = 0; i < baseVertexesSize; ++i) {
        baseVertexes.push_back(transformedVertexes[i]);
    }
    addBasePath(baseVertexes);


    if (transformedVertexes.size() >= 2*baseVertexesSize+vertexesStep) {
        for (size_t i = 0; i < baseVertexesSize-1; ++i) {
            addSidePath(
                    transformedVertexes[i],
                    transformedVertexes[i+1],
                    transformedVertexes[baseVertexesSize+(i+1)*vertexesStep],
                    transformedVertexes[baseVertexesSize+i*vertexesStep]
                    );
        }
        addSidePath(
                transformedVertexes[baseVertexesSize-1],
                transformedVertexes[0],
                transformedVertexes[baseVertexesSize],
                transformedVertexes[baseVertexesSize+(baseVertexesSize-1)*vertexesStep]
                );
    }

    for (size_t i = baseVertexesSize; i < transformedVertexes.size()-vertexesStep-1; ++i) {
        QPainterPath path;
        if ((i-baseVertexesSize+1) % vertexesStep == 0)
            continue;
        addSidePath(
                transformedVertexes[i],
                transformedVertexes[i+vertexesStep],
                transformedVertexes[i+vertexesStep+1],
                transformedVertexes[i+1]
                );
    }
    for (size_t i = 0; i < vertexesStep-1; ++i) {
        addSidePath(
                transformedVertexes[transformedVertexes.size()-vertexesStep-1+i],
                transformedVertexes[baseVertexesSize+i],
                transformedVertexes[baseVertexesSize+i+1],
                transformedVertexes[transformedVertexes.size()-vertexesStep+i]
                );
    }

    for (size_t i = 0; i < baseVertexesSize-1; ++i) {
        addTopPath(
                transformedVertexes[baseVertexesSize+vertexesStep*(i+1)-1],
                transformedVertexes[baseVertexesSize+vertexesStep*(i+2)-1],
                transformedVertexes[transformedVertexes.size()-1]
                );
    }
    addTopPath(
            transformedVertexes[transformedVertexes.size()-2],
            transformedVertexes[baseVertexesSize+vertexesStep-1],
            transformedVertexes[transformedVertexes.size()-1]
            );

    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(QPen(Qt::black,1));
    for(auto pair: parent->frontEdges) {
        //painter.fillPath(path, Qt::green);
        double k = pair.second;
        painter.fillPath(pair.first, QColor{int(lightColor.red()*k), int(lightColor.green()*k), int(lightColor.blue()*k)});
        painter.drawPath(pair.first);
    }
    painter.restore();
}

void Surface::addSidePath(const QVector4D &v1, const QVector4D &v2, const QVector4D &v3, const QVector4D &v4) {
    QPainterPath path;
    QPointF p1 {v1.x(), v1.y()};
    QPointF p2 {v2.x(), v2.y()};
    QPointF p3 {v3.x(), v3.y()};
    QPointF p4 {v4.x(), v4.y()};
    path.moveTo(p1);
    path.lineTo(p2);
    path.lineTo(p3);
    path.lineTo(p4);
    path.closeSubpath();
    const QVector3D vector1 {p2 - p1};
    const QVector3D vector2 {p3 - p2};
    const QVector3D normal = QVector3D::crossProduct(vector1, vector2).normalized();
    QVector3D center = QVector3D{(p1+p2+p3+p4)/4}.normalized();
    double k = abs(QVector3D::dotProduct((normal+center).normalized(), lightSource.normalized()));
    (normal.z() > 0) ? parent->frontEdges.emplace_back(path, k) : parent->backEdges.emplace_back(path, k);
}

void Surface::addBasePath(const QList<QVector4D>& v) {
    QPainterPath path;
    path.moveTo(QPointF{v[0].x(), v[0].y()});
    for (size_t i = 1; i < v.size(); ++i) {
        QPointF p{v[i].x(), v[i].y()};
        path.lineTo(p);
    }
    path.closeSubpath();
    QPointF p1 {v[0].x(), v[0].y()};
    QPointF p2 {v[1].x(), v[1].y()};
    QPointF p3 {v[2].x(), v[2].y()};
    const QVector3D vector1 {p2 - p1};
    const QVector3D vector2 {p3 - p2};
    const QVector3D normal = QVector3D::crossProduct(vector1, vector2).normalized();
    double k = abs(QVector3D::dotProduct((normal*parent->rotateMatrix).normalized(), lightSource));
    (normal.z() < 0)? parent->frontEdges.emplace_back(path, k) : parent->backEdges.emplace_back(path, k);
}

void Surface::addTopPath(const QVector4D &v1, const QVector4D &v2, const QVector4D &v3) {
    QPainterPath path;
    QPointF p1 {v1.x(), v1.y()};
    QPointF p2 {v2.x(), v2.y()};
    QPointF p3 {v3.x(), v3.y()};
    path.moveTo(p1);
    path.lineTo(p2);
    path.lineTo(p3);
    path.closeSubpath();
    const QVector3D vector1 {p2 - p1};
    const QVector3D vector2 {p3 - p2};
    const QVector3D normal = QVector3D::crossProduct(vector1, vector2).normalized();
    QVector3D center = QVector3D{(p1+p2+p3)/3}.normalized();
    double k = abs(QVector3D::dotProduct((center+normal).normalized(), lightSource));
    (normal.z() > 0) ? parent->frontEdges.emplace_back(path, k) : parent->backEdges.emplace_back(path, k);
}
