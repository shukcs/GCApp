﻿/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Map Tile Cache
 *
 *   @author Gus Grubba <mavlink@grubba.com>
 *
 */

#include <math.h>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <stdio.h>
#include <QtMath>

#include "QGCMapEngine.h"
#include "QGCMapTileSet.h"
#include "VGMapGlobalFunction.h"
#include "base.h"

Q_DECLARE_METATYPE(QGCMapTask::TaskType)
Q_DECLARE_METATYPE(QGCTile)
Q_DECLARE_METATYPE(QList<QGCTile*>)

static const char* kDbFileName = "vgMapCache.db";
static QLocale kLocale;

#define CACHE_PATH_VERSION  "300"

struct stQGeoTileCacheQGCMapTypes {
    const char* name;
    UrlFactory::MapType type;
};

//-- IMPORTANT
//   Changes here must reflect those in QGeoTiledMappingManagerEngineQGC.cpp

stQGeoTileCacheQGCMapTypes kMapTypes[] = {
    {"Google Street Map",       UrlFactory::GoogleMap},
    {"Google Satellite Map",    UrlFactory::GoogleSatellite},
    {"Google Terrain Map",      UrlFactory::GoogleTerrain},
    {"Bing Street Map",         UrlFactory::BingMap},
    {"Bing Satellite Map",      UrlFactory::BingSatellite},
    {"Bing Hybrid Map",         UrlFactory::BingHybrid},
    {"Baidu Street Map",         UrlFactory::BaiduMap},
    {"Baidu Satellite Map",      UrlFactory::BaiduSatellite},
    {"Baidu Hybrid Map",         UrlFactory::BaiduHybrid},
    {"AMap Street Map",         UrlFactory::AMap},
    {"AMap Satellite Map",      UrlFactory::AMapSatelliteMap},
    {"AMap Hybrid Ma",          UrlFactory::AMapHybridMap},
    {"MapQuest Street Map",     UrlFactory::MapQuestMap},
    {"MapQuest Satellite Map",  UrlFactory::MapQuestSat}
    /*
    {"Open Street Map",         UrlFactory::OpenStreetMap}
     */
};

#define NUM_MAPS (sizeof(kMapTypes) / sizeof(stQGeoTileCacheQGCMapTypes))

stQGeoTileCacheQGCMapTypes kMapBoxTypes[] = {
    {"MapBox Street Map",       UrlFactory::MapBoxStreets},
    {"MapBox Satellite Map",    UrlFactory::MapBoxSatellite},
    {"MapBox High Contrast Map",UrlFactory::MapBoxHighContrast},
    {"MapBox Light Map",        UrlFactory::MapBoxLight},
    {"MapBox Dark Map",         UrlFactory::MapBoxDark},
    {"MapBox Hybrid Map",       UrlFactory::MapBoxHybrid},
    {"MapBox Wheat Paste Map",  UrlFactory::MapBoxWheatPaste},
    {"MapBox Streets Basic Map",UrlFactory::MapBoxStreetsBasic},
    {"MapBox Comic Map",        UrlFactory::MapBoxComic},
    {"MapBox Outdoors Map",     UrlFactory::MapBoxOutdoors},
    {"MapBox Run, Byke and Hike Map",   UrlFactory::MapBoxRunBikeHike},
    {"MapBox Pencil Map",       UrlFactory::MapBoxPencil},
    {"MapBox Pirates Map",      UrlFactory::MapBoxPirates},
    {"MapBox Emerald Map",      UrlFactory::MapBoxEmerald}
};

#define NUM_MAPBOXMAPS (sizeof(kMapBoxTypes) / sizeof(stQGeoTileCacheQGCMapTypes))

static const char* kMapBoxTokenKey  = "MapBoxToken";
static const char* kMaxDiskCacheKey = "MaxDiskCache";
static const char* kMaxMemCacheKey  = "MaxMemoryCache";

//-----------------------------------------------------------------------------
// Singleton
static QGCMapEngine *kMapEngine = NULL;
QGCMapEngine *getQGCMapEngine()
{
    if(!kMapEngine)
        kMapEngine = new QGCMapEngine();
    return kMapEngine;
}

//-----------------------------------------------------------------------------
void destroyMapEngine()
{
    if(kMapEngine) {
        delete kMapEngine;
        kMapEngine = NULL;
    }
}

//-----------------------------------------------------------------------------
QGCMapEngine::QGCMapEngine()
    : _urlFactory(new UrlFactory())
