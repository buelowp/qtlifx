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
    m_socket = new QUdpSocket();
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
    }
}

bool LifxProtocol::newPacketAvailable()
{
    return m_socket->hasPendingDatagrams();
}

void LifxProtocol::initialize()
{
}

void LifxProtocol::getPowerForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    
    packet.getBulbPower(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::getLabelForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    packet.getBulbLabel(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::getFirmwareForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    packet.getBulbFirmware(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::getVersionForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    packet.getBulbVersion(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::getColorForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    packet.getBulbColor(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());    
}

void LifxProtocol::setBulbColor(LifxBulb* bulb, int source, bool ackRequired)
{
    LifxPacket packet;
    packet.setBulbColor(bulb, source, ackRequired);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::setBulbColor(LifxBulb* bulb, QColor color, int source, bool ackRequired)
{
    LifxPacket packet;
    bulb->setColor(color);
    packet.setBulbColor(bulb, source, ackRequired);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());    
}

void LifxProtocol::getGroupForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    packet.getBulbGroup(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::getWifiInfoForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    packet.getWifiInfoForBulb(bulb, source);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::rebootBulb(LifxBulb* bulb)
{
    LifxPacket packet;
    packet.rebootBulb(bulb);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
}

void LifxProtocol::setBulbState(LifxBulb* bulb, bool state, int source, bool ackRequired)
{
    uint16_t power = state ? 65535 : 0;
    LifxPacket packet;
    bulb->setPower(power);
    packet.setBulbPower(bulb, source, ackRequired);
    m_socket->writeDatagram(packet.datagram(), bulb->address(), bulb->port());
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

