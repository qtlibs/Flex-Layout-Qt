#include "RectWidget.hpp"

RectWidget::RectWidget(const QSize& size, const QColor& color, int index, QWidget* parent)
    : QWidget { parent }
    , m_size(size)
    , m_color(color)
    , m_index(QString::number(index))
    , indexInt(index)
{
    setObjectName("widget" + m_index);
}

void RectWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    //    painter.setPen(QPen(Qt::black, 2));
    painter.fillRect(rect(), QBrush(m_color, static_cast<Qt::BrushStyle>(indexInt + 2)));
    painter.setPen(QPen(Qt::black));
    const QFont& font = painter.font();
    QFont& fontPtr = const_cast<QFont&>(font);
    fontPtr.setPointSize(25);
    painter.drawText(0, 0, size().width(), size().height(), Qt::AlignmentFlag::AlignCenter, m_index);

    QRect borderRect = rect().adjusted(9, 9, -9, -9);
    QPen pen2(Qt::black, 2); // Цвет линии
    painter.setPen(pen2);
    painter.drawRect(borderRect);
}
