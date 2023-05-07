#include "LinkManager.h"
#include <QList>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTimerEvent>
#include <MAVLinkProtocol.h>

#ifndef __ios__
#include "SerialPortInfo.h"
#endif

#include "UDPLink.h"
#include "LinkInterface.h"

#ifdef QGC_ENABLE_BLUETOOTH
#include "BluetoothLink.h"
#endif

#ifndef __ios__
#include "SerialLink.h"
#endif

Q_DECLARE_LOGGING_CATEGORY(LinkManagerLog)
Q_DECLARE_LOGGING_CATEGORY(LinkManagerVerboseLog)

#define ActiveLinkCheckTimeoutSecs 15 //< Amount of time to wait for a heatbeat. Keep in mind ArduPilot stack heartbeat is slow to come.
#define AutoconnectUpdateTimerMSecs 1000
#ifdef Q_OS_WIN
#define AutoconnectConnectDelayMSecs 6000
#else
#define AutoconnectConnectDelayMSecs 1000
#endif


LinkManager::LinkManager(QObject *parent) : QObject(parent), m_mavlinkChannelsUsedBitMask(0)
, m_tmLinkCheck(-1), m_mavlinkProtocol(NULL), m_bConnectUAV(false), m_bBleAutoLink(false)
, m_bCheckPx4(false), m_UAVRadioBaud(57600)
{
    m_mavlinkProtocol = new MAVLinkProtocol(this);
    m_tmLinkCheck = startTimer(AutoconnectUpdateTimerMSecs);
    connect(this, &LinkManager::_shutdown, this, &LinkManager::onShutdown);
    m_bOpenUdp = _readUdpLinkConfig();
}

LinkManager::~LinkManager()
{
    while (m_linkCmds.count() > 0)
    {
        delete m_linkCmds.takeFirst();
    }
    delete m_mavlinkProtocol;
}

void LinkManager::_checkUdpLink()
{
    if (!m_bOpenUdp)
        return;

    for (LinkCommand *cmd : m_linkCmds)
    {
        if (cmd->type() == LinkCommand::TypeUdp)
            return;
    }

    auto udpConfig = LinkCommand::createSettings(LinkCommand::TypeUdp, this);
    m_linkCmds.append(udpConfig);
    udpConfig->connectLink();
    _addLink(udpConfig->link());
}

void LinkManager::_checkBleLink()
{
    if (!m_bBleAutoLink)
        return;

#ifdef QGC_ENABLE_BLUETOOTH
    for (auto cmd : _getLinkCmds(LinkCommand::TypeBluetooth))
    {
        if (!cmd->getName().isEmpty())
            cmd->connectLink();
    }
#endif
}

bool LinkManager::_readUdpLinkConfig() const
{
    if (QSettings *st = Application::Instance()->GetSettings())
    {
        st->beginGroup("udpLinkConfig");
        bool bOpen = st->value("open", Application::Instance()->IsPcApp()).toBool();
        st->endGroup();
        return bOpen;
    }
    return Application::Instance()->IsPcApp();
}

void LinkManager::_writeUdpLinkConfig() const
{
    if (QSettings *st = Application::Instance()->GetSettings())
    {
        st->beginGroup("udpLinkConfig");
        st->setValue("open", m_bOpenUdp);
        st->endGroup();
    }
}

void LinkManager::_updateAutoSerialLinks(QStringList &currentPorts)
{
    for (const SerialPortInfo portInfo : SerialPortInfo::availablePorts())
    {
        currentPorts << portInfo.systemLocation();
        SerialPortInfo::BoardType_t boardType = portInfo.boardType();

        if (boardType != SerialPortInfo::BoardTypeUnknown)
        {
            if (portInfo.isBootloader())
                continue;
#ifndef __android__
            if (portInfo.boardTypeGPS())
                continue;
#endif
            if (_autoconnectCommandOfName(portInfo.systemLocation()))
                continue;

            if (!m_autoconnectWaitList.contains(portInfo.systemLocation()))
            {
                m_autoconnectWaitList[portInfo.systemLocation()] = 1;
                continue;
            }

            if (++m_autoconnectWaitList[portInfo.systemLocation()] * AutoconnectUpdateTimerMSecs > AutoconnectConnectDelayMSecs)
            {
                SerialCommand *pSerialCmd = NULL;
                m_autoconnectWaitList.remove(portInfo.systemLocation());
                switch (boardType)
                {
                case SerialPortInfo::BoardTypePX4FMUV1:
                case SerialPortInfo::BoardTypePX4FMUV2:
                case SerialPortInfo::BoardTypePX4FMUV4:
                case SerialPortInfo::BoardTypeAeroCore:
                    pSerialCmd = new SerialCommand(this);
                    pSerialCmd->setUsbDirect(true);
                    break;
                case SerialPortInfo::BoardTypePX4Flow:
                    pSerialCmd = new SerialCommand();
                    break;
                case SerialPortInfo::BoardType3drRadio:
                    pSerialCmd = new SerialCommand(this);
                    pSerialCmd->setStopBits(1);
                    pSerialCmd->setFlowControl(0);
                    pSerialCmd->setDataBits(8);
                    pSerialCmd->setParity(0);
                    break;
                default:
                    qWarning() << "Internal error";
                    continue;
                }

                if (pSerialCmd)
                {
                    if (boardType == SerialPortInfo::BoardType3drRadio)
                        pSerialCmd->setBaud(m_UAVRadioBaud);
                    else
                        pSerialCmd->setBaud(115200);

                    pSerialCmd->setName(portInfo.systemLocation());
                    connect(pSerialCmd, &QObject::destroyed, this, &LinkManager::onLinkDistroied);
                    m_autolinkCmds.append(pSerialCmd);
                    pSerialCmd->connectLink();
                    _addLink(pSerialCmd->link());
                }
            }
        }
    }
}

