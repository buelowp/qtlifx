#pragma once
/*
 * Common structures and defines for the library
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

/* \file defines.h
 * Provides the structures and const ints that allow us to talk
 * to a LIFX device
 */

/**
 * \def BROADCAST_PORT
 * \brief The default port we start with. 
 * 
 * Once bulbs start responding we will get the port for individual bulbs from the bulb itself.
 */
#define BROADCAST_PORT      56700

/**
 * \def PROTOCOL_NUMBER
 * \brief This is a defined value from LIFX and is used when communicating with a device
 */
#define PROTOCOL_NUMBER     1024

/**
 * \def LIFX_UDP_SERVICE
 * \brief This is a defined value from LIFX and is used when communicating with a device
 */
#define LIFX_UDP_SERVICE    1

/**
 * \def ADDRESSABLE
 * \brief This is a defined value from LIFX and is used when communicating with a device
 */
#define ADDRESSABLE         1

/**
 * \def ORIGIN
 * \brief This is a defined value from LIFX and is used when communicating with a device
 */
#define ORIGIN              0

#pragma pack(push, 1)

/**
 * \struct lx_protocol_header_t
 * \brief (PRIVATE) The header for all packets sent to/from a bulb.
 * 
 * This is the LIFX header and is used internally for communication. Parts may
 * be exposed by the LifxBulb class, but overall, this isn't available for use
 * by applications.
 */
typedef struct {
  /* frame */
  uint16_t size;                /**< Size of the entire packet */
  uint16_t protocol:12;         /**< Protocol, currently hardcoded to 1024 */
  uint8_t  addressable:1;       /**< Boolean indicating ? */
  uint8_t  tagged:1;            /**< Boolean indicating ? */
  uint8_t  origin:2;            /**< Origin flag */
  uint32_t source;              /**< 32bit number which is returned in kind, can be used to identify messages */
  uint8_t target[8];            /**< MAC address as an uint64_t */
  uint8_t  protoid[6];         /**< Reserved space, used by bulbs but not by this library */
  uint8_t  res_required:1;      /**< Indicates the bulb should be forced to respond to a message */
  uint8_t  ack_required:1;      /**< Indicates the bulb should be forced to ACK a message */
  uint8_t  reserved1:6;         /**< Reserved space, used by bulbs but not by this library */
  uint8_t  sequence;            /**< Sequence indicator */
  uint64_t reserved2;           /**< Reserved space, used by bulbs but not by this library */
  uint16_t type;                /**< Message type as defined in the constexpr below */
  uint16_t reserved3;           /**< Reserved space, used by bulbs but not by this library */
  /* variable length payload follows */
} lx_protocol_header_t;

/**
 * \struct lx_dev_service_t
 * \brief (PRIVATE) payload for a service state response from a bulb
 */
typedef struct {
    uint8_t service;        /**< Service type from the bulb */
    uint32_t port;          /**< Port type from the bulb */
} lx_dev_service_t;

/**
 * \struct lx_dev_lightstate_t
 * \brief (PRIVATE) Color response from a bulb
 * 
 * This is the state container returned from the bulb when GET_COLOR
 * is requested. This struct will be parsed and stored as the 
 * color structure, which is what is sent.
 * 
 * LIFX colors are HSBK, not HSV or RGB, so brightness here is V in HSV
 * and Kelvin is generally left at 3500 for all operations, but can be
 * changed.
 */
typedef struct {
    uint16_t hue;           /**< Hue value returned from the bulb */
    uint16_t saturation;    /**< Saturation value returned from the bulb */
    uint16_t brightness;    /**< Brightess value returned from the bulb */
    uint16_t kelvin;        /**< Kelvin color temperature returned from the bulb */
    uint8_t r1[2];          /**< Reserved bytes, not used by this library */
    uint16_t power;         /**< Power the bulb is currently operating at, either 65535 (on) or 0 (off) */
    uint8_t label[32];      /**< Bulb name */
    uint8_t r2[8];          /**< Reserved bytes, not used by this library */
} lx_dev_lightstate_t;

/**
 * \struct lx_dev_firmware_t
 * \brief (PRIVATE) The firmware version from a bulb
 * 
 * This is the state firmware payload returned from the bulb when
 * GET_HOST_FIRMWARE is requested.
 * 
 * The firmware is dotted decimal, where the major is a different number
 * for each bulb type, and the minor is the actual firmware revision.
 */
typedef struct {
    uint64_t build;         /**< Build time in millis */
    uint8_t reserved[8];    /**< Reserved bytes, not used by this library */
    uint16_t minor;         /**< Minor version number (x.yy where yy is minor) */
    uint16_t major;         /**< Major version number (x.yy where x is major) */
} lx_dev_firmware_t;

/**
 * \struct lx_dev_power_t
 * \brief (PRIVATE) Power state from a bulb
 * 
 * This is the state power payload returned from the bulb when GET_POWER is
 * requested. For all bulbs this library services, 65535 is ON while 0 is
 * off. There are no other values expected.
 */
