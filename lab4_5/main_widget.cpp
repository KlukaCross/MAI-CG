#include "main_widget.hpp"

#include <QFormLayout>

#include <cmath>


MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
    opengl_widget = new OpenglWidget();

    QHBoxLayout *hbox = new QHBoxLayout(this);
    QGridLayout *gbox = new QGridLayout();
    gbox->setAlignment(Qt::AlignTop);

    QLabel *accuracyNameLabel = new QLabel("Accuracy");
    accuracyValueLabel = new QLabel();
    int vertexesNumber = opengl_widget->calculateVertexesNumber();
    accuracyValueLabel->setText(QString::number(vertexesNumber) + " vertexes");
    accuracyValueLabel->setMinimumSize(100, 10);
    QSlider *accuracySlider = new QSlider(Qt::Horizontal);
    accuracySlider->setRange(4, 350);
    accuracySlider->setSliderPosition(50);

    QLabel *colorNameLabel = new QLabel("Light color");
    redColorValueLabel = new QLabel("red: 255");
    greenColorValueLabel = new QLabel("green: 255");
    blueColorValueLabel = new QLabel("blue: 255");
    QSlider *redColorSlider = new QSlider(Qt::Horizontal);
    QSlider *greenColorSlider = new QSlider(Qt::Horizontal);
    QSlider *blueColorSlider = new QSlider(Qt::Horizontal);
    redColorSlider->setRange(0, 255);
    redColorSlider->setSliderPosition(255);
    greenColorSlider->setRange(0, 255);
    greenColorSlider->setSliderPosition(255);
    blueColorSlider->setRange(0, 255);
    blueColorSlider->setSliderPosition(255);

    QLabel *glossCoefficientLabel = new QLabel("Gloss coefficient");
    glossCoefficientValue = new QLabel("5");
    QSlider *glossCoefficientSlider = new QSlider(Qt::Horizontal);
    glossCoefficientSlider->setRange(0, 10);
    glossCoefficientSlider->setSliderPosition(5);

    connect(accuracySlider, SIGNAL(sliderReleased()), this, SLOT(accuracyChanged()));
    connect(accuracySlider, SIGNAL(sliderMoved(int)), this, SLOT(accuracyLabelChanged(int)));
    connect(redColorSlider, SIGNAL(sliderMoved(int)), this, SLOT(redColorChanged(int)));
    connect(greenColorSlider, SIGNAL(sliderMoved(int)), this, SLOT(greenColorChanged(int)));
    connect(blueColorSlider, SIGNAL(sliderMoved(int)), this, SLOT(blueColorChanged(int)));
    connect(glossCoefficientSlider, SIGNAL(sliderMoved(int)), this, SLOT(glossCoefficientChanged(int)));

    gbox->addWidget(accuracyNameLabel, 1, 1);
    gbox->addWidget(accuracySlider, 2, 1);
    gbox->addWidget(accuracyValueLabel, 2, 2);
    gbox->addWidget(colorNameLabel, 3, 1);
    gbox->addWidget(redColorSlider, 5, 1);
    gbox->addWidget(greenColorSlider, 6, 1);
    gbox->addWidget(blueColorSlider, 7, 1);
    gbox->addWidget(redColorValueLabel, 5, 2);
    gbox->addWidget(greenColorValueLabel, 6, 2);
    gbox->addWidget(blueColorValueLabel, 7, 2);
    gbox->addWidget(glossCoefficientLabel, 8, 1);
    gbox->addWidget(glossCoefficientSlider, 9, 1);
    gbox->addWidget(glossCoefficientValue, 9, 2);
    hbox->addLayout(gbox, 0);
    hbox->addWidget(opengl_widget, 3);
}

void MainWidget::accuracyChanged() {
    opengl_widget->rebuildFigure();
}

void MainWidget::redColorChanged(int value) {
    opengl_widget->setRedColor(value);
    redColorValueLabel->setText("red: " + QString::number(value));
}

void MainWidget::greenColorChanged(int value) {
    opengl_widget->setGreenColor(value);
    greenColorValueLabel->setText("green: " + QString::number(value));
}

void MainWidget::blueColorChanged(int value) {
    opengl_widget->setBlueColor(value);
    blueColorValueLabel->setText("blue: " + QString::number(value));
}

void MainWidget::accuracyLabelChanged(int value) {
    opengl_widget->setAccuracyCoefficient(value);
    size_t vertexesNumber = opengl_widget->calculateVertexesNumber();
    accuracyValueLabel->setText(QString::number(vertexesNumber) + " vertexes");
}

void MainWidget::glossCoefficientChanged(int value) {
    opengl_widget->setGlossCoefficient(value);
    glossCoefficientValue->setText(QString::number(std::pow(2, value)));
}
