#include "VGApplication.h"
#include <QDir>
#include <QSharedMemory>
#include "QLog.h"
#include "VGMacro.h"

int main(int argc, char *argv[])
{
	Q_IMPORT_PLUGIN(VGGeoServiceProviderFactory)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#ifndef __mobile__
    QSharedMemory shm(VG_ORG_NAME);
    if (shm.attach())
    {
        qWarning() << "Another instance already running. Exiting.";
        exit(-1);
    }
    shm.create(1);
#endif
    VGApplication app(argc, argv);
    //检测是否记录系统运行日志
    bool enableLog = false;
    if (QSettings *settings = app.GetSettings())
    {
        settings->beginGroup("config");
        enableLog = settings->value("enableLog", true).toBool();
        settings->endGroup();
    }
    if (enableLog)
    {
        QString qStLogDir;
        qStLogDir = app.GetDefualtPath() + QString("vgPlantLog/");
        QDir log_dir(qStLogDir);
        if (!log_dir.exists(qStLogDir))
            log_dir.mkpath(qStLogDir);

        InitSystemLog(qStLogDir);
    }

    app.initApp();
    app.initUI();

    return app.exec();
}
