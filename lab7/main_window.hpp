#ifndef LAB7_MAIN_WINDOW_HPP
#define LAB7_MAIN_WINDOW_HPP
#include <QWidget>
#include <QMouseEvent>


class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent=nullptr);

private:
    QPointF* currentPoint = nullptr;
    QVector<QPointF*> points;

private:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void generatePoints(size_t count);

    QPointF B(double t);
    QPointF BIteration(size_t n, size_t k, double t);
    QPainterPath GetPath();
};


#endif //LAB7_MAIN_WINDOW_HPP
