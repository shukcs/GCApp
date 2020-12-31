#include "VGPlanningWorker.h"
#include <QDebug>
#include <QDateTime>
#include "VGFlyRoute.h"
#include "VGLandBoundary.h"
#include "VGLandPolygon.h"
#include "VGCoordinate.h"
#include "QtMath"

static CoordinateList tranceToGeoPolygon(const QGeoCoordinate &home, const VGPolygon &plg)
{
    CoordinateList ret;
    QGeoCoordinate coor;
    for (const VGPoint &pnt : plg.Points())
    {
        VGCoordinate::transXY2Coor(home, pnt.GetX(), pnt.GetY(), coor);
        ret << coor;
    }
    return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//VGPlanningWorker
//////////////////////////////////////////////////////////////////////////////////////////////////////
VGPlanningWorker::VGPlanningWorker(QObject *parent) : QObject(parent)
, m_landBoundary(NULL), m_timerID(-1)
{
    connect(this, &VGPlanningWorker::planning, this, &VGPlanningWorker::onPlanning);
}

QList<CoordinateList> VGPlanningWorker::GenShrinkBoudary(VGFlyRoute *rt, const QGeoCoordinate &c)
{
    QList<CoordinateList> ret;
    if (!_genPlanBoudary(rt))
        return ret;

    m_home = c;
    m_boundary.SetBlockExpandValue(rt->GetBlockSafeDis());
    for (int idx : rt->GetEdgeShinkLs())
    {
        m_boundary.SetEdgeChange(-rt->GetEdgeShink(idx), idx);
    }
    m_boundary.Shrink();
    ret.append(tranceToGeoPolygon(m_home, m_boundary.ShrinkBoudary()));
    for (const VGPolygon &plg : m_boundary.ExpandBlocks())
    {
        ret.append(tranceToGeoPolygon(m_home, plg));
    }
    return ret;
}

bool VGPlanningWorker::IsSafeRoute(VGFlyRoute *rt, const QGeoCoordinate &p1, const QGeoCoordinate &p2)
{
    if (!_genPlanBoudary(rt))
        return true;

    m_boundary.SetSpaceValue(rt->GetSprinkleWidth());
    m_boundary.SetBlockExpandValue(rt->GetBlockSafeDis());
    for (int idx : rt->GetEdgeShinkLs())
    {
        m_boundary.SetEdgeChange(-rt->GetEdgeShink(idx), idx);
    }
    m_boundary.Shrink();
    VGPoint pnt1;
    VGCoordinate::transCoor2XY(m_home, p1, pnt1.X(), pnt1.Y());
    VGPoint pnt2;
    VGCoordinate::transCoor2XY(m_home, p2, pnt2.X(), pnt2.Y());
    return m_boundary.IsSafeRoute(pnt1, pnt2);
}


VGPlanningWorker::PointPosType VGPlanningWorker::GetCoordinatePos(const QGeoCoordinate &c) const
{
    VGPoint pnt1;
    VGCoordinate::transCoor2XY(m_home, c, pnt1.X(), pnt1.Y());
    if (!m_boundary.ShrinkBoudary().IsContains(pnt1))
        return OutBoundary;

    for (const VGPolygon &itr : m_boundary.ExpandBlocks())
    {
        if (itr.IsContains(pnt1))
            return InBlock;
    }

    return OK;
}

void VGPlanningWorker::BoundaryPlan(VGFlyRoute *rt, const QGeoCoordinate &c)
{
    if (rt)
        emit planning(rt, c);
}

QGeoCoordinate VGPlanningWorker::GetPolygonCenter(const VGLandPolygon &plg)
{
    if (plg.CountCoordinate() <= 0)
        return QGeoCoordinate();

    QGeoCoordinate coor = plg.GetCoordinates().first()->GetCoordinate();
    VGPolygon pg; 
    TrancePlygon(coor, plg, pg);
    VGPoint pnt = pg.GetCenter();
    QGeoCoordinate ret;
    VGCoordinate::transXY2Coor(coor, pnt.GetX(), pnt.GetY(), ret);
    return ret;
}

void VGPlanningWorker::onPlanning(VGFlyRoute *rt, const QGeoCoordinate &home)
{
    if (!GenMissionBoundary(rt, this, home))
        return;

    m_boundary.SetSpaceValue(rt->GetSprinkleWidth());
    m_boundary.SetOrientation(qDegreesToRadians(rt->GetAngle()));
    m_boundary.SetAnti(rt->GetAnti());
    VGRoute res;
    if (Palnning(res))
        emit  planFinished(res, rt);
}

bool VGPlanningWorker::_genPlanBoudary(VGFlyRoute *rt)
{
    if (!rt)
        return false;

    VGLandBoundary *bdr = rt->GetBelongedBoundary();
    bool ret = bdr && m_home.isValid();
    if (ret && m_landBoundary!=bdr )
    {
        m_landBoundary = bdr;
        m_boundary.ClearBlocks();
        foreach(VGLandPolygon *plg, m_landBoundary->Polygos())
        {
            if (plg->GetId() == VGLandPolygon::Boundary)
            {
                TrancePlygon(m_home, *plg, m_boundary);
            }
            else if (plg->GetId() == VGLandPolygon::BlockBoundary)
            {
                VGPolygon pg;
                TrancePlygon(m_home, *plg, pg);
                m_boundary.AddBlock(pg);
            }
        }
    }
    return ret;
}

void VGPlanningWorker::TrancePlygon(const QGeoCoordinate &home, const VGLandPolygon &ldPlg, VGPolygon &plg)
{
    plg.Clear();
    for(const QVariant &var : ldPlg.path())
    {
        double x, y;
        VGCoordinate::transCoor2XY(home, var.value<QGeoCoordinate>(), x, y);
        plg.SetPoint(x, y);
    }
}

double VGPlanningWorker::CalculatePolygonArea(const VGLandPolygon &plg)
{
    VGPolygon plgTmp;
    TrancePlygon(plg.GetCoordinates().first()->GetCoordinate(), plg, plgTmp);
    return plgTmp.CalculatePolygonArea();
}

bool VGPlanningWorker::GenMissionBoundary(VGFlyRoute *fr, VGPlanningWorker *bdStruct, const QGeoCoordinate &home)
{
    if (!bdStruct || !fr)
        return false;

    bdStruct->m_home = home;
    if (!bdStruct->_genPlanBoudary(fr))
        return false;

    bdStruct->m_boundary.SetBlockExpandValue(fr->GetBlockSafeDis() + .1);
    for (int idx : fr->GetEdgeShinkLs())
    {
        bdStruct->m_boundary.SetEdgeChange(-fr->GetEdgeShink(idx) - .1, idx);
    }
    bdStruct->m_boundary.Shrink();
    return true;
}
