#include "MyWidet.hpp"
#include "RectWidget.hpp"

#include <QPainter>
#include <QApplication>
#include <QRandomGenerator>

MyWidet::MyWidet()
    : QWidget()
{
    setAttribute(Qt::WA_DeleteOnClose, true);
}

void MyWidet::OnAddNeuBtn() noexcept
{
    QLayout* myLayout = layout();
    int count = myLayout->count();
    if (count < s_maxWidgets)
    {
        QRandomGenerator* generator = QRandomGenerator::global();
        int red = generator->bounded(256);
        int green = generator->bounded(256);
        int blue = generator->bounded(256);

        // Create a QColor object with the random values
        QColor color(red, green, blue);
        int index = count;
        QString futurObjName = "widget" + QString::number(index);
        while (true)
        {
            bool hasIndex = false;
            for (int i = 0; i < count; ++i)
            {
                QLayoutItem* item = myLayout->itemAt(i);
                if (item->widget()->objectName() == futurObjName)
                {
                    hasIndex = true;
                }
            }
            if (hasIndex)
            {
                ++index;
                futurObjName = "widget" + QString::number(index);
            }
            else
            {
                break;
            }
        }
        myLayout->addWidget(new RectWidget(QSize(50, 50), color, index));
    }
}

void MyWidet::keyPressEvent(QKeyEvent* event)
{
    //    if (event->key() == Qt::Key::Key_Z)
    //    {
    //        if (event->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier) && event->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier))
    //        {
    //            QLayout* myLayout = layout();
    //            qApp->postEvent(myLayout, new CustomLayoutStepEvent(QAbstractAnimation::Direction::Backward));
    //        }
    //        else
    //        {
    //            if (event->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier))
    //            {
    //                QLayout* myLayout = layout();
    //                qApp->postEvent(myLayout, new CustomLayoutStepEvent(QAbstractAnimation::Direction::Forward));
    //            }
    //        }
    //    }
}
