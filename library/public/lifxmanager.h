/*
 * Manages interaction with LIFX bulbs
 * 
 * Copyright (C) 2021  Peter Buelow goballstate at gmail
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

#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <QtCore/QtCore>

#include "defines.h"
#include "lifxprotocol.h"
#include "lifxbulb.h"
#include "lifxpacket.h"
#include "lifxgroup.h"
#include "hsbk.h"

/**
 * \class LifxManager
 * \brief (PUBLIC) Primary interface to the LIFX bulbs
 * 
 * This is the brains, and does the heavy lifting. It creates the
 * comms socket, and owns starting discovery and communicating
 * with bulbs/groups/zones. All work done by an APP should use
 * the manager directly, and not attempt to work independently.
 * 
 * Note, there is no state management here. Because the API is
 * functionally stateless, it's possible to ask a bulb to do
 * something even if discovery is still going on. The first
 * signal indicating a bulb is available is not sent until
 * there is enough information to indicate that bulb can
 * do something (label, version, firmware).
 * 
 * There will be several signals sent over the course of
 * discovery, and once complete, a complete message will
 * be sent for each bulb. Note, there will be no overall complete
 * message as it's possible for bulbs to take longer to
 * respond, and as such, it's difficutl to determine if
 * discovery is complete without a timeout. No such timeout
 * will be done in the library, but it is possible to manage
 * state in the application.
 */
class Q_DECL_EXPORT LifxManager : public QObject
{
    Q_OBJECT

public:
    LifxManager(QObject *parent = nullptr);
    ~LifxManager();
    LifxManager(const LifxManager &object);
    
    void initialize();
    void discoverBulb(QHostAddress address, int port);
    LifxBulb* getBulbByName(QString &name);
    LifxBulb* getBulbByMac(uint64_t target);
    LifxGroup* getGroupByName(QString &name);
    LifxGroup* getGroupByUUID(QByteArray &uuid);
    QList<LifxBulb*> getBulbsByPID(int pid);
    void enableDebug(bool debug) { m_debug = debug; }
    void enableBulbEcho(QString &name, int timeout);
    void enableBulbEcho(uint64_t target, int timeout);
    void disableEcho(QString name);
    void disableEcho(uint64_t target);
    
public slots:
    void discoveryFailed();
    void newPacket(LifxPacket *packet);
    void changeBulbColor(uint64_t target, QColor color, uint32_t duration = 400);
    void changeBulbColor(LifxBulb* bulb, QColor color, uint32_t duration = 400);
    void changeBulbColor(uint64_t target, HSBK color, uint32_t duration = 400);
    void changeBulbColor(LifxBulb* bulb, HSBK color, uint32_t duration = 400);    
    void changeBulbBrightness(uint64_t target, uint16_t brightness);
    void changeBulbBrightness(LifxBulb* bulb, uint16_t brightness);
    void changeGroupColor(QByteArray &uuid, QColor color);
    void changeGroupColor(QByteArray &uuid, HSBK color);
    void changeGroupState(QByteArray &uuid, bool state);
    void setProductCapabilities(QJsonDocument &doc);
    void changeBulbState(uint64_t target, bool state);
    void changeBulbState(LifxBulb* bulb, bool state);
    void rebootBulb(LifxBulb *bulb);
    void rebootBulb(uint64_t target);
    void rebootGroup(QByteArray &uuid);
    void updateState();

signals:
    void bulbDiscoveryFinished(LifxBulb *bulb);
    void newBulbAvailable(QString, uint64_t);
    void bulbStateChange(LifxBulb *bulb);
    void newGroupFound(QString, QByteArray);
    void echoReply(LifxBulb *bulb);
    void bulbLabelChange(LifxBulb *bulb);
    void bulbGroupChange(LifxGroup *group);
    void bulbPowerChange(LifxBulb *bulb);
    void bulbRSSIChange(LifxBulb *bulb);

private:
    void echoFunction(LifxBulb *bulb, int timeout);
    
    LifxProtocol *m_protocol;
    QMap<uint64_t, LifxBulb*> m_bulbs;
    QMap<QByteArray, LifxGroup*> m_groups;
    QMultiMap<int, LifxBulb*> m_bulbsByPID;
    QMap<int, QJsonObject> m_productObjects;
    QMap<uint64_t, QTimer*> m_echoTimers;
    bool m_debug;
};

Q_DECLARE_METATYPE(LifxManager);
#endif // LIGHTMANAGER_H
