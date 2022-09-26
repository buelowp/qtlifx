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
#include "bulbmessagehandler.h"

class LifxApplication : public QMainWindow
{
    Q_OBJECT
    
public:
    LifxApplication();
    ~LifxApplication();
    
    void setProductsJsonFile(QString path);
    void go();

public slots:
    void widgetStateChange(QString label, bool state);
    void newColorForBulb(QString label, QColor color);
    void handlerTimeout();
    void bulbStateChange(LifxBulb *bulb);
    void bulbDiscoveryFinished(LifxBulb *bulb);
    void handlerComplete(uint32_t m_uniqueId);
    void runStateCheck();
    void ackReceived(QHostAddress address, int port, uint64_t target);

protected:
    void showEvent(QShowEvent *e);
    
private:
    int xPosition();
    int yPosition();
    void createDisplayObject(LifxBulb *bulb);
    void updateDisplayObject(LifxBulb *bulb);
    BulbMessageHandler *createHandler();
    BulbMessageHandler *createHandler(QHostAddress address, int port);

    LifxManager *m_manager;
    QGridLayout *m_layout;
    QTimer *m_stateCheckInterval;
    QTimer *m_discoverInterval;
    QMap<QString, LightBulb*> m_widgets;
    QMap<int, BulbMessageHandler*> m_handlers;
    int m_x;
    int m_y;
    uint32_t m_uniqueId;
};

#endif // LIFXAPPLICATION_H
