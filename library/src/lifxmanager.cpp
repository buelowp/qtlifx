/*
 * Manages interaction with LIFX bulbs
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

#include "lifxmanager.h"

LifxManager::LifxManager(QObject *parent) : QObject(parent), m_debug(false)
{
    m_protocol = new LifxProtocol();
    connect(m_protocol, &LifxProtocol::discoveryFailed, this, &LifxManager::discoveryFailed);
    connect(m_protocol, &LifxProtocol::newPacket, this, &LifxManager::newPacket);
    QByteArray debug = qgetenv("LIFX_DEBUG");
    if (debug[0] == '1') {
        qDebug() << __PRETTY_FUNCTION__ << ": LIFX debug enabled";
        m_debug = true;
    }
}

LifxManager::LifxManager(const LifxManager& object) : QObject()
{
    m_protocol = object.m_protocol;
    m_bulbs = object.m_bulbs;
    m_groups = object.m_groups;
    m_bulbsByPID = object.m_bulbsByPID;
    m_productObjects = object.m_productObjects;
}

LifxManager::~LifxManager()
{
}

/**
 * \fn void LifxManager::discoveryFailed()
 * \brief SLOT that is called when the discovery operation times out
 * 
 * This will be called if a timer is set by the Manager to limit
 * how long we wait for discovery. This is better handled by the
 * application
 */
void LifxManager::discoveryFailed()
{
    if (m_debug)
        qWarning() << __PRETTY_FUNCTION__ << ": Timed out waiting for light discovery, retrying...";

    emit bulbDiscoveryFailed();
}

/**
 * \fn void LifxManager::discover()
 * \brief SLOT which is called when the app wants to start talking to LIFX bulbs
 */
void LifxManager::discover()
{
    m_protocol->discover();
}

/**
 * \fn void LifxManager::updateState(LifxBulb* bulb)
 * \param bulb Pointer to the LifxBulb we are operating on
 */
void LifxManager::updateState(LifxBulb* bulb)
{
    if (bulb) {
        qDebug() << __PRETTY_FUNCTION__ << ": Updating state for" << bulb->label();
        getColorForBulb(bulb);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": Called with a null bulb pointer";
    }
}

/**
 * \fn void LifxManager::updateState(uint64_t target)
 * \param target The uin64_t MAC of the bulb we are looking for
 *
 * This is an overloaded function which will call updateState(bulb)
 */
void LifxManager::updateState(uint64_t target)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        updateState(bulb);
    }
}

/**
 * \fn void LifxManager::updateState()
 *
 * This is an overloaded function which will call updateState(bulb). It
 * will iterate the entire bulb list and update each one.
 */
void LifxManager::updateState()
{
    QMapIterator<uint64_t, LifxBulb*> i(m_bulbs);
    while (i.hasNext()) {
        i.next();
        updateState(i.value());
    }    
}

void LifxManager::discoverBulb(QHostAddress address, int port)
{
//    discoverBulbByAddress(address, port);
}

/**
 * \fn void LifxManager::newPacket(LifxPacket* packet)
 * \param packet Pointer to a LifxPacket container
 * \brief SLOT called when a new packet arrives from the protocol manager
 * 
 * This handles the state messages and decodes the packets by stuffing them into
 * a bulb container and emitting signals to indicate something happened.
 */
