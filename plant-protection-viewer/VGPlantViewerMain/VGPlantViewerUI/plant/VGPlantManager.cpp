#include "VGPlantManager.h"
#include <QSettings>
#include <QDateTime>
#include <QtCore/private/qnumeric_p.h>

#include "VGApplication.h"
#include "VGNetManager.h"
#include "VGFlyRoute.h"
#include "das.pb.h"
#include "VGMavLinkCode.h"
#include "VGGlobalFun.h"
#include "ParametersLimit.h"
#include "MissionItem.h"
#include "VGPlantEvents.h"
#include "VGMapManager.h"
#include "VGController.h"
#include "VGVehicleMission.h"

#define d_p_ClassName(_cls) _cls::descriptor()->full_name().c_str()
using namespace das::proto;
using namespace std;
typedef union {
    float tmp[7];
    MAVPACKED(struct {
        float velocity[3];
        uint16_t precision; //航线精度
        uint16_t gndHeight; //地面高度
        uint16_t gpsVSpeed; //垂直速度
        uint16_t curMs;     //当前任务点
        uint8_t fixType;    //定位模式及模块状态
        uint8_t baseMode;   //飞控模块状态
        uint8_t satellites; //卫星数
        uint8_t sysStat;        //飞控状态
        uint8_t missionRes : 4; //任务状态
        uint8_t voltageErr : 4; //电压报警
        uint8_t sprayState : 4; //喷洒报警
        uint8_t magneErr : 2;   //校磁报警
        uint8_t gpsJam : 1;     //GPS干扰
        uint8_t stDown : 1;     //下载状态 0:没有或者完成，1:正下载
        uint8_t sysType;        //飞控类型
        uint8_t qxwzStat;       //千寻状态
    });
} GpsAdtionValue;

MAVPACKED(typedef struct {
    int32_t lat;            //维度
    int32_t lon;            //经度 
    int32_t angle : 16;     //角度
    uint32_t distance : 16; //距离
}) BlockInfo;

enum VoiceFlag
{
    Voice_Height = 1,
    Voice_Speed = Voice_Height << 1,
    Voice_VolumnS = Voice_Speed << 1,
    Voice_Sat = Voice_VolumnS << 1,

    Voice_ALL = Voice_Height|Voice_Speed|Voice_VolumnS|Voice_Sat,
};

static void _parseGPS(QGeoCoordinate &coor, const GpsInformation &gpsInfo)
{
    double lon = (double)gpsInfo.longitude() / 1e7;
    double lat = (double)gpsInfo.latitude() / 1e7;
    double altitude = (double)gpsInfo.altitude() / 1e3;

    ShareFunction::gpsCorrect(lat, lon);
    coor.setLatitude(lat);
    coor.setLongitude(lon);
    coor.setAltitude(altitude);
}

VGPlantManager::VGPlantManager(QObject *parent /*= 0*/) : QObject(parent)
, m_plantSurvey(NULL), m_plantCur(NULL), m_bExistBound(false), m_idTimerQureryPlant(-1)
, m_bInitNet(false), m_bOneKeyArm(false), m_voice(0), m_bMission(false)
, m_tmRoit(float(.1)), m_bUTrance(false)
{
    _readConfig();
}

VGPlantManager::~VGPlantManager()
{
    for (auto p : m_plants)
    {
        p->deleteLater();
    }
}

bool VGPlantManager::IsConnected() const
{
    if (VGPlantInformation *p = GetCurrentPlant())
        return p->GetStatus() == VGPlantInformation::Connected;

    return false;
}

void VGPlantManager::syncPlanFlight(VGVehicleMission *vm)
{
    if (VGPlantInformation *p = GetCurrentPlant())
        p->syncPlanFlight(vm);
}

void VGPlantManager::addNewPlant(const QString &planeId, bool bMnt)
{
    if (bMnt && !m_mntPants.contains(planeId))
    {
        _addMonitorPant(planeId);
        _writeConfig();
    }

    if (VGPlantInformation *info = GetPlantById(planeId))
    {
        if (bMnt)
            info->SetMonitor(bMnt);
        return;
    }

    VGNetManager *nm = qvgApp->netManager();
    if (nm)
        nm->requestUavInfo(QStringList(planeId));
}

void VGPlantManager::clearTmpPlant()
{
    foreach(VGPlantInformation *planeInfo, m_plants)
    {
        if (planeInfo->isDirectLink() || m_mntPants.contains(planeInfo->planeId()))
            continue;

        planeInfo->deleteLater();
    }
}

void VGPlantManager::searchPlant(QGeoCoordinate &coor, double dis/*=200*/)
{
    VGNetManager *nm = qvgApp->netManager();
    if (nm)
        nm->requestUavInfo(coor, dis);
}

void VGPlantManager::remove(VGPlantInformation *pl)
{
    if (!pl)
        return;

    if (!pl->isDirectLink())
    {
        m_plants.removeAll(pl);
        m_mntPants.removeAll(pl->planeId());
        emit mntPantsChanged();

        _writeConfig();
        if (m_strCur == pl->planeId())
            m_strCur.clear();
            
        if (VGNetManager *nm = qvgApp->netManager())
            nm->requestBindUav(pl->planeId(), 0);
    }

    pl->deleteLater();
}

