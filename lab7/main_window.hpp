#ifndef LAB7_MAIN_WINDOW_HPP
#define LAB7_MAIN_WINDOW_HPP
#include <QWidget>
#include "graphics_view.hpp"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(size_t a, size_t b, QWidget *parent=nullptr);

private:
    GraphicsView *view;
};


#endif //LAB7_MAIN_WINDOW_HPP