typedef struct {
    uint16_t power;         /**< Power where 65535 is ON, 0 is OFF */
} lx_dev_power_t;

/**
 * \struct lx_group_info_t
 * \brief (PRIVATE) Group data as returned from a bulb
 * 
 * Container struct for the group info state message
 */
typedef struct {
    char group[16];         /**< Group UUID */
    char label[32];         /**< Group Name */
    uint64_t updated_at;    /**< Timestamp  */
} lx_group_info_t;

/**
 * \struct lx_dev_version_t
 * \brief (PRIVATE) VID and PID from the bulb
 * 
 * Container struct for the version state reply
 */
typedef struct {
    uint32_t vendor;        /**< Vendor ID (always 1 right now) */
    uint32_t product;       /**< Product ID, as defined in products.json */
    uint8_t reserverd[4];   /**< Reserved */
} lx_dev_version_t;

/**
 * \struct lx_dev_color_t
 * \brief (PRIVATE) The struct used to send color data to a bulb
 * 
 * There is a difference between color data coming from and going to
 * bulbs, so there are two structs. This one is what is populated
 * as the payload to send color data to a bulb
 */
typedef struct {
    uint8_t reserved;       /**< Reserved bytes, not used by this class */
    uint16_t hue;           /**< Hue value to be set on the light */
    uint16_t saturation;    /**< Saturation value to be set on the light */
    uint16_t brightness;    /**< Brightness value to be set on the light */
    uint16_t kelvin;        /**< Kelvin color temperature to be set on the light */
    uint32_t duration;      /**< Duration in millis for how long the next transition will take */
} lx_dev_color_t;

typedef struct {
    uint64_t value;
} lx_dev_echo_t;

#pragma pack(pop)

namespace LIFX_DEFINES {
    // Discovery
    static constexpr uint16_t GET_SERVICE = 2;
    static constexpr uint16_t STATE_SERVICE = 3;

    static constexpr uint16_t ACKNOWLEDGEMENT = 45;
    // Device Queries
    static constexpr uint16_t GET_HOST_FIRMWARE = 14;
    static constexpr uint16_t STATE_HOST_FIRMWARE = 15;
    static constexpr uint16_t GET_WIFI_INFO = 16;
    static constexpr uint16_t STATE_WIFI_INFO = 17;
    static constexpr uint16_t GET_DEV_WIFI_FIRMWARE = 18;
    static constexpr uint16_t STATE_WIFI_FIRMWARE = 19;
    static constexpr uint16_t GET_POWER = 20;
    static constexpr uint16_t SET_POWER = 21;
    static constexpr uint16_t STATE_POWER = 22;
    static constexpr uint16_t GET_LABEL = 23;
    static constexpr uint16_t SET_LABEL = 24;
    static constexpr uint16_t STATE_LABEL = 25;
    static constexpr uint16_t GET_VERSION = 32;
    static constexpr uint16_t STATE_VERSION = 33;
    static constexpr uint16_t GET_INFO = 34;
    static constexpr uint16_t STATE_INFO = 35;
    static constexpr uint16_t SET_REBOOT = 38;
    static constexpr uint16_t GET_LOCATION = 48;
    static constexpr uint16_t SET_LOCATION = 49;
    static constexpr uint16_t STATE_LOCATION = 50;
    static constexpr uint16_t GET_GROUP = 51;
    static constexpr uint16_t SET_GROUP = 52;
    static constexpr uint16_t STATE_GROUP = 53;
    static constexpr uint16_t ECHO_REQUEST = 58;
    static constexpr uint16_t ECHO_REPLY = 59;
    
    // LED Queries
    static constexpr uint16_t GET_COLOR = 101;
    static constexpr uint16_t SET_COLOR = 102;
    static constexpr uint16_t SET_WAVEFORM = 103;
    static constexpr uint16_t LIGHT_STATE = 107;
    static constexpr uint16_t GET_LIGHT_POWER = 116;
    static constexpr uint16_t SET_LIGHT_POWER = 117;
    static constexpr uint16_t STATE_LIGHT_POWER = 118;
    static constexpr uint16_t SET_WAVEFORM_OPTIONAL = 119;
    static constexpr uint16_t GET_IR = 120;
    static constexpr uint16_t SET_IR = 122;
    static constexpr uint16_t STATE_IR = 121;
    static constexpr uint16_t GET_HEV_CYCLE = 142;
    static constexpr uint16_t SET_HEV_CYCLE = 143;
    static constexpr uint16_t STATE_HEV_CYCLE = 144;
    static constexpr uint16_t GET_HEV_CYCLE_CONFIG = 145;
    static constexpr uint16_t SET_HEV_CYCLE_CONFIG = 146;
    static constexpr uint16_t STATE_HEV_CYCLE_CONFIG = 147;
    static constexpr uint16_t GET_LAST_HEV_CYCLE_RESULT = 148;
    static constexpr uint16_t STATE_LAST_HEV_CYCLE_RESULT = 149;
};

