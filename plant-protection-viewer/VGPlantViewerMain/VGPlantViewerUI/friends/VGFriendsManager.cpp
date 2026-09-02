#include "VGFriendsManager.h"
#include "VGNetManager.h"
#include "VGApplication.h"
#include "VGLandBoundary.h"
#include "VGLandManager.h"
#include "VGLandInformation.h"
#include "VGMapManager.h"
#include "QmlObjectListModel.h"
#include <QDebug>
#include "VGFriend.h"
#include "das.pb.h"

#define PtbClassName(_cls) QString(_cls::descriptor()->full_name().c_str())

using namespace das::proto;

VGFriendsManager::VGFriendsManager(QObject *parent) : QObject(parent)
, m_seqNo(1), m_curFriend(NULL), m_newFriendMsgs(new QmlObjectListModel(this))
{
}

VGFriendsManager::~VGFriendsManager()
{
    savaLocalMsg();
}

void VGFriendsManager::deleteFriend(const QString &id)
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (netMgr && 1 == netMgr->GetConnectState())
    {
        GroundStationsMessage msg;
        msg.set_seqno(m_seqNo++);
        msg.set_from(netMgr->GetUserId().toUtf8().data());
        msg.set_to(id.toUtf8().data());
        msg.set_type(DeleteFriend);
        netMgr->SendNetWork(msg);
        _deleteFriend(id);
    }
}

void VGFriendsManager::deleteFriend(VGFriend *f)
{
    if (f)
    {
        m_vgFriends.removeAll(f);
        delete f;
    }
}

VGFriend * VGFriendsManager::GetCurFriend() const
{
    return m_curFriend;
}

void VGFriendsManager::SetCurFriend(VGFriend *f)
{
    if (m_curFriend != f)
    {
        if (!f)
            m_curFriend->SetSelected(false);
        else
            f->SetSelected(true);

        m_curFriend = f;
        emit curFriendChanged();
    }
}

void VGFriendsManager::requestFriends()
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (netMgr && 1 == netMgr->GetConnectState())
    {
        RequestFriends msg;
        msg.set_seqno(m_seqNo++);
        netMgr->SendNetWork(msg);
    }
}

void VGFriendsManager::requestFriend(const QString &id)
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (netMgr && 1 == netMgr->GetConnectState())
    {
        GroundStationsMessage msg;
        msg.set_seqno(m_seqNo++);
        msg.set_from(netMgr->GetUserId().toUtf8().data());
        msg.set_to(id.toUtf8().data());
        msg.set_type(RequestFriend);
        netMgr->SendNetWork(msg);
    }
}

bool VGFriendsManager::SendText(const QString &id, const QString &msg)
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (netMgr && 1 == netMgr->GetConnectState())
    {
        GroundStationsMessage msgGs;
        msgGs.set_seqno(m_seqNo++);
        msgGs.set_from(netMgr->GetUserId().toUtf8().data());
        msgGs.set_to(id.toUtf8().data());
        msgGs.set_type(Text);
        msgGs.set_content(msg.toUtf8().data());
        netMgr->SendNetWork(msgGs);
        return true;
    }
    return false;
}

bool VGFriendsManager::SendLand(const QString &id, VGLandBoundary *bdr)
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (bdr && netMgr && 1 == netMgr->GetConnectState())
    {
        GroundStationsMessage msgGs;
        if (ParcelDescription *pd = VGNetManager::tranceBoundary(*bdr, netMgr->GetUserId()))
        {
            QByteArray a(pd->ByteSize(), '\0');
            pd->SerializeToArray(a.data(), a.size());
            msgGs.set_content(a.data(), a.size());
        }
        else
        {
            return false;
        }
        msgGs.set_seqno(m_seqNo++);
        msgGs.set_from(netMgr->GetUserId().toUtf8().data());
        msgGs.set_to(id.toUtf8().data());
        msgGs.set_type(Land);
        netMgr->SendNetWork(msgGs);
        return true;
    }
    return false;
}

bool VGFriendsManager::RejectFriend(const QString &id)
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (netMgr && 1 == netMgr->GetConnectState())
    {
        GroundStationsMessage msg;
        msg.set_seqno(m_seqNo++);
        msg.set_from(netMgr->GetUserId().toUtf8().data());
        msg.set_to(id.toUtf8().data());
        msg.set_type(RequestFriend);
        netMgr->SendNetWork(msg);
        return true;
    }
    return false;
}

bool VGFriendsManager::AcceptFriend(const QString &id)
{
    VGNetManager *netMgr = qvgApp->netManager();
    if (netMgr && 1 == netMgr->GetConnectState())
    {
        GroundStationsMessage msg;
        msg.set_seqno(m_seqNo++);
        msg.set_from(netMgr->GetUserId().toUtf8().data());
        msg.set_to(id.toUtf8().data());
        msg.set_type(AgreeFriend);
        netMgr->SendNetWork(msg);
        _addFriend(id);
        return true;
    }
    return false;
}

