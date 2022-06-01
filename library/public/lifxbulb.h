/*
 * Complete encapsulation of a single LIFX bulb
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

#ifndef LIFXBULB_H
#define LIFXBULB_H

#include <limits>

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>
#include <QtGui/QtGui>

#include "defines.h"
#include "lifxproduct.h"
#include "hsbk.h"

/**
 * \class LifxBulb
 * \brief (PUBLIC) Container/controller class for a LIFX bulb
 * 
 * This class is just a container. It keeps all the details about a bulb
 * that is returned from the physical device in one place. It is referenced
 * for every command that may be sent to the physical bulb. Updating data
 * here does not send that data to the physical bulb. That is done by the
 * manager directly.
 * 
 * LIFX defins colors to be HSBK, not HSV or RGB (or any other). This means
 * H = Hue
 * S = Saturation
 * B = Brightness (reasonably still value which is how this library interprets)
 * K = Kelvin (color temperature
 * 
 * The K value is generally retrieved from the bulb and not used otherwise as
 * it's not clear how to interpret it using Qt5. The higher the K value, the
 * bluer the color.
 * 
 * This library substitutes B with V in the QColor object and uses it directly
 * as a replacement. At this time, I can't find a difference, but it may
 * change if I find a need.
 * 
 * The color is maintained in two places.
 * 1. The QColor object so it can be used with a Gui
 * 2. The Lifx color struct which is what is used to send to the bulb
 * 
 * This library doesn't keep the LIGHT_STATE structure, but instead
 * copies the contents into the SET_COLOR structure and maintains that. 
 */
class LifxBulb
{
public:
    LifxBulb();
    ~LifxBulb() = default;
    
    bool operator==(const LifxBulb &bulb);
    bool operator==(const uint8_t*);
    
    bool isOn() const;

    void setAddress(QHostAddress address, uint32_t port);
    void setAddress(QHostAddress address);
    void setService(uint8_t service);
    void setPort(uint32_t port);
    void setTarget(uint8_t *target);
    void setMajor(uint16_t major);
    void setMinor(uint16_t minor);
    void setLabel(QString label);
    void setPower(uint16_t power);
    void setDevColor(lx_dev_lightstate_t *color);
    void setColor(QColor &color);
    void setColor(lx_dev_color_t &color);
    void setColor(HSBK &color);
    void setKelvin(uint16_t kelvin);
    void setDuration(uint32_t duration);
    void setGroup(QString group);
    void setPID(uint32_t pid);
    void setVID(uint32_t vid);
    void setProduct(QJsonObject &obj);
    void setDiscoveryActive(bool discovery);
    void setBrightness(uint16_t brightness);
    void setRSSI(float rssi);
    uint64_t echoRequest(bool generate);
    bool echoPending(bool state) { m_pendingEcho = state; return m_pendingEcho; }   //!< Set the flag that says we sent an echo request to the bulb
    bool echoPending() { return m_pendingEcho; }                                    //!< Get the flag indicating whether we are waiting for an echo
    
    QHostAddress& address() { return m_address; }   //!< Returns the IP address associated with this bulb
    uint8_t service() const { return m_service; }   //!< Returns the service which was set by STATE_SERVICE
    uint8_t* target() { return m_target; }          //!< Returns the MAC as an array of ints
    uint64_t targetAsLong();                        //!< Convienence function which turns the MAC into a number for indexing
    uint32_t port() const { return m_port; }        //!< Returns IP Port this bulb is listening to
    uint16_t major() const { return m_major; }      //!< Returns the major part of the version #
    uint16_t minor() const { return m_minor; }      //!< Returns the minor part of the version #
    QString label() const { return m_label; }       //!< Returns the bulb name
    uint16_t power() const { return m_power; }
    uint16_t brightness() const { return m_deviceColor->brightness; }
    uint16_t kelvin() const { return m_deviceColor->kelvin; }
    uint32_t duration() const { return m_deviceColor->duration; }
    QString group() const { return m_group; }
    uint32_t pid() const { return m_pid; }
    uint32_t vid() const { return m_vid; }
    bool inDiscovery() const { return m_inDiscovery; }
    QColor color() const { return m_color; }
    int rssi() const { return m_rssi; }

    QString macToString() const;
    QString addressToString(bool isIPV6) const;
    lx_dev_color_t* toDeviceColor() const;

private:
    QHostAddress m_address;         //!< The QHostAddress with the bulbs IP address
    QString m_label;                //!< The QString label returned as part of the STATE_LABEL reply
    QString m_group;                //!< The group that this bulb belongs to
    QColor m_color;                 //!< The color object this bulb is currently set to
    uint8_t m_service;              //!< The service value in the header
    uint32_t m_port;                //!< The IP port we are communicating with
    uint8_t m_target[8];            //!< The MAC, encoded as a UINT64, where bytes 7 and 8 are 0
    uint64_t m_build;               //!< The build timestamp from the bulb (not used in this library)
    uint16_t m_major;               //!< The major version
    uint16_t m_minor;               //!< The minor version
    uint16_t m_power;               //!< The current bulb power output (0 means OFF, all other values are considered ON)
    uint32_t m_vid;                 //!< The vendor ID the bulb, should generally always be 1
    uint32_t m_pid;                 //!< The product ID from the bulb, can be used to determine capabilities from the JSON
    lx_dev_color_t *m_deviceColor;  //!< The color structure as returned from the bulb
    LifxProduct *m_product;         //!< A container class with static product details from LIFX
    bool m_inDiscovery;             //!< Flag indicating whether the bulb has been completely discovered yet
    bool m_pendingEcho;             //!< An echo request for this bulb has been sent
    uint64_t m_echoSemaphore;       //!< This is the random value we will use to validate the echo did what we needed it to
    int m_rssi;                   //!< The returned RSSI value from the bulb. This converts from raw to a scale from 0 - 16
};

QDebug operator<<(QDebug debug, const LifxBulb &bulb);
QDebug operator<<(QDebug debug, const LifxBulb *bulb);
#endif // LIFXBULB_H
