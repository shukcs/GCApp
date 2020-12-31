#include "VGPlantInformation.h"
#include "VGApplication.h"
#include "VGPlantManager.h"
#include "VGNetManager.h"
#include "VGFlyRoute.h"
#include "VGVehicleMission.h"
#include "VGGlobalFun.h"
#include "VGCoordinate.h"
#include "VGOutline.h"
#include "VGMavLinkCode.h"
#include "VGLandInformation.h"
#include "VGLandPolygon.h"
#include "ParametersLimit.h"
#include "MissionManager.h"
#include "VGSupportPolyline.h"
#include "VGMapManager.h"
#include "VGToolBox.h"
#include "VGVehicle.h"
#include "VGVehicleManager.h"
#include "MAVLinkProtocol.h"
#include "VGBlock.h"
#include <stdio.h>

#define MAXHEIGHT 1000
#define DOUBLELIIM 1000000
#define ISEQDouble(f1, f2) (int)((f1)*DOUBLELIIM)==(int)((f2)*DOUBLELIIM)
#define TESTMOTORPALSEW 1150
#define TMOUT_CONNECT   5000
#define MINVTGFORMS   3.7

enum SupportStat
{
    NoOperator = 0,
    SetSupport = 1,
    ClearSupport,
    SetHomePos = 0x10000,
    UAVSetHome = SetHomePos << 1,
    UAVGetHome = UAVSetHome << 1,
    GetSupport = UAVGetHome << 1,
    GetHomeAndSupport = UAVGetHome | GetSupport,
};

static QString getStringFormBool(bool b)
{
    return b ? VGPlantInformation::tr("success")//"成功"
        : VGPlantInformation::tr("fail"); //"失败");
}

