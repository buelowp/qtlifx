/*
 * Simple QT app for interacting with LIFX bulbs
 * 
 * Copyright (C) 2021  Peter Buelow <goballstate at gmail>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
