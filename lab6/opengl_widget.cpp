#include "opengl_widget.hpp"

#include <QMouseEvent>
#include <QDateTime>

#include <cmath>

OpenglWidget::~OpenglWidget() {
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete geometries;
    doneCurrent();
}

void OpenglWidget::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::MouseButton::LeftButton) {
        // Save mouse press position
        mousePressPosition = QVector2D(e->pos());
    } else if (e->button() == Qt::MouseButton::RightButton) {
        // Stop rotation
        angularSpeed = 0;
    }
}

void OpenglWidget::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::MouseButton::RightButton)
        return;
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->pos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

void OpenglWidget::timerEvent(QTimerEvent *) {
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
    }
    tParam += 0.01;
    update();
}

void OpenglWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();

    geometries = new GeometryEngine;
    geometries->setVertexesNumber(50, 50);
    geometries->initFigureGeometry(&program);

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

void OpenglWidget::initShaders() {
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, "./vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, "./fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void OpenglWidget::resizeGL(int w, int h) {
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 2.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projectionMatrix.setToIdentity();

    // Set perspective projection
    projectionMatrix.perspective(fov, aspect, zNear, zFar);
}

void OpenglWidget::paintGL() {
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    program.bind();

    // Calculate modelMatrix viewMatrix transformation
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(0.0, 0.0, -0.3);
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(0.0, 0.0, -4.0);
    viewMatrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("model", modelMatrix);
    program.setUniformValue("view", viewMatrix);
    program.setUniformValue("projection", projectionMatrix);

    // Set colors and light positions
    program.setUniformValue("objectColor", 0.5f, 0.5f, 0.5f);
    program.setUniformValue("lightColor", lightRedColor, lightGreenColor, lightBlueColor);
    program.setUniformValue("lightPos", 0.0f, 0.0f, -1.0f);
    program.setUniformValue("viewPos", 0.0f, 0.0f, -0.5f);
    program.setUniformValue("glossCoefficient", glossCoefficient);

    program.setUniformValue("t", GLfloat(tParam));

    // Draw cube geometry
    geometries->drawFigureGeometry();
}

void OpenglWidget::rebuildFigure() {
    geometries->initFigureGeometry(&program);
}

void OpenglWidget::setGlossCoefficient(int value) {
    this->glossCoefficient = std::pow(2, value);
}

void OpenglWidget::setRedColor(int value) {
    lightRedColor = value/float(255);
}

void OpenglWidget::setGreenColor(int value) {
    lightGreenColor = value/float(255);
}

void OpenglWidget::setBlueColor(int value) {
    lightBlueColor = value/float(255);
}

void OpenglWidget::setAccuracyCoefficient(int value) {
    geometries->setVertexesNumber(value, value);
}

int OpenglWidget::calculateVertexesNumber() {
    if (geometries)
        return geometries->calculateVertexesNumber();
    else
        return 50 + 4*50*49 + 3*50;
}