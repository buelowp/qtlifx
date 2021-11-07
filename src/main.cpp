#include <QtCore/QtCore>
#include "lifxmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    LifxManager manager;

    QObject::connect(&manager, &LifxManager::finished, &app, &QCoreApplication::quit);
    manager.initialize();
    
    return app.exec();
}

