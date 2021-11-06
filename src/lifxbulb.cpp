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

#include "lifxbulb.h"

LifxBulb::LifxBulb()
{
    m_port = 0;
    m_major = 0;
    m_minor = 0;
    m_label = "Unknown";
    m_power = 0;
    m_deviceColor = (lx_dev_color_t*)malloc(sizeof(lx_dev_color_t));
    memset(m_deviceColor, 0, sizeof(lx_dev_color_t));
    memset(m_target, 0, 8);
}

void LifxBulb::setAddress(QHostAddress address, uint32_t port)
{
    m_address = address;
    m_port = port;
}

void LifxBulb::setPort(uint32_t port)
{
    m_port = port;
}

void LifxBulb::setService(uint8_t service)
{
    m_service = service;
}

void LifxBulb::setTarget(uint8_t *target)
{
    memcpy(m_target, target, 8);
}

void LifxBulb::setPower(uint16_t power)
{
    m_power = power;
}


QString LifxBulb::macToString() const
{
    return QString("%1:%2:%3:%4:%5:%6")
                    .arg(m_target[0], 2, 16)
                    .arg(m_target[1], 2, 16)
                    .arg(m_target[2], 2, 16)
                    .arg(m_target[3], 2, 16)
                    .arg(m_target[4], 2, 16)
                    .arg(m_target[5], 2, 16);
}

QString LifxBulb::addressToString() const
{
    bool conversionOK = false;
    QHostAddress temp(m_address.toIPv4Address(&conversionOK));
    return temp.toString();
}

bool LifxBulb::operator==(const LifxBulb &bulb)
{
    for (int i = 0; i < 8; i++) {
        if (m_target[i] != bulb.m_target[i])
            return false;
    }
        
    return true;
}

bool LifxBulb::operator==(const uint8_t* array)
{
    for (int i = 0; i < 8; i++) {
        if (m_target[i] != array[i])
            return false;
    }
        
    return true;
}

uint64_t LifxBulb::targetAsLong()
{
    uint64_t target;
    memcpy(&target, m_target, sizeof(uint64_t));
    return target;
}

void LifxBulb::setMajor(uint16_t major)
{
    m_major = major;
}

void LifxBulb::setMinor(uint16_t minor)
{
    m_minor = minor;
}

void LifxBulb::setLabel(QString label)
{
    m_label = label;
}

void LifxBulb::setDuration(uint32_t duration)
{
    m_deviceColor->duration = duration;
}


bool LifxBulb::isOn() const
{
    if (m_power)
        return true;
    
    return false;
}

void LifxBulb::setDevColor(lx_dev_lightstate_t* color)
{
    QString label((char*)color->label);
    qreal h = 0;
    qreal s = 0;
    qreal v = 0;

    if (color->hue > 0)
        h = ((qreal)color->hue / (qreal)std::numeric_limits<uint16_t>::max());
    if (color->saturation > 0)
        s = ((qreal)color->saturation / (qreal)std::numeric_limits<uint16_t>::max());
    if (color->brightness > 0)
        v = ((qreal)color->brightness / (qreal)std::numeric_limits<uint16_t>::max());
    
    m_deviceColor->brightness = color->brightness;
    m_deviceColor->hue = color->hue;
    m_deviceColor->saturation = color->saturation;
    m_deviceColor->kelvin = color->kelvin;
    m_power = color->power;
    m_color.setHsvF(h, s, v);
}

void LifxBulb::setColor(QColor &color)
{
    uint16_t h = color.hsvHueF() * std::numeric_limits<uint16_t>::max();
    uint16_t s = color.hsvSaturationF() * std::numeric_limits<uint16_t>::max();
    uint16_t v = color.valueF() * std::numeric_limits<uint16_t>::max();
    
    m_color = color;
    m_deviceColor->brightness = v;
    m_deviceColor->saturation = s;
    m_deviceColor->hue = h;
}

void LifxBulb::setKelvin(uint16_t kelvin)
{
    m_deviceColor->kelvin = kelvin;
}

lx_dev_color_t* LifxBulb::toDeviceColor() const
{
    return m_deviceColor;
}

QDebug operator<<(QDebug debug, const LifxBulb &bulb)
{
    float b = 0;
    float p = 0;
    
    if (bulb.brightness() > 0)
        b = ((qreal)bulb.brightness() / (qreal)std::numeric_limits<uint16_t>::max()) * 100;
    
    if (bulb.power() > 0)
        p = ((float)bulb.power() / (float)std::numeric_limits<uint16_t>::max()) * 100;

    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << bulb.label() << ": [" << bulb.macToString() << "]" << " " << bulb.addressToString() << ":" << bulb.port() << " Version: " << bulb.major() << "." << bulb.minor();
    if (bulb.isOn()) {
        debug.nospace().noquote()  << " Power: " << p << "% Brightness: " << b << "%";
    }
    else {
        debug.nospace().noquote()  << " OFF";
    }
    return debug;
}

QDebug operator<<(QDebug debug, const LifxBulb *bulb)
{
    float b = 0;
    float p = 0;
    
    if (bulb->brightness() > 0)
        b = ((float)bulb->brightness() / (float)std::numeric_limits<uint16_t>::max()) * 100;
    
    if (bulb->power() > 0)
        p = ((float)bulb->power() / (float)std::numeric_limits<uint16_t>::max()) * 100;
        
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << bulb->label() << ": [" << bulb->macToString() << "]" << " " << bulb->addressToString() << ":" << bulb->port() << " Version: " << bulb->major() << "." << bulb->minor();
    if (bulb->isOn()) {
        debug.nospace().noquote() << " Power: " << p << "%" << " Kelvin " << bulb->kelvin();
        debug.nospace().noquote() << " Color(" << bulb->toDeviceColor()->hue << "," <<  bulb->toDeviceColor()->saturation << "," << bulb->toDeviceColor()->brightness << ")";
    }
    else {
        debug.nospace().noquote()  << " OFF";
    }
    return debug;
}
