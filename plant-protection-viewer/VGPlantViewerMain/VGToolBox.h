#ifndef VGTOOLBOX_H
#define VGTOOLBOX_H

#include <QObject>

class MAVLinkProtocol;
class VGVehicleManager;
class LinkManager;
class AudioWorker;
class VGVehicleLogManager;
class VGVehicle;
class VGBluetoothManager;

class VGToolBox : public QObject
{
    Q_OBJECT
public:
    explicit VGToolBox(QObject *parent = 0);
    ~VGToolBox();
public:
    void connectLink();
public:
    MAVLinkProtocol*        mavlinkProtocol()const;
    VGVehicleManager*       vehicleManager()const;
    LinkManager*            linkManager()const;
    AudioWorker*            audioOutput()const;
    VGVehicleLogManager*    vgLogManager()const;
    VGVehicle*              activeVehicle()const;
    VGVehicle*              surveyVehicle()const;
    VGBluetoothManager*     bluetoothManager()const;
    void                    SpeakNotice(const QString &value, const QStringList &param);
    int                     countVoiceQue();
public slots:
    //void    sltVehicleVoiceAlarm(const QString &text); //��������
    void    sltRequestLogData(int logId, long size, long time_utc);
    void    sltActiveVehicleChanged(VGVehicle* activeVehicle);
private slots:
    void    sltLinkManagerError(const QString& title, const QString& error);
signals :
    //��ʾ��Ϣ
    void    sigShowNote(const QString &note);
    //��ѯ��־�б�
    void    sigRequestLogList();
    //��־��ѯ����ص���Ϣ
    void    updateLogList(uint32_t time_utc, uint32_t size, uint16_t id, uint16_t num_logs, bool downloaded);
    //����������־״̬
    void    sigUpdateDownloadStatus(int logId, uint32_t time_utc, const QString &status);
    //����log�������
    void    sigAllLogDownloaded();

    void    sigRequestLogDataThread(int logId, long size, long time_utc);
    void    sigLoadDownloadedLogThread();
    void    sigSetActiveVehicle(VGVehicle *activeVehicle);
	void    sigSetLogDownloadedDir(const QString &strDir);
private:
    LinkManager             *m_linkManager;
    MAVLinkProtocol         *m_mavlinkProtocol;
    VGVehicleManager        *m_vgVehicleManager;
    VGBluetoothManager      *m_bltManager;
    AudioWorker             *m_vgAudioWorker;
    VGVehicleLogManager     *m_vgLogManager;
    QThread                 *m_logThread;
};

#endif // VGTOOLBOX_H
