#include "VGVehicle.h"
#include <QDebug>
#include <QtMath>
#include <QtEndian>
#include "VGApplication.h"
#include "base.h"
#include "MissionManager.h"
#include "VGMacro.h"
#include "VGOutline.h"
#include "VGVehicleMission.h"
#include "VGGlobalFun.h"
#include "VGMavLinkCode.h"
#include "VGQXManager.h"
#include "SerialLink.h"
#include "MAVLinkProtocol.h"
#include "LinkManager.h"

#define PARAM_UAV_ID "SYS_UAV_ID"
#define PARAM_MAXSPEED_ID "MAX_SPEED_ID"
#define PARAM_MAXACC_ID "MAX_ACC_ID"
#define MaxRetry 3
////////////////////////////////////////////////////////////////////////////////////////
//VGVehicle
////////////////////////////////////////////////////////////////////////////////////////
VGVehicle::VGVehicle(QObject *parent) : QObject(parent), m_sysID(0), m_countTimer(0)
, m_timerId(-1), m_vehicleType(MAV_TYPE_QUADROTOR), m_onboardTimeOffset(0)
, m_nSatellites(0), m_connectionLostEnabled(true), m_autoDisconnect(true)
, m_connectionLost(true), m_altitude(0), m_altitudeRelative(0), m_firewareType(MAV_AUTOPILOT_PX4)
, m_bArm(false), m_bMissionItemInt(false), m_voltage(0), m_mgrMission(new MissionManager(this))
, m_homePosition(0.0, 0.0), m_mavCommandRetryCount(0), m_countNoGPos(0)
, m_tmLastRtcm(QDateTime::currentMSecsSinceEpoch())
{
    m_timerId = startTimer(500);
    connect(m_mgrMission, &MissionManager::sendComplete, this, &VGVehicle::_onMissionSych);
}

VGVehicle::VGVehicle(LinkInterface *link, int vehicleId, MAV_AUTOPILOT firmwareType, MAV_TYPE vehicleType)
: QObject(NULL), m_countTimer(0), m_timerId(-1), m_sysID(vehicleId), m_vehicleType(vehicleType)
, m_onboardTimeOffset(0), m_nSatellites(0), m_connectionLostEnabled(true), m_autoDisconnect(true)
, m_connectionLost(true), m_altitude(0), m_altitudeRelative(0), m_firewareType(firmwareType)
, m_bMissionItemInt(true), m_mgrMission(new MissionManager(this)), m_voltage(0), m_homePosition(0.0, 0.0)
, m_bArm(false), m_countNoGPos(0), m_mavCommandRetryCount(0)
{
    _addLink(link);
    m_timerId = startTimer(500);

    connect(m_mgrMission, &MissionManager::sendComplete, this, &VGVehicle::_onMissionSych);
    connect(m_mgrMission, &MissionManager::error,        this, &VGVehicle::onMissionError);
    connect(this, &VGVehicle::_sendMessageOnLinkOnThread, this, &VGVehicle::_sendMessageOnLink, Qt::QueuedConnection);

    // Connection Lost time
    m_connectionLostTimer.setInterval(3000);
    m_connectionLostTimer.setSingleShot(true);
    connect(&m_connectionLostTimer, &QTimer::timeout, this, &VGVehicle::_connectionLostTimeout);
}

VGVehicle::~VGVehicle()
{
    delete m_mgrMission;
}

int VGVehicle::sysID() const
{
    return m_sysID;
}

bool VGVehicle::_containsLink(LinkInterface* link)
{
    return m_links.contains(link);
}

void VGVehicle::_addLink(LinkInterface* link)
{
    if (!_containsLink(link))
    {
        m_links.append(link);
        connect(link, &LinkInterface::linkConnected, this, &VGVehicle::_linkConnected, Qt::UniqueConnection);
    }
}

void VGVehicle::_linkConnected(bool b)
{
    auto link = (LinkInterface*)sender();
    if (b || !link || m_links.count() == 0)
        return;

    m_links.removeOne(link);
    if (m_links.count() == 0)
        qDebug() << "All links inactive";
}

void VGVehicle::_mavlinkMessageReceived(LinkInterface *link, const mavlink_message_t &message)
{
    if (message.sysid != m_sysID && message.sysid != 0) 
        return;

    if (!_containsLink(link))
        _addLink(link);

    m_connectionLostTimer.stop();
    m_connectionLostTimer.start();

    if (m_connectionLost)
    {
        m_connectionLost = false;
        emit connectionLostChanged(m_connectionLost);
        emit sysAvalibleChanged(isMissionIdle());
    }

    switch (message.msgid)
    {
    case MAVLINK_MSG_ID_HEARTBEAT: //心跳
        _handleHeartbeat(message); return;
    case MAVLINK_MSG_ID_SYS_STATUS: //系统状态
        _handleSysStatus(message); break;
    case MAVLINK_MSG_ID_GPS_RAW_INT:
        _handleGpsSatellite(message); break;
    case MAVLINK_MSG_ID_GPS_STATUS: //GPS状态
        _handleGpsStatus(message); break;
    case MAVLINK_MSG_ID_ATTITUDE_QUATERNION://姿态
        _handleAttitudeQuaternion(message); break;
    case MAVLINK_MSG_ID_LOCAL_POSITION_NED://NED坐标系
        break;
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: //经纬度等；
        _handleGlobalPositon(message); break;
    case MAVLINK_MSG_ID_ATTITUDE://姿态
    case MAVLINK_MSG_ID_RC_CHANNELS:
    case MAVLINK_MSG_ID_VIGA_EVENT:
    case MAVLINK_MSG_ID_HOME_POSITION://home點
    case MAVLINK_MSG_ID_QX_ACCOUNT:
    case MAVLINK_MSG_ID_QX_ACC_CMD:
    case MAVLINK_MSG_ID_QXSDK_ACCOUNT:
    case MAVLINK_MSG_ID_OBSTACLE_INFO:
    case MAVLINK_MSG_ID_ASSIST_POSITION:
    case MAVLINK_MSG_ID_MISSION_CURRENT:
        emit recvMavlink(this, message); break;
    case MAVLINK_MSG_ID_VFR_HUD://HUD速率等
        _handleVfrHud(message); break;
    case MAVLINK_MSG_ID_ATTITUDE_TARGET:
        break;
    case MAVLINK_MSG_ID_BATTERY_STATUS://电池
        _handleBattery(message); break;
    case MAVLINK_MSG_ID_ALTITUDE://高度
        _handleAltitude(message); break;
	case MAVLINK_MSG_ID_AUTOPILOT_VERSION:
		_handleAutopilotVersion(link, message); break;
    case MAVLINK_MSG_ID_WIND_COV://风速等
        break;
    case MAVLINK_MSG_ID_COMMAND_ACK:
        _handleCommandAck(message); break;
    case MAVLINK_MSG_ID_LOG_ENTRY:  //接收日志文件信息
        _handleLogEntry(message); break;
    case MAVLINK_MSG_ID_LOG_DATA:   //接收日志数据；
        _handleLogData(message); break;
    case MAVLINK_MSG_ID_PARAM_VALUE:
        _handleParameter(message); break;
    case MAVLINK_MSG_ID_SPRAY_VALUE:
        _handleSpray(message); break;
    case MAVLINK_MSG_ID_INTERRUPT_POINT:
        _handleInterrupt(message); break;
    default:
        break;
    }
    if (m_mgrMission)
        m_mgrMission->handleMessage(message);
}

void VGVehicle::_sendMessageOnLink(LinkInterface* link, const mavlink_message_t &message)
{
    // Make sure this is still a good link
    if (!link || !m_links.contains(link) || !link->isConnected())
        return;

    // Write message into buffer, prepending start sign
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    int len = mavlink_msg_to_send_buffer(buffer, &message);
    link->writeBytes(QByteArray((const char*)buffer, len));
}

