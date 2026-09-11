#ifndef TCPLINK_H
#define TCPLINK_H

#include <QString>

#if defined(QGC_ZEROCONF_ENABLED)
#include <dns_sd.h>
#endif

//#include "QGCConfig.h"
#include "LinkCommand.h"
#include "LinkInterface.h"
#include "vgcomm_global.h"

// Even though QAbstractSocket::SocketError is used in a signal by Qt, Qt doesn't declare it as a meta type.
// This in turn causes debug output to be kicked out about not being able to queue the signal. We declare it
// as a meta type to silence that.
#include <QMetaType>
#include <QHostAddress>

//#define TCPLINK_READWRITE_DEBUG   // Use to debug data reads/writes

class QTcpSocket;
class TCPCommand : public LinkCommand
{
    Q_OBJECT

public:
    Q_PROPERTY(quint16  port    READ port   WRITE setPort   NOTIFY portChanged)
    Q_PROPERTY(QString  host    READ host   WRITE setHost   NOTIFY hostChanged)

    TCPCommand(QObject * p=nullptr);

    quint16 port   () { return m_port; }
    void setPort   (quint16 port);

    const QHostAddress& address   () { return m_address; }
    const QString       host      () { return m_address.toString(); }

    void setAddress (const QHostAddress& address);
    void setHost    (const QString host);

    LinkType    type()const override;
    bool        isHighLatencyAllowed () { return true; }
    void        loadSettings(const QString& root);
    void        saveSettings(const QString& root);
    void        updateSettings  ();
    QString     settingsURL     () { return "TcpSettings.qml"; }
    QString     settingsTitle   () { return tr("TCP Link Settings"); }
    bool IsValid()const;

    QString getName()const;
    void setName(const QString &s);
    Q_INVOKABLE void setServer(const QString &host, uint16_t port);
protected:
    LinkInterface *CreateLink();
signals:
    void portChanged();
    void hostChanged();
private:
    QHostAddress m_address;
    quint16 m_port;
};

class TCPLink : public LinkInterface
{
    Q_OBJECT
public:
    QTcpSocket* getSocket(void) { return m_socket; }

    void signalBytesWritten(void);

    // LinkInterface methods
    virtual QString getName(void) const;
    virtual bool isConnected(void) const;
    virtual void requestReset(void) {};

    // Extensive statistics for scientific purposes
    qint64 getConnectionSpeed() const;
    qint64 getCurrentInDataRate() const;
    qint64 getCurrentOutDataRate() const;

    // These are left unimplemented in order to cause linker errors which indicate incorrect usage of
    // connect/disconnect on link directly. All connect/disconnect calls should be made through LinkManager.
    bool connect(void);
    bool disconnect(void);
public slots:
    void writeBytes(const QByteArray &array);
protected slots:
    void _socketError(QAbstractSocket::SocketError socketError);

    // From LinkInterface
    virtual void readBytes(void);
private:
    // Links are only created/destroyed by LinkManager so constructor/destructor is not public
    TCPLink(TCPCommand *config);
    ~TCPLink();

    // From LinkInterface
    virtual bool _connect(void);
    virtual void _disconnect(void);

    bool _hardwareConnect();
    void _restartConnection();
signals:
    void sendData(const QByteArray &);
#ifdef TCPLINK_READWRITE_DEBUG
    void _writeDebugBytes(const QByteArray data);
#endif
private:
    friend class TCPCommand;
    friend class LinkManager;
    TCPCommand          * m_tcpConfig;
    QTcpSocket*         m_socket;
    QThread             *m_thread;
    bool                m_connectState;
    int                 m_timerId;
};

#endif //TCPLINK_H