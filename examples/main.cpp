#include <QtCore/QtCore>
#include "lifxmanager.h"

/* Very early example, more to come soon */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    LifxManager manager;

    manager.initialize();
    
    return app.exec();
}