void LifxManager::newPacket(LifxPacket *packet)
{
    LifxBulb *bulb;
    uint64_t target = packet->targetAsLong();
    
    switch (packet->type()) {
        case LIFX_DEFINES::STATE_SERVICE:
            if (!m_bulbs.contains(target)) {
                bulb = new LifxBulb();
                lx_dev_service_t *service = (lx_dev_service_t*)packet->payload().data();
                bulb->setAddress(packet->address(), service->port);
                bulb->setService(service->service);
                bulb->setTarget(packet->target());
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": SERVICE:" << bulb;
                m_bulbs[target] = bulb;
                m_protocol->getLabelForBulb(bulb);
            }
            else {
                bulb = m_bulbs[target];
                bulb->setAddress(packet->address(), packet->port());
                emit bulbStateChange(bulb);
            }
            break;
        case LIFX_DEFINES::STATE_LABEL:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                bulb->setLabel(QString::fromUtf8(packet->payload()));
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": LABEL:" << bulb;
                if (bulb->inDiscovery()) {
                    m_protocol->getFirmwareForBulb(bulb);
                }
                else
                    emit bulbLabelChange(bulb);
            }
            else {
                if (m_debug)
                    qWarning() << __PRETTY_FUNCTION__ << ": Got a STATE_LABEL for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_HOST_FIRMWARE:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_firmware_t *firmware = (lx_dev_firmware_t*)packet->payload().data();
                bulb->setMajor(firmware->major);
                bulb->setMinor(firmware->minor);
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": FIRMWARE:" << bulb;
                if (bulb->inDiscovery()) {
                    m_protocol->getWifiInfoForBulb(bulb);
                }
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_HOST_FIRMWARE for a bulb (" << target << ") which isn't in the map";
            }
            break;            
        case LIFX_DEFINES::STATE_VERSION:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_version_t *version = (lx_dev_version_t*)packet->payload().data();
                bulb->setVID(version->vendor);
                bulb->setPID(version->product);
                if (m_productObjects.size() && m_productObjects.contains(version->product)) {
                    bulb->setProduct(m_productObjects[version->product]);
                }
                m_bulbsByPID.insert(version->product, bulb);
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": VERSION:" << bulb;

                if (bulb->inDiscovery()) {
                    m_protocol->getGroupForBulb(bulb);
                }
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_VERSION for a bulb (" << target << ") which isn't in the map";
            }
            break;            
        case LIFX_DEFINES::STATE_GROUP:
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
                            emit bulbGroupChange(g);
                        }
                        if (m_debug)
                            qDebug() << __PRETTY_FUNCTION__ << ": GROUP (add):" << g;
                    }
                }
                else {
                    LifxGroup *g = new LifxGroup(label, uuid, group->updated_at);
                    g->addBulb(bulb);
                    m_groups[uuid] = g;
                    emit newGroupFound(label, uuid);
                    emit bulbGroupChange(g);
                    if (m_debug)
                        qDebug() << __PRETTY_FUNCTION__ << ": GROUP (new):" << g;
                }
                if (bulb->inDiscovery()) {
                    m_protocol->getColorForBulb(bulb);
                }
            }
            break;
        case LIFX_DEFINES::LIGHT_STATE:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_lightstate_t *color = (lx_dev_lightstate_t*)packet->payload().data();
                bulb->setDevColor(color);
                if (bulb->inDiscovery()) {
                    bulb->setDiscoveryActive(false);
                    emit bulbDiscoveryFinished(bulb);
                }
                else
                    emit bulbStateChange(bulb);
                
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": COLOR:" << bulb;
            }
            else {
                qWarning() << __PRETTY_FUNCTION__ << ": Got a LIGHT_STATE for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_POWER:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                uint16_t power = 0;
                memcpy(&power, packet->payload().data(), 2);
                qDebug() << __PRETTY_FUNCTION__ << ": Power has changed to" << power;
                
                bulb->setPower(power);
                if (!bulb->inDiscovery())
                    emit bulbPowerChange(bulb);

                if (m_debug)
                    qDebug().nospace() << "POWER: bulb returned " << packet->payload() << " [" << power << "]" << " from bulb " << bulb->label();
            }
            break;
        case LIFX_DEFINES::ECHO_REPLY:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                bulb->setAddress(packet->address());
                lx_dev_echo_t *echo = (lx_dev_echo_t*)packet->payload().data();
                if (echo->value == bulb->echoRequest(false)) {
                    bulb->echoPending(false);
                    emit echoReply(bulb, QByteArray());
                }
                else {
                    qDebug() << "Got an echo response value of " << echo->value << ", but was expecting" << bulb->echoRequest(false);
                }
            }
            else {
                qWarning() << "Got an echo reply to a bulb we dno't seem to know about [" << target << "]";
            }
            break;
        case LIFX_DEFINES::STATE_WIFI_INFO:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                float signal = 0;
                memcpy(&signal, packet->payload().data(), 4);
                bulb->setRSSI(signal);
                if (!bulb->inDiscovery())
                    emit bulbRSSIChange(bulb);
                else {
                    m_protocol->getVersionForBulb(bulb);
                }
            }
            break;
        case LIFX_DEFINES::ACKNOWLEDGEMENT:
            qDebug() << __PRETTY_FUNCTION__ << ": Acknowledgment sent from" << packet->address().toString() << ":" << packet;
            break;
        default:
            if (packet->type() != 2) {
                qWarning() << __PRETTY_FUNCTION__ << ": Unknown packet type" << packet->type();
                qWarning() << packet;
            }
            break;
    }
    
    delete packet;
}

