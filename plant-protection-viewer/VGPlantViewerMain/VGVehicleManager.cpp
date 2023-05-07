#include "VGVehicleManager.h"
#include <QDebug>
#include <QtMath>
#include <math.h>
#include "VGApplication.h"
#include "MissionManager.h"
#include "VGLandManager.h"
#include "VGSurveyMonitor.h"
#include "VGPlantManager.h"
#include "VGGlobalFun.h"
#include "VGMacro.h"
#include "VGPlantEvents.h"
#include "MAVLinkProtocol.h"
#include "VGController.h"
#include "VGMavLinkCode.h"
#include <QtCore/private/qnumeric_p.h>

VGVehicleManager::VGVehicleManager(QObject *parent) : QObject(parent)
  , _activeVehicle(NULL)
  , _activeVehicleAvailable(false)
  , _vehicleBeingSetActive(NULL)
  , _vgsHeartbeatEnabled(true)
  , m_nBatteryAlarm(0)
  , m_bAlarmFirst(true)
{
    if (QSettings *settings = qvgApp->GetSettings())
    {
        settings->beginGroup("config");
        m_nBatteryAlarm = settings->value("batteryAlarm", 30.0).toDouble();
        settings->endGroup();
    }
    _vgsHeartbeatTimer.setInterval(VGSHeartbeatRateMSecs);
    _vgsHeartbeatTimer.setSingleShot(false);
    connect(&_vgsHeartbeatTimer, &QTimer::timeout, this, &VGVehicleManager::sltSendVGHeartbeat);
    if (_vgsHeartbeatEnabled) {
        _vgsHeartbeatTimer.start();
    }
}

void VGVehicleManager::onHeartbeat(LinkInterface *link, int vehicleId, int, int vehicleFirmwareType, int vehicleType)
{
    bool bVehivle = vehicleType != MAVTYPESURVEY;
    if (auto v = getVehicleById(vehicleId))
    {
        if (bVehivle)
            setActiveVehicle(v);
        return;
    }

    VGVehicle* vehicle = new VGVehicle(link, vehicleId, (MAV_AUTOPILOT)vehicleFirmwareType, (MAV_TYPE)vehicleType);
    vehicle->setAutoDisconnect(true);
    connect(this, &VGVehicleManager::sigMavlinkMessageReceived, vehicle, &VGVehicle::_mavlinkMessageReceived);
    qRegisterMetaType<VGVehicle*>("VGVehicle");
    connect(vehicle, &VGVehicle::altitudeChanged, this, &VGVehicleManager::onVehicelAltitudeChanged);
    connect(vehicle, &VGVehicle::gndHeightChanged, this, &VGVehicleManager::onGndHeightChanged);
    connect(vehicle, &VGVehicle::speedChanged, this, &VGVehicleManager::onVehicelSpeedChanged);
    connect(vehicle, &VGVehicle::postionChanged, this, &VGVehicleManager::onVehicelGpsChanged);
    connect(vehicle, &VGVehicle::posTypeChanged, this, &VGVehicleManager::onPosTypeChanged);
    connect(vehicle, &VGVehicle::precisionChanged, this, &VGVehicleManager::onPrecisionChanged);
    connect(vehicle, &VGVehicle::batterryStatus, this, &VGVehicleManager::onVehicelBatteryStatus);

    connect(vehicle, &VGVehicle::connectionLostChanged, this, &VGVehicleManager::onConnectionLostChanged);
    connect(vehicle, &VGVehicle::updateLogList, this, &VGVehicleManager::sltUpdateLogList);
    connect(vehicle, &VGVehicle::receivedLogData, this, &VGVehicleManager::sltReceiveLogData);
    connect(vehicle, &VGVehicle::vehicleUavIdChanged, this, &VGVehicleManager::onVehicleUavIdChanged);
    connect(vehicle, &VGVehicle::sysAvalibleChanged, this, &VGVehicleManager::onSysAvalibleChange);
    connect(vehicle, &VGVehicle::flightModeChanged, this, &VGVehicleManager::onModeChanged);
    connect(vehicle, &VGVehicle::mavCommandResult, this, &VGVehicleManager::onMavCommandResult);
	

    if (bVehivle)
    {
        MissionManager *mm = vehicle->missionManager();
        connect(mm, &MissionManager::newMissionItemsAvailable, this, &VGVehicleManager::onNewMissionItemsAvailable);
        connect(mm, &MissionManager::progressPct, this, &VGVehicleManager::onMissionPrcs);
        connect(mm, &MissionManager::sendComplete, this, &VGVehicleManager::onSyschFinish);
        connect(vehicle, &VGVehicle::sysStatus, this, &VGVehicleManager::onSysStatus);
        connect(vehicle, &VGVehicle::recvSuspend, this, &VGVehicleManager::onRecvSuspend);
        connect(vehicle, &VGVehicle::recvMavlink, this, &VGVehicleManager::OnRecvMavlink);
        connect(vehicle, &QObject::destroyed, this, &VGVehicleManager::onVehicleDestroyed);

        setActiveVehicle(vehicle);
    }
    else if (VGSurveyMonitor *mnt = qvgApp->surveyMonitor())
    {
        mnt->SetConnected(true, VGSurveyMonitor::Dev_CQCDQ);
    }

    _vehicles.append(vehicle);
    // Mark link as active
    link->SetActive(true);
}

VGVehicle* VGVehicleManager::getVehicleById(int vehicleId)
{
    foreach(VGVehicle* vehicle, _vehicles)
    {
        if (vehicle->sysID() == vehicleId)
            return vehicle;
    }

    return NULL;
}

VGVehicle *VGVehicleManager::activeVehicle() const
{
    return _activeVehicle;
}

VGVehicle *VGVehicleManager::surveyVehicle() const
{
    foreach(VGVehicle* vehicle, _vehicles)
    {
        if (vehicle->vehicleType() == MAVTYPESURVEY)//测绘
            return vehicle;
    }

    return NULL;
}

void VGVehicleManager::setActiveVehicle(VGVehicle* vehicle)
{
    if (vehicle != _activeVehicle)
    {
        m_bAlarmFirst = true;
        if (_activeVehicle) {
            //_activeVehicle->setActive(false);

            _activeVehicleAvailable = false;
            emit sigActiveVehicleAvailableChanged(false);
        }

        // See explanation in _deleteVehiclePhase1
        _vehicleBeingSetActive = vehicle;
        QTimer::singleShot(50, this, &VGVehicleManager::sltSetActiveVehiclePhase2);
    }
}

void VGVehicleManager::processSurveysMavLink(const mavlink_message_t &msg)
{
    if (msg.msgid == MAVLINK_MSG_ID_PARAM_VALUE)
    {
        QVariant v;
        QString id = VGMavLinkCode::DecodeParameter(msg, v);
        qvgApp->surveyMonitor()->DealParam(id, v);
    }
}

void VGVehicleManager::sltSetActiveVehiclePhase2(void)
{
    _activeVehicle = _vehicleBeingSetActive;
    emit sigActiveVehicleChanged(_activeVehicle);

    // And finally vehicle availability
    if (_activeVehicle->vehicleType() != MAVTYPESURVEY)
    {
        if (VGPlantManager *mgr = qvgApp->plantManager())
            mgr->ViaDrectPlantCreate(_activeVehicle->flightMode());

        _activeVehicleAvailable = true;
        emit sigActiveVehicleAvailableChanged(true);
        onVehicleUavIdChanged(_activeVehicle, _activeVehicle->uavid());
    } 
    else
    {
        if (VGSurveyMonitor *mgr = qvgApp->surveyMonitor())
            mgr->SetConnected(true, VGSurveyMonitor::Dev_CQCDQ);
    }
}

void VGVehicleManager::sltSendVGHeartbeat()
{
    for (VGVehicle* vehicle : _vehicles)
    {
        mavlink_message_t message;
        mavlink_msg_heartbeat_pack(qvgApp->mavLink()->getSystemId(),
                                   qvgApp->mavLink()->getComponentId(),
                                   &message,
                                   MAV_TYPE_GCS,            // MAV_TYPE
                                   MAV_AUTOPILOT_INVALID,   // MAV_AUTOPILOT
                                   MAV_MODE_MANUAL_ARMED,   // MAV_MODE
                                   0,                       // custom mode
                                   MAV_STATE_ACTIVE);       // MAV_STATE
        vehicle->sendMessage(message);
    }
}

void VGVehicleManager::onConnectionLostChanged(bool b)
{
    VGVehicle *v = qobject_cast<VGVehicle *>(sender());
    if (_activeVehicle == v && v)
    {
        qvgApp->SetQmlTip(b ? tr("Link timeout,communicate break") : tr("Device linked"), b);//"链接超时，断开通信"

        if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
            plant->SetStatus(!b ? VGPlantInformation::Connected : VGPlantInformation::UnConnect);

        if (b)
        {
            _activeVehicle = NULL;
            _activeVehicleAvailable = false;
            emit sigActiveVehicleAvailableChanged(false);
        }

    }
    else if (v == surveyVehicle())
    {
        if (VGSurveyMonitor *mgr = qvgApp->surveyMonitor())
            mgr->SetConnected(!b, VGSurveyMonitor::Dev_CQCDQ);
    }
}

