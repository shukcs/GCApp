#include "VGPlantManager.h"
#include <QSettings>
#include <QDateTime>
#include <QtCore/private/qnumeric_p.h>

#include "VGApplication.h"
#include "VGNetManager.h"
#include "VGMavLinkCode.h"
#include "VGGlobalFun.h"
#include "VGMapManager.h"
#include "das.pb.h"

using namespace das::proto;

#ifdef __GNUC__
#define BITPACKED( _DecStruct ) _DecStruct __attribute__((packed))
#else
#define BITPACKED( _DecStruct ) __pragma( pack(push, 1) ) _DecStruct __pragma( pack(pop) )
#endif
#define  INTCOORLATORLONPOW 1e7

typedef union {
float tmp[7];
BITPACKED(struct {
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
});
} GpsAdtionValue;

enum VoiceFlag
{
Voice_Height = 1,
Voice_Speed = Voice_Height << 1,
Voice_VolumnS = Voice_Speed << 1,
Voice_Sat = Voice_VolumnS << 1,

Voice_ALL = Voice_Height|Voice_Speed|Voice_VolumnS|Voice_Sat,
};

VGPlantManager::VGPlantManager(QObject *parent /*= 0*/) : QObject(parent)
, m_plantCur(NULL), m_bExistBound(false), m_bInitNet(false), m_voice(0)
, m_seq(1)
{
    _readConfig();
}

VGPlantManager::~VGPlantManager()
{
    qDeleteAll(m_palnts);
}

void VGPlantManager::writeConfig()
{
    _writeConfig();
}

VGPlantInformation *VGPlantManager::GetPlantById(const QString &id)
{
    if (id.isEmpty())
        return NULL;

    foreach(VGPlantInformation *itr, m_palnts)
    {
        if (id.compare(itr->planeId(), Qt::CaseInsensitive) == 0)
            return itr;
    }
    return NULL;
}

void VGPlantManager::Insert(VGPlantInformation *info, int i)
{
    if (!info || m_palnts.contains(info))
        return;

    info->setParent(this);
    if (i > m_palnts.count() || i < 0)
        m_palnts.append(info);
    else
        m_palnts.insert(i, info);

    connect(info, &QObject::destroyed, this, &VGPlantManager::onChildDestroyed);
    info->Show(true);
}

void VGPlantManager::InitialNetWork(VGNetManager *nm)
{
    if (m_bInitNet)
        return;

    if (nm)
    {
        m_bInitNet = true;
        connect(nm, &VGNetManager::recvPlantProtobuf, this, &VGPlantManager::onRecvBrotobuff);
        connect(nm, &VGNetManager::connectStateChanged, this, &VGPlantManager::onNetStatchanged);
        connect(nm, &VGNetManager::sendUavErro, this, &VGPlantManager::onSendUavErro);
    }
}

void VGPlantManager::onRecvBrotobuff(const google::protobuf::Message &msg)
{
    const QString &name = msg.GetTypeName().c_str();
    if (name == d_p_ClassName(PostOperationInformation))
        _prcsPostInformation(msg);
    else if (name == d_p_ClassName(AckRequestUavStatus))
        _prcsAckUavStatus(msg);
    else if (name == d_p_ClassName(AckRequestBindUav))
        _prcsAckBindUav(msg);
    else if (name == d_p_ClassName(AckSyncDeviceList))
        _prcsDeviceList(msg);
    else if (name == d_p_ClassName(UpdateDeviceList))
        _prcsUpdateDevice(msg);
}

void VGPlantManager::prcsPlaneInfo(const das::proto::UavStatus &uav)
{
    QString uavId = uav.uavid().c_str();
    if (uavId.isEmpty())
        return;

    if (auto info = GetPlantById(uavId))
    {
        info->setPlaneId(uavId);
        info->SetBinded(uav.binded());
        info->setMaster(uav.binder().c_str());
        if (uav.has_pos() && info->GetStatus()==VGPlantInformation::Unlogin)
            _parseGPS(*info, uav.pos());
    }
}

void VGPlantManager::prcsOpStatus(const QString &id, const OperationStatus &st)
{
    if (VGPlantInformation *info = GetPlantById(id))
    {
        if (st.has_operationmode())
            info->ChangeFlightMode(st.operationmode().c_str());
    }
}

void VGPlantManager::prcsGpsInfo(const QString &id, const GpsInformation &gps)
{
    if (VGPlantInformation *info = GetPlantById(id))
    {
        double lat = gps.latitude() / 1e7;
        double lon = gps.longitude() / 1e7;
        double altitude = gps.altitude() / 1e3;
        info->setCoordinate(VGGlobalFunc::gpsCorrect(QGeoCoordinate(lat, lon, altitude)));
        GpsAdtionValue gpsAdt = { 0 };
        info->SetStatus(VGPlantInformation::Monitor);
    }
}

void VGPlantManager::prcsUavAttitude(const QString &, const UavAttitude &)
{
}

void VGPlantManager::_parseGPS(VGPlantInformation &pi, const das::proto::GpsInformation &gpsInfo)
{
    double lon = (double)gpsInfo.longitude() / INTCOORLATORLONPOW;
    double lat = (double)gpsInfo.latitude() / INTCOORLATORLONPOW;
    double altitude = (double)gpsInfo.altitude() / (float)1e3;

    VGGlobalFunc::gpsCorrect(lat, lon);
    QGeoCoordinate coordinate;
    coordinate.setLatitude(lat);
    coordinate.setLongitude(lon);
    coordinate.setAltitude(altitude);
    pi.setCoordinate(coordinate);
}