/**
 * \fn LifxBulb * LifxManager::getBulbByMac(uint64_t target)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \return Returns the bulb if a bulb with target MAC is found, nullptr if no bulb exists
 */
LifxBulb * LifxManager::getBulbByMac(uint64_t target)
{
    if (m_bulbs.contains(target))
        return m_bulbs[target];
    
    return nullptr;
}

/**
 * \fn LifxBulb * LifxManager::getBulbByName(QString& name)
 * \param name QString name of the bulb being queried
 * \return Retuns the bulb if a bulb with name is found, nullptr if no bulb exists
 */
LifxBulb * LifxManager::getBulbByName(QString& name)
{
    QMapIterator<uint64_t, LifxBulb*> i(m_bulbs);
    while (i.hasNext()) {
        i.next();
        if (i.value()->label() == name)
            return i.value();
    }

    return nullptr;
}

/**
 * \fn void LifxManager::changeBulbColor(uint64_t target, QColor color, uint32_t duration)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \param color QColor object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(uint64_t target, QColor color, uint32_t duration, int source, bool ackRequired)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb, source, ackRequired);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": bulb for target" << target << "not found in bulbs map";
    }
}

/**
 * \fn void LifxManager::changeBulbColor(LifxBulb *bulb, QColor color, uint32_t duration)
 * \param bulb Pointer to LifxBulb object
 * \param color QColor object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(LifxBulb *bulb, QColor color, uint32_t duration, int source, bool ackRequired)
{
    if (bulb) {
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb, source, ackRequired);
    }
}

/**
 * \fn void LifxManager::changeBulbColor(uint64_t target, QColor color, uint32_t duration)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \param color QColor object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(uint64_t target, HSBK color, uint32_t duration, int source, bool ackRequired)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb, source, ackRequired);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": bulb for target" << target << "not found in bulbs map";
    }
}

/**
 * \fn void LifxManager::changeBulbColor(LifxBulb *bulb, HSBK color, uint32_t duration)
 * \param bulb Pointer to LifxBulb object
 * \param color HSBK object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(LifxBulb *bulb, HSBK color, uint32_t duration, int source, bool ackRequired)
{
    if (bulb) {
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb, source, ackRequired);
    }
}

/**
 * \fn void LifxManager::changeBulbBrightness(uint64_t target, uint16_t brightness)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \param brightness New brightness to set the bulb to
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbBrightness(uint64_t target, uint16_t brightness, int source, bool ackRequired)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setBrightness(brightness);
        m_protocol->setBulbColor(bulb, source, ackRequired);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": bulb for target" << target << "not found in bulbs map";
    }
}

/**
 * \fn void LifxManager::changeBulbBrightness(LifxBulb *bulb, uint16_t brightness)
 * \param bulb Pointer to LifxBulb object
 * \param brightness New brightness to set the bulb to
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbBrightness(LifxBulb *bulb, uint16_t brightness, int source, bool ackRequired)
{
    if (bulb) {
        bulb->setBrightness(brightness);
        m_protocol->setBulbColor(bulb, source, ackRequired);
    }
}

/**
 * \fn void LifxManager::changeGroupColor(QByteArray& uuid, QColor color)
 * \param uuid The group UUID to query for bulbs
 * \param color The QColor color to assign to the group
 * \brief Sets the color of all bulbs in the group identified by uuid to color
 */
void LifxManager::changeGroupColor(QByteArray& uuid, QColor color, int source, bool ackRequired)
{
    if (m_groups.contains(uuid)) {
        LifxGroup *group = m_groups[uuid];
        QVector<LifxBulb*> bulbs = group->bulbs();
        for (auto bulb : bulbs) {
            changeBulbColor(bulb->targetAsLong(), color, source, ackRequired);
        }
    }
}

