#ifndef LAB4_MAINWIDGET_HPP
#define LAB4_MAINWIDGET_HPP

#include <QLabel>
#include <QSlider>
#include "opengl_widget.hpp"


class MainWidget : public QWidget {
Q_OBJECT
public:
    explicit MainWidget(QWidget *parent=nullptr);

private:
    OpenglWidget *opengl_widget;

    QLabel *accuracyValueLabel, *redColorValueLabel, *greenColorValueLabel, *blueColorValueLabel, *glossCoefficientValue;

private slots:
    void accuracyChanged();
    void accuracyLabelChanged(int value);
    void redColorChanged(int value);
    void greenColorChanged(int value);
    void blueColorChanged(int value);
    void glossCoefficientChanged(int value);
};

#endif //LAB4_MAINWIDGET_HPP