void VGPlantManager::testPump()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->TestPump();
}

void VGPlantManager::testMotor(int idx, int time, int speed)
{
	if (VGPlantInformation *info = GetCurrentPlant())
		info->TestMotor(idx, time, speed);
}

void VGPlantManager::shutdown()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->Shutdown();
}

void VGPlantManager::disarm()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->Disarm();
}

void VGPlantManager::magnetic()
{
    if (IsImitateGround())
    {
        qvgApp->SetQmlTip(tr("while Magic correct,mast close Imitation!"), true);//"空中校磁，必须先关闭仿地!"
        return;
    }

	if (VGPlantInformation *info = GetCurrentPlant())
        info->SetFlightMode(MagMission);
}

bool VGPlantManager::setParameter(const QString &id, const QString &val)
{
    if (VGPlantInformation *info = GetCurrentPlant())
    {
        QVariant v = qvgApp->parameters()->stringToVariant(id, val);
        if (v.isValid())
        {
            if(id == "SPRAY_QUAN")
                v = v.toDouble() *1000;
            info->SetPlantParammeter(id, v);
            return true;
        }
    }
    return false;
}

void VGPlantManager::setParameterIndex(const QString &id, int idx)
{
    if (id.isEmpty() || idx < 0)
        return;

    if (VGPlantInformation *info = GetCurrentPlant())
    {
        QVariant v = qvgApp->parameters()->getValue(id, idx);
        info->SetPlantParammeter(id, v);
    }
}

QString VGPlantManager::getParamValue(const QString &key) const
{
    QString ret;
    if (VGPlantInformation *info = GetCurrentPlant())
    {
        QVariant v = info->GetParamValue(key);
        ret = qvgApp->parameters()->getStrValue(key, v);
    }
	
    return ret;
}

int VGPlantManager::getParamIndex(const QString &key) const
{
    int ret = -1;
    if (VGPlantInformation *info = GetCurrentPlant())
        ret = qvgApp->parameters()->getVarIndex(key, info->GetParamValue(key));

    return ret;
}

void VGPlantManager::clear()
{
    if (m_plantCur)
        m_plantCur->ClearWay();
}

void VGPlantManager::writeConfig()
{
     _writeConfig();
}

void VGPlantManager::setQxAccount(const QString &acc, const QString &pswd)
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->SetQxAccount(acc, pswd);
}

void VGPlantManager::setQxSdkAccount(const QString &acc, const QString &pswd, const QString &devid, int tp)
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->SetQxSdkAccount(acc, pswd, devid, tp);
}

void VGPlantManager::getQxSdkAccount()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->GetQXSdkAccount();
}

void VGPlantManager::clearQxSdkAccount()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->ClearQxSdkAccount(true);
}

void VGPlantManager::getQxAccount()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->GetQxAccount();
}

void VGPlantManager::clearQxAccount()
{
    if (VGPlantInformation *info = GetCurrentPlant())
        info->ClearQxAccount();
}

void VGPlantManager::processOperationInformation(const das::proto::PostOperationInformation &pi)
{
    int messageCount = pi.oi_size();
    for (int i = 0; i < messageCount; i++)
    {
        const das::proto::OperationInformation &oi = pi.oi(i);
        QString uavid(oi.uavid().c_str());

        if (oi.has_gps())
            prcsGpsInfo(uavid, oi.gps());

        if (oi.has_attitude())
            prcsVGUavAttitude(uavid, oi.attitude());

        if (oi.has_status())
            prcsOpStatus(uavid, oi.status());
    }
}

void VGPlantManager::processMavlink(const das::proto::PostStatus2GroundStation &mav)
{
    mavlink_message_t msg = { 0 };
    auto info = GetPlantById(mav.uavid().c_str());
    int count = mav.data_size();
    for (int i = 0; i < count; ++i)
    {
        const std::string &dt = mav.data(i);
        if (dt.size() < 3)
            return;

        const char* p = dt.c_str();
        msg.msgid = *(uint16_t*)p;
        msg.len = uint8_t(dt.size()) - sizeof(uint16_t);
        memcpy((char*)msg.payload64, p + sizeof(uint16_t), msg.len);
        prcsUavMavlink(info, msg, i==count-1);
    }
}

void VGPlantManager::processSuspend(const das::proto::AckMissionSuspend &ack)
{
    QString uav;
    QString planId;
    QGeoCoordinate c;
    int ridge = -1;
    if (ack.result() == 1)
    {
        const MissionSuspend &sus = ack.suspend();
        uav = sus.uav().c_str();
        planId = sus.planid().c_str();
        ridge = sus.curridge();
        if (sus.has_continiulat() && sus.has_continiulon())
        {
            c.setLatitude(sus.continiulat() / 1e7);
            c.setLongitude(sus.continiulon() / 1e7);
        }

        if (VGPlantInformation *info = GetPlantById(uav))
            info->ProcessSuspend(ridge, planId, c);
    }
}

