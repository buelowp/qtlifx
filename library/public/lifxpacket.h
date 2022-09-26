/*
 * LIFX specific packet container class
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

#ifndef LIFXPACKET_H
#define LIFXPACKET_H

#include <iostream>

#include <QtCore/QtCore>

#include "defines.h"
#include "lifxbulb.h"

/**
 * \class LifxPacket
 * \brief (PRIVATE) This is an internal class to the library and should not be used by apps
 * 
 * The packet object contains the payload to be sent over the wire. It is created by the
 * protocol manager and destroyed in kind.
 */
class LifxPacket
{
public:
    LifxPacket();
    ~LifxPacket();
    
    LifxPacket(const LifxPacket &object);
    static constexpr uint8_t protoid[] = { 0x4c, 0x49, 0x46, 0x58, 0x56, 0x32 };

    void makeDiscoveryPacket();
    void makeDiscoveryPacketForBulb(QHostAddress address, int port);
    void setHeader(const char *data);
    void setPayload(QByteArray ba);
    void setDatagram(char *data, int len, QHostAddress &addr, quint16 port);
    void setDatagram(QNetworkDatagram &datagram);

    uint16_t size() { return m_size; }
    uint16_t type() { return m_type; }
    QHostAddress address() const { return m_address; }
    QByteArray datagram();
    QByteArray payload() const { return m_payload; }
    QByteArray header() const { return m_hdr; }
    lx_protocol_header_t protocolHeader() const { return m_header; }
    uint16_t port() const { return m_port; }
    uint8_t* target() { return m_target; }
    uint64_t targetAsLong();
    bool isValid();
    
    uint16_t getBulbPower(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t getBulbFirmware(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t getBulbLabel(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t getBulbColor(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t getBulbGroup(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t getBulbVersion(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t getWifiInfoForBulb(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t setBulbColor(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t setBulbPower(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    uint16_t rebootBulb(LifxBulb *bulb);
    void echoBulb(LifxBulb *bulb, QByteArray bytes, int source = 0);

    static constexpr int HEADER_SIZE = 36;
    
private:
    void createHeader(LifxBulb *bulb, bool blankTarget = true);

    QByteArray m_payload;
    QByteArray m_hdr;
    QByteArray m_datagram;
    QHostAddress m_address;
    uint8_t m_tagged;
    bool m_ackRequired;
    bool m_resRequired;
    uint16_t m_type;
    uint32_t m_source;
    uint32_t m_port;
    uint8_t m_target[8];
    uint16_t m_protocol;
    uint16_t m_size;
    uint8_t m_addressable;
    uint8_t m_headerSize;
    uint8_t m_protoid[6];
    lx_protocol_header_t m_header;
    char *m_packet;
};

QDebug operator<<(QDebug debug, LifxPacket &packet);
QDebug operator<<(QDebug debug, LifxPacket *packet);
#endif // LIFXPACKET_H