static QMap<int, QString> defines_names_map {
    { LIFX_DEFINES::GET_SERVICE, QString("GET_SERVICE") },
    { LIFX_DEFINES::STATE_SERVICE, QString("STATE_SERVICE") },
    { LIFX_DEFINES::ACKNOWLEDGEMENT, QString("ACKNOWLEDGEMENT") },
    { LIFX_DEFINES::GET_HOST_FIRMWARE, QString("GET_HOST_FIRMWARE") },
    { LIFX_DEFINES::STATE_HOST_FIRMWARE, QString("STATE_HOST_FIRMWARE") },
    { LIFX_DEFINES::GET_WIFI_INFO, QString("GET_WIFI_INFO") },
    { LIFX_DEFINES::STATE_WIFI_INFO, QString("STATE_WIFI_INFO") },
    { LIFX_DEFINES::GET_DEV_WIFI_FIRMWARE, QString("GET_DEV_WIFI_FIRMWARE") },
    { LIFX_DEFINES::STATE_WIFI_FIRMWARE, QString("STATE_WIFI_FIRMWARE") },
    { LIFX_DEFINES::GET_POWER, QString("GET_POWER") },
    { LIFX_DEFINES::SET_POWER, QString("SET_POWER") },
    { LIFX_DEFINES::STATE_POWER, QString("STATE_POWER") },
    { LIFX_DEFINES::GET_LABEL, QString("GET_LABEL") },
    { LIFX_DEFINES::SET_LABEL, QString("SET_LABEL") },
    { LIFX_DEFINES::STATE_LABEL, QString("STATE_LABEL") },
    { LIFX_DEFINES::STATE_VERSION, QString("STATE_VERSION") },
    { LIFX_DEFINES::GET_INFO, QString("GET_INFO") },
    { LIFX_DEFINES::STATE_INFO, QString("STATE_INFO") },
    { LIFX_DEFINES::SET_REBOOT, QString("SET_REBOOT") },
    { LIFX_DEFINES::GET_LOCATION, QString("GET_LOCATION") },
    { LIFX_DEFINES::SET_LOCATION, QString("SET_LOCATION") },
    { LIFX_DEFINES::STATE_LOCATION, QString("STATE_LOCATION") },
    { LIFX_DEFINES::GET_GROUP, QString("GET_GROUP") },
    { LIFX_DEFINES::SET_GROUP, QString("SET_GROUP") },
    { LIFX_DEFINES::STATE_GROUP, QString("STATE_GROUP") },
    { LIFX_DEFINES::ECHO_REQUEST, QString("ECHO_REQUEST") },
    { LIFX_DEFINES::ECHO_REPLY, QString("ECHO_REPLY") },
    { LIFX_DEFINES::GET_COLOR, QString("GET_COLOR") },
    { LIFX_DEFINES::SET_COLOR, QString("SET_COLOR") },
    { LIFX_DEFINES::SET_WAVEFORM, QString("SET_WAVEFORM") },
    { LIFX_DEFINES::LIGHT_STATE, QString("LIGHT_STATE") },
    { LIFX_DEFINES::GET_LIGHT_POWER, QString("GET_LIGHT_POWER") },
    { LIFX_DEFINES::SET_LIGHT_POWER, QString("SET_LIGHT_POWER") },
    { LIFX_DEFINES::STATE_LIGHT_POWER, QString("STATE_LIGHT_POWER") },
    { LIFX_DEFINES::SET_WAVEFORM_OPTIONAL, QString("SET_WAVEFORM_OPTIONAL") },
    { LIFX_DEFINES::GET_IR, QString("GET_IR") },
    { LIFX_DEFINES::STATE_IR, QString("STATE_IR") },
    { LIFX_DEFINES::GET_HEV_CYCLE, QString("GET_HEV_CYCLE") },
    { LIFX_DEFINES::SET_HEV_CYCLE, QString("SET_HEV_CYCLE") },
    { LIFX_DEFINES::STATE_HEV_CYCLE, QString("STATE_HEV_CYCLE") },
    { LIFX_DEFINES::GET_HEV_CYCLE_CONFIG, QString("GET_HEV_CYCLE_CONFIG") },
    { LIFX_DEFINES::SET_HEV_CYCLE_CONFIG, QString("SET_HEV_CYCLE_CONFIG") },
    { LIFX_DEFINES::STATE_HEV_CYCLE_CONFIG, QString("STATE_HEV_CYCLE_CONFIG") },
    { LIFX_DEFINES::GET_LAST_HEV_CYCLE_RESULT, QString("GET_LAST_HEV_CYCLE_RESULT") },
    { LIFX_DEFINES::STATE_LAST_HEV_CYCLE_RESULT, QString("STATE_LAST_HEV_CYCLE_RESULT") },
};