void VGPlantManager::processSyscRoute(const SyscOperationRoutes &sr)
{
    QString uav = sr.has_uavid() ? QString::fromUtf8(sr.uavid().c_str()) : QString();
    int nCount = sr.result() < 0 ? -1 : sr.count();
    int index = nCount < 0 ? -1 : sr.index();

    VGPlantInformation *info = GetPlantById(uav);
    bool showProcess = info != NULL && nCount >= 0;
    if (showProcess)
    {
        double par = double(index + 1) / nCount;
        qvgApp->SetQmlTip(tr("Synchronize mission to UAV(%1)").arg(QString::number(par * 100, 10, 1)));//"同步航线到飞机(%1%)"
    }
}

void VGPlantManager::processUavStatus(const UavStatus &us)
{
    QString uavId = us.uavid().c_str();
    if (!uavId.isEmpty())
    {
        QGeoCoordinate coor;
        if (us.has_pos())
            _parseGPS(coor, us.pos());

        VGPlantInformation *info = GetPlantById(uavId);
        if (info)
        {
            info->setPlaneId(uavId);
            info->SetBinded(us.binded());
            info->setMaster(us.binder().c_str());
            info->setBindTime(us.time());
            info->setCoordinate(coor);
        }
        else if (info = new VGPlantInformation(this))
        {
            info->SetStatus(VGPlantInformation::UnConnect);
            info->setPlaneId(uavId);
            info->setMaster(us.binder().c_str());
            info->SetBinded(us.binded());
            info->setBindTime(us.time());
            info->setCoordinate(coor);
            Insert(info);
            if (m_mntPants.contains(uavId))
                info->SetMonitor(true);
        }
    }
}

void VGPlantManager::processAssists(const PostOperationAssist &us)
{
    auto plant = GetPlantById(us.uavid().c_str());
    if (!plant)
        return;
    QGeoCoordinate et;
    QGeoCoordinate ret;
    int st = 0;
    if (us.has_enter())
    {
        et.setLatitude(us.enter().latitude() / 1e7);
        et.setLongitude(us.enter().longitude() / 1e7);
        st |= 1;
    }
    if (us.has_return_())
    {
        ret.setLatitude(us.return_().latitude() / 1e7);
        ret.setLongitude(us.return_().longitude() / 1e7);
        st |= 2;
    }
    plant->ReceiveSupports(et, ret, st);
}

void VGPlantManager::processABPoints(const PostABPoint &ab)
{
    auto plant = GetPlantById(ab.uavid().c_str());
    if (!plant)
        return;
    QGeoCoordinate pa;
    QGeoCoordinate pb;
    if (ab.has_pointa())
    {
        pa.setLatitude(ab.pointa().latitude() / 1e7);
        pa.setLongitude(ab.pointa().longitude() / 1e7);
    }
    if (ab.has_pointb())
    { 
        pb.setLatitude(ab.pointb().latitude() / 1e7);
        pb.setLongitude(ab.pointb().longitude() / 1e7);
    }
    plant->ReceiveSupports(pa, pb, 0x10);
}

void VGPlantManager::processReturn(const PostOperationReturn &por)
{
    auto plant = GetPlantById(por.uavid().c_str());
    if (!plant)
        return;

    if (!por.has_suspend())
        return plant->SetContinueCoor(QGeoCoordinate(), 0);

    QGeoCoordinate c;
    c.setLatitude(por.suspend().latitude() / 1e7);
    c.setLongitude(por.suspend().longitude() / 1e7);
    plant->SetContinueCoor(c, por.mission() ? 1 : 2);
}

void VGPlantManager::processBlocks(const das::proto::PostBlocks &blks)
{
    auto plant = GetPlantById(blks.uavid().c_str());
    if (!plant)
        return;

    QList<VGPlantInformation::BlockInfo> ls;
    for (int i=0; 3*i<blks.blocks_size(); ++i)
    {
        BlockInfo tmp = {0};
        for (int j=0; j<3 ; ++j)
        {
            ((int*)&tmp)[j] = blks.blocks(3 * i + j);
        }
        ls << VGPlantInformation::BlockInfo(tmp.lat, tmp.lon, tmp.angle, tmp.distance);
    }
    plant->PrcsBlocks(ls);
}

