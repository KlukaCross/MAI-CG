#ifndef LAB1_MAIN_WINDOW_HPP
#define LAB1_MAIN_WINDOW_HPP
#include <QWidget>
#include <QMatrix4x4>

#include "prism.hpp"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);

private:
    Prism *prism;
    QPoint mousePos;
    bool mousePressed = false;
    QMatrix4x4 rotateMatrix;

private:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif //LAB1_MAIN_WINDOW_HPP