void LinkManager::_addLink(LinkInterface *link)
{
    if (!link || !link->getLinkCommand())
        return;

    if (!containsLink(link))
    {
        int i = 1;
        for (; i<32; i++)
        {
            if (!(m_mavlinkChannelsUsedBitMask & 1 << i))
            {
                mavlink_reset_channel_status(i);
                link->_setMavlinkChannel(i);
                m_mavlinkChannelsUsedBitMask |= i << i;
                break;
            }
        }

        if (i >= 32)
            qWarning() << "Ran out of mavlink channels";
    }

    connect(link, &LinkInterface::bytesReceived, m_mavlinkProtocol, &MAVLinkProtocol::receiveBytes, Qt::UniqueConnection);
    connect(link, &LinkInterface::communicationError, this, &LinkManager::communicationError, Qt::UniqueConnection);
    m_mavlinkProtocol->resetMetadataForLink(link);
}
 
void LinkManager::DeleteLink(LinkInterface *link)
{
    LinkCommand *cmd = link ? link->getLinkCommand() : NULL;
    if (!cmd || cmd->type() ==LinkCommand::TypeUdp)
        return;

    if (m_linkCmds.removeAll(cmd) + m_autolinkCmds.removeAll((SerialCommand*)cmd) < 1)
        return;

    m_mavlinkChannelsUsedBitMask &= ~(1 << link->getMavlinkChannel());
    cmd->deleteLater();
}

bool LinkManager::IsBleAutotLink() const
{
    return m_bBleAutoLink;
}

void LinkManager::SetBleAutotLink(bool b)
{
    if (m_bBleAutoLink == b)
        return;

    m_bBleAutoLink = b;
    emit bleAutotLinkChanged();
}

void LinkManager::onShutdown(const QList<LinkCommand*> &cmds)
{
    for (LinkCommand *itr : cmds)
    {
        DeleteLink(itr->link());
    }
}

void LinkManager::onCommandDeleted(LinkInterface *link)
{
    LinkCommand *cmd = link->getLinkCommand();
    if (!link || !cmd || !m_linkCmds.contains(cmd) || !cmd->link())
        return;

    m_mavlinkChannelsUsedBitMask &= ~(1 << link->getMavlinkChannel());
    m_linkCmds.removeAll(cmd);
    if (m_autolinkCmds.contains((SerialCommand*)cmd))
        m_autolinkCmds.removeAll((SerialCommand*)cmd);
}

void LinkManager::onLinkDistroied()
{
    auto s = (SerialCommand*)sender();
    m_autolinkCmds.removeAll(s);
}

#ifndef __ios__
SerialCommand* LinkManager::_autoconnectCommandOfName(const QString& portName)
{
    QString searchPort = portName.trimmed();
    for (SerialCommand *cmd: m_autolinkCmds)
    {
        if (cmd && cmd->getName() == searchPort)
        {
            return cmd;
        }
    }
    return NULL;
}
#endif

void LinkManager::_updateAutoConnectLinks(void)
{
    if (m_bConnectUAV)
        return;

    _checkUdpLink();
#ifndef __ios__
    QStringList currentPorts;
    if (m_bCheckPx4)
        _updateAutoSerialLinks(currentPorts);

    _checkBleLink();
    for (SerialCommand *linkCmd : m_autolinkCmds)
    {
        if (!linkCmd)
            continue;

        if (!currentPorts.contains(linkCmd->getName()))
        {
            if (linkCmd->isConnect() && linkCmd->link()->IsActive())
                continue;

            DeleteLink(linkCmd->link());
        }
    }
#endif // !__ios__
}

