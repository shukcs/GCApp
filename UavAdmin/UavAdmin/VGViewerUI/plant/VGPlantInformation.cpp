#include "VGPlantInformation.h"
#include "VGApplication.h"
#include "VGPlantManager.h"
#include "VGNetManager.h"
#include "VGGlobalFun.h"
#include "VGMavLinkCode.h"
#include "VGMapManager.h"
#include <stdio.h>

#define MAXHEIGHT 1000
#define TESTMOTORPALSEW 1150
#define TMOUT_CONNECT   5000

//////////////////////////////////////////////////////////////////////////////////////////////
//VGPlantInformation
//////////////////////////////////////////////////////////////////////////////////////////////
VGPlantInformation::VGPlantInformation(const QString &id, QObject *parent)
: MapAbstractItem(parent), m_planeId(id), m_bBind(false), m_stat(Unlogin)
{
}

VGPlantInformation::VGPlantInformation(const VGPlantInformation &oth) : MapAbstractItem(oth)
, m_planeId(oth.m_planeId), m_bBind(oth.m_bBind), m_stat(oth.m_stat)
, m_lastPosition(oth.m_lastPosition), m_master(oth.m_master)
, m_flightMode(oth.m_flightMode)
{
}

VGPlantInformation::~VGPlantInformation()
{
}

QString VGPlantInformation::planeId() const
{
    return m_planeId;
}

void VGPlantInformation::setPlaneId(const QString &id)
{
    if (id == m_planeId)
        return;

    m_planeId = id;
    emit planeIdChanged(id);
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

void VGPlantInformation::setCoordinate(const QGeoCoordinate &coor)
{
    if (coor!= m_lastPosition)
    {
        m_lastPosition = coor.isValid() ? coor : QGeoCoordinate();
        emit coordinateChanged(m_lastPosition);
    }
}

QGeoCoordinate VGPlantInformation::lastCoordinate() const
{
    return m_lastPosition;
}

bool VGPlantInformation::operator==(const MapAbstractItem &item)const
{
    if (item.ItemType() != ItemType())
        return false;

    return true;
}

MapAbstractItem::MapItemType VGPlantInformation::ItemType() const
{
    return Type_PlantInfo;
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
}

VGPlantInformation::PlantStatus VGPlantInformation::GetStatus() const
{
    return m_stat;
}

void VGPlantInformation::SetStatus(PlantStatus s)
{
    if (m_stat != s)
    {
        m_stat = s;
        emit statChanged();
    }
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

    return strMode;
}
