#include "VGFriend.h"
#include "VGApplication.h"
#include "VGFriendsManager.h"
#include "QmlObjectListModel.h"
#include <QDebug>

VGGSMessage::VGGSMessage (bool rcv, Msg_Type tp, QObject *parent): QObject(parent)
, m_iMsgType(tp), m_bdr(NULL), m_text(_genText(tp))
, m_stat(tp==Msg_RequestFriend ? UnProcess : NoStat)
, m_bRecv(rcv)
{
}

QString VGGSMessage::GetFromID() const
{
    return m_strFromID;
}

QString VGGSMessage::GetToID() const
{
    return m_strToID;
}

QString VGGSMessage::GetText() const
{
    return m_text;
}

VGGSMessage::Msg_Type VGGSMessage::GetMsgType() const
{
    return m_iMsgType;
}

VGGSMessage::ProcessStat VGGSMessage::GetProcess() const
{
    return m_stat;
}

void VGGSMessage::SetProcess(ProcessStat st)
{
    if (m_stat != st)
    {
        if (st == Agree)
        {
            if (!qvgApp->friendsManager()->AcceptFriend(m_strFromID))
                return;
        }
        else if (st == Reject)
        {
            if (!qvgApp->friendsManager()->RejectFriend(m_strFromID))
                return;
        }

        m_stat = st;
        emit processChanged();
    }
}

void VGGSMessage::SetFromID(const QString &fromID)
{
    m_strFromID = fromID;
}

void VGGSMessage::SetToID(const QString& toID)
{
    m_strToID = toID;
}

void VGGSMessage::SetText(const QString& content)
{
    m_iMsgType = Msg_Text;
    m_text = content;
    m_stat = NoStat;
}

VGLandBoundary * VGGSMessage::GetLand() const
{
    return m_bdr;
}

void VGGSMessage::SetLand(VGLandBoundary *land)
{
    m_iMsgType = Msg_Land;
    m_bdr = land;
    m_stat = NoStat;
}

QString VGGSMessage::_genText(Msg_Type tp) const
{
    switch (tp)
    {
    case VGGSMessage::Msg_DeleteFriend:
        return tr("Delete from friends");
    case VGGSMessage::Msg_RequestFriend:
        return tr("Request friend relativity");
    case VGGSMessage::Msg_AgreeFriend:
        return tr("Agree friend relativity");
    case VGGSMessage::Msg_RejectFriend:
        return tr("Reject friend relativity");
    default:
        break;
    }
    return QString();
}

bool VGGSMessage::IsRecv() const
{
    return m_bRecv;
}
///////////////////////////////////////////////////////////////////////////////////
//VGFriend
///////////////////////////////////////////////////////////////////////////////////
VGFriend::VGFriend(const QString &name, QObject *parent)
: SingleTriggerItem<MapAbstractItem>(parent, 1)
, m_msgs(new QmlObjectListModel(this))
, m_bOnline(true), m_strFriend(name)
{
}

QmlObjectListModel *VGFriend::GetMessages()const
{
    return m_msgs;
}

void VGFriend::AddGSMessage(VGGSMessage *msg)
{
    if (m_msgs && msg)
        m_msgs->append(msg);
}

QString VGFriend::GetFriendName()
{
    return m_strFriend;
}

void VGFriend::SetOnLine(bool b)
{
    if (m_bOnline != b)
    {
        m_bOnline = b;
        emit onlineChanged();
    }
}

MapAbstractItem *VGFriend::Clone(QObject *) const
{
    return NULL;
}

MapAbstractItem::MapItemType VGFriend::ItemType() const
{
    return Type_Friend;
}

bool VGFriend::operator==(const MapAbstractItem &item) const
{
    if (!ItemType() != item.ItemType())
        return false;

    return m_strFriend == ((VGFriend*)&item)->m_strFriend;
}

void VGFriend::sendText(const QString &text)
{
    VGFriendsManager *mgr = qvgApp->friendsManager();
    if (mgr && mgr->SendText(m_strFriend, text))
        _addSend(text);
}

void VGFriend::sendLand(VGLandBoundary *bdr)
{
    VGFriendsManager *mgr = qvgApp->friendsManager();
    if (mgr && mgr->SendLand(m_strFriend, bdr))
        _addSend(bdr);
}

void VGFriend::_addSend(const QString &text)
{
    if (VGGSMessage *msg = new VGGSMessage(false))
    {
        msg->SetFromID(qvgApp->friendsManager()->GetUser());
        msg->SetToID(m_strFriend);
        msg->SetText(text);
        AddGSMessage(msg);
    }
}

void VGFriend::_addSend(VGLandBoundary *bdr)
{
    if (VGGSMessage *msg = new VGGSMessage(false))
    {
        msg->SetFromID(qvgApp->friendsManager()->GetUser());
        msg->SetToID(m_strFriend);
        msg->SetLand(bdr);
        AddGSMessage(msg);
    }
}

bool VGFriend::IsOnline() const
{
    return m_bOnline;
}

