#ifndef __VG_MISSIONRECORD_H__
#define __VG_MISSIONRECORD_H__

#include <QObject>
#include <QGeoCoordinate>

class VGMissionRecord : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString landId READ GetLandID CONSTANT)
    Q_PROPERTY(QString planId READ GetPlanID CONSTANT)
    Q_PROPERTY(QString user READ GetUser CONSTANT)
    Q_PROPERTY(QString uav READ GetUav CONSTANT)
    Q_PROPERTY(QString finishTime READ GetFinishTime CONSTANT)
    Q_PROPERTY(float acreage READ GetAcreage CONSTANT)
    Q_PROPERTY(int begRidge READ GetBegRidgee CONSTANT)
    Q_PROPERTY(int endRidge READ GetEndRidgee CONSTANT)
    Q_PROPERTY(QGeoCoordinate suspend READ GetSuspend CONSTANT)
    Q_PROPERTY(QString mode READ GetMode CONSTANT)
    Q_PROPERTY(QString date READ GetDate CONSTANT)
    Q_PROPERTY(QString time READ GetTime CONSTANT)
public:
    VGMissionRecord(QObject *p=NULL);
    VGMissionRecord(const QString &uav, const QString &plan, const QString &land, const QString &user,
                    qint64 time, float acreage, int begRidgee,int endRidgee,const QGeoCoordinate & coor);
    QString GetLandID()const;
    QString GetPlanID()const;
    QString GetUser()const;
    QString GetUav()const;
    QString GetFinishTime()const;
    float GetAcreage()const;
    int GetBegRidgee()const;
    int GetEndRidgee()const;
    QGeoCoordinate GetSuspend()const;
    QString GetMode()const;
    QString GetDate() const;
    QString GetTime() const;


    void SetLandID(const QString & ID);
    void SetPlanID(const QString & ID);
    void SetUser(const QString & user);
    void SetUav(const QString & uav);
    void SetFinishTime(qint64 time);
    void SetAcreage(float acreage);
    void SetBegRidgee(int ridgee);
    void SetEndRidgee(int ridgee);
    void SetSuspend(const QGeoCoordinate & coor);

private:
    qint64          m_finishTm;
    float           m_acreage;
    int             m_beg;
    int             m_end;
    QString         m_landID;
    QString         m_planID;
    QString         m_user;
    QString         m_uav;
    QGeoCoordinate  m_suspend;
};

#endif // __VG_MISSIONRECORD_H__