#ifdef WE_ARE_KOSHER
    //-- TODO: Get proper version
    #if defined Q_OS_MAC
        , _userAgent("QGroundControl (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.75.14 (KHTML, like Gecko) Version/2.9.0")
    #elif defined Q_OS_WIN32
        , _userAgent("QGroundControl (Windows; Windows NT 6.0) (KHTML, like Gecko) Version/2.9.0")
    #else
        , _userAgent("QGroundControl (X11; Ubuntu; Linux x86_64) (KHTML, like Gecko) Version/2.9.0")
    #endif
#else
    #if defined Q_OS_MAC
        , _userAgent("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:25.0) Gecko/20100101 Firefox/25.0")
    #elif defined Q_OS_WIN32
        , _userAgent("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20130401 Firefox/31.0")
    #else
        , _userAgent("Mozilla/5.0 (X11; Linux i586; rv:31.0) Gecko/20100101 Firefox/31.0")
    #endif
#endif
    , _maxDiskCache(0)
    , _maxMemCache(0)
    , _prunning(false)
    , _cacheWasReset(false)
    , _isInternetActive(false)
{
    qRegisterMetaType<QGCMapTask::TaskType>();
    qRegisterMetaType<QGCTile>();
    qRegisterMetaType<QList<QGCTile*>>();
    connect(&_worker, &QGCCacheWorker::updateTotals,   this, &QGCMapEngine::_updateTotals);
    connect(&_worker, &QGCCacheWorker::internetStatus, this, &QGCMapEngine::_internetStatus);
	connect(&_worker, SIGNAL(sigImportMapFinished(int)), this, SIGNAL(sigImportMapFinished(int)));
}

//-----------------------------------------------------------------------------
QGCMapEngine::~QGCMapEngine()
{
    _worker.quit();
    _worker.wait();
    if(_urlFactory)
        delete _urlFactory;
}

void QGCMapEngine::_checkWipeDirectory(const QString& dirPath)
{
    QDir dir(dirPath);
    if (dir.exists(dirPath)) {
        _cacheWasReset = true;
        _wipeDirectory(dirPath);
    }
}

void QGCMapEngine::_wipeOldCaches()
{
    QString oldCacheDir;
#ifdef __mobile__
    oldCacheDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)      + QLatin1String("/QGCMapCache100");
#else
    oldCacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QLatin1String("/QGCMapCache100");
#endif
    _checkWipeDirectory(oldCacheDir);
}
//-----------------------------------------------------------------------------
void QGCMapEngine::init()
{
    //-- Delete old style caches (if present)
    _wipeOldCaches();
    QString cacheDir = Application::Instance()->GetDefualtPath() + QLatin1String("QGCMapCache" CACHE_PATH_VERSION);
    if(!QDir::root().mkpath(cacheDir))
    {
        qWarning() << "Could not create mapping disk cache directory: " << cacheDir;
        cacheDir = QDir::homePath() + QLatin1String("/.qgcmapscache/");
        if(!QDir::root().mkpath(cacheDir))
            cacheDir.clear();
    }
    _cachePath = cacheDir;
    if(!_cachePath.isEmpty())
    {
        _cacheFile = kDbFileName;
        _worker.setDatabaseFile(_cachePath + "/" + _cacheFile);
    }
    QGCMapTask* task = new QGCMapTask(QGCMapTask::taskInit);
    _worker.enqueueTask(task);
}

//-----------------------------------------------------------------------------
bool QGCMapEngine::_wipeDirectory(const QString& dirPath)
{
    bool result = true;
    QDir dir(dirPath);
    if (dir.exists(dirPath)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = _wipeDirectory(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }
            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirPath);
    }
    return result;
}

//-----------------------------------------------------------------------------
void QGCMapEngine::addTask(QGCMapTask* task)
{
    _worker.enqueueTask(task);
}

//-----------------------------------------------------------------------------
void QGCMapEngine::cacheTile(UrlFactory::MapType type, int x, int y, int z, const QByteArray& image, const QString &format, qulonglong set)
{
    QString hash = getTileHash(type, x, y, z);
    cacheTile(type, hash, image, format, set);
}

//-----------------------------------------------------------------------------
void QGCMapEngine::cacheTile(UrlFactory::MapType type, const QString& hash, const QByteArray& image, const QString& format, qulonglong set)
{
    QGCSaveTileTask* task = new QGCSaveTileTask(new QGCCacheTile(hash, image, format, type, set));
    _worker.enqueueTask(task);
}

