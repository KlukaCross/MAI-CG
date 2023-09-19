#include "main_window.hpp"
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(size_t a, size_t b, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignRight);

    QLabel *aLabel = new QLabel("a: ");
    aSpinBox = new QSpinBox();
    aSpinBox->setMinimum(1);
    aSpinBox->setMaximum(10000);
    aSpinBox->setValue(a);

    QLabel *bLabel = new QLabel("b: ");
    bSpinBox = new QSpinBox();
    bSpinBox->setMinimum(1);
    bSpinBox->setMaximum(10000);
    bSpinBox->setValue(a);

    QPushButton *build_button = new QPushButton("Build");
    connect(build_button, SIGNAL(clicked()), this, SLOT(build()));

    view = new GraphicsView();
    view->set_constants(a, b);

    vbox->addWidget(view);
    hbox->addWidget(aLabel, 0, Qt::AlignLeft);
    hbox->addWidget(aSpinBox, 0, Qt::AlignLeft);
    hbox->addWidget(bLabel, 0, Qt::AlignLeft);
    hbox->addWidget(bSpinBox, 0, Qt::AlignLeft);
    hbox->addWidget(build_button, 0, Qt::AlignLeft);
    vbox->addLayout(hbox);

    setLayout(vbox);
}

void MainWindow::build() {
    view->set_constants(aSpinBox->value(), bSpinBox->value());
    view->draw();
}