void VGVehicle::_sendMessage(const mavlink_message_t &message)
{
    // Emit message on all links that are currently connected
    for (LinkInterface* link : m_links)
    {
        if (link->isConnected())
        {
            _sendMessageOnLink(link, message);
        }
    }
}

int VGVehicle::firewareType() const
{
    return m_firewareType;
}

MAV_TYPE VGVehicle::vehicleType() const
{
    return m_vehicleType;
}

void VGVehicle::sendMessage(const mavlink_message_t &message)
{
    sendMessageOnLink(priorityLink(), message);
}

void VGVehicle::sendRTCM(const QList<uint16_t> idxs, const QList<QByteArray> &rtcms)
{
    qint64 tm = QDateTime::currentMSecsSinceEpoch();
    if (idxs.size()!=rtcms.size() || (tm < m_tmLastRtcm+1000 && m_tmLastRtcm<tm) || !priorityLink())
        return;

    int flag = 0;
    QList<uint16_t>::const_iterator itr1 = idxs.begin();
    QList<QByteArray>::const_iterator itr2 = rtcms.begin();
    for (; itr1 != idxs.end(); ++itr1, ++itr2)
    {
        mavlink_message_t msg = { 0 };
        VGMavLinkCode::EncodeRTCM(msg, *itr2, flag++, qvgApp->mavLink(), priorityLink()->getMavlinkChannel(), sysID());
        sendMessageOnLink(priorityLink(), msg);
    }

    if (flag > 0)
        m_tmLastRtcm = tm;
}

bool VGVehicle::sendMessageOnLink(LinkInterface* link, mavlink_message_t message)
{
    if (!link || !m_links.contains(link) || !link->isConnected())
        return false;

    emit _sendMessageOnLinkOnThread(link, message);
    return true;
}

/**
* Check if time is smaller than 40 years, assuming no system without Unix
* timestamp runs longer than 40 years continuously without reboot. In worst case
* this will add/subtract the communication delay between GCS and MAV, it will
* never alter the timestamp in a safety critical way.
*/
quint64 VGVehicle::getUnixReferenceTime(quint64 time)
{
    // Same as getUnixTime, but does not react to attitudeStamped mode
    if (time == 0)
        return BASE::groundTimeMilliseconds();

#ifndef _MSC_VER
    else if (time < 1261440000000000LLU)
#else
    else if (time < 1261440000000000)
#endif
    {
        if (m_onboardTimeOffset == 0)
        {
            m_onboardTimeOffset = BASE::groundTimeMilliseconds() - time/1000;
        }
        return time/1000 + m_onboardTimeOffset;
    }
    else
    {
        // Time is not zero and larger than 40 years -> has to be
        // a Unix epoch timestamp. Do nothing.
        return time/1000;
    }
}

void VGVehicle::_handleHeartbeat(const mavlink_message_t &message)
{
    if (MAVTYPESURVEY == m_vehicleType)
        return;

    if (m_uavID.isEmpty())
        prcsVehicleParameter(PARAM_UAV_ID);

    mavlink_heartbeat_t heartbeat;
	mavlink_msg_heartbeat_decode(&message, &heartbeat);
	if (heartbeat.base_mode != m_baseMode || heartbeat.custom_mode != m_customMode)
    {
		m_baseMode = heartbeat.base_mode;
		if (m_customMode != heartbeat.custom_mode)
        {
            m_customMode = heartbeat.custom_mode;
		    emit flightModeChanged(flightMode());
        }
    }
    m_bArm = MAV_STATE_ACTIVE == (MAV_STATE_ACTIVE & heartbeat.system_status);
    emit sysStatus(heartbeat.system_status);
    if (heartbeat.system_status & 0x20)
        loadFromVehicle(MAV_MISSION_TYPE_AOBPATH);
}

void VGVehicle::setAutoDisconnect(bool autoDisconnect)
{
    m_autoDisconnect = autoDisconnect;
}

