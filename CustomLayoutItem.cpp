#include "CustomLayoutItem.hpp"

CustomLayoutItem::CustomLayoutItem(QWidget* widget, int index)
    : m_index(index)
    , m_widget(widget)
{
}


QSize CustomLayoutItem::sizeHint() const
{
    return m_widget->sizeHint();
}

QSize CustomLayoutItem::minimumSize() const
{
    return QSize(130, 130);
}

Qt::Orientations CustomLayoutItem::expandingDirections() const
{
    return Qt::Orientation::Horizontal | Qt::Orientation::Vertical;
}

bool CustomLayoutItem::isEmpty() const
{
    return false;
}

void CustomLayoutItem::setGeometry(const QRect& rect)
{
    Q_UNUSED(rect);
    Q_UNREACHABLE();
}

void CustomLayoutItem::SetGeometry(const QRectF& rect) noexcept
{
    m_geometry = rect;
    m_widget->setGeometry(rect.toRect());
}

const QRectF& CustomLayoutItem::Geometry() const noexcept
{
    return m_geometry;
}

void CustomLayoutItem::CopyGeometryFrom(CustomLayoutItem* item) noexcept
{
    m_geometry = item->m_geometry;
    m_widget->setGeometry(m_geometry.toRect());
}

QRect CustomLayoutItem::geometry() const
{
    Q_UNREACHABLE();
}

QWidget* CustomLayoutItem::widget()
{
    return m_widget;
}

QSize CustomLayoutItem::maximumSize() const
{
    return QSize(1920, 1080);
}
