#include "CustomLayout.hpp"
#include "CustomLayoutItem.hpp"
#include "RectWidget.hpp"

CustomLayout::CustomLayout(QWidget* parent) noexcept
    : QLayout(parent)
{
    Q_CHECK_PTR(parent);
    Q_STATIC_ASSERT(s_binWidgetPersentSize > 0.0);
    Q_STATIC_ASSERT(s_smallWidgetPercentSize > 0.0);
    Q_STATIC_ASSERT(s_adjust > 0);


    QSizeF parentSize(1920, 1080);
    double xStep = parentSize.width() / static_cast<double>(s_edgePointsCount);
    double yStep = parentSize.height() / static_cast<double>(s_edgePointsCount);
    m_centerRectPoints.reserve((s_edgePointsCount - 1) * (s_edgePointsCount - 1));
    for (int i = 1; i < s_edgePointsCount; ++i)
    {
        for (int j = 1; j < s_edgePointsCount; ++j)
        {
            m_centerRectPoints.emplace_back(i * xStep, j * yStep);
        }
    }
}

CustomLayout::~CustomLayout() noexcept
{
    std::for_each(m_items.begin(), m_items.end(), std::default_delete<CustomLayoutItem>());
}

void CustomLayout::addItem(QLayoutItem* item)
{
    QWidget* widget = item->widget();
    delete item;
    item = nullptr;
    RectWidget* rectItem = qobject_cast<RectWidget*>(widget);
    rectItem->installEventFilter(this);
    CustomLayoutItem* layoutItem = new CustomLayoutItem(rectItem, rectItem->m_index.toInt());
    if (parentWidget()->isVisible())
    {
        QSizeF minSize = layoutItem->minimumSize();
        QPointF pointToInsert = parentWidget()->rect().topRight() - QPointF(0.0, 10.0);
        layoutItem->SetGeometry(QRectF(pointToInsert, minSize));
        m_items.push_back(layoutItem);
        std::list<CustomLayoutItem*> sortBySize = SortBySize();
        PackingWidgetsTopLeft(nullptr);
        MaximizeDistance(nullptr);
        FillFreeSpaceIteration(nullptr);
        FillFreeSpaceFull(nullptr);
    }
    else
    {
        m_items.push_back(layoutItem);
    }
}

QSize CustomLayout::sizeHint() const
{
    QSize size;
    for (const CustomLayoutItem* item : m_items)
        size = size.expandedTo(item->sizeHint());
    return size;
}

QSize CustomLayout::minimumSize() const
{
    // минимальный размер, необходимый для размещения всех его дочерних виджетов.
    QSize size;
    for (const CustomLayoutItem* item : m_items)
        size = size.expandedTo(item->minimumSize());
    return size;
}

int CustomLayout::count() const
{
    return static_cast<int>(m_items.size());
}

QLayoutItem* CustomLayout::itemAt(int index) const
{
    if (static_cast<size_t>(index) < m_items.size())
    {
        std::list<CustomLayoutItem*>::const_iterator it = std::next(m_items.cbegin(), index);
        return *it;
    }
    return nullptr;
}

QLayoutItem* CustomLayout::takeAt(int index)
{
    if (static_cast<size_t>(index) < m_items.size())
    {
        std::list<CustomLayoutItem*>::const_iterator it = std::next(m_items.cbegin(), index);
        CustomLayoutItem* item = *it;
        m_items.erase(it);
        return item;
    }
    return nullptr;
}

void CustomLayout::setGeometry(const QRect& rect)
{
    if (m_items.empty() || m_items.front()->Geometry().isEmpty())
    {
        QLayout::setGeometry(rect);
        SetDefault();
    }
    else
    {
        // Получаем новые размеры окна
        QSize newSize = rect.size();

        // Вычисляем коэффициенты масштабирования
        double scaleX = static_cast<double>(newSize.width()) / m_prevSize.width();
        double scaleY = static_cast<double>(newSize.height()) / m_prevSize.height();
        for (CustomLayoutItem* item : m_items)
        {
            const QRectF& itemOldGeometry = item->Geometry();
            QRectF newGeometry = QRectF(itemOldGeometry.x() * scaleX, itemOldGeometry.y() * scaleY, itemOldGeometry.width() * scaleX, itemOldGeometry.height() * scaleY);
            item->SetGeometry(newGeometry);
        }
        m_prevSize = rect.size();
    }
}