QStringList LinkManager::linkTypeStrings(void) const
{
    //-- Must follow same order as enum LinkType in LinkCommand.h
    static QStringList list;
    if(!list.size())
    {
#ifndef __ios__
        list += tr("Serial");
#endif
        list += tr("UDP");
        //list += "TCP";
#ifdef QGC_ENABLE_BLUETOOTH
        list += tr("BlueTooth");
#endif
    }
    return list;
}

void LinkManager::_updateSerialPorts()
{
    m_commPortDisplayList.clear();
#ifndef __ios__
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portList)
    {
        m_commPortDisplayList += SerialCommand::cleanPortDisplayname(info.systemLocation().trimmed());
    }
    emit commPortsChanged();
#endif
}

QStringList LinkManager::serialPorts(void)
{
    return m_commPortDisplayList;
}

bool LinkManager::containsLink(LinkInterface *link)const
{
    if (!link)
        return false;
    for (LinkCommand *itr : m_linkCmds)
    {
        if (itr->link() == link)
            return true;
    }
    for (LinkCommand *itr : m_autolinkCmds)
    {
        if (itr->link() == link)
            return true;
    }
    return false;
}

QStringList LinkManager::serialBaudRates(void)
{
#ifdef __ios__
    QStringList foo;
    return foo;
#else
    return SerialCommand::supportedBaudRates();
#endif
}

LinkCommand *LinkManager::createLinkCmd(int tp)
{
#ifndef __ios__
    if(tp == LinkCommand::TypeSerial)
        _updateSerialPorts();
#endif
    return LinkCommand::createSettings(tp, this);
}

void LinkManager::endCreateLinkCmd(LinkCommand *cmd, bool bLink)
{
    if (!cmd)
        return;

    if (!bLink || (isLinked(cmd->getName(), cmd->type()) && !m_linkCmds.contains(cmd)))
    {
        cmd->deleteLater();
        return;
    }

    if (!m_linkCmds.contains(cmd))
        m_linkCmds.append(cmd);

    cmd->connectLink();
    _addLink(cmd->link());
}

bool LinkManager::isLinked(const QString &name, int tp)
{
    for (LinkCommand *itr : m_linkCmds)
    {
        if (itr->type() == tp && itr->getName() == name)
            return itr->isConnect();
    }

    return false;
}

void LinkManager::shutdown(int tp)
{
    emit _shutdown(_getLinkCmds((LinkCommand::LinkType)tp));
}

bool LinkManager::IsAutoLinkPX4() const
{
    return m_bCheckPx4;
}

void LinkManager::SetAutoLinkPX4(bool b)
{
    if (m_bCheckPx4 != b)
    {
        m_bCheckPx4 = b;
        emit autoLinkPX4Changed();
    }
}

QList<LinkCommand *> LinkManager::_getLinkCmds(LinkCommand::LinkType tp)
{
    if (tp == LinkCommand::TypeLast)
        return m_linkCmds;

    QList<LinkCommand *> ret;
    for (LinkCommand *itr : m_linkCmds)
    {
        if (itr->type() == tp)
            ret << itr;
    }
    return ret;
}

void LinkManager::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_tmLinkCheck)
    {
        _updateAutoConnectLinks();
        _updateSerialPorts();
        return;
    }
    QObject::timerEvent(e);
}

bool LinkManager::isAutoconnectLink(LinkCommand *cmd)const
{
    if (!cmd || cmd->type() != LinkCommand::TypeSerial)
        return false;

    return m_autolinkCmds.contains((SerialCommand*)cmd);
}

MAVLinkProtocol * LinkManager::getMavlinkProtocol() const
{
    return m_mavlinkProtocol;
}

void LinkManager::setConnectUAVStatus(bool isConnect)
{
    m_bConnectUAV = isConnect;
}

void LinkManager::setUAVRadioBaud(long baud)
{
    m_UAVRadioBaud = baud;
}

bool LinkManager::IsOpenUdp() const
{
    return m_bOpenUdp;
}

void LinkManager::SetOpenUdp(bool b)
{
    if (m_bOpenUdp == b)
        return;

    m_bOpenUdp = b;
    emit openUdpChanged(b);
    _writeUdpLinkConfig();
}

QList<LinkCommand *> LinkManager::linkCmds() const
{
    return m_linkCmds;
}

bool LinkManager::IsBleAvailable(void)const
{
#ifdef QGC_ENABLE_BLUETOOTH
    return BluetoothCommand::IsBluetoothAvailable();
#else
    return false;
#endif
}