//-----------------------------------------------------------------------------
QString QGCMapEngine::getTileHash(UrlFactory::MapType type, int x, int y, int z)
{
    return QString().sprintf("%04d%08d%08d%03d", (int)type, x, y, z);
}

//-----------------------------------------------------------------------------
UrlFactory::MapType QGCMapEngine::hashToType(const QString &hash)
{
    QString type = hash.mid(0,4);
    return (UrlFactory::MapType)type.toInt();
}

//-----------------------------------------------------------------------------
QGCFetchTileTask *QGCMapEngine::createFetchTileTask(UrlFactory::MapType type, int x, int y, int z)
{
    QString hash = getTileHash(type, x, y, z);
    QGCFetchTileTask* task = new QGCFetchTileTask(hash);
    return task;
}

//-----------------------------------------------------------------------------
QGCTileSet QGCMapEngine::getTileCount(int zoom, double topleftLon, double topleftLat, double bottomRightLon, double bottomRightLat, UrlFactory::MapType mapType)
{    //增加对百度地图的支持
    QGCTileSet set;

    if(zoom <  1) zoom = 1;
    if(zoom > MAX_MAP_ZOOM) zoom = MAX_MAP_ZOOM;
    if(mapType == 100 || mapType == 101 || mapType == 102)
    {
        QPoint ptTopLeft = latLon2TileXYBd(topleftLon, topleftLat, zoom);
        set.tileX0 = ptTopLeft.x();
        set.tileY0 = ptTopLeft.y();
        QPoint ptBottomRight = latLon2TileXYBd(bottomRightLon, bottomRightLat, zoom);
        set.tileX1 = ptBottomRight.x();
        set.tileY1 = ptBottomRight.y();
    }
    else
    {
        set.tileX0 = long2tileX(topleftLon,     zoom);
        set.tileY0 = lat2tileY(topleftLat,      zoom);
        set.tileX1 = long2tileX(bottomRightLon, zoom);
        set.tileY1 = lat2tileY(bottomRightLat,  zoom);
    }

    quint64 nx = qAbs(set.tileX1 - set.tileX0) + 1;
    quint64 ny = qAbs(set.tileY1 - set.tileY0) + 1;
    set.tileCount = nx * ny;
    set.tileSize  = UrlFactory::averageSizeForType(mapType) * set.tileCount;
    return set;
}