void VGVehicleManager::onNewMissionItemsAvailable(bool, MAV_MISSION_TYPE tp)
{
    MissionManager *mm = qobject_cast<MissionManager*>(sender());

    if (!mm || _activeVehicle != mm->vehicle())
        return;

    if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
        plant->SetMissionValible(true, tp);
}

void VGVehicleManager::onSysAvalibleChange(bool)
{
}

void VGVehicleManager::onModeChanged(const QString &mode)
{
    if (_activeVehicle != qobject_cast<VGVehicle*>(sender()))
        return;

    if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
        plant->ChangeFlightMode(mode);
}

void VGVehicleManager::onMavCommandResult(MAV_CMD cmd, bool res)
{
    if (_activeVehicle == sender())
    {
        if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
            plant->PrcsCommandRes(cmd, res);
    }
}

void VGVehicleManager::onMissionPrcs(double par, bool bs)
{
    MissionManager *mm = qobject_cast<MissionManager*>(sender());
    if (mm && bs)
    {
        if (mm->IsMissonDown())
            qvgApp->SetQmlTip(tr("Download mission (%1%)").arg(QString::number(par * 100, 10, 1)));//下载设备航线(%1%)
        else
            qvgApp->SetQmlTip(tr("Synchronize mission (%1%)").arg(QString::number(par * 100, 10, 1)));//"同步航线到设备(%1%)"
    }
}

void VGVehicleManager::onVehicleUavIdChanged(VGVehicle *v, const QString &uavid)
{
    if (v == activeVehicle() && !uavid.isEmpty())
    {
        if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
            plant->setPlaneId(uavid);
    }
}

void VGVehicleManager::onSyschFinish(bool error)
{
    if (VGPlantManager *mgr = qvgApp->plantManager())
    {
        if (VGPlantInformation *plant = mgr->GetPlantViaDrect())
            plant->SychFinish(!error);
    }
}

void VGVehicleManager::onSysStatus(uint8_t st)
{
    if (sender() != _activeVehicle)
        return;

    VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect();
    if (plant)
    {
        plant->SetSysStatus(st);
        if (plant->getConnectedTime() > 1000)
            plant->setLastConnected();
    }
}

void VGVehicleManager::onRecvSuspend(const QGeoCoordinate &c, int type)
{
    if (sender() != _activeVehicle)
        return;

    if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
        plant->SetContinueCoor(c, type);
}

void VGVehicleManager::OnRecvMavlink(VGVehicle *v, const mavlink_message_t &msg)
{
    if (!v || v != _activeVehicle)
        return;

    if (v->vehicleType() == MAVTYPESURVEY)
        processSurveysMavLink(msg);
    else if (VGPlantInformation *plant = qvgApp->plantManager()->GetPlantViaDrect())
        qvgApp->plantManager()->prcsUavMavlink(plant, msg, true);
}

void VGVehicleManager::onVehicleDestroyed(QObject *v)
{
    for (VGVehicle* vehicle : _vehicles)
    {
        if (v == vehicle)
        {
            _vehicles.removeAll(vehicle);
            break;
        }
    }
}

void VGVehicleManager::onVehicelAltitudeChanged(VGVehicle *vehicle, double altitude)
{
    if (!vehicle)
        return;

    if (vehicle != _activeVehicle && vehicle->vehicleType() != MAVTYPESURVEY)
        return;

    if (altitude + 1 == altitude)
        altitude = 0.0;

    if (vehicle->vehicleType() == MAVTYPESURVEY)
    {
        if (VGSurveyMonitor *mnt = qvgApp->surveyMonitor())
            mnt->SetAltitude(altitude);
    }
    else
    {
        VGPlantManager *mgr = qvgApp->plantManager();
        if (VGPlantInformation *plant = mgr ? mgr->GetPlantViaDrect() : NULL)
            plant->SetAltitude(altitude);
    }
}

void VGVehicleManager::onGndHeightChanged(VGVehicle* vehicle, double height)
{
    if (!vehicle || vehicle != _activeVehicle)
        return;

    VGPlantManager *mgr = qvgApp->plantManager();
    if (VGPlantInformation *plant = mgr ? mgr->GetPlantViaDrect() : NULL)
        plant->SetGndHeight(height);
}

