/*
 * LIFX protocol encapsulation
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

#ifndef LIFXPROTOCOL_H
#define LIFXPROTOCOL_H

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

#include "defines.h"
#include "lifxbulb.h"
#include "lifxpacket.h"
#include "lifxgroup.h"

class LifxGroup;
class LifxBulb;
class LifxPacket;

/**
 * \class LifxProtocol
 * \brief (PRIVATE) The UDP protocol manager for sending/receiving bulb data
 * 
 * This class controls access to the bulbs over the wire. It used LifxPacket
 * internally to create a header/payload to send and then decodes the incoming
 * packet into a header and payload which can then be used to update a bulb
 * or to provide state data to the application
 */
class LifxProtocol : public QObject
{
    Q_OBJECT

public:
    LifxProtocol(int port = BROADCAST_PORT, QObject *parent = nullptr);
    LifxProtocol(const LifxProtocol &protocol);
    ~LifxProtocol();

    void discover();
    
    uint16_t setBulbColor(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t setBulbState(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    void setGroupState(LifxGroup *group, int source = 0, bool ackRequired = false);
    uint16_t rebootBulb(LifxBulb *bulb);
    
    uint16_t getPowerForBulb(LifxBulb *bulb, int source = 0);
    uint16_t getLabelForBulb(LifxBulb *bulb, int source = 0);
    uint16_t getVersionForBulb(LifxBulb *bulb, int source = 0);
    uint16_t getFirmwareForBulb(LifxBulb *bulb, int source = 0);
    uint16_t getColorForBulb(LifxBulb *bulb, int source = 0);
    uint16_t getGroupForBulb(LifxBulb *bulb, int source = 0);
    uint16_t getWifiInfoForBulb(LifxBulb *bulb, int source = 0);
    
    void enableDebug(bool debug) { m_debug = debug; }

public slots:
    void readDatagram();
    void socketError(QAbstractSocket::SocketError socketError);

signals:
    void newBulbFound(LifxBulb *bulb);
    void newGroupFound(LifxGroup *group);

private:
    void newPacket(LifxPacket *packet);
    void sendPacket(QByteArray data, QHostAddress &address, quint16 port);

    QUdpSocket *m_socket;
    int m_port;
    QMap<uint64_t, LifxBulb*> m_bulbs;
    QMap<QByteArray, LifxGroup*> m_groups;
    bool m_debug;
    QMutex m_mutex;
};

Q_DECLARE_METATYPE(LifxProtocol);
#endif // LIFXPROTOCOL_H
