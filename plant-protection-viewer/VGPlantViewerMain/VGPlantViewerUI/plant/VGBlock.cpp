#include "VGBlock.h"

VGBlock::VGBlock(const QGeoCoordinate &coor, double dis, double angle, QObject *parent):
    MapAbstractItem(parent),m_coor(coor),m_dis(dis),m_angle(angle)
{
}

VGBlock::VGBlock(VGBlock &block):MapAbstractItem(block),
    m_coor(block.m_coor),m_dis(block.m_dis),m_angle(block.angle())
{
}

VGBlock::~VGBlock()
{
}

double VGBlock::dis() const
{
    return m_dis;
}

const QGeoCoordinate &VGBlock::coordinate() const
{
    return m_coor;
}

void VGBlock::setDis(double dis)
{
    m_dis = dis;
    emit disChanged();
}

void VGBlock::setCoordinate(const QGeoCoordinate &coor)
{
    m_coor = coor;
    emit coorChanged();
}

double VGBlock::angle() const
{
    return m_angle;
}

void VGBlock::setAngle(double angle)
{
    m_angle = angle;
    emit angleChanged();
}

MapAbstractItem::MapItemType VGBlock::ItemType() const
{
    return Type_Block;
}

bool VGBlock::operator==(const MapAbstractItem &item) const
{
    if (item.ItemType() != ItemType())
        return false;

    const VGBlock &ol = *static_cast<const VGBlock*>(&item);
    if (ol.coordinate() == m_coor && ol.dis() == m_dis && ol.angle() == m_angle)
        return true;
    return false;
}