void VGPlantManager::startQueryMyPlaneStatus()
{
    if (m_idTimerQureryPlant > 0)
        return;

    m_idTimerQureryPlant = startTimer(1000);
    QTimerEvent t(m_idTimerQureryPlant);
    timerEvent(&t);
    VGNetManager *nm = qvgApp->netManager();
    if (nm && m_mntPants.size() > 0)
        nm->requestUavInfo(m_mntPants);
}

 void VGPlantManager::stopQueryMyPlaneStatus()
 {
     if (m_idTimerQureryPlant > 0)
     {
         killTimer(m_idTimerQureryPlant);
         m_idTimerQureryPlant = -1;
         foreach(VGPlantInformation *planeInfo, m_plants)
         {
             if (!planeInfo->isDirectLink())
                 planeInfo->SetStatus(VGPlantInformation::UnConnect);
         }
     }
 }

 VGPlantInformation *VGPlantManager::GetPlantViaDrect() const
 {
     if (m_plants.count() && m_plants.first()->isDirectLink())
         return m_plants.first();

     return NULL;
 }

 void VGPlantManager::ViaDrectPlantCreate(const QString &mode)
 {
     if (GetPlantViaDrect())
         return;

     if (VGPlantInformation *pl = new VGPlantInformation(this))
     {
         pl->SetStatus(VGPlantInformation::Connected);
         pl->setDirectLink(true);
         pl->ChangeFlightMode(mode);
         Insert(pl, 0);
         if (!m_plantCur)
             SetCurrentPlant(pl);
     }
 }

 void VGPlantManager::RemoveViaCloudPlant(const QString &planeId)
 {
     foreach(VGPlantInformation *itr, m_plants)
     {
         if (itr->isDirectLink())
             continue;

         if (itr->planeId() == planeId)
         {
             m_plants.removeAll(itr);
             itr->deleteLater();
         }
     }
 }

 VGPlantInformation *VGPlantManager::GetPlantById(const QString &id)
 {
     if (id.isEmpty())
         return NULL;

     foreach(VGPlantInformation *itr, m_plants)
     {
         if (id.compare(itr->planeId(), Qt::CaseInsensitive) == 0)
             return itr;
     }
     return NULL;
 }

 void VGPlantManager::Insert(VGPlantInformation *info, int i)
 {
     if (!info || m_plants.contains(info))
         return;

     info->setParent(this);
     if (i > m_plants.count() || i < 0)
         m_plants.append(info);
     else
         m_plants.insert(i, info);
     
     //info->LoadFromVehicle();

     connect(info, &QObject::destroyed, this, &VGPlantManager::onChildDestroyed);
     connect(info, &VGPlantInformation::statusChanged, this, &VGPlantManager::onPlantConnectChanged);
     connect(qvgApp->mapManager(), &VGMapManager::mgrObjChanged, info, &VGPlantInformation::onMissionChange);
     info->Show(true);

     QString str = info->planeId();
     if (m_strCur == str && !str.isEmpty())
         SetCurPlant(m_strCur);
 }

 void VGPlantManager::InitialNetWork(VGNetManager *nm)
 {
     if (m_bInitNet)
         return;

     if (nm)
     {
         m_bInitNet = true;
         connect(nm, &VGNetManager::connectStateChanged, this, &VGPlantManager::onNetStatchanged);
         connect(nm, &VGNetManager::sendUavErro, this, &VGPlantManager::onSendUavErro);
         connect(nm, &VGNetManager::receiveUavProto, this, &VGPlantManager::onReceiveProto);
     }
}

void VGPlantManager::prcsOpStatus(const QString &id, const OperationStatus &st)
{
    if (VGPlantInformation *info = GetPlantById(id))
    {
        if (st.has_surplusenergy())
            info->SetPowerPercent(st.surplusenergy());
        if (st.has_jetvelocity())
            info->SetMedicineSpeed(st.jetvelocity());
        if (st.has_sprayeddose())
            info->SetMedicineVol(st.sprayeddose());
        if (st.has_operationmode())
            info->ChangeFlightMode(st.operationmode().c_str());
        if (st.has_voltage())
            info->SetVoltage(st.voltage());
    }
}

void VGPlantManager::prcsGpsInfo(const QString &id, const GpsInformation &gps)
{
    if (VGPlantInformation *info = GetPlantById(id))
    {
        double lat = gps.latitude() / 1e7;
        double lon = gps.longitude() / 1e7;
        double altitude = gps.altitude() / 1e3;
        info->setCoordinate(QGeoCoordinate(lat, lon, altitude));
        GpsAdtionValue gpsAdt = { 0 };
        int count = (sizeof(GpsAdtionValue) + sizeof(float) - 1) / sizeof(float);
        for (int i = 0; i < gps.velocity_size() && i < count; ++i)
        {
            gpsAdt.tmp[i] = gps.velocity(i);
        }

        info->SetFixType(gpsAdt.fixType);
        info->SetSatlateNumb(gpsAdt.satellites);
        info->SetBaseMode(gpsAdt.baseMode);
        info->SetSpeedVer(gpsAdt.gpsVSpeed/100.0);
        info->SetPrecision(gpsAdt.precision/100.0);
        info->SetGndHeight(gpsAdt.gndHeight/100.0);
        info->SetVoltErr(gpsAdt.voltageErr);
        info->SetSysStatus(gpsAdt.sysStat);
        info->SetMedicineGrade(gpsAdt.sprayState);
        info->SetDownMisson(gpsAdt.stDown);
        info->SetQxwzStat(gpsAdt.qxwzStat);

        if (gpsAdt.missionRes < MAV_MISSION_RESULT_ENUM_END)
            info->SychFinish(gpsAdt.missionRes == MAV_MISSION_ACCEPTED);
        
        info->SetCurrentExecuteItem(gpsAdt.curMs);
        info->setLastConnected();
    }
    else if(VGNetManager *nm = qvgApp->netManager())
    {
        _addMonitorPant(id);
        if (nm && m_mntPants.size() > 0)
            nm->requestUavInfo(QStringList(id));
    }
}

void VGPlantManager::prcsVGUavAttitude(const QString &id, const UavAttitude &attInfo)
{
    if (VGPlantInformation *info = GetPlantById(id))
    {
        if (attInfo.has_roll())
        {
            double f = BASE::toMirrorAngle(attInfo.roll());
            if (qt_is_inf(f))
                f = 0;
            else
                f = f * (180.0 / M_PI);

            info->SetRollAngle(f);
        }
        if (attInfo.has_pitch())
        {
            double f = BASE::toMirrorAngle(attInfo.pitch());
            if (qt_is_inf(f))
                f = 0;
            else
                f = f * (180.0 / M_PI);
            info->SetPitchAngle(f);
        }
        if (attInfo.has_yaw())
        {
            double f = BASE::toMirrorAngle(attInfo.yaw());
            if (qt_is_inf(f))
                f = 0;
            else
                f = f * (180.0 / M_PI);
            info->SetCompassAngle(f);
        }

        if (attInfo.has_relative_alt())
            info->SetRelativeAltitude(attInfo.relative_alt()/1e3);
        if (attInfo.has_groundspeed())
            info->SetSpeedHor(attInfo.groundspeed());
    }
}

void VGPlantManager::onNetStatchanged(int stat)
{
    if (stat == 1)
        startQueryMyPlaneStatus();
    else
        stopQueryMyPlaneStatus();
}

void VGPlantManager::onPlantConnectChanged()
{
    VGPlantInformation *p = GetCurrentPlant();
    if (p && sender() == p)
        emit connectedChanged(IsConnected());
}

void VGPlantManager::onSendUavErro(const QString &id, int res)
{
    QString strErr;
    if (res == -3)
        strErr = tr("No authorize control UAV %1").arg(id);//无权控制飞机 %1
    else if (res == -2)
        strErr = tr("UAV %1 invalid").arg(id);//"飞机%1 无效"
    else if (res == -1)
        strErr = tr("UAV %1 denied").arg(id);//"飞机%1 被禁用"

    if (!strErr.isEmpty())
        qvgApp->SetQmlTip(strErr, true);
}

void VGPlantManager::prcsUavMavlink(VGPlantInformation *info, const mavlink_message_t &msg, bool finish)
{
    if (!info)
        return;

    switch (msg.msgid)
    {
    case MAVLINK_MSG_ID_COMMAND_ACK:
        _prcsCmdAck(*info, msg); break;
    case MAVLINK_MSG_ID_MISSION_CURRENT:
        _prcsMissionCur(*info, msg); break;
    case MAVLINK_MSG_ID_PARAM_VALUE:
        _prcsParamVal(*info, msg); break;
    case MAVLINK_MSG_ID_ATTITUDE:
        _prcsAttitude(*info, msg); break;
    case MAVLINK_MSG_ID_MISSION_COUNT:
        _prcsMissionCount(*info, msg, finish); break;
    case MAVLINK_MSG_ID_MISSION_ITEM_INT:
        _prcsMissionItem(*info, msg, finish); break;
    case MAVLINK_MSG_ID_HOME_POSITION:
        _prcsHome(*info, msg); break;
    case MAVLINK_MSG_ID_VIGA_EVENT:
        _prcsUavEvent(info->planeId(), msg); break;
    case MAVLINK_MSG_ID_QX_ACCOUNT:
        _prcsQXAccount(*info, msg); break;
    case MAVLINK_MSG_ID_QX_ACC_CMD:
        _prcsQXCmd(*info, msg); break;
    case MAVLINK_MSG_ID_QXSDK_ACCOUNT:
        _prcsQXSdkAcc(*info, msg); break;
    case MAVLINK_MSG_ID_QXSDK_ACC_CMD:
        _prcsQXSdkCmd(*info, msg); break;
    case MAVLINK_MSG_ID_ASSIST_POSITION:
        _prcsSupport(*info, msg);  break;
    case MAVLINK_MSG_ID_RC_CHANNELS:
        _prcsRcChannels(*info, msg); break;
    case MAVLINK_MSG_ID_SPRAY_VALUE:
        _prcsSprayVal(*info, msg); break;
    default:
        break;
    }
}

void VGPlantManager::onReceiveProto(const google::protobuf::Message &msg)
{
    string name = msg.GetTypeName();
    if (name == d_p_ClassName(PostOperationInformation))
        processOperationInformation(*(PostOperationInformation*)&msg);
    else if (name == d_p_ClassName(PostStatus2GroundStation))
        processMavlink(*(PostStatus2GroundStation*)&msg);
    else if (name == d_p_ClassName(AckMissionSuspend))
        processSuspend(*(AckMissionSuspend*)&msg);
    else if (name == d_p_ClassName(SyscOperationRoutes))
        processSyscRoute(*(SyscOperationRoutes*)&msg);
    else if (name == d_p_ClassName(UavStatus))
        processUavStatus(*(UavStatus*)&msg);
    else if (name == d_p_ClassName(PostOperationAssist))
        processAssists(*(PostOperationAssist*)&msg);
    else if (name == d_p_ClassName(PostABPoint))
        processABPoints(*(PostABPoint*)&msg);
    else if (name == d_p_ClassName(PostOperationReturn))
        processReturn(*(PostOperationReturn*)&msg);
    else if (name == d_p_ClassName(PostBlocks))
        processBlocks(*(PostBlocks*)&msg);
}

void VGPlantManager::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != m_idTimerQureryPlant)
        return QObject::timerEvent(event);

    qint64 curMs = QDateTime::currentMSecsSinceEpoch();
    foreach(VGPlantInformation *planeInfo, m_plants)
    {
        if (!planeInfo->isDirectLink())
            planeInfo->CheckConnect(curMs);
    }
}

void VGPlantManager::SetCurPlant(const QString &id)
{
    VGPlantInformation *pl = GetPlantById(id);
    if (!pl)
        m_strCur = id;
    else if (m_plantCur == pl)
        return;

    if (pl)
        SetCurrentPlant(pl);
}

void VGPlantManager::SetCurrentPlant(VGPlantInformation *pl)
{
    if (m_plantCur == pl)
        return;

    if (m_plantCur)
        m_plantCur->SetMonitor(true, false);

    QString strLast = m_plantCur ? m_plantCur->planeId() : QString();
    QString strTmp = pl ? pl->planeId() : QString();
    m_plantCur = pl;
    _addMonitorPant(strTmp);
    qvgApp->controller()->SetCurUav(pl);

    emit currentPlantChanged();
    emit connectedChanged(IsConnected());
    emit imGndChanged();
    emit spsChanged();
    emit qxsdkChanged();
    if (pl)
        pl->SetMonitor(true, true);

    if (strTmp != strLast && !strTmp.isEmpty())
    {
        VGNetManager *nm = qvgApp->netManager();
        if (!nm)
            return;

        if (!strLast.isEmpty())
            nm->requestBindUav(strLast, 0);
        nm->requestBindUav(strTmp, 1);
        m_strCur = strTmp;
    }
}

VGPlantInformation * VGPlantManager::GetCurrentPlant() const
{
    return m_plantCur;
}

VGPlantInformation *VGPlantManager::_getPlantByIdAndUser(const QString &id, const QString &user)
{
    foreach(VGPlantInformation *itr, m_plants)
    {
        if (itr->master() == user && itr->planeId() == id)
            return itr;
    }

    return NULL;
}

VGPlantInformation * VGPlantManager::GetSurveyUav() const
{
    return m_plantSurvey;
}

void VGPlantManager::SetSurveyUav(VGPlantInformation *p)
{
    if (p == m_plantSurvey)
        return;

    m_plantSurvey = p;
    emit surveyUavChanged();
}

bool VGPlantManager::IsOneKeyArm() const
{
    return m_bOneKeyArm;
}

void VGPlantManager::DealParam(VGPlantInformation *info, const QString &id, const QVariant &var)
{
    if (!info)
        return;

    if (info->DealPlantParameter(id, var) && GetCurrentPlant()==info)
    {
        emit paramChanged(id);
        if (Imitation_Mode == id)
            emit imGndChanged();
        else if (SPS_Permision == id)
            emit spsChanged();
        else if (QX_UserSdk == id)
            emit qxsdkChanged();
    }
}

bool VGPlantManager::IsImitateGround() const
{
    if (VGPlantInformation *info = GetCurrentPlant())
        return info->IsImitateGround();

    return false;
}

bool VGPlantManager::IsSpsPermission() const
{
    if (VGPlantInformation *info = GetCurrentPlant())
        return info->IsSpsPermission();

    return false;
}

bool VGPlantManager::IsQXUseSdk() const
{
    if (VGPlantInformation *info = GetCurrentPlant())
        return info->GetParamValue(QX_UserSdk) != 0;

    return false;
}

void VGPlantManager::ControllerCalibrat()
{
    if (m_plantCur)
    {
        VGController *c = qvgApp->controller();
        for (int i=0; i<VGController::RC_CHNS; ++i)
        {
            for (int j=-1; j<2; ++j)
            {
                QVariant param = QVariant::fromValue(c->GetCalibratValue(i, j));
                m_plantCur->SetPlantParammeter(c->GetCalibratParamID(i, j), param);
            }
        }
    }
}

float VGPlantManager::GetRoitTime() const
{
    return m_tmRoit;
}

double VGPlantManager::GetRoitRadius(double sprayWidth) const
{
    if (m_bUTrance && sprayWidth >= 2)
    {
        if (sprayWidth < 4)
            return 1;

        return sprayWidth / 4;
    }
    return 0.5;
}

bool VGPlantManager::IsUTrance() const
{
    return m_bUTrance;
}

bool VGPlantManager::IsPlantId(const QString &id)
{
    QStringList lsStr = id.split(":");
    if (lsStr.count() != 2)
        return false;

    return lsStr.first() == "VIGAU" && lsStr.last().length() == 8;
}

void VGPlantManager::SetOneKeyArm(bool b)
{
    if (m_bOneKeyArm == b)
        return;
    m_bOneKeyArm = b;
    emit oneKeyArmChanged(b);
}

void VGPlantManager::SetVoiceHeight(bool b)
{
    if (b == IsVoiceHeight())
        return;

    if (b)
        m_voice |= Voice_Height;
    else
        m_voice &= ~Voice_Height;

    emit voiceHeightChanged(b);
}

bool VGPlantManager::IsVoiceHeight() const
{
    return m_voice & Voice_Height;
}

