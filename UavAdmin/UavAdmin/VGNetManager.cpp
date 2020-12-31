#include "VGNetManager.h"
#include <QDebug>
#include <QThread>
#include "VGApplication.h"
#include "VGPlantManager.h"
#include "VGPlantInformation.h"
#include "VGTcpClient.h"
#include "VGGlobalFun.h"
#include "das.pb.h"

using namespace das::proto;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//VGNetManager
///////////////////////////////////////////////////////////////////////////////////////////////////////
VGNetManager::VGNetManager() :QObject(), m_tcpClient(new VGTcpClient)
, m_seqno(0), m_port(0), m_tmAutoLoggin(-1), m_autoLoggin(true)
, m_nHeartTimes(0), m_connectState(2), m_nRegTip(2)
, m_thread(new QThread), m_bInitial(false)
{
    readConfig();

    m_heartTimer.setInterval(6 * 1000);
    connect(&m_heartTimer, &QTimer::timeout, this, &VGNetManager::sltHeartTimeout);
    if (m_thread && m_tcpClient)
    {
        m_tcpClient->moveToThread(m_thread);
        connect(m_tcpClient, &VGTcpClient::sigCommunicateResult, this, &VGNetManager::sltTcpResult);
        connect(m_tcpClient, &VGTcpClient::sigInfoAck, this, &VGNetManager::sltAckInfo);

        connect(this, &VGNetManager::sigSendRequestInfo, m_tcpClient, &VGTcpClient::sltSendRequestInfo);
        connect(this, &VGNetManager::sigConnectServer, m_tcpClient, &VGTcpClient::connetServer);
        m_thread->start();
        setConnectState(2);
    }
}

VGNetManager::~VGNetManager()
{
    m_thread->terminate();
    delete m_thread;
    delete m_tcpClient;
}

void VGNetManager::readConfig()
{
    if (QSettings *settings = qvgApp->GetSettings())
    {
        settings->beginGroup("config");
        m_autoLoggin = settings->value("autoLoggin", true).toBool();
        m_strServer = settings->value("serverIp", "101.200.55.175").toString();
        m_port = settings->value("port", "8198").toString().toInt();
        m_strUserId = settings->value("user", "").toString();
        m_strPassword = settings->value("password", "").toString();
        settings->endGroup();
        emit sigUserIdChanged(m_strUserId);
        _autoLoggin(m_autoLoggin);
    }
}

void VGNetManager::setConnectState(int state)
{
    m_connectState = state;
    emit connectStateChanged(state);
}

QString VGNetManager::GetCheck() const
{
    return m_strCheck;
}

void VGNetManager::logginServer()
{
    RequestGSIdentityAuthentication cmdRgsia;
    _checSeq();

    cmdRgsia.set_seqno(m_seqno);
    cmdRgsia.set_userid(m_strUserId.toUtf8().data());
    cmdRgsia.set_password(m_strPassword.toUtf8().data());
    SendProtobuff(cmdRgsia);
}

void VGNetManager::SetHostPort(quint16 port)
{
    m_port = port;
}

void VGNetManager::SetUserId(const QString &user)
{
    if (m_strUserId == user)
        return;

    m_strUserId = user;
    emit sigUserIdChanged(user);
}

QString VGNetManager::GetPassword() const
{
    return m_strPassword;
}

void VGNetManager::SetPassword(const QString &pswd)
{
    m_strPassword = pswd;
}

bool VGNetManager::IsAutoLoggin()const
{
    return m_autoLoggin;
}

void VGNetManager::SetAutoLoggin(bool bAuto)
{
    m_autoLoggin = bAuto;
    _autoLoggin(bAuto);
}

void VGNetManager::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != m_tmAutoLoggin)
        return QObject::timerEvent(e);

    if ( m_connectState != 0
      && m_connectState != 1
      && !m_strUserId.isEmpty()
      && !m_strPassword.isEmpty() )
        loggin();

    e->accept();
}

QString VGNetManager::GetHost() const
{
    return m_strServer;
}

void VGNetManager::SetHost(const QString &host)
{
    m_strServer = host;
}

quint16 VGNetManager::GetHostPort() const
{
    return m_port;
}

void VGNetManager::writeConfig(const QString &server, quint16 port, const QString &userId, const QString &password)
{
    if (QSettings *settings = qvgApp->GetSettings())
    {
        settings->beginGroup("config");
        settings->setValue("serverIp", server);
        settings->setValue("port", port);
        settings->setValue("user", userId);
        settings->setValue("password", password);
        settings->setValue("autoLoggin", m_autoLoggin);
        settings->endGroup();
    }
}

bool VGNetManager::connectServer()
{
    if (m_strServer.isEmpty()
        || m_port <= 0
        || m_connectState == 1
        || !m_tcpClient)
        return false;

    m_tcpClient->InitSock();
    writeConfig(m_strServer, m_port, m_strUserId, m_strPassword);
    setConnectState(0);
    emit sigConnectServer(m_strServer, m_port);
    return true;
}

void VGNetManager::_processLoggin(const QByteArray &byte)
{
    AckGSIdentityAuthentication ack;
    ack.ParseFromArray(byte.data(), byte.size());
    int result = ack.result();
    if (result == 1 && m_seqno == ack.seqno())
    {
        setConnectState(1);
        m_heartTimer.start();
    }
    else if (result != 1)
    {
        m_tcpClient->DisconnectTcp();
        setConnectState(-1);
        _autoLoggin(false);
    }
}

