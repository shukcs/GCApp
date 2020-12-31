#include "BluetoothLink.h"
#include <QtGlobal>
#include <QTimer>
#include <QList>
#include <QDebug>

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothUuid>
#include <QtBluetooth/QBluetoothSocket>
#include <QLowEnergyController>
#include <QSettings>
#include <QDateTime>


#define JDK_TCUID "FFE1"   //透传特征
enum {
    MAX_WRITTEN = 20,   //蓝牙一次最多写20字节
    BLTWriteEnable = (int)QLowEnergyCharacteristic::Write|QLowEnergyCharacteristic::WriteNoResponse,
};

BluetoothLink::BluetoothLink(LinkCommand *cmd) : LinkInterface(cmd)
, m_connectState(false), m_leCtrl(NULL), m_leSvc(NULL)
#ifdef __ios__
, _discoveryAgent(NULL)
#endif
{
}

BluetoothLink::~BluetoothLink()
{
    qDeleteAll(m_servics);
    _disconnect();
#ifdef __ios__
    if(_discoveryAgent)
    {
        _discoveryAgent->stop();
        _discoveryAgent->deleteLater();
        _discoveryAgent = NULL;
    }
#endif
}

LinkCommand* BluetoothLink::getLinkCommand() const
{
    return dynamic_cast<LinkCommand*>(parent());
}

void BluetoothLink::_restartConnection()
{
    if(this->isConnected())
    {
        _disconnect();
        _connect();
    }
}

void BluetoothLink::characteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)
{
}

void BluetoothLink::characteristicChanged(const QLowEnergyCharacteristic &chr, const QByteArray &array)
{
    if (m_leSvc != sender())
        m_leSvc = (QLowEnergyService *)sender();

    if (!m_chrWrite.isValid() && (chr.properties()&BLTWriteEnable))
        m_chrWrite = chr;

    emit bytesReceived(this, array);
}

void BluetoothLink::writeBytes(const QByteArray &array)
{
    m_sendBuff.append(array);
    if (m_sendBuff.size() <= array.size())
        _send();
}

void BluetoothLink::_send()
{
    if (m_leSvc && m_chrWrite.isValid())
    {
        int nSend = m_sendBuff.size() < MAX_WRITTEN ? m_sendBuff.size() : MAX_WRITTEN;
        m_leSvc->writeCharacteristic(m_chrWrite, m_sendBuff.left(nSend));
        m_sendBuff.remove(0, nSend);
    }
    if (!m_sendBuff.isEmpty())
        _send();
}

BluetoothCommand * BluetoothLink::bltCommand()const
{
    return dynamic_cast<BluetoothCommand*>(getLinkCommand());
}

void BluetoothLink::_prcsServiceDiscovered(QLowEnergyService *svc)
{
    if (!svc || m_leSvc)
        return;

    QLowEnergyCharacteristic tmp;
    for (const QLowEnergyCharacteristic &var : svc->characteristics())
    {
        QString uuid = var.uuid().toString();
        QLowEnergyDescriptor dsc = var.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
        bool bJDk = uuid.contains(JDK_TCUID, Qt::CaseInsensitive);
        if (bJDk || dsc.isValid())
        {
            if (m_leSvc && m_leSvc != svc)
                m_leSvc->deleteLater();
            svc->writeDescriptor(dsc, QByteArray::fromHex("0100"));  //回写，接受数据
            m_leSvc = svc;
            m_connectState = true;
            emit linkConnected(true);
            if (bJDk)
                m_chrWrite = var;
        }
        if (!m_chrWrite.isValid() && m_leSvc == svc && BLTWriteEnable==(var.properties() & BLTWriteEnable))
            m_chrWrite = var;
    }
    if (tmp.isValid() && !m_leSvc)
    {
        m_leSvc = svc;
        m_chrWrite = tmp;
    }
}