void CustomLayout::customEvent(QEvent* event)
{
    //    // CTRL Z
    //    event->accept();
    //    QEvent::Type type = event->type();
    //    Q_ASSERT(CustomLayoutStepEventType == type);
    //    CustomLayoutStepEvent* targetEvent = static_cast<CustomLayoutStepEvent*>(event);
    //    QAbstractAnimation::Direction directionUndoRedo = targetEvent->GetDirection();
    //    if (QAbstractAnimation::Direction::Backward == directionUndoRedo)
    //    {
    //        if (!m_history.empty())
    //        {
    //            const std::list<std::pair<QRectF, QString>>& lastOperation = m_history.front();
    //            if (lastOperation.size() == m_items.size())
    //            {
    //                //                QStringList firstList;
    //                //                firstList.reserve(lastOperation.size());
    //                //                std::transform(lastOperation.cbegin(), lastOperation.cend(), std::back_inserter(firstList), [](const std::pair<QRectF, QString>& pair) {
    //                return
    //                //                pair.second; }); QStringList secondList; secondList.reserve(m_items.size()); std::transform(m_items.cbegin(), m_items.cend(),
    //                //                std::back_inserter(secondList), [](CustomLayoutItem* item) { return item->widget()->objectName(); }); bool isNamesSame =
    //                //                std::is_permutation(firstList.cbegin(), secondList.cend(), secondList.cbegin(), firstList.cend());
    //            }
    //            else
    //            {
    //                m_history.clear();
    //            }
    //        }
    //    }
    //    else
    //    {
    //        qDebug() << "Не реализовано";
    //    }
}

int CustomLayout::indexOf(QWidget* widget) const
{
    int index = 0;
    for (CustomLayoutItem* item : m_items)
    {
        if (item->widget() == widget)
        {
            return index;
        }
        else
        {
            ++index;
        }
    }
    return -1;
}

bool CustomLayout::isEmpty() const
{
    return m_items.empty();
}

