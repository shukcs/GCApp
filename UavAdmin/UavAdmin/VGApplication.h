#ifndef VGAPPLICATION_H
#define VGAPPLICATION_H

#include <QApplication>
#include "base.h"

class QThread;
class QQmlApplicationEngine;
class VGUIInstance;
class VGNetManager;
class VGMapManager;
class VGPlantManager;
class AudioWorker;

class VGApplication : public QApplication, public Application
{
    Q_OBJECT
public:
    VGApplication(int &argc, char* argv[]);
    ~VGApplication();

    void                    initUI();
    void                    initApp();

    VGMapManager            *mapManager()const;
    VGNetManager            *netManager()const;
    VGPlantManager          *plantManager()const;
    QThread                 *qmlThread()const;
    QQmlApplicationEngine   *getQmlEngine()const;

    int                     getNetConnectStat()const;
	QString					GetUserId()const;
    QSettings               *GetSettings()const;
	QString					GetDefualtPath();
    void                    speakNotice(const QString &value, const QStringList &param=QStringList());
    int                     countVoiceQue()const;
    void                    SetQmlTip(const QString &tip, bool bWarm=false);
	void					SetMessageBox(const QString &tip, bool bWarn = false);
    bool                    IsPcApp()const;
protected:
    bool    notify(QObject *, QEvent *);
signals:
    void    sigBack();  //截获返回键(android)、windows（关闭键盘）
    //设备各个状态信息
    void    sigAttitudeChanged(double compass, double roll, double pitch);
    void    sigSpeedChanged(double speedX, double speedY);
    void    sigPositionChanged(double lat, double lon, int nSatNum);
    void    sigAltitudeChanged(double altitude);
    void    sigRelativeAltitudeChanged(double altitude);
    void    sigMedicalSpeedChanged(double speed);
    void    sigMedicalTotalChanged(double total);
    void    sigBatteryChanged(int battery);
    void    sigDistanceChanged(double distance);
    void    sigConnectionLostChanged(bool connectionLost);
    //提示信息
	void    sigShowNote(const QString &);
private:
    void    _initMember();
    void    _initSettings();
    void    _initParamsAndEvents();
private:
    VGUIInstance    *m_pMainUIInterface;
    AudioWorker     *m_audioWorker;
    QSettings       *m_setting;
	QString         m_defualtPath;
};

#define qvgApp qobject_cast<VGApplication*>(qApp)

#endif // VGAPPLICATION_H
