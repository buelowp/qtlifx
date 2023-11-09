/*
 * Internal protocol management
 * 
 * Copyright (C) 2021 Peter Buelow <goballstate at gmail>
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

#include "lifxprotocol.h"

LifxProtocol::LifxProtocol(QObject *parent) : QObject(parent)
{
    m_socket = nullptr;
    m_socket = new QUdpSocket(this);
    m_socket->bind(LIFX_PORT);
    connect(m_socket, &QUdpSocket::readyRead, this, &LifxProtocol::readDatagram);
}

LifxProtocol::~LifxProtocol()
{
}

LifxProtocol::LifxProtocol(const LifxProtocol& object) : QObject()
{
    m_socket = object.m_socket;
}

qint64 LifxProtocol::discover()
{
    LifxPacket packet;
    packet.makeDiscoveryPacket();
    return m_socket->writeDatagram(packet.datagram(), QHostAddress::Broadcast, LIFX_PORT);
}

qint64 LifxProtocol::discoverBulbByAddress(QHostAddress address, int port)
{
    LifxPacket packet;
    packet.makeDiscoveryPacketForBulb(address, port);
    return m_socket->writeDatagram(packet.datagram(), address, port);
}

void LifxProtocol::readDatagram()
{
    QByteArray datagram;
    QHostAddress address;

    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        if (datagram.isValid()) {
            LifxPacket *packet = new LifxPacket;

            packet->setDatagram(datagram);
            emit newPacket(packet);
        }
        else {
            qWarning() << __PRETTY_FUNCTION__ << ": Invalid datagram detected";
        }
    }
}

bool LifxProtocol::newPacketAvailable()
{
    return m_socket->hasPendingDatagrams();
}

void LifxProtocol::initialize()
{
}

uint16_t LifxProtocol::getPowerForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;
    
    type = packet.getBulbPower(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getLabelForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbLabel(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getFirmwareForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbFirmware(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getVersionForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbVersion(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getColorForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbColor(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::setBulbColor(LifxBulb* bulb, int source, bool ackRequired)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.setBulbColor(bulb, source, ackRequired);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::setBulbColor(LifxBulb* bulb, QColor color, int source, bool ackRequired)
{
    LifxPacket packet;
    uint16_t type;

    bulb->setColor(color);
    type = packet.setBulbColor(bulb, source, ackRequired);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getGroupForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbGroup(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getWifiInfoForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getWifiInfoForBulb(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::rebootBulb(LifxBulb* bulb)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.rebootBulb(bulb);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::setBulbState(LifxBulb* bulb, bool state, int source, bool ackRequired)
{
    uint16_t power = state ? 65535 : 0;
    LifxPacket packet;
    uint16_t type;

    if (bulb) {
        bulb->setPower(power);
        type = packet.setBulbPower(bulb, source, ackRequired);
        m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
        return type;
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Protocol request for a NULL bulb";
    }
    return 0;
}

void LifxProtocol::setGroupState(LifxGroup* group, bool state, int source, bool ackRequired)
{
    uint16_t power = state ? 65535 : 0;
    
    LifxPacket packet;

    QVector<LifxBulb*> bulbs = group->bulbs();
    for (auto bulb : bulbs) {
        bulb->setPower(power);
        packet.setBulbPower(bulb, source, ackRequired);
        m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    }
}

void LifxProtocol::echoRequest(LifxBulb* bulb, QByteArray echoing)
{
    if (bulb) {
        LifxPacket packet;
        packet.echoBulb(bulb, echoing);
        m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
    }
}

