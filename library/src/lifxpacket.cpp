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

#include "lifxpacket.h"

LifxPacket::LifxPacket()
{
    m_headerSize = sizeof(lx_protocol_header_t);
}

LifxPacket::~LifxPacket()
{
}

LifxPacket::LifxPacket(const LifxPacket& object)
{
    memcpy(&m_header, &(object.m_header), m_headerSize);
    m_tagged = object.m_tagged;
    m_ackRequired = object.m_ackRequired;
    m_resRequired = object.m_resRequired;
    m_type = object.m_type;
    m_source = object.m_source;
    m_port = object.m_port;
    memcpy(m_target, object.m_target, 8);
    m_protocol = object.m_protocol;
    m_size = object.m_size;
    m_addressable = object.m_addressable;
    m_headerSize = object.m_headerSize;
    m_address = object.m_address;
    m_payload = object.m_payload;
    m_datagram = object.m_datagram;
}


void LifxPacket::createHeader(LifxBulb* bulb, bool blankTarget)
{
    memset(&m_header, 0, m_headerSize);
    
    m_header.size = static_cast<uint16_t>(m_headerSize + m_datagram.size());
    m_header.protocol = PROTOCOL_NUMBER;
    m_header.addressable = ADDRESSABLE;
    m_header.tagged = m_tagged;
    m_header.origin = ORIGIN;
    m_header.source = m_source;
    // frame address
    if (!blankTarget)
        memcpy(m_header.target, bulb->target(), 8);
    else
        memset(m_header.target, 0, 8);
    m_header.ack_required = m_ackRequired;
    m_header.res_required = m_resRequired;
    // all set to 0
    // protocol header
    m_header.type = m_type;
    
    char *packet = static_cast<char*>(malloc(m_headerSize));
    memcpy(packet, static_cast<void*>(&m_header), m_headerSize);
    m_hdr.clear();
    m_hdr = QByteArray::fromRawData(packet, m_headerSize);
}

void LifxPacket::makeDiscoveryPacket()
{
    LifxBulb bulb;
    
    bulb.setService(1);
    bulb.setPort(BROADCAST_PORT);
    m_port = BROADCAST_PORT;
    
    m_tagged = 1;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_SERVICE;
    m_source = 919000;
    
    createHeader(&bulb);
}

void LifxPacket::makeDiscoveryPacketForBulb(QHostAddress address, int port)
{
    LifxBulb bulb;
    bulb.setService(1);
    bulb.setPort(port);
    bulb.setAddress(address);
    m_port = port;
    m_tagged = 1;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_SERVICE;
    m_source = 908082;

    createHeader(&bulb);
}

QByteArray LifxPacket::datagram()
{
    uint16_t size = m_headerSize + m_payload.size();
    m_hdr[0] = size << 0;
    m_hdr[1] = size << 8;
    m_datagram.clear();
    m_datagram.append(m_hdr);
    m_datagram.append(m_payload);
    
    return m_datagram;
}

void LifxPacket::setHeader(const char* data)
{
    memcpy(&m_header, data, m_headerSize);
    m_source = m_header.source;
    m_ackRequired = m_header.ack_required;
    m_resRequired = m_header.res_required;
    m_type = m_header.type;
    m_tagged = m_header.tagged;
    m_protocol = m_header.protocol;
    m_addressable = m_header.addressable;
    m_size = m_header.size;
    
    // m_target is the 6 byte MAC, leaving 2 bytes out
    // the full 8 byte field is stored in the raw header
    for (int i = 0; i < 8; i++) {
        m_target[i] = m_header.target[i];
    }
    m_hdr = QByteArray::fromRawData(data, m_headerSize);
}

void LifxPacket::setPayload(QByteArray ba)
{
    m_payload = ba;
}

void LifxPacket::setDatagram(QNetworkDatagram &datagram)
{
    m_address = datagram.senderAddress();
    m_port = datagram.senderPort();
    if (datagram.data().size() >= m_headerSize) {
        setHeader(datagram.data());
        if (datagram.data().size() > m_headerSize) {
            setPayload(datagram.data().mid(m_headerSize));
        }
    }
}

/**
 * Currently, this only can accept individual commands
 * Group requests will break everything
 */
