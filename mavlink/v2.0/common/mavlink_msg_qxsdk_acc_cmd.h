#pragma once
// MESSAGE QXSDK_ACC_CMD PACKING

#define MAVLINK_MSG_ID_QXSDK_ACC_CMD 412

MAVPACKED(
typedef struct __mavlink_qxsdk_acc_cmd_t {
 uint8_t sdkcmd; /*<  qxsdk acc command*/
}) mavlink_qxsdk_acc_cmd_t;

#define MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN 1
#define MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN 1
#define MAVLINK_MSG_ID_412_LEN 1
#define MAVLINK_MSG_ID_412_MIN_LEN 1

#define MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC 83
#define MAVLINK_MSG_ID_412_CRC 83



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_QXSDK_ACC_CMD { \
    412, \
    "QXSDK_ACC_CMD", \
    1, \
    {  { "sdkcmd", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_qxsdk_acc_cmd_t, sdkcmd) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_QXSDK_ACC_CMD { \
    "QXSDK_ACC_CMD", \
    1, \
    {  { "sdkcmd", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_qxsdk_acc_cmd_t, sdkcmd) }, \
         } \
}
#endif

/**
 * @brief Pack a qxsdk_acc_cmd message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param sdkcmd  qxsdk acc command
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_qxsdk_acc_cmd_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t sdkcmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN];
    _mav_put_uint8_t(buf, 0, sdkcmd);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN);
#else
    mavlink_qxsdk_acc_cmd_t packet;
    packet.sdkcmd = sdkcmd;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_QXSDK_ACC_CMD;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
}

/**
 * @brief Pack a qxsdk_acc_cmd message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param sdkcmd  qxsdk acc command
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_qxsdk_acc_cmd_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t sdkcmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN];
    _mav_put_uint8_t(buf, 0, sdkcmd);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN);
#else
    mavlink_qxsdk_acc_cmd_t packet;
    packet.sdkcmd = sdkcmd;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_QXSDK_ACC_CMD;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
}

/**
 * @brief Encode a qxsdk_acc_cmd struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param qxsdk_acc_cmd C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_qxsdk_acc_cmd_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_qxsdk_acc_cmd_t* qxsdk_acc_cmd)
{
    return mavlink_msg_qxsdk_acc_cmd_pack(system_id, component_id, msg, qxsdk_acc_cmd->sdkcmd);
}

/**
 * @brief Encode a qxsdk_acc_cmd struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param qxsdk_acc_cmd C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_qxsdk_acc_cmd_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_qxsdk_acc_cmd_t* qxsdk_acc_cmd)
{
    return mavlink_msg_qxsdk_acc_cmd_pack_chan(system_id, component_id, chan, msg, qxsdk_acc_cmd->sdkcmd);
}

/**
 * @brief Send a qxsdk_acc_cmd message
 * @param chan MAVLink channel to send the message
 *
 * @param sdkcmd  qxsdk acc command
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_qxsdk_acc_cmd_send(mavlink_channel_t chan, uint8_t sdkcmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN];
    _mav_put_uint8_t(buf, 0, sdkcmd);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACC_CMD, buf, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
#else
    mavlink_qxsdk_acc_cmd_t packet;
    packet.sdkcmd = sdkcmd;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACC_CMD, (const char *)&packet, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
#endif
}

/**
 * @brief Send a qxsdk_acc_cmd message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_qxsdk_acc_cmd_send_struct(mavlink_channel_t chan, const mavlink_qxsdk_acc_cmd_t* qxsdk_acc_cmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_qxsdk_acc_cmd_send(chan, qxsdk_acc_cmd->sdkcmd);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACC_CMD, (const char *)qxsdk_acc_cmd, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
#endif
}

#if MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_qxsdk_acc_cmd_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t sdkcmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, sdkcmd);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACC_CMD, buf, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
#else
    mavlink_qxsdk_acc_cmd_t *packet = (mavlink_qxsdk_acc_cmd_t *)msgbuf;
    packet->sdkcmd = sdkcmd;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACC_CMD, (const char *)packet, MAVLINK_MSG_ID_QXSDK_ACC_CMD_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN, MAVLINK_MSG_ID_QXSDK_ACC_CMD_CRC);
#endif
}
#endif

#endif

// MESSAGE QXSDK_ACC_CMD UNPACKING


/**
 * @brief Get field sdkcmd from qxsdk_acc_cmd message
 *
 * @return  qxsdk acc command
 */
static inline uint8_t mavlink_msg_qxsdk_acc_cmd_get_sdkcmd(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Decode a qxsdk_acc_cmd message into a struct
 *
 * @param msg The message to decode
 * @param qxsdk_acc_cmd C-struct to decode the message contents into
 */
static inline void mavlink_msg_qxsdk_acc_cmd_decode(const mavlink_message_t* msg, mavlink_qxsdk_acc_cmd_t* qxsdk_acc_cmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    qxsdk_acc_cmd->sdkcmd = mavlink_msg_qxsdk_acc_cmd_get_sdkcmd(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN? msg->len : MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN;
        memset(qxsdk_acc_cmd, 0, MAVLINK_MSG_ID_QXSDK_ACC_CMD_LEN);
    memcpy(qxsdk_acc_cmd, _MAV_PAYLOAD(msg), len);
#endif
}