void VGPlantManager::requestUavInfo(const std::string &uav)
{
    auto nm = qvgApp->netManager();
    if (nm && nm->GetConnectState() == 1 && !uav.empty())
    {
        RequestUavStatus cmdRUS;
        cmdRUS.set_seqno(m_seq++);
        cmdRUS.add_uavid(uav);
        nm->SendProtobuff(cmdRUS);
    }
}

void VGPlantManager::onNetStatchanged(int stat)
{
    if (stat == 1 && m_palnts.isEmpty())
    {
        SyncDeviceList sdl;
        sdl.set_seqno(m_seq++);
        qvgApp->netManager()->SendProtobuff(sdl);
    }
}

void VGPlantManager::onPlantConnectChanged()
{
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

void VGPlantManager::SetCurrentPlant(VGPlantInformation *pl)
{
    if (m_plantCur == pl)
        return;

    m_plantCur = pl;
    emit currentPlantChanged(pl);
}

VGPlantInformation * VGPlantManager::GetCurrentPlant() const
{
    return m_plantCur;
}

bool VGPlantManager::IsPlantId(const QString &id)
{
    QStringList lsStr = id.split(":");
    if (lsStr.count() != 2)
        return false;

    return lsStr.first() == "VIGAU" && lsStr.last().length() == 8;
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

bool VGPlantManager::IsVoiceSat() const
{
    return m_voice & Voice_Sat;
}

void VGPlantManager::onChildDestroyed(QObject *obj)
{
    int idx = m_palnts.indexOf((VGPlantInformation *)obj);
    if (idx >= 0)
        m_palnts.removeAt(idx);

    if (m_plantCur == obj)
        SetCurrentPlant(NULL);
}

void VGPlantManager::_readConfig()
{
    if (QSettings *st = qvgApp->GetSettings())
    {
        st->beginGroup("PlantManager");
        m_voice = st->value("voice", Voice_ALL).toUInt();
        st->endGroup();
    }
}

void VGPlantManager::_writeConfig()
{
    if (QSettings *st = qvgApp->GetSettings())
    {
        st->beginGroup("PlantManager");
        st->setValue("voice", m_voice);
        st->endGroup();
    }
}

void VGPlantManager::_syncDeviceList()
{
    if (VGNetManager *nm = qvgApp->netManager())
    {
        SyncDeviceList sync;
        sync.set_seqno(m_seq++);
        nm->SendProtobuff(sync);
    }
}

void VGPlantManager::_prcsPostInformation(const google::protobuf::Message &msg)
{
    PostOperationInformation &message = *(PostOperationInformation*)&msg;
    int messageCount = message.oi_size();
    for (int i = 0; i < messageCount; i++)
    {
        const OperationInformation &oi = message.oi(i);
        QString uavid(oi.uavid().c_str());
        if (oi.has_gps())
            prcsGpsInfo(uavid, oi.gps());
        if (oi.has_attitude())
            prcsUavAttitude(uavid, oi.attitude());
        if (oi.has_status())
            prcsOpStatus(uavid, oi.status());
    }
}

void VGPlantManager::_prcsAckUavStatus(const google::protobuf::Message &msg)
{
    const AckRequestUavStatus &ack = *(AckRequestUavStatus*)&msg;
    int uavSize = ack.status_size();
    for (int i = 0; i < uavSize; i++)
    {
        prcsPlaneInfo(ack.status(i));
    }
}

void VGPlantManager::_prcsAckBindUav(const google::protobuf::Message &msg)
{
    const AckRequestBindUav &ack = *(AckRequestBindUav*)&msg;
    if (ack.result() != 1 || !ack.has_status())
        return;
    const UavStatus &st = ack.status();
    if (ack.opid() == 0)
    {
        if (auto info = GetPlantById(st.uavid().c_str()))
            info->SetStatus(st.online() ? VGPlantInformation::Logined : VGPlantInformation::Unlogin);
    }
}

void VGPlantManager::_prcsDeviceList(const google::protobuf::Message &msg)
{
    const AckSyncDeviceList &ack = *(AckSyncDeviceList*)&msg;
    if (ack.result() != 1)
        return;

    auto nm = qvgApp->netManager();
    if (nm->GetConnectState() == 1)
    {
        RequestUavStatus cmdRUS;
        cmdRUS.set_seqno(m_seq++);
        for (int i = 0; i < ack.id_size(); ++i)
        {
            const std::string &uav = ack.id(i).c_str();
            if (!GetPlantById(uav.c_str()))
            {
                if (auto info = new VGPlantInformation(uav.c_str(), this))
                {
                    info->SetStatus(VGPlantInformation::Logined);
                    Insert(info);
                }
                cmdRUS.add_uavid(uav);
                if (cmdRUS.uavid_size() >= 10)
                {
                    nm->SendProtobuff(cmdRUS);
                    cmdRUS.clear_uavid();
                }
            }
        }
        if (cmdRUS.uavid_size() > 0)
            nm->SendProtobuff(cmdRUS);
    }
}

void VGPlantManager::_prcsUpdateDevice(const google::protobuf::Message &msg)
{
    const UpdateDeviceList &udl = *(UpdateDeviceList*)&msg;
    for (int i = 0; i < udl.id_size(); ++i)
    {
        auto info = GetPlantById(udl.id(i).c_str());
        if (udl.operation() == 0 && info)
            info->SetStatus(VGPlantInformation::Unlogin);
        else if (info)
            info->SetStatus(VGPlantInformation::Logined);
        else
            Insert(new VGPlantInformation(udl.id(i).c_str()));
    }
}