void VGVehicle::_connectionLostTimeout(void)
{
    if (m_connectionLostEnabled && !m_connectionLost)
    {
        m_connectionLost = true;
        qDebug() << "_connectionLostTimeout";
        if (m_autoDisconnect)
            disconnectInactiveVehicle();
      
        emit connectionLostChanged(m_connectionLost);
        emit sysAvalibleChanged(false);
        m_countTimer = 0;
    }
}

void VGVehicle::disconnectInactiveVehicle(void)
{
    LinkManager* linkMgr = qvgApp->linkManager();
    if (!linkMgr)
        return;

    for (LinkInterface *link : m_links)
    {
        if (link)
        {
            linkMgr->DeleteLink(link); 
            linkMgr->setConnectUAVStatus(false);
        }
    }
    m_links.clear();
    m_uavID = QString();
}

QString VGVehicle::uavid() const
{
    return m_uavID;
}

void VGVehicle::SetFlightMode(const QString &fm)
{
    mavlink_message_t msg;
	if (VGMavLinkCode::EncodeSetMode(msg, fm, m_baseMode, qvgApp->mavLink(), priorityLink()->getMavlinkChannel(), sysID()))
		sendMessageOnLink(priorityLink(), msg);
}

QStringList VGVehicle::GetFlightModes()
{
	return VGMavLinkCode::SurpportModes();
}

bool VGVehicle::supportsMissionItemInt() const
{
	return m_bMissionItemInt;
}

void VGVehicle::_onMissionSych(bool bError)
{
    if (sender())
        emit sychMissionFinish(!bError);

    emit sysAvalibleChanged(isMissionIdle());
    if (m_mgrMission && m_mgrMission->isWriteIdle() && m_pfInfos.count())
    {
        PairMission ms = m_pfInfos.takeFirst();

        if (!ms.second)
            m_mgrMission->writeMissionItems(ms.first->MissionItems());
        else if (ms.first->CountBlock() > 0)
            m_mgrMission->writeMissionItems(ms.first->BoundaryItems());
        else if (m_mgrMission)
            m_mgrMission->removeAllWorker(MAV_MISSION_TYPE_OBSTACLE);
    }
}

bool VGVehicle::_contains(VGVehicleMission *ms, bool bBdr)
{
    return m_pfInfos.contains(PairMission(ms, bBdr));
}

QGeoCoordinate VGVehicle::_vehiclePostion() const
{
    return QGeoCoordinate(m_latitude, m_longitude, m_altitude);
}

