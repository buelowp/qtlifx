// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>
#include "lifxbulb.h"
#include "lifxpacket.h"
#include "lifxgroup.h"
#include "lifxprotocol.h"

class AsyncHandler : public QObject
{
    Q_OBJECT

public:
    AsyncHandler(uint32_t id, bool debug, QObject *parent = nullptr);
    AsyncHandler(uint32_t id, bool debug, LifxBulb* bulb, QObject *parent = nullptr);
    AsyncHandler(uint32_t id, bool debug, QHostAddress address, int port, QObject *parent = nullptr);
    ~AsyncHandler();

    void setRetryCount(int count);
    void setMessageTimeout(int timeout);
    void setUniqueId(uint32_t id) { m_uniqueId = id; }
    uint32_t uniqueId() const { return m_uniqueId; }
    uint16_t type() const { return m_type; }
    LifxBulb* bulb() { return m_bulb; }
    int retryCount() const { return m_retryCount; }
    int timeoutValue() const { return m_messageTimeout; }
    bool stopped() const { return m_stopped; }
    int retries() const { return m_messageFailureCount; }
    void stop();

public slots:
    void messageResponse(LifxPacket *packet);
    void discoverBulbByAddress();
    void getPowerForBulb(LifxBulb *bulb, int source = 0);
    void getLabelForBulb(LifxBulb *bulb, int source = 0);
    void getVersionForBulb(LifxBulb *bulb, int source = 0);
    void getFirmwareForBulb(LifxBulb *bulb, int source = 0);
    void getColorForBulb(LifxBulb *bulb, int source = 0);
    void getGroupForBulb(LifxBulb *bulb, int source = 0);
    void getWifiInfoForBulb(LifxBulb *bulb, int source = 0);
    void setBulbColor(LifxBulb *bulb, int source = 0, bool ackRequired = false);
    void setBulbColor(LifxBulb *bulb, QColor color, int source = 0, bool ackRequired = false);
    void setBulbState(LifxBulb *bulb, bool state, int source = 0, bool ackRequired = false);
    void setGroupState(LifxGroup *group, bool state, int source = 0, bool ackRequired = false);
    void timeout();
    void sendDiscover();

signals:
    void newPacket(LifxPacket *packet);
    void messageTimeout(uint32_t uniqueId);
    void packetAck(uint32_t uniqueId);
    void complete(uint32_t uniqueId);

private:
    LifxProtocol *m_protocol;
    QHostAddress m_address;
    QTimer *m_messageStatusTimer;
    LifxBulb *m_bulb;
    int m_port;
    int m_messageFailureCount;
    int m_retryCount;
    int m_messageTimeout;
    bool m_stopped;
    uint32_t m_uniqueId;
    uint16_t m_type;
    bool m_debug;
};

QDebug operator<<(QDebug debug, AsyncHandler *handler);
