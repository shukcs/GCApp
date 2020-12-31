#ifndef VGAPPLICATION_H
#define VGAPPLICATION_H

#include <QApplication>
#include "base.h"

class VGToolBox;
class QQmlApplicationEngine;
class QThread;
class VGDbManager;
class VGUIInstance;
class VGNetManager;
class VGBDLocation;
class LinkManager;
class VGVehicleManager;
class VGQXManager;
class ParametersLimit;
class VGPlantEvents;
class VGMapManager;
class VGSurveyMonitor;
class VGPlantManager;
class VGLandManager;
class MAVLinkProtocol;
class VGBluetoothManager;
class VGController;
class VGFriendsManager;

class VGApplication : public QApplication, public Application
{
    Q_OBJECT
public:
    VGApplication(int &argc, char* argv[]);
    ~VGApplication();

    void                    initUI();
    void                    initApp();

    VGToolBox               *toolbox()const;
    LinkManager             *linkManager()const;
    MAVLinkProtocol         *mavLink()const;
    VGMapManager            *mapManager()const;
    VGDbManager             *dbManager()const;
    VGNetManager            *netManager()const;
    VGLandManager           *landManager()const;
    QThread                 *qmlThread()const;
    QQmlApplicationEngine   *getQmlEngine()const;
    VGPlantManager          *plantManager()const;
    VGSurveyMonitor         *surveyMonitor()const;
    VGVehicleManager        *vehicleManager()const;
    VGQXManager             *qxManager()const;
    VGBluetoothManager      *bluetoothManager()const;
    ParametersLimit         *parameters()const;
    VGPlantEvents           *plantEvents()const;
    VGController            *controller()const;
    VGFriendsManager        *friendsManager() const;

    int                     getNetConnectStat()const;
	QString					GetUserId()const;
    QSettings               *GetSettings()const;
	QString					GetDefualtPath();
    void                    speakNotice(const QString &value, const QStringList &param=QStringList());
    int                     countVoiceQue()const;
    void                    SetQmlTip(const QString &tip, bool bWarm=false);
	void					SetMessageBox(const QString &tip, bool bWarn = false);
    VGBDLocation            *GetBDLocation()const;
    bool                    IsPcApp()const;
public slots:
    //日志查询命令返回的信息
    void sltUpdateLogList(long time_utc, long size, int id, int num_logs, bool downloaded);
    //返回下载日志状态
    void sltUpdateLogDownloadStatus(int logId, ulong time_utc, const QString &status);
    //log下载完成
    void sltAllLogDownloaded();
protected:
    bool    notify(QObject *, QEvent *);
signals:
    void    sigBack();  //截获返回键(android)、windows（关闭键盘）
private:
    void    _initMember();
    void    _initSettings();
    void    _initParamsAndEvents();
private:
	VGUIInstance    *m_pMainUIInterface;
	VGToolBox       *m_toolBox;
	VGDbManager     *m_dbManager;
	QThread         *m_threadDb;
    QSettings       *m_setting;
    VGQXManager     *m_qxMgr;
    ParametersLimit *m_paramsLimit;
    VGPlantEvents   *m_plantEvents;
    VGController    *m_controller;
	QString         m_defualtPath;
};

#define qvgApp qobject_cast<VGApplication*>(qApp)

#endif // VGAPPLICATION_H