bool CustomLayout::eventFilter(QObject* obj, QEvent* event)
{
    QWidget* filteredWidget = qobject_cast<QWidget*>(obj);
    Q_CHECK_PTR(filteredWidget);
    CustomLayoutItem* filteredLayoutItem = GetLayoutItem(filteredWidget);
    QEvent::Type type = event->type();
    switch (type)
    {
    case QEvent::Type::MouseButtonPress:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            QPoint clickLayoutPos = filteredWidget->mapToParent(mouseEvent->localPos().toPoint());
            const QPoint localPos = mouseEvent->pos();
            m_lastMouseClickPosParentWidget = clickLayoutPos;
            m_prevMouseMoveLayoutPos = clickLayoutPos;
            QPoint clickLocalPos = mouseEvent->pos();
            bool isInContentLeft = localPos.x() > s_adjust;
            bool isInContentRight = localPos.x() < filteredWidget->width() - s_adjust;
            bool isInContentTop = localPos.y() > s_adjust;
            bool isInContentBottom = localPos.y() < filteredWidget->height() - s_adjust;
            if (isInContentLeft && isInContentRight && isInContentTop && isInContentBottom)
            {
                //тут начинаем перенос виджета
                filteredWidget->raise();
                m_isTranslating = true;
                m_movedWidgetInitalGeometry = filteredLayoutItem->Geometry();
                parentWidget()->setCursor(Qt::CursorShape::OpenHandCursor);
            }
            else
            {
                //тут начинаем ресайз виджета, определяем сторону за который взял пользователь
                m_isResizing = true;
                m_resizeWidgetEdge = FindClickedWidgetEdge(filteredWidget, clickLocalPos);
                if (m_resizeWidgetEdge.testFlag(Qt::Edge::BottomEdge) || m_resizeWidgetEdge.testFlag(Qt::Edge::TopEdge))
                {
                    if (m_resizeWidgetEdge.testFlag(Qt::Edge::RightEdge) || m_resizeWidgetEdge.testFlag(Qt::Edge::LeftEdge))
                    {
                        m_isResizing = false;
                        m_resizeWidgetEdge = Qt::Edges();
                        m_lastMouseClickPosParentWidget = QPoint();
                        m_prevMouseMoveLayoutPos = QPoint();
                    }
                    else
                    {
                        parentWidget()->setCursor(Qt::CursorShape::SizeVerCursor); // Верхняя или нижняя сторона
                    }
                }
                else
                {
                    parentWidget()->setCursor(Qt::CursorShape::SizeHorCursor); // Боковая сторона
                }
            }
        }
        break;
    }
    case QEvent::Type::MouseMove:
    {
        if (m_isTranslating || m_isResizing) // если вдруг виджет имеет mouseTracking true
        {
            m_hasMouseMove = true;
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QPoint moveLayoutPos = filteredWidget->mapToParent(mouseEvent->localPos().toPoint());

            QPoint deltaMove = moveLayoutPos - m_prevMouseMoveLayoutPos;
            const QRectF& prevGeometry = filteredLayoutItem->Geometry();
            QRectF neuGeometry = prevGeometry;
            if (m_isResizing)
            {
                //тут изменяем геометрию в зависимости от края за который тащим
                //тут надо подумать
                if (m_resizeWidgetEdge.testFlag(Qt::Edge::LeftEdge))
                {
                    neuGeometry.setLeft(neuGeometry.left() + deltaMove.x());
                }
                else
                {
                    if (m_resizeWidgetEdge.testFlag(Qt::Edge::RightEdge))
                    {
                        neuGeometry.setRight(neuGeometry.right() + deltaMove.x());
                    }
                }

                if (m_resizeWidgetEdge.testFlag(Qt::Edge::TopEdge))
                {
                    neuGeometry.setTop(neuGeometry.top() + deltaMove.y());
                }
                else
                {
                    if (m_resizeWidgetEdge.testFlag(Qt::Edge::BottomEdge))
                    {
                        neuGeometry.setBottom(neuGeometry.bottom() + deltaMove.y());
                    }
                }

                bool isinParentLeft = neuGeometry.left() >= 0;
                bool isInParentRight = neuGeometry.right() <= parentWidget()->width();
                bool isInParentTop = neuGeometry.top() >= 0;
                bool isInParentBottom = neuGeometry.bottom() <= parentWidget()->height();
                bool isInParentWidget = isinParentLeft && isInParentRight && isInParentTop && isInParentBottom;
                if (isInParentWidget)
                {
                    bool isWidthInRange = neuGeometry.width() >= filteredLayoutItem->minimumSize().width();
                    bool isHeightInRange = neuGeometry.height() >= filteredLayoutItem->minimumSize().height();
                    bool isMaxSize = neuGeometry.width() < parentWidget()->width() * s_binWidgetPersentSize + 0.01
                        || neuGeometry.height() < parentWidget()->height() * s_binWidgetPersentSize + 0.01;
                    bool isNeuSizeSmaller = neuGeometry.height() < prevGeometry.height() || neuGeometry.width() < prevGeometry.width();
                    if (isWidthInRange && isHeightInRange && (isMaxSize || isNeuSizeSmaller))
                    {
                        filteredLayoutItem->SetGeometry(neuGeometry);
                        //                Обработка наложения виджетов
                        std::list<CustomLayoutItem*> intersectedItems = FindIntersectedItems(filteredLayoutItem);
                        if (!intersectedItems.empty())
                        {
                            std::list<CustomLayoutItem*>::iterator it = intersectedItems.begin();
                            while (it != intersectedItems.end())
                            {
                                CustomLayoutItem* item = *it;

                                const QRectF& itemRect = item->Geometry();

                                if (itemRect.width() > filteredLayoutItem->minimumSize().width() && itemRect.height() > filteredLayoutItem->minimumSize().height())
                                {
                                    QRectF itemWantedRect = itemRect;
                                    if (m_resizeWidgetEdge.testFlag(Qt::Edge::TopEdge))
                                    {
                                        if (itemWantedRect.bottom() > neuGeometry.top())
                                        {
                                            itemWantedRect.setBottom(neuGeometry.top() - 0.001);
                                        }
                                    }
                                    else
                                    {
                                        if (m_resizeWidgetEdge.testFlag(Qt::Edge::BottomEdge))
                                        {
                                            if (itemWantedRect.top() < neuGeometry.bottom())
                                            {
                                                itemWantedRect.setTop(neuGeometry.bottom() + 0.001);
                                            }
                                        }
                                    }

                                    if (m_resizeWidgetEdge.testFlag(Qt::Edge::LeftEdge))
                                    {
                                        if (itemWantedRect.right() > neuGeometry.left())
                                        {
                                            itemWantedRect.setRight(neuGeometry.left() - 0.001);
                                        }
                                    }
                                    else
                                    {
                                        if (m_resizeWidgetEdge.testFlag(Qt::Edge::RightEdge))
                                        {
                                            if (itemWantedRect.left() < neuGeometry.right())
                                            {
                                                itemWantedRect.setLeft(neuGeometry.right() + 0.001);
                                            }
                                        }
                                    }


                                    if (itemWantedRect.size().width() > filteredLayoutItem->minimumSize().width()
                                        && itemWantedRect.size().height() > filteredLayoutItem->minimumSize().height())
                                    {
                                        item->SetGeometry(itemWantedRect);
                                        item->widget()->raise();
                                        it = intersectedItems.erase(it);
                                        continue;
                                    }
                                }
                                ++it;
                            }
                            if (!intersectedItems.empty())
                            {
                                PackingWidgetsTopLeft(filteredLayoutItem);
                                MaximizeDistance(filteredLayoutItem);
                                FillFreeSpaceIteration(filteredLayoutItem);
                            }
                        }

                        FillFreeSpaceFull(filteredLayoutItem);
                    }
                }
            }
            else
            {
                neuGeometry.moveTopLeft(neuGeometry.topLeft() + deltaMove);
                filteredLayoutItem->SetGeometry(neuGeometry);
            }
            m_prevMouseMoveLayoutPos = moveLayoutPos;
        }
        break;
    }
    case QEvent::Type::MouseButtonRelease:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        Qt::MouseButton btn = mouseEvent->button();
        switch (btn)
        {
        case Qt::MouseButton::LeftButton:
        {
            if (m_isTranslating && m_hasMouseMove)
            {
                QPoint mousePosAtParentWidget = filteredWidget->mapToParent(mouseEvent->localPos().toPoint());
                std::list<CustomLayoutItem*>::const_iterator it
                    = std::find_if(m_items.cbegin(), m_items.cend(), [filteredLayoutItem, mousePosAtParentWidget](CustomLayoutItem* item) noexcept {
                          return item != filteredLayoutItem && item->Geometry().contains(mousePosAtParentWidget);
                      });
                if (it == m_items.cend())
                {
                    filteredLayoutItem->SetGeometry(m_movedWidgetInitalGeometry);
                }
                else
                {
                    CustomLayoutItem* layoutItemUnderMouse = *it;
                    filteredLayoutItem->CopyGeometryFrom(layoutItemUnderMouse);
                    layoutItemUnderMouse->SetGeometry(m_movedWidgetInitalGeometry);
                }
            }
            else
            {
                // reisze
                MaximizeDistance(filteredLayoutItem);
                FillFreeSpaceIteration(filteredLayoutItem);
                FillFreeSpaceFull(filteredLayoutItem);
            }
            break;
        }
        case Qt::MouseButton::RightButton:
        {
            m_items.remove(filteredLayoutItem);
            filteredWidget->setVisible(false);
            delete filteredLayoutItem;
            if (!m_items.empty())
            {
                PackingWidgetsTopLeft(nullptr);
                MaximizeDistance(nullptr);
                FillFreeSpaceIteration(nullptr);
                FillFreeSpaceFull(nullptr);
            }
            break;
        }
        default:
        {
            break;
        }
        }
        m_isResizing = false;
        m_isTranslating = false;
        m_hasMouseMove = false;
        m_resizeWidgetEdge = Qt::Edges();
        m_movedWidgetInitalGeometry = QRect();
        m_lastMouseClickPosParentWidget = QPoint();
        m_prevMouseMoveLayoutPos = QPoint();
        parentWidget()->setCursor(Qt::ArrowCursor);
        break;
    }
    case QEvent::Type::MouseButtonDblClick:
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
        if (modifiers.testFlag(Qt::KeyboardModifier::ControlModifier))
        {
            SetDefault();
        }
        else
        {
            MaximizeWidget(filteredLayoutItem);
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return false;
}

