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
    m_vid = 0;
    m_pid = 0;
    m_inDiscovery = true;
    m_deviceColor = (lx_dev_color_t*)malloc(sizeof(lx_dev_color_t));
    memset(m_deviceColor, 0, sizeof(lx_dev_color_t));
    memset(m_target, 0, 8);
}

/**
 * \fn void LifxBulb::setDiscoveryActive(bool discovery)
 * \param discovery bool indicating state of discovery operation for this bulb
 * \brief True if we are in discovery, false if the bulb is ready to use
 */
void LifxBulb::setDiscoveryActive(bool discovery)
{
    m_inDiscovery = discovery;
}

/**
 * \fn void LifxBulb::setAddress(QHostAddress address, uint32_t port)
 * \param address The QHostAddress container with the IP that the bulb sent data from
 * \param port The Port encoded in the STATE_SERVICE reply
 * 
 * This will encode the IP address we use when communicating with this
 * bulb individually.
 */
void LifxBulb::setAddress(QHostAddress address, uint32_t port)
{
    m_address = address;
    m_port = port;
}

/**
 * \fn void LifxBulb::setPort(uint32_t port)
 * \param port The IP port value used when communicating with this device
 * 
 * The port will generally always be 57600, but this value is set as part
 * of the reply from the bulb in the STATE_SERVICE message, and is not
 * assumed to be the default port.
 */
void LifxBulb::setPort(uint32_t port)
{
    m_port = port;
}

/**
 * \fn void LifxBulb::setService(uint8_t service)
 * \param service The service value as defined by the LIFX protocol
 */
void LifxBulb::setService(uint8_t service)
{
    m_service = service;
}

/**
 * \fn void LifxBulb::setTarget(uint8_t *target)
 * \param target Pointer to an 8 byte array containing the MAC as the first 6 bytes
 * 
 * The LIFX protocol uses 8 bytes for a MAC for whatever reason (uint64_t) and
 * encodes the MAC values as 6 2 byte parts of an 8 byte array. There is a
 * distinction in this library between the MAC and the target. They are used
 * somewhat interchangeably, but whenever communicating with the bulb (send/recv),
 * the full 8 bytes are used. Whenever doing comparisons or providing human readable
 * content, only the first 6 bytes are used.
 */
void LifxBulb::setTarget(uint8_t *target)
{
    memcpy(m_target, target, 8);
}

/**
 * \fn void LifxBulb::setPower(uint16_t power)
 * \param power The 2 byte power value to be sent to the bulb
 * 
 * This is the power value to be used when sending an update to the bulb.
 */
void LifxBulb::setPower(uint16_t power)
{
    m_power = power;
}

/**
 * \fn QString LifxBulb::macToString() const
 * \return Returns the MAC address as a human readable string
 * 
 * This is just debug formatting, and does not zero pad just yet
 */
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

/**
 * \fn QString LifxBulb::addressToString() const
 * \param isIPV6 True if the returned address should include IPV6 content
 * \return Returns a QString format of the IPV4/6 address
 * 
 * This will return a printable version of the QHostAddress for debug purposes
 */
QString LifxBulb::addressToString(bool isIPV6) const
{
    if (!isIPV6) {
        bool conversionOK = false;
        QHostAddress temp(m_address.toIPv4Address(&conversionOK));
        return temp.toString();
    }
    return m_address.toString();
}

/**
 * \fn bool LifxBulb::operator==(const LifxBulb &bulb)
 * \param bulb A reference to the bulb being compared
 * \return returns True if the bulbs are considered equal
 * \brief compares the internals of two light bulbs
 * 
 * This will compare the MAC and the variable contents
 * of a light bulb to determine if they are the same
 * object.
 */
bool LifxBulb::operator==(const LifxBulb &bulb)
{
    bool rval;
    
    for (int i = 0; i < 8; i++) {
        if (m_target[i] != bulb.m_target[i])
            return false;
    }
    
    rval &= (m_address == bulb.m_address);
    rval &= (m_label == bulb.m_label);
    rval &= (m_color == bulb.m_color);
    rval &= (m_service == bulb.m_service);
    rval &= (m_port == bulb.m_port);
    rval &= (m_major == bulb.m_major);
    rval &= (m_minor == bulb.m_minor);
    rval &= (m_power == bulb.m_power);
    rval &= (m_build == bulb.m_build);
    return rval;
}

/**
 * \fn bool LifxBulb::operator==(const uint8_t* array)
 * \return Returns True if the MAC addresses are equal
 * \param array Pointer to a 6 byte mac address array
 * \brief Checks to see if two classes are equal if they MAC is equal
 * 
 * This will ONLY compare the MAC addresses, and will consider the
 * classes to be equal IFF the MAC values are the same. This does
 * not prove the classes were constructed the same way.
 */