void VGVehicleManager::onVehicelGpsChanged(double lat, double lon, double alt, int nSatNum)
{
    VGVehicle *vehicle = qobject_cast<VGVehicle*>(sender());
    if (!vehicle)
        return;

    if (vehicle != _activeVehicle && vehicle->vehicleType() != MAVTYPESURVEY)
        return;

    //判断是否有效
    if (lat+1 ==lat)
        return;
    if (lon + 1 == lon)
        return;
    if (alt + 1 == alt)
        return;

    if (vehicle->vehicleType() == MAVTYPESURVEY)
    {
        if (VGSurveyMonitor *mnt = qvgApp->surveyMonitor())
        {
            mnt->SetSatNum(nSatNum);
            mnt->SetRelativeAltitude(alt);
            mnt->SetLatAndLon(lat, lon, vehicle->vehicleType() < 1);
        }
    }
    else
    {
        VGPlantManager *mgr = qvgApp->plantManager();
        if (VGPlantInformation *plant = mgr ? mgr->GetPlantViaDrect() : NULL)
        {
            plant->SetSatlateNumb(nSatNum);
            plant->setCoordinate(QGeoCoordinate(lat, lon));
            plant->SetRelativeAltitude(alt);
        }
    }
}

void VGVehicleManager::onPosTypeChanged(VGVehicle *vehicle, int fix)
{
    if (vehicle->vehicleType() == MAVTYPESURVEY)
    {
        if (VGSurveyMonitor *mnt = qvgApp->surveyMonitor())
            mnt->SetFixType(fix);
    }
    else if(VGPlantManager *mgr = qvgApp->plantManager())
    {
        if (VGPlantInformation *plant = mgr->GetPlantViaDrect())
            plant->SetFixType(fix);
    }
}

void VGVehicleManager::onVehicelSpeedChanged(VGVehicle *vehicle, double speedX, double speedY)
{
    if(vehicle != _activeVehicle)
        return;

    VGPlantManager *mgr = qvgApp->plantManager();
    if (VGPlantInformation *plant = mgr ? mgr->GetPlantViaDrect() : NULL)
    {
        plant->SetSpeedHor(speedX);
        plant->SetSpeedVer(speedY);
    }
}

void VGVehicleManager::onVehicelBatteryStatus(VGVehicle *vehicle, int batterry, uint16_t mv, uint8_t warn)
{
    if (!vehicle)
        return;

    if (vehicle != _activeVehicle && vehicle->vehicleType() != MAVTYPESURVEY)
        return;

    if (vehicle->vehicleType() == MAVTYPESURVEY)
    {
        if (VGSurveyMonitor *mnt = qvgApp->surveyMonitor())
            mnt->SetPowerPercent(batterry);
    }
    else if (VGPlantManager *mgr = qvgApp->plantManager())
    {
        if (VGPlantInformation *plant = mgr->GetPlantViaDrect())
        {
            plant->SetPowerPercent(batterry, batterry<=m_nBatteryAlarm && m_bAlarmFirst);
            if (batterry <= m_nBatteryAlarm)
                m_bAlarmFirst = false;

            if (warn<=16)
                plant->SetVoltErr(warn);

            plant->SetVoltage(mv);
        }
    }
}

void VGVehicleManager::onPrecisionChanged(VGVehicle* vehicle, double pre)
{
    if (vehicle->vehicleType() == MAVTYPESURVEY)
    {
        if (VGSurveyMonitor *mnt = qvgApp->surveyMonitor())
            mnt->SetPrecision(pre);
    }
    else if (VGPlantManager *mgr = qvgApp->plantManager())
    {
        if (VGPlantInformation *plant = mgr->GetPlantViaDrect())
            plant->SetPrecision(pre);
    }
}

void VGVehicleManager::sltUpdateLogList(VGVehicle *vehicle, uint32_t time_utc, uint32_t size, uint16_t id, uint16_t num_logs)
{
    if(vehicle != _activeVehicle)
        return;

    emit updateLogList(time_utc, size, id, num_logs, false);

    if(id == 49 && id < num_logs - 1)
    {
        _activeVehicle->sltRequestLogList(50, num_logs - 1);
    }
}

void VGVehicleManager::sltRequestLogData(int vehicleId, int logId, ulong offset, ulong count)
{
    if(_activeVehicle->sysID() == vehicleId)
    {
        _activeVehicle->setConnectionLostEnabled(false);
        _activeVehicle->sltRequestLogData(logId, offset, count);
    }
}

void VGVehicleManager::sltReceiveLogData(VGVehicle *vehicle, uint32_t ofs, uint16_t id, uint8_t count, const uint8_t *data)
{
    if(_activeVehicle != vehicle)
    {
        return;
    }

    emit receivedLogData(ofs, id, count, data);
}