std::tuple<double, std::list<QRectF>::const_iterator> CustomLayout::FindClosestPoint(const QPointF& point, const std::list<QRectF>& points) noexcept
{
    std::list<QRectF>::const_iterator closestIt = points.cbegin();
    double minDistance = std::numeric_limits<double>::max();

    for (std::list<QRectF>::const_iterator it = points.cbegin(); it != points.cend(); ++it)
    {
        const QPointF& curPoint = it->topLeft();
        double dist = std::sqrt(std::pow(point.x() - curPoint.x(), 2) + std::pow(point.y() - curPoint.y(), 2));
        if (dist < minDistance)
        {
            minDistance = dist;
            closestIt = it;
        }
    }

    return std::make_tuple(minDistance, closestIt);
}

CustomLayoutItem* CustomLayout::GetLayoutItem(QWidget* widget) const noexcept
{
    std::list<CustomLayoutItem*>::const_iterator it = std::find_if(m_items.cbegin(), m_items.cend(), [widget](CustomLayoutItem* item) { return item->widget() == widget; });
    if (it != m_items.cend())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

void CustomLayout::PackingWidgetsTopLeft(CustomLayoutItem* itemWithFixedPos) noexcept
{
    //    itemWithFixedPos->widget()->removeEventFilter(this);
    std::list<CustomLayoutItem*> layoutWidgets = m_items;

    std::vector<WidgetGeometry> geometriesList;
    geometriesList.reserve(layoutWidgets.size());

    if (itemWithFixedPos != nullptr)
    {
        layoutWidgets.remove(itemWithFixedPos);
        const QRectF& rect = itemWithFixedPos->Geometry();
        geometriesList.push_back(WidgetGeometry(itemWithFixedPos, rect.topLeft(), true));
    }


    for (CustomLayoutItem* item : layoutWidgets)
    {
        geometriesList.push_back(WidgetGeometry(item));
    }

    std::sort(geometriesList.begin(), geometriesList.end(), [](const WidgetGeometry& a, const WidgetGeometry& b) {
        if (a.m_isPosFixed != b.m_isPosFixed)
        {
            return a.m_isPosFixed > b.m_isPosFixed;
        }
        const QRectF& rectA = a.m_layoutItem->Geometry();
        const QRectF& rectB = b.m_layoutItem->Geometry();
        if (std::abs(rectA.y() - rectB.y()) <= s_thresholdByY)
        {
            return rectA.x() < rectB.x();
        }
        else
        {
            return rectA.y() < rectB.y();
        }
    });


    double scaleParam = 0.5;
    bool canPack = false;
    std::vector<WidgetGeometry> contents;
    while (scaleParam > 0)
    {
        ResizedLayoutGridCanvas canvas(parentWidget()->size());
        canPack = canvas.Insert(geometriesList, scaleParam);
        if (canPack)
        {
            qDebug() << "canPack" << scaleParam;
            contents = canvas.GetGeometries();
            break;
        }
        else
        {
            scaleParam -= 0.1;
            qDebug() << "neuScale" << scaleParam;
        }
    }

    if (canPack)
    {
        for (const WidgetGeometry& geometry : contents)
        {
            CustomLayoutItem* item = geometry.m_layoutItem;
            QRectF neuRect(geometry.m_futurPos, geometry.m_size);
            item->SetGeometry(neuRect);
        }
    }
    else
    {
        QMessageBox::information(parentWidget(), "НЕ РАБОТАЕТ !", "НУЖЕН ДРУГОЙ АЛГОРИТМ!!!!!!!!!!!!");
    }
}

void CustomLayout::MaximizeDistance(CustomLayoutItem* expected) noexcept
{
    std::list<CustomLayoutItem*> layoutWidgets = m_items;
    layoutWidgets.remove(expected);
    const QRectF parentArea = parentWidget()->rect();

    size_t iteration = 0;
    while (iteration < 5)
    {
        std::list<CustomLayoutItem*>::iterator it = layoutWidgets.begin();
        for (size_t i = 0; i < layoutWidgets.size(); ++i)
        {
            std::list<CustomLayoutItem*> anotherRects = m_items;
            CustomLayoutItem* layoutItem = *it;
            anotherRects.remove(layoutItem);

            QPointF bestCenter;

            QRectF rect = layoutItem->Geometry();
            double originalDistance = CalcMaxDistToRects(rect, anotherRects);
            double maxDist = 0.0;
            // Попробуем переместить прямоугольник в по разным точкам
            for (const QPointF& dir : m_centerRectPoints)
            {
                rect.moveCenter(dir);
                if (parentArea.contains(rect))
                {
                    if (CanPlaceRect(rect, anotherRects))
                    {
                        maxDist = CalcMaxDistToRects(rect, anotherRects);
                        if (maxDist > originalDistance)
                        {
                            bestCenter = dir;
                            originalDistance = maxDist;
                        }
                    }
                }
            }
            if (!bestCenter.isNull())
            {
                rect.moveCenter(bestCenter);
                layoutItem->SetGeometry(rect);
            }
            ++it;
        }
        ++iteration;
    }


    for (CustomLayoutItem* item : m_items)
    {
        for (CustomLayoutItem* item2 : m_items)
        {
            if (item != item2)
            {
                const QRectF& rect = item->Geometry();
                const QRectF& rect2 = item2->Geometry();
                if (rect.intersects(rect2))
                {
                    qDebug() << "MaximizeDistance " << item->widget()->objectName() << item2->widget()->objectName();
                    Q_UNREACHABLE();
                }
            }
        }
    }
}

void CustomLayout::FillFreeSpaceIteration(CustomLayoutItem* expected) noexcept
{
    std::list<CustomLayoutItem*> layoutWidgets = m_items;
    layoutWidgets.remove(expected);

    size_t iteration = 0;
    while (iteration < 5)
    {
        std::list<CustomLayoutItem*>::const_iterator it = layoutWidgets.cend();
        while (it != layoutWidgets.cbegin())
        {

            --it;
            CustomLayoutItem* itemToExpand = *it;
            const QRectF& itemGeometry = itemToExpand->Geometry();
            double aspectRatio = itemGeometry.width() / itemGeometry.height();
            if (aspectRatio < 1.4)
            {
                std::optional<double> neuLeft = GetMaximizedBorderPos(itemToExpand, Qt::Edge::LeftEdge);
                std::optional<double> neuRight = GetMaximizedBorderPos(itemToExpand, Qt::Edge::RightEdge);
                if (neuLeft.has_value() || neuRight.has_value())
                {

                    QRectF maximizedRect = itemGeometry;
                    if (neuRight.has_value())
                    {
                        maximizedRect.setRight(maximizedRect.right() + ((neuRight.value() - maximizedRect.right())) / 10);
                    }
                    if (neuLeft.has_value())
                    {
                        maximizedRect.setLeft(maximizedRect.left() - ((maximizedRect.left() - neuLeft.value())) / 10);
                    }
                    itemToExpand->SetGeometry(maximizedRect);
                }
            }

            double aspectRatioNeu = itemGeometry.width() / itemGeometry.height();
            if (aspectRatioNeu > 0.7)
            {

                std::optional<double> neuTop = GetMaximizedBorderPos(itemToExpand, Qt::Edge::TopEdge);
                std::optional<double> neuBottom = GetMaximizedBorderPos(itemToExpand, Qt::Edge::BottomEdge);
                if (neuTop.has_value() || neuBottom.has_value())
                {
                    QRectF maximizedRect = itemGeometry;
                    if (neuTop.has_value())
                    {
                        maximizedRect.setTop(maximizedRect.top() - ((maximizedRect.top() - neuTop.value()) / 10));
                    }
                    if (neuBottom.has_value())
                    {
                        maximizedRect.setBottom(maximizedRect.bottom() + ((neuBottom.value() - maximizedRect.bottom()) / 10));
                    }
                    itemToExpand->SetGeometry(maximizedRect);
                }
            }

            for (CustomLayoutItem* item2 : m_items)
            {
                if (itemToExpand != item2)
                {
                    const QRectF& rect = itemToExpand->Geometry();
                    const QRectF& rect2 = item2->Geometry();
                    if (rect.intersects(rect2))
                    {
                        qDebug() << "FillFreeSpaceIteration " << itemToExpand->widget()->objectName() << item2->widget()->objectName();
                        Q_UNREACHABLE();
                    }
                }
            }
        }
        ++iteration;
    }

    for (CustomLayoutItem* item : m_items)
    {
        for (CustomLayoutItem* item2 : m_items)
        {
            if (item != item2)
            {
                const QRectF& rect = item->Geometry();
                const QRectF& rect2 = item2->Geometry();
                if (rect.intersects(rect2))
                {
                    qDebug() << "FillFreeSpaceIteration " << item->widget()->objectName() << item2->widget()->objectName();
                    Q_UNREACHABLE();
                }
            }
        }
    }
}

void CustomLayout::FillFreeSpaceFull(CustomLayoutItem* expected) noexcept
{
    std::list<CustomLayoutItem*> sortedBySize = SortBySize();
    sortedBySize.remove(expected);
    std::list<CustomLayoutItem*>::const_iterator it = sortedBySize.cend();
    while (it != sortedBySize.cbegin())
    {
        --it;
        CustomLayoutItem* itemToExpand = *it;
        const QRectF& itemGeometry = itemToExpand->Geometry();
        std::optional<double> neuLeft = GetMaximizedBorderPos(itemToExpand, Qt::Edge::LeftEdge);
        std::optional<double> neuRight = GetMaximizedBorderPos(itemToExpand, Qt::Edge::RightEdge);
        if (neuLeft.has_value() || neuRight.has_value())
        {
            QRectF maximizedRect = itemGeometry;
            if (neuLeft.has_value())
            {
                maximizedRect.setLeft(neuLeft.value());
            }
            if (neuRight.has_value())
            {
                maximizedRect.setRight(neuRight.value());
            }
            itemToExpand->SetGeometry(maximizedRect);
        }
        std::optional<double> neuTop = GetMaximizedBorderPos(itemToExpand, Qt::Edge::TopEdge);
        std::optional<double> neuBottom = GetMaximizedBorderPos(itemToExpand, Qt::Edge::BottomEdge);
        if (neuTop.has_value() || neuBottom.has_value())
        {
            QRectF maximizedRect = itemGeometry;
            if (neuTop.has_value())
            {
                maximizedRect.setTop(neuTop.value());
            }
            if (neuBottom.has_value())
            {
                maximizedRect.setBottom(neuBottom.value());
            }
            itemToExpand->SetGeometry(maximizedRect);
        }
    }
}

Qt::Edges CustomLayout::FindClickedWidgetEdge(QWidget* widget, const QPoint& clickLocalPos) noexcept
{
    Qt::Edges clickedWidgetEdge = Qt::Edge();
    if (clickLocalPos.x() < s_adjust)
    {
        if (clickLocalPos.y() < s_adjust)
        {
            clickedWidgetEdge = Qt::Edge::TopEdge | Qt::Edge::LeftEdge;
        }
        else
        {
            if (clickLocalPos.y() > widget->height() - s_adjust)
            {
                clickedWidgetEdge = Qt::Edge::BottomEdge | Qt::Edge::LeftEdge;
            }
            else
            {
                clickedWidgetEdge = Qt::Edge::LeftEdge;
            }
        }
    }
    else
    {
        if (clickLocalPos.x() > widget->width() - s_adjust)
        {
            if (clickLocalPos.y() < s_adjust)
            {

                clickedWidgetEdge = Qt::Edge::TopEdge | Qt::Edge::RightEdge;
            }
            else
            {
                if (clickLocalPos.y() > widget->height() - s_adjust)
                {
                    clickedWidgetEdge = Qt::Edge::BottomEdge | Qt::Edge::RightEdge;
                }
                else
                {

                    clickedWidgetEdge = Qt::Edge::RightEdge;
                }
            }
        }
        else
        {
            if (clickLocalPos.y() < s_adjust)
            {
                clickedWidgetEdge = Qt::Edge::TopEdge;
            }
            else
            {
                if (clickLocalPos.y() > widget->height() - s_adjust)
                {
                    clickedWidgetEdge = Qt::Edge::BottomEdge;
                }
            }
        }
    }
    return clickedWidgetEdge;
}

std::list<CustomLayoutItem*> CustomLayout::FindIntersectedItems(CustomLayoutItem* item) const noexcept
{
    std::list<CustomLayoutItem*> intersectedItems;
    std::copy_if(m_items.begin(), m_items.end(), std::back_inserter(intersectedItems),
        [item](CustomLayoutItem* layoutItem) { return layoutItem != item && layoutItem->Geometry().intersects(item->Geometry()); });
    return intersectedItems;
}

std::list<CustomLayoutItem*> CustomLayout::SortBySize() const noexcept
{
    std::list<CustomLayoutItem*> sortedItems = m_items;
    sortedItems.sort([](CustomLayoutItem* a, CustomLayoutItem* b) {
        const QRectF& geometryA = a->Geometry();
        const QRectF& geometryB = b->Geometry();
        double sizeA = geometryA.width() + geometryA.height();
        double sizeB = geometryB.width() + geometryB.height();
        return sizeA < sizeB;
    });
    return sortedItems;
}

std::optional<double> CustomLayout::GetMaximizedBorderPos(CustomLayoutItem* itemToExpand, Qt::Edge edge) noexcept
{
    const QRectF& itemGeometry = itemToExpand->Geometry();
    switch (edge)
    {
    case Qt::Edge::LeftEdge:
    {
        double neuLeft = 0.0;
        QRectF maximizedRect(QPointF(neuLeft, itemGeometry.top() + 0.01), QPointF(itemGeometry.left(), itemGeometry.bottom() - 0.01));
        for (CustomLayoutItem* neighboringItem : m_items)
        {
            if (neighboringItem != itemToExpand)
            {
                const QRectF& anotherItemRect = neighboringItem->Geometry();
                QRectF intersectedRect = maximizedRect.intersected(anotherItemRect);
                if (intersectedRect.isValid())
                {
                    if (neuLeft < intersectedRect.right())
                    {
                        neuLeft = intersectedRect.right();
                    }
                }
            }
        }
        if (neuLeft < itemGeometry.left() && neuLeft >= 0)
        {
            return std::make_optional(neuLeft);
        }
        break;
    }
    case Qt::Edge::RightEdge:
    {
        double neuRight = parentWidget()->width();
        QRectF maximizedRect(QPointF(itemGeometry.right(), itemGeometry.top() + 0.01), QPointF(neuRight, itemGeometry.bottom() - 0.01));
        for (CustomLayoutItem* neighboringItem : m_items)
        {
            if (neighboringItem != itemToExpand)
            {
                const QRectF& anotherItemRect = neighboringItem->Geometry();
                QRectF intersectedRect = maximizedRect.intersected(anotherItemRect);
                if (intersectedRect.isValid())
                {
                    if (neuRight > intersectedRect.left())
                    {
                        neuRight = intersectedRect.left();
                    }
                }
            }
        }
        if (neuRight > itemGeometry.right() && neuRight <= parentWidget()->width())
        {
            return std::make_optional(neuRight);
        }
        break;
    }
    case Qt::Edge::TopEdge:
    {
        double neuTop = 0.0;
        QRectF maximizedRect(QPointF(itemGeometry.left() + 0.01, neuTop), QPointF(itemGeometry.right() - 0.01, itemGeometry.top()));
        for (CustomLayoutItem* neighboringItem : m_items)
        {
            if (neighboringItem != itemToExpand)
            {
                const QRectF& anotherItemRect = neighboringItem->Geometry();
                QRectF intersectedRect = maximizedRect.intersected(anotherItemRect);
                if (intersectedRect.isValid())
                {
                    if (neuTop < intersectedRect.bottom())
                    {
                        neuTop = intersectedRect.bottom();
                    }
                }
            }
        }
        if (neuTop < itemGeometry.top() && neuTop >= 0)
        {
            return std::make_optional(neuTop);
        }
        break;
    }
    case Qt::Edge::BottomEdge:
    {
        double neuBottom = parentWidget()->height();
        QRectF maximizedRect(QPointF(itemGeometry.left() + 0.01, itemGeometry.bottom()), QPointF(itemGeometry.right() - 0.01, neuBottom));
        for (CustomLayoutItem* neighboringItem : m_items)
        {
            if (neighboringItem != itemToExpand)
            {
                const QRectF& anotherItemRect = neighboringItem->Geometry();
                QRectF intersectedRect = maximizedRect.intersected(anotherItemRect);
                if (intersectedRect.isValid())
                {
                    if (neuBottom > intersectedRect.top())
                    {
                        neuBottom = intersectedRect.top();
                    }
                }
            }
        }
        if (neuBottom > itemGeometry.bottom() && neuBottom <= parentWidget()->height())
        {
            return std::make_optional(neuBottom);
        }
        break;
    }
    default:
    {
        break;
    }
    }
    return std::nullopt;
}

void CustomLayout::MaximizeWidget(CustomLayoutItem* item) noexcept
{
    const QRectF& startGeometry = item->Geometry();
    //сначала определяю 2 точки угла в котором будут виджеты
    double x = 0.0;
    double y = 0.0;
    if (startGeometry.left() == parentWidget()->rect().left())
    {
        x = parentWidget()->rect().right() - parentWidget()->width() * s_smallWidgetPercentSize + 1.3;
    }
    if (startGeometry.top() == parentWidget()->rect().top())
    {
        y = parentWidget()->rect().bottom() - parentWidget()->height() * s_smallWidgetPercentSize + 1.3;
    }

    QSizeF itemRectSize = parentWidget()->size();
    itemRectSize *= s_binWidgetPersentSize;

    QPointF topLeftBigRect;
    if (x == 0)
    {
        topLeftBigRect.setX(x + parentWidget()->width() * s_smallWidgetPercentSize);
    }
    else
    {
        topLeftBigRect.setX(x - itemRectSize.width());
    }

    if (y == 0)
    {
        topLeftBigRect.setY(y + parentWidget()->height() * s_smallWidgetPercentSize);
    }
    else
    {
        topLeftBigRect.setY(y - itemRectSize.height());
    }
    item->SetGeometry(QRectF(topLeftBigRect, itemRectSize));


    std::list<CustomLayoutItem*> anotherItems;
    std::copy(m_items.cbegin(), m_items.cend(), std::back_inserter(anotherItems));
    anotherItems.remove(item);


    //потом создаю список из точек для вставки
    std::list<QRectF> pointsToInsertWidgets;
    int horizontalItemsCount = anotherItems.size() / 2 + 1;
    int verticalItemsCount = anotherItems.size() - horizontalItemsCount;


    QPointF horizontalPointBegin(0, y);
    double neuWidetWidth = parentWidget()->width() / horizontalItemsCount;
    for (int i = 0; i < horizontalItemsCount; ++i)
    {
        pointsToInsertWidgets.push_back(
            QRectF(horizontalPointBegin + QPointF(i * neuWidetWidth, 0), QSizeF(neuWidetWidth, parentWidget()->height() * s_smallWidgetPercentSize - 1.3)));
    }

    QPointF verticalPointBegin(x, 0);
    double verticalHeightEnd = parentWidget()->height();
    if (y == 0.0)
    {
        verticalPointBegin.setY(parentWidget()->height() * s_smallWidgetPercentSize);
    }
    else
    {
        verticalHeightEnd -= parentWidget()->height() * s_smallWidgetPercentSize;
    }
    double totalHeight = verticalHeightEnd - verticalPointBegin.y();
    double neuWidetHeight = totalHeight / verticalItemsCount;
    for (int i = 0; i < verticalItemsCount; ++i)
    {
        pointsToInsertWidgets.push_back(
            QRectF(verticalPointBegin + QPointF(0.0, i * neuWidetHeight), QSizeF(parentWidget()->width() * s_smallWidgetPercentSize - 1.3, neuWidetHeight - 1.3)));
    }


    //теперь я смотрю какой виджет ближайший
    //сначала прохожу по всем виджетам
    //потом по всем точкам, ищу для виджета ближайшую точку
    //
    std::list<std::pair<CustomLayoutItem*, QRectF>> itemPointPairs;

    for (CustomLayoutItem* item : anotherItems)
    {
        std::list<QRectF>::const_iterator closestIt = pointsToInsertWidgets.cend();
        QPointF itemPosition = item->Geometry().topLeft();
        auto [dist, iter] = FindClosestPoint(itemPosition, pointsToInsertWidgets);
        closestIt = iter;
        Q_ASSERT(closestIt != pointsToInsertWidgets.cend());
        itemPointPairs.emplace_back(item, *closestIt);
        pointsToInsertWidgets.erase(closestIt);
    }

    for (const std::pair<CustomLayoutItem*, QRectF>& widgetPos : itemPointPairs)
    {
        CustomLayoutItem* item = widgetPos.first;
        item->SetGeometry(widgetPos.second);
    }
}

void CustomLayout::SetDefault() noexcept
{
    double itemCount = static_cast<double>(count());
    int numRows = static_cast<int>(sqrt(itemCount));
    int numCols = std::ceil(itemCount / numRows);
    if (numCols > 0 && numRows > 0)
    {
        double itemWidth = parentWidget()->rect().width() / numCols;
        double itemHeight = parentWidget()->rect().height() / numRows;

        std::list<CustomLayoutItem*>::iterator it = m_items.begin();
        for (size_t i = 0; i < m_items.size(); ++i)
        {
            int row = i / numCols;
            int col = i % numCols;
            QPointF topLeft(parentWidget()->rect().x() + col * itemWidth, parentWidget()->rect().y() + row * itemHeight);
            QRectF itemRect(topLeft, QSize(itemWidth, itemHeight));
            (*it)->SetGeometry(itemRect);
            ++it;
        }
        m_prevSize = parentWidget()->rect().size();
    }
}

double CustomLayout::CalculateDistance(const CustomLayoutItem* r1, const QRectF& r2) noexcept
{
    const QRectF& geometry1 = r1->Geometry();
    const QPointF& center1 = geometry1.center();
    const QPointF& center2 = r2.center();
    return std::pow(center1.x() - center2.x(), 2) + std::pow(center1.y() - center2.y(), 2.0);
}

bool CustomLayout::CanPlaceRect(const QRectF& newRect, const std::list<CustomLayoutItem*>& placedRectangles)
{
    for (const CustomLayoutItem* placed : placedRectangles)
    {
        const QRectF& placedRect = placed->Geometry();
        if (placedRect.intersects(newRect))
        {
            return false;
        }
    }
    return true;
}

double CustomLayout::CalcMaxDistToRects(const QRectF& curRect, const std::list<CustomLayoutItem*>& placedRectangles)
{
    double minDistance = std::numeric_limits<double>::max();
    for (const CustomLayoutItem* item : placedRectangles)
    {
        double distance = CalculateDistance(item, curRect);
        minDistance = std::min(minDistance, distance);
    }
    return minDistance;
}