void VGPlantManager::SetVoiceSpeed(bool b)
{
    if (b == IsVoiceSpeed())
        return;

    if (b)
        m_voice |= Voice_Speed;
    else
        m_voice &= ~Voice_Speed;

    emit voiceSpeedChanged(b);
}

bool VGPlantManager::IsVoiceSpeed() const
{
    return m_voice & Voice_Speed;
}

void VGPlantManager::SetVoiceVS(bool b)
{
    if (b == IsVoiceVS())
        return;

    if (b)
        m_voice |= Voice_VolumnS;
    else
        m_voice &= ~Voice_VolumnS;

    emit voiceVSChanged(b);
}

bool VGPlantManager::IsVoiceVS() const
{
    return m_voice & Voice_VolumnS;
}

void VGPlantManager::SetVoiceSat(bool b)
{
    if (b == IsVoiceSat())
        return;

    if (b)
        m_voice |= Voice_Sat;
    else
        m_voice &= ~Voice_Sat;

    emit voiceSatChanged(b);
}

void VGPlantManager::SetRoitTime(float f)
{
    if (f < 0 && f>5)
    {
        qvgApp->SetQmlTip(tr("Hold time must between 0 and 5!"));//"停留时间必须在0--5之间!"
        return;
    }
    if (!DoubleEqu(f, m_tmRoit))
    {
        m_tmRoit = f;
        emit tmRoitChanged();
        if (VGVehicleMission *vm = m_plantCur ? m_plantCur->GetPrepareMission() : NULL)
            vm->UpdateMissionItem();
    }
}

void VGPlantManager::SetUTrance(bool b)
{
    if (b != m_bUTrance)
    {
        m_bUTrance = b;
        emit uTranceChanged();
        if (VGVehicleMission *vm = m_plantCur ? m_plantCur->GetPrepareMission() : NULL)
            vm->UpdateMissionItem();
    }
}

const QStringList &VGPlantManager::GetMntPants() const
{
    return m_mntPants;
}

bool VGPlantManager::IsVoiceSat() const
{
    return m_voice & Voice_Sat;
}

void VGPlantManager::onChildDestroyed(QObject *obj)
{
    int idx = m_plants.indexOf((VGPlantInformation *)obj);
    if (idx >= 0)
        m_plants.removeAt(idx);

    if (m_plantCur == obj)
        SetCurrentPlant(NULL);
    if (m_plantSurvey == obj)
        SetSurveyUav(NULL);
}

void VGPlantManager::_readConfig()
{
    if (QSettings *st = qvgApp->GetSettings())
    {
        st->beginGroup("PlantManager");
        m_strCur = st->value("CurPlant", "").toString();
        m_bOneKeyArm = st->value("OneKeyArm", false).toBool();
        m_voice = st->value("voice", Voice_ALL).toUInt();

        int sz = st->beginReadArray("Vehicles");
        for (int i = 0; i < sz; ++i)
        {
            st->setArrayIndex(i);
            _addMonitorPant(st->value("name").toString());
        }
        st->endArray();
        st->endGroup();
    }
}

void VGPlantManager::_writeConfig()
{
    if (QSettings *st = qvgApp->GetSettings())
    {
        st->beginGroup("PlantManager");
        st->beginWriteArray("Vehicles");
        int i = 0;
        for (const QString &inf : m_mntPants)
        {
            st->setArrayIndex(i++);
            st->setValue("name", inf);
        }
        st->endArray();
        st->setValue("CurPlant", m_strCur);
        st->setValue("OneKeyArm", m_bOneKeyArm);
        st->setValue("voice", m_voice);
        st->endGroup();
    }
}

void VGPlantManager::_addMonitorPant(const QString &id)
{
    if (m_mntPants.contains(id) || id.isEmpty() || !IsPlantId(id))
        return;

    m_mntPants << id;
    emit mntPantsChanged();
}

void VGPlantManager::_prcsCmdAck(VGPlantInformation &p, const mavlink_message_t &m)
{
    uint16_t cmd;
    MAV_RESULT result;
    if (VGMavLinkCode::DecodeCommandAck(m, cmd, result))
        p.PrcsCommandRes(cmd, result == MAV_RESULT_ACCEPTED);
}

void VGPlantManager::_prcsMissionCur(VGPlantInformation &p, const mavlink_message_t &m)
{
    mavlink_mission_current_t missionCurrent;
    mavlink_msg_mission_current_decode(&m, &missionCurrent);
    p.SetCurrentExecuteItem(missionCurrent.seq);
}

void VGPlantManager::_prcsAttitude(VGPlantInformation &p, const mavlink_message_t &msg)
{
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&msg, &attitude);
    double activeCompass = attitude.yaw;
    double activeRoll;
    double activePitch;

    if (qt_is_inf(attitude.roll))
        activeRoll = 0;
    else
        activeRoll = attitude.roll * (180.0 / M_PI);

    if (qt_is_inf(attitude.pitch))
        activePitch = 0;
    else
        activePitch = attitude.pitch * (180.0 / M_PI);

    if (qt_is_inf(attitude.yaw))
    {
        activeCompass = 0;
    }
    else
    {
        activeCompass = attitude.yaw * (180.0 / M_PI);
        if (activeCompass < 0)
            activeCompass += 360;
    }

    p.SetCompassAngle(activeCompass);
    p.SetRollAngle(activeRoll);
    p.SetPitchAngle(activePitch);
}

