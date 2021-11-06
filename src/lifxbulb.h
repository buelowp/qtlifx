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

#ifndef LIFXBULB_H
#define LIFXBULB_H

#include <limits>

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>
#include <QtGui/QtGui>

#include "defines.h"

class LifxBulb
{
public:
    LifxBulb();
    ~LifxBulb() = default;
    
    LifxBulb(const LifxBulb &) = default;
    LifxBulb &operator=(const LifxBulb &) = default;
    bool operator==(const LifxBulb &bulb);
    bool operator==(const uint8_t*);
    
    bool isOn() const;

    void setAddress(QHostAddress address, uint32_t port);
    void setService(uint8_t service);
    void setPort(uint32_t port);
    void setTarget(uint8_t *target);
    void setMajor(uint16_t major);
    void setMinor(uint16_t minor);
    void setLabel(QString label);
    void setPower(uint16_t power);
    void setDevColor(lx_dev_lightstate_t *color);
    void setColor(QColor color);
    void setKelvin(uint16_t kelvin);
    
    QHostAddress& address() { return m_address; }
    uint8_t service() const { return m_service; }
    uint8_t* target() { return m_target; }
    uint64_t targetAsLong();
    uint32_t port() const { return m_port; }
    uint16_t major() const { return m_major; }
    uint16_t minor() const { return m_minor; }
    QString label() const { return m_label; }
    uint16_t power() const { return m_power; }
    uint16_t brightness() const { return m_deviceColor->brightness; }
    uint16_t kelvin() const { return m_deviceColor->kelvin; }
    uint32_t duration() const { return m_deviceColor->duration; }
    
    QString macToString() const;
    QString addressToString() const;
    lx_dev_color_t* toDeviceColor();

private:
    QHostAddress m_address;
    QString m_label;
    QColor m_color;
    uint8_t m_service;
    uint32_t m_port;
    uint8_t m_target[8];
    uint64_t m_build;
    uint16_t m_major;
    uint16_t m_minor;
    uint16_t m_power;
    lx_dev_color_t *m_deviceColor;
};

QDebug operator<<(QDebug debug, const LifxBulb &bulb);
QDebug operator<<(QDebug debug, const LifxBulb *bulb);
#endif // LIFXBULB_H
