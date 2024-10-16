#include "WidgetGeometry.hpp"

WidgetGeometry::WidgetGeometry(const WidgetGeometry& src) noexcept
    : m_layoutItem(src.m_layoutItem)
    , m_futurPos(src.m_futurPos)
    , m_size(src.m_size)
    , m_isRotated(src.m_isRotated)
    , m_isPosFixed(src.m_isPosFixed)
{
}

WidgetGeometry::WidgetGeometry(CustomLayoutItem* widgetItem) noexcept
    : m_layoutItem(widgetItem)
    , m_size(widgetItem->Geometry().size())
    , m_isPosFixed(false)
{
}

WidgetGeometry::WidgetGeometry(CustomLayoutItem* widgetItem, const QPointF& coord, bool isPosFixed) noexcept
    : m_layoutItem(widgetItem)
    , m_futurPos(coord)
    , m_size(widgetItem->Geometry().size())
    , m_isPosFixed(isPosFixed)
{
}

void WidgetGeometry::Transpose() noexcept
{
    m_isRotated = !m_isRotated;
    m_size.transpose();
}

bool WidgetGeometry::Intersects(const WidgetGeometry& that) const noexcept
{
    if (m_futurPos.x() >= (that.m_futurPos.x() + that.m_size.width()))
    {
        return false;
    }


    if (m_futurPos.y() >= (that.m_futurPos.y() + that.m_size.height()))
        return false;

    if (that.m_futurPos.x() >= (m_futurPos.x() + m_size.width()))
        return false;

    if (that.m_futurPos.y() >= (m_futurPos.y() + m_size.height()))
        return false;

    return true;
}