void VGPlantManager::_prcsParamVal(VGPlantInformation &p, const mavlink_message_t &m)
{
    QVariant v;
    QString key = VGMavLinkCode::DecodeParameter(m, v);
    DealParam(&p, key, v);
}

void VGPlantManager::_prcsMissionCount(VGPlantInformation &p, const mavlink_message_t &msg, bool b)
{
    mavlink_mission_count_t msCount;
    mavlink_msg_mission_count_decode(&msg, &msCount);
    p.SetMissionCount(msCount.count, b);
}

void VGPlantManager::_prcsMissionItem(VGPlantInformation &p, const mavlink_message_t &m, bool b)
{
    MissionItem item;
    if (VGMavLinkCode::DecodeMissionItem(m, item))
        p.SetMissionItem(item, b);
}

void VGPlantManager::_prcsHome(VGPlantInformation &p, const mavlink_message_t &msg)
{
    QGeoCoordinate c;
    if (VGMavLinkCode::DecodeHomePos(msg, c))
        p.SetHome(VGGlobalFunc::gpsCorrect(c), true);
}

void VGPlantManager::_prcsUavEvent(const QString &id, const mavlink_message_t &msg)
{
    mavlink_viga_event_t e;
    mavlink_msg_viga_event_decode(&msg, &e);
    qvgApp->plantEvents()->AddEvent(id, e.viga_event);
    VGPlantInformation *pl = GetPlantById(id);
    if (pl && 63 == e.viga_event)
        pl->GetPlantParammeter(Imitation_Mode);
}

void VGPlantManager::_prcsQXCmd(VGPlantInformation &plant, const mavlink_message_t &msg)
{
    mavlink_qx_acc_cmd_t qxcmd = { 0 };
    mavlink_msg_qx_acc_cmd_decode(&msg, &qxcmd);
    if (qxcmd.cmd == 2 || qxcmd.cmd == 3)
    {
        QString str = qxcmd.cmd == 2 ? tr("Set QX account success!") : tr("Clear QX account success!");//设置千寻账号成功！" "清除千寻账号成功！"
        qvgApp->SetQmlTip(str);
        qvgApp->speakNotice(str);
        if (qxcmd.cmd == 3)
            plant.ClearQxAccount(false);
    }
}

void VGPlantManager::_prcsQXSdkAcc(VGPlantInformation &p, const mavlink_message_t &m)
{
    QString acc;
    QString key;
    QString id;
    QString type;
    if (VGMavLinkCode::DecodeQXSDKAccount(m, acc, key, id, type))
        p.SetQxSdkAccount(acc, key, id, type.toInt() - 1, false);
}

void VGPlantManager::_prcsQXSdkCmd(VGPlantInformation &plant, const mavlink_message_t &msg)
{
    mavlink_qxsdk_acc_cmd_t qxsdkcmd = { 0 };
    mavlink_msg_qxsdk_acc_cmd_decode(&msg, &qxsdkcmd);
    if (qxsdkcmd.sdkcmd == 2 || qxsdkcmd.sdkcmd == 3)
    {
        QString str = qxsdkcmd.sdkcmd == 2 ? tr("Set QXSdk account success!") : tr("Clear QXSdk account success!");//设置千寻账号成功！" "清除千寻账号成功！"
        qvgApp->SetQmlTip(str);
        qvgApp->speakNotice(str);
        if (qxsdkcmd.sdkcmd == 3)
            plant.ClearQxSdkAccount(false);
    }
}

void VGPlantManager::_prcsRcChannels(VGPlantInformation &p, const mavlink_message_t &m)
{
    if (&p == m_plantCur && m.msgid == MAVLINK_MSG_ID_RC_CHANNELS)
    {
        mavlink_rc_channels_t rc;
        mavlink_msg_rc_channels_decode(&m, &rc);
        qvgApp->controller()->OnRecvRCChannels(p.planeId(), rc);
    }
}

void VGPlantManager::_prcsSupport(VGPlantInformation &p, const mavlink_message_t &msg)
{
    QGeoCoordinate cE, cR;
    int ret = VGMavLinkCode::DecodeSupport(msg, cE, cR);
    if (ret >= 0)
        p.ReceiveSupports(cE, cR, ret);
}


void VGPlantManager::_prcsSprayVal(VGPlantInformation &p, const mavlink_message_t &msg)
{
    double speed;
    double vol;
    uint8_t stat;
    uint8_t mode;
    if (VGMavLinkCode::DecodeSpray(msg, speed, vol, stat, mode))
    {
        p.SetMedicineSpeed(speed);
        p.SetMedicineVol(vol);
        p.SetMedicineGrade(stat);
    }
}

void VGPlantManager::_prcsQXAccount(VGPlantInformation &plant, const mavlink_message_t &msg)
{
    QString pswd;
    QString acc = VGMavLinkCode::DecodeQXAccount(msg, pswd);
    plant.SetQxAccount(acc, pswd, false);
}
