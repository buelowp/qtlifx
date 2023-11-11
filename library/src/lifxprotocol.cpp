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

LifxProtocol::LifxProtocol(int port, QObject *parent) : QObject(parent), m_port(port)
{
    m_socket = new QUdpSocket();
    if (m_socket->bind(BROADCAST_PORT)) {
        if (m_socket->state() != QAbstractSocket::BoundState)
            qDebug() << __PRETTY_FUNCTION__ << ": m_socket is not bound";
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": bind returned false";
    }

    connect(m_socket, &QUdpSocket::readyRead, this, &LifxProtocol::readDatagram);
    connect(m_socket, &QUdpSocket::errorOccurred, this, &LifxProtocol::socketError);
}

LifxProtocol::LifxProtocol(const LifxProtocol& protocol) : QObject()
{
    m_socket = protocol.m_socket;
}

LifxProtocol::~LifxProtocol()
{
}

void LifxProtocol::readDatagram()
{
    QHostAddress address;

    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        if (datagram.isValid()) {
            LifxPacket *packet = new LifxPacket(datagram);
            if (packet->type() != 2)
                newPacket(packet);
        }
        else {
            qWarning() << __PRETTY_FUNCTION__ << ": Invalid datagram detected";
        }
    }
}

void LifxProtocol::socketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << __PRETTY_FUNCTION__ << ":" << socketError;
}

void LifxProtocol::discover()
{
    LifxPacket packet;
    packet.makeDiscoveryPacket();
    QHostAddress addr(QHostAddress::Broadcast);

    sendPacket(packet.datagram(), addr, m_port);
}

void LifxProtocol::sendPacket(QByteArray data, QHostAddress& address, quint16 port)
{
    m_mutex.lock();
    m_socket->writeDatagram(data, address, port);
    m_mutex.unlock();
}

uint16_t LifxProtocol::getPowerForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;
    m_mutex.lock();
    type = packet.getBulbPower(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    m_mutex.unlock();
    return type;
}