void LifxPacket::setDatagram(char* data, int len, QHostAddress &addr, quint16 port)
{
    QByteArray pl(data, len);
    setHeader(data);
    if (len > m_headerSize) {
        setPayload(pl.mid(m_headerSize));
        m_address = addr;
        m_port = port;
    }
}

uint64_t LifxPacket::targetAsLong()
{
    uint64_t target;
    memcpy(&target, m_target, sizeof(uint64_t));
    return target;
}

void LifxPacket::echoBulb ( LifxBulb* bulb )
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::ECHO_REQUEST;
    m_source = 919882;
    
    createHeader(bulb, false);
    QBuffer buffer(&m_payload);
    buffer.open(QIODevice::WriteOnly);
    uint64_t num = bulb->echoRequest(true);
    buffer.write((char*)&num, sizeof(uint64_t));
}

void LifxPacket::getBulbFirmware(LifxBulb *bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_HOST_FIRMWARE;
    m_source = 919820;
    
    createHeader(bulb, false);
}

void LifxPacket::getBulbPower(LifxBulb *bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_POWER;
    m_source = 919821;
    
    createHeader(bulb, false);
}

void LifxPacket::getBulbLabel(LifxBulb* bulb)
{
    m_tagged = 1;
    m_ackRequired = 0;
    m_resRequired = 0;
    m_type = LIFX_DEFINES::GET_LABEL;
    m_source = 919822;
    
    createHeader(bulb);    
}

void LifxPacket::getBulbColor(LifxBulb* bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_COLOR;
    m_source = 919823;
    
    createHeader(bulb, false);
}

void LifxPacket::setBulbColor(LifxBulb* bulb)
{
    lx_dev_color_t *color = bulb->toDeviceColor();
    
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::SET_COLOR;
    m_source = 919824;
    
    createHeader(bulb, false);
    m_payload.clear();
    m_payload = QByteArray::fromRawData((char*)color, sizeof(lx_dev_color_t));
}

void LifxPacket::getBulbGroup(LifxBulb* bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_GROUP;
    m_source = 919825;
    
    createHeader(bulb, false);    
}

void LifxPacket::getBulbVersion(LifxBulb* bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_VERSION;
    m_source = 919826;
    
    createHeader(bulb, false);        
}

void LifxPacket::getWifiInfoForBulb(LifxBulb* bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::GET_WIFI_INFO;
    m_source = 918345;

    createHeader(bulb, false);
}

void LifxPacket::setBulbPower(LifxBulb* bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::SET_POWER;
    m_source = 919844;
    
    createHeader(bulb, false);
    if (bulb->power() == 0) {
        m_payload = QByteArrayLiteral("\x00\x00");
    }
    else {
        m_payload = QByteArrayLiteral("\xff\xff");
    }
}

void LifxPacket::rebootBulb(LifxBulb* bulb)
{
    m_tagged = 0;
    m_ackRequired = 0;
    m_resRequired = 1;
    m_type = LIFX_DEFINES::SET_REBOOT;
    m_source = 919828;
    
    createHeader(bulb, false);
}

/**
 * \fn QDebug operator<<(QDebug debug, const LifxBulb &bulb)
 * \brief Pretty print the LifxBulb object
 * 
 * For use with qDebug() only
 * 
 * Only prints the ipv4 address at this time
 */
