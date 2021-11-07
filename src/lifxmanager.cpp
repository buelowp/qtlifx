/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2021  <copyright holder> <email>
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

LifxManager::LifxManager(QObject *parent) : QObject(parent)
{
    m_protocol = new LifxProtocol();
    connect(m_protocol, &LifxProtocol::discoveryFailed, this, &LifxManager::discoveryFailed);
    connect(m_protocol, &LifxProtocol::newPacket, this, &LifxManager::newPacket);
}

LifxManager::~LifxManager()
{
}

void LifxManager::discoveryFailed()
{
    qDebug() << __PRETTY_FUNCTION__ << ": Timed out waiting for light discovery, retrying...";
    m_protocol->discover();
}

void LifxManager::initialize()
{
    m_protocol->initialize();
    m_protocol->discover();
}

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
                qDebug() << __PRETTY_FUNCTION__ << ": LABEL:" << bulb;
                emit newBulbAvailable(bulb->label(), target);
                m_protocol->getVersionForBulb(bulb);
            }
            else {
                qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_LABEL for a bulb (" << target << ") which isn't in the map";
            }
            break;
        case LIFX_DEFINES::STATE_HOST_FIRMWARE:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_firmware_t *firmware = (lx_dev_firmware_t*)packet->payload().data();
                bulb->setMajor(firmware->major);
                bulb->setMinor(firmware->minor);
                qDebug() << __PRETTY_FUNCTION__ << ": FIRMWARE:" << bulb;
                m_protocol->getColorForBulb(bulb);
            }
            else {
                qDebug() << __PRETTY_FUNCTION__ << ": Got a STATE_HOST_FIRMWARE for a bulb (" << target << ") which isn't in the map";
            }
            break;            
        case LIFX_DEFINES::LIGHT_STATE:
            if (m_bulbs.contains(target)) {
                bulb = m_bulbs[target];
                lx_dev_lightstate_t *color = (lx_dev_lightstate_t*)malloc(sizeof(lx_dev_lightstate_t));
                memcpy(color, packet->payload().data(), sizeof(lx_dev_lightstate_t)); 
                bulb->setDevColor(color);
                emit bulbStateChanged(bulb->label(), target);
                qDebug() << __PRETTY_FUNCTION__ << ": COLOR:" << bulb;
            }
            else {
                qWarning() << __PRETTY_FUNCTION__ << ": Got a LIGHT_STATE for a bulb (" << target << ") which isn't in the map";
            }
            break;            

        default:
//            qDebug() << __PRETTY_FUNCTION__ << ": Unknown type received" << packet->type();
            break;
    }
    
    delete packet;
}

LifxBulb * LifxManager::getBulbByMac(uint64_t target)
{
    if (m_bulbs.contains(target))
        return m_bulbs[target];
    
    return nullptr;
}

LifxBulb * LifxManager::getBulbByName(QString& name)
{
    if (m_bulbsByName.contains(name))
        return m_bulbsByName[name];
    
    return nullptr;
}

void LifxManager::changeBulbColor(QString& name, QColor color)
{
    if (m_bulbsByName.contains(name)) {
        LifxBulb *bulb = m_bulbsByName[name];
        bulb->setColor(color);
        m_protocol->setBulbColor(bulb);
    }
}

void LifxManager::changeBulbColor(uint64_t target, QColor color)
{
    if (m_bulbs.contains(target)) {
        LifxBulb *bulb = m_bulbs[target];
        bulb->setColor(color);
        m_protocol->setBulbColor(bulb);
    }
}