uint16_t LifxProtocol::getLabelForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbLabel(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getFirmwareForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbFirmware(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getVersionForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbVersion(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getColorForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbColor(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::setBulbColor(LifxBulb* bulb, int source, bool ackRequired)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.setBulbColor(bulb, source, ackRequired);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getGroupForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getBulbGroup(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::getWifiInfoForBulb(LifxBulb* bulb, int source)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.getWifiInfoForBulb(bulb, source);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::rebootBulb(LifxBulb* bulb)
{
    LifxPacket packet;
    uint16_t type;

    type = packet.rebootBulb(bulb);
    sendPacket(packet.datagram(), bulb->address(), bulb->port());
    return type;
}

uint16_t LifxProtocol::setBulbState(LifxBulb* bulb, int source, bool ackRequired)
{
    LifxPacket packet;
    uint16_t type;

    if (bulb) {
        type = packet.setBulbPower(bulb, source, ackRequired);
        sendPacket(packet.datagram(), bulb->address(), bulb->port());
        return type;
    }
    return 0;
}

void LifxProtocol::setGroupState(LifxGroup* group, int source, bool ackRequired)
{
    LifxPacket packet;

    QVector<LifxBulb*> bulbs = group->bulbs();
    for (auto bulb : bulbs) {
        packet.setBulbPower(bulb, source, ackRequired);
        sendPacket(packet.datagram(), bulb->address(), bulb->port());
        QThread::msleep(1);
    }
}

/**
 * \fn void LifxManager::newPacket(LifxPacket* packet)
 * \param packet Pointer to a LifxPacket container
 * \brief SLOT called when a new packet arrives from the protocol manager
 *
 * This handles the state messages and decodes the packets by stuffing them into
 * a bulb container and emitting signals to indicate something happened.
 */
void LifxProtocol::newPacket(LifxPacket *packet)
{
    LifxBulb *bulb;
    uint64_t target = packet->targetAsLong();

    switch (packet->type()) {
        case LIFX_DEFINES::STATE_SERVICE:
            if (!m_bulbs.contains(target)) {
                bulb = new LifxBulb(this);
                bulb->setDiscoveryActive(true);
                lx_dev_service_t *service = (lx_dev_service_t*)packet->payload().data();
                bulb->setAddress(packet->address(), service->port);
                bulb->setService(service->service);
                bulb->setTarget(packet->target());
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": SERVICE:" << bulb;
                m_bulbs[target] = bulb;
                getLabelForBulb(bulb);
            }
            else {
                bulb = m_bulbs[target];
                bulb->setAddress(packet->address(), packet->port());
            }
            break;
        case LIFX_DEFINES::STATE_LABEL:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                bulb->setLabel(QString::fromUtf8(packet->payload()));
                if (bulb->inDiscovery()) {
                    getFirmwareForBulb(bulb);
                }
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": LABEL:" << bulb;
            }
            else {
                qWarning() << __PRETTY_FUNCTION__ << ": Got a STATE_LABEL for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_HOST_FIRMWARE:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_firmware_t *firmware = (lx_dev_firmware_t*)packet->payload().data();
                bulb->setMajor(firmware->major);
                bulb->setMinor(firmware->minor);
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": FIRMWARE:" << bulb;
                if (bulb->inDiscovery()) {
                    getWifiInfoForBulb(bulb);
                }
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_HOST_FIRMWARE for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_VERSION:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_version_t *version = (lx_dev_version_t*)packet->payload().data();
                bulb->setVID(version->vendor);
                bulb->setPID(version->product);
//                if (m_productObjects.size() && m_productObjects.contains(version->product)) {
//                    bulb->setProduct(m_productObjects[version->product]);
//                }
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": VERSION:" << bulb;

                if (bulb->inDiscovery()) {
                    getGroupForBulb(bulb);
                }
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_VERSION for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_GROUP:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                QString label;
                QByteArray uuid;
                bulb = m_bulbs[target];
                lx_group_info_t *group = (lx_group_info_t*)packet->payload().data();
                label = QString(group->label);
                uuid = QByteArray::fromRawData(group->group, 16);
                bulb->setGroup(group->label);
                if (m_groups.contains(uuid)) {
                    LifxGroup *g = m_groups[uuid];
                    if (g != nullptr) {
                        if (!g->contains(bulb)) {
                            g->addBulb(bulb);
                        }
                        if (m_debug)
                            qDebug() << __PRETTY_FUNCTION__ << ": GROUP (add):" << g;
                    }
                }
                else {
                    LifxGroup *g = new LifxGroup(label, uuid, group->updated_at);
                    g->addBulb(bulb);
                    m_groups[uuid] = g;
                    emit newGroupFound(g);
                    if (m_debug)
                        qDebug() << __PRETTY_FUNCTION__ << ": GROUP (new):" << g;
                }
                if (bulb->inDiscovery()) {
                    getColorForBulb(bulb);
                }
            }
            break;
        case LIFX_DEFINES::LIGHT_STATE:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_lightstate_t *color = (lx_dev_lightstate_t*)packet->payload().data();
                bulb->setDevColor(color);
                if (bulb->inDiscovery()) {
                    bulb->setDiscoveryActive(false);
                    emit newBulbFound(bulb);
                }

                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": COLOR:" << bulb;
            }
            else {
                qWarning() << __PRETTY_FUNCTION__ << ": Got a LIGHT_STATE for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_POWER:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                uint16_t power = 0;
                memcpy(&power, packet->payload().data(), 2);
                bulb->setPower(power);
                if (m_debug)
                    qDebug().nospace() << "POWER: bulb returned " << packet->payload() << " [" << power << "]" << " from bulb " << bulb->label();
            }
            break;
        case LIFX_DEFINES::ECHO_REPLY:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            break;
        case LIFX_DEFINES::STATE_WIFI_INFO:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                float signal = 0;
                memcpy(&signal, packet->payload().data(), 4);
                bulb->setRSSI(signal);
                if (bulb->inDiscovery())
                    getVersionForBulb(bulb);
            }
            break;
        case LIFX_DEFINES::ACKNOWLEDGEMENT:
            if (m_debug)
                qDebug() << __PRETTY_FUNCTION__ << ": Acknowledgment sent from" << packet->address().toString() << ":" << packet;
            break;
        default:
            if (packet->type() != LIFX_DEFINES::GET_SERVICE) {
                qWarning() << __PRETTY_FUNCTION__ << ": Unknown packet type" << packet->type();
                qWarning() << packet;
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ":" << packet;
            }
            break;
    }

    delete packet;
}
