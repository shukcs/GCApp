#include "VGMapManager.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QSettings>
#include <qmath.h>

#include "VGGlobalFun.h"
#include "VGApplication.h"
#include "VGCoordinate.h"
#include "VGOutline.h"
#include "QmlObjectListModel.h"
#include "QGCMapEngine.h"

const int s_scaleLengths[] = { 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000 };
///////////////////////////////////////////////////////////////////////////////////
//VGMapManager
///////////////////////////////////////////////////////////////////////////////////
VGMapManager::VGMapManager(QObject *parent) : QObject(parent), m_bShowSR(true)
, m_mapCenter(40.12121, 119.12121), m_mapTypeID(UrlFactory::AMapSatelliteMap)
, m_zoomLevel(14), m_bMapChanged(false), m_pixLength(0), m_propLength(80)
, m_mgrObj(0), m_bOnlyStreet(false)
{
	_readConfig();
    QGeoCoordinate coor = VGGlobalFunc::toGps(m_mapCenter);
    m_centerLat = coor.latitude();
    m_centerLon = coor.longitude();
}

VGMapManager::~VGMapManager()
{
}

void VGMapManager::sltQmlCreated(QObject *, const QUrl &)
{
	QString strMapTypeName = getQGCMapEngine()->getMapTypeName((UrlFactory::MapType)m_mapTypeID);
}

void VGMapManager::init()
{
    emit mapTypeIDChanged(m_mapTypeID);
    qDebug() << m_mapTypeID;
    emit finished();
}

bool VGMapManager::isShowScaleRule() const
{
	return m_bShowSR;
}

void VGMapManager::setShowScaleRule(bool b)
{
	if (m_bShowSR!=b)
	{
		m_bShowSR = b;
		emit showScaleRuleChanged(b);
	}
}

void VGMapManager::sltSetMapZoomLevel(int level)
{
    if(level <= 0 || level > 19)
        return;

    setZoomLevel(level);
}

int VGMapManager::mapTypeID() const
{
	return m_mapTypeID;
}

void VGMapManager::setMapTypeID(int type)
{
	if (m_mapTypeID == type)
		return;

    m_mapTypeID = type;
	m_bMapChanged = true;
    getQGCMapEngine()->getMapTypeName((UrlFactory::MapType)type);
    emit mapTypeIDChanged(type);
}

QString VGMapManager::GetMapTypeName() const
{
	return getQGCMapEngine()->getMapTypeName((UrlFactory::MapType)m_mapTypeID);
}

QString VGMapManager::GetStreetMapName() const
{
    if (m_mapTypeID > 0 && m_mapTypeID <= UrlFactory::GoogleHybrid)
        return getQGCMapEngine()->getMapTypeName(UrlFactory::GoogleMap);
    if (m_mapTypeID >= UrlFactory::AMap && m_mapTypeID <= UrlFactory::AMapHybridMap)
        return getQGCMapEngine()->getMapTypeName(UrlFactory::AMap);

    return getQGCMapEngine()->getMapTypeName(UrlFactory::AMap);
}


QGeoCoordinate VGMapManager::mapCenter() const
{
	return m_mapCenter;
}

bool VGMapManager::IsMissionPage() const
{
    return m_mgrObj & Mission;
}

void VGMapManager::setMapCenter(const QGeoCoordinate &coordinate)
{
    if (coordinate != m_mapCenter)
    {
        m_mapCenter = coordinate;
		m_bMapChanged = true;
		emit mapCenterChanged(coordinate);
        QGeoCoordinate coor = VGGlobalFunc::toGps(coordinate);
        m_centerLat = coor.latitude();
        emit centerLatChanged(m_centerLat);
        m_centerLon = coor.longitude();
        emit centerLonChanged(m_centerLat);
    }
}

int VGMapManager::zoomLevel() const
{
    return m_zoomLevel;
}

void VGMapManager::setZoomLevel(int level)
{
    if (m_zoomLevel!=level)
    {
        m_zoomLevel = level;
		m_bMapChanged = true;
		emit zoomLevelChanged(level);
    }
}

double VGMapManager::centerLat() const
{
    return m_centerLat;
}

void VGMapManager::setCenterLat(double lat)
{
    if (m_centerLat == lat)
        return;

    m_centerLat = lat;
    emit centerLatChanged(lat);
    m_mapCenter = VGGlobalFunc::gpsCorrect(QGeoCoordinate(m_centerLat, m_centerLon));
    emit mapCenterChanged(m_mapCenter);
}

double VGMapManager::centerLon() const
{
    return m_centerLon;
}

void VGMapManager::setCenterLon(double lon)
{
    if (m_centerLon == lon)
        return;

    m_centerLon = lon;
    emit centerLonChanged(lon);
    m_mapCenter = VGGlobalFunc::gpsCorrect(QGeoCoordinate(m_centerLat, m_centerLon));
    emit mapCenterChanged(m_mapCenter);
}

int VGMapManager::GetPropLength() const
{
    return m_propLength;
}

