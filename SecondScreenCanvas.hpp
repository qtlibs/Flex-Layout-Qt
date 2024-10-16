#ifndef SECOND_SCREEN_CANVAS
#define SECOND_SCREEN_CANVAS

#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <QSize>
#include <QPoint>

#include "WidgetGeometry.hpp"


class ResizedLayoutGridCanvas final
{

public:
    explicit ResizedLayoutGridCanvas(const QSizeF& size) noexcept;
    virtual ~ResizedLayoutGridCanvas() noexcept = default;

public:
    bool Insert(std::vector<WidgetGeometry>& contentVector, double scaleParam = 1.0) noexcept;

public:
    const std::vector<WidgetGeometry>& GetGeometries() const noexcept;

private:
    void Scale(std::vector<WidgetGeometry>& contentVector, double coef) noexcept;
    bool Insert(WidgetGeometry content) noexcept;
    bool CanInsert(const WidgetGeometry& content) const noexcept;
    bool Use(const WidgetGeometry& content) noexcept;
    void Sort() noexcept;


private:
    const QSizeF m_size;
    bool m_needToSort = false;
    std::list<QPointF> m_widgetFuturPoses;
    std::vector<WidgetGeometry> m_widgetsToInsert;
};

#endif
