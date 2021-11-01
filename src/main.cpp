#include "qtlifx.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qtlifx w;
    w.show();

    return app.exec();
}

