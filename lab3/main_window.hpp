#ifndef LAB3_MAIN_WINDOW_HPP
#define LAB3_MAIN_WINDOW_HPP
#include <QWidget>
#include <QMatrix4x4>
#include <QSpinBox>
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

private:
    void paintEvent(QPaintEvent *event) override;
};

#endif //LAB3_MAIN_WINDOW_HPP
