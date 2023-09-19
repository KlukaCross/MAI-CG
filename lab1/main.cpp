#include <QApplication>
#include "main_window.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w(10, 10);
    w.resize(300, 200);
    w.show();
    return QApplication::exec();
}
