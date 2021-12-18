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
}

LifxManager::LifxManager(const LifxManager& object) : QObject()
{
    m_protocol = object.m_protocol;
    m_bulbs = object.m_bulbs;
    m_bulbsByName =  object.m_bulbsByName;
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
        qDebug() << __PRETTY_FUNCTION__ << ": Timed out waiting for light discovery, retrying...";
    m_protocol->discover();
}

/**
 * \fn void LifxManager::initialize()
 * \brief SLOT which is called when the app wants to start talking to LIFX bulbs
 */
void LifxManager::initialize()
{
    m_protocol->initialize();
    m_protocol->discover();
}

/**
 * \fn void LifxManager::newPacket(LifxPacket* packet)
 * \param packet Pointer to a LifxPacket container
 * \brief SLOT called when a new packet arrives from the protocol manager
 * 
 * This handles the state messages and decodes the packets by stuffing them into
 * a bulb container and emitting signals to indicate something happened.
 */
void LifxManager::newPacket(LifxPacket* packet)
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
            break;
        case LIFX_DEFINES::STATE_LABEL:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                bulb->setLabel(QString::fromUtf8(packet->payload()));
                m_bulbsByName[bulb->label()] = bulb;
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": LABEL:" << bulb;
                emit newBulbAvailable(bulb->label(), target);
                m_protocol->getFirmwareForBulb(bulb);
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_LABEL for a bulb (" << target << ") which isn't in the map";
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
                m_protocol->getVersionForBulb(bulb);
            }
            else {
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_HOST_FIRMWARE for a bulb (" << target << ") which isn't in the map";
            }
            break;            
        case LIFX_DEFINES::STATE_VERSION:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_version_t *version = (lx_dev_version_t*)malloc(sizeof(lx_dev_version_t));
                memcpy(version, packet->payload().data(), sizeof(lx_dev_version_t)); 
                bulb->setVID(version->vendor);
                bulb->setPID(version->product);
                if (m_productObjects.size() && m_productObjects.contains(version->product)) {
                    bulb->setProduct(m_productObjects[version->product]);
                }
                m_bulbsByPID.insert(version->product, bulb);
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": VERSION:" << bulb;
                m_protocol->getGroupForBulb(bulb);
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
                lx_group_info_t *group = (lx_group_info_t*)malloc(sizeof(lx_group_info_t));
                memcpy(group, packet->payload().data(), sizeof(lx_group_info_t));
                label = QString(group->label);
                uuid = QByteArray::fromRawData(group->group, 16);
                bulb->setGroup(group->label);
                if (m_groups.contains(uuid)) {
                    LifxGroup *g = m_groups[uuid];
                    if (g != nullptr) {
                        if (!g->contains(bulb))
                            g->addBulb(bulb);
                            if (m_debug)
                                qDebug() << __PRETTY_FUNCTION__ << ": GROUP (add):" << g;
                    }
                }
                else {
                    LifxGroup *g = new LifxGroup(label, uuid, group->updated_at);
                    g->addBulb(bulb);
                    m_groups[uuid] = g;
                    emit newGroupFound(label, uuid);
                    if (m_debug)
                        qDebug() << __PRETTY_FUNCTION__ << ": GROUP (new):" << g;
                }
                m_protocol->getColorForBulb(bulb);
            }
            break;
        case LIFX_DEFINES::LIGHT_STATE:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_lightstate_t *color = (lx_dev_lightstate_t*)malloc(sizeof(lx_dev_lightstate_t));
                memcpy(color, packet->payload().data(), sizeof(lx_dev_lightstate_t)); 
                bulb->setDevColor(color);
                if (bulb->inDiscovery()) {
                    bulb->setDiscoveryActive(false);
                    emit bulbDiscoveryFinished(bulb);
                }
                emit bulbStateChanged(bulb->label(), target);
                if (m_debug)
                    qDebug() << __PRETTY_FUNCTION__ << ": COLOR:" << bulb;
            }
            else {
                qWarning() << __PRETTY_FUNCTION__ << ": Got a LIGHT_STATE for a bulb (" << target << ") which isn't in the map";
            }
            break;            
        default:
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
    if (m_bulbsByName.contains(name))
        return m_bulbsByName[name];
    
    return nullptr;
}

