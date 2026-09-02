#include "VGNetManager.h"
#include <QDebug>
#include <QThread>
#include "VGGlobalFun.h"
#include "VGMacro.h"
#include "VGLandInformation.h"
#include "VGCoordinate.h"
#include "VGLandPolygon.h"
#include "VGLandPolyline.h"
#include "VGFlyRoute.h"
#include "VGLandBoundary.h"
#include "VGApplication.h"
#include "VGGlobalFun.h"
#include "VGMavLinkCode.h"
#include "MissionItem.h"
#include "VGPlantManager.h"
#include "VGPlantInformation.h"
#include "VGVehicleMission.h"
#include "friends/VGFriend.h"
#include "das.pb.h"

#define d_p_ClassName(_cls) QString(_cls::descriptor()->full_name().c_str())
#define INTCOORLATORLONPOW   1e7

using namespace das::proto;

static void _toPolygon(const Outline &ol, VGLandPolygon &plg)
{
    int nCount = ol.coordinates_size();
    for (int i = 0; i < nCount; i++)
    {
        Coordinate coordinateOutline = ol.coordinates(i);
        QGeoCoordinate coordinate;
        coordinate.setLatitude(double(coordinateOutline.latitude()) / INTCOORLATORLONPOW);
        coordinate.setLongitude(double(coordinateOutline.longitude()) / INTCOORLATORLONPOW);
        coordinate.setAltitude(double(coordinateOutline.altitude()) / 1e3);

        int tp = plg.GetId() == VGLandPolygon::Boundary ? VGCoordinate::Boundary : VGCoordinate::BlockBoundary;   
        plg.AddCoordinate(coordinate, tp);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
//VGNetManager
///////////////////////////////////////////////////////////////////////////////////////////////////////
VGNetManager::VGNetManager() :QObject(), m_tcpClient(new VGTcpClient)
, m_seqno(0), m_port(0), m_tmAutoLoggin(-1), m_autoLoggin(true)
, m_nHeartTimes(0), m_connectState(2), m_nRegTip(2)
, m_thread(new QThread), m_bInitial(false), m_auth(0)
{
    readConfig();

    m_heartTimer.setInterval(HEART_INTERVAL * 1000);
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
    if (m_tcpClient)
        m_tcpClient->deleteLater();
    if (m_thread)
    {
        m_thread->terminate();
        m_thread->deleteLater();
    }
}

void VGNetManager::readConfig()
{
    if (QSettings *settings = qvgApp->GetSettings())
    {
        settings->beginGroup("config");
        m_autoLoggin = settings->value("autoLoggin", true).toBool();
        m_strServer = settings->value("serverIp", "101.200.55.175").toString();
        m_port = settings->value("port", "18198").toString().toInt();
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
    if (m_connectState == 1 && !m_bInitial)
        requestLandinfo(m_strUserId);
}

QString VGNetManager::GetCheck() const
{
    return m_strCheck;
}

int VGNetManager::GetRegTip() const
{
    return m_nRegTip;
}

void VGNetManager::registerUser(const QString &user, const QString &pswd, const QString &check)
{
    if(connectServer())
    {
        m_strUserId = user;
        RequestNewGS gs;
        _checSeq();
        gs.set_seqno(m_seqno);
        gs.set_userid(user.toUtf8().data());
        if (!pswd.isEmpty() && !check.isEmpty())
        {
            m_strPassword = pswd;
            gs.set_password(pswd.toUtf8().data());
            gs.set_check(check.toUtf8().data());
        }

        SendNetWork(gs);
    }
}

QString VGNetManager::regTipStr(int tip)
{
    switch (tip)
    {
    case -2:
        return tr("User not fit");//"用户名不合法"
    case -1:
        return tr("server error");//服务器出错
    case 0:
        return tr("User exist");//"用户名已存在"
    case 1:
        return tr("User OK");//"用户名可用"
    default:
        return tr("letter,number,_ combine(3-24)");//"字母,数字,_组合(3-24)
    }
}

void VGNetManager::logginServer()
{
    RequestGSIdentityAuthentication cmdRgsia;
    _checSeq();

    cmdRgsia.set_seqno(m_seqno);
    cmdRgsia.set_userid(m_strUserId.toUtf8().data());
    cmdRgsia.set_password(m_strPassword.toUtf8().data());
    SendNetWork(cmdRgsia);
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
    emit autoLogginChanged();
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
    if (result == 1)
    {
        setConnectState(1);
        m_heartTimer.start();
        m_auth = ack.auth();
    }
    else if (result != 1)
    {
        m_tcpClient->DisconnectTcp();
        setConnectState(-1);
        _autoLoggin(false);
    }
}

void VGNetManager::_processDelPDAck(const QByteArray &)
{
}

void VGNetManager::_processPostInfo(const QByteArray &byte)
{
    PostOperationInformation cmdPoi;
    if (cmdPoi.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(cmdPoi);
}

void VGNetManager::_processUploadSurveyResult(const QByteArray &byte)
{
    static AckPostParcelDescription sAckPpdes;
    if (sAckPpdes.ParseFromArray(byte, byte.size()))
    {
        int result = sAckPpdes.result();
        if (sAckPpdes.seqno() == _getSendNo(d_p_ClassName(PostParcelDescription)) && result==1)
        {
            QString surveyId = QString(sAckPpdes.pdid().c_str());
            QString boundaryId = QString(sAckPpdes.psiid().c_str());
            QString ownerId = QString(sAckPpdes.pcid().c_str());

            m_mapDesUpload[LandID] = surveyId;
            m_mapDesUpload[BoundaryId] = boundaryId;
            emit sigSaveResult(m_mapDesUpload);
        }
        m_mapDesUpload.clear();
    }
}

void VGNetManager::_processSurveyLands(const QByteArray &byte)
{
    AckRequestParcelDescriptions sAckPDess;
    if (sAckPDess.ParseFromArray(byte.data(), byte.size()))
    {
        if (sAckPDess.result() != 1)
            return;

        for (int i = 0; i < sAckPDess.pds_size(); i++)
        {
            parseLand(sAckPDess.pds(i));
        }
        if (!m_bInitial)
            requestPlanning();
        m_bInitial = true;
    }
}

void VGNetManager::_processOparetionPlan(const QByteArray &byte)
{
    AckRequestOperationDescriptions ackRODess;  //应答作业描述信息
    if (ackRODess.ParseFromArray(byte.data(), byte.size()))
    {
        if (ackRODess.result() != 1)
            return;

        int odsCount = ackRODess.ods_size();
        for (int i = 0; i < odsCount; i++)
        {
            DescribeMap dsc;
            if (VGFlyRoute *fr = _parseFlyRoute(ackRODess.ods(i), dsc))
                emit sigQueryItem(fr, dsc);
        }
    }
}

void VGNetManager::_processUploadOperationPlan(const QByteArray &byte)
{
    AckPostOperationDescription ackPor;          //上传规划应答
    if (ackPor.ParseFromArray(byte.data(), byte.size()))
    {
        int result = ackPor.result();
        if (result == 1)
            m_mapDesUpload["routeId"] = ackPor.odid().c_str();

        emit sigSaveResult(m_mapDesUpload);
        m_mapDesUpload.clear();
    }
}

void VGNetManager::_processPlantState(const QByteArray &byte)
{
    static AckRequestUavStatus sAckRUS;                 //应答查询无人机状态
    if (sAckRUS.ParseFromArray(byte, byte.size()))
    {
        int uavSize = sAckRUS.status_size();
        for (int i = 0; i < uavSize; i++)
        {
            _prcsPBUavStat(sAckRUS.status(i));
        }
    }
}

void VGNetManager::_processPlantBind(const QByteArray &byte)
{
    AckRequestBindUav ack;                 //应答绑定无人机
    if (ack.ParseFromArray(byte.data(), byte.size()))
    {
        if (ack.has_status())
            _prcsPBUavStat(ack.status());

        return;
    }
    qDebug() << "Parse protobuf is false";
}

void VGNetManager::_processSysAck(const QByteArray &byte)
{
    SyscOperationRoutes ack;       //向无人机同步指定航线应答
    if (ack.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(ack);
}

void VGNetManager::_processAckPostControl2Uav(const QByteArray &byte)
{
    AckPostControl2Uav sAck;
    if (sAck.ParseFromArray(byte.data(), byte.size()))
    {
        if (sAck.result() != 1)
        {
            QString strId = QString::fromUtf8(sAck.uavid().c_str());
            emit sendUavErro(strId, sAck.result());
        }
    }
}

void VGNetManager::_processUav2Gc(const QByteArray &byte)
{
    PostStatus2GroundStation uav2Gc;
    if (uav2Gc.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(uav2Gc);
}

void VGNetManager::_processAckControl2Uav(const QByteArray &byte)
{
    AckPostControl2Uav ack;
    if (ack.ParseFromArray(byte.data(), byte.size()))
    {
    }
}

void VGNetManager::_processAckNewGS(const QByteArray &byte)
{
    AckNewGS ackGs;
    if (ackGs.ParseFromArray(byte.data(), byte.size()))
    {
        m_strCheck.clear();
        if (ackGs.has_check())
        {
            m_strCheck = ackGs.check().c_str();
            emit checkChanged();
        }
        if (m_nRegTip != ackGs.result())
        {
            m_nRegTip = ackGs.result();
            emit regTipChanged();
        }
        if (m_nRegTip == 3)
            qvgApp->SetQmlTip(tr("Code error"));//"验证码错误"
        else if (m_strCheck.isEmpty() && 1 == m_nRegTip)
            registerSuc();
    }
}

void VGNetManager::_processAckFriends(const QByteArray &byte)
{
    AckFriends ackFriends;                 //应答查询无人机状态
    if (ackFriends.ParseFromArray(byte, byte.size()))
        emit receiveGSMsg(ackFriends);
}

void VGNetManager::_processAckGSMsg (const QByteArray &byte)
{
    AckGroundStationsMessage  recvAckMsg;
    if (recvAckMsg.ParseFromArray(byte.data(), byte.size()))
        emit receiveGSMsg(recvAckMsg);
}

void VGNetManager::_processAckSuspend(const QByteArray &byte)
{
    AckMissionSuspend  ackSuspend;
    if (ackSuspend.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(ackSuspend);
}

void VGNetManager::_processAckUavMission(const QByteArray &byte)
{
    AckUavMission  ack;
    if (ack.ParseFromArray(byte.data(), byte.size()))
        emit receiveMissionInfo(ack);
}

void VGNetManager::_processAckMissionAcreage(const QByteArray &byte)
{
    AckUavMissionAcreage  ack;
    if (ack.ParseFromArray(byte.data(), byte.size()))
        emit receiveMissionInfo(ack);
}

void VGNetManager::_processPostAssist(const QByteArray &byte)
{
    PostOperationAssist poa;
    if (poa.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(poa);
}

void VGNetManager::_processPostABPoint(const QByteArray &byte)
{
    PostABPoint pab;
    if (pab.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(pab);
}

void VGNetManager::_processPostReturn(const QByteArray &byte)
{
    PostOperationReturn por;
    if (por.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(por);
}

void VGNetManager::_processPostBlocks(const QByteArray &byte)
{
    PostBlocks blocks;
    if (blocks.ParseFromArray(byte.data(), byte.size()))
        emit receiveUavProto(blocks);
}

void VGNetManager::_processReceiveGSMsg(const QByteArray &byte)
{
    GroundStationsMessage  recMsg;
    if (recMsg.ParseFromArray(byte.data(), byte.size()))
        emit receiveGSMsg(recMsg);
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
    else if (name == d_p_ClassName(AckDeleteParcelDescription))
        _processDelPDAck(byte);             //删除地块
    else if(name == d_p_ClassName(PostOperationInformation))
        _processPostInfo(byte);                  //GPS 位置信息
    else if(name == d_p_ClassName(AckPostParcelDescription))
        _processUploadSurveyResult(byte);   //上传测绘信息结果
    else if(name == d_p_ClassName(AckRequestParcelDescriptions))
		_processSurveyLands(byte);          //地块描述信息
    else if(name == d_p_ClassName(AckPostOperationDescription))
        _processUploadOperationPlan(byte);   //上传规划结果
    else if(name == d_p_ClassName(AckRequestOperationDescriptions))
        _processOparetionPlan(byte);        //下载规划
    else if(name == d_p_ClassName(AckPostOperationRoute))
        _processUploadMissionResult(byte); //上传航线结果
    else if(name == d_p_ClassName(SyscOperationRoutes))
        _processSysAck(byte);               //同步航线结果
    else if(name == d_p_ClassName(AckRequestUavStatus))
        _processPlantState(byte);           //查询飞机状态结果
    else if(name == d_p_ClassName(AckRequestBindUav))
        _processPlantBind(byte);            //请求绑定、解除绑定应答
    else if (name == d_p_ClassName(AckPostControl2Uav))
        _processAckControl2Uav(byte);
    else if (name == d_p_ClassName(PostStatus2GroundStation))
        _processUav2Gc(byte);
    else if (name == d_p_ClassName(AckNewGS))
        _processAckNewGS(byte);
    else if (name == d_p_ClassName(AckFriends))
        _processAckFriends(byte);
    else if (name == d_p_ClassName(AckGroundStationsMessage ))
        _processAckGSMsg(byte);
    else if (name == d_p_ClassName(GroundStationsMessage))//接收到的消息
        _processReceiveGSMsg(byte);
    else if (name == d_p_ClassName(AckMissionSuspend))//接收到的消息
        _processAckSuspend(byte);
    else if (name == d_p_ClassName(AckUavMission))//接收详细作业
        _processAckUavMission(byte);
    else if (name == d_p_ClassName(AckUavMissionAcreage))//接收作业面积
        _processAckMissionAcreage(byte);
    else if (name == d_p_ClassName(AckUavMission))//接收详细作业
        _processAckUavMission(byte);
    else if (name == d_p_ClassName(AckUavMissionAcreage))//接收作业面积
        _processAckMissionAcreage(byte);
    else if (name == d_p_ClassName(PostOperationAssist))//接收辅助点
        _processPostAssist(byte);
    else if (name == d_p_ClassName(PostABPoint))//接收AB点
        _processPostABPoint(byte);
    else if (name == d_p_ClassName(PostOperationReturn))//接收AB点
        _processPostReturn(byte);
    else if (name == d_p_ClassName(PostBlocks))//接收障碍物
        _processPostBlocks(byte);
    m_nHeartTimes = 0;
}

void VGNetManager::sltHeartTimeout()
{
    m_nHeartTimes++;
    if(m_tcpClient && m_connectState == 1)
    {
        if(m_nHeartTimes < HEART_TIMEOUT_COUNT)
        {
            PostHeartBeat cmdPhb;
            _checSeq();
            cmdPhb.Clear();
            cmdPhb.set_seqno(m_seqno);
            cmdPhb.set_id(m_strUserId.toUtf8().data());
            SendNetWork(cmdPhb);
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

void VGNetManager::_safeDeleteBoundary(const VGLandBoundary &bdr)
{
    DeleteParcelDescription sCmdDPD;
    if (m_connectState != 1)
        return;

    _checSeq();
    sCmdDPD.set_seqno(m_seqno);

    QString actId = bdr.GetLandActId();
    if (!actId.isEmpty())
    {
        sCmdDPD.set_pdid(actId.toUtf8().data());
        sCmdDPD.set_delpsi(true);
        sCmdDPD.set_delpc(true);
        SendNetWork(sCmdDPD);
    }
}

void VGNetManager::_safeDeleteFlyRoute(const VGFlyRoute &fr)
{
    DeleteOperationDescription sDelOp;
    if (m_connectState != 1)
        return;

    QString actId = fr.GetActId();
    if (!actId.isEmpty())
    {
        _checSeq();
        sDelOp.set_seqno(m_seqno);
        sDelOp.set_odid(actId.toUtf8().data());
        SendNetWork(sDelOp);
    }
}

void VGNetManager::_prcsPBUavStat(const UavStatus &uavStatus)
{
    QString uavId = uavStatus.uavid().c_str();

    if (!uavId.isEmpty())
        emit receiveUavProto(uavStatus);
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

das::proto::OperationPlan *VGNetManager::_transOperationPlan(const VGFlyRoute &fr)
{
    if (OperationPlan *operationPlan = new OperationPlan())
    {
        operationPlan->set_width(fr.GetSprinkleWidth());
        operationPlan->set_angle(fr.GetAngle());
        operationPlan->set_anti(fr.GetAnti());
        operationPlan->set_single(fr.GetSingleShrink());
        for (int bdr : fr.GetEdgeShinkLs())
        {
            SingleShrink *sb = operationPlan->add_bdrsafe();
            sb->set_bdr(bdr);
            sb->set_safe(fr.GetEdgeShink(bdr));
        }
        operationPlan->set_block(fr.GetBlockSafeDis());
        return operationPlan;
    }

    return NULL;
}

void VGNetManager::registerSuc()
{
    qvgApp->SetQmlTip(tr("Register success"));//"注册成功"
    disConnect();
    loggin();
    emit registed();
    emit connectChanged();
    m_nRegTip = 0;
    m_strCheck = QString();
    emit checkChanged();
    emit regTipChanged();
}

VGLandBoundary *VGNetManager::parseBoundary(const ParcelSurveyInformation &psi, QObject *p)
{
    VGLandBoundary *bdr = new VGLandBoundary(p);
    if (!bdr)
        return NULL;

    bdr->m_cloudID = psi.id().c_str();
    bdr->m_time = psi.surverytime();

    VGLandPolygon *plg = bdr->_addBoundary(VGLandPolygon::Boundary);
    _toPolygon(psi.po(), *plg);
    int nBlockCount = psi.bo_size();

    for (int i = 0; i < nBlockCount; i++)
    {
        VGLandPolygon *plgTmp = bdr->_addBoundary(VGLandPolygon::BlockBoundary);
        _toPolygon(psi.bo(i), *plgTmp);
    }
    bdr->m_bUploaded = true;
    return bdr;
}

ParcelDescription *VGNetManager::tranceBoundary(const VGLandBoundary &boundary, const QString &user)
{
    VGLandInformation *land = boundary.GetBelongedLand();
    if (!land)
        return NULL;

    ParcelDescription* pDes = new ParcelDescription();
    pDes->set_name(land->GetAddress().toUtf8().data());
    pDes->set_registerid(user.toUtf8().data());
    pDes->set_acreage(boundary.GetLandArea());
    if (!land->actualSurveyId().isEmpty())
        pDes->set_id(land->actualSurveyId().toUtf8().data());

    ParcelContracter* owner = new ParcelContracter();
    owner->set_name(land->ownerName().toUtf8().data());
    owner->set_address(land->ownerAddr().toUtf8().data());
    owner->set_birthdate(land->m_time);
    owner->set_mobileno(land->ownerPhone().toUtf8().data());
    pDes->set_allocated_pc(owner);

    QGeoCoordinate coor = boundary.GetCenter();
    if (coor.isValid())
    {
        Coordinate* onecoordiante = new Coordinate();
        onecoordiante->set_latitude((qint32)(coor.latitude()*INTCOORLATORLONPOW));
        onecoordiante->set_longitude((qint32)(coor.longitude()*INTCOORLATORLONPOW));
        onecoordiante->set_altitude((qint32)(coor.altitude() * 1000));
        pDes->set_allocated_coordinate(onecoordiante);
    }

    ParcelSurveyInformation* psi = _boundaryToParcelSurvey(&boundary);
    if (!psi)
    {
        delete pDes;
        return NULL;
    }
    pDes->set_allocated_psi(psi);
    return pDes;
}

void VGNetManager::uploadSurveyInfo(const VGLandBoundary &desInfo)
{
    VGLandInformation *land = desInfo.GetBelongedLand();
    if (m_connectState != 1 || !land)
        return;
    ParcelDescription* pDes = tranceBoundary(desInfo, m_strUserId);
    PostParcelDescription m_cmdPpdes;
    _checSeq();
    m_cmdPpdes.set_seqno(m_seqno);
    m_cmdPpdes.set_allocated_pd(pDes);
    SendNetWork(m_cmdPpdes);
    VGGlobalFunc::initialItemDescribe(m_mapDesUpload, desInfo, false);
}

void VGNetManager::uploadPlanningRoute(const VGFlyRoute &fr)
{
    if (m_connectState != 1 || !fr.IsValide())
        return;

    PostOperationDescription cmdPor;
    _checSeq();
    cmdPor.set_seqno(m_seqno);
    if (OperationDescription *od = new OperationDescription)
    {
        od->set_registerid(m_strUserId.toUtf8().data());
        od->set_pdid(fr.GetBoundaryID().toUtf8().data());
        if (!fr.m_strActId.isEmpty())
            od->set_odid(fr.m_strActId.toUtf8().data());

        od->set_crop(fr.GetCropper().toUtf8().data());
        od->set_drug(fr.GetPesticide().toUtf8().data());
        od->set_ridge(fr.CountRidges());
        od->set_prize(fr.m_fPrise);
        if (!fr.m_strDescribe.isEmpty())
            od->set_notes(fr.m_strDescribe.toUtf8().data());
        od->set_plantime(fr.m_time);
        od->set_allocated_op(_transOperationPlan(fr));

        cmdPor.set_allocated_od(od);
    }

    SendNetWork(cmdPor);
    VGGlobalFunc::initialItemDescribe(m_mapDesUpload, fr, false);
}

void VGNetManager::requestLandinfo(const QString &userId, const DescribeMap &dcs)
{
    if (m_connectState != 1)
        return;
    RequestParcelDescriptions cmdRPDess;
    _checSeq();
    cmdRPDess.set_seqno(m_seqno);
    if (!userId.isEmpty())
        cmdRPDess.set_registerid(userId.toUtf8().data());

    DescribeMap::const_iterator itr = dcs.find("id");
    if (itr != dcs.end())
        cmdRPDess.set_pdid(itr.value().toString().toUtf8().data());

    itr = dcs.find("surveryorid");
    if (itr != dcs.end())
        cmdRPDess.set_surveryorid(itr.value().toString().toUtf8().data());

    itr = dcs.find("parcelname");
    if (itr != dcs.end())
        cmdRPDess.set_parcelname(itr.value().toString().toUtf8().data());

    itr = dcs.find("contracter");
    if (itr != dcs.end())
        cmdRPDess.set_contracter(itr.value().toString().toUtf8().data());

    itr = dcs.find("mobileno");
    if (itr != dcs.end())
        cmdRPDess.set_mobileno(itr.value().toString().toUtf8().data());

    itr = dcs.find("zones");
    if (itr != dcs.end())
    {
        QList<QGeoCoordinate> lsCoor = itr.value().value<QList<QGeoCoordinate> >();
        foreach (const QGeoCoordinate &coordinate, lsCoor)
        {
            Coordinate* coordinateRect = new Coordinate();
            coordinateRect->set_latitude(coordinate.latitude() * INTCOORLATORLONPOW);
            coordinateRect->set_longitude(coordinate.longitude() * INTCOORLATORLONPOW);
            coordinateRect->set_altitude(coordinate.altitude() * 1e3);

            Coordinate* coordinate2 = cmdRPDess.add_zones();
            *coordinate2 = *coordinateRect;
        }
    }
    SendNetWork(cmdRPDess);
}

void VGNetManager::DeleteItemSafe(const MapAbstractItem *item)
{
    if (!item)
        return;

    if (item->ItemType() == MapAbstractItem::Type_LandBoundary)
        _safeDeleteBoundary(*(const VGLandBoundary*)item);
    if (item->ItemType() == MapAbstractItem::Type_FlyRoute)
        _safeDeleteFlyRoute(*(const VGFlyRoute*)item);
}

void VGNetManager::requestOperationPlan(const QString &userId, const DescribeMap &dcs)
{
    if (m_connectState != 1)
        return;
    _checSeq();

    RequestOperationDescriptions cmdRODess;
    cmdRODess.set_seqno(m_seqno);

    if (!userId.isEmpty())
        cmdRODess.set_registerid(userId.toUtf8().data());

    DescribeMap::const_iterator itr = dcs.find("odid");
    if (itr != dcs.end())
        cmdRODess.set_odid(itr.value().toString().toUtf8().data());

    itr = dcs.find("pdid");
    if (itr != dcs.end())
        cmdRODess.set_pdid(itr.value().toString().toUtf8().data());

    SendNetWork(cmdRODess);
}

void VGNetManager::requestPlanning(const QString &pdid, const QString &odid)
{
    if (m_connectState != 1)
        return;
    RequestOperationDescriptions cmdROR;
    _checSeq();
    cmdROR.set_seqno(m_seqno);

    if (!pdid.isEmpty())
        cmdROR.set_pdid(pdid.toUtf8().data());
    else if (!odid.isEmpty())
        cmdROR.set_odid(odid.toUtf8().data());
    else
        cmdROR.set_registerid(m_strUserId.toUtf8().data());

    SendNetWork(cmdROR);
}

void VGNetManager::_processUploadMissionResult(const QByteArray &byte)
{
    static AckPostOperationRoute sAckPor;               //上传航线规划应答
    if (sAckPor.ParseFromArray(byte.data(), byte.size()))
    {
        if (sAckPor.result() == 1)
            emit missionUploaded(m_mapDesUpload["CurrentPlan"].toString());
    }
}

void VGNetManager::uploadMission(VGVehicleMission *ms, const QString &uavId)
{
    VGFlyRoute *fr = ms ? ms->GetFlyRoute() : NULL;
    if (m_connectState != 1 || !fr || !fr->IsValide())
        return;

    _checSeq();
    PostOperationRoute cmdPor;
    cmdPor.set_seqno(m_seqno);

    OperationRoute *opr = new OperationRoute();
    if (auto *land = fr->GetBelongedBoundary())
        opr->set_landid(land->GetBoundaryId().toUtf8().data());
    opr->set_gsid(m_strUserId.toUtf8().data());
    opr->set_rpid(fr->GetActId().toUtf8().data());
    opr->set_uavid(uavId.toUtf8().data());
    opr->set_createtime(QDateTime::currentMSecsSinceEpoch());
    opr->set_maxvoyage(fr->GetOperateVoyage());
    opr->set_beg(ms->GetBegin());
    opr->set_end(ms->GetEnd());
    float area = fr->GetAcreage()*ms->GetOpVoyage() / fr->GetOperateVoyage();
    opr->set_acreage(area);

    //航点列表
    for (MissionItem *item : ms->MissionItems())
    {
        mavlink_message_t message;
        VGMavLinkCode::EncodeMissionItemInt(message, *item, item->sequenceNumber() == 0);
        opr->add_missions(message.payload64, MAVLINK_MSG_ID_MISSION_ITEM_INT_LEN);
        if (DoubleEqu(item->param3().toFloat(), 1))
            opr->add_ridgebeg(item->sequenceNumber());
    }
    if (fr->CountBlock() > 0)
    {
        for (MissionItem *item : ms->BoundaryItems())
        {
            mavlink_message_t message;
            VGMavLinkCode::EncodeMissionItemInt(message, *item, item->sequenceNumber() == 0);
            opr->add_boundarys(message.payload64, MAVLINK_MSG_ID_MISSION_ITEM_INT_LEN);
        }
    }

    m_mapDesUpload["CurrentPlan"] = uavId;
    cmdPor.set_allocated_or_(opr);
    SendNetWork(cmdPor);
}

void VGNetManager::QuerySuspend(const QString &uavId, const QString &plan)
{
    RequestMissionSuspend req;
    _checSeq();
    req.set_seqno(m_seqno);
    req.set_uav(uavId.toUtf8().data());
    req.set_planid(plan.toUtf8().data());

    SendNetWork(req);
}

void VGNetManager::SendControlUav(const QString &uav, const mavlink_message_t &mav)
{
    if (mav.len > 128 || m_connectState != 1)
        return;

    PostControl2Uav gc2uav;
    _checSeq();
    gc2uav.set_seqno(m_seqno);
    gc2uav.set_uavid(uav.toUtf8().data());
    gc2uav.set_userid(m_strUserId.toUtf8().data());

    uint8_t buffer[MAVLINK_MAX_PACKET_LEN] = {0};
    *(uint16_t*)buffer = mav.msgid;
    int len = mav.len+sizeof(uint16_t);
    memcpy(buffer + sizeof(uint16_t), mav.payload64, mav.len);
    gc2uav.add_data(buffer, len);

    SendNetWork(gc2uav);
}

void VGNetManager::SendControlUav(const QString &uav, const QList<MissionItem*> &items)
{
    if (items.count() <= 0 || m_connectState != 1)
        return;

    PostControl2Uav gc2uav;
    _checSeq();
    gc2uav.set_seqno(m_seqno);
    gc2uav.set_uavid(uav.toUtf8().data());
    gc2uav.set_userid(m_strUserId.toUtf8().data());
    mavlink_message_t message;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN + 2];
    for (MissionItem *item : items)
    {
        VGMavLinkCode::EncodeMissionItemInt(message, *item, item->sequenceNumber() == 0);
        *(uint16_t*)buffer = message.msgid;
        memcpy(buffer + sizeof(uint16_t), message.payload64, message.len);
        gc2uav.add_data(buffer, message.len+2);
    }
    SendNetWork(gc2uav);
}

void VGNetManager::SendControlUav(const QString &uav, const QMap<QString, QVariant> &params)
{
    if (params.count() <= 0 || m_connectState != 1)
        return;

    PostControl2Uav gc2uav;
    _checSeq();
    gc2uav.set_seqno(m_seqno);
    gc2uav.set_uavid(uav.toUtf8().data());
    gc2uav.set_userid(m_strUserId.toUtf8().data());
    mavlink_message_t message;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN + 2];
    int nLen = 70;
    QMap<QString, QVariant>::const_iterator itr = params.begin();
    for (; itr!= params.end(); ++itr)
    {
        if (!itr.value().isValid())
            VGMavLinkCode::EncodeGetParameter(message, itr.key());
        else
            VGMavLinkCode::EncodeSetParameter(message, itr.key(), itr.value());

        *(uint16_t*)buffer = message.msgid;
        memcpy(buffer + sizeof(uint16_t), message.payload64, message.len);
        gc2uav.add_data(buffer, message.len + 2);
        nLen += message.len + 2;
        if(nLen > 200)
            break;
    }
    SendNetWork(gc2uav);
}

void VGNetManager::requestUavInfo(const QStringList &lstPlaneId)
{
    if(m_connectState != 1 && lstPlaneId.size() <= 0)
        return;

    RequestUavStatus cmdRUS;
    _checSeq();
    cmdRUS.set_seqno(m_seqno);

    foreach(const QString &plantId, lstPlaneId)
    {
        if (plantId.isEmpty())
            continue;

        cmdRUS.add_uavid(plantId.toUtf8().data());
    }

    SendNetWork(cmdRUS);
}

void VGNetManager::requestUavInfo(const QGeoCoordinate &/*coor*/, double /*dis*/)
{
}

void VGNetManager::requestFromUav(const QString &uav, uint32_t mavType)
{
    if(m_connectState != 1 || uav.isEmpty())
        return;

    if (0 == mavType)
    {
        RequestUavStatus cmdRUS;
        _checSeq();
        cmdRUS.set_seqno(m_seqno);
        cmdRUS.add_uavid(uav.toUtf8().data());
        SendNetWork(cmdRUS);
    }
    else if (MAV_CMD_GET_START_POINT == mavType || MAV_CMD_GET_END_POINT == mavType)
    {
        RequestOperationAssist cmdRUS;
        _checSeq();
        cmdRUS.set_seqno(m_seqno);
        cmdRUS.set_uavid(uav.toUtf8().data());
        SendNetWork(cmdRUS);
    }
    else if (MAV_CMD_GET_AB_POINT == mavType)
    {
        RequestABPoint cmdRUS;
        _checSeq();
        cmdRUS.set_seqno(m_seqno);
        cmdRUS.set_uavid(uav.toUtf8().data());
        SendNetWork(cmdRUS);
    }
    else if (MAV_CMD_ENUM_END == mavType)
    {
        RequestOperationReturn cmdRUS;
        _checSeq();
        cmdRUS.set_seqno(m_seqno);
        cmdRUS.set_uavid(uav.toUtf8().data());
        SendNetWork(cmdRUS);
    }
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

    SendNetWork(cmdRBS);
}

bool VGNetManager::UploadItem(const MapAbstractItem *item)
{
    if (!item)
        return false;

    m_mapDesUpload.clear();
    if (item->ItemType() == MapAbstractItem::Type_LandBoundary)
        uploadSurveyInfo(*(const VGLandBoundary*)item);
    else if (item->ItemType() == MapAbstractItem::Type_FlyRoute)
        uploadPlanningRoute(*(const VGFlyRoute*)item);
    else
        return false;

    return true;
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

void VGNetManager::SendNetWork(const ::google::protobuf::Message &msg)
{
    int msglen = msg.ByteSize();
    QString name = msg.GetTypeName().c_str();
    QByteArray ay(msglen, 0);
    msg.SerializeToArray(ay.data(), msglen);
    emit sigSendRequestInfo(name, ay);
    _setSendNo(name, m_seqno);
}

ParcelSurveyInformation *VGNetManager::_boundaryToParcelSurvey(const VGLandBoundary *bdr)
{
    if (!bdr)
        return NULL;
    ParcelSurveyInformation *psi(new ParcelSurveyInformation());
    if (!psi)
        return NULL;

    psi->set_surverytime(bdr->GetMsTime());
    psi->set_surveryorid(bdr->GetUser().toUtf8().data());
    psi->set_stype(bdr->GetSurvey());

    if (!bdr->m_cloudID.isEmpty())
        psi->set_id(bdr->m_cloudID.toUtf8().data());

    foreach(VGLandPolygon *ply, bdr->Polygos())
    {
        if (ply->GetId() == VGLandPolygon::Boundary)
        {
            Outline *outline(new Outline);
            outline = _polygonToOutline(outline, *ply);
            psi->set_allocated_po(outline);
            if (!outline || outline->coordinates_size() < 2)
            {
                delete psi;
                return NULL;
            }
        }
        else if (ply->GetId() == VGLandPolygon::BlockBoundary)
        {
            Outline *outlineBlock = _polygonToOutline(psi->add_bo(), *ply);
            if (!outlineBlock)
            {
                delete psi;
                return NULL;
            }
        }
    }

    return psi;
}

Outline *VGNetManager::_polygonToOutline(Outline *outline, const VGLandPolygon &plg)
{
    if (!outline)
        return NULL;

    foreach (VGCoordinate *itr, plg.GetCoordinates())
    {
        QGeoCoordinate coordinate = itr->GetCoordinate();
        Coordinate *coordinateOutline = outline->add_coordinates();
        coordinateOutline->set_altitude((int)(coordinate.altitude() * 1000));
        coordinateOutline->set_latitude((qint32)(coordinate.latitude() * INTCOORLATORLONPOW));
        coordinateOutline->set_longitude((qint32)(coordinate.longitude() * INTCOORLATORLONPOW));
    }
    return outline;
}

VGLandBoundary *VGNetManager::parseLand(const ParcelDescription &pDes)
{
    VGLandInformation *land = new VGLandInformation(this);
    if (!land)
        return NULL;

    land->m_actualSurveyId = pDes.id().c_str();
    const ParcelSurveyInformation &psi = pDes.psi();
    land->m_adress = pDes.name().c_str();
    land->m_typeSurvey = psi.stype();
    const ParcelContracter &owner = pDes.pc();
    land->m_owner.strName = owner.name().c_str();
    land->m_owner.strAddress = owner.address().c_str();
    land->m_owner.strPhone = owner.mobileno().c_str();
    land->m_time = owner.birthdate();
    land->m_bSaveLocal = false;

    land->m_userId = psi.surveryorid().c_str();
    land->m_bUploaded = true;

    DescribeMap dsc;
    VGGlobalFunc::initialItemDescribe(dsc, *land);
    emit sigQueryItem(land, dsc);

    if (VGLandBoundary *bdr = parseBoundary(pDes.psi(), this))
    {
        bdr->SetParentItem(land);
        VGGlobalFunc::initialItemDescribe(dsc, *bdr);
        emit sigQueryItem(bdr, dsc);

        return bdr;
    }
    return NULL;
}

VGFlyRoute *VGNetManager::_parseFlyRoute(const das::proto::OperationDescription &od, DescribeMap &dsc)
{
    if (VGFlyRoute *fr = new VGFlyRoute())
    {
        dsc["boundaryId"] = od.pdid().c_str();
        fr->m_bUploaded = true;
        const OperationPlan &op = od.op();
        fr->m_sprinkleWidth = op.width();
        fr->m_fAngle = op.angle();
        fr->m_bAnti = op.anti();
        fr->m_bSingleShrink = op.single();
        for (int i = 0; i < op.bdrsafe_size(); ++i)
        {
            const SingleShrink &ss = op.bdrsafe(i);
            if (ss.bdr() < 0)
                fr->m_fSafeDis = ss.safe();
            else
                fr->m_lsOutlineSafe.push_back(VGFlyRoute::EdgeShrinkPair(ss.bdr(), ss.safe()));
        }
        fr->m_fBlockSafeDis = op.block();

        fr->m_strActId = od.odid().c_str();
        if (od.has_crop())
            fr->m_strCropper = od.crop().c_str();
        if (od.has_drug())
            fr->m_strPesticide = od.drug().c_str();
        fr->m_fPrise = od.prize();
        if (od.has_notes())
            fr->m_strDescribe = od.notes().c_str();

        fr->m_time = od.plantime();
        return fr;
    }
    return NULL;
}

void VGNetManager::_parseOperationRoute(const OperationRoute &)
{
}
