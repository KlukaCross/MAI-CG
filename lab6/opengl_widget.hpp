#ifndef LAB4_OPENGL_WIDGET_HPP
#define LAB4_OPENGL_WIDGET_HPP

#include "geometry_engine.hpp"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class OpenglWidget : public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    ~OpenglWidget();
    void rebuildFigure();
    void setGlossCoefficient(int value);
    void setAccuracyCoefficient(int value);
    void setRedColor(int value);
    void setGreenColor(int value);
    void setBlueColor(int value);
    int calculateVertexesNumber();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent *e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();

public:
    int glossCoefficient = 32;
    float lightRedColor = 1;
    float lightGreenColor = 1;
    float lightBlueColor = 1;

private:
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries = nullptr;

    QMatrix4x4 projectionMatrix;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed = 0;
    QQuaternion rotation;

    float tParam = 0;

};

#endif //LAB4_OPENGL_WIDGET_HPP
