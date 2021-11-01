#ifndef QTLIFX_H
#define QTLIFX_H

#include <QMainWindow>
#include <QScopedPointer>

namespace Ui {
class qtlifx;
}

class qtlifx : public QMainWindow
{
    Q_OBJECT

public:
    explicit qtlifx(QWidget *parent = nullptr);
    ~qtlifx() override;

private:
    QScopedPointer<Ui::qtlifx> m_ui;
};

#endif // QTLIFX_H
