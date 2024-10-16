#ifndef MYWIDET_HPP
#define MYWIDET_HPP

#include <QWidget>
#include <QMessageBox>
#include <QLayout>
#include <QMouseEvent>

class MyWidet : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidet();

public Q_SLOTS:
    void OnAddNeuBtn() noexcept;

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    static constexpr const int s_maxWidgets = 16;
};

#endif // MYWIDET_HPP
