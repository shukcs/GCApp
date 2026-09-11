#include <QTimer>
#include <QList>
#include <QDebug>
#include <QMutexLocker>
#include <QHostInfo>
#include <QThread>
#include <QSettings>
#include <QTcpSocket>
#include <iostream>

#include "TCPLink.h"
#include "LinkManager.h"
#include "base.h"

#define QGC_TCP_PORT 5760
/// @file
///     @brief TCP link type for SITL support
///
///     @author Don Gagne <don@thegagnes.com>

TCPLink::TCPLink(TCPCommand *config) : LinkInterface(config)
, m_tcpConfig(config), m_socket(nullptr), m_thread(new QThread)
#if defined(QGC_ZEROCONF_ENABLED)
    , _dnssServiceRef(NULL)
#endif
    , m_timerId(-1)
{
    if (m_thread)
        moveToThread(m_thread);

    QObject::connect(this, &TCPLink::sendData, this, [=](const QByteArray &array) {
        if (!m_tcpConfig || array.size() < 1)
            return;

        m_socket->write(array);
    });
    m_thread->start(QThread::NormalPriority);
}

TCPLink::~TCPLink()
{
    _disconnect();
    delete m_socket;
    delete m_thread;
}

#ifdef TCPLINK_READWRITE_DEBUG
void TCPLink::_writeDebugBytes(const QByteArray data)
{
    QString bytes;
    QString ascii;
    for (int i=0, size = data.size(); i<size; i++)
    {
        unsigned char v = data[i];
        bytes.append(QString::asprintf("%02x ", v));
        if (data[i] > 31 && data[i] < 127)
        {
            ascii.append(data[i]);
        }
        else
        {
            ascii.append(219);
        }
    }
    qDebug() << "Sent" << size << "bytes to" << m_tcpConfig->address().toString() << ":" << m_tcpConfig->port() << "data:";
    qDebug() << bytes;
    qDebug() << "ASCII:" << ascii;
}
#endif

void TCPLink::writeBytes(const QByteArray &array)
{
    if (!m_socket || !m_thread || !m_thread->isFinished())
        return;

    emit sendData(array);
}

/**
 * @brief Read a number of bytes from the interface.
 *
 * @param data Pointer to the data byte array to write the bytes to
 * @param maxLength The maximum number of bytes to write
 **/
void TCPLink::readBytes()
{
    if (m_socket) {
        qint64 byteCount = m_socket->bytesAvailable();
        if (byteCount)
        {
            QByteArray buffer;
            buffer.resize(byteCount);
            m_socket->read(buffer.data(), buffer.size());
            emit bytesReceived(this, buffer);
#ifdef TCPLINK_READWRITE_DEBUG
            writeDebugBytes(buffer.data(), buffer.size());
#endif
        }
    }
}

/**
 * @brief Disconnect the connection.
 *
 * @return True if connection has been disconnected, false if connection couldn't be disconnected.
 **/
void TCPLink::_disconnect(void)
{
    if (!m_thread || !m_thread->isRunning())
        return;

    m_thread->quit();
    m_thread->wait();
    if (m_socket)
    {
        m_socket->deleteLater();
        m_socket = NULL;
        emit linkConnected(false);
    }
    m_connectState = false;
}

/**
 * @brief Connect the connection.
 *
 * @return True if connection has been established, false if connection couldn't be established.
 **/
bool TCPLink::_connect(void)
{
    if (!m_thread)
        m_thread = new QThread();

    if (m_thread && m_thread->isRunning())
    {
        m_thread->quit();
        m_thread->wait();
    }
    _hardwareConnect();
    m_thread->start(QThread::NormalPriority);
    return true;
}

bool TCPLink::_hardwareConnect()
{
    if (!m_tcpConfig || !m_tcpConfig->IsValid())
        return false;
    m_socket = new QTcpSocket();

    m_socket->connectToHost(m_tcpConfig->address(), m_tcpConfig->port());
    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &TCPLink::readBytes);
    QObject::connect(m_socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
                     this, &TCPLink::_socketError);

    // Give the socket a second to connect to the other side otherwise error out
    if (!m_socket->waitForConnected(1000))
    {
        emit communicationError(tr("Link Error"), tr("Error on link %1. Connection failed").arg(getName()));
        delete m_socket;
        m_socket = nullptr;
        return false;
    }
    m_connectState = true;
    emit linkConnected(true);
    return true;
}

