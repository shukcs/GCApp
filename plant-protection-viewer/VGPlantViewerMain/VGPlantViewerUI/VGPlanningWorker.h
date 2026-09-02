#ifndef VGPLANNINGWORKER_H
#define VGPLANNINGWORKER_H

#include <QObject>
#include <QGeoCoordinate>
#include "RoutePlanning.h"
#include <QVariant>
#include <QList>
/*
 *航线规划线程对象类
 *
*/
class VGFlyRoute;
class VGLandPolygon;
class VGLandBoundary;
typedef QList<QGeoCoordinate> CoordinateList;

class VGPlanningWorker : public QObject, public RoutePlanning
{
    Q_OBJECT
public:
    enum PointPosType {
        OK,
        OutBoundary,
        InBlock,
    };
public:
    explicit VGPlanningWorker(QObject *parent = 0);

    QList<CoordinateList> GenShrinkBoudary(VGFlyRoute *rt, const QGeoCoordinate &c);
    bool IsSafeRoute(VGFlyRoute *rt, const QGeoCoordinate &p1, const QGeoCoordinate &p2);
    PointPosType GetCoordinatePos(const QGeoCoordinate &c)const;
    void BoundaryPlan(VGFlyRoute *rt, const QGeoCoordinate &c);
public:
    static QGeoCoordinate GetPolygonCenter(const VGLandPolygon &plg);
    static void TrancePlygon(const QGeoCoordinate &home, const VGLandPolygon &ldPlg, VGPolygon &plg);
    static double CalculatePolygonArea(const VGLandPolygon &ldPlg);
    static bool GenMissionBoundary(VGFlyRoute *fr, VGPlanningWorker *bdStruct, const QGeoCoordinate &home);
protected slots:
    void onPlanning(VGFlyRoute *rt, const QGeoCoordinate &home);
signals:
    void planFinished(const VGRoute &pll, VGFlyRoute *rt);
    void planning(VGFlyRoute *rt, const QGeoCoordinate &home);
private:
    bool _genPlanBoudary(VGFlyRoute *rt);
private:
    VGLandBoundary      *m_landBoundary;
    int                 m_timerID;
    QGeoCoordinate      m_home;
};

#endif // VGPLANNINGWORKER_H
