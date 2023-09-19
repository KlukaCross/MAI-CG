#ifndef LAB1_MAIN_WINDOW_HPP
#define LAB1_MAIN_WINDOW_HPP
#include <QWidget>
#include <QSpinBox>
#include "graphics_view.hpp"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(size_t a, size_t b, QWidget *parent=nullptr);

private:
    GraphicsView *view;
    QSpinBox *aSpinBox;
    QSpinBox *bSpinBox;

private slots:
    void build();
};


#endif //LAB1_MAIN_WINDOW_HPP