void VGNetManager::_processOperationInformation(const QByteArray &byte)
{
    static PostOperationInformation cmdPoi;
    if (cmdPoi.ParseFromArray(byte.data(), byte.size()))
        emit recvPlantProtobuf(cmdPoi);
}

void VGNetManager::_processPlantState(const QByteArray &byte)
{
    AckRequestUavStatus sAckRUS;                 //应答查询无人机状态
    if (sAckRUS.ParseFromArray(byte, byte.size()))
        emit recvPlantProtobuf(sAckRUS);
}

void VGNetManager::_processPlantBind(const QByteArray &byte)
{
    AckRequestBindUav ack;                 //应答绑定无人机
    if (ack.ParseFromArray(byte.data(), byte.size()))
        emit recvPlantProtobuf(ack);
    else
        qDebug() << "Parse protobuf is false";
}

void VGNetManager::_processDeviceList(const QByteArray &byte)
{
    AckSyncDeviceList devs;
    if (devs.ParseFromArray(byte.data(), byte.size()))
        emit recvPlantProtobuf(devs);
}

void VGNetManager::loggin()
{
    if (connectServer())
    {
        logginServer();
        _autoLoggin(true);
    }
}

void VGNetManager::disConnect()
{
    m_tcpClient->DisconnectTcp();
    setConnectState(2);
    _autoLoggin(false);
}

int VGNetManager::GetConnectState() const
{
    return m_connectState;
}

QString VGNetManager::GetUserId() const
{
    return m_strUserId;
}

void VGNetManager::sltTcpResult(bool connected)
{
    if (!connected)
        setConnectState(2);
}

void VGNetManager::sltAckInfo(const QString &name, const QByteArray &byte)
{
    if (name == d_p_ClassName(AckHeartBeat))
        m_nHeartTimes = 0;                  //心跳
    else if(name == d_p_ClassName(AckGSIdentityAuthentication))
        _processLoggin(byte);               //登陆结果
    else if(name == d_p_ClassName(PostOperationInformation))
        _processOperationInformation(byte);                  //GPS 位置信息
    else if(name == d_p_ClassName(AckRequestUavStatus))
        _processPlantState(byte);           //查询飞机状态结果
    else if(name == d_p_ClassName(AckRequestBindUav))
        _processPlantBind(byte);            //请求绑定、解除绑定应答;
    else if(name == d_p_ClassName(AckSyncDeviceList))
        _processDeviceList(byte);            //请求绑定、解除绑定应答;

    m_nHeartTimes = 0;
}

void VGNetManager::sltHeartTimeout()
{
    if(m_tcpClient && m_connectState == 1)
    {
        ++m_nHeartTimes;
        if (m_nHeartTimes < 3)
        {
            PostHeartBeat cmdPhb;
            _checSeq();
            cmdPhb.Clear();
            cmdPhb.set_seqno(m_seqno);
            cmdPhb.set_id(m_strUserId.toUtf8().data());
            SendProtobuff(cmdPhb);
            m_nHeartTimes++;
        }
        else
        {
            m_heartTimer.stop();
            m_nHeartTimes = 0;
            disConnect();
            _autoLoggin(true);
        }
    }
}

int VGNetManager::_getSendNo(const QString &name) const
{
    QMap<QString, int>::const_iterator itr = m_mapSendSeqs.find(name);
    if (itr != m_mapSendSeqs.end())
        return itr.value();

    return -1;
}

void VGNetManager::_setSendNo(const QString &name, int seq)
{
    m_mapSendSeqs[name] = seq;
}

void VGNetManager::requestUavInfo(const QGeoCoordinate &/*coor*/, double /*dis*/)
{
}

void VGNetManager::requestUavStatus(const QString &plantId)
{
    if(m_connectState != 1 || plantId.isEmpty())
        return;

    RequestUavStatus cmdRUS;
    _checSeq();
    cmdRUS.set_seqno(m_seqno);
    cmdRUS.add_uavid(plantId.toUtf8().data());
    SendProtobuff(cmdRUS);
}

void VGNetManager::requestBindUav(const QString &uavId, int type)
{
    if (m_connectState != 1 || uavId.isEmpty())
        return;

    RequestBindUav cmdRBS;
    _checSeq();
    cmdRBS.set_seqno(m_seqno);

    if(!uavId.isEmpty())
    {
        cmdRBS.set_uavid(uavId.toUtf8().data());
        cmdRBS.set_opid(type);
        cmdRBS.set_binder(m_strUserId.toUtf8().data());
    }
    else
    {
        return;
    }

    SendProtobuff(cmdRBS);
}

void VGNetManager::_autoLoggin(bool bAuto)
{
    if (bAuto && m_autoLoggin)
    {
        if (m_tmAutoLoggin > 0)
            return;

        m_tmAutoLoggin = startTimer(3000);
        QTimerEvent e(m_tmAutoLoggin);
        timerEvent(&e);
    }
    else
    {
        if (m_tmAutoLoggin < 0)
            return;

        killTimer(m_tmAutoLoggin);
        m_tmAutoLoggin = -1;
    }
}

void VGNetManager::_checSeq()
{
    m_seqno++;
    if (!m_seqno)
        m_seqno = 1;
}

void VGNetManager::SendProtobuff(const ::google::protobuf::Message &msg)
{
    int msglen = msg.ByteSize();
    QString name = msg.GetTypeName().c_str();
    QByteArray ay(msglen, 0);
    msg.SerializeToArray(ay.data(), msglen);
    emit sigSendRequestInfo(name, ay);
    _setSendNo(name, m_seqno);
}
