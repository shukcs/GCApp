#ifndef __VGFRIEND_H__
#define __VGFRIEND_H__

#include "MapAbstractItem.h"

class VGLandBoundary;
class QmlObjectListModel;
class VGGSMessage : public QObject
{
    Q_OBJECT
    Q_ENUMS(Msg_Type)
    Q_ENUMS(ProcessStat)
    Q_PROPERTY(bool recv READ IsRecv CONSTANT)
    Q_PROPERTY(QString from READ GetFromID CONSTANT)
    Q_PROPERTY(QString dst READ GetToID  CONSTANT)
    Q_PROPERTY(QString text READ GetText CONSTANT)
    Q_PROPERTY(VGLandBoundary* land READ GetLand CONSTANT)
    Q_PROPERTY(Msg_Type type READ GetMsgType CONSTANT)
    Q_PROPERTY(ProcessStat process READ GetProcess WRITE SetProcess NOTIFY processChanged)
public:
    enum Msg_Type {
        Msg_DeleteFriend = 1,    //删除好友
        Msg_RequestFriend,       //好友申请
        Msg_AgreeFriend,         //同意好友
        Msg_RejectFriend,        //拒绝好友
        Msg_Text,                //文字
        Msg_Land,                //地块
    };
    enum ProcessStat {
        NoStat,         //无处理状态
        UnProcess,      //删除好友
        Agree,          //同意好友
        Reject,         //拒绝好友
    };
public:
    explicit VGGSMessage(bool rcv, Msg_Type tp=Msg_Land, QObject *parent = NULL);
public:
    QString GetFromID() const;
    void SetFromID(const QString &fromID);
    QString GetToID() const;
    void SetToID(const QString& toID);
    QString GetText() const;
    void SetText(const QString& content);
    VGLandBoundary *GetLand()const;
    void SetLand(VGLandBoundary *land);
    Msg_Type GetMsgType() const;
    ProcessStat GetProcess()const;
    void SetProcess(ProcessStat st);
    bool IsRecv()const;
protected:

    QString _genText(Msg_Type tp)const;
signals:
    void processChanged();
private:
    Msg_Type        m_iMsgType;
    VGLandBoundary  *m_bdr;
    QString         m_text;
    ProcessStat     m_stat;
    bool            m_bRecv;
    QString         m_strFromID;
    QString         m_strToID;
};

class VGFriend : public SingleTriggerItem<MapAbstractItem>
{
    Q_OBJECT
    Q_PROPERTY(QString name READ GetFriendName CONSTANT)
    Q_PROPERTY(bool online READ IsOnline NOTIFY onlineChanged)
    Q_PROPERTY(QmlObjectListModel* msgs READ GetMessages CONSTANT)
public:
    explicit VGFriend(const QString &name, QObject *parent = NULL);
public:
    QmlObjectListModel* GetMessages()const;
    void AddGSMessage(VGGSMessage *msg);
    QString GetFriendName();
    void SetOnLine(bool b);
protected:
    Q_INVOKABLE void sendText(const QString &text);
    Q_INVOKABLE void sendLand(VGLandBoundary *bdr);
    
    void _addSend(const QString &text);
    void _addSend(VGLandBoundary *bdr);
    bool IsOnline()const;
    MapItemType ItemType()const;
    bool operator==(const MapAbstractItem &item)const;
    MapAbstractItem *Clone(QObject *parent)const;
signals:
    void onlineChanged();
private:
    QmlObjectListModel  *m_msgs;
    bool                m_bOnline;
    QString             m_strFriend;
};

#endif // __VGFRIEND_H__
