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
    static constexpr int LIFX_PORT = 56700;
    
    LifxProtocol(QObject *parent = nullptr);
    ~LifxProtocol();
    LifxProtocol(const LifxProtocol &object);

    void initialize();
    qint64 discover();
    LifxPacket* nextPacket();
    bool newPacketAvailable();
    LifxBulb *createNewBulb();
    
    void setBulbColor(LifxBulb *bulb);
    void setBulbColor(LifxBulb *bulb, QColor color);
    void setBulbState(LifxBulb *bulb, bool state);
    void setGroupState(LifxGroup *group, bool state);
    void rebootBulb(LifxBulb *bulb);
    
    void getPowerForBulb(LifxBulb *bulb);
    void getLabelForBulb(LifxBulb *bulb);
    void getVersionForBulb(LifxBulb *bulb);
    void getFirmwareForBulb(LifxBulb *bulb);
    void getColorForBulb(LifxBulb *bulb);
    void getGroupForBulb(LifxBulb *bulb);
    
    void echoRequest(LifxBulb *bulb);

protected slots:
    void readDatagram();

signals:
    void datagramAvailable();
    void discoveryFailed();
    void newPacket(LifxPacket *packet);
    
private:
    QUdpSocket *m_socket;
};

Q_DECLARE_METATYPE(LifxProtocol);
#endif // LIFXPROTOCOL_H
