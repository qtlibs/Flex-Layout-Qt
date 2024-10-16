#ifndef CUSTOMLAYOUTITEM_HPP
#define CUSTOMLAYOUTITEM_HPP

#include <QLayoutItem>
#include <QWidget>
#include <QDebug>

class CustomLayoutItem final : public QLayoutItem
{
public:
    explicit CustomLayoutItem(QWidget* widget, int index);
    virtual ~CustomLayoutItem() noexcept
    {
        qDebug() << "~CustomLayoutItem()";
    }


public:
    void SetGeometry(const QRectF& rect) noexcept;
    const QRectF& Geometry() const noexcept;
    void CopyGeometryFrom(CustomLayoutItem* item) noexcept;

private:
    void UpdateGeometry();

public:
    QSize sizeHint() const override;
    QSize minimumSize() const override;
    Qt::Orientations expandingDirections() const override;
    bool isEmpty() const override;
    Q_DECL_HIDDEN Q_DECL_DEPRECATED_X("Используйте SetGeometry") void setGeometry(const QRect& rect) override;
    Q_DECL_HIDDEN Q_DECL_DEPRECATED_X("Используйте Geometry") QRect geometry() const override;
    QWidget* widget() override;
    QSize maximumSize() const override;

public:
    int m_index;

private:
    QWidget* m_widget;
    QRectF m_geometry;
};


#endif // CUSTOMLAYOUTITEM_HPP
