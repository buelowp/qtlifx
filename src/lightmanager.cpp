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

#include "lightmanager.h"

LightManager::LightManager(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer();
    m_protocol = new LifxProtocol();
    connect(m_protocol, &LifxProtocol::discoveryFailed, this, &LightManager::discoveryFailed);
    connect(m_protocol, &LifxProtocol::newPacket, this, &LightManager::newPacket);
    connect(m_timer, &QTimer::timeout, this, &LightManager::timeout);
}

LightManager::~LightManager()
{
}

void LightManager::discoveryFailed()
{
    qDebug() << __PRETTY_FUNCTION__ << ": Timed out waiting for light discovery, retrying...";
    m_protocol->discover();
}

void LightManager::initialize()
{
    m_protocol->initialize();
    m_protocol->discover();
}

void LightManager::timeout()
{
    static bool next = false;
    QColor c;
    if (next)
        c.setNamedColor(QString("green"));
    else
        c.setNamedColor(QString("blue"));
    
    LifxBulb *bulb = m_bulbsByName["Office West"];
    if (bulb) {
        bulb->setColor(c);
        bulb->setDuration(500);
        m_protocol->setBulbColor(bulb);
    }
    next = !next;
}


void LightManager::newPacket(LifxPacket* packet)
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
                qDebug() << __PRETTY_FUNCTION__ << ": COLOR:" << bulb;
            }
            else {
                qWarning() << __PRETTY_FUNCTION__ << ": Got a LIGHT_STATE for a bulb (" << target << ") which isn't in the map";
            }
            m_timer->start(5000);
            break;            

        default:
//            qDebug() << __PRETTY_FUNCTION__ << ": Unknown type received" << packet->type();
            break;
    }
    
    delete packet;
}


