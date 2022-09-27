// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: Apache-2.0

#include "asynchandler.h"

AsyncHandler::AsyncHandler(uint32_t id, bool debug, QObject *parent) : QObject(parent), m_messageFailureCount(0), m_retryCount(10), m_messageTimeout(1000), m_debug(debug)
{
    m_messageStatusTimer = new QTimer(this);
    m_messageStatusTimer->setInterval(m_messageTimeout);
    connect(m_messageStatusTimer, &QTimer::timeout, this, &AsyncHandler::timeout);
    m_protocol = new LifxProtocol(this);
    connect(m_protocol, &LifxProtocol::newPacket, this, &AsyncHandler::messageResponse);
    m_stopped = true;
    m_type = 0;
    m_bulb = nullptr;
    m_uniqueId = id;
}

AsyncHandler::AsyncHandler(uint32_t id, bool debug, LifxBulb *bulb, QObject *parent) : QObject(parent), m_messageFailureCount(0), m_retryCount(10), m_messageTimeout(1000), m_debug(debug)
{
    m_messageStatusTimer = new QTimer(this);
    m_messageStatusTimer->setInterval(m_messageTimeout);
    connect(m_messageStatusTimer, &QTimer::timeout, this, &AsyncHandler::timeout);
    m_protocol = new LifxProtocol(this);
    connect(m_protocol, &LifxProtocol::newPacket, this, &AsyncHandler::messageResponse);
    m_stopped = true;
    m_type = 0;
    m_bulb = bulb;
    m_uniqueId = id;
}

AsyncHandler::AsyncHandler(uint32_t id, bool debug, QHostAddress address, int port, QObject *parent) : QObject(parent), m_address(address), m_port(port), m_messageFailureCount(0), m_retryCount(15), m_messageTimeout(1500), m_debug(debug)
{
    m_messageStatusTimer = new QTimer(this);
    m_protocol = new LifxProtocol(this);
    connect(m_protocol, &LifxProtocol::newPacket, this, &AsyncHandler::messageResponse);
    m_stopped = true;
    m_type = 0;
    m_bulb = nullptr;
    m_uniqueId = id;
}

AsyncHandler::~AsyncHandler()
{
    if (m_debug)
        qDebug() << __PRETTY_FUNCTION__ << ": Cleaning up handler id" << m_uniqueId;
    m_messageStatusTimer->stop();
    m_messageStatusTimer->deleteLater();
}

void AsyncHandler::sendDiscover()
{
    m_protocol->discover();
    m_type = 2;
}

void AsyncHandler::messageResponse(LifxPacket* packet)
{
    if (packet->isValid()) {
        if (m_debug)
            qDebug() << __PRETTY_FUNCTION__ << ": Packet received from" << packet->address() << "with type" << packet->type() << "to handler id" << m_uniqueId;

        m_messageStatusTimer->stop();
        if (m_messageFailureCount > 0 && m_debug) {
            qDebug() << __PRETTY_FUNCTION__ << ": Retried this message" << m_messageFailureCount << "times";
        }

        if (packet->type() == 45) {
            emit packetAck(m_uniqueId);
        }
        else {
            emit newPacket(packet);
        }
        m_stopped = true;
        emit complete(m_uniqueId);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": PACKET ERROR:" << packet;
    }
}

void AsyncHandler::setRetryCount(int count)
{
    if (count > 0)
        m_retryCount = count;
}

void AsyncHandler::setMessageTimeout(int timeout)
{
    if (timeout > 500) {
        m_messageTimeout = timeout;
    }
}

void AsyncHandler::timeout()
{
    m_messageFailureCount++;
    qDebug() << __PRETTY_FUNCTION__ << ":" << m_messageFailureCount;
    if (m_messageFailureCount == m_retryCount) {
        qWarning() << __PRETTY_FUNCTION__ << ": No response from message handler ID" << m_uniqueId;
        m_messageStatusTimer->stop();
        emit messageTimeout(m_uniqueId);
        emit complete(m_uniqueId);
    }
}

void AsyncHandler::discoverBulbByAddress()
{
    m_type = m_protocol->discoverBulbByAddress(m_address, m_port);
    m_messageStatusTimer->start();
}

void AsyncHandler::getColorForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getColorForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::getFirmwareForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getFirmwareForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::getGroupForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getGroupForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::getLabelForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getLabelForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::getPowerForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getPowerForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::getVersionForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getVersionForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::getWifiInfoForBulb(LifxBulb* bulb, int source)
{
    m_type = m_protocol->getWifiInfoForBulb(bulb, source);
    m_messageStatusTimer->start();
}

void AsyncHandler::setBulbColor(LifxBulb* bulb, int source, bool ackRequired)
{
    m_type = m_protocol->setBulbColor(bulb, source, ackRequired);
    m_messageStatusTimer->start();
}

void AsyncHandler::setBulbColor(LifxBulb* bulb, QColor color, int source, bool ackRequired)
{
    m_type = m_protocol->setBulbColor(bulb, color, source, ackRequired);
    m_messageStatusTimer->start();
}

void AsyncHandler::setBulbState(LifxBulb* bulb, bool state, int source, bool ackRequired)
{
    m_type = m_protocol->setBulbState(bulb, state, source, ackRequired);
    m_messageStatusTimer->start();
}

void AsyncHandler::setGroupState(LifxGroup* group, bool state, int source, bool ackRequired)
{
    m_protocol->setGroupState(group, state, source, ackRequired);
    m_type = LIFX_DEFINES::SET_GROUP;
    m_messageStatusTimer->start();
}

void AsyncHandler::stop()
{
    m_messageStatusTimer->stop();
    m_stopped = true;
}


QDebug operator<<(QDebug debug, AsyncHandler *handler)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "HANDLER Contents" << Qt::endl;
    debug.nospace().noquote() << "\tUnique ID: " << handler->uniqueId() << Qt::endl;
    debug.nospace().noquote() << "\tMessage Type: " << handler->type() << Qt::endl;
    debug.nospace().noquote() << "\tRetry Attempts: " << handler->retryCount() << Qt::endl;
    debug.nospace().noquote() << "\tTimeout Value: " << handler->timeoutValue() << Qt::endl;
    debug.nospace().noquote() << "\tRetries Attempted: " << handler->retries() << Qt::endl;
    debug.nospace().noquote() << "\tCurrent State: " << handler->stopped();
    return debug;
}
