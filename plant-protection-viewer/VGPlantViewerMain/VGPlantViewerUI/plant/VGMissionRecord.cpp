#include "VGMissionRecord.h"
#include <QDateTime>

VGMissionRecord::VGMissionRecord(QObject *p/*=NULL*/) : QObject(p)
, m_finishTm(0), m_acreage(0), m_beg(0), m_end(0)
{
}

VGMissionRecord::VGMissionRecord(const QString &uav, const QString &plan, const QString &land,
const QString &user, qint64 time, float acreage, int begRidgee,int endRidgee, const QGeoCoordinate &coor)
    :m_uav(uav),m_planID(plan),m_landID(land),m_user(user),m_finishTm(time),m_acreage(acreage),
      m_end(endRidgee),m_beg(begRidgee),m_suspend(coor)
{
}

QString VGMissionRecord::GetLandID() const
{
    return m_landID;
}

QString VGMissionRecord::GetPlanID() const
{
    return m_planID;
}

QString VGMissionRecord::GetUser() const
{
    return m_user;
}

QString VGMissionRecord::GetUav() const
{
    return m_uav.right(8);
}

QString VGMissionRecord::GetFinishTime() const
{
    return QDateTime::fromMSecsSinceEpoch(m_finishTm).toString("yyyy-MM-dd h:m:s");
}

float VGMissionRecord::GetAcreage() const
{
    return m_acreage;
}

int VGMissionRecord::GetBegRidgee() const
{
    return m_beg;
}

int VGMissionRecord::GetEndRidgee() const
{
    return m_end;
}

QGeoCoordinate VGMissionRecord::GetSuspend() const
{
    return m_suspend;
}

QString VGMissionRecord::GetMode() const
{
    return m_planID.isEmpty()?tr("AB-point mode"):tr("Autonomy");
}

QString VGMissionRecord::GetDate() const
{
    return QDateTime::fromMSecsSinceEpoch(m_finishTm).toString("yyyy-MM-dd");
}

QString VGMissionRecord::GetTime() const
{
    return QDateTime::fromMSecsSinceEpoch(m_finishTm).toString("h:m:s");
}

void VGMissionRecord::SetLandID(const QString & ID)
{
    m_landID = ID;
}

void VGMissionRecord::SetPlanID(const QString & ID)
{
    m_planID = ID;
}

void VGMissionRecord::SetUser(const QString & user)
{
    m_user = user;
}

void VGMissionRecord::SetUav(const QString & uav)
{
   m_uav= uav;
}

void VGMissionRecord::SetFinishTime(qint64 time)
{
    m_finishTm = time;
}

void VGMissionRecord::SetAcreage(float acreage)
{
    m_acreage = acreage;
}

void VGMissionRecord::SetBegRidgee(int ridgee)
{
    m_beg = ridgee;
}

void VGMissionRecord::SetEndRidgee(int ridgee)
{
    m_end = ridgee;
}

void VGMissionRecord::SetSuspend(const QGeoCoordinate & coor)
{
    m_suspend = coor;
}
