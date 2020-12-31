#include "VGUIInstance.h"
#include <QDebug>
#include <QQmlApplicationEngine>

#include "VGApplication.h"
#include "VGPlantManager.h"
#include "VGMapManager.h"
#include "VGNetManager.h"
#include "VGPlantManager.h"
#include "srcload/VGImageProvider.h"
#include "VGMainPage.h"
#include "QGCMapUrlEngine.h"
#include "QmlObjectListModel.h"

VGUIInstance::VGUIInstance(QObject* parent) : QObject(parent)
, m_vgMainPage(new VGMainPage(this)), m_plantManager(new VGPlantManager(this))
, m_mapManager(new VGMapManager(this)), m_vgNetManager(new VGNetManager)
, m_qmlEngine(new QQmlApplicationEngine(this))
{
    qmlRegisterUncreatableType<UrlFactory>("VGUavAdmin", 1, 0, "UrlFactory", "Reference only");
    qmlRegisterUncreatableType<QmlObjectListModel>("VGUavAdmin", 1, 0, "QmlObjectListModel", "Reference only");
    qmlRegisterUncreatableType<MapAbstractItem>("VGUavAdmin", 1, 0, "MapAbstractItem", "Reference only");
    qmlRegisterUncreatableType<VGPlantInformation>("VGUavAdmin", 1, 0, "VGPlantInformation", "Reference only");
    qmlRegisterUncreatableType<VGMapManager>("VGUavAdmin", 1, 0, "VGMapManager", "Reference only");
    qmlRegisterUncreatableType<VGMainPage>("VGUavAdmin", 1, 0, "VGMainPage", "Reference only");

    qRegisterMetaType<QList<double>>("QList<double>");

    if(m_qmlEngine)
        m_qmlEngine->addImageProvider("rscLoader", new VGImageProvider);
}

VGUIInstance::~VGUIInstance()
{
    m_vgNetManager->deleteLater();
    m_vgNetManager = NULL;
    m_qmlEngine->deleteLater();
    m_qmlEngine = NULL;
    m_vgMainPage->deleteLater();
    m_vgMainPage = NULL;
    m_vgNetManager->deleteLater();
    m_vgNetManager = NULL;

    m_plantManager->deleteLater();
    m_plantManager = NULL;
}

void VGUIInstance::createQml()
{
    connect(m_qmlEngine, &QQmlApplicationEngine::objectCreated, m_mapManager, &VGMapManager::sltQmlCreated);

    m_qmlEngine->rootContext()->setContextProperty("vgMainPage", m_vgMainPage);
    m_qmlEngine->rootContext()->setContextProperty("mapManager", m_mapManager);
    m_qmlEngine->rootContext()->setContextProperty("netManager", m_vgNetManager);
    m_qmlEngine->rootContext()->setContextProperty("plantManager", m_plantManager);

	m_qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    m_plantManager->InitialNetWork(m_vgNetManager);
}

void VGUIInstance::setUIFixedSize(const QSize &size)
{
    if(m_vgMainPage)
    {
        m_vgMainPage->setQmlRootWidth(size.width());
        m_vgMainPage->setQmlRootHeight(size.height());
    }
}

void VGUIInstance::setUIFontSize(int fontPixelSize)
{
    if(m_vgMainPage)
    {
        QFont ft = m_vgMainPage->font();
        ft.setPointSize(fontPixelSize);
        m_vgMainPage->setFont(ft);
    }
}

QQmlApplicationEngine &VGUIInstance::getQQmlApplicationEngine()
{
	return *m_qmlEngine;
}

VGMapManager *VGUIInstance::mapManager() const
{
    return m_mapManager;
}

VGNetManager *VGUIInstance::netManager() const
{
    return m_vgNetManager;
}

VGPlantManager * VGUIInstance::plantManager() const
{
    return m_plantManager;
}

VGMainPage * VGUIInstance::vgMainPage() const
{
    return m_vgMainPage;
}

void VGUIInstance::sltSetUIErrorInfo(const QString &title, const QString &notes)
{
    if(m_vgMainPage)
        m_vgMainPage->setErrorInfo(title, notes);
}

void VGUIInstance::sltUpdateLogList()
{
    emit sigUpdateLogList();
}
