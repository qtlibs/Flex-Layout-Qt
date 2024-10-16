#ifndef WIDGET_GEOMETRY_HPP
#define WIDGET_GEOMETRY_HPP

#include <QPoint>
#include <QSize>
#include <QString>

#include "CustomLayoutItem.hpp"

class WidgetGeometry final
{

public:
    explicit WidgetGeometry(CustomLayoutItem* widgetItem) noexcept;
    explicit WidgetGeometry(CustomLayoutItem* widgetItem, const QPointF& coord, bool isPosFixed) noexcept;
    WidgetGeometry(const WidgetGeometry& src) noexcept;
    virtual ~WidgetGeometry() noexcept = default;

public:
    void Transpose() noexcept;
    bool Intersects(const WidgetGeometry& that) const noexcept;

public:
    CustomLayoutItem* m_layoutItem;
    QPointF m_futurPos;
    QSizeF m_size;
    bool m_isRotated = false; //Для дебага
    bool m_isPosFixed = false;
};

#endif // WIDGET_GEOMETRY_HPP
