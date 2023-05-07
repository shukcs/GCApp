﻿#ifndef __VG_PLANTMANAGER_H__
#define __VG_PLANTMANAGER_H__

#include <QObject>
#include "VGPlantInformation.h"
#include "mavlink_types.h"

//QML飞机管理
class VGNetManager;
class QGeoCoordinate;
class VGVehicleMission;

namespace google {
    namespace protobuf {
        class Message;
    }
}
namespace das {
    namespace proto {
        class UavStatus;
        class OperationStatus;
        class GpsInformation;
        class UavAttitude;
        class PostOperationInformation;
        class AckMissionSuspend;
        class PostStatus2GroundStation;
        class SyscOperationRoutes;
        class PostOperationAssist;
        class PostABPoint;
        class PostOperationReturn;
        class PostBlocks;
    }
}
class VGPlantManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(VGPlantInformation*  surveyUav READ GetSurveyUav WRITE SetSurveyUav NOTIFY surveyUavChanged) //绑定飞机
    Q_PROPERTY(VGPlantInformation* currentPlant READ GetCurrentPlant WRITE SetCurrentPlant NOTIFY currentPlantChanged)//当前选中的飞机
    Q_PROPERTY(bool oneKeyArm READ IsOneKeyArm WRITE SetOneKeyArm NOTIFY oneKeyArmChanged)
    Q_PROPERTY(bool voiceHeight READ IsVoiceHeight WRITE SetVoiceHeight NOTIFY voiceHeightChanged)
    Q_PROPERTY(bool voiceSpeed READ IsVoiceSpeed WRITE SetVoiceSpeed NOTIFY voiceSpeedChanged)
    Q_PROPERTY(bool voiceVS READ IsVoiceVS WRITE SetVoiceVS NOTIFY voiceVSChanged)
    Q_PROPERTY(bool voiceSat READ IsVoiceSat WRITE SetVoiceSat NOTIFY voiceSatChanged)
    Q_PROPERTY(bool connected READ IsConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool imGnd READ IsImitateGround NOTIFY imGndChanged)
    Q_PROPERTY(QStringList mntPants READ GetMntPants NOTIFY mntPantsChanged)
    Q_PROPERTY(float tmRoit READ GetRoitTime WRITE SetRoitTime NOTIFY tmRoitChanged)
    Q_PROPERTY(bool  uTrance READ IsUTrance WRITE SetUTrance NOTIFY uTranceChanged)
    Q_PROPERTY(bool sps READ IsSpsPermission NOTIFY spsChanged) //单点作业
    Q_PROPERTY(bool qxsdk READ IsQXUseSdk NOTIFY qxsdkChanged)  //QX使用SDK
public:
    explicit VGPlantManager(QObject *parent = 0);
    ~VGPlantManager();

    Q_INVOKABLE void addNewPlant(const QString &planeId, bool bMnt = false);
    Q_INVOKABLE void clearTmpPlant();
    Q_INVOKABLE void searchPlant(QGeoCoordinate &coor, double dis = 200);
    Q_INVOKABLE void remove(VGPlantInformation *pl);
    Q_INVOKABLE void testPump();
    Q_INVOKABLE void testMotor(int idx, int msTm = 1000, int speed = 1150);
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void disarm();
	Q_INVOKABLE void magnetic();
    Q_INVOKABLE bool setParameter(const QString &id, const QString &val);
    Q_INVOKABLE void setParameterIndex(const QString &id, int idx);
    Q_INVOKABLE QString getParamValue(const QString &key) const;
    Q_INVOKABLE int getParamIndex(const QString &key) const;
    Q_INVOKABLE void clear();

    void    startQueryMyPlaneStatus();
    void    stopQueryMyPlaneStatus();
    VGPlantInformation *GetPlantViaDrect()const;
    void    ViaDrectPlantCreate(const QString &mode);
    void    RemoveViaCloudPlant(const QString &planeId);
    VGPlantInformation *GetPlantById(const QString &id);
    void  Insert(VGPlantInformation *info, int i=-1);
    void  InitialNetWork(VGNetManager *nm);
    void  SetCurPlant(const QString &id);
    bool IsConnected()const;
    void syncPlanFlight(VGVehicleMission *vm);
    bool IsVoiceHeight()const;
    bool IsVoiceSpeed()const;
    bool IsVoiceVS()const;
    bool IsVoiceSat()const;
    bool IsOneKeyArm()const;
    void DealParam(VGPlantInformation *info, const QString &id, const QVariant &var);
    bool IsImitateGround()const;
    bool IsSpsPermission()const;
    bool IsQXUseSdk()const;
    void ControllerCalibrat();
    float GetRoitTime()const;
    double GetRoitRadius(double sprayWidth)const;
    VGPlantInformation *GetCurrentPlant()const;

    void prcsUavMavlink(VGPlantInformation *info, const mavlink_message_t &msg, bool finish);
