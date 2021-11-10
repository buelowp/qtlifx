#pragma once

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
typedef struct {
  /* frame */
  uint16_t size;
  uint16_t protocol:12;
  uint8_t  addressable:1;
  uint8_t  tagged:1;
  uint8_t  origin:2;
  uint32_t source;
  /* frame address */
  uint8_t target[8];
  uint8_t  reserved[6];
  uint8_t  res_required:1;
  uint8_t  ack_required:1;
  uint8_t  :6;
  uint8_t  sequence;
  /* protocol header */
  //uint64_t :64;
  uint64_t reserved1;
  uint16_t type;
  //uint16_t :16;
  uint16_t reserved2;
  /* variable length payload follows */
} lx_protocol_header_t;

typedef struct {
    uint8_t service;
    uint32_t port;
} lx_dev_service_t;

typedef struct {
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
    uint8_t r1[2];
    uint16_t power;
    uint8_t label[32];
    uint8_t r2[8]; 
} lx_dev_lightstate_t;

typedef struct {
    uint64_t build;
    uint8_t reserved[8];
    uint16_t minor;
    uint16_t major;
} lx_dev_firmware_t;

typedef struct {
    uint16_t power;
} lx_dev_power_t;

/**
 * Container struct for the group info state message
 */
typedef struct {
    char group[16];         /**< Group UUID */
    char label[32];         /**< Group Name */
    uint64_t updated_at;    /**< Timestamp  */
} lx_group_info_t;

/**
 * Container struct for the version state reply
 */
typedef struct {
    uint32_t vendor;        /**< VID */
    uint32_t product;       /**< PID */
    uint8_t reserverd[4];   /**< Reserved */
} lx_dev_version_t;

typedef struct {
    uint8_t reserved;
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
    uint32_t duration;
} lx_dev_color_t;
#pragma pack(pop)

namespace LIFX_DEFINES {
    // Discovery
    static constexpr int GET_SERVICE = 2;
    static constexpr int STATE_SERVICE = 3;

    // Device Queries
    static constexpr int GET_HOST_FIRMWARE = 14;
    static constexpr int STATE_HOST_FIRMWARE = 15;
    static constexpr int GET_WIFI_INFO = 16;
    static constexpr int STATE_WIFI_INFO = 17;
    static constexpr int GET_DEV_WIFI_FIRMWARE = 18;
    static constexpr int STATE_WIFI_FIRMWARE = 19;
    static constexpr int GET_POWER = 20;
    static constexpr int SET_POWER = 21;
    static constexpr int STATE_POWER = 22;
    static constexpr int GET_LABEL = 23;
    static constexpr int SET_LABEL = 24;
    static constexpr int STATE_LABEL = 25;
    static constexpr int GET_VERSION = 32;
    static constexpr int STATE_VERSION = 33;
    static constexpr int GET_INFO = 34;
    static constexpr int STATE_INFO = 35;
    static constexpr int SET_REBOOT = 38;
    static constexpr int GET_LOCATION = 48;
    static constexpr int SET_LOCATION = 49;
    static constexpr int STATE_LOCATION = 50;
    static constexpr int GET_GROUP = 51;
    static constexpr int SET_GROUP = 52;
    static constexpr int STATE_GROUP = 53;
    static constexpr int ECHO_REQUEST = 58;
    static constexpr int ECHO_REPLY = 59;
    
    // LED Queries
    static constexpr int GET_COLOR = 101;
    static constexpr int SET_COLOR = 102;
    static constexpr int SET_WAVEFORM = 103;
    static constexpr int LIGHT_STATE = 107;
    static constexpr int GET_LIGHT_POWER = 116;
    static constexpr int SET_LIGHT_POWER = 117;
    static constexpr int STATE_LIGHT_POWER = 118;
    static constexpr int SET_WAVEFORM_OPTIONAL = 119;
    static constexpr int GET_IR = 120;
    static constexpr int SET_IR = 122;
    static constexpr int STATE_IR = 121;
    static constexpr int GET_HEV_CYCLE = 142;
    static constexpr int SET_HEV_CYCLE = 143;
    static constexpr int STATE_HEV_CYCLE = 144;
    static constexpr int GET_HEV_CYCLE_CONFIG = 145;
    static constexpr int SET_HEV_CYCLE_CONFIG = 146;
    static constexpr int STATE_HEV_CYCLE_CONFIG = 147;
    static constexpr int GET_LAST_HEV_CYCLE_RESULT = 148;
    static constexpr int STATE_LAST_HEV_CYCLE_RESULT = 149;
    /*
    static constexpr int GET_LIGHT = 101;
    static constexpr int SET_COLOR = 102;
    static constexpr int LIGHT_STATE = 107;
    static constexpr int GET_POWER = 116;
    static constexpr int SET_POWER = 117;
    static constexpr int STATE_POWER = 118;
    */
};
