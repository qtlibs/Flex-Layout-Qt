#ifndef RECTWIDGET_HPP
#define RECTWIDGET_HPP

#include <QWidget>
#include <QPainter>
#include <QDebug>

class RectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RectWidget(const QSize& size, const QColor& color, int index, QWidget* parent = nullptr);
    virtual ~RectWidget()
    {
        qDebug() << "~RectWidget";
    }

public:
    const QSize m_size;
    const QColor m_color;
    QString m_index = 0;
    int indexInt = 0;

protected:
    void paintEvent(QPaintEvent* event) override;
};

#endif // RECTWIDGET_HPP
