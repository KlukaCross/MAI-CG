#include "main_widget.hpp"

#include <QMouseEvent>

#include <cmath>

MainWidget::~MainWidget() {
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete geometries;
    doneCurrent();
}

void MainWidget::mousePressEvent(QMouseEvent *e) {
    // Save mouse press position
    mousePressPosition = QVector2D(e->pos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e) {
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

void MainWidget::timerEvent(QTimerEvent *) {
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        update();
    }
}

void MainWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();

    geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

void MainWidget::initShaders() {
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

void MainWidget::resizeGL(int w, int h) {
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projectionMatrix.setToIdentity();

    // Set perspective projection
    projectionMatrix.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL() {
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    program.bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("model", projectionMatrix * matrix);
    program.setUniformValue("view", projectionMatrix * matrix);
    program.setUniformValue("projection", projectionMatrix);

    // Draw cube geometry
    geometries->drawFigureGeometry(&program);
}