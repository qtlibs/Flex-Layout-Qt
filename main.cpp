#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QPainter>
#include <QRectF>

#include "CustomLayout.hpp"
#include "MyWidet.hpp"
#include "RectWidget.hpp"
#include "SecondScreenCanvas.hpp"

#include <algorithm>
#include <vector>
#include <random>
#include <limits>

#include <iostream>
#include <vector>
#include <algorithm>
#include <QRectF>

#include <QSizeF>
#include <vector>
#include <iostream>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MyWidet* mainWidget = new MyWidet;
    QLayout* layout = new CustomLayout(mainWidget);

    RectWidget* widget0 = new RectWidget(QSize(300, 300), Qt::red, 0);
    RectWidget* widget1 = new RectWidget(QSize(50, 50), Qt::yellow, 1);
    RectWidget* widget2 = new RectWidget(QSize(50, 50), Qt::cyan, 2);
    RectWidget* widget3 = new RectWidget(QSize(50, 50), Qt::green, 3);
    RectWidget* widget4 = new RectWidget(QSize(50, 50), Qt::gray, 4);
    RectWidget* widget5 = new RectWidget(QSize(50, 50), Qt::darkRed, 5);
    RectWidget* widget6 = new RectWidget(QSize(50, 50), Qt::magenta, 6);
    RectWidget* widget7 = new RectWidget(QSize(50, 50), Qt::darkYellow, 7);
    RectWidget* widget8 = new RectWidget(QSize(50, 50), Qt::blue, 8);

    layout->addWidget(widget0);
    layout->addWidget(widget1);
    layout->addWidget(widget2);
    layout->addWidget(widget3);
    layout->addWidget(widget4);
    layout->addWidget(widget5);
    layout->addWidget(widget6);
    layout->addWidget(widget7);
    layout->addWidget(widget8);

    //    mainWidget->setLayout(layout);
    mainWidget->setObjectName("parent");
    mainWidget->showFullScreen();

    QPushButton* neuWidgetBtn = new QPushButton("Новый виджет добавить");
    neuWidgetBtn->setWindowFlags(Qt::WindowType::Window | Qt::WindowType::WindowStaysOnTopHint);
    neuWidgetBtn->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    QObject::connect(neuWidgetBtn, &QPushButton::clicked, mainWidget, &MyWidet::OnAddNeuBtn);
    QObject::connect(mainWidget, &MyWidet::destroyed, neuWidgetBtn, &QWidget::close);
    QObject::connect(neuWidgetBtn, &QWidget::destroyed, mainWidget, &QWidget::close);
    neuWidgetBtn->show();

    return app.exec();
}