QString VGMapManager::_transString(int len)
{
    int prpL = len<s_scaleLengths[0] ? 0: 5000000;
    for (int i = 1; i < sizeof(s_scaleLengths) / sizeof(int); ++i)
    {
        if (len >= s_scaleLengths[i-1] && len < s_scaleLengths[i])
        {
            prpL = s_scaleLengths[i];
            break;
        }
    }
    int nL = prpL == 0 ? 100 : prpL / m_pixLength;
    if (m_propLength != nL)
    {
        m_propLength = nL;
        emit propLengthChanged(nL);
    }
    if (prpL == 0)
    {
        if (len < s_scaleLengths[0])
            return QString("<5m");
        else if (len > 2000000)
            return QString(">2000Km");
    }

    if (prpL >= 1000)
        return QString("%1 Km").arg(prpL / 1000);

    return QString("%1 m").arg(prpL);
}

bool VGMapManager::IsOnlyStreet() const
{
    return m_bOnlyStreet;
}

void VGMapManager::SetOnlyStreet(bool b)
{
    if (m_bOnlyStreet == b)
        return;

    m_bOnlyStreet = b;
    emit onlyStreetChanged();
}

int VGMapManager::GetManagerObj() const
{
    return m_mgrObj;
}

void VGMapManager::SetManagerObj(int n)
{
    if (m_mgrObj == n)
        return;
    m_mgrObj = n;
    emit mgrObjChanged();
}

void VGMapManager::sltSetMapCenter(double lat, double lon)
{
    QGeoCoordinate mapCenter(lat, lon);
    qDebug() << "new Map Center" << lat << lon;
    setMapCenter(mapCenter);
}

QString VGMapManager::propertyMapName()
{
    if (m_mgrObj==0 || m_bOnlyStreet)
        return GetStreetMapName();
    else
        return GetMapTypeName();
}

void VGMapManager::SetItemSelected(MapAbstractItem *item, bool b)
{
    if (!item)
        return;

    MapAbstractItem *itemLast = m_mapSelectedItem.value(item->ItemType(), NULL);
    if (b)
    {
        if (itemLast && item != itemLast)
            itemLast->SetSelected(false);
        m_mapSelectedItem[item->ItemType()] = item;
    }
    else if (item == itemLast)
    {
        m_mapSelectedItem[item->ItemType()] = NULL;
    }
}

MapAbstractItem *VGMapManager::GetSelecedItem(MapAbstractItem::MapItemType tp)
{
    QMap<MapAbstractItem::MapItemType, MapAbstractItem*>::const_iterator itr = m_mapSelectedItem.find(tp);
    if (itr != m_mapSelectedItem.end())
        return itr.value();

    return NULL;
}

bool VGMapManager::IsItemSelecte(const MapAbstractItem *item)const
{
    if (!item)
        return false;

    QMap<MapAbstractItem::MapItemType, MapAbstractItem*>::const_iterator itr = m_mapSelectedItem.find(item->ItemType());
    if (itr != m_mapSelectedItem.end() && itr.value() == item)
        return true;

    return false;
}

double VGMapManager::GetPixLength() const
{
    return m_pixLength;
}

void VGMapManager::_readConfig()
{
	if (QSettings *settings = qvgApp->GetSettings())
	{
		settings->beginGroup("map");
        int tp = settings->value("type", m_mapTypeID).toInt();
        setMapTypeID(tp);
        m_bOnlyStreet = settings->value("onlyStreet", 0).toInt() != 0;
        m_zoomLevel = settings->value("zoomLevel", 14).toInt();
		QGeoCoordinate coor = VGCoordinate::coordinateFromString(settings->value("center").toString());
		if (coor.isValid())
			setMapCenter(coor);
		m_bMapChanged = false;
		settings->endGroup();
	}
}

void VGMapManager::_writeConfig()
{
	if (QSettings *settings = qvgApp->GetSettings())
	{
		settings->beginGroup("map");
		settings->setValue("type", m_mapTypeID);
		settings->setValue("onlyStreet", m_bOnlyStreet?1:0);
		settings->setValue("zoomLevel", m_zoomLevel);
		settings->setValue("center", VGCoordinate::coordinate2String(m_mapCenter));
		settings->endGroup();
	}
}

void VGMapManager::_checkSelected(QObject *item)
{
    QMap<MapAbstractItem::MapItemType, MapAbstractItem*>::iterator itr = m_mapSelectedItem.begin();
    for (; itr != m_mapSelectedItem.end(); ++itr)
    {
        if (itr.value() == item)
        {
            m_mapSelectedItem.erase(itr);
            break;
        }
    }
}

void VGMapManager::SetMissionBoundary(const QList<double> &bdy)
{
    double east = bdy.at(0);
    double west = bdy.at(1);
    double south = bdy.at(2);
    double north = bdy.at(3);
    emit boundaryMissionChanged(east, west, south, north);
}

QmlObjectListModel *VGMapManager::getSpecItems(int tp)
{
    QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::const_iterator itr = m_mapItems.find((MapAbstractItem::MapItemType)tp);
    if (itr != m_mapItems.end())
        return itr.value();

    QmlObjectListModel *model = new QmlObjectListModel(this);
    m_mapItems[(MapAbstractItem::MapItemType)tp] = model;
    return model;
}

