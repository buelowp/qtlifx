/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2021  <copyright holder> <email>
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

#ifndef LIFXPROTOCOL_H
#define LIFXPROTOCOL_H

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

#include "defines.h"
#include "lifxbulb.h"
#include "lifxpacket.h"

class LifxProtocol : public QObject
{
    Q_OBJECT

    static constexpr int LIFX_PORT = 56700;
    
public:
    LifxProtocol(QObject *parent = nullptr);
    ~LifxProtocol();

    void initialize();
    qint64 discover();
    LifxPacket* nextPacket();
    bool newPacketAvailable();
    LifxBulb *createNewBulb();
    
    void setBulbColor(LifxBulb *bulb);
    
    void getPowerForBulb(LifxBulb *bulb);
    void getLabelForBulb(LifxBulb *bulb);
    void getVersionForBulb(LifxBulb *bulb);
    void getColorForBulb(LifxBulb *bulb);

protected slots:
    void readDatagram();
    void discoveryTimeout();

signals:
    void datagramAvailable();
    void discoveryFailed();
    void newPacket(LifxPacket *packet);
    
private:
    QUdpSocket *m_socket;
    QTimer *m_discovery;
    QMutex m_mutex;
};

#endif // LIFXPROTOCOL_H