/**
 * \fn void LifxManager::changeGroupColor(QByteArray& uuid, HSBK color)
 * \param uuid The group UUID to query for bulbs
 * \param color The HSBK color to assign to the group
 * \brief Sets the color of all bulbs in the group identified by uuid to color
 */
void LifxManager::changeGroupColor(QByteArray& uuid, HSBK color, int source, bool ackRequired)
{
    if (m_groups.contains(uuid)) {
        LifxGroup *group = m_groups[uuid];
        QVector<LifxBulb*> bulbs = group->bulbs();
        for (auto bulb : bulbs) {
            changeBulbColor(bulb->targetAsLong(), color, source, ackRequired);
        }
    }
}

/**
 * \fn LifxGroup * LifxManager::getGroupByName(QString& name)
 * \param name Name of the group to retrieve
 * \return Returns a pointer to a LifxGroup object if name exists, nullptr otherwise
 * \brief Will look through the group map and if name exists, return that group container
 */
LifxGroup * LifxManager::getGroupByName(QString& name)
{
    QMapIterator<QByteArray, LifxGroup*> i(m_groups);
    
    while (i.hasNext()) {
        i.next();
        LifxGroup *group = i.value();
        if (group->label() == name) {
            return group;
        }
    }
    
    return nullptr;
}

/**
 * \fn void LifxManager::getColorForBulb(LifxBulb *bulb, int source)
 * \param bulb The LifxBulb object we are working on
 * \param source An option source field to help identify the byte stream messages
 *
 * This asks the manager to go get the color value from the bulb
 */
void LifxManager::getColorForBulb(LifxBulb *bulb, int source)
{
    if (bulb) {
        m_protocol->getColorForBulb(bulb, source);
    }
}

/**
 * \fn void LifxManager::getColorForBulb(LifxBulb *bulb, int source)
 * \param target The mac address for the bulb object we are working on
 * \param source An option source field to help identify the byte stream messages
 *
 * This asks the manager to go get the color value from the bulb
 */
void LifxManager::getColorForBulb(uint64_t target, int source)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        m_protocol->getColorForBulb(bulb, source);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": bulb for target" << target << "not found in bulbs map";
    }
}

/**
 * \fn LifxGroup * LifxManager::getGroupByUUID(QByteArray& uuid)
 * \param uuid UUID of the group to retrieve
 * \return Returns a pointer to a LifxGroup object if name exists, nullptr otherwise
 * \brief Will look through the group map and if uuid exists, return that group container
 */
LifxGroup * LifxManager::getGroupByUUID(QByteArray& uuid)
{
    if (m_groups.contains(uuid)) {
        return m_groups[uuid];
    }
    
    return nullptr;
}

/**
 * \fn void LifxManager::changeGroupState(QByteArray& uuid, bool state)
 * \param uuid UUID of the group to retrieve
 * \param state Sets the bulbs in a group to ON/OFF based on state TRUE/FALSE
 * \brief Finds the group by uuid and attempts to either turn on or off each bulb
 */
void LifxManager::changeGroupState(QByteArray& uuid, bool state, int source, bool ackRequired)
{
    if (m_groups.contains(uuid)) {
        LifxGroup *group = m_groups[uuid];
        m_protocol->setGroupState(group, state, source, ackRequired);
    }
}

/**
 * \fn QList<LifxBulb *> LifxManager::getBulbsByPID(int pid)
 * \param pid Bulb product ID
 * \return Returns a list of bulbs by PID, may be an empty set of PID isn't valid
 * \brief Get a list of bulbs that have the same product ID
 */
QList<LifxBulb *> LifxManager::getBulbsByPID(int pid)
{
    return m_bulbsByPID.values(pid);
}

/**
 * \fn void LifxManager::setProductCapabilities(QJsonDocument& doc)
 * \param doc Valid QJsonDocument
 * \brief Parses the products.json file
 * 
 * This function takes a reference to a valid and validated QJsonDocument object.
 * If this is not true, this function will fail. It will then parse
 * the file either storing each object because bulb discovery hasn't started
 * or assigning the correct PID content to a bulb because it's being done after.
 */
