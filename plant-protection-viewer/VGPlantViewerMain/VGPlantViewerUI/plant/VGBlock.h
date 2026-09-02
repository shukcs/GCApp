#ifndef __VG_BLOCK_H__
#define __VG_BLOCK_H__

#include "MapAbstractItem.h"
#include <QGeoCoordinate>
class VGBlock : public MapAbstractItem
{
    Q_OBJECT
    Q_PROPERTY(double dis READ dis NOTIFY disChanged)
    Q_PROPERTY(double angle READ angle NOTIFY angleChanged)
    Q_PROPERTY(QGeoCoordinate coor READ coordinate NOTIFY coorChanged)
public:
    explicit VGBlock(const QGeoCoordinate &coor , double dis, double angle,QObject *parent = 0);
    VGBlock(VGBlock &block);
    ~VGBlock();

    double dis()const;
    const QGeoCoordinate &coordinate()const;
    void setDis(double dis);
    void setCoordinate(const QGeoCoordinate &coor);
    double angle()const;
    void setAngle(double angle);

    MapAbstractItem::MapItemType ItemType() const;
    bool operator==(const MapAbstractItem &item)const;
signals:
    void disChanged();
    void coorChanged();
    void angleChanged();
    void indexChanged();
private:
    double m_dis;
    QGeoCoordinate m_coor;
    double m_angle;
};
#endif // VGBLOCK_H
