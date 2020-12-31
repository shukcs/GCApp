#ifndef __VG_PLANTMANAGER_H__
#define __VG_PLANTMANAGER_H__

#include <QObject>
#include "VGPlantInformation.h"

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
        class OperationStatus;
        class GpsInformation;
        class UavAttitude;
        class UavStatus;
    }
}

class VGPlantManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(VGPlantInformation* currentPlant READ GetCurrentPlant WRITE SetCurrentPlant NOTIFY currentPlantChanged)//当前选中的飞机
    Q_PROPERTY(bool voiceHeight READ IsVoiceHeight WRITE SetVoiceHeight NOTIFY voiceHeightChanged)
    Q_PROPERTY(bool voiceSpeed READ IsVoiceSpeed WRITE SetVoiceSpeed NOTIFY voiceSpeedChanged)
    Q_PROPERTY(bool voiceVS READ IsVoiceVS WRITE SetVoiceVS NOTIFY voiceVSChanged)
    Q_PROPERTY(bool voiceSat READ IsVoiceSat WRITE SetVoiceSat NOTIFY voiceSatChanged)
public:
    explicit VGPlantManager(QObject *parent = 0);
    ~VGPlantManager();

    void  Insert(VGPlantInformation *info, int i=-1);
    void  InitialNetWork(VGNetManager *nm);
    bool IsVoiceHeight()const;
    bool IsVoiceSpeed()const;
    bool IsVoiceVS()const;
    bool IsVoiceSat()const;
    VGPlantInformation *GetCurrentPlant()const;
    void SetCurrentPlant(VGPlantInformation *pl);
    VGPlantInformation *GetPlantById(const QString &id);
public:
    static bool IsPlantId(const QString &id);
public slots:
    void onRecvBrotobuff(const google::protobuf::Message &msg);
    void onNetStatchanged(int stat);
protected:
    void prcsPlaneInfo(const das::proto::UavStatus &uav);
    void prcsOpStatus(const QString &id, const das::proto::OperationStatus &st);
    void prcsGpsInfo(const QString &id, const das::proto::GpsInformation &gpsInfo);
    void prcsUavAttitude(const QString &id, const das::proto::UavAttitude &attInfo);
    void _parseGPS(VGPlantInformation &pi, const das::proto::GpsInformation &gpsInfo);
    void requestUavInfo(const std::string &uav);
protected:
    void   SetVoiceHeight(bool b);
    void   SetVoiceSpeed(bool b);
    void   SetVoiceVS(bool b);
    void   SetVoiceSat(bool b);
    Q_INVOKABLE void writeConfig();
signals:
    void currentPlantChanged(VGPlantInformation *plant);

    void voiceHeightChanged(bool b);
    void voiceSpeedChanged(bool b);
    void voiceVSChanged(bool b);
    void voiceSatChanged(bool b);
protected slots:
    void onChildDestroyed(QObject *obj);
    void onPlantConnectChanged();
    void onSendUavErro(const QString &id, int res);
protected:
    void _readConfig();
    void _writeConfig();
    void _syncDeviceList();
    void _prcsPostInformation(const google::protobuf::Message &msg);
    void _prcsAckUavStatus(const google::protobuf::Message &msg);
    void _prcsAckBindUav(const google::protobuf::Message &msg);
    void _prcsDeviceList(const google::protobuf::Message &msg);
    void _prcsUpdateDevice(const google::protobuf::Message &msg);
private:
    QList <VGPlantInformation *>    m_palnts;
    VGPlantInformation              *m_plantCur;    //当前飞机
    bool                            m_bExistBound;
    bool                            m_bInitNet;
    unsigned                        m_voice;
    int                             m_seq;
};

#endif // __VG_PLANTMANAGER_H__