QDebug operator<<(QDebug debug, LifxPacket &packet)
{
    QDebugStateSaver saver(debug);
    uint8_t *rawmac = packet.target();
    QString mac = QString("%1:%2:%3:%4:%5:%6")
                    .arg(rawmac[0], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[1], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[2], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[3], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[4], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[5], 2, 16, QLatin1Char('0'));
    QString reserved = QString("%1:%2:%3:%4:%5:%6")
                    .arg(packet.protocolHeader().reserved[0], 2, 16, QLatin1Char('0'))
                    .arg(packet.protocolHeader().reserved[1], 2, 16, QLatin1Char('0'))
                    .arg(packet.protocolHeader().reserved[2], 2, 16, QLatin1Char('0'))
                    .arg(packet.protocolHeader().reserved[3], 2, 16, QLatin1Char('0'))
                    .arg(packet.protocolHeader().reserved[4], 2, 16, QLatin1Char('0'))
                    .arg(packet.protocolHeader().reserved[5], 2, 16, QLatin1Char('0'));

    debug.nospace().noquote() << "Packet From: " << packet.address().toString() << Qt::endl;
    debug.nospace().noquote() << "Packet Raw: " << packet.header().toHex() << Qt::endl;
    debug.nospace().noquote() << "\tsize: " << packet.protocolHeader().size << Qt::endl;
    debug.nospace().noquote() << "\tprotocol: " << packet.protocolHeader().protocol << Qt::endl;
    debug.nospace().noquote() << "\taddressable: " << packet.protocolHeader().addressable << Qt::endl;
    debug.nospace().noquote() << "\ttagged: " << packet.protocolHeader().tagged << Qt::endl;
    debug.nospace().noquote() << "\torigin: " << packet.protocolHeader().origin << Qt::endl;
    debug.nospace().noquote() << "\tsource: " << packet.protocolHeader().source << Qt::endl;
    debug.nospace().noquote() << "\ttarget: " << mac << Qt::endl;
    debug.nospace().noquote() << "\treserved: " << reserved << Qt::endl;
    debug.nospace().noquote() << "\tresponse required: " << packet.protocolHeader().res_required << Qt::endl;
    debug.nospace().noquote() << "\tack required: " << packet.protocolHeader().ack_required << Qt::endl;

    if (packet.payload().size()) {
        debug.nospace().noquote() << "Payload: " << packet.payload().toHex() << Qt::endl;
    }
    return debug;
}

/**
 * \fn QDebug operator<<(QDebug debug, const LifxBulb *bulb)
 * \brief Pretty print the LifxBulb object
 * 
 * For use with qDebug() only
 * 
 * Only prints the ipv4 address at this time
 */
QDebug operator<<(QDebug debug, LifxPacket *packet)
{
    QDebugStateSaver saver(debug);
    uint8_t *rawmac = packet->target();
    QString mac = QString("%1:%2:%3:%4:%5:%6")
                    .arg(rawmac[0], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[1], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[2], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[3], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[4], 2, 16, QLatin1Char('0'))
                    .arg(rawmac[5], 2, 16, QLatin1Char('0'));
    QString reserved = QString("%1:%2:%3:%4:%5:%6")
                    .arg(packet->protocolHeader().reserved[0], 2, 16, QLatin1Char('0'))
                    .arg(packet->protocolHeader().reserved[1], 2, 16, QLatin1Char('0'))
                    .arg(packet->protocolHeader().reserved[2], 2, 16, QLatin1Char('0'))
                    .arg(packet->protocolHeader().reserved[3], 2, 16, QLatin1Char('0'))
                    .arg(packet->protocolHeader().reserved[4], 2, 16, QLatin1Char('0'))
                    .arg(packet->protocolHeader().reserved[5], 2, 16, QLatin1Char('0'));

    debug.nospace().noquote() << "Packet From: " << packet->address().toString() << Qt::endl;
    debug.nospace().noquote() << "Packet Raw: " << packet->header().toHex() << Qt::endl;
    debug.nospace().noquote() << "\tsize: " << packet->protocolHeader().size << Qt::endl;
    debug.nospace().noquote() << "\tprotocol: " << packet->protocolHeader().protocol << Qt::endl;
    debug.nospace().noquote() << "\taddressable: " << packet->protocolHeader().addressable << Qt::endl;
    debug.nospace().noquote() << "\ttagged: " << packet->protocolHeader().tagged << Qt::endl;
    debug.nospace().noquote() << "\torigin: " << packet->protocolHeader().origin << Qt::endl;
    debug.nospace().noquote() << "\tsource: " << packet->protocolHeader().source << Qt::endl;
    debug.nospace().noquote() << "\ttarget: " << mac << Qt::endl;
    debug.nospace().noquote() << "\treserved: " << reserved << Qt::endl;
    debug.nospace().noquote() << "\tresponse required: " << packet->protocolHeader().res_required << Qt::endl;
    debug.nospace().noquote() << "\tack required: " << packet->protocolHeader().ack_required << Qt::endl;
    if (packet->payload().size()) {
        debug.nospace().noquote() << "Payload: " << packet->payload().toHex() << Qt::endl;
    }
    return debug;
}
