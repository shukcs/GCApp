/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/*!
 * @file
 *   @brief Bluetooth connection for unmanned vehicles
 *   @author Gus Grubba <mavlink@grubba.com>
 *
 */

#ifndef BTLINK_H
#define BTLINK_H

#include <QString>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothSocket>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>

//#include "QGCConfig.h"
#include "LinkCommand.h"
#include "LinkInterface.h"

class QBluetoothDeviceDiscoveryAgent;
class QBluetoothServiceDiscoveryAgent;
class QBluetoothServiceInfo;
class QLowEnergyController;

class VGCOMMSHARED_EXPORT BluetoothCommand : public LinkCommand
{
    Q_OBJECT
#ifndef __ios__
    Q_PROPERTY(QString      address     READ address   NOTIFY addressChanged)
#endif //!__ios__
    Q_PROPERTY(QStringList  devices     READ devices   NOTIFY devicesChanged)
    Q_PROPERTY(int          countDev    READ GetDevSize NOTIFY countChanged)
    Q_PROPERTY(bool         scanning    READ scanning  NOTIFY scanningChanged)
public:
    BluetoothCommand(QObject *p=NULL);
    ~BluetoothCommand();

    Q_INVOKABLE void        startScan();
    Q_INVOKABLE void        stopScan();

    QString getName()const;
    void setName(const QString &s);
    QString address()const;
    bool scanning()const;

    const QBluetoothDeviceInfo &device()const;
    LinkType    type()const;
    void updateSettings();
    QStringList devices()const;
    int GetDevSize()const;
public:
    static bool IsBluetoothAvailable();
protected:
    LinkInterface *CreateLink();
signals:
    void        newDevice(const QBluetoothDeviceInfo &info);
    void        countChanged(int sz);
#ifndef __ios__
    void        addressChanged(const QString &str);
#endif //!__ios__
    void        devNameChanged(const QString &str);
    void        devicesChanged();
    void        scanningChanged         (bool b);
protected slots:
    void        deviceDiscovered        (const QBluetoothDeviceInfo &info);
    void        doneScanning            ();
private:
    QBluetoothDeviceInfo                m_device;
    QString                             m_name;
private:
    static QBluetoothDeviceDiscoveryAgent*      s_deviceDiscover;
    static QMap<QString, QBluetoothDeviceInfo>  s_devices;
    static bool                                 s_bScan;
};

class  BluetoothLink : public LinkInterface
{
    Q_OBJECT
public:
    void    requestReset();
    bool    isConnected             () const;

    // Extensive statistics for scientific purposes
    qint64  getConnectionSpeed      () const;
    qint64  getCurrentInDataRate    () const;
    qint64  getCurrentOutDataRate   () const;
    LinkCommand* getLinkCommand()const;
public slots:
    //void    readBytes               ();
/*#ifdef __ios__
    void    serviceDiscovered       (const QBluetoothServiceInfo &info);
    void    discoveryFinished       ();
#endif*/
protected slots:
    void    serviceStateChanged(QLowEnergyService::ServiceState st);
    void    characteristicWritten(const QLowEnergyCharacteristic &chr, const QByteArray &array);
    void    characteristicChanged(const QLowEnergyCharacteristic &chr, const QByteArray &array);
    void    finishScanService();
    void    disconnect();
private:
    BluetoothLink(LinkCommand *cmd);
    ~BluetoothLink();

    // From LinkInterface
    bool _connect(void);
    void _disconnect(void);

    bool _hardwareConnect();
    void _restartConnection();
    void writeBytes(const QByteArray &array);
private:
    void _send();
    BluetoothCommand *bltCommand()const;
    void _prcsServiceDiscovered(QLowEnergyService *svc);
private:
    friend class BluetoothCommand;
#ifdef __ios__
    QBluetoothServiceDiscoveryAgent* _discoveryAgent;
#endif
    bool                            m_connectState;
    QLowEnergyCharacteristic        m_chrWrite;
    QLowEnergyService               *m_leSvc;
    QLowEnergyController            *m_leCtrl;
    QObjectList                     m_servics;
    QByteArray                      m_sendBuff;
};

#endif // BTLINK_H