bool LifxBulb::operator==(const uint8_t* array)
{
    for (int i = 0; i < 6; i++) {
        if (m_target[i] != array[i])
            return false;
    }
        
    return true;
}

/**
 * \fn uint64_t LifxBulb::targetAsLong()
 * \return Returns the 64bit numeric representation of the mac address field
 * 
 * This number is effectively a UUID for the bulb and will be unique to each
 * bulb. It is used internally as a QMap key along with the bulb name
 * as we may not have a bulb name at every point in the process. The bulbs
 * should be handled by name by consumers of this API.
 */
uint64_t LifxBulb::targetAsLong()
{
    uint64_t target;
    memcpy(&target, m_target, sizeof(uint64_t));
    return target;
}

/**
 * \fn void LifxBulb::setMajor(uint16_t major)
 * \param major 16bit major version number as returned from the physical bulb
 */
void LifxBulb::setMajor(uint16_t major)
{
    m_major = major;
}

/**
 * \fn void LifxBulb::setMinor(uint16_t minor)
 * \param minor 16bit minor version number as returned from the physical bulb
 */
void LifxBulb::setMinor(uint16_t minor)
{
    m_minor = minor;
}

/**
 * \fn void LifxBulb::setLabel(QString label)
 * \param label QString container for the name of this bulb as returned from the physical bulb
 */
void LifxBulb::setLabel(QString label)
{
    m_label = label;
}

/**
 * \fn void LifxBulb::setDuration(uint32_t duration)
 * \param duration A 32bit time value used to set a duration on the bulb
 * \brief Sets a duration for set functions which require it
 */
void LifxBulb::setDuration(uint32_t duration)
{
    m_deviceColor->duration = duration;
}

/**
 * \fn bool LifxBulb::isOn() const
 * \returns True if the bulb is on, False otherwise
 * \brief If the bulb reports a power value, it's considered ON.
 */
bool LifxBulb::isOn() const
{
    if (m_power)
        return true;
    
    return false;
}

/**
 * \fn void LifxBulb::setDevColor(lx_dev_lightstate_t* color)
 * \param color A pointer to a lightstate struct
 * 
 * This works a bit different than translating directly from a QColor. It will
 * store the original value into the m_deviceColor member variable, but it will
 * also set the m_color QColor object to the reasonable approximation values. The
 * reasonable approx values are determined by finding the percentage of max for
 * each value and using that as the qreal value which is then set into the QColor.
 */
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

/**
 * \fn void LifxBulb::setColor(QColor &color)
 * \param color A reference to a QColor which contains the color we want to show
 * 
 * This will translate the QColor values which are qreal between 0 and 1 as a percentage
 * into a uint16_t where the value is the percentage of the max uint16_t range. Lifx
 * bulbs use brightness, or HSBK (Hue, Saturation, Brightness, Kelvin) for setting
 * a color. This doesn't map so well to other color containers, so I do it the following way.
 * The 16 bit color is changed to a percentage by multiplying the qreal by max (45000 * .15%)
 * and then store that value into bulb color struct to be used when a command is sent.
 */
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

/**
 * \fn void LifxBulb::setKelvin(uint16_t kelvin)
 * \param kelvin A 2 byte number which represents the kelvin value for the color being shown (higher is bluer)
 * \brief Sets the kelvin value this bulb will use when a new command is sent
 */
void LifxBulb::setKelvin(uint16_t kelvin)
{
    m_deviceColor->kelvin = kelvin;
}

/**
 * \fn lx_dev_color_t* LifxBulb::toDeviceColor() const
 * \brief Returns the device color struct to the caller
 * \return Returns lx_dev_color_t pointer
 */
lx_dev_color_t* LifxBulb::toDeviceColor() const
{
    return m_deviceColor;
}

/**
 * \fn void LifxBulb::addGroupName(QString group)
 * \param group A QString with a valid group name as returned by the groups get message
 * \brief Add a group name to the bulb for reference
 */
void LifxBulb::setGroup(QString group)
{
    m_group = group;
}

/**
 * \fn void LifxBulb::setPID(uint32_t pid)
 * \param pid Product ID for this bulb
 * 
 * Sets the PID
 */
void LifxBulb::setPID(uint32_t pid)
{
    m_pid = pid;
}

/**
 * \fn void LifxBulb::setVID(uint32_t pid)
 * \param pid Product ID for this bulb
 * 
 * Sets the VID
 */
void LifxBulb::setVID(uint32_t vid)
{
    m_vid = vid;
}

void LifxBulb::setProduct(QJsonObject& obj)
{
    if (obj.contains("pid")) {
        if (obj["pid"].toInt() == m_pid) {
            LifxProduct *product = new LifxProduct();
            if (obj.contains("name")) {
                product->setName(obj["name"].toString());
                if (obj.contains("features") && obj["features"].isObject()) {
                    QJsonObject features = obj["features"].toObject();
                    
                    if (features.contains("color"))
                        product->setColor(features["color"].toBool());
                    
                    if (features.contains("chain"))
                        product->setChain(features["chain"].toBool());
                    
                    if (features.contains("matrix"))
                        product->setMatrix(features["matrix"].toBool());
                    
                    if (features.contains("infrared"))
                        product->setIR(features["infrared"].toBool());
                    
                    if (features.contains("multizone"))
                        product->setMultiZone(features["multizone"].toBool());
                    
                    if (features.contains("temperature_range") && features["temperature_range"].isArray()) {
                        QJsonArray array = features["temperature_range"].toArray();
                        if (array.size() > 0) {
                            product->setRange(array[0].toInt(), array[1].toInt());
                        }
                    }
                }
            }
            if (obj.contains("upgrades") && obj["upgrades"].isArray()) {
                QJsonArray upgrades = obj["upgrades"].toArray();
                for (int i = 0; i < upgrades.size(); ++i) {
                    QJsonObject upgrade = upgrades[i].toObject();
                    int major = upgrade["major"].toInt();
                    int minor = upgrade["minor"].toInt();
                    QJsonObject features = upgrade["features"].toObject();
                    if (features.contains("temperature_range") && features["temperature_range"].isArray()) {
                        QJsonArray array = features["temperature_range"].toArray();
                        if (array.size() > 0) {
                            if (m_major >= major && m_minor >= minor)
                                product->setRange(array[0].toInt(), array[1].toInt());
                        }
                    }
                }
            }
            m_product = product;
            qDebug() << __PRETTY_FUNCTION__ << ":" << product;
        }
    }
}


/**
 * \fn QDebug operator<<(QDebug debug, const LifxBulb &bulb)
 * \brief Pretty print the LifxBulb object
 * 
 * For use with qDebug() only
 * 
 * Only prints the ipv4 address at this time
 */
QDebug operator<<(QDebug debug, const LifxBulb &bulb)
{
    float b = 0;
    float p = 0;
    
    if (bulb.brightness() > 0)
        b = ((qreal)bulb.brightness() / (qreal)std::numeric_limits<uint16_t>::max()) * 100;
    
    if (bulb.power() > 0)
        p = ((float)bulb.power() / (float)std::numeric_limits<uint16_t>::max()) * 100;

    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << bulb.label() << ": [" << bulb.macToString() << "] (" << bulb.group() << ") (" << bulb.vid() << "," << bulb.pid() <<") " << bulb.addressToString(false) << ":" << bulb.port() << " Version: " << bulb.major() << "." << bulb.minor();
    if (bulb.isOn()) {
        debug.nospace().noquote()  << " Power: " << p << "% Brightness: " << b << "%";
    }
    else {
        debug.nospace().noquote()  << " OFF";
    }
    return debug;
}

/**
 * \fn QDebug operator<<(QDebug debug, const LifxBulb *bulb)
 * \brief Pretty print the LifxBulb object
 * 
 * For use with qDebug() only
 * 
 * Only prints the ipv4 address at this time
 */
QDebug operator<<(QDebug debug, const LifxBulb *bulb)
{
    float b = 0;
    float p = 0;
    
    if (bulb->brightness() > 0)
        b = ((float)bulb->brightness() / (float)std::numeric_limits<uint16_t>::max()) * 100;
    
    if (bulb->power() > 0)
        p = ((float)bulb->power() / (float)std::numeric_limits<uint16_t>::max()) * 100;
        
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << bulb->label() << ": [" << bulb->macToString() << "] (" << bulb->group() << ") (" << bulb->vid() << "," << bulb->pid() <<") " << bulb->addressToString(false) << ":" << bulb->port() << " Version: " << bulb->major() << "." << bulb->minor();
    if (bulb->isOn()) {
        debug.nospace().noquote() << " Power: " << p << "%" << " Kelvin " << bulb->kelvin();
        debug.nospace().noquote() << " Color(" << bulb->toDeviceColor()->hue << "," <<  bulb->toDeviceColor()->saturation << "," << bulb->toDeviceColor()->brightness << ")";
    }
    else {
        debug.nospace().noquote()  << " OFF";
    }
    return debug;
}
