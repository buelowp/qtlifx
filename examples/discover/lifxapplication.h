/*
 * Example of a simple LIFX controller application
 * 
 * Copyright (C) 2021  LIFX product details from the product.json file
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

#ifndef LIFXAPPLICATION_H
#define LIFXAPPLICATION_H

#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>
#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

#include <lifxbulb.h>
#include <lifxmanager.h>

#include "lightbulb.h"

class LifxApplication : public QMainWindow
{
    Q_OBJECT
    
public:
    LifxApplication();
    ~LifxApplication();
    
    void setProductsJsonFile(QString path);
    void go();

public slots:
    void widgetStateChange(LifxBulb *bulb, bool state);
    void newColorForBulb(LifxBulb *bulb, QColor color);
    void handlerTimeout(uint32_t m_uniqueId);
    void bulbStateChange(LifxBulb *bulb);
    void bulbDiscoveryFinished(LifxBulb *bulb);
    void runStateCheck(LifxBulb *bulb);
    void ackReceived(uint32_t uniqueId);
    void discoveryFailed();

protected:
    void showEvent(QShowEvent *e);
    
private:
    int xPosition();
    int yPosition();
    void createDisplayObject(LifxBulb *bulb);
    void updateDisplayObject(LifxBulb *bulb);
    LightBulb* getWidgetByName(QString label);

    LifxManager *m_manager;
    QGridLayout *m_layout;
    QTimer *m_stateCheckInterval;
    QTimer *m_discoverInterval;
    QMap<LifxBulb*, LightBulb*> m_widgets;
    int m_x;
    int m_y;
    uint32_t m_uniqueId;
};

#endif // LIFXAPPLICATION_H