static QString getStatDescribe(int stat, MAV_CMD cmd)
{
    if (cmd == MAV_CMD_SET_START_POINT)
    {
        SupportStat stTmp = SupportStat(stat & 0xff);
        if (SetSupport == stTmp)
            return VGPlantInformation::tr("setting enter support");//"设置起飞辅助点"
        else if (ClearSupport == stTmp)
            return VGPlantInformation::tr("clear enter support");//"清除起飞辅助点"
    }
    else if (cmd == MAV_CMD_SET_END_POINT)
    {
        stat >>= 8;
        SupportStat stTmp = SupportStat(stat & 0xff);
        if (SetSupport == stTmp)
            return VGPlantInformation::tr("setting return support");//设置返航辅助点"
        else if (ClearSupport == stTmp)
            return VGPlantInformation::tr("clear return support");//""清除返航辅助点"
    }
    return QString();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//VGPlantInformation::BlockInfo
////////////////////////////////////////////////////////////////////////////////////////////////////
VGPlantInformation::BlockInfo::BlockInfo(int32_t lat, int32_t lon, int32_t angle, uint32_t distance)
    : m_angle(angle/100.0), m_distance(distance/100.0)
{
    m_coor.setLatitude(lat / 1e7);
    m_coor.setLongitude(lon / 1e7);
    m_coor = VGGlobalFunc::gpsCorrect(m_coor);
}

QGeoCoordinate VGPlantInformation::BlockInfo::GetCoordinate() const
{
    return m_coor;
}

float VGPlantInformation::BlockInfo::GetAngle() const
{
    return m_angle;
}

float VGPlantInformation::BlockInfo::GetDistance() const
{
    return m_distance;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//VGPlantInformation
////////////////////////////////////////////////////////////////////////////////////////////////////
VGPlantInformation::VGPlantInformation(QObject *parent) : MapAbstractItem(parent)
, m_status(UnConnect), m_bBind(0), m_distance(0), m_bDirectLink(false), m_bLanded(true)
, m_satNum(0), m_horSpeed(0), m_verSpeed(0), m_relativeAltitude(0), m_compassAngle(0)
, m_rollAngle(0), m_pitchAngle(0), m_powerPercent(0), m_medicineSpeed(0), m_medicineVol(0)
, m_precision(0), m_uploadVm(NULL), m_bMonitor(false), m_lastTime(0), m_lat(0), m_lon(0)
, m_lastLacalTime(QDateTime::currentMSecsSinceEpoch()), m_bMissionValible(false)
, m_tpPos(0), m_homeCoor(NULL), m_cntnCoor(NULL), m_planMissionLine(NULL), m_acc(0)
, m_baseMode(0), m_bSetArm(false), m_bSychUav(false), m_gndHeight(0), m_bSys(false)
, m_powerGrade(0), m_medicineGrade(0), m_voltage(0), m_bWlcLose(true), m_bRestrain(false)
, m_bArmOp(false), m_missionCount(0), m_bDownMs(false), m_rtl(NULL), m_devType(-1)
, m_statSupport(GetHomeAndSupport),m_aPointCoor(NULL),m_bPointCoor(NULL)
{
}

VGPlantInformation::VGPlantInformation(const VGPlantInformation &oth) : MapAbstractItem(oth)
, m_medicineVol(oth.m_medicineVol), m_uploadVm(NULL), m_bBind(oth.m_bBind), m_distance(oth.m_distance)
, m_lastTime(oth.m_lastTime), m_master(oth.m_master), m_lastPosition(oth.m_lastPosition)
, m_bindTime(oth.m_bindTime), m_bDirectLink(oth.m_bDirectLink), m_bLanded(oth.m_bLanded)
, m_satNum(oth.m_satNum), m_horSpeed(oth.m_horSpeed), m_verSpeed(oth.m_verSpeed), m_lat(0), m_lon(0)
, m_relativeAltitude(oth.m_relativeAltitude), m_compassAngle(oth.m_compassAngle)
, m_rollAngle(oth.m_rollAngle), m_pitchAngle(oth.m_pitchAngle), m_powerPercent(oth.m_powerPercent)
, m_medicineSpeed(oth.m_medicineSpeed), m_precision(0), m_status(oth.m_status), m_bMonitor(oth.m_bMonitor)
, m_lastLacalTime(oth.m_lastLacalTime), m_homeCoor(NULL), m_cntnCoor(NULL), m_bMissionValible(false)
, m_tpPos(oth.m_tpPos), m_bSychUav(false), m_planMissionLine(NULL), m_acc(oth.m_acc), m_bSetArm(false)
, m_baseMode(oth.m_baseMode), m_gndHeight(oth.m_gndHeight), m_bSys(false), m_powerGrade(0), m_bRestrain(false)
, m_medicineGrade(0), m_voltage(oth.m_voltage), m_bWlcLose(true), m_bArmOp(false), m_missionCount(0)
, m_bDownMs(false), m_devType(-1), m_rtl(NULL), m_statSupport(GetHomeAndSupport)
,m_aPointCoor(NULL),m_bPointCoor(NULL)
{
    if (oth.m_homeCoor)
        SetHome(oth.m_homeCoor->GetCoordinate());
    if (oth.m_cntnCoor)
        m_cntnCoor = new VGCoordinate(*oth.m_cntnCoor);
    if(oth.m_aPointCoor)
        SetAPointCoor(oth.m_aPointCoor->GetCoordinate());
    if(oth.m_bPointCoor)
        SetBPointCoor(oth.m_bPointCoor->GetCoordinate());

    setPlaneId(oth.planeId());
}

VGPlantInformation::~VGPlantInformation()
{
    if (m_planMissionLine)
        m_planMissionLine->deleteLater();
    if (m_uploadVm)
        m_uploadVm->deleteLater();
}

QString VGPlantInformation::planeId() const
{
    return m_planeId;
}

void VGPlantInformation::setPlaneId(const QString &id)
{
    if (id == m_planeId)
        return;

    if(m_bDirectLink)
    {
        if (VGPlantManager *mgr = qvgApp->plantManager())
            mgr->RemoveViaCloudPlant(id);
    }
    m_planeId = id;
    emit planeIdChanged(id);
}

VGPlantInformation::PlantStatus VGPlantInformation::GetStatus() const
{
    return m_status;
}

void VGPlantInformation::SetStatus(PlantStatus status)
{
    if (status == m_status)
        return;

    m_status = status;
    emit statusChanged();
    if (status == UnConnect)
    {
        SetPosType(0);
        SetAccType(0);
        SetGpsType(0);
        m_bSetArm = false;
        SetFlightMode(QString());
        if (!m_homeCoor || !m_homeCoor->IsValid())
            m_statSupport |= UAVGetHome;
    }
    else
    {
        GetParamValue("BAT_N_CELLS");
    }

    if (GetSelected())
        _showBlocks(true);

    emit posTipChanged();
    emit accTipChanged();
    emit missionValibleChanged(IsMissionValible());
}

bool VGPlantInformation::IsBinded() const
{
    return m_bBind;
}

void VGPlantInformation::SetBinded(bool status)
{
    if (status == m_bBind)
        return;

    m_bBind = status;
    emit bindedChanged(status);
}

QString VGPlantInformation::master() const
{
    return m_master;
}

void VGPlantInformation::setMaster(const QString &master)
{
    if (master == m_master)
        return;

    m_master = master;
    emit masterChanged(master);
}

qint64 VGPlantInformation::lastTime() const
{
    return m_lastTime;
}

void VGPlantInformation::setLastTime(qint64 time)
{
    if (m_lastTime != time)
        m_lastTime = time;
}

void VGPlantInformation::setLastConnected()
{
    m_lastLacalTime = QDateTime::currentMSecsSinceEpoch();
    SetStatus(Connected);
    VGFlyRoute *rt = m_uploadVm ? m_uploadVm->GetFlyRoute() : NULL;
    if (rt && m_flightMode == GetFlightModeDscb(MissionMod))
    {
        float tmp = rt->GetSprinkleWidth();
        if (GetParamValue("SPRAY_SWAT") != tmp)
            SetPlantParammeter("SPRAY_SWAT", tmp);

        tmp = m_uploadVm->GetSpeed();
        if (GetParamValue("MPC_XY_CRUISE") != tmp)
            SetPlantParammeter("MPC_XY_CRUISE", tmp);
    }
    _sysFlightMode();
    prcsSupportAndHome();
    if (m_strLsReqParams.size() > 0)
        setParameter(m_strLsReqParams.firstKey(), m_strLsReqParams.first(), true);
}

uint32_t VGPlantInformation::getConnectedTime() const
{
    return QDateTime::currentMSecsSinceEpoch() - m_lastLacalTime;
}

qint64 VGPlantInformation::bindTime() const
{
    return m_bindTime;
}

void VGPlantInformation::setBindTime(qint64 time)
{
    m_bindTime = time;
}

void VGPlantInformation::SetLanded(bool isLanded)
{
    if (m_bLanded == isLanded)
        return;

    m_bLanded = isLanded;
    qvgApp->speakNotice(m_bLanded?tr("disarmed"):tr("armed"));//"已加锁":"已解锁"
    emit landedChanged();
}

void VGPlantInformation::setCoordinate(const QGeoCoordinate &coor)
{
    bool bValidLast = m_lastPosition.isValid();
    bool bValid = coor.isValid();

    m_lat = coor.latitude();
    m_lon = coor.longitude();
    m_lastPosition = bValid ? VGGlobalFunc::gpsCorrect(coor) : QGeoCoordinate();
    emit coordinateChanged();
    if (bValid != bValidLast)
    {
        emit posTipChanged();
        emit accTipChanged();
        emit gpsTipChanged();
    }

    if (IsShowWay(m_lastPosition))
        m_planMissionLine->addCoordinate(m_lastPosition);
}

QGeoCoordinate VGPlantInformation::lastCoordinate() const
{
    return m_lastPosition;
}

double VGPlantInformation::distance() const
{
    return m_distance;
}

void VGPlantInformation::setDistance(double distance)
{
    if (distance == m_distance)
        return;

    m_distance = distance;
}

double VGPlantInformation::GetHeigth()
{
    return !IsLanded()&&IsImitateGround() ? m_gndHeight : m_relativeAltitude;
}

void VGPlantInformation::SetRelativeAltitude(double f)
{
    if (!IsImitateGround() && GetSelected() && qvgApp->plantManager()->IsVoiceHeight())
        qvgApp->speakNotice(tr("height:%1"), QStringList(VGGlobalFunc::getDecString(f)));//"高度:%1"

    if (m_gndHeight < -MAXHEIGHT || m_gndHeight > MAXHEIGHT)
        return;

    m_relativeAltitude = f;
    if (!IsImitateGround())
        emit heigthChanged(f);
}

void VGPlantInformation::SetAltitude(double f)
{
    m_lastPosition.setAltitude(f);
}

int VGPlantInformation::GetSatlateNumb() const
{
    return m_satNum;
}

void VGPlantInformation::SetSatlateNumb(int n)
{
    if (GetSelected() && qvgApp->plantManager()->IsVoiceSat())
        qvgApp->speakNotice(tr("satellites:%1"), QStringList(QString::number(n)));//"星数:%1"
    
    if (n == m_satNum)
        return;

    m_satNum = n;
    emit satlateNumbChanged(n);
}

double VGPlantInformation::GetSpeedHor() const
{
    return m_horSpeed;
}

void VGPlantInformation::SetSpeedHor(double f)
{
    if (GetSelected() && qvgApp->plantManager()->IsVoiceSpeed())
        qvgApp->speakNotice(tr("Speed:%1"), QStringList(VGGlobalFunc::getDecString(f)));//"速度:%1"
    
    if (f == m_horSpeed)
        return;
    m_horSpeed = f;
    emit horSpeedChanged(f);
}

double VGPlantInformation::GetSpeedVer() const
{
    return m_verSpeed;
}

void VGPlantInformation::SetSpeedVer(double f)
{
    if (f == m_verSpeed)
        return;

    m_verSpeed = f;
    emit verSpeedChanged(f);
}

double VGPlantInformation::GetCompassAngle() const
{
    return m_compassAngle;
}

void VGPlantInformation::SetCompassAngle(double f)
{
    if (f == m_compassAngle)
        return;

    m_compassAngle = f;
    emit compassAngleChanged(f);
}

double VGPlantInformation::GetRollAngle() const
{
    return m_rollAngle;
}

void VGPlantInformation::SetRollAngle(double f)
{
    if (f == m_rollAngle || f > 180 || f < -180)
        return;

    m_rollAngle = f;
    emit rollAngleChanged(f);
}

double VGPlantInformation::GetPitchAngle() const
{
    return m_pitchAngle;
}

void VGPlantInformation::SetPitchAngle(double f)
{
    if (f == m_pitchAngle || f > 180 || f < -180)
        return;

    m_pitchAngle = f;
    emit pitchAngleChanged(f);
}

double VGPlantInformation::GetPowerPercent() const
{
    return m_powerPercent;
}

void VGPlantInformation::SetPowerPercent(double f, bool alarm)
{
    if (f > 100 || f < 0)
        f = 100;

    if (alarm)
        qvgApp->speakNotice(tr("electric charge too low, please return hurry"));//"可用电量过低，请及时返航"
    if (f == m_powerPercent)
        return;

    m_powerPercent = f;
    emit powPercentChanged(f);
}

double VGPlantInformation::GetMedicineSpeed() const
{
    return m_medicineSpeed;
}

void VGPlantInformation::SetMedicineSpeed(double f)
{
    if (GetSelected() && qvgApp->plantManager()->IsVoiceVS())
        qvgApp->speakNotice(tr("Speed of flow:%1"), QStringList(VGGlobalFunc::getDecString(f)));//"流速:%1"

    if (f == m_medicineSpeed)
        return;

    m_medicineSpeed = f;
    emit medSpeedChanged(f);
}

double VGPlantInformation::GetMedicineVol() const
{
    return m_medicineVol;
}

void VGPlantInformation::SetMedicineVol(double f)
{
    if (f == m_medicineVol)
        return;

    m_medicineVol = f;
    emit medVolChanged(f);
}

double VGPlantInformation::GetPrecision() const
{
    return m_precision;
}

void VGPlantInformation::SetPrecision(double f)
{
    if (f == m_precision)
        return;
    m_precision = f;
    emit precisionChanged(f);
}

void VGPlantInformation::SetMonitor(bool b, bool bCur)
{
    if (bCur)
        SetSelected(true);

    if (m_bMonitor == b)
        return;

    m_bMonitor = b;
    emit monitorChanged(b);
}

bool VGPlantInformation::isDirectLink() const
{
    return m_bDirectLink;
}

void VGPlantInformation::setDirectLink(bool b)
{
    if (m_bDirectLink == b)
        return;

    m_bDirectLink = b;
    emit directLinkChanged(b);
    emit posTipChanged();
}

bool VGPlantInformation::IsLanded() const
{
    return m_bLanded;
}

bool VGPlantInformation::operator==(const MapAbstractItem &item)const
{
    if (item.ItemType() != ItemType())
        return false;

    return true;
}

void VGPlantInformation::CheckConnect(qint64 curMs)
{
    if (!isDirectLink() && curMs - m_lastLacalTime > TMOUT_CONNECT)
    {
        SetStatus(UnConnect);
        return;
    }
}

void VGPlantInformation::syncPlanFlight(VGVehicleMission *vm)
{
    if (vm && vm->MissionItems().size() > 100)
    {
        qvgApp->SetQmlTip(tr("Mission support 100 items at most!"), true);//"飞机每次任务最多支持100航路点！"
        return;
    }
    if (vm && vm->BoundaryItems().size() > 100)
    {
        qvgApp->SetQmlTip(tr("Point of boundary and block less than 100!"), true);//"边界点加障碍物点不能超过100！"
        return;
    }
    if (!IsLanded())
    {
        qvgApp->SetQmlTip(tr("Upload mission unsafe while armed!"), true);//"飞行中上传航线不安全！"
        return;
    }

    if (m_uploadVm && vm != m_uploadVm)
        m_uploadVm->deleteLater();

    m_uploadVm = vm;
    if (m_planMissionLine)
        m_planMissionLine->clear();

    if (!m_uploadVm)
        return;
    m_uploadVm->AtachByPlant();
    connect(m_uploadVm, &QObject::destroyed, this, &VGPlantInformation::onContentDestroied);

    emit missionChanged(vm);
    if (GetStatus() != Connected || !vm)
        return;

    m_bMissionValible = false;
    SetFlightMode(QString());
    SetArm(false);
    if (m_bDirectLink)
    {
        VGVehicle *vk = qvgApp->vehicleManager()->activeVehicle();
        if (vk && vk->isMissionIdle())
        {
            vk->syncPlanFlight(vm);
            SetSychBegin();
        }
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        nm->uploadMission(vm, m_planeId);
        connect(nm, &VGNetManager::missionUploaded, this, &VGPlantInformation::onSysFinish);
    }
}

MapAbstractItem::MapItemType VGPlantInformation::ItemType() const
{
    return Type_PlantInfo;
}

void VGPlantInformation::SetHome(const QGeoCoordinate &c, bool bRcv)
{
    if (!c.isValid())
        return;

    if (!m_homeCoor)
        m_homeCoor = new VGCoordinate(c, VGCoordinate::Home, this);

    if (!m_homeCoor)
        return;

    m_homeCoor->SetId(bRcv ? VGCoordinate::Home : VGCoordinate::HomeSet);
    m_homeCoor->SetCoordinate(c);

    if (m_homeCoor && GetSelected())
        m_homeCoor->Show(true);
}

void VGPlantInformation::CommandRes(unsigned short cmd, bool res)
{
    switch (cmd)
    {
    case MAV_CMD_COMPONENT_ARM_DISARM:
        _prcsArmOrDisarm(res); break;
    case MAV_CMD_DO_SET_MODE:
        _prcsFligtMode(res); break;
    case MAV_CMD_SET_START_POINT:
    case MAV_CMD_SET_END_POINT:
        _prcsSetAssist(cmd, res); break;
    case MAV_CMD_GET_HOME_POSITION:
        m_statSupport &= ~UAVGetHome; break;
    case MAV_CMD_DO_SET_HOME:
        _prcsSetHome(res); break;
    default:
        break;
    }
}

QString VGPlantInformation::GetFlightMode() const
{
    return m_flightMode;
}

void VGPlantInformation::ChangeFlightMode(const QString &mod)
{
    QString str = GetFlightModeDscb(mod);
    if (m_flightMode == str)
		return;

	m_flightMode = str;
	emit flightModeChanged();
    setMissionRun();
}

void VGPlantInformation::SetFlightMode(const QString &mod)
{
    if (m_flightModeSet == mod)
        return;

    m_flightModeSet = mod;
    if (IsMissionValible() || m_flightMode != GetFlightModeDscb(MissionMod))
        _sysFlightMode();
}

void VGPlantInformation::SetMissionValible(bool b, int tp)
{
    if (m_bMissionValible == b)
        return;

    SetSychBegin();
    m_bMissionValible = b;
    emit missionValibleChanged(IsMissionValible());
    if (IsMissionValible())
        _sysFlightMode();

    MissionManager *mm = dynamic_cast<MissionManager *>(sender());
    if (mm && MAV_MISSION_TYPE_AOBPATH == tp)
    {
        SetDownMisson(true);
        SetMissionCount(mm->missionItems().count(), false);
        for (MissionItem *itr : mm->missionItems())
        {
            SetMissionItem(*itr, false);
        }
        SetDownMisson(false);
    }
}

void VGPlantInformation::SetCurrentExecuteItem(int idx)
{
    if (m_uploadVm)
        m_uploadVm->SetCurrentExecuteItem(idx);
}

void VGPlantInformation::SetFixType(unsigned char fixType)
{
    int acc, gps;
    int rtk = VGGlobalFunc::GetPosTypeByMavFix(fixType, acc, gps);
    SetPosType(rtk);
    SetAccType(acc);
    SetGpsType(gps);
}

void VGPlantInformation::SetPosType(int t)
{
    if (m_tpPos == t)
        return;

    if (m_tpPos == 4 && m_uploadVm && GetStatus()==Connected)
        qvgApp->SetQmlTip(tr("Navigate is un-difference, please land hurry"), true);//导航非差分，请尽快就近降落

    m_tpPos = t;
    emit posTipChanged();
}

void VGPlantInformation::SetAccType(int t)
{
    if (m_acc == t)
        return;

    m_acc = t;
    emit accTipChanged();
}

void VGPlantInformation::SetGpsType(int t)
{
    if (m_gps == t)
        return;

    m_gps = t;
    emit gpsTipChanged();
}

void VGPlantInformation::SetMedicineGrade(int t)
{
	if (t == m_medicineGrade)
		return;

    m_medicineGrade = t;
}

void VGPlantInformation::SetVoltErr(int t)
{
	if (t == m_powerGrade)
		return;

	m_powerGrade = t;
}

VGVehicleMission * VGPlantInformation::GetVehicleMission() const
{
    return m_uploadVm;
}

void VGPlantInformation::Disarm()
{
    _arm(false);
}

void VGPlantInformation::SetBaseMode(unsigned short bm)
{
    m_baseMode = bm;
}

void VGPlantInformation::SetSelected(bool b)
{
    MapAbstractItem::SetSelected(b);
    if (b && m_uploadVm)
        m_uploadVm->SetSelected(true);
    else if (b && !m_uploadVm)
        loadMission();

    if (m_rtl)
        m_rtl->Show(b);

    if (m_homeCoor)
        m_homeCoor->Show(b);

    showContent(b);
    _showBlocks(b);
}

void VGPlantInformation::SetSychBegin()
{
    m_bSychUav = true;
    m_bSys = true;
}

bool VGPlantInformation::IsImitateGround()
{
    QVariant var = GetParamValue(Imitation_Mode);
    return var.toInt() != 0;
}

bool VGPlantInformation::IsSpsPermission()
{
    QVariant var = GetParamValue(SPS_Permision);
    return var.toInt() != 0;
}

void VGPlantInformation::SetVoltage(uint16_t mv)
{
    if (int(m_voltage * 1000) == mv)
        return;
    m_voltage = float(mv) / 1000;
    emit voltageChanged();
}

float VGPlantInformation::GetVoltage() const
{
    return m_voltage;
}

void VGPlantInformation::SendSupport(bool bEnter, VGCoordinate *spt)
{
    int id = spt ? spt->GetId() : 0;
    if (id == VGCoordinate::SupportEnter || id == VGCoordinate::SupportReturn)
        return;

    if (!m_uploadVm)
    {
        static const QString sStrTip = tr("please upload mission first!");// "请先上传航线再设置辅助点！";
        qvgApp->SetQmlTip(sStrTip);
        qvgApp->speakNotice(sStrTip);
        return;
    }

    MAV_CMD cmd = bEnter ? MAV_CMD_SET_START_POINT : MAV_CMD_SET_END_POINT;
    int lat = 0;
    int lon = 0;
    float alt = 0;
    if (spt)
    {
        QGeoCoordinate coor = VGGlobalFunc::toGps(spt->GetCoordinate());
        lat = coor.latitude()*1e7;
        lon = coor.longitude()*1e7;
        alt = coor.altitude();
    }
    float cl = spt ? 1 : 0;
    if (m_bDirectLink)
    {
        if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
            v->sendMavCommandInt(cmd, 0, 0, 0, cl, lat, lon, alt);
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        mavlink_message_t msg;
        VGMavLinkCode::EncodeCommandInt(msg, 0, cmd, NULL, 0, 0, 0, 0, 0, cl, lat, lon, alt);
        nm->SendControlUav(m_planeId, msg);
    }

    int stat = spt ? SetSupport : ClearSupport;
    if (!bEnter)
        stat = (stat << 8) | (m_statSupport & 0xff);
    else
        stat |= (m_statSupport & 0xff00);
    m_statSupport = stat;
}

void VGPlantInformation::SetSysStatus(uint8_t st)
{
    bool bLand = true;
    switch (st & 0x0f)
    {
    case MAV_STATE_STANDBY:
        bLand = true;
        break;
    case MAV_STATE_ACTIVE:
        bLand = false;
        break;
    }
    if (bLand != m_bLanded)
    {
        SetLanded(bLand);
        if (m_uploadVm && bLand && m_rtl)
        {
            m_rtl->Show(false);
            m_rtl->Clear();
        }
        setMissionRun();
    }
    bool bLTmp = 0 != (st & 0x10);
    if (m_bWlcLose != bLTmp)
    {
        m_bWlcLose = bLTmp;
        emit wlcLoseChanged();
    }
    bLTmp = 0 != (st & 0x40);
    if (m_bRestrain != bLTmp)
    {
        m_bRestrain = bLTmp;
        emit restrainChanged();
    }
}

bool VGPlantInformation::IsWlcLose() const
{
    return m_bWlcLose;
}

void VGPlantInformation::SetMissionCount(int c, bool finish)
{
    m_missionCount = c;
    if (finish)
    {
        if (!m_rtl)
        {
            m_rtl = new VGSupportPolyline(this);
            m_rtl->SetBorderColor(Qt::green);
        }
        m_rtl->Show(false);
        m_rtl->Clear();
        reqMissionFromNet(0);
    }
}

void VGPlantInformation::SetMissionItem(const MissionItem &item, bool finish)
{
    QGeoCoordinate coor = VGGlobalFunc::gpsCorrect(item.coordinate());
    if(m_rtl->CountPoint() == item.sequenceNumber() && m_missionCount>0)
    {
        m_rtl->AddPoint(coor);
        if (finish)
            reqMissionFromNet(m_rtl->CountPoint());
    }

    if (item.sequenceNumber() + 1 == m_missionCount)
        m_rtl->Show(GetSelected());
}

void VGPlantInformation::SetDownMisson(bool b)
{
    if(!b && m_bDownMs!=b && m_missionCount>0)
    {
        m_missionCount = 0;
        if(m_missionCount = m_rtl->CountPoint())
        {
            QString str = tr("Download safe route completely!");//"下载飞机避障路线完成！"
            qvgApp->SetQmlTip(str);
            qvgApp->speakNotice(str);
        }
    }
    m_bDownMs = b;
}

void VGPlantInformation::SetQxwzStat(int stat)
{
    if(GetQxStatDscb(stat) == m_qxwzStat)
        return;
    m_qxwzStat = GetQxStatDscb(stat);
    emit qxwzStatChanged();
}

QString VGPlantInformation::GetQxwzStat() const
{
    return m_qxwzStat;
}

void VGPlantInformation::SendSetHome(const QGeoCoordinate &c)
{
    bool bValid = c.isValid();
    if(bValid)
    {
        SetHome(VGGlobalFunc::gpsCorrect(c));
        if (m_homeCoor)
            m_homeCoor->SetId(VGCoordinate::HomeSet);
    }
    m_statSupport &= ~(SetHomePos | UAVSetHome);
    m_statSupport |= bValid ? SetHomePos : UAVSetHome;
    sendSetHome(!bValid, c);
}

void VGPlantInformation::ReceiveSupports(const QGeoCoordinate &beg, const QGeoCoordinate &end, int st)
{
    if (st < 0)
        return;

    if (st & 0x10)
    {
        if (beg.isValid())
            SetAPointCoor(VGGlobalFunc::gpsCorrect(beg));
        if (end.isValid())
            SetBPointCoor(VGGlobalFunc::gpsCorrect(end));
        return;
    }

    if (m_uploadVm)
    {
        if ((st & 1) == 0)
            m_uploadVm->clearSupport(true, true);
        else if (beg.isValid())
            m_uploadVm->addSupport(VGGlobalFunc::gpsCorrect(beg), true, true);

        if ((st & 2) == 0)
            m_uploadVm->clearSupport(false, true);
        else if (end.isValid())
            m_uploadVm->addSupport(VGGlobalFunc::gpsCorrect(end), false, true);
    }
}

void VGPlantInformation::SetQxAccount(const QString &acc, const QString &pswd, bool bSend)
{
    if (bSend)
    {
        if (m_bDirectLink)
        {
            if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
                v->setQxAccount(acc, pswd);
        }
        else if (VGNetManager *nm = qvgApp->netManager())
        {
            mavlink_message_t message;
            VGMavLinkCode::EncodeQXAccount(message, acc, pswd);
            nm->SendControlUav(m_planeId, message);
        }
    }

    if (acc == m_strQxAcc && pswd == m_strQxPswd)
        return;

    m_strQxAcc = acc;
    m_strQxPswd = pswd;
    emit qxaccChanged();
    emit qxpswdChanged();
}

void VGPlantInformation::GetQxAccount()
{
    if (m_bDirectLink)
    {
        VGVehicle *v = qvgApp->toolbox()->activeVehicle();
        MAVLinkProtocol *p = qvgApp->mavLink();
        if (v && p)
        {
            mavlink_message_t message;
            mavlink_msg_qx_acc_cmd_pack(p->getSystemId(), p->getComponentId(), &message, MAV_QX_ACC_CMD_READ);
            v->sendMessage(message);
        }
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        mavlink_message_t message;
        mavlink_msg_qx_acc_cmd_pack(0, 0, &message, MAV_QX_ACC_CMD_READ);
        nm->SendControlUav(m_planeId, message);
    }
}

void VGPlantInformation::ClearQxAccount(bool bSend)
{
    if (bSend)
    {
        if (m_bDirectLink)
        {
            VGVehicle *v = qvgApp->toolbox()->activeVehicle();
            MAVLinkProtocol *p = qvgApp->mavLink();
            if (v && p)
            {
                mavlink_message_t message;
                mavlink_msg_qx_acc_cmd_pack(p->getSystemId(), p->getComponentId(), &message, MAV_QX_ACC_CMD_CLEAR);
                v->sendMessage(message);
            }
        }
        else if (VGNetManager *nm = qvgApp->netManager())
        {
            mavlink_message_t message;
            mavlink_msg_qx_acc_cmd_pack(0, 0, &message, MAV_QX_ACC_CMD_CLEAR);
            nm->SendControlUav(m_planeId, message);
        }
    }
    else
    {
        m_strQxAcc.clear();
        m_strQxPswd.clear();
        emit qxaccChanged();
        emit qxpswdChanged();
    }
}

void VGPlantInformation::ProcessSuspend(int ridge, const QString &plan, const QGeoCoordinate &c)
{
    if (m_prepareVm && m_prepareVm->GetFlyRouteID() == plan)
        m_prepareVm->SetSuspend(ridge, VGGlobalFunc::gpsCorrect(c));
}

void VGPlantInformation::GetQXSdkAccount()
{
    if (m_bDirectLink)
    {
        VGVehicle *v = qvgApp->toolbox()->activeVehicle();
        MAVLinkProtocol *p = qvgApp->mavLink();
        if (v && p)
        {
            mavlink_message_t message;
            mavlink_msg_qxsdk_acc_cmd_pack(p->getSystemId(), p->getComponentId(), &message, MAV_QX_ACC_CMD_READ);
            v->sendMessage(message);
        }
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        mavlink_message_t message;
        mavlink_msg_qxsdk_acc_cmd_pack(0, 0, &message, MAV_QX_ACC_CMD_READ);
        nm->SendControlUav(m_planeId, message);
    }
}

void VGPlantInformation::SetQxSdkAccount(const QString &key, const QString &secret, const QString &devid, int type, bool bSend)
{
    m_strAppkey = key;
    m_strAppsecret = secret;
    m_devid = devid;
    m_devType = type;
    emit appkeyChanged();
    emit appsecretChanged();
    emit devidChanged();
    emit devtypeChanged();


    if (bSend)
    {
        if (m_bDirectLink)
        {
            if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
                v->setQxSdkAccount(key, secret, devid, QString::number(type + 1));
        }
        else if (VGNetManager *nm = qvgApp->netManager())
        {
            mavlink_message_t message;
            VGMavLinkCode::EncodeQXSdkAccount(message,key, secret, devid, QString::number(type+ 1) );
            nm->SendControlUav(m_planeId, message);
        }
    }
}

void VGPlantInformation::ClearQxSdkAccount(bool bSend)
{
    if (bSend)
    {
        if (m_bDirectLink)
        {
            VGVehicle *v = qvgApp->toolbox()->activeVehicle();
            MAVLinkProtocol *p = qvgApp->mavLink();
            if (v && p)
            {
                mavlink_message_t message;
                mavlink_msg_qxsdk_acc_cmd_pack(p->getSystemId(), p->getComponentId(), &message, MAV_QXSDK_ACC_CMD_CLEAR);
                v->sendMessage(message);
            }
        }
        else if (VGNetManager *nm = qvgApp->netManager())
        {
            mavlink_message_t message;
            mavlink_msg_qxsdk_acc_cmd_pack(0, 0, &message, MAV_QXSDK_ACC_CMD_CLEAR);
            nm->SendControlUav(m_planeId, message);
        }
    }
    else
    {
        m_strAppkey.clear();
        m_strAppsecret.clear();
        m_devid.clear();
        m_devType = -1;
        emit appkeyChanged();
        emit appsecretChanged();
        emit devidChanged();
        emit devtypeChanged();
    }
}

void VGPlantInformation::PrcsBlocks(const QList<VGPlantInformation::BlockInfo> &blocks)
{
    auto itr = m_blocks.begin();
    int idx = 1;
    for (const BlockInfo &blk : blocks)
    {
        if (itr != m_blocks.end())
        {
            (*itr)->setCoordinate(blk.GetCoordinate());
            (*itr)->setAngle(blk.GetAngle());
            (*itr)->setDis(blk.GetDistance());
            (*itr)->SetId(idx++);
            ++itr;
        }
        else if (VGBlock *tmp = new VGBlock(blk.GetCoordinate(), blk.GetAngle(), blk.GetDistance(), this))
        {
            tmp->SetId(idx++);
            m_blocks.append(tmp);
            itr = m_blocks.end();
        }
    }

    for (int i=blocks.size(); i<m_blocks.size(); ++i)
    {
        (*itr)->deleteLater();
        itr = m_blocks.erase(itr);
    }

    _showBlocks(GetSelected());
}

VGVehicleMission *VGPlantInformation::GetPrepareMission() const
{
    return m_prepareVm;
}

void VGPlantInformation::reqMissionFromNet(int idx)
{
    if (idx >= 0 && !m_bDirectLink && m_missionCount>0)
    {
        mavlink_message_t msg = {0};
        if (idx == m_missionCount)
            mavlink_msg_mission_ack_pack_chan(0, 0, 0, &msg, 0, 0, 4, 0);
        else
            mavlink_msg_mission_request_pack_chan(0, 0, 0, &msg, 0, 0, idx, 4);
        
        qvgApp->netManager()->SendControlUav(m_planeId, msg);
    }
}

void VGPlantInformation::SetContinueCoor(const QGeoCoordinate &c, int type)
{
    if (!c.isValid() || (type!=1 && type!=2))
    {
        if (m_cntnCoor)
            m_cntnCoor->SetCoordinate(QGeoCoordinate());
        return;
    }

    auto pntType = (1 == type ? VGCoordinate::ContinueFly : VGCoordinate::ABContinueFly);
    if (!m_cntnCoor)
        m_cntnCoor = new VGCoordinate(c, pntType, this);

    m_cntnCoor->SetCoordinate(VGGlobalFunc::gpsCorrect(c));
    if (m_cntnCoor->GetId() != pntType)
    {
        m_cntnCoor->Show(false);
        m_cntnCoor->SetId(pntType);
    }

    _showContinue(GetSelected());
}

void VGPlantInformation::SetAPointCoor(const QGeoCoordinate &a, bool clear)
{
    if (!a.isValid() || clear)
    {
        if (m_aPointCoor)
            m_aPointCoor->SetCoordinate(QGeoCoordinate());
        return;
    }

    if (!m_aPointCoor)
        m_aPointCoor = new VGCoordinate(a, VGCoordinate::A_Point, this);
    else
        m_aPointCoor->SetCoordinate(a);

    if (m_aPointCoor && GetSelected())
        m_aPointCoor->Show(!clear);
}

void VGPlantInformation::SetBPointCoor(const QGeoCoordinate &b, bool clear)
{
    if (!b.isValid() || clear)
    {
        if (m_bPointCoor)
            m_bPointCoor->SetCoordinate(QGeoCoordinate());
        return;
    }

    if (!m_bPointCoor)
        m_bPointCoor = new VGCoordinate(b, VGCoordinate::B_Point, this);
    else
        m_bPointCoor->SetCoordinate(b);

    if (m_bPointCoor && GetSelected())
        m_bPointCoor->Show(!clear);
}

QString VGPlantInformation::GetQXAcc() const
{
    return m_strQxAcc;
}

QString VGPlantInformation::GetQXPswd() const
{
    return m_strQxPswd;
}

QString VGPlantInformation::GetAppkey() const
{
    return m_strAppkey;
}

QString VGPlantInformation::GetAppsecret() const
{
    return m_strAppsecret;
}

QString VGPlantInformation::GetDevid() const
{
    return m_devid;
}

int VGPlantInformation::GetDevtype() const
{
    return m_devType;
}

bool VGPlantInformation::IsRestrain() const
{
    return m_bRestrain;
}

bool VGPlantInformation::IsShowWay(const QGeoCoordinate &coor) const
{
    if (m_planMissionLine && !IsLanded() && (m_gps == 2 || m_gps == 3))
    {
        if (!coor.isValid())
            return false;

        if (m_planMissionLine->coordinateCount() < 1)
            return true;

        return coor.distanceTo(m_planMissionLine->lastCoordinate()) < 1e3;
    }

    return false;
}

double VGPlantInformation::GetLat() const
{
    return m_lat;
}

double VGPlantInformation::GetLon() const
{
    return m_lon;
}

VGVehicleMission *VGPlantInformation::beginOperation(VGFlyRoute *fr)
{
    m_prepareVm = NULL;
    if (fr)
    {
        qvgApp->netManager()->QuerySuspend(m_planeId, fr->GetActId());
        m_prepareVm = fr->beginOperation();
    }

    return m_prepareVm;
}

QString VGPlantInformation::GetFlightModeDscb(const QString &str)
{
    QString strMode = tr("Unknown");//"未知"
    if (str == DenyMod)
        strMode = tr("deny");//"禁飞"
    else if (str == MissionMod)
        strMode = tr("Mission");//"任务"
    else if (str == ManualMod)
        strMode = tr("Manual");//"手动"
    else if (str == HoldMod)
        strMode = tr("Hold");//"悬停"
    else if (str == ReturnMod)
        strMode = tr("Return");//"返航"
    else if (str == MagMission)
        strMode = tr("Magic correct");//"校磁"
    else if (str == Landing)
        strMode = tr("Landing");//"着陆"
    else if(str == ABPMod)
        strMode = tr("ABPoint");//"AB点模式"
    return strMode;
}

QString VGPlantInformation::GetQxStatDscb(int stat)
{
    QString strStat = tr("Unknown");//"未知"
    if(stat == QXWZSOCK_UNINIT_ST)
        strStat = tr("QXWZSOCK UNINIT");//"千寻未初始化"
    else if(stat == QXWZTASK_BEGIN_ST)
        strStat = tr("QXWZTASK BEGIN");//千寻开始运行
    else if(stat == QXWZ_READGPS_ST)
        strStat = tr("QXWZ READGPS");//"千寻读GPS有效信号"
    else if(stat == QXWZ_NBEGIN_ST )
        strStat = tr("QXWZ NBEGIN");//"千寻进入ntrip模式"
    else if(stat == QXWZ_NGETACC_ST)
        strStat = tr("QXWZ NGETACC");//"千寻读ntrip账号密码"
    else if(stat == QXWZ_NNOSOCK_ST)
        strStat = tr("QXWZ NNOSOCK");//"千寻ntrip方式创建sock"
    else if(stat == QXWZ_NCONNECT_ST)
        strStat = tr("QXWZ NCONNECT");//"千寻ntrip方式connect"
    else if(stat == QXWZ_NLOGIN_ST)
        strStat = tr("QXWZ NLOGIN");//"千寻ntrip方式登录"
    else if(stat == QXWZ_NUNAUTHORIZD_ST)
        strStat = tr("QXWZ NUNAUTHORIZD");//"千寻ntrip方式账号未授权"
    else if(stat == QXWZ_NRECRTCM_ST)
        strStat = tr("QXWZ NRECRTCM");//"千寻ntrip方式正接收rtcm"
    else if(stat == QXWZ_NSENDGGA_ST)
        strStat = tr("QXWZ NSENDGGA");//"千寻ntrip方式正发送GGA"
    else if(stat == QXWZ_SDKBEGIN_ST )
        strStat = tr("QXWZ SDKBEGIN");//"千寻进入SDK模式"
    else if(stat == QXWZ_SDKGETACC_ST )
        strStat = tr("QXWZ SDKGETACC");//"千寻读SDK账号密码"
    else if(stat == QXWZ_SDKNOSOCK_ST)
        strStat = tr("QXWZ SDKNOSOCK");//"千寻SDK方式创建sock"
    else if(stat == QXWZ_SDKCONNECT_ST)
        strStat = tr("QXWZ SDKCONNECT");//"千寻SDK方式connect"
    else if(stat == QXWZ_SDKLOGIN_ST)
        strStat = tr("QXWZ SDKLOGIN");//"千寻SDK方式登录"
    else if(stat == QXWZ_SDKUNAUTHORIZD_ST)
        strStat = tr("QXWZ SDKUNAUTHORIZD");//"千寻SDK方式账号未授权"
    else if(stat == QXWZ_SDKEXPIRETIME_ST)
        strStat = tr("QXWZ SDKEXPIRETIME");//"千寻SDK方式账号过期"
    else if(stat == QXWZ_SDKRECRTCM_ST)
        strStat = tr("QXWZ SDKRECRTCM");//"千寻SDK方式正接收rtcm"
    else if(stat == QXWZ_SDKSENDGGA_ST)
        strStat = tr("QXWZ SDKSENDGGA");//"千寻SDK方式正发送GGA"
    return strStat;
}

bool VGPlantInformation::DealPlantParameter(const QString &key, const QVariant &v)
{
    m_strLsReqParams.remove(key);
    if (qvgApp->parameters()->hasParam(key))
    {
        QMap<QString, QVariant>::iterator itr = m_params.find(key);
        if (itr == m_params.end() || itr.value() != v)
        {
            if (key == "SPRAY_QUAN")
                m_params[key] = v.toFloat() / 1000;
            else
                m_params[key] = v;

            return true;
        }

        if (key == Imitation_Mode || key == Imitation_Precision)
            _checkParams();
    }

    return false;
}

void VGPlantInformation::SetGndHeight(double height)
{
    if (IsImitateGround() && GetSelected() && qvgApp->plantManager()->IsVoiceHeight())
        qvgApp->speakNotice(tr("height:%1"), QStringList(VGGlobalFunc::getDecString(height)));//"高度:%1"

    if (m_gndHeight < -MAXHEIGHT || m_gndHeight > MAXHEIGHT)
        return;

    m_gndHeight = height;
    if (IsImitateGround())
        emit  heigthChanged(height);
}

QVariant VGPlantInformation::GetParamValue(const QString &key)
{
    QMap<QString, QVariant>::const_iterator itr = m_params.find(key);
    if (itr != m_params.end())
        return itr.value();

    if(qvgApp->parameters()->hasParam(key))
        GetPlantParammeter(key);

    return QVariant();
}

void VGPlantInformation::onMissionChange()
{
    if (!GetSelected())
        return;

    setMissionRun();
}

void VGPlantInformation::LoadFromVehicle()
{
    if (m_bDirectLink)
    {
        if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
            v->loadFromVehicle();
    }
    else
    {
    }
}

void VGPlantInformation::TestPump()
{
    sendCommand(MAV_CMD_VAGA_TEST, 1);
}

void VGPlantInformation::TestMotor(int idx, int ms, int speed)
{
	if (idx >= 8)
		return;

    sendCommand(MAV_CMD_VAGA_TEST, 0, 1<<idx, speed, ms);
}

void VGPlantInformation::Shutdown()
{
    sendCommand(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, 1);
}

void VGPlantInformation::SetPlantParammeter(const QString &key, const QVariant &var)
{
    QVariant tmp = GetParamValue(key);
    if (tmp == var)
        return;

    setParameter(key, var);
}

void VGPlantInformation::GetPlantParammeter(const QString &key)
{
    setParameter(key);
}

void VGPlantInformation::SychFinish(bool bsuc)
{
    if (!m_bSys)
        return;

    if (m_bSychUav)
    { 
        m_bSychUav = false;
        QString suc = bsuc ? tr("Upload mission success!") : tr("Upload mission fail!");//"上传航线到飞机成功！""上传航线到飞机失败！"
        qvgApp->SetQmlTip(suc, !bsuc);
        if (bsuc)
        { 
            qvgApp->speakNotice(suc);
            saveMission();
        }
    }
 
    if (bsuc)
    {
        if (m_planMissionLine)
            m_planMissionLine->clear();

        if(qvgApp->plantManager()->IsOneKeyArm())
            SetArm(true);
    }
}

bool VGPlantInformation::IsMonitor() const
{
    return m_bMonitor;
}

bool VGPlantInformation::IsSetArm() const
{
    return m_bSetArm;
}

void VGPlantInformation::SetArm(bool b)
{
    QVariant var = GetParamValue("BAT_N_CELLS");
    if (b && var.isValid() && GetVoltage()/var.toInt()<MINVTGFORMS)
    {
        qvgApp->SetQmlTip(tr("Battery charge too low, please change battery!"),true);//"电池电压低，请更换电池再作业!"
        return;
    }

    if(b)
    {
        m_bSetArm = b;
        if (m_flightMode == GetFlightModeDscb(MissionMod))
            _arm(b);
        else
            SetFlightMode(MissionMod);
    }

    emit armChanged();
}

bool VGPlantInformation::IsMissionValible() const
{
    return m_status==Connected && m_bMissionValible;
}

QString VGPlantInformation::GetPosTip() const
{
    
    if (GetStatus() == Connected && lastCoordinate().isValid())
        return VGGlobalFunc::GetPosTypeTip(m_tpPos);

    return tr("N/A");
}

QString VGPlantInformation::GetAccTip() const
{
    if (GetStatus() == UnConnect || !lastCoordinate().isValid())
        return tr("N/A");

    switch (m_acc)
    {
    case 3:
        return tr("invalid");   //"不可用"
    case 5:
        return tr("valid");     //"可用"
    default:
        break;
    }

    return tr("un-work");//"未工作"
}

QString VGPlantInformation::GetGpsTip() const
{
    if (!lastCoordinate().isValid())
        return tr("N/A");

    switch (m_gps)
    {
    case 0:
        return tr("un-work");
    case 1:
        return tr("no position");// "未定位"
    case 2:
        return tr("2D position");//"2D定位"
    case 3:
        return tr("Single position");//"单点定位"
    default:
        break;
    }
    return tr("N/A");
}

void VGPlantInformation::showContent(bool b)
{
    if (m_homeCoor)
        m_homeCoor->Show(b);

    setMissionRun();
}

void VGPlantInformation::_arm(bool bArm)
{
    if (m_bDirectLink)
    {
        if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
            v->setArmed(bArm);
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        mavlink_message_t msg;
        VGMavLinkCode::EncodeCommands(msg, 0, MAV_CMD_COMPONENT_ARM_DISARM, NULL, 0, 0, bArm ? 1.0 : 0);
        nm->SendControlUav(m_planeId, msg);
    }
    m_bArmOp = !bArm;
    if (bArm)
        CheckConnect(m_lastLacalTime);
}

void VGPlantInformation::_sysFlightMode()
{
    if ((m_flightModeSet!=MissionMod || IsMissionValible()) && !m_flightModeSet.isEmpty())
    {
        if (m_bDirectLink)
        {
            if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
                v->SetFlightMode(m_flightModeSet);
        }
        else if (VGNetManager *nm = qvgApp->netManager())
        {
            mavlink_message_t msg;
            VGMavLinkCode::EncodeSetMode(msg, m_flightModeSet, m_baseMode);
            nm->SendControlUav(m_planeId, msg);
        }
    }
}

void VGPlantInformation::onSysFinish(const QString &id)
{
    if(id == m_planeId)
        SetMissionValible(true, MAV_MISSION_TYPE_MISSION);
}

void VGPlantInformation::onContentDestroied(QObject *o)
{
    if (m_uploadVm == o)
        m_uploadVm = NULL;
}

void VGPlantInformation::setMissionRun()
{
    bool bShow = qvgApp->mapManager()->IsMissionPage() && GetSelected();
    bool bMission = m_flightMode == GetFlightModeDscb(MissionMod);
    if(m_uploadVm)
        m_uploadVm->showSquences(bShow && !IsLanded() && !IsLanded());

    _showContinue(GetSelected());
    if (!m_planMissionLine && (bMission || m_flightMode==GetFlightModeDscb(ABPMod)))
    {
        m_planMissionLine = new VGOutline(this, VGLandPolygon::NoPoint);
        m_planMissionLine->SetBorderColor(QColor("#F01010"));//"#F01010"
        m_planMissionLine->SetShowType(Show_Line);
    }

    if (m_planMissionLine)
        m_planMissionLine->Show(bShow);
}

void VGPlantInformation::setParameter(const QString &str, const QVariant &var, bool doAnyway)
{
    if (!m_bDirectLink)
    {
        bool bSend = m_strLsReqParams.size() == 0 || doAnyway;
        m_strLsReqParams[str] = var;

        if (var.isValid() && str == Imitation_Mode && var.toInt() == 0)
            m_strLsReqParams[Imitation_Precision] = QVariant::fromValue<float>(1.2f);

        if (bSend)
            qvgApp->netManager()->SendControlUav(m_planeId, m_strLsReqParams);
    }
    else if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
    {
        v->prcsVehicleParameter(str, var);
    }
}

void VGPlantInformation::saveMission()
{
    if (m_uploadVm && !m_planeId.isEmpty())
    {
        if (QSettings *st = qvgApp->GetSettings())
        {
            QString str = QString("Mission%1").arg(m_planeId);
            st->beginGroup(str);
            st->setValue("info", m_uploadVm->GetInfo());
            st->endGroup();
        }
    }
}

void VGPlantInformation::loadMission()
{
    if (m_uploadVm || m_planeId.isEmpty())
        return;

    if (QSettings *st = qvgApp->GetSettings())
    {
        QString str = QString("Mission%1").arg(m_planeId);
        st->beginGroup(str);
        m_uploadVm = VGVehicleMission::fromInfo(st->value("info").toString());
        m_bMissionValible = true;
        emit missionValibleChanged(IsMissionValible());
        st->endGroup();
    }
}

void VGPlantInformation::sendCommand(int cmd, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    if (m_bDirectLink)
    {
        if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
            v->sendMavCommand((MAV_CMD)cmd, f1, f2, f3, f4, f5, f6, f7);
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        mavlink_message_t msg;
        VGMavLinkCode::EncodeCommands(msg, 0, (MAV_CMD)cmd, NULL, 0, 0, f1, f2, f3, f4, f5, f6, f7);
        nm->SendControlUav(m_planeId, msg);
    }
}

void VGPlantInformation::sendSetHome(bool uav, const QGeoCoordinate &c)
{
    int lat = uav ? 0 : c.latitude()*1e7;
    int lon = uav ? 0 : c.longitude()*1e7;
    float alt = uav ? 0 : c.altitude();
    if (m_bDirectLink)
    {
        if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
            v->sendMavCommandInt(MAV_CMD_DO_SET_HOME, uav?1:0, 0, 0, 0, lat, lon, alt);
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        mavlink_message_t msg;
        VGMavLinkCode::EncodeCommandInt(msg, 0, MAV_CMD_DO_SET_HOME, NULL, 0, 0, uav ? 1 : 0, 0, 0, 0, lat, lon, alt);
        nm->SendControlUav(m_planeId, msg);
    }
}

void VGPlantInformation::prcsSupportAndHome()
{
    if (UAVGetHome & m_statSupport)
        sendCommand(MAV_CMD_GET_HOME_POSITION);
    if ((m_statSupport&SetHomePos) && m_homeCoor && m_homeCoor->GetId() == VGCoordinate::HomeSet)
        sendSetHome(false, VGGlobalFunc::toGps(m_homeCoor->GetCoordinate()));

    SupportStat stTmp1 = SupportStat(m_statSupport & 0xff);
    if (SetSupport == stTmp1 && m_uploadVm && m_uploadVm->GetSupportEnter())
        SendSupport(true, m_uploadVm->GetSupportEnter());
    else if (ClearSupport == stTmp1)
        SendSupport(true, NULL);
    SupportStat stTmp2 = SupportStat((m_statSupport >> 8) & 0xff);
    if (SetSupport == stTmp1 && m_uploadVm && m_uploadVm->GetSupportReturn())
        SendSupport(false, m_uploadVm->GetSupportReturn());
    else if (ClearSupport == stTmp2)
        SendSupport(false, NULL);

    if ((m_statSupport&GetSupport) && m_flightMode == GetFlightModeDscb(MissionMod))
        requestFromUav(MAV_CMD_GET_START_POINT);

    if(!m_aPointCoor && !m_bPointCoor && m_flightMode == GetFlightModeDscb(ABPMod))
        requestFromUav(MAV_CMD_GET_AB_POINT);
}

void VGPlantInformation::requestFromUav(uint32_t mavType)
{
    if (m_bDirectLink)
    {
        if (mavType >= MAV_CMD_ENUM_END)
            return;
        if (VGVehicle *v = qvgApp->toolbox()->activeVehicle())
            v->sendMavCommandInt(MAV_CMD_GET_START_POINT);
    }
    else if (VGNetManager *nm = qvgApp->netManager())
    {
        nm->requestFromUav(m_planeId, mavType);
    }
}

void VGPlantInformation::_prcsArmOrDisarm(bool res)
{
    QString str = tr("UAV %1 %2").arg(m_bArmOp ? tr("disarm") : tr("arm")).arg(getStringFormBool(res));
    qvgApp->SetQmlTip(str, !res);
    m_bSetArm = false;

    VGFlyRoute *rt = m_uploadVm ? m_uploadVm->GetFlyRoute() : NULL;
    if(!m_bArmOp && res && rt)
    {
        float tmp = rt->GetMedPerAcre() * 1000;
        if (GetParamValue("SPRAY_QUAN") != tmp)
            SetPlantParammeter("SPRAY_QUAN", tmp);
    }
}

void VGPlantInformation::_prcsFligtMode(bool res)
{
    if (res)
    {
        if (m_flightModeSet == MissionMod && m_uploadVm)
        {
            QVariant tmp((float)m_uploadVm->GetOpHeight());
            setParameter("MPC_WORK_MIN_ALT", tmp);
            setParameter("RTL_DESCEND_ALT", tmp);
            setParameter("MIS_TAKEOFF_ALT", tmp);
            setParameter("EKF2_ALT1_SAFETY", QVariant(m_uploadVm->MaxWorkAlt() + 3), true);
        }
        else if (m_flightModeSet == MagMission)
        {
            setParameter("MIS_TAKEOFF_ALT", QVariant::fromValue<float>(3), true);
        }
    }

    if (!res)
        qvgApp->SetQmlTip(tr("change flight model fail"), true);//"设置飞行模式失败"

    else if (m_bSetArm || m_flightModeSet == MagMission)
        _arm(true);

    m_flightModeSet = QString();
}

void VGPlantInformation::_prcsSetAssist(int cmd, bool res)
{
    QString str = getStatDescribe(m_statSupport, (MAV_CMD)cmd);
    if (!str.isEmpty())
        qvgApp->SetQmlTip(str + getStringFormBool(res) + "!", !res);

    if (cmd == MAV_CMD_SET_START_POINT)
        m_statSupport &= ~0xff;
    else
        m_statSupport &= ~0xff00;
}

void VGPlantInformation::_prcsSetHome(bool res)
{
    qvgApp->SetQmlTip(tr("%1 set home point %2!").arg(m_bSetArm ?"UAV":"").arg(getStringFormBool(res)), !res);//"%1设置Home点%2!"
    m_statSupport &= ~(SetHomePos | UAVSetHome);

    if(res && m_bSetArm)
    {
        if (m_flightMode == GetFlightModeDscb(MissionMod) && m_bMissionValible)
            _arm(true);
        else
            _sysFlightMode();
    }
}

void VGPlantInformation::_checkParams()
{
    QVariant var = GetParamValue("MPC_Z_P");
    bool bImitateGround = IsImitateGround();
    if (!var.isValid() && bImitateGround)
        return;

    bool bSend = m_strLsReqParams.count() < 1;
    float fZP = var.isNull() ? -1 : var.toFloat();
    if (!IsImitateGround() || DoubleEqu(fZP, 1.2))
    {
        m_strLsReqParams["MPC_ALT_KP"] = QVariant::fromValue(float(3.0));
        m_strLsReqParams["MPC_ALT_KI"] = QVariant::fromValue(float(.15));
        m_strLsReqParams["MPC_ALT_KD"] = QVariant::fromValue(float(.5));
        m_strLsReqParams["EKF2_DISFILTER_N"] = QVariant::fromValue(50);
    }
    else if (DoubleEqu(fZP, 1.0))
    {
        m_strLsReqParams["MPC_ALT_KP"] = QVariant::fromValue(float(2.0));
        m_strLsReqParams["MPC_ALT_KI"] = QVariant::fromValue(float(0.0));
        m_strLsReqParams["MPC_ALT_KD"] = QVariant::fromValue(float(.75));
        m_strLsReqParams["EKF2_DISFILTER_N"] = QVariant::fromValue(50);
    }
    else if (DoubleEqu(fZP, .8))
    {
        m_strLsReqParams["MPC_ALT_KP"] = QVariant::fromValue(float(1.4));
        m_strLsReqParams["MPC_ALT_KI"] = QVariant::fromValue(float(0));
        m_strLsReqParams["MPC_ALT_KD"] = QVariant::fromValue(float(1.1));
        m_strLsReqParams["EKF2_DISFILTER_N"] = QVariant::fromValue(50);
    }
    else
    {
        return;
    }

    if (!bSend)
        qvgApp->netManager()->SendControlUav(m_planeId, m_strLsReqParams);
}

void VGPlantInformation::_showContinue(bool b)
{
    if (!m_cntnCoor)
        return;

    if (!b)
    {
        m_cntnCoor->Show(false);
        return;
    }
    auto tp = GetFlightModeDscb(ABPMod) == m_flightMode ? VGCoordinate::ABContinueFly : VGCoordinate::ContinueFly;
    m_cntnCoor->Show(m_cntnCoor->GetId() == tp);
}

void VGPlantInformation::_showBlocks(bool b)
{
    b = b && GetStatus()==Connected;
    for (VGBlock *itr : m_blocks)
    {
        itr->Show(b);
    }
}
