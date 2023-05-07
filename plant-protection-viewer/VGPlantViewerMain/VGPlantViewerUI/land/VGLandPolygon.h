﻿#ifndef VGLANDPOLYGON_H
#define VGLANDPOLYGON_H

#include <QVariant>
#include "MapAbstractItem.h"
#include <QGeoCoordinate>


//我的地块中轮廓对象类（主要用来做为model中对象）
//class QGeoCoordinate;
class VGCoordinate;

class VGLandDis : public MapAbstractItem
{
    Q_OBJECT

    Q_PROPERTY(double dis READ dis NOTIFY disChanged)
    Q_PROPERTY(QGeoCoordinate coor READ coordinate NOTIFY coorChanged)
public:
    explicit VGLandDis(const QGeoCoordinate &coor , double dis, QObject *parent = 0);
    VGLandDis(VGLandDis &landDis);
    ~VGLandDis();

    double dis()const;
    const QGeoCoordinate &coordinate()const;
    void setDis(double dis);
    void setCoordinate(const QGeoCoordinate &coor);

    MapAbstractItem::MapItemType ItemType() const;
    bool operator==(const MapAbstractItem &item)const;
signals:
    void disChanged();
    void coorChanged();
private:
    double m_dis;
    QGeoCoordinate m_coor;
};

class VGLandPolygon : public MapAbstractItem
{
    Q_OBJECT

    //Quick中path对象，经纬度点
    Q_PROPERTY(QVariantList path READ GetPath NOTIFY pathChanged)
    Q_PROPERTY(int countCoor READ CountCoordinate NOTIFY countCoorChanged)
public:
    enum Type{
        NoPoint,
        Outline,
        Block,
        Boundary,
        BlockBoundary,
        SafeBoundary,
        FlyRoute,
        RoutePlanning,
        FreePlan,

        Unknow,
    };
public:
    explicit VGLandPolygon(QObject *parent = 0, int tp=Outline);
    VGLandPolygon(VGLandPolygon &polygon);
    ~VGLandPolygon();

    QVariantList GetPath()const;
    void SetPath(const QVariantList &path, const QList<VGCoordinate *> &coors = QList<VGCoordinate *>());
    void AddCoordinate(const QGeoCoordinate &coor, int nSat = 0, VGCoordinate *c=NULL);
    void AddCoordinate(const VGCoordinate &coor);
    QList <VGCoordinate *> GetCoordinates()const;
    int CountCoordinate()const;
    double CalculateArea()const;

    MapItemType ItemType()const;
    bool operator==(const MapAbstractItem &item)const;
    void Show(bool b);
    void SetShowType(int t);

    int ItemIndex(const MapAbstractItem *item)const;
    bool IsValid()const;
    void Clear();
    bool IsFreePlan()const;
private slots :
    void onIdChange(int id);
    void onPointDelete();
    void onPointChanged(const QGeoCoordinate &co);
private:
    VGCoordinate *_getCoordinate(const QGeoCoordinate &co, int id);
    void _insertCoor(VGCoordinate *item, VGCoordinate *behind);
signals:
    void    pathChanged(const QVariantList &path);
    void    countCoorChanged(int);
private:
    QVariantList            m_path;     //
    QList<VGCoordinate *>   m_coors;    //测绘点
    int                     m_typeShow;
};

#endif // VGLANDPOLYGON_H
