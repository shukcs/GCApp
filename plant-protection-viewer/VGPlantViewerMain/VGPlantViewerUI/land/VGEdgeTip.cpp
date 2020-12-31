#include "VGEdgeTip.h"
#include "VGCoordinate.h"
#include "VGGlobalFun.h"

VGEdgeTip::VGEdgeTip(VGCoordinate *beg, VGCoordinate *end, QObject *parent)
: MapAbstractItem(parent), m_beg(beg), m_end(end)
{
    prcsSignals();
}

VGEdgeTip::VGEdgeTip(const VGEdgeTip &oth) : MapAbstractItem(oth)
, m_beg(oth.m_beg), m_end(oth.m_end)
{
    prcsSignals();
}

VGEdgeTip::~VGEdgeTip()
{
}

QString VGEdgeTip::GetTip() const
{
    if (!IsValid())
        return "N/A";

    double dis = m_beg->DistanceTo(*m_end);
    if (dis > 1000)
        return VGGlobalFunc::getDecString(dis / 1000) + "KM";

    return VGGlobalFunc::getDecString(dis) + "M";
}

const QGeoCoordinate VGEdgeTip::GetCoordinate() const
{
    if (!IsValid())
        return QGeoCoordinate();

    return m_beg->MidOf(*m_end);
}

double VGEdgeTip::GetRotation() const
{
    if (!IsValid())
        return 0;

    auto ret = m_beg->AzimuthTo(*m_end);
    return ret >= 0 ? ret - 90 : ret + 90;
}

bool VGEdgeTip::IsValid() const
{
    return m_beg && m_end;
}

MapAbstractItem::MapItemType VGEdgeTip::ItemType() const
{
    return MapAbstractItem::Type_EdgeTip;
}

bool VGEdgeTip::operator==(const MapAbstractItem &item) const
{
    return false;
}

void VGEdgeTip::prcsSignals()
{
    if (m_beg)
    {
        connect(m_beg, &QObject::destroyed, this, &VGEdgeTip::onTipDestroied);
        connect(m_beg, &VGCoordinate::coordinateChanged, this, &VGEdgeTip::changed);
    }
    if (m_end)
    {
        connect(m_end, &QObject::destroyed, this, &VGEdgeTip::onTipDestroied);
        connect(m_end, &VGCoordinate::coordinateChanged, this, &VGEdgeTip::changed);
    }
}

void VGEdgeTip::onTipDestroied()
{
    auto sdner = sender();
    if (sdner != m_beg && sdner != m_end)
        return;

    if (sdner == m_beg)
        m_beg = NULL;
    else
        m_end = NULL;

    emit changed();
}
