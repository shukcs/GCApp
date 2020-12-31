#ifndef VGUIINSTANCE_H
#define VGUIINSTANCE_H

#include <QtPlugin>
#include <QQmlContext>

class QQmlApplicationEngine;
class VGMapManager;
class VGPlantManager;
class VGNetManager;
class VGMainPage;

class VGUIInstance : public QObject
{
    Q_OBJECT
public:
    VGUIInstance(QObject* parent = NULL);
    ~VGUIInstance();

    void        createQml();
    void        setUIFixedSize(const QSize &size);
    void        setUIFontSize(int fontPixelSize);

	QQmlApplicationEngine &getQQmlApplicationEngine();
    VGMapManager        *mapManager()const;
    VGNetManager        *netManager()const;
    VGPlantManager      *plantManager()const;
    VGMainPage          *vgMainPage()const;
public slots:
	void sltSetUIErrorInfo(const QString &title, const QString &notes);
private slots:
    void sltUpdateLogList();
private:
signals:
    void    sigUpdateLogList();
    void    sigRequestLogData(int logId, long size, long time_utc);
private:
    QQmlApplicationEngine   *m_qmlEngine;
    VGMainPage              *m_vgMainPage;
    VGNetManager            *m_vgNetManager;
    VGMapManager            *m_mapManager;
    VGPlantManager          *m_plantManager;
};

#endif // VGUIINSTANCE_H
