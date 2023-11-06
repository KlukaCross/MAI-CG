#ifndef LAB3_MAIN_WINDOW_HPP
#define LAB3_MAIN_WINDOW_HPP
#include <QWidget>
#include <QMatrix4x4>
#include <QSpinBox>
#include <QPainterPath>
#include <QPainter>
#include "figure.hpp"

class MainWindow : public QWidget {
Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);

public:
    Figure *figure;
    QWidget *surface;
    QPoint mousePos;
    bool mousePressed = false;
    QMatrix4x4 rotateMatrix;
    size_t figureRadius, figureHeight, accuracy;
    QList<QPair<QPainterPath, double>> backEdges;
    QList<QPair<QPainterPath, double>> frontEdges;

    QSpinBox *radiusSpinBox, *heightSpinBox, *accuracySpinBox;

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void build();

private slots:
    void radiusChanged(int newRadius);
    void heightChanged(int newHeight);
    void accuracyChanged(int newAccuracy);
};


class Surface : public QWidget {
Q_OBJECT
public:
    explicit Surface(MainWindow *parent = nullptr);
    MainWindow *parent;
    const QColor lightColor {255, 255, 255};
    const QVector3D lightSource = QVector3D{1, 1, -1}.normalized();

private:
    void paintEvent(QPaintEvent *event) override;
    void addSidePath(const QVector4D& v1, const QVector4D& v2, const QVector4D& v3, const QVector4D& v4);
    void addBasePath(const QList<QVector4D>& v);
    void addTopPath(const QVector4D& v1, const QVector4D& v2, const QVector4D& v3);
};

#endif //LAB3_MAIN_WINDOW_HPP