//-----------------------------------------------------------------------------
int QGCMapEngine::long2tileX(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

//-----------------------------------------------------------------------------
int QGCMapEngine::lat2tileY(double lat, int z)
{
    return (int)(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}


//-----------------------------------------------------------------------------
UrlFactory::MapType QGCMapEngine::getTypeFromName(const QString& name)
{
    size_t i;
    for(i = 0; i < NUM_MAPS; i++) {
        if(name.compare(kMapTypes[i].name, Qt::CaseInsensitive) == 0)
            return kMapTypes[i].type;
    }
    for(i = 0; i < NUM_MAPBOXMAPS; i++) {
        if(name.compare(kMapBoxTypes[i].name, Qt::CaseInsensitive) == 0)
            return kMapBoxTypes[i].type;
    }
    return UrlFactory::Invalid;
}

QString QGCMapEngine::getMapTypeName(UrlFactory::MapType type)
{
    size_t i;
    for(i = 0; i < NUM_MAPS; i++) {
        if(type == kMapTypes[i].type)
            return kMapTypes[i].name;
    }
    for(i = 0; i < NUM_MAPBOXMAPS; i++) {
        if(type == kMapBoxTypes[i].type)
            return kMapBoxTypes[i].name;
    }
    return "";
}

//-----------------------------------------------------------------------------
QStringList QGCMapEngine::getMapNameList()
{
    QStringList mapList;
    for(size_t i = 0; i < NUM_MAPS; i++) {
        mapList << kMapTypes[i].name;
    }
    if(!getMapBoxToken().isEmpty()) {
        for(size_t i = 0; i < NUM_MAPBOXMAPS; i++) {
            mapList << kMapBoxTypes[i].name;
        }
    }
    return mapList;
}

//-----------------------------------------------------------------------------
void QGCMapEngine::setMapBoxToken(const QString& token)
{
    QSettings settings;
    settings.setValue(kMapBoxTokenKey, token);
    _mapBoxToken = token;
}

//-----------------------------------------------------------------------------
QString QGCMapEngine::getMapBoxToken()
{
    if(_mapBoxToken.isEmpty()) {
        QSettings settings;
        _mapBoxToken = settings.value(kMapBoxTokenKey).toString();
    }
    return _mapBoxToken;
}

//-----------------------------------------------------------------------------
quint32 QGCMapEngine::getMaxDiskCache()
{
    if(!_maxDiskCache) {
        QSettings settings;
        _maxDiskCache = settings.value(kMaxDiskCacheKey, 1024).toUInt();
    }
    return _maxDiskCache;
}

//-----------------------------------------------------------------------------
void QGCMapEngine::setMaxDiskCache(quint32 size)
{
    QSettings settings;
    settings.setValue(kMaxDiskCacheKey, size);
    _maxDiskCache = size;
}

//-----------------------------------------------------------------------------
quint32 QGCMapEngine::getMaxMemCache()
{
    if(!_maxMemCache) {
        QSettings settings;
#ifdef __mobile__
        _maxMemCache = settings.value(kMaxMemCacheKey, 32).toUInt();
#else
        _maxMemCache = settings.value(kMaxMemCacheKey, 256).toUInt();
#endif
    }
    //-- Size in MB
    if(_maxMemCache > 1024)
        _maxMemCache = 1024;
    return _maxMemCache;
}

//-----------------------------------------------------------------------------
void QGCMapEngine::setMaxMemCache(quint32 size)
{
    //-- Size in MB
    if(size > 1024)
        size = 1024;
    QSettings settings;
    settings.setValue(kMaxMemCacheKey, size);
    _maxMemCache = size;
}

//-----------------------------------------------------------------------------
QString QGCMapEngine::bigSizeToString(quint64 size)
{
    if(size < 1024)
        return kLocale.toString(size);
    else if(size < 1024 * 1024)
        return kLocale.toString((double)size / 1024.0, 'f', 1) + "kB";
    else if(size < 1024 * 1024 * 1024)
        return kLocale.toString((double)size / (1024.0 * 1024.0), 'f', 1) + "MB";
    else if(size < 1024.0 * 1024.0 * 1024.0 * 1024.0)
        return kLocale.toString((double)size / (1024.0 * 1024.0 * 1024.0), 'f', 1) + "GB";
    else
        return kLocale.toString((double)size / (1024.0 * 1024.0 * 1024.0 * 1024), 'f', 1) + "TB";
}

//-----------------------------------------------------------------------------
QString QGCMapEngine::numberToString(quint64 number)
{
    return kLocale.toString(number);
}

//-----------------------------------------------------------------------------
void QGCMapEngine::_updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize)
{
    emit updateTotals(totaltiles, totalsize, defaulttiles, defaultsize);
    quint64 maxSize = (quint64)getMaxDiskCache() * 1024L * 1024L;
    if(!_prunning && defaultsize > maxSize) {
        //-- Prune Disk Cache
        _prunning = true;
        QGCPruneCacheTask* task = new QGCPruneCacheTask(defaultsize - maxSize);
        connect(task, &QGCPruneCacheTask::pruned, this, &QGCMapEngine::_pruned);
        getQGCMapEngine()->addTask(task);
    }
}
//-----------------------------------------------------------------------------
void QGCMapEngine::_pruned()
{
    _prunning = false;
}

//-----------------------------------------------------------------------------
int QGCMapEngine::concurrentDownloads(UrlFactory::MapType type)
{
    switch(type) {
    case UrlFactory::GoogleMap:
    case UrlFactory::GoogleSatellite:
    case UrlFactory::GoogleTerrain:
    case UrlFactory::BingMap:
    case UrlFactory::BingSatellite:
    case UrlFactory::BingHybrid:
        return 12;
    case UrlFactory::MapQuestMap:
    case UrlFactory::MapQuestSat:
        return 8;
    default:
        break;
    }
    return 6;
}

//-----------------------------------------------------------------------------
QGCCreateTileSetTask::~QGCCreateTileSetTask()
{
    //-- If not sent out, delete it
    if(!_saved && _tileSet)
        delete _tileSet;
}

//-----------------------------------------------------------------------------
void QGCMapEngine::testInternet()
{
    getQGCMapEngine()->addTask(new QGCTestInternetTask());
}

//-----------------------------------------------------------------------------
void QGCMapEngine::_internetStatus(bool active)
{
    _isInternetActive = active;
}

// Resolution math: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Resolution_and_Scale