void BluetoothLink::_disconnect(void)
{
#ifdef __ios__
    if(_discoveryAgent) {
        _shutDown = true;
        _discoveryAgent->stop();
        _discoveryAgent->deleteLater();
        _discoveryAgent = NULL;
    }
#endif
    if(m_leCtrl)
    {
        m_leCtrl->disconnectFromDevice();
        m_leCtrl->deleteLater();
        m_leCtrl = NULL;
        m_leSvc = NULL;
        m_chrWrite = QLowEnergyCharacteristic();
    }
    if (m_connectState)
    {
        m_connectState = false;
        emit linkConnected(false);
    }
}

bool BluetoothLink::_connect(void)
{
    _hardwareConnect();
    return true;
}

bool BluetoothLink::_hardwareConnect()
{
    BluetoothCommand *cmd = bltCommand();
    if (!cmd || !cmd->device().isValid())
        return false;

    if (m_leCtrl)
        delete m_leCtrl;

    m_leSvc = NULL;
    m_chrWrite = QLowEnergyCharacteristic();
    if (m_leCtrl = QLowEnergyController::createCentral(cmd->device(), this))
    {
        connect(m_leCtrl, &QLowEnergyController::discoveryFinished, this, &BluetoothLink::finishScanService);
        connect(m_leCtrl, &QLowEnergyController::disconnected, this, &BluetoothLink::disconnect);
        connect(m_leCtrl, &QLowEnergyController::stateChanged, this, 
        [this](QLowEnergyController::ControllerState st)
        {
            switch (st)
            {
            case QLowEnergyController::UnconnectedState:
                emit linkConnected(m_connectState=false);
                break;
            case QLowEnergyController::ConnectedState:
                m_leCtrl->discoverServices();
                break;
            default:
                break;
            }
        });
        connect(m_leCtrl, static_cast<void(QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error), this,
        [this](QLowEnergyController::Error er)
        {
            m_connectState = false;
            qWarning() << "BlueTooth error" << er;
            emit communicationError("BlueTooth Link Error", m_leCtrl->errorString());
        });

        m_leCtrl->connectToDevice();
    }
    return true;
}

void BluetoothLink::finishScanService()
{
    if (!m_leCtrl)
        return;
    qDeleteAll(m_servics);
    m_servics.clear();

    for (const QBluetoothUuid &uuid : m_leCtrl->services())
    {
        QLowEnergyService *svc = m_leCtrl->createServiceObject(uuid, this);
        int type = svc ? int(svc->type()) : 0;
        if (type == QLowEnergyService::IncludedService)
        {
            svc->deleteLater();
        }
        else if (type & QLowEnergyService::PrimaryService)
        {
            connect(svc, &QLowEnergyService::stateChanged, this, &BluetoothLink::serviceStateChanged);
            connect(svc, &QLowEnergyService::characteristicChanged, this, &BluetoothLink::characteristicChanged);
            connect(svc, &QLowEnergyService::characteristicRead, this, &BluetoothLink::characteristicChanged);
            connect(svc, &QLowEnergyService::characteristicWritten, this, &BluetoothLink::characteristicWritten);
            svc->discoverDetails();
            m_servics.append(svc);
            QObject::connect(svc, static_cast<void(QLowEnergyService::*)(QLowEnergyService::ServiceError)>(&QLowEnergyService::error), this,
            [this](QLowEnergyService::ServiceError er)
            {
                if (er == QLowEnergyService::CharacteristicWriteError)
                    m_sendBuff.clear();
            });
        }
    }
}

void BluetoothLink::disconnect()
{
    _disconnect();
}

void BluetoothLink::serviceStateChanged(QLowEnergyService::ServiceState st)
{
    QLowEnergyService *svc = qobject_cast<QLowEnergyService *>(sender());
    if (!svc)
        return;

    switch (st)
    {
    case QLowEnergyService::ServiceDiscovered:
        _prcsServiceDiscovered(svc);
        break;
    default:
        break;
    }
}

void BluetoothLink::requestReset()
{
    if (BluetoothCommand *cmd = bltCommand())
        cmd->deleteLater();
}

