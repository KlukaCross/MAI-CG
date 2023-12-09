#include "main_window.hpp"

MainWindow::MainWindow(size_t a, size_t b, QWidget *parent) : QWidget(parent) {
    view = new GraphicsView(this);
}
