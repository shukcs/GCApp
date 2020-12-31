#pragma once
// MESSAGE INTERRUPT_POINT_ACK PACKING

#define MAVLINK_MSG_ID_INTERRUPT_POINT_ACK 416

MAVPACKED(
typedef struct __mavlink_interrupt_point_ack_t {
 uint8_t ack_value; /*<  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid*/
}) mavlink_interrupt_point_ack_t;

#define MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN 1
#define MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN 1
#define MAVLINK_MSG_ID_416_LEN 1
#define MAVLINK_MSG_ID_416_MIN_LEN 1

#define MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC 203
#define MAVLINK_MSG_ID_416_CRC 203



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_INTERRUPT_POINT_ACK { \
    416, \
    "INTERRUPT_POINT_ACK", \
    1, \
    {  { "ack_value", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_interrupt_point_ack_t, ack_value) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_INTERRUPT_POINT_ACK { \
    "INTERRUPT_POINT_ACK", \
    1, \
    {  { "ack_value", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_interrupt_point_ack_t, ack_value) }, \
         } \
}
#endif

/**
 * @brief Pack a interrupt_point_ack message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param ack_value  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_interrupt_point_ack_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t ack_value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN];
    _mav_put_uint8_t(buf, 0, ack_value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN);
#else
    mavlink_interrupt_point_ack_t packet;
    packet.ack_value = ack_value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_INTERRUPT_POINT_ACK;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
}

/**
 * @brief Pack a interrupt_point_ack message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param ack_value  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_interrupt_point_ack_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t ack_value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN];
    _mav_put_uint8_t(buf, 0, ack_value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN);
#else
    mavlink_interrupt_point_ack_t packet;
    packet.ack_value = ack_value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_INTERRUPT_POINT_ACK;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
}

/**
 * @brief Encode a interrupt_point_ack struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param interrupt_point_ack C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_interrupt_point_ack_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_interrupt_point_ack_t* interrupt_point_ack)
{
    return mavlink_msg_interrupt_point_ack_pack(system_id, component_id, msg, interrupt_point_ack->ack_value);
}

/**
 * @brief Encode a interrupt_point_ack struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param interrupt_point_ack C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_interrupt_point_ack_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_interrupt_point_ack_t* interrupt_point_ack)
{
    return mavlink_msg_interrupt_point_ack_pack_chan(system_id, component_id, chan, msg, interrupt_point_ack->ack_value);
}

/**
 * @brief Send a interrupt_point_ack message
 * @param chan MAVLink channel to send the message
 *
 * @param ack_value  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_interrupt_point_ack_send(mavlink_channel_t chan, uint8_t ack_value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN];
    _mav_put_uint8_t(buf, 0, ack_value);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK, buf, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
#else
    mavlink_interrupt_point_ack_t packet;
    packet.ack_value = ack_value;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK, (const char *)&packet, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
#endif
}

/**
 * @brief Send a interrupt_point_ack message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_interrupt_point_ack_send_struct(mavlink_channel_t chan, const mavlink_interrupt_point_ack_t* interrupt_point_ack)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_interrupt_point_ack_send(chan, interrupt_point_ack->ack_value);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK, (const char *)interrupt_point_ack, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
#endif
}

#if MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_interrupt_point_ack_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t ack_value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, ack_value);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK, buf, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
#else
    mavlink_interrupt_point_ack_t *packet = (mavlink_interrupt_point_ack_t *)msgbuf;
    packet->ack_value = ack_value;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK, (const char *)packet, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_MIN_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_CRC);
#endif
}
#endif

#endif

// MESSAGE INTERRUPT_POINT_ACK UNPACKING


/**
 * @brief Get field ack_value from interrupt_point_ack message
 *
 * @return  0x01 means mission interrupt point, 0x02 means ABpiont mode interrupt point valid
 */
static inline uint8_t mavlink_msg_interrupt_point_ack_get_ack_value(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Decode a interrupt_point_ack message into a struct
 *
 * @param msg The message to decode
 * @param interrupt_point_ack C-struct to decode the message contents into
 */
static inline void mavlink_msg_interrupt_point_ack_decode(const mavlink_message_t* msg, mavlink_interrupt_point_ack_t* interrupt_point_ack)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    interrupt_point_ack->ack_value = mavlink_msg_interrupt_point_ack_get_ack_value(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN? msg->len : MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN;
        memset(interrupt_point_ack, 0, MAVLINK_MSG_ID_INTERRUPT_POINT_ACK_LEN);
    memcpy(interrupt_point_ack, _MAV_PAYLOAD(msg), len);
#endif
}
