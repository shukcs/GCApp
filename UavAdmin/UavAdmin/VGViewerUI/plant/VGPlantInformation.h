#ifndef __VGMYPLANEINFORMATION_H__
#define __VGMYPLANEINFORMATION_H__

#include <QObject>
#include <QGeoCoordinate>
#include <QDateTime>
#include "MapAbstractItem.h"

class VGPlantInformation : public MapAbstractItem
{
    Q_OBJECT
    Q_ENUMS(PlantStatus)

    Q_PROPERTY(QString flightMode READ GetFlightMode NOTIFY flightModeChanged)
    Q_PROPERTY(QString planeId READ planeId NOTIFY planeIdChanged)
    Q_PROPERTY(bool binded READ IsBinded NOTIFY bindedChanged)
    Q_PROPERTY(QString master READ master  NOTIFY masterChanged)
    Q_PROPERTY(QGeoCoordinate coordinate READ lastCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(PlantStatus stat READ GetStatus NOTIFY statChanged)
public:
    enum PlantStatus {
        Unlogin,    //没有在线
        Logined,    //在线
        Monitor,    //在线监控
    };
public:
    explicit VGPlantInformation(const QString &id, QObject *parent = NULL);
    VGPlantInformation(const VGPlantInformation &oth);
    ~VGPlantInformation();

    QString planeId()const;
    void setPlaneId(const QString &id);

    bool IsBinded()const;
    void SetBinded(bool status);

    QString master()const;
    void setMaster(const QString &master);

    void setCoordinate(const QGeoCoordinate &coor);
    QGeoCoordinate lastCoordinate()const;

    QString GetFlightMode()const;
    void ChangeFlightMode(const QString &mod);

    PlantStatus GetStatus()const;
    void SetStatus(PlantStatus s);

    bool operator==(const MapAbstractItem &item)const;
    MapItemType ItemType()const;
public:
    static QString GetFlightModeDscb(const QString &str);
protected:
    bool IsRestrain()const;
signals:
    void flightModeChanged();
    void planeIdChanged(const QString &id);
    void bindedChanged(bool bBind);
    void masterChanged(const QString &master);
    void coordinateChanged(const QGeoCoordinate &c);
    void statChanged();
private:
    QString         m_planeId;          //飞机ID
    bool            m_bBind;            //0 未绑定 1 绑定
    PlantStatus     m_stat;
    QGeoCoordinate  m_lastPosition;     //最后一次通信的坐标
    QString         m_master;           //绑定者
    QString         m_flightMode;       //飞行模式
};

#endif // VGMYPLANEINFORMATION_H