void LifxManager::setProductCapabilities(QJsonDocument& doc)
{
    QJsonArray parent = doc.array();
    for (int i = 0; i < parent.size(); i++) {
        if (parent[i].isObject()) {
            QJsonObject vendor = parent[i].toObject();
            if (vendor.contains("products")) {
                QJsonArray products = vendor["products"].toArray();
                for (int i = 0; i < products.size(); i++) {
                    if (products[i].isObject()) {
                        QJsonObject obj = products[i].toObject();
                        int pid = obj["pid"].toInt();
                        
                        if (m_bulbs.size() == 0) {
                            m_productObjects.insert(pid, obj);
                        }
                        else {
                            if (m_bulbsByPID.contains(pid)) {
                                QList<LifxBulb*> bulbs = m_bulbsByPID.values(pid);
                                for (auto bulb : bulbs) {
                                    bulb->setProduct(obj);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * \fn void LifxManager::changeBulbState(LifxBulb* bulb, bool state, int source, bool ackRequired)
 * \param bulb LifBulb object pointer
 * \param state Boolean state for the bulb true = on, false = off
 * \param source Optional source identification for this message
 * \param ackRequired Optional ackRequired for this setter, defaults to false or no
 */
void LifxManager::changeBulbState(LifxBulb* bulb, bool state, int source, bool ackRequired)
{
    if (bulb != nullptr) {
        if (m_debug)
            qDebug() << __PRETTY_FUNCTION__ << ": Setting" << bulb->label() << "to" << state;

        m_protocol->setBulbState(bulb, state, source, ackRequired);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": state change requested on a NULL bulb";
    }
}

void LifxManager::changeBulbState(uint64_t target, bool state, int source, bool ackRequired)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        changeBulbState(bulb, state, source, ackRequired);
    }
}

void LifxManager::rebootBulb(LifxBulb* bulb)
{
    if (bulb != nullptr)
        m_protocol->rebootBulb(bulb);
}

void LifxManager::rebootBulb(uint64_t target)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        rebootBulb(bulb);
    }
}

void LifxManager::rebootGroup(QByteArray& uuid)
{
    if (m_groups.contains(uuid)) {
        LifxGroup *group = m_groups[uuid];
        QVector<LifxBulb*> bulbs = group->bulbs();
        
        for (auto bulb : bulbs) {
            rebootBulb(bulb);
        }
    }
}

void LifxManager::enableBulbEcho(QString& name, int timeout, QByteArray echoing)
{
    if (timeout >= 1000) {
        LifxBulb *bulb = getBulbByName(name);
        if (bulb) {
            echoFunction(bulb, timeout, echoing);
        }
    }
}

void LifxManager::enableBulbEcho(uint64_t target, int timeout, QByteArray echoing)
{
    if (timeout >= 1000) {
        LifxBulb *bulb = getBulbByMac(target);
        if (bulb)
            echoFunction(bulb, timeout, echoing);
    }
}


void LifxManager::echoFunction(LifxBulb* bulb, int timeout, QByteArray echoing)
{
    QTimer *timer = new QTimer();
    m_echoTimers.insert(bulb->targetAsLong(), timer);
    connect(timer, &QTimer::timeout, [this,bulb]() {
        if (bulb->echoPending()) {
            qDebug() << "Bulb " << bulb->label() << " did not respond to it's last echo request";
        }
        m_protocol->echoRequest(bulb, QByteArray());
        bulb->echoPending(true);
    });
    timer->setInterval(timeout);
    timer->start();
}

void LifxManager::disableEcho ( QString name )
{
    LifxBulb *bulb = getBulbByName(name);
    if (bulb) {
        if (m_echoTimers.contains(bulb->targetAsLong())) {
            m_echoTimers[bulb->targetAsLong()]->stop();
            m_echoTimers[bulb->targetAsLong()]->deleteLater();
        }
    }
}

void LifxManager::disableEcho ( uint64_t target )
{
    if (m_echoTimers.contains(target)) {
        m_echoTimers[target]->stop();
        m_echoTimers[target]->deleteLater();
    }
}
