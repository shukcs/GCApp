#ifndef _LINKMANAGER_H_
#define _LINKMANAGER_H_

#include <QList>
#include <QMap>
#include <QTimer>
#include "vgcomm_global.h"
#include "LinkCommand.h"

class MAVLinkProtocol;
class SerialCommand;

class VGCOMMSHARED_EXPORT LinkManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool        bleAvailable      READ IsBleAvailable    CONSTANT)
    Q_PROPERTY(bool        bleAutotLink      READ IsBleAutotLink    WRITE SetBleAutotLink NOTIFY bleAutotLinkChanged)
    Q_PROPERTY(bool        openUdp           READ IsOpenUdp         WRITE SetOpenUdp      NOTIFY openUdpChanged)
    Q_PROPERTY(QStringList serialBaudRates   READ serialBaudRates   CONSTANT)
    Q_PROPERTY(QStringList serialPortStrings READ serialPortStrings NOTIFY commPortStringsChanged)
    Q_PROPERTY(QStringList serialPorts       READ serialPorts       NOTIFY commPortsChanged)
    /// Unit Test has access to private constructor/destructor
public:
    explicit LinkManager(QObject *parent = 0);
    ~LinkManager();
    bool IsBleAvailable(void)const;
    QStringList linkTypeStrings(void) const;
    QStringList serialBaudRates(void);
    QStringList serialPortStrings(void);
    QStringList serialPorts(void);

    bool containsLink(LinkInterface *link)const;
    /// @return true: specified link is an autoconnect link
    bool isAutoconnectLink(LinkCommand *cmd)const;
    // Override from QGCTool
    //virtual void setToolbox(QGCToolbox *toolbox);
    MAVLinkProtocol *getMavlinkProtocol()const;
    void setConnectUAVStatus(bool isConnect);
    void setUAVRadioBaud(long baud); //set baud rate for UAV radio station
    bool IsOpenUdp()const;
    void SetOpenUdp(bool b);
    QList<LinkCommand *> linkCmds()const;
    void DeleteLink(LinkInterface *cmd);
    bool IsBleAutotLink()const;
    void SetBleAutotLink(bool b);
protected:
    Q_INVOKABLE LinkCommand *createLinkCmd(int tp);
    Q_INVOKABLE void endCreateLinkCmd(LinkCommand* cmd, bool bLink=true);
    Q_INVOKABLE bool isLinked(const QString &name, int tp);
    Q_INVOKABLE void shutdown(int tp);

    QList<LinkCommand *> _getLinkCmds(LinkCommand::LinkType tp);
    void timerEvent(QTimerEvent *);
signals:
    void autoconnectPixhawkChanged(bool autoconnect);
    void autoconnect3DRRadioChanged(bool autoconnect);
    void autoconnectPX4FlowChanged(bool autoconnect);
    void autoconnectRTKGPSChanged(bool autoconnect);
    void autoconnectLibrePilotChanged(bool autoconnect);
    void openUdpChanged(bool b);
    void connectLink(LinkInterface* link);
    void commPortStringsChanged();
    void commPortsChanged();
    void communicationError(const QString& title, const QString& error);
    void _shutdown(const QList<LinkCommand*> &cmds);
    void bleAutotLinkChanged();
private slots:
    void onShutdown(const QList<LinkCommand*> &cmds);
    void onCommandDeleted(LinkInterface *link);
    void onLinkDistroied();
private:
#ifndef __ios__
    void _activeLinkCheck(void);
#endif
    bool _connectionsSuspendedMsg(void);
    void _updateAutoConnectLinks(void);
    void _updateSerialPorts();
    void _addLink(LinkInterface *link);
    void _checkUdpLink();
    void _checkBleLink();
    bool _readUdpLinkConfig()const;
    void _writeUdpLinkConfig()const;
    void _updateAutoSerialLinks(QStringList &currentPorts);
#ifndef __ios__
    SerialCommand* _autoconnectCommandOfName(const QString& portName);
#endif
private:                 ///< true: Link configurations have been loaded
    uint32_t m_mavlinkChannelsUsedBitMask;
    MAVLinkProtocol *m_mavlinkProtocol;
    int             m_tmLinkCheck;
    bool            m_bConnectUAV;
    bool            m_bOpenUdp;
    bool            m_bBleAutoLink;
    long            m_UAVRadioBaud;         //波特率

    QList<LinkCommand *> m_linkCmds;
    QList<SerialCommand *> m_autolinkCmds;
    QMap<QString, int>  m_autoconnectWaitList;   ///< key: SerialPortInfo.systemLocation, value: wait count
    QStringList m_commPortList;
    QStringList m_commPortDisplayList;
};

#endif
