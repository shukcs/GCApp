#pragma once
// MESSAGE INTERRUPT_POINT PACKING

#define MAVLINK_MSG_ID_INTERRUPT_POINT 415

MAVPACKED(
typedef struct __mavlink_interrupt_point_t {
 int32_t int_latitude; /*< [degE7] interrupt point, Latitude (WGS84), in degrees * 1E7*/
 int32_t int_longitude; /*< [degE7] interrupt point, Longitude (WGS84, in degrees * 1E7*/
 int32_t int_altitude; /*< [mm] interrupt point, Altitude (AMSL), in meters * 1000 (positive for up)*/
 uint8_t int_type; /*<  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid*/
}) mavlink_interrupt_point_t;

#define MAVLINK_MSG_ID_INTERRUPT_POINT_LEN 13
#define MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN 13
#define MAVLINK_MSG_ID_415_LEN 13
#define MAVLINK_MSG_ID_415_MIN_LEN 13

#define MAVLINK_MSG_ID_INTERRUPT_POINT_CRC 185
#define MAVLINK_MSG_ID_415_CRC 185



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_INTERRUPT_POINT { \
    415, \
    "INTERRUPT_POINT", \
    4, \
    {  { "int_latitude", NULL, MAVLINK_TYPE_INT32_T, 0, 0, offsetof(mavlink_interrupt_point_t, int_latitude) }, \
         { "int_longitude", NULL, MAVLINK_TYPE_INT32_T, 0, 4, offsetof(mavlink_interrupt_point_t, int_longitude) }, \
         { "int_altitude", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_interrupt_point_t, int_altitude) }, \
         { "int_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_interrupt_point_t, int_type) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_INTERRUPT_POINT { \
    "INTERRUPT_POINT", \
    4, \
    {  { "int_latitude", NULL, MAVLINK_TYPE_INT32_T, 0, 0, offsetof(mavlink_interrupt_point_t, int_latitude) }, \
         { "int_longitude", NULL, MAVLINK_TYPE_INT32_T, 0, 4, offsetof(mavlink_interrupt_point_t, int_longitude) }, \
         { "int_altitude", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_interrupt_point_t, int_altitude) }, \
         { "int_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_interrupt_point_t, int_type) }, \
         } \
}
#endif

