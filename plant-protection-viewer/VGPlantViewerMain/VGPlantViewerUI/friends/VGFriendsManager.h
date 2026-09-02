#ifndef VGFRIENDSMANAGER_H
#define VGFRIENDSMANAGER_H

#include <QObject>

class VGLandBoundary;
class VGNetManager;
class VGFriend;
class VGGSMessage;
class QmlObjectListModel;

namespace google {
    namespace protobuf {
        class Message;
    }
}

class VGFriendsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QmlObjectListModel *newFriendMsgs READ GetNewFriendMsgs CONSTANT)
    Q_PROPERTY(VGFriend *curFriend READ GetCurFriend WRITE SetCurFriend NOTIFY curFriendChanged)
    Q_PROPERTY(QString user READ GetUser)
public:
    explicit VGFriendsManager(QObject *parent = nullptr);
    ~VGFriendsManager();
public:
    void InitialNetWork(VGNetManager *nm);
    QmlObjectListModel* GetNewFriendMsgs()const;
    void AddFriend(VGFriend *fd);
    bool SendText(const QString &id, const QString &msg);
    bool SendLand(const QString &id, VGLandBoundary *bdr);
    bool AcceptFriend(const QString &id);
    bool RejectFriend(const QString &id);
    VGFriend *GetFriendById(const QString &id);
    QString GetUser()const;
protected:
    Q_INVOKABLE void requestFriend(const QString &id);
    Q_INVOKABLE void deleteFriend(const QString &id);
    Q_INVOKABLE void requestFriends();

    VGFriend *GetCurFriend()const;
    void SetCurFriend(VGFriend *f);
    void deleteFriend(VGFriend *f);
private:
    void readLocalMsg();
    void savaLocalMsg();
    void _processFriends(const google::protobuf::Message &msg);
    void _processMessage(const google::protobuf::Message &msg);
    void _processMessageAck(const google::protobuf::Message &msg);
    void _addMsg(VGGSMessage *msg);
    void _addFriend(const QString &id);
    void _deleteFriend(const QString &id);
signals:
    void curFriendChanged();
private slots:
    void onRecvGSMsg(const google::protobuf::Message &msg);
    void onNetStatchanged(int state);
private:
    int                 m_seqNo;
    VGFriend            *m_curFriend;
    QList<VGFriend *>   m_vgFriends;
    QString             m_strUser;
    QmlObjectListModel  *m_newFriendMsgs;//新的好友列表
};

#endif // VGFRIENDSMANAGER_H