/**
 * \fn void LifxManager::changeBulbColor(QString& name, QColor color, uint32_t duration)
 * \param name QString name of the bulb being changed
 * \param color New color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(QString& name, QColor color, uint32_t duration)
{
    if (m_bulbsByName.contains(name)) {
        LifxBulb *bulb = m_bulbsByName[name];
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbColor(uint64_t target, QColor color, uint32_t duration)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \param color QColor object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(uint64_t target, QColor color, uint32_t duration)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbColor(LifxBulb *bulb, QColor color, uint32_t duration)
 * \param bulb Pointer to LifxBulb object
 * \param color QColor object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(LifxBulb *bulb, QColor color, uint32_t duration)
{
    if (bulb) {
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbColor(QString& name, QColor color, uint32_t duration)
 * \param name QString name of the bulb being changed
 * \param color New color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(QString& name, HSBK color, uint32_t duration)
{
    if (m_bulbsByName.contains(name)) {
        LifxBulb *bulb = m_bulbsByName[name];
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbColor(uint64_t target, QColor color, uint32_t duration)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \param color QColor object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(uint64_t target, HSBK color, uint32_t duration)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbColor(LifxBulb *bulb, HSBK color, uint32_t duration)
 * \param bulb Pointer to LifxBulb object
 * \param color HSBK object containing the new color to set the bulb to
 * \param duration The uint32_t value in millis to slow the transition down
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbColor(LifxBulb *bulb, HSBK color, uint32_t duration)
{
    if (bulb) {
        bulb->setColor(color);
        bulb->setDuration(duration);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbBrightness(QString& name, uint16_t brightness)
 * \param name QString name of the bulb being changed
 * \param brightness New brightness to set the bulb to
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbBrightness(QString& name, uint16_t brightness)
{
    if (m_bulbsByName.contains(name)) {
        LifxBulb *bulb = m_bulbsByName[name];
        bulb->setBrightness(brightness);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbBrightness(uint64_t target, uint16_t brightness)
 * \param target 64bit integer which has an encoded version of the MAC address
 * \param brightness New brightness to set the bulb to
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbBrightness(uint64_t target, uint16_t brightness)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setBrightness(brightness);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeBulbBrightness(LifxBulb *bulb, uint16_t brightness)
 * \param bulb Pointer to LifxBulb object
 * \param brightness New brightness to set the bulb to
 * \brief Sets the color of a single bulb to color
 */
void LifxManager::changeBulbBrightness(LifxBulb *bulb, uint16_t brightness)
{
    if (bulb) {
        bulb->setBrightness(brightness);
        m_protocol->setBulbColor(bulb);
    }
}

/**
 * \fn void LifxManager::changeGroupColor(QByteArray& uuid, QColor color)
 * \param uuid The group UUID to query for bulbs
 * \param color The QColor color to assign to the group
 * \brief Sets the color of all bulbs in the group identified by uuid to color
 */
void LifxManager::changeGroupColor(QByteArray& uuid, QColor color)
{
    if (m_groups.contains(uuid)) {
        LifxGroup *group = m_groups[uuid];
        QVector<LifxBulb*> bulbs = group->bulbs();
        for (auto bulb : bulbs) {
            changeBulbColor(bulb->targetAsLong(), color);
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
void LifxManager::changeGroupState(QByteArray& uuid, bool state)
{
    if (m_groups.contains(uuid)) {
        LifxGroup *group = m_groups[uuid];
        m_protocol->setGroupState(group, state);
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
                                QList<LifxBulb*> bulbs = m_bulbs.values(pid);
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

void LifxManager::changeBulbState(LifxBulb* bulb, bool state)
{
    if (m_debug)
        qDebug() << __PRETTY_FUNCTION__ << ": Setting" << bulb->label() << "to" << state;
    if (bulb != nullptr)
        m_protocol->setBulbState(bulb, state);
}

void LifxManager::changeBulbState(QString& name, bool state)
{
    if (m_bulbsByName.contains(name)) {
        LifxBulb *bulb = m_bulbsByName[name];
        changeBulbState(bulb, state);
    }
}

void LifxManager::changeBulbState(uint64_t target, bool state)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        changeBulbState(bulb, state);
    }
}

void LifxManager::rebootBulb(LifxBulb* bulb)
{
    if (bulb != nullptr)
        m_protocol->rebootBulb(bulb);
}

void LifxManager::rebootBulb(QString& name)
{
    if (m_bulbsByName.contains(name)) {
        LifxBulb *bulb = m_bulbsByName[name];
        rebootBulb(bulb);
    }    
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