void VGVehicle::_sendMavCommandAgain()
{
    if (!m_mavCommandQueue.size())
        return;

    MavCommandQueueEntry_t& queuedCommand = m_mavCommandQueue[0];
    if (m_mavCommandRetryCount++ > MaxRetry)
    {
        emit mavCommandResult(queuedCommand.command, false);
        m_mavCommandQueue.removeFirst();
        _sendMavCommandAgain();
        return;
    }
    if (!qvgApp->mavLink() || !priorityLink())
        return;

    mavlink_message_t       msg;
    float *p = queuedCommand.rgParam;
    if (queuedCommand.bInt)
        VGMavLinkCode::EncodeCommandInt(msg, queuedCommand.component, queuedCommand.command,
            qvgApp->mavLink(), priorityLink()->getMavlinkChannel(), m_sysID,
            p[0], p[1], p[2], p[3], *(int*)(p+4), *(int*)(p+5), p[6]);
    else
        VGMavLinkCode::EncodeCommands(msg, queuedCommand.component, queuedCommand.command,
            qvgApp->mavLink(), priorityLink()->getMavlinkChannel(), m_sysID,
            p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
    sendMessageOnLink(priorityLink(), msg);
}

void VGVehicle::onMissionError(int errorCode, const QString &str)
{
    QString strErr;
    switch (errorCode)
    {
    case MissionManager::InternalError:
        strErr = tr("Internal error");//"内部错误!"
        break;
    case MissionManager::AckTimeoutError:
        strErr = tr("Device respond timeout");//"等待设备回应超时"
        break;
    case MissionManager::ProtocolOrderError:
        strErr = tr("Request order of mission item error");//"任务点请求序列错误，
        break;
    case MissionManager::RequestRangeError:
        strErr = tr("Request mission item out of range");//"请求范围错误，"
        break;
    case MissionManager::ItemMismatchError:
        strErr = tr("Item not fit");//"任务点不匹配，"
        break;
    case MissionManager::VehicleError:
        strErr = tr("Respond of device timeout");//"等待设备回应超时，"
        break;
    case MissionManager::MissingRequestsError:
        strErr = tr("Mission request error");//"任务请求错误，"
        break;
    case MissionManager::MaxRetryExceeded:
    default:
        break;
    }
    strErr += str;
    qvgApp->SetQmlTip(strErr, true);
}

void VGVehicle::onMissionDestroed(QObject *obj)
{
    int i = 0;
    for (const PairMission &itr : m_pfInfos)
    {
        if (obj == itr.first)
            m_pfInfos.takeAt(i);
        ++i;
    }
}

bool VGVehicle::syncPlanFlight(VGVehicleMission *ms, bool bBd)
{
    if (!ms || _contains(ms, bBd))
        return false;

    m_pfInfos << PairMission(ms, bBd);
    connect(ms, &QObject::destroyed, this, &VGVehicle::onMissionDestroed, Qt::UniqueConnection);
    _onMissionSych(true);
    return true;
}

void VGVehicle::setQxAccount(const QString &acc, const QString &pswd)
{
    mavlink_message_t msg = { 0 };
    VGMavLinkCode::EncodeQXAccount(msg, acc, pswd, qvgApp->mavLink(), priorityLink()->getMavlinkChannel());
    sendMessageOnLink(priorityLink(), msg);
}

void VGVehicle::setQxSdkAccount(const QString &key, const QString &secret, const QString &id, const QString &type)
{
    mavlink_message_t msg = { 0 };
    VGMavLinkCode::EncodeQXSdkAccount(msg, key, secret, id, type, qvgApp->mavLink(), priorityLink()->getMavlinkChannel());
    sendMessageOnLink(priorityLink(), msg);
}

bool VGVehicle::isMissionIdle() const
{
    return m_mgrMission && m_pfInfos.isEmpty();
}

void VGVehicle::timerEvent(QTimerEvent *evt)
{
    if (m_timerId != evt->timerId())
        return QObject::timerEvent(evt);

    ++m_countTimer;
    if (m_countTimer % 5 == 0 && m_mavCommandQueue.size()>0)
        _sendMavCommandAgain();
    else if (m_mavParameterQue.size() > 0)
        _prcsParameters();

    if (m_countNoGPos < 20)
        ++m_countNoGPos;
}

void VGVehicle::_prcsParameters()
{
    LinkInterface *link = priorityLink();
    if (!link || m_mavParameterQue.size()<1)
        return;

    PairParameter p = m_mavParameterQue.takeFirst();
    if (p.second.second >= MaxRetry)
    {
        _removeParam(p.first);
        return;
    }

    p.second.second++;
    mavlink_message_t msg;
    if (p.second.first.isValid())
    {
        if (VGMavLinkCode::EncodeSetParameter(msg, p.first, p.second.first, qvgApp->mavLink(), link->getMavlinkChannel(), sysID()))
            sendMessageOnLink(link, msg);
    }
    else if (VGMavLinkCode::EncodeGetParameter(msg, p.first, qvgApp->mavLink(), BASE::defaultComponentId, sysID()))
    {
        sendMessageOnLink(priorityLink(), msg);
    }
    m_mavParameterQue.append(p);
}

void VGVehicle::_removeParam(const QString &key)
{
    for (auto itr = m_mavParameterQue.begin(); itr != m_mavParameterQue.end(); ++itr)
    {
        if (itr->first == key)
        {
            m_mavParameterQue.erase(itr);
            _prcsParameters();
            break;
        }
    }
}

void VGVehicle::sendMavCommand(MAV_CMD command, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
    MavCommandQueueEntry_t entry;

    entry.component = qvgApp->mavLink()->getComponentId();
    entry.command = command;
    entry.bInt = false;
    entry.rgParam[0] = param1;
    entry.rgParam[1] = param2;
    entry.rgParam[2] = param3;
    entry.rgParam[3] = param4;
    entry.rgParam[4] = param5;
    entry.rgParam[5] = param6;
    entry.rgParam[6] = param7;

    m_mavCommandQueue.append(entry);

    if (m_mavCommandQueue.count() == 1){
        m_mavCommandRetryCount = 0;
        _sendMavCommandAgain();
    }
}

void VGVehicle::sendMavCommandInt(MAV_CMD command, float param1, float param2, float param3, float param4, int param5, int param6, float param7)
{
    MavCommandQueueEntry_t entry;

    entry.component = qvgApp->mavLink()->getComponentId();
    entry.command = command;
    entry.bInt = true;
    entry.rgParam[0] = param1;
    entry.rgParam[1] = param2;
    entry.rgParam[2] = param3;
    entry.rgParam[3] = param4;
    *(int*)(entry.rgParam + 4) = param5;
    *(int*)(entry.rgParam + 5) = param6;
    entry.rgParam[6] = param7;

    m_mavCommandQueue.append(entry);

    if (m_mavCommandQueue.count() == 1) {
        m_mavCommandRetryCount = 0;
        _sendMavCommandAgain();
    }
}

void VGVehicle::loadFromVehicle(MAV_MISSION_TYPE tp)
{
    if (m_mgrMission)
        m_mgrMission->loadFromVehicle(tp);
}

const QList<MissionItem*> & VGVehicle::missionItems() const
{
    return m_mgrMission->missionItems();
}

bool VGVehicle::hasValidMissionItems() const
{
    return m_mgrMission->hasValidMissionItems();
}

MissionManager * VGVehicle::missionManager() const
{
    return m_mgrMission;
}

void VGVehicle::setArmed(bool armed)
{
    if (!qvgApp->mavLink())
        return;

    sendMavCommand(MAV_CMD_COMPONENT_ARM_DISARM,
        true,    // show error if fails
        armed ? 1.0f : 0.0f);
}

LinkInterface* VGVehicle::priorityLink(void)
{
#ifndef __ios__
    foreach (LinkInterface* link, m_links) {
        if (link->isConnected())
		{
			if (link->linkType()== LinkCommand::TypeSerial)
			{
				if (SerialCommand *cmd = qobject_cast<SerialCommand*>(link->getLinkCommand()))
				{
                    if (cmd->usbDirect())
                        return link;
                }
            }
#ifdef QGC_ENABLE_BLUETOOTH
			else if (link->linkType() == LinkCommand::TypeBluetooth)
			{
				return link;
			}
#endif
        }
    }
#endif
    return m_links.count() ? m_links[0] : NULL;
}

void VGVehicle::sltRequestLogData(int logId, long offset, long count)
{
    mavlink_message_t msg;
    mavlink_msg_log_request_data_pack(
        qvgApp->mavLink()->getSystemId(),
        qvgApp->mavLink()->getComponentId(),
        &msg,
        sysID(),
        BASE::defaultComponentId,
        logId, offset, count);

    sendMessageOnLink(this->priorityLink(), msg);
}

void VGVehicle::setConnectionLostEnabled(bool connectionLostEnabled)
{
    if (m_connectionLostEnabled != connectionLostEnabled) {
        m_connectionLostEnabled = connectionLostEnabled;
    }
}

void VGVehicle::sltRequestLogList(int start, int end)
{
    mavlink_message_t msg;
    mavlink_msg_log_request_list_pack(
                qvgApp->mavLink()->getSystemId(),
                qvgApp->mavLink()->getComponentId(),
                &msg,
                sysID(),
                BASE::defaultComponentId,
                start,
                end);
    sendMessageOnLink(priorityLink(), msg);
}

void VGVehicle::prcsVehicleParameter(const QString &strParam, const QVariant &param)
{
    bool bHas = false;
    for (auto itr = m_mavParameterQue.begin(); itr!= m_mavParameterQue.end(); ++itr)
    {
        if (itr->first == strParam)
        {
            bHas = true;
            if (param.isValid())
            {
                m_mavParameterQue.back().second.first = param;
                m_mavParameterQue.back().second.second = 0;
            }
            break;
        }
    }

    if (!bHas)
        m_mavParameterQue << PairParameter(strParam, ParamVPair(param, 0));

	LinkInterface *link = priorityLink();
    if (!link || m_mavParameterQue.size()>1)
        return;

    _prcsParameters();
}

QString VGVehicle::flightMode() const
{
	return VGMavLinkCode::GetFlightModeName(m_customMode);
}

void VGVehicle::_handleAttitudeQuaternion(const mavlink_message_t& message)
{
    mavlink_attitude_quaternion_t attitude;
    mavlink_msg_attitude_quaternion_decode(&message, &attitude);
}

void VGVehicle::_handleAutopilotVersion(LinkInterface*, const mavlink_message_t& message)
{
	mavlink_autopilot_version_t autopilotVersion;
	mavlink_msg_autopilot_version_decode(&message, &autopilotVersion);

	if (autopilotVersion.capabilities & MAV_PROTOCOL_CAPABILITY_MISSION_INT)
		m_bMissionItemInt = true;
}

void VGVehicle::_handleGlobalPositon(const mavlink_message_t& message)
{
    QGeoCoordinate newPosition;
    double groundSpeed = 0;
    double verSpeed = 0;
    if (VGMavLinkCode::DecodeGlobalPos(message, newPosition, groundSpeed, verSpeed, m_altitude))
    {
        m_latitude = newPosition.latitude();
        m_longitude = newPosition.longitude();

        if (m_bArm)
            m_altitudeRelative = newPosition.altitude();

        emit postionChanged(m_latitude, m_longitude, m_altitudeRelative, m_nSatellites);
        emit altitudeChanged(this, m_altitude);
        emit speedChanged(this, groundSpeed, verSpeed);
        m_countNoGPos = 0;
    }
}

void VGVehicle::_handleSysStatus(const mavlink_message_t& message)
{
    mavlink_sys_status_t sysStatus;
    mavlink_msg_sys_status_decode(&message, &sysStatus);

    int nBatterry = sysStatus.battery_remaining;
    m_voltage = sysStatus.voltage_battery;
    emit batterryStatus(this, nBatterry, m_voltage, sysStatus.errors_count1);
}

void VGVehicle::_handleGpsSatellite(const mavlink_message_t& message)
{
    QGeoCoordinate newPosition;
    uint8_t fix;
    double pre;
	uint16_t cnt;
    if (VGMavLinkCode::DecodeGpsSatlate(message, newPosition, m_nSatellites, fix, pre, cnt))
    {
        if (MAVTYPESURVEY == vehicleType())
        {
            m_latitude = newPosition.latitude();
            m_longitude = newPosition.longitude();
            m_altitude = newPosition.altitude();
            emit altitudeChanged(this, m_altitude);
        }
		emit vehicleCog(cnt);
        emit precisionChanged(this, pre);
        emit posTypeChanged(this, fix);
        emit postionChanged(m_latitude, m_longitude, m_altitudeRelative, m_nSatellites);
    }
}

void VGVehicle::_handleGpsStatus(const mavlink_message_t& message)
{
    mavlink_gps_status_t pos;
    mavlink_msg_gps_status_decode(&message, &pos);

    m_nSatellites = pos.satellites_visible;
    emit postionChanged(m_latitude, m_longitude, m_altitudeRelative, m_nSatellites);
}

void VGVehicle::_handleAltitude(const mavlink_message_t& message)
{
    mavlink_altitude_t alt_status;
    mavlink_msg_altitude_decode(&message, &alt_status);
    if (m_countNoGPos > 8)
    {
        if (!m_bArm)
            m_altitudeRelative = alt_status.altitude_local;

        if (m_nSatellites)
        {
            m_altitude = alt_status.altitude_amsl;
            emit altitudeChanged(this, m_altitude);
        }
        emit postionChanged(m_latitude, m_longitude, m_altitudeRelative, m_nSatellites);
    }
    emit gndHeightChanged(this, alt_status.bottom_clearance);
}

void VGVehicle::_handleVfrHud(const mavlink_message_t& message)
{
    mavlink_vfr_hud_t hud;
    mavlink_msg_vfr_hud_decode(&message, &hud);

    m_altitude = hud.alt;
    emit speedChanged(this, hud.groundspeed, hud.climb);
    emit altitudeChanged(this, m_altitude);
}

void VGVehicle::_handleBattery(const mavlink_message_t& message)
{
    mavlink_battery_status_t bat_status;
    mavlink_msg_battery_status_decode(&message, &bat_status);

    int nBatterry = bat_status.battery_remaining;
    emit batterryStatus(this, nBatterry, m_voltage, 255);
}

void VGVehicle::_handleLogEntry(const mavlink_message_t& message)
{
    mavlink_log_entry_t log;
    mavlink_msg_log_entry_decode(&message, &log);
    qDebug() << "MAVLINK_MSG_ID_LOG_ENTRY: " << log.size << log.id << log.num_logs;
    emit updateLogList(this, log.time_utc, log.size, log.id, log.num_logs);
}

void VGVehicle::_handleLogData(const mavlink_message_t& message)
{
    mavlink_log_data_t log;
    mavlink_msg_log_data_decode(&message, &log);
    emit receivedLogData(this, log.ofs, log.id, log.count, log.data);
}

void VGVehicle::_handleParameter(const mavlink_message_t& message)
{
    QVariant val;
    QString key = VGMavLinkCode::DecodeParameter(message, val);
    if (!key.isEmpty())
    {
        if (key == PARAM_UAV_ID)
        {
            QString strUavid = QString("VIGAU:%1").arg(val.toUInt(), 8, 16, QChar('0')).toUpper();
            if (m_uavID != strUavid)
            {
                m_uavID = strUavid;
                emit vehicleUavIdChanged(this, m_uavID);
            }
        }
        emit recvMavlink(this, message);
    }
    _removeParam(key);
}

void VGVehicle::_handleCommandAck(const mavlink_message_t& message)
{
    uint16_t cmd = 0;
    MAV_RESULT result = MAV_RESULT_ACCEPTED;
    if (VGMavLinkCode::DecodeCommandAck(message, cmd, result))
    {
        bool bSuc = result == MAV_RESULT_ACCEPTED;
        emit mavCommandResult((MAV_CMD)cmd, bSuc);
        _sendMavCommandAgain();
    }
}

void VGVehicle::_handleSpray(const mavlink_message_t& message)
{
    double speed;
    double vol;
    uint8_t stat;
    uint8_t mode;
    if (VGMavLinkCode::DecodeSpray(message, speed, vol, stat, mode))
        emit sprayGot(this, speed, vol, stat, mode);
}

void VGVehicle::_handleInterrupt(const mavlink_message_t& message)
{
    mavlink_interrupt_point_t suspend;
    mavlink_msg_interrupt_point_decode(&message, &suspend);
    QGeoCoordinate c(suspend.int_latitude / 1e7, suspend.int_longitude / 1e7, suspend.int_altitude / 1e3);
    emit recvSuspend(c, suspend.int_type);
    if (auto p = qvgApp->mavLink())
    {
        mavlink_message_t msg;
        mavlink_msg_interrupt_point_ack_pack_chan(p->getSystemId(), p->getComponentId(), sysID(), &msg, suspend.int_type);
        emit sendMessageOnLink(priorityLink(), msg);
    }
}
