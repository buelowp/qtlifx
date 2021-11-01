#include "qtlifx.h"
#include "ui_qtlifx.h"

qtlifx::qtlifx(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::qtlifx)
{
    m_ui->setupUi(this);
}

qtlifx::~qtlifx() = default;