/**
 * @brief Pack a interrupt_point message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param int_latitude [degE7] interrupt point, Latitude (WGS84), in degrees * 1E7
 * @param int_longitude [degE7] interrupt point, Longitude (WGS84, in degrees * 1E7
 * @param int_altitude [mm] interrupt point, Altitude (AMSL), in meters * 1000 (positive for up)
 * @param int_type  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_interrupt_point_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               int32_t int_latitude, int32_t int_longitude, int32_t int_altitude, uint8_t int_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_INTERRUPT_POINT_LEN];
    _mav_put_int32_t(buf, 0, int_latitude);
    _mav_put_int32_t(buf, 4, int_longitude);
    _mav_put_int32_t(buf, 8, int_altitude);
    _mav_put_uint8_t(buf, 12, int_type);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN);
#else
    mavlink_interrupt_point_t packet;
    packet.int_latitude = int_latitude;
    packet.int_longitude = int_longitude;
    packet.int_altitude = int_altitude;
    packet.int_type = int_type;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_INTERRUPT_POINT;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
}

/**
 * @brief Pack a interrupt_point message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param int_latitude [degE7] interrupt point, Latitude (WGS84), in degrees * 1E7
 * @param int_longitude [degE7] interrupt point, Longitude (WGS84, in degrees * 1E7
 * @param int_altitude [mm] interrupt point, Altitude (AMSL), in meters * 1000 (positive for up)
 * @param int_type  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_interrupt_point_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   int32_t int_latitude,int32_t int_longitude,int32_t int_altitude,uint8_t int_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_INTERRUPT_POINT_LEN];
    _mav_put_int32_t(buf, 0, int_latitude);
    _mav_put_int32_t(buf, 4, int_longitude);
    _mav_put_int32_t(buf, 8, int_altitude);
    _mav_put_uint8_t(buf, 12, int_type);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN);
#else
    mavlink_interrupt_point_t packet;
    packet.int_latitude = int_latitude;
    packet.int_longitude = int_longitude;
    packet.int_altitude = int_altitude;
    packet.int_type = int_type;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_INTERRUPT_POINT;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
}

/**
 * @brief Encode a interrupt_point struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param interrupt_point C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_interrupt_point_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_interrupt_point_t* interrupt_point)
{
    return mavlink_msg_interrupt_point_pack(system_id, component_id, msg, interrupt_point->int_latitude, interrupt_point->int_longitude, interrupt_point->int_altitude, interrupt_point->int_type);
}

/**
 * @brief Encode a interrupt_point struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param interrupt_point C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_interrupt_point_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_interrupt_point_t* interrupt_point)
{
    return mavlink_msg_interrupt_point_pack_chan(system_id, component_id, chan, msg, interrupt_point->int_latitude, interrupt_point->int_longitude, interrupt_point->int_altitude, interrupt_point->int_type);
}

/**
 * @brief Send a interrupt_point message
 * @param chan MAVLink channel to send the message
 *
 * @param int_latitude [degE7] interrupt point, Latitude (WGS84), in degrees * 1E7
 * @param int_longitude [degE7] interrupt point, Longitude (WGS84, in degrees * 1E7
 * @param int_altitude [mm] interrupt point, Altitude (AMSL), in meters * 1000 (positive for up)
 * @param int_type  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_interrupt_point_send(mavlink_channel_t chan, int32_t int_latitude, int32_t int_longitude, int32_t int_altitude, uint8_t int_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_INTERRUPT_POINT_LEN];
    _mav_put_int32_t(buf, 0, int_latitude);
    _mav_put_int32_t(buf, 4, int_longitude);
    _mav_put_int32_t(buf, 8, int_altitude);
    _mav_put_uint8_t(buf, 12, int_type);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT, buf, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
#else
    mavlink_interrupt_point_t packet;
    packet.int_latitude = int_latitude;
    packet.int_longitude = int_longitude;
    packet.int_altitude = int_altitude;
    packet.int_type = int_type;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT, (const char *)&packet, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
#endif
}

/**
 * @brief Send a interrupt_point message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_interrupt_point_send_struct(mavlink_channel_t chan, const mavlink_interrupt_point_t* interrupt_point)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_interrupt_point_send(chan, interrupt_point->int_latitude, interrupt_point->int_longitude, interrupt_point->int_altitude, interrupt_point->int_type);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT, (const char *)interrupt_point, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
#endif
}

#if MAVLINK_MSG_ID_INTERRUPT_POINT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_interrupt_point_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int32_t int_latitude, int32_t int_longitude, int32_t int_altitude, uint8_t int_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_int32_t(buf, 0, int_latitude);
    _mav_put_int32_t(buf, 4, int_longitude);
    _mav_put_int32_t(buf, 8, int_altitude);
    _mav_put_uint8_t(buf, 12, int_type);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT, buf, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
#else
    mavlink_interrupt_point_t *packet = (mavlink_interrupt_point_t *)msgbuf;
    packet->int_latitude = int_latitude;
    packet->int_longitude = int_longitude;
    packet->int_altitude = int_altitude;
    packet->int_type = int_type;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT, (const char *)packet, MAVLINK_MSG_ID_INTERRUPT_POINT_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_CRC);
#endif
}
#endif

#endif

// MESSAGE INTERRUPT_POINT UNPACKING


/**
 * @brief Get field int_latitude from interrupt_point message
 *
 * @return [degE7] interrupt point, Latitude (WGS84), in degrees * 1E7
 */
static inline int32_t mavlink_msg_interrupt_point_get_int_latitude(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  0);
}

/**
 * @brief Get field int_longitude from interrupt_point message
 *
 * @return [degE7] interrupt point, Longitude (WGS84, in degrees * 1E7
 */
static inline int32_t mavlink_msg_interrupt_point_get_int_longitude(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  4);
}

/**
 * @brief Get field int_altitude from interrupt_point message
 *
 * @return [mm] interrupt point, Altitude (AMSL), in meters * 1000 (positive for up)
 */
static inline int32_t mavlink_msg_interrupt_point_get_int_altitude(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  8);
}

/**
 * @brief Get field int_type from interrupt_point message
 *
 * @return  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 */
static inline uint8_t mavlink_msg_interrupt_point_get_int_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  12);
}

/**
 * @brief Decode a interrupt_point message into a struct
 *
 * @param msg The message to decode
 * @param interrupt_point C-struct to decode the message contents into
 */
static inline void mavlink_msg_interrupt_point_decode(const mavlink_message_t* msg, mavlink_interrupt_point_t* interrupt_point)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    interrupt_point->int_latitude = mavlink_msg_interrupt_point_get_int_latitude(msg);
    interrupt_point->int_longitude = mavlink_msg_interrupt_point_get_int_longitude(msg);
    interrupt_point->int_altitude = mavlink_msg_interrupt_point_get_int_altitude(msg);
    interrupt_point->int_type = mavlink_msg_interrupt_point_get_int_type(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_INTERRUPT_POINT_LEN? msg->len : MAVLINK_MSG_ID_INTERRUPT_POINT_LEN;
        memset(interrupt_point, 0, MAVLINK_MSG_ID_INTERRUPT_POINT_LEN);
    memcpy(interrupt_point, _MAV_PAYLOAD(msg), len);
#endif
}
