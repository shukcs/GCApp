#include "VGApplication.h"
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QGCMapEngine.h>
#include <VGNetManager.h>
#include <QDir>
#include <QFont>
#include <QThread>
#include "audio/AudioWorker.h"
#include "VGUIInstance.h"
#include "VGMainPage.h"

#include "math.h"

#ifdef __mobile__
#include <QStandardPaths>
#endif

#define CATCHFOLD "UavAdmin"

VGApplication::VGApplication(int &argc, char* argv[])
    : QApplication(argc, argv), m_pMainUIInterface(NULL)
    , m_setting(NULL)
{
    setFont(QFont("", 12));
    setApplicationName("UavAdmin");

    if (QTranslator *translator = new QTranslator(this))
    {
#ifdef __mobile__
        QString path = "assets:/lng/UavAdmin.qm";
#else
        QString path = "UavAdmin.qm";
#endif
        translator->load(path);
        installTranslator(translator);
    }
    _initSettings();
    _initMember();
}

VGApplication::~VGApplication()
{
    delete m_pMainUIInterface;
    m_pMainUIInterface = NULL;
	delete m_setting;
    m_setting = NULL;
}

void VGApplication::initApp()
{
    getQGCMapEngine()->init();
    _initParamsAndEvents();
}

VGMapManager *VGApplication::mapManager() const
{
    return m_pMainUIInterface ? m_pMainUIInterface->mapManager() : NULL;
}

VGNetManager * VGApplication::netManager() const
{
    if (m_pMainUIInterface)
        return m_pMainUIInterface->netManager();

    return NULL;
}

QThread *VGApplication::qmlThread() const
{
    if (m_pMainUIInterface)
        m_pMainUIInterface->getQQmlApplicationEngine().thread();

    return NULL;
}

QQmlApplicationEngine *VGApplication::getQmlEngine() const
{
    if (m_pMainUIInterface)
        m_pMainUIInterface->getQQmlApplicationEngine();

    return NULL;
}

VGPlantManager *VGApplication::plantManager() const
{
    if (m_pMainUIInterface)
        return m_pMainUIInterface->plantManager();

    return NULL;
}

int VGApplication::getNetConnectStat() const
{
    if (m_pMainUIInterface && m_pMainUIInterface->netManager())
        return m_pMainUIInterface->netManager()->GetConnectState();

    return 2;
}

QString VGApplication::GetUserId() const
{
	if (m_pMainUIInterface)
	{
		if (VGNetManager *mgr = m_pMainUIInterface->netManager())
			return mgr->GetUserId();
	}

	return QString();
}

QSettings *VGApplication::GetSettings() const
{
    return m_setting;
}

QString VGApplication::GetDefualtPath()
{
	if (m_defualtPath.isEmpty())
	{
		m_defualtPath = applicationDirPath() + QString::fromLocal8Bit("/" CATCHFOLD);
		if (!QDir::root().exists(m_defualtPath) && !QDir::root().mkdir(m_defualtPath))
        {
#ifdef __mobile__
            m_defualtPath =  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QString::fromLocal8Bit("/" CATCHFOLD);
#else
			m_defualtPath = QDir::homePath() + QString::fromLocal8Bit("/" CATCHFOLD);
#endif
            if (!QDir::root().exists(m_defualtPath))
                QDir::root().mkdir(m_defualtPath);
        }
        if (!m_defualtPath.endsWith("/"))
            m_defualtPath += "/";
	}
	return m_defualtPath;
}

void VGApplication::speakNotice(const QString &value, const QStringList &param)
{
    if (m_audioWorker)
        m_audioWorker->say(value, param);
}

int VGApplication::countVoiceQue() const
{
    if (m_audioWorker)
        return m_audioWorker->countVoiceQue();

    return 0;
}

void VGApplication::SetQmlTip(const QString &tip, bool b)
{
    if (m_pMainUIInterface && m_pMainUIInterface->vgMainPage())
        m_pMainUIInterface->vgMainPage()->SetQmlTip(tip, b);
    if (b)
        speakNotice(tip);
}

void VGApplication::SetMessageBox(const QString & tip, bool bWarn)
{
	if (m_pMainUIInterface && m_pMainUIInterface->vgMainPage())
		m_pMainUIInterface->vgMainPage()->SetMessageBox(tip);
	if (bWarn)
		speakNotice(tip);
}

bool VGApplication::IsPcApp() const
{
    VGMainPage  *mp = m_pMainUIInterface ? m_pMainUIInterface->vgMainPage() : NULL;
    if (mp)
        return mp->IsShowSrSt();

    return false;
}

void VGApplication::initUI()
{
    if(m_pMainUIInterface)
    {
        QFont f = qApp->font();
        m_pMainUIInterface->setUIFontSize(f.pointSize());
        m_pMainUIInterface->createQml();
    }
}

bool VGApplication::notify(QObject *receiver, QEvent *event)
{
#ifdef Q_OS_ANDROID
    if(event->type() == QEvent::Close)
    {
        emit sigBack();
        return false;
    }
    else if(event->type() == QEvent::Quit)
    {
        qDebug() << "quit...............";
        return false;
    }
    else
    {
        return QGuiApplication::notify(receiver, event);
    }
#else
    if(event->type() == QEvent::Close)
    {
        emit sigBack();
        event->accept();
        return true;
    }

    return QGuiApplication::notify(receiver, event);
#endif
}

void VGApplication::_initMember()
{
    m_pMainUIInterface = new VGUIInstance;
    m_audioWorker = new AudioWorker(this);
}

void VGApplication::_initSettings()
{
    QString strConfigPath = GetDefualtPath() + "config.ini";
    QFile file(strConfigPath);
    if (!file.exists())
    {
        file.open(QIODevice::WriteOnly);
        file.close();
        m_setting = new QSettings(strConfigPath, QSettings::IniFormat);

        m_setting->beginGroup("config");
        m_setting->setValue("enableLog", true);
        m_setting->setValue("batteryAlarm", 30.0);
        m_setting->endGroup();

        m_setting->beginGroup("serialPort");
        m_setting->setValue("baud", QVariant("57600"));
        m_setting->endGroup();

        m_setting->beginGroup("udpLinkConfig");
        m_setting->setValue("open", IsPcApp());
        m_setting->setValue("ip", "127.0.0.1");
        m_setting->setValue("localPort", 14550);
        m_setting->endGroup();
    }

    if (!m_setting)
        m_setting = new QSettings(strConfigPath, QSettings::IniFormat);
}

void VGApplication::_initParamsAndEvents()
{
}
