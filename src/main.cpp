#include <QtCore/QtCore>
#include "lightmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    LightManager manager;

    QObject::connect(&manager, &LightManager::finished, &app, &QCoreApplication::quit);
//    QTimer::singleShot(0, &manager, &LightManager::run);
    manager.initialize();
    
    return app.exec();
}