void TCPLink::_socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit communicationError(tr("Link Error"), tr("Error on link %1. Error on socket: %2.").arg(getName()).arg(m_socket->errorString()));
}

/**
 * @brief Check if connection is active.
 *
 * @return True if link is connected, false otherwise.
 **/
bool TCPLink::isConnected() const
{
    return m_connectState;
}

QString TCPLink::getName() const
{
    return m_tcpConfig->host();
}

qint64 TCPLink::getConnectionSpeed() const
{
    return 54000000; // 54 Mbit
}

qint64 TCPLink::getCurrentInDataRate() const
{
    return 0;
}

qint64 TCPLink::getCurrentOutDataRate() const
{
    return 0;
}

void TCPLink::_restartConnection()
{
    if(this->isConnected())
    {
        _disconnect();
        _connect();
    }
}

//--------------------------------------------------------------------------
//-- TCPCommand

static bool is_ip(const QString& address)
{
    int a,b,c,d;
    if (sscanf(address.toStdString().c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) != 4
            && strcmp("::1", address.toStdString().c_str())) {
        return false;
    } else {
        return true;
    }
}

static QString get_ip_address(const QString& address)
{
    if(is_ip(address))
        return address;
    // Need to look it up
    QHostInfo info = QHostInfo::fromName(address);
    if (info.error() == QHostInfo::NoError)
    {
        QList<QHostAddress> hostAddresses = info.addresses();
        for (int i = 0; i < hostAddresses.size(); i++)
        {
            // Exclude all IPv6 addresses
            if (!hostAddresses.at(i).toString().contains(":"))
            {
                return hostAddresses.at(i).toString();
            }
        }
    }
    return {};
}

TCPCommand::TCPCommand(QObject * p) : LinkCommand(p)
{
    loadSettings("tcpLinkConfig");
}

void TCPCommand::setPort(quint16 port)
{
    m_port = port;
}

void TCPCommand::setAddress(const QHostAddress& address)
{
    m_address = address;
}

void TCPCommand::setHost(const QString host)
{
    QString ipAdd = get_ip_address(host);
    if(ipAdd.isEmpty()) {
        qWarning() << "TCP:" << "Could not resolve host:" << host;
    } else {
        m_address = QHostAddress(ipAdd);
    }
}

LinkCommand::LinkType TCPCommand::type() const
{
    return LinkCommand::TypeTcp;
}

void TCPCommand::saveSettings(const QString& root)
{
    if (QSettings *settings = Application::Instance()->GetSettings())
    {
        settings->beginGroup(root);
        settings->setValue("port", m_port);
        settings->setValue("ip", m_address.toString());
        settings->endGroup();
    }
}

void TCPCommand::loadSettings(const QString& root)
{
    if (QSettings *settings = Application::Instance()->GetSettings())
    {
        settings->beginGroup(root);
        m_port = (quint16)settings->value("port", QGC_TCP_PORT).toUInt();
        m_address = QHostAddress(settings->value("ip", "127.0.0.1").toString());
        settings->endGroup();
    }
}

bool TCPCommand::IsValid() const
{
    return m_port > 0 && !m_address.isNull();
}

QString TCPCommand::getName() const
{
    return QString("%1:%2").arg(m_address.toString()).arg(m_port);
}

void TCPCommand::setName(const QString &s)
{
    Q_UNUSED(s);
}

void TCPCommand::setServer(const QString &host, uint16_t port)
{
    m_address = QHostAddress(host);
    m_port = port;
    saveSettings("tcpLinkConfig");
    emit hostChanged();
    emit portChanged();
    emit nameChanged(getName());
}

LinkInterface * TCPCommand::CreateLink()
{
    return new TCPLink(this);
}
