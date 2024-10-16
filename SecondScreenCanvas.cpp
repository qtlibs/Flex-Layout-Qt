#include "SecondScreenCanvas.hpp"

ResizedLayoutGridCanvas::ResizedLayoutGridCanvas(const QSizeF& size) noexcept
    : m_size(size)
    , m_needToSort(false)
{
    m_widgetFuturPoses.emplace_back(0.0, 0.0);
}

void ResizedLayoutGridCanvas::Scale(std::vector<WidgetGeometry>& contentVector, double coef) noexcept
{
    Q_ASSERT(coef > 0 && coef <= 1.0);
    for (WidgetGeometry& geometry : contentVector)
    {
        if (!geometry.m_isPosFixed)
        {
            geometry.m_size = geometry.m_layoutItem->Geometry().size() * coef;
            QSizeF minSize = geometry.m_layoutItem->minimumSize();
            if (geometry.m_size.width() < minSize.width() || geometry.m_size.height() < minSize.height())
            {
                geometry.m_size = minSize;
            }
        }
    }
}


const std::vector<WidgetGeometry>& ResizedLayoutGridCanvas::GetGeometries() const noexcept
{
    return m_widgetsToInsert;
}

bool ResizedLayoutGridCanvas::Insert(std::vector<WidgetGeometry>& contentVector, double scaleParam) noexcept
{

    if (contentVector.empty())
    {
        return true;
    }
    else
    {
        std::vector<WidgetGeometry>::const_iterator it = contentVector.cbegin();
        while (it!=contentVector.cend() && it->m_isPosFixed)
        {
            if (CanInsert(*it))
            {
                Use(*it);
                std::advance(it, 1);
            }
            else
            {
                return false;
            }
        }
        Scale(contentVector, scaleParam);
        while (it != contentVector.cend())
        {
            const WidgetGeometry& content = *it;
            if (Insert(content))
            {
                ++it;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
}

bool ResizedLayoutGridCanvas::Insert(WidgetGeometry content) noexcept
{

    Sort();

    for (std::list<QPointF>::const_iterator itor = m_widgetFuturPoses.cbegin(); itor != m_widgetFuturPoses.cend(); ++itor)
    {

        content.m_futurPos = *itor;

        if (CanInsert(content))
        {

            Use(content);
            m_widgetFuturPoses.erase(itor);
            return true;
        }
    }

    // Попробовать поворот
    content.Transpose();
    for (std::list<QPointF>::const_iterator itor = m_widgetFuturPoses.cbegin(); itor != m_widgetFuturPoses.cend(); ++itor)
    {

        content.m_futurPos = *itor;

        if (CanInsert(content))
        {
            Use(content);
            m_widgetFuturPoses.erase(itor);
            return true;
        }
    }


    return false;
}

bool ResizedLayoutGridCanvas::CanInsert(const WidgetGeometry& content) const noexcept
{

    if ((content.m_futurPos.x() + content.m_size.width()) > m_size.width())
        return false;

    if ((content.m_futurPos.y() + content.m_size.height()) > m_size.height())
        return false;

    for (std::vector<WidgetGeometry>::const_iterator itor = m_widgetsToInsert.cbegin(); itor != m_widgetsToInsert.cend(); ++itor)
    {
        if (content.Intersects(*itor))
        {
            return false;
        }
    }


    return true;
}

bool ResizedLayoutGridCanvas::Use(const WidgetGeometry& content) noexcept
{
    const QSizeF& size = content.m_size;
    const QPointF& coord = content.m_futurPos;

    m_widgetFuturPoses.emplace_front(coord.x() + size.width(), coord.y());
    m_widgetFuturPoses.emplace_back(coord.x(), coord.y() + size.height());

    m_widgetsToInsert.push_back(content);

    m_needToSort = true;

    return true;
}

void ResizedLayoutGridCanvas::Sort() noexcept
{
    if (m_needToSort)
    {
        // если расстояние от начала координат до точки a меньше, чем расстояние от начала координат до точки b
        //Расстояние от начала координат до точки (x, y) вычисляется как sqrt(x^2 + y^2), но в данном случае, чтобы избежать вычисления квадратного корня (которое может быть
        //дорогостоящим оператором), используется квадрат расстояния (x^2 + y^2).
        //        m_widgetFuturPoses.sort([](const QPointF& a, const QPointF& b) { return (a.x() * a.x() + a.y() * a.y()) < (b.x() * b.x() + b.y() * b.y()); });

        m_widgetFuturPoses.sort([](const QPointF& a, const QPointF& b) { return a.y() < b.y(); });
        m_needToSort = false;
    }
}
