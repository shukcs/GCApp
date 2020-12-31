#ifndef VGMAPCONTROL_H
#define VGMAPCONTROL_H

#include <QtPlugin>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "VGPlantInformation.h"

class QThread;
class VGTcpClient;

namespace google {
    namespace protobuf {
        class Message;
    }
}

namespace das {
    namespace proto {
        class OperationStatus;
        class ParcelSurveyInformation;
        class Outline;
        class GpsInformation;
        class UavProductInfo;
        class ParcelDescription;
        class OperationDescription;
        class OperationPlan;
        class OperationRoute;
        class UavAttitude;
        class OperationParams;
        class UavStatus;
    }
}

#define d_p_ClassName(_cls) QString(_cls::descriptor()->full_name().c_str())
class VGNetManager:public QObject
{
    Q_OBJECT
    //tcp连接是否正常
    //0 正在连接， 1 连接成功，2 未连接，-1 连接失败；
    Q_PROPERTY(int connectState READ GetConnectState NOTIFY connectStateChanged)
    Q_PROPERTY(QString host READ GetHost WRITE SetHost NOTIFY connectChanged)
    Q_PROPERTY(quint16 port READ GetHostPort WRITE SetHostPort NOTIFY connectChanged)
    Q_PROPERTY(QString userId READ GetUserId WRITE SetUserId NOTIFY sigUserIdChanged NOTIFY connectChanged)
    Q_PROPERTY(QString password READ GetPassword WRITE SetPassword NOTIFY connectChanged)
    Q_PROPERTY(QString check READ GetCheck  NOTIFY checkChanged)
    Q_PROPERTY(bool autoLoggin READ IsAutoLoggin WRITE SetAutoLoggin NOTIFY connectChanged)
public:
    VGNetManager();
    ~VGNetManager();

    //登陆服务器
	Q_INVOKABLE void loggin();
    Q_INVOKABLE void disConnect();          //客户端主动断开
public:
    int GetConnectState()const;
    QString GetUserId()const;

    //无人机; type 0: 解除, 1: 绑定
    void    requestBindUav(const QString &uavId, int type);
    void    requestUavInfo(const QGeoCoordinate &coor, double dis);
    void    requestUavStatus(const QString &lstPlaneId);
    void    SendProtobuff(const google::protobuf::Message &msg);
protected:
    void    timerEvent(QTimerEvent *event);
    //登录
    QString GetHost()const;
    void    SetHost(const QString &host);
    quint16 GetHostPort()const;
    void    SetHostPort(quint16 port);
    void    SetUserId(const QString &user);
    QString GetPassword()const;
    void    SetPassword(const QString &pswd);
    bool    IsAutoLoggin()const;
    void    SetAutoLoggin(bool bAuto);
    //登陆状态（用户名和密码验证是否通过）
    void    setConnectState(int state);

    QString GetCheck()const;
    void logginServer();
private:
    //服务器数据
    void _processLoggin(const QByteArray &byte);
    void _processOperationInformation(const QByteArray &byte);
    void _processPlantState(const QByteArray &byte);
    void _processPlantBind(const QByteArray &byte);
    void _processDeviceList(const QByteArray &byte);
signals:
    void sigConnectServer(const QString &, int port, int = 3000, const QString & = "");
    void sigSendRequestInfo(const QString &, const QByteArray &arr);
    void connectStateChanged(int state);
    void recvLandProtobuf(const google::protobuf::Message &msg);
    void recvPlantProtobuf(const google::protobuf::Message &msg);

    //无人机状态查询结果
    void plantQueryed(const VGPlantInformation &pUavInfo);
    void sigUserIdChanged(const QString &usr);
    void connectChanged();
    void sendUavErro(const QString &uav, int res);
    void checkChanged();
    void regTipChanged();
    void registed();
private slots:
    void    sltTcpResult(bool connected);
    void    sltAckInfo(const QString &name, const QByteArray &byte);
    void    sltHeartTimeout();
private:
    void    readConfig();
    void    writeConfig(const QString &server, quint16 port, const QString &userId, const QString &password);
    bool    connectServer();//tcp连接
    //自动登陆
    void    _autoLoggin(bool bAuto);
    void    _checSeq();
    int     _getSendNo(const QString &name)const;
    void    _setSendNo(const QString &name, int seq);
private:
    VGTcpClient*        m_tcpClient;
    quint16             m_port;
    int                 m_nRegTip;
    int                 m_connectState;      //连接状态, 0 正在登陆， 1，成功， -1 登陆失败， 2 断开状态
    bool                m_autoLoggin;        //是否自动重连
    int                 m_tmAutoLoggin;     //自动登录定时器
    int                 m_nHeartTimes;      //心跳超时次数
    unsigned int        m_seqno;
    bool                m_bInitial;         //是否已获取作业
	QString             m_strServer;
	QString             m_strUserId;
    QString             m_strPassword;
    QString             m_strCheck;
    QStringList         m_strPdids;
	QTimer              m_heartTimer;
	QThread             *m_thread;
	QString             m_curUploadPlantID;     //当前上传的地块ID（本地ID）

    DescribeMap         m_mapDesUpload;
    QMap<QString, int> m_mapSendSeqs;
};

#endif // VGMAPCONTROL_H
