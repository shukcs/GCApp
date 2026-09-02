#pragma once
// MESSAGE GET_OBSTACLE_INFO PACKING

#define MAVLINK_MSG_ID_GET_OBSTACLE_INFO 417

MAVPACKED(
typedef struct __mavlink_get_obstacle_info_t {
 uint8_t obs_type; /*<  obstacle type,it is not necessary*/
}) mavlink_get_obstacle_info_t;

#define MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN 1
#define MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN 1
#define MAVLINK_MSG_ID_417_LEN 1
#define MAVLINK_MSG_ID_417_MIN_LEN 1

#define MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC 80
#define MAVLINK_MSG_ID_417_CRC 80



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_GET_OBSTACLE_INFO { \
    417, \
    "GET_OBSTACLE_INFO", \
    1, \
    {  { "obs_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_get_obstacle_info_t, obs_type) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_GET_OBSTACLE_INFO { \
    "GET_OBSTACLE_INFO", \
    1, \
    {  { "obs_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_get_obstacle_info_t, obs_type) }, \
         } \
}
#endif

/**
 * @brief Pack a get_obstacle_info message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param obs_type  obstacle type,it is not necessary
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_obstacle_info_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t obs_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN];
    _mav_put_uint8_t(buf, 0, obs_type);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN);
#else
    mavlink_get_obstacle_info_t packet;
    packet.obs_type = obs_type;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GET_OBSTACLE_INFO;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
}

/**
 * @brief Pack a get_obstacle_info message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param obs_type  obstacle type,it is not necessary
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_obstacle_info_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t obs_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN];
    _mav_put_uint8_t(buf, 0, obs_type);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN);
#else
    mavlink_get_obstacle_info_t packet;
    packet.obs_type = obs_type;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GET_OBSTACLE_INFO;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
}

/**
 * @brief Encode a get_obstacle_info struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param get_obstacle_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_obstacle_info_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_get_obstacle_info_t* get_obstacle_info)
{
    return mavlink_msg_get_obstacle_info_pack(system_id, component_id, msg, get_obstacle_info->obs_type);
}

/**
 * @brief Encode a get_obstacle_info struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param get_obstacle_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_obstacle_info_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_get_obstacle_info_t* get_obstacle_info)
{
    return mavlink_msg_get_obstacle_info_pack_chan(system_id, component_id, chan, msg, get_obstacle_info->obs_type);
}

/**
 * @brief Send a get_obstacle_info message
 * @param chan MAVLink channel to send the message
 *
 * @param obs_type  obstacle type,it is not necessary
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_get_obstacle_info_send(mavlink_channel_t chan, uint8_t obs_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN];
    _mav_put_uint8_t(buf, 0, obs_type);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_OBSTACLE_INFO, buf, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
#else
    mavlink_get_obstacle_info_t packet;
    packet.obs_type = obs_type;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_OBSTACLE_INFO, (const char *)&packet, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
#endif
}

/**
 * @brief Send a get_obstacle_info message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_get_obstacle_info_send_struct(mavlink_channel_t chan, const mavlink_get_obstacle_info_t* get_obstacle_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_get_obstacle_info_send(chan, get_obstacle_info->obs_type);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_OBSTACLE_INFO, (const char *)get_obstacle_info, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
#endif
}

#if MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_get_obstacle_info_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t obs_type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, obs_type);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_OBSTACLE_INFO, buf, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
#else
    mavlink_get_obstacle_info_t *packet = (mavlink_get_obstacle_info_t *)msgbuf;
    packet->obs_type = obs_type;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_OBSTACLE_INFO, (const char *)packet, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_CRC);
#endif
}
#endif

#endif

// MESSAGE GET_OBSTACLE_INFO UNPACKING


/**
 * @brief Get field obs_type from get_obstacle_info message
 *
 * @return  obstacle type,it is not necessary
 */
static inline uint8_t mavlink_msg_get_obstacle_info_get_obs_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Decode a get_obstacle_info message into a struct
 *
 * @param msg The message to decode
 * @param get_obstacle_info C-struct to decode the message contents into
 */
static inline void mavlink_msg_get_obstacle_info_decode(const mavlink_message_t* msg, mavlink_get_obstacle_info_t* get_obstacle_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    get_obstacle_info->obs_type = mavlink_msg_get_obstacle_info_get_obs_type(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN? msg->len : MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN;
        memset(get_obstacle_info, 0, MAVLINK_MSG_ID_GET_OBSTACLE_INFO_LEN);
    memcpy(get_obstacle_info, _MAV_PAYLOAD(msg), len);
#endif
}
