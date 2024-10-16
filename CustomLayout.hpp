#ifndef CUSTOMLAYOUT_HPP
#define CUSTOMLAYOUT_HPP

#include <cmath>
#include <optional>
#include <array>


#include <QLayout>
#include <QList>
#include <QRect>
#include <QMessageBox>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QApplication>
#include <QTime>

#include "CustomLayoutItem.hpp"
#include "SecondScreenCanvas.hpp"

class CustomLayout final : public QLayout
{
    Q_OBJECT

public:
    explicit CustomLayout(QWidget* parent = nullptr) noexcept;
    virtual ~CustomLayout() noexcept;

public:
    void addItem(QLayoutItem* item) override;
    QLayoutItem* takeAt(int index) override;

public:
    QSize sizeHint() const override;
    QSize minimumSize() const override;

public:
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    int indexOf(QWidget* widget) const override;
    bool isEmpty() const override;

public:
    void setGeometry(const QRect& rect) override;

protected:
    void customEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;


private:
    void MaximizeWidget(CustomLayoutItem* item) noexcept;
    static std::tuple<double, std::list<QRectF>::const_iterator> FindClosestPoint(const QPointF& point, const std::list<QRectF>& points) noexcept;


private:
    CustomLayoutItem* GetLayoutItem(QWidget* widget) const noexcept;
    void PackingWidgetsTopLeft(CustomLayoutItem* expected) noexcept;
    void MaximizeDistance(CustomLayoutItem* expected) noexcept;
    void FillFreeSpaceIteration(CustomLayoutItem* expected) noexcept;
    void FillFreeSpaceFull(CustomLayoutItem* expected) noexcept;

private:
    static Qt::Edges FindClickedWidgetEdge(QWidget* widget, const QPoint& clickLocalPos) noexcept;
    std::list<CustomLayoutItem*> FindIntersectedItems(CustomLayoutItem* item) const noexcept;
    std::list<CustomLayoutItem*> SortBySize() const noexcept;

private:
    std::optional<double> GetMaximizedBorderPos(CustomLayoutItem* itemToExpand, Qt::Edge edge) noexcept;

    void SetDefault() noexcept;

private:
    double CalculateDistance(const CustomLayoutItem* r1, const QRectF& r2) noexcept;
    bool CanPlaceRect(const QRectF& newRect, const std::list<CustomLayoutItem*>& placedRectangles);
    double CalcMaxDistToRects(const QRectF& curRect, const std::list<CustomLayoutItem*>& placedRectangles);

private:
    static constexpr const int s_edgePointsCount = 100;
    static constexpr const int s_adjust = 10;
    static constexpr const double s_thresholdByY = 20;
    static constexpr const double s_smallWidgetPercentSize = 0.25;
    static constexpr const double s_binWidgetPersentSize = 1.0 - s_smallWidgetPercentSize;

private:
    QSize m_prevSize;
    bool m_hasMouseMove = false;
    bool m_isTranslating = false;
    QRectF m_movedWidgetInitalGeometry;

    bool m_isResizing = false;
    Qt::Edges m_resizeWidgetEdge;
    QPoint m_lastMouseClickPosParentWidget;
    QPoint m_prevMouseMoveLayoutPos;

private:
    std::vector<QPointF> m_centerRectPoints;
    std::list<CustomLayoutItem*> m_items;
    std::list<std::list<std::pair<QRectF, QString>>> m_history;
};


#endif // CUSTOMLAYOUT_HPP
