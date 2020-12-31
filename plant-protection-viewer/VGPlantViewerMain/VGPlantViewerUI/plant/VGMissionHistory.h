#ifndef __VG_MISSIION_HISTORY_H__
#define __VG_MISSIION_HISTORY_H__

#include <QObject>
#include <QDateTime>

namespace google {
    namespace protobuf {
        class Message;
    }
}
class QmlObjectListModel;
class VGNetManager;
class VGMissionHistory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QmlObjectListModel *histories READ GetHistories CONSTANT)
    Q_PROPERTY(QString begTime READ GetBeginTime WRITE SetBeginTime NOTIFY begTimeChanged)
    Q_PROPERTY(QString endTime READ GetEndinTime WRITE SetEndinTime NOTIFY endTimeChanged)
    Q_PROPERTY(QString allArea READ GetAllArea NOTIFY allAreaChanged)
    Q_PROPERTY(QString uav READ GetUav WRITE SetUav NOTIFY uavChanged)
    Q_PROPERTY(bool valid READ IsValid NOTIFY validChanged)
    Q_PROPERTY(QString msTip READ GetMsTip NOTIFY msTipChanged)
public:
    VGMissionHistory(QObject *p=NULL);

    QmlObjectListModel *GetHistories()const;
    void InitialNet(VGNetManager *nm);
public slots:
    void OnRecvRecord(const google::protobuf::Message &msg);
protected:
    QString GetBeginTime()const;
    void SetBeginTime(const QString &t);
    QString GetEndinTime()const;
    void SetEndinTime(const QString &t);
    float GetAllArea()const;
    QString GetUav()const;
    void SetUav(const QString &uav);
    bool IsValid()const;
    QString GetMsTip()const;

    Q_INVOKABLE void reqMissionArea();
    Q_INVOKABLE void reqDetail();
signals:
    void begTimeChanged();
    void endTimeChanged();
    void allAreaChanged();
    void uavChanged();
    void validChanged();
    void msTipChanged();
private:
    QmlObjectListModel  *m_records;
    float               m_areaAll;
    bool                m_valid;
    int                 m_seq;
    QString             m_uav;
    QString             m_msTip;
    QDateTime           m_begTime;
    QDateTime           m_endTime;
};

#endif // __VG_MISSIION_HISTORY_H__
