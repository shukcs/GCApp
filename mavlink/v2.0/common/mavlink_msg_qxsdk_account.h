#pragma once
// MESSAGE QXSDK_ACCOUNT PACKING

#define MAVLINK_MSG_ID_QXSDK_ACCOUNT 411

MAVPACKED(
typedef struct __mavlink_qxsdk_account_t {
 char akey[16]; /*<  qxsdk akey*/
 char asec[65]; /*<  qxsdk asec*/
 char devid[9]; /*<  qxsdk devid*/
 char type[2]; /*<  qxsdk type*/
 uint8_t sdkserver; /*<  qxsdk sdkserver*/
}) mavlink_qxsdk_account_t;

#define MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN 93
#define MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN 93
#define MAVLINK_MSG_ID_411_LEN 93
#define MAVLINK_MSG_ID_411_MIN_LEN 93

#define MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC 162
#define MAVLINK_MSG_ID_411_CRC 162

#define MAVLINK_MSG_QXSDK_ACCOUNT_FIELD_AKEY_LEN 16
#define MAVLINK_MSG_QXSDK_ACCOUNT_FIELD_ASEC_LEN 65
#define MAVLINK_MSG_QXSDK_ACCOUNT_FIELD_DEVID_LEN 9
#define MAVLINK_MSG_QXSDK_ACCOUNT_FIELD_TYPE_LEN 2

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_QXSDK_ACCOUNT { \
    411, \
    "QXSDK_ACCOUNT", \
    5, \
    {  { "akey", NULL, MAVLINK_TYPE_CHAR, 16, 0, offsetof(mavlink_qxsdk_account_t, akey) }, \
         { "asec", NULL, MAVLINK_TYPE_CHAR, 65, 16, offsetof(mavlink_qxsdk_account_t, asec) }, \
         { "devid", NULL, MAVLINK_TYPE_CHAR, 9, 81, offsetof(mavlink_qxsdk_account_t, devid) }, \
         { "type", NULL, MAVLINK_TYPE_CHAR, 2, 90, offsetof(mavlink_qxsdk_account_t, type) }, \
         { "sdkserver", NULL, MAVLINK_TYPE_UINT8_T, 0, 92, offsetof(mavlink_qxsdk_account_t, sdkserver) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_QXSDK_ACCOUNT { \
    "QXSDK_ACCOUNT", \
    5, \
    {  { "akey", NULL, MAVLINK_TYPE_CHAR, 16, 0, offsetof(mavlink_qxsdk_account_t, akey) }, \
         { "asec", NULL, MAVLINK_TYPE_CHAR, 65, 16, offsetof(mavlink_qxsdk_account_t, asec) }, \
         { "devid", NULL, MAVLINK_TYPE_CHAR, 9, 81, offsetof(mavlink_qxsdk_account_t, devid) }, \
         { "type", NULL, MAVLINK_TYPE_CHAR, 2, 90, offsetof(mavlink_qxsdk_account_t, type) }, \
         { "sdkserver", NULL, MAVLINK_TYPE_UINT8_T, 0, 92, offsetof(mavlink_qxsdk_account_t, sdkserver) }, \
         } \
}
#endif

/**
 * @brief Pack a qxsdk_account message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param akey  qxsdk akey
 * @param asec  qxsdk asec
 * @param devid  qxsdk devid
 * @param type  qxsdk type
 * @param sdkserver  qxsdk sdkserver
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_qxsdk_account_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               const char *akey, const char *asec, const char *devid, const char *type, uint8_t sdkserver)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN];
    _mav_put_uint8_t(buf, 92, sdkserver);
    _mav_put_char_array(buf, 0, akey, 16);
    _mav_put_char_array(buf, 16, asec, 65);
    _mav_put_char_array(buf, 81, devid, 9);
    _mav_put_char_array(buf, 90, type, 2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN);
#else
    mavlink_qxsdk_account_t packet;
    packet.sdkserver = sdkserver;
    mav_array_memcpy(packet.akey, akey, sizeof(char)*16);
    mav_array_memcpy(packet.asec, asec, sizeof(char)*65);
    mav_array_memcpy(packet.devid, devid, sizeof(char)*9);
    mav_array_memcpy(packet.type, type, sizeof(char)*2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_QXSDK_ACCOUNT;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
}

/**
 * @brief Pack a qxsdk_account message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param akey  qxsdk akey
 * @param asec  qxsdk asec
 * @param devid  qxsdk devid
 * @param type  qxsdk type
 * @param sdkserver  qxsdk sdkserver
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_qxsdk_account_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   const char *akey,const char *asec,const char *devid,const char *type,uint8_t sdkserver)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN];
    _mav_put_uint8_t(buf, 92, sdkserver);
    _mav_put_char_array(buf, 0, akey, 16);
    _mav_put_char_array(buf, 16, asec, 65);
    _mav_put_char_array(buf, 81, devid, 9);
    _mav_put_char_array(buf, 90, type, 2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN);
#else
    mavlink_qxsdk_account_t packet;
    packet.sdkserver = sdkserver;
    mav_array_memcpy(packet.akey, akey, sizeof(char)*16);
    mav_array_memcpy(packet.asec, asec, sizeof(char)*65);
    mav_array_memcpy(packet.devid, devid, sizeof(char)*9);
    mav_array_memcpy(packet.type, type, sizeof(char)*2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_QXSDK_ACCOUNT;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
}

/**
 * @brief Encode a qxsdk_account struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param qxsdk_account C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_qxsdk_account_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_qxsdk_account_t* qxsdk_account)
{
    return mavlink_msg_qxsdk_account_pack(system_id, component_id, msg, qxsdk_account->akey, qxsdk_account->asec, qxsdk_account->devid, qxsdk_account->type, qxsdk_account->sdkserver);
}

/**
 * @brief Encode a qxsdk_account struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param qxsdk_account C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_qxsdk_account_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_qxsdk_account_t* qxsdk_account)
{
    return mavlink_msg_qxsdk_account_pack_chan(system_id, component_id, chan, msg, qxsdk_account->akey, qxsdk_account->asec, qxsdk_account->devid, qxsdk_account->type, qxsdk_account->sdkserver);
}

/**
 * @brief Send a qxsdk_account message
 * @param chan MAVLink channel to send the message
 *
 * @param akey  qxsdk akey
 * @param asec  qxsdk asec
 * @param devid  qxsdk devid
 * @param type  qxsdk type
 * @param sdkserver  qxsdk sdkserver
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_qxsdk_account_send(mavlink_channel_t chan, const char *akey, const char *asec, const char *devid, const char *type, uint8_t sdkserver)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN];
    _mav_put_uint8_t(buf, 92, sdkserver);
    _mav_put_char_array(buf, 0, akey, 16);
    _mav_put_char_array(buf, 16, asec, 65);
    _mav_put_char_array(buf, 81, devid, 9);
    _mav_put_char_array(buf, 90, type, 2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACCOUNT, buf, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
#else
    mavlink_qxsdk_account_t packet;
    packet.sdkserver = sdkserver;
    mav_array_memcpy(packet.akey, akey, sizeof(char)*16);
    mav_array_memcpy(packet.asec, asec, sizeof(char)*65);
    mav_array_memcpy(packet.devid, devid, sizeof(char)*9);
    mav_array_memcpy(packet.type, type, sizeof(char)*2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACCOUNT, (const char *)&packet, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
#endif
}

/**
 * @brief Send a qxsdk_account message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_qxsdk_account_send_struct(mavlink_channel_t chan, const mavlink_qxsdk_account_t* qxsdk_account)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_qxsdk_account_send(chan, qxsdk_account->akey, qxsdk_account->asec, qxsdk_account->devid, qxsdk_account->type, qxsdk_account->sdkserver);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACCOUNT, (const char *)qxsdk_account, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
#endif
}

#if MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_qxsdk_account_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const char *akey, const char *asec, const char *devid, const char *type, uint8_t sdkserver)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 92, sdkserver);
    _mav_put_char_array(buf, 0, akey, 16);
    _mav_put_char_array(buf, 16, asec, 65);
    _mav_put_char_array(buf, 81, devid, 9);
    _mav_put_char_array(buf, 90, type, 2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACCOUNT, buf, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
#else
    mavlink_qxsdk_account_t *packet = (mavlink_qxsdk_account_t *)msgbuf;
    packet->sdkserver = sdkserver;
    mav_array_memcpy(packet->akey, akey, sizeof(char)*16);
    mav_array_memcpy(packet->asec, asec, sizeof(char)*65);
    mav_array_memcpy(packet->devid, devid, sizeof(char)*9);
    mav_array_memcpy(packet->type, type, sizeof(char)*2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_QXSDK_ACCOUNT, (const char *)packet, MAVLINK_MSG_ID_QXSDK_ACCOUNT_MIN_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN, MAVLINK_MSG_ID_QXSDK_ACCOUNT_CRC);
#endif
}
#endif

#endif

// MESSAGE QXSDK_ACCOUNT UNPACKING


/**
 * @brief Get field akey from qxsdk_account message
 *
 * @return  qxsdk akey
 */
static inline uint16_t mavlink_msg_qxsdk_account_get_akey(const mavlink_message_t* msg, char *akey)
{
    return _MAV_RETURN_char_array(msg, akey, 16,  0);
}

/**
 * @brief Get field asec from qxsdk_account message
 *
 * @return  qxsdk asec
 */
static inline uint16_t mavlink_msg_qxsdk_account_get_asec(const mavlink_message_t* msg, char *asec)
{
    return _MAV_RETURN_char_array(msg, asec, 65,  16);
}

/**
 * @brief Get field devid from qxsdk_account message
 *
 * @return  qxsdk devid
 */
static inline uint16_t mavlink_msg_qxsdk_account_get_devid(const mavlink_message_t* msg, char *devid)
{
    return _MAV_RETURN_char_array(msg, devid, 9,  81);
}

/**
 * @brief Get field type from qxsdk_account message
 *
 * @return  qxsdk type
 */
static inline uint16_t mavlink_msg_qxsdk_account_get_type(const mavlink_message_t* msg, char *type)
{
    return _MAV_RETURN_char_array(msg, type, 2,  90);
}

/**
 * @brief Get field sdkserver from qxsdk_account message
 *
 * @return  qxsdk sdkserver
 */
static inline uint8_t mavlink_msg_qxsdk_account_get_sdkserver(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  92);
}

/**
 * @brief Decode a qxsdk_account message into a struct
 *
 * @param msg The message to decode
 * @param qxsdk_account C-struct to decode the message contents into
 */
static inline void mavlink_msg_qxsdk_account_decode(const mavlink_message_t* msg, mavlink_qxsdk_account_t* qxsdk_account)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_qxsdk_account_get_akey(msg, qxsdk_account->akey);
    mavlink_msg_qxsdk_account_get_asec(msg, qxsdk_account->asec);
    mavlink_msg_qxsdk_account_get_devid(msg, qxsdk_account->devid);
    mavlink_msg_qxsdk_account_get_type(msg, qxsdk_account->type);
    qxsdk_account->sdkserver = mavlink_msg_qxsdk_account_get_sdkserver(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN? msg->len : MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN;
        memset(qxsdk_account, 0, MAVLINK_MSG_ID_QXSDK_ACCOUNT_LEN);
    memcpy(qxsdk_account, _MAV_PAYLOAD(msg), len);
#endif
}
