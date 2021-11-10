#include <QtCore/QtCore>
#include "lifxapplication.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LifxApplication manager;
    
    manager.setProductsJsonFile(QString(argv[1]));
    manager.go();
    manager.setGeometry(100, 100, 800, 480);
    manager.show();
    
    return app.exec();
}