QString VGMapManager::calculatePixLength(const QGeoCoordinate &c1, const QGeoCoordinate &c2, int pix)
{
    if (pix <= 0)
        return QString();

    double pixL = c1.distanceTo(c2) / pix;
    if (pixL != m_pixLength)
        m_pixLength = pixL;

    return _transString(pixL*pix);
}

void VGMapManager::saveConfig()
{
	if (m_bMapChanged)
	{
		_writeConfig();
		m_bMapChanged = false;
	}
}

bool VGMapManager::InsertItem(MapAbstractItem *item, int index)
{
    if (!item)
        return false;
    
    if (QmlObjectListModel *model = getSpecItems(item->ItemType()))
    {
        if (model->indexOf(item) >= 0)
            return false;

        model->insert(index, item);
		connect(item, &MapAbstractItem::destroyed, this, &VGMapManager::_checkSelected, Qt::UniqueConnection);
        item->SetVisible(true);
        return true;
    }

    return false;
}

bool VGMapManager::InsertItems(MapAbstractItem::MapItemType tp, const QList<MapAbstractItem *> &items, int index)
{
	if (QmlObjectListModel *model = getSpecItems(tp))
	{
        QObjectList lsObj;
		for (MapAbstractItem *itr : items)
		{
			connect(itr, &QObject::destroyed, this, &VGMapManager::_checkSelected, Qt::UniqueConnection);
            if  (!itr || itr->ItemType() != tp || model->contains(itr))
                continue;
            itr->SetVisible(true);
            lsObj << itr;
		}
		model->insertList(lsObj, index);
		return true;
	}
	return false;
}

bool VGMapManager::RemoveItem(MapAbstractItem *item)
{
    if (!item)
        return false;

    QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::iterator itr = m_mapItems.find(item->ItemType());
    if (itr == m_mapItems.end())
        return false;

    item->SetVisible(false);
    return itr.value()->removeOne(item) != NULL;
}

void VGMapManager::RemoveItems(MapAbstractItem::MapItemType tp, const QList<MapAbstractItem *> &items)
{
	if (items.size() == 0)
		return ;

	QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::iterator itr = m_mapItems.find(tp);
	if (itr == m_mapItems.end())
		return ;

	QObjectList objects;
	for (MapAbstractItem *item : items)
    {
        item->SetVisible(false);
        if (!item || item->ItemType()!= tp || !itr.value()->contains(item))
            continue;

		objects << item;
	}
	if (objects.size() > 0)
		itr.value()->removeItmes(objects);
}

MapAbstractItem *VGMapManager::SpecItemAt(MapAbstractItem::MapItemType tp, int index)const
{
    QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::const_iterator itr = m_mapItems.find(tp);
    if (itr == m_mapItems.end())
        return NULL;

    QmlObjectListModel *model = itr.value();
    if (index >= model->count() || index < 0)
        return NULL;

    return (MapAbstractItem *)model->get(index);
}

void VGMapManager::ClearSpecItem(MapAbstractItem::MapItemType tp)
{
    QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::iterator itr = m_mapItems.find(tp);
    if (itr == m_mapItems.end())
        return;

    QmlObjectListModel *model = itr.value();
    while (model->count())
    {
        if (MapAbstractItem *item = (MapAbstractItem *)model->removeAt(0))
            item->SetVisible(false);
    }
}

int VGMapManager::CountSpecItem(MapAbstractItem::MapItemType tp) const
{
    QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::const_iterator itr = m_mapItems.find(tp);
    if (itr == m_mapItems.end())
        return 0;

    return itr.value()->count();
}


bool VGMapManager::HasItem(MapAbstractItem *item) const
{
    if (!item)
        return false;

    QMap<MapAbstractItem::MapItemType, QmlObjectListModel*>::const_iterator itr = m_mapItems.find(item->ItemType());
    if (itr == m_mapItems.end())
        return false;

    return itr.value()->hasItem(*item);
}

void VGMapManager::CalcBoundaryByCoor(QList<double> &ls, const QGeoCoordinate &coordinate)
{
    if (ls.size() == 0)
    {
        ls.append(coordinate.longitude());
        ls.append(coordinate.longitude());
        ls.append(coordinate.latitude());
        ls.append(coordinate.latitude());
    }
    else
    {
        ls.replace(0, qMax(ls.at(0), coordinate.longitude()));
        ls.replace(1, qMin(ls.at(1), coordinate.longitude()));
        ls.replace(2, qMin(ls.at(2), coordinate.latitude()));
        ls.replace(3, qMax(ls.at(3), coordinate.latitude()));
    }
}

void VGMapManager::CalcBoundaryByCoor(QList<double> &ls, const VGOutline &ol)
{
    foreach (VGCoordinate *itr, ol.GetCoordinates())
    {
        CalcBoundaryByCoor(ls, itr->GetCoordinate());
    }
}
