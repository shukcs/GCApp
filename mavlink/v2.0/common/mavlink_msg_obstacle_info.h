#pragma once
// MESSAGE OBSTACLE_INFO PACKING

#define MAVLINK_MSG_ID_OBSTACLE_INFO 418

MAVPACKED(
typedef struct __mavlink_obstacle_info_t {
 int32_t obs_lat[3]; /*< [degE7] obstacle point, Latitude (WGS84), in degrees * 1E7*/
 int32_t obs_lon[3]; /*< [degE7] obstacle point, Longitude (WGS84), in degrees * 1E7*/
 int32_t obs_distance[3]; /*< [cm] obstacle distance, in 0.01m*/
 int32_t obs_azi[3]; /*< [degE2] obstacle angle in 0.01deg*/
 int8_t obs_count; /*<  obstacle count*/
 int8_t obs_danger_id; /*<  closest obstacle id*/
}) mavlink_obstacle_info_t;

#define MAVLINK_MSG_ID_OBSTACLE_INFO_LEN 50
#define MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN 50
#define MAVLINK_MSG_ID_418_LEN 50
#define MAVLINK_MSG_ID_418_MIN_LEN 50

#define MAVLINK_MSG_ID_OBSTACLE_INFO_CRC 39
#define MAVLINK_MSG_ID_418_CRC 39

