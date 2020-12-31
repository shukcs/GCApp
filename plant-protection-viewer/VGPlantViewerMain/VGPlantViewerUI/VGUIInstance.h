﻿#ifndef VGUIINSTANCE_H
#define VGUIINSTANCE_H

#include <QtPlugin>
#include <QQmlContext>

class QQmlApplicationEngine;
class VGMapManager;
class VGLandManager;
class VGPlantManager;
class VGNetManager;
class VGSurveyMonitor;
class VGBluetoothManager;
class VGMainPage;
class VGLogDataObject;
class VGFriendsManager;
class VGMissionHistory;

class VGUIInstance : public QObject
{
    Q_OBJECT
public:
    VGUIInstance(QObject* parent = NULL);
    ~VGUIInstance();

    void        createQml();
    void        setUIFontSize(int fontPixelSize);

	QQmlApplicationEngine &getQQmlApplicationEngine();
    VGLandManager       *landManager()const;
    VGMapManager        *mapManager()const;
    VGNetManager        *netManager()const;
    VGPlantManager      *plantManager()const;
    VGMainPage          *vgMainPage()const;
    VGSurveyMonitor     *surveyMonitor()const;
    VGFriendsManager    *friendsManager() const;
    VGMissionHistory    *missionHistory() const;
public slots:
    //收到日志文件列表信息
    void sltReceiveLogList(long time_utc, long size, int id, int num_logs, bool downloaded);
	void sltUpdateLogDownloadStatus(int logId, ulong time_utc, const QString &status);
    void sltAllLogDownload();

	void sltSetUIErrorInfo(const QString &title, const QString &notes);
private slots:
    void sltUpdateLogList();
private:
signals:
    void    updateLogList();
    void    sigRequestLogData(int logId, long size, long time_utc);
private:
    QQmlApplicationEngine   *m_qmlEngine;
    VGMainPage              *m_vgMainPage;
    VGNetManager            *m_vgNetManager;
    VGMapManager            *m_mapManager;
    VGFriendsManager        *m_friendsManager;
    VGLogDataObject         *m_logDataObject;
    VGLandManager           *m_landManager;
    VGPlantManager          *m_plantManager;
    VGSurveyMonitor         *m_surveyMonitor;
    VGBluetoothManager      *m_bltMgr;
    VGMissionHistory        *m_missionHistory;
};

#endif // VGUIINSTANCE_H