bool BluetoothLink::isConnected() const
{
    return m_connectState;
}

qint64 BluetoothLink::getConnectionSpeed() const
{
    return 100000; // 1 Mbit
}

qint64 BluetoothLink::getCurrentInDataRate() const
{
    return 0;
}

qint64 BluetoothLink::getCurrentOutDataRate() const
{
    return 0;
}

bool BluetoothCommand::IsBluetoothAvailable()
{
    QBluetoothLocalDevice localDevice;
    if (localDevice.isValid())
        return true;

#ifdef _DEBUG
    return true;
#else
    return false;
#endif
}

LinkInterface * BluetoothCommand::CreateLink()
{
    return new BluetoothLink(this);
}

///////////////////////////////////////////////////////////////////////////////
//-- BluetoothCommand
///////////////////////////////////////////////////////////////////////////////
QBluetoothDeviceDiscoveryAgent* BluetoothCommand::s_deviceDiscover = NULL;
QMap<QString, QBluetoothDeviceInfo> BluetoothCommand::s_devices;
bool BluetoothCommand::s_bScan = false;

BluetoothCommand::BluetoothCommand(QObject *p): LinkCommand(p)
{
    if (!s_deviceDiscover)
    {
        s_deviceDiscover = new QBluetoothDeviceDiscoveryAgent();
        s_deviceDiscover->setLowEnergyDiscoveryTimeout(3000);
        emit scanningChanged(s_bScan);
    }

    connect(s_deviceDiscover, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothCommand::deviceDiscovered);
    connect(s_deviceDiscover, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothCommand::doneScanning);
}

BluetoothCommand::~BluetoothCommand()
{
}

void BluetoothCommand::updateSettings()
{
    if(m_link)
    {
        BluetoothLink *ulink = dynamic_cast<BluetoothLink*>(m_link);
        if(ulink)
            ulink->_restartConnection();
    }
}

void BluetoothCommand::stopScan()
{
    if(s_deviceDiscover)
    {
        s_deviceDiscover->stop();
        s_bScan = false;
        emit scanningChanged(s_bScan);
    }
}

QString BluetoothCommand::getName() const
{
    return m_device.name();
}

void BluetoothCommand::setName(const QString &s)
{
    auto itr = s_devices.find(s);
    if (itr!= s_devices.end())
    {
        auto data = itr.value();
        m_device = data;
        emit nameChanged(m_device.name());
#ifndef __ios__
        emit addressChanged(data.address().toString());
#endif
    }
}

void BluetoothCommand::startScan()
{
    if (IsBluetoothAvailable() && s_deviceDiscover)
    {
        s_deviceDiscover->stop();
        s_deviceDiscover->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
        s_deviceDiscover->start();
        s_bScan = true;
        emit scanningChanged(s_bScan);
        s_devices.clear();
        emit devicesChanged();
    }
}

void BluetoothCommand::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    bool bChanged = false;
    if(info.isValid())
    {    
        QString name = info.name();
        if (name.isEmpty())
            name = info.address().toString();
        else
            name.remove(QRegExp("\r|\n"));
        s_devices[name] = info;
        emit devicesChanged();
        bChanged = true;
    }
    if (bChanged)
        emit countChanged(GetDevSize());
}

void BluetoothCommand::doneScanning()
{
    s_bScan = false;
    if(s_deviceDiscover)
        emit scanningChanged(s_bScan);
}

const QBluetoothDeviceInfo &BluetoothCommand::device() const
{
    return m_device;
}

QString BluetoothCommand::address()const
{
#ifdef __ios__
    return QString("");
#else
    return m_device.address().toString();
#endif
}

QStringList BluetoothCommand::devices()const
{
    return s_devices.keys();
}

int BluetoothCommand::GetDevSize()const
{
    return s_devices.size();
}

bool BluetoothCommand::scanning() const
{
    return s_bScan;
}

LinkCommand::LinkType BluetoothCommand::type() const
{
    return LinkCommand::TypeBluetooth;
}
