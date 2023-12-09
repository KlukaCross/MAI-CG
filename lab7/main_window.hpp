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

    float GetDistance(QPointF* pt1, QPointF* pt2) const;
    QPointF GetLineStart(QPointF* pt1, QPointF* pt2) const;
    QPointF GetLineEnd(QPointF* pt1, QPointF* pt2) const;
    QPainterPath GetPath();
};


#endif //LAB7_MAIN_WINDOW_HPP