#define MAVLINK_MSG_OBSTACLE_INFO_FIELD_OBS_LAT_LEN 3
#define MAVLINK_MSG_OBSTACLE_INFO_FIELD_OBS_LON_LEN 3
#define MAVLINK_MSG_OBSTACLE_INFO_FIELD_OBS_DISTANCE_LEN 3
#define MAVLINK_MSG_OBSTACLE_INFO_FIELD_OBS_AZI_LEN 3

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_OBSTACLE_INFO { \
    418, \
    "OBSTACLE_INFO", \
    6, \
    {  { "obs_lat", NULL, MAVLINK_TYPE_INT32_T, 3, 0, offsetof(mavlink_obstacle_info_t, obs_lat) }, \
         { "obs_lon", NULL, MAVLINK_TYPE_INT32_T, 3, 12, offsetof(mavlink_obstacle_info_t, obs_lon) }, \
         { "obs_distance", NULL, MAVLINK_TYPE_INT32_T, 3, 24, offsetof(mavlink_obstacle_info_t, obs_distance) }, \
         { "obs_azi", NULL, MAVLINK_TYPE_INT32_T, 3, 36, offsetof(mavlink_obstacle_info_t, obs_azi) }, \
         { "obs_count", NULL, MAVLINK_TYPE_INT8_T, 0, 48, offsetof(mavlink_obstacle_info_t, obs_count) }, \
         { "obs_danger_id", NULL, MAVLINK_TYPE_INT8_T, 0, 49, offsetof(mavlink_obstacle_info_t, obs_danger_id) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_OBSTACLE_INFO { \
    "OBSTACLE_INFO", \
    6, \
    {  { "obs_lat", NULL, MAVLINK_TYPE_INT32_T, 3, 0, offsetof(mavlink_obstacle_info_t, obs_lat) }, \
         { "obs_lon", NULL, MAVLINK_TYPE_INT32_T, 3, 12, offsetof(mavlink_obstacle_info_t, obs_lon) }, \
         { "obs_distance", NULL, MAVLINK_TYPE_INT32_T, 3, 24, offsetof(mavlink_obstacle_info_t, obs_distance) }, \
         { "obs_azi", NULL, MAVLINK_TYPE_INT32_T, 3, 36, offsetof(mavlink_obstacle_info_t, obs_azi) }, \
         { "obs_count", NULL, MAVLINK_TYPE_INT8_T, 0, 48, offsetof(mavlink_obstacle_info_t, obs_count) }, \
         { "obs_danger_id", NULL, MAVLINK_TYPE_INT8_T, 0, 49, offsetof(mavlink_obstacle_info_t, obs_danger_id) }, \
         } \
}
#endif

/**
 * @brief Pack a obstacle_info message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param obs_lat [degE7] obstacle point, Latitude (WGS84), in degrees * 1E7
 * @param obs_lon [degE7] obstacle point, Longitude (WGS84), in degrees * 1E7
 * @param obs_distance [cm] obstacle distance, in 0.01m
 * @param obs_azi [degE2] obstacle angle in 0.01deg
 * @param obs_count  obstacle count
 * @param obs_danger_id  closest obstacle id
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_obstacle_info_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               const int32_t *obs_lat, const int32_t *obs_lon, const int32_t *obs_distance, const int32_t *obs_azi, int8_t obs_count, int8_t obs_danger_id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_OBSTACLE_INFO_LEN];
    _mav_put_int8_t(buf, 48, obs_count);
    _mav_put_int8_t(buf, 49, obs_danger_id);
    _mav_put_int32_t_array(buf, 0, obs_lat, 3);
    _mav_put_int32_t_array(buf, 12, obs_lon, 3);
    _mav_put_int32_t_array(buf, 24, obs_distance, 3);
    _mav_put_int32_t_array(buf, 36, obs_azi, 3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN);
#else
    mavlink_obstacle_info_t packet;
    packet.obs_count = obs_count;
    packet.obs_danger_id = obs_danger_id;
    mav_array_memcpy(packet.obs_lat, obs_lat, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_lon, obs_lon, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_distance, obs_distance, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_azi, obs_azi, sizeof(int32_t)*3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_OBSTACLE_INFO;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
}

/**
 * @brief Pack a obstacle_info message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param obs_lat [degE7] obstacle point, Latitude (WGS84), in degrees * 1E7
 * @param obs_lon [degE7] obstacle point, Longitude (WGS84), in degrees * 1E7
 * @param obs_distance [cm] obstacle distance, in 0.01m
 * @param obs_azi [degE2] obstacle angle in 0.01deg
 * @param obs_count  obstacle count
 * @param obs_danger_id  closest obstacle id
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_obstacle_info_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   const int32_t *obs_lat,const int32_t *obs_lon,const int32_t *obs_distance,const int32_t *obs_azi,int8_t obs_count,int8_t obs_danger_id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_OBSTACLE_INFO_LEN];
    _mav_put_int8_t(buf, 48, obs_count);
    _mav_put_int8_t(buf, 49, obs_danger_id);
    _mav_put_int32_t_array(buf, 0, obs_lat, 3);
    _mav_put_int32_t_array(buf, 12, obs_lon, 3);
    _mav_put_int32_t_array(buf, 24, obs_distance, 3);
    _mav_put_int32_t_array(buf, 36, obs_azi, 3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN);
#else
    mavlink_obstacle_info_t packet;
    packet.obs_count = obs_count;
    packet.obs_danger_id = obs_danger_id;
    mav_array_memcpy(packet.obs_lat, obs_lat, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_lon, obs_lon, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_distance, obs_distance, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_azi, obs_azi, sizeof(int32_t)*3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_OBSTACLE_INFO;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
}

/**
 * @brief Encode a obstacle_info struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param obstacle_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_obstacle_info_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_obstacle_info_t* obstacle_info)
{
    return mavlink_msg_obstacle_info_pack(system_id, component_id, msg, obstacle_info->obs_lat, obstacle_info->obs_lon, obstacle_info->obs_distance, obstacle_info->obs_azi, obstacle_info->obs_count, obstacle_info->obs_danger_id);
}

/**
 * @brief Encode a obstacle_info struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param obstacle_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_obstacle_info_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_obstacle_info_t* obstacle_info)
{
    return mavlink_msg_obstacle_info_pack_chan(system_id, component_id, chan, msg, obstacle_info->obs_lat, obstacle_info->obs_lon, obstacle_info->obs_distance, obstacle_info->obs_azi, obstacle_info->obs_count, obstacle_info->obs_danger_id);
}

/**
 * @brief Send a obstacle_info message
 * @param chan MAVLink channel to send the message
 *
 * @param obs_lat [degE7] obstacle point, Latitude (WGS84), in degrees * 1E7
 * @param obs_lon [degE7] obstacle point, Longitude (WGS84), in degrees * 1E7
 * @param obs_distance [cm] obstacle distance, in 0.01m
 * @param obs_azi [degE2] obstacle angle in 0.01deg
 * @param obs_count  obstacle count
 * @param obs_danger_id  closest obstacle id
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_obstacle_info_send(mavlink_channel_t chan, const int32_t *obs_lat, const int32_t *obs_lon, const int32_t *obs_distance, const int32_t *obs_azi, int8_t obs_count, int8_t obs_danger_id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_OBSTACLE_INFO_LEN];
    _mav_put_int8_t(buf, 48, obs_count);
    _mav_put_int8_t(buf, 49, obs_danger_id);
    _mav_put_int32_t_array(buf, 0, obs_lat, 3);
    _mav_put_int32_t_array(buf, 12, obs_lon, 3);
    _mav_put_int32_t_array(buf, 24, obs_distance, 3);
    _mav_put_int32_t_array(buf, 36, obs_azi, 3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_OBSTACLE_INFO, buf, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
#else
    mavlink_obstacle_info_t packet;
    packet.obs_count = obs_count;
    packet.obs_danger_id = obs_danger_id;
    mav_array_memcpy(packet.obs_lat, obs_lat, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_lon, obs_lon, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_distance, obs_distance, sizeof(int32_t)*3);
    mav_array_memcpy(packet.obs_azi, obs_azi, sizeof(int32_t)*3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_OBSTACLE_INFO, (const char *)&packet, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
#endif
}

/**
 * @brief Send a obstacle_info message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_obstacle_info_send_struct(mavlink_channel_t chan, const mavlink_obstacle_info_t* obstacle_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_obstacle_info_send(chan, obstacle_info->obs_lat, obstacle_info->obs_lon, obstacle_info->obs_distance, obstacle_info->obs_azi, obstacle_info->obs_count, obstacle_info->obs_danger_id);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_OBSTACLE_INFO, (const char *)obstacle_info, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
#endif
}

#if MAVLINK_MSG_ID_OBSTACLE_INFO_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_obstacle_info_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const int32_t *obs_lat, const int32_t *obs_lon, const int32_t *obs_distance, const int32_t *obs_azi, int8_t obs_count, int8_t obs_danger_id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_int8_t(buf, 48, obs_count);
    _mav_put_int8_t(buf, 49, obs_danger_id);
    _mav_put_int32_t_array(buf, 0, obs_lat, 3);
    _mav_put_int32_t_array(buf, 12, obs_lon, 3);
    _mav_put_int32_t_array(buf, 24, obs_distance, 3);
    _mav_put_int32_t_array(buf, 36, obs_azi, 3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_OBSTACLE_INFO, buf, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
#else
    mavlink_obstacle_info_t *packet = (mavlink_obstacle_info_t *)msgbuf;
    packet->obs_count = obs_count;
    packet->obs_danger_id = obs_danger_id;
    mav_array_memcpy(packet->obs_lat, obs_lat, sizeof(int32_t)*3);
    mav_array_memcpy(packet->obs_lon, obs_lon, sizeof(int32_t)*3);
    mav_array_memcpy(packet->obs_distance, obs_distance, sizeof(int32_t)*3);
    mav_array_memcpy(packet->obs_azi, obs_azi, sizeof(int32_t)*3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_OBSTACLE_INFO, (const char *)packet, MAVLINK_MSG_ID_OBSTACLE_INFO_MIN_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN, MAVLINK_MSG_ID_OBSTACLE_INFO_CRC);
#endif
}
#endif

#endif

// MESSAGE OBSTACLE_INFO UNPACKING


/**
 * @brief Get field obs_lat from obstacle_info message
 *
 * @return [degE7] obstacle point, Latitude (WGS84), in degrees * 1E7
 */
static inline uint16_t mavlink_msg_obstacle_info_get_obs_lat(const mavlink_message_t* msg, int32_t *obs_lat)
{
    return _MAV_RETURN_int32_t_array(msg, obs_lat, 3,  0);
}

/**
 * @brief Get field obs_lon from obstacle_info message
 *
 * @return [degE7] obstacle point, Longitude (WGS84), in degrees * 1E7
 */
static inline uint16_t mavlink_msg_obstacle_info_get_obs_lon(const mavlink_message_t* msg, int32_t *obs_lon)
{
    return _MAV_RETURN_int32_t_array(msg, obs_lon, 3,  12);
}

/**
 * @brief Get field obs_distance from obstacle_info message
 *
 * @return [cm] obstacle distance, in 0.01m
 */
static inline uint16_t mavlink_msg_obstacle_info_get_obs_distance(const mavlink_message_t* msg, int32_t *obs_distance)
{
    return _MAV_RETURN_int32_t_array(msg, obs_distance, 3,  24);
}

/**
 * @brief Get field obs_azi from obstacle_info message
 *
 * @return [degE2] obstacle angle in 0.01deg
 */
static inline uint16_t mavlink_msg_obstacle_info_get_obs_azi(const mavlink_message_t* msg, int32_t *obs_azi)
{
    return _MAV_RETURN_int32_t_array(msg, obs_azi, 3,  36);
}

/**
 * @brief Get field obs_count from obstacle_info message
 *
 * @return  obstacle count
 */
static inline int8_t mavlink_msg_obstacle_info_get_obs_count(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int8_t(msg,  48);
}

/**
 * @brief Get field obs_danger_id from obstacle_info message
 *
 * @return  closest obstacle id
 */
static inline int8_t mavlink_msg_obstacle_info_get_obs_danger_id(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int8_t(msg,  49);
}

/**
 * @brief Decode a obstacle_info message into a struct
 *
 * @param msg The message to decode
 * @param obstacle_info C-struct to decode the message contents into
 */
static inline void mavlink_msg_obstacle_info_decode(const mavlink_message_t* msg, mavlink_obstacle_info_t* obstacle_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_obstacle_info_get_obs_lat(msg, obstacle_info->obs_lat);
    mavlink_msg_obstacle_info_get_obs_lon(msg, obstacle_info->obs_lon);
    mavlink_msg_obstacle_info_get_obs_distance(msg, obstacle_info->obs_distance);
    mavlink_msg_obstacle_info_get_obs_azi(msg, obstacle_info->obs_azi);
    obstacle_info->obs_count = mavlink_msg_obstacle_info_get_obs_count(msg);
    obstacle_info->obs_danger_id = mavlink_msg_obstacle_info_get_obs_danger_id(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_OBSTACLE_INFO_LEN? msg->len : MAVLINK_MSG_ID_OBSTACLE_INFO_LEN;
        memset(obstacle_info, 0, MAVLINK_MSG_ID_OBSTACLE_INFO_LEN);
    memcpy(obstacle_info, _MAV_PAYLOAD(msg), len);
#endif
}