void VGFriendsManager::readLocalMsg()
{
}

void VGFriendsManager::savaLocalMsg()
{
}

void VGFriendsManager::_processFriends(const google::protobuf::Message &msg)
{
    const AckFriends &ackFriends = *(AckFriends*)&msg;
    for (int i = 0; i < ackFriends.friends_size(); i++)
    {
        _addFriend(ackFriends.friends(i).c_str());
    }
}

void VGFriendsManager::_processMessage(const google::protobuf::Message &p)
{
    const GroundStationsMessage &gsMsg = *(GroundStationsMessage*)&p;
    VGGSMessage *msg = new VGGSMessage(true, (VGGSMessage::Msg_Type)gsMsg.type());
    if (!msg)
        return;
    msg->SetFromID(gsMsg.from().c_str());
    msg->SetToID(gsMsg.to().c_str());
    switch (gsMsg.type())
    {
    case Land:
        {
            ParcelDescription psi;
            psi.ParseFromArray(gsMsg.content().c_str(), gsMsg.content().size());
            VGLandBoundary *bdr = qvgApp->netManager()->parseLand(psi);
            if (bdr)
                msg->SetLand(bdr);
        }
        break;
    case Text:
        {
            QString text = QString::fromUtf8(gsMsg.content().c_str(), gsMsg.content().size());
            msg->SetText(text);
        }
        break;
    default:
        break;
    }
    VGFriend *frd = GetFriendById(msg->GetFromID());
    if (frd && msg->GetProcess()==VGGSMessage::NoStat)
        frd->AddGSMessage(msg);
    else
        _addMsg(msg);
}

void VGFriendsManager::_processMessageAck(const google::protobuf::Message &msg)
{
    const AckGroundStationsMessage ack = *(AckGroundStationsMessage*)&msg;
    if (VGFriend *fr = GetFriendById(ack.gs().c_str()))
        fr->SetOnLine(ack.res() == 1);
}

void VGFriendsManager::_addMsg(VGGSMessage *msg)
{
    if (msg)
    {
        m_newFriendMsgs->append(msg);
        if (msg->GetMsgType() == VGGSMessage::Msg_AgreeFriend)
            _addFriend(msg->GetFromID());
        else if (msg->GetMsgType() == VGGSMessage::Msg_DeleteFriend)
            _deleteFriend(msg->GetFromID());
    }
}

void VGFriendsManager::_addFriend(const QString &id)
{
    if (id.length() < 3 || GetFriendById(id))
        return;

    if (auto fr = new VGFriend(id, this))
    {
        m_vgFriends << fr;
        qvgApp->mapManager()->InsertItem(fr);
    }
}

void VGFriendsManager::_deleteFriend(const QString &id)
{
    if (auto fr = GetFriendById(id))
        deleteFriend(fr);
}

QmlObjectListModel *VGFriendsManager::GetNewFriendMsgs()const
{
    return m_newFriendMsgs;
}

VGFriend *VGFriendsManager::GetFriendById(const QString &id)
{
    for (VGFriend *f : m_vgFriends)
    {
        if (f->GetFriendName() == id)
            return f;
    }
    return NULL;
}

QString VGFriendsManager::GetUser() const
{
    return m_strUser;
}

void VGFriendsManager::InitialNetWork(VGNetManager *nm)
{
    connect(nm, &VGNetManager::receiveGSMsg, this, &VGFriendsManager::onRecvGSMsg);
    connect(nm, &VGNetManager::connectStateChanged, this, &VGFriendsManager::onNetStatchanged);
}

void VGFriendsManager::AddFriend(VGFriend *fd)
{
    if(fd)
    {
        m_vgFriends << fd;
        qvgApp->mapManager()->InsertItem(fd);
    }
}

void VGFriendsManager::onRecvGSMsg(const google::protobuf::Message &msg)
{
    QString strName = msg.GetTypeName().c_str();
    if (strName == PtbClassName(AckFriends))
        _processFriends(msg);
    else if (strName == PtbClassName(GroundStationsMessage))
        _processMessage(msg);
    else if (strName == PtbClassName(AckGroundStationsMessage))
        _processMessageAck(msg);
}

void VGFriendsManager::onNetStatchanged(int state)
{
    QString user = qvgApp->netManager()->GetUserId();
    if(state == 1 && m_strUser!=user)
    {
        SetCurFriend(NULL);
        qDeleteAll(m_vgFriends);
        m_vgFriends.clear();
        savaLocalMsg();
        while (m_newFriendMsgs->count())
        {
            delete m_newFriendMsgs->removeAt(0);
        }
        m_strUser = user;
        requestFriends();
    }
}