public:
    static bool IsPlantId(const QString &id);
protected:
    void timerEvent(QTimerEvent *event);
    void SetCurrentPlant(VGPlantInformation *pl);
    VGPlantInformation *GetBoundPlant()const;

    VGPlantInformation *_getPlantByIdAndUser(const QString &id, const QString &user);
    VGPlantInformation * GetSurveyUav()const;
    void SetSurveyUav(VGPlantInformation *);
    void SetOneKeyArm(bool b);
    void SetVoiceHeight(bool b);
    void SetVoiceSpeed(bool b);
    void SetVoiceVS(bool b);
    void SetVoiceSat(bool b);
    void SetRoitTime(float f);
    bool IsUTrance()const;
    void SetUTrance(bool b);
    const QStringList &GetMntPants()const;
    Q_INVOKABLE void writeConfig();
    Q_INVOKABLE void setQxAccount(const QString &acc, const QString &pswd);
    Q_INVOKABLE void getQxAccount();
    Q_INVOKABLE void clearQxAccount();
    Q_INVOKABLE void setQxSdkAccount(const QString &key, const QString &secret,  const QString &devid, int tp);
    Q_INVOKABLE void getQxSdkAccount();
    Q_INVOKABLE void clearQxSdkAccount();

    void processOperationInformation(const das::proto::PostOperationInformation &pi);
    void processMavlink(const das::proto::PostStatus2GroundStation &mav);
    void processSuspend(const das::proto::AckMissionSuspend &ack);
    void processSyscRoute(const das::proto::SyscOperationRoutes &sr);
    void processUavStatus(const das::proto::UavStatus &us);
    void processAssists(const das::proto::PostOperationAssist &as);
    void processABPoints(const das::proto::PostABPoint &ab);
    void processReturn(const das::proto::PostOperationReturn &ab);
    void processBlocks(const das::proto::PostBlocks &blcs);

    void prcsOpStatus(const QString &id, const das::proto::OperationStatus &st);
    void prcsGpsInfo(const QString &id, const das::proto::GpsInformation &gpsInfo);
    void prcsVGUavAttitude(const QString &id, const das::proto::UavAttitude &attInfo);
signals:
    void surveyUavChanged();
    void currentPlantChanged();

    void oneKeyArmChanged(bool b);
    void voiceHeightChanged(bool b);
    void voiceSpeedChanged(bool b);
    void voiceVSChanged(bool b);
    void voiceSatChanged(bool b);
    void connectedChanged(bool b);
    void imGndChanged();
    void spsChanged();
    void qxsdkChanged();
    void paramChanged(const QString &idStr);
    void mntPantsChanged();
    void uTranceChanged();
    void tmRoitChanged();
protected slots:
    void onNetStatchanged(int stat);
    void onChildDestroyed(QObject *obj);
    void onPlantConnectChanged();
    void onSendUavErro(const QString &id, int res);
    void onReceiveProto(const google::protobuf::Message &msg);
protected:
    void _readConfig();
    void _writeConfig();
    void _addMonitorPant(const QString &id);

    void _prcsCmdAck(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsMissionCur(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsAttitude(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsParamVal(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsMissionCount(VGPlantInformation &p, const mavlink_message_t &m, bool b);
    void _prcsMissionItem(VGPlantInformation &p, const mavlink_message_t &m, bool b);
    void _prcsHome(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsUavEvent(const QString &id, const mavlink_message_t &m);
    void _prcsQXAccount(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsQXCmd(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsQXSdkAcc(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsQXSdkCmd(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsRcChannels(VGPlantInformation &p, const mavlink_message_t &m);
    void _prcsSupport(VGPlantInformation &p, const mavlink_message_t &msg);
    void _prcsSprayVal(VGPlantInformation &p, const mavlink_message_t &msg);
private:
    QList <VGPlantInformation *>    m_plants;
    VGPlantInformation              *m_plantSurvey;                //当前踩点飞机
    VGPlantInformation              *m_plantCur;                   //当前飞机
    bool                            m_bExistBound;
    bool                            m_bInitNet;
    QStringList                     m_mntPants;
    QString                         m_strCur;
    int                             m_idTimerQureryPlant;
    bool                            m_bOneKeyArm;
    bool                            m_bMission;
    unsigned                        m_voice;
    float                           m_tmRoit;
    bool                            m_bUTrance;
};

#endif // __VG_PLANTMANAGER_H__
