#include "RoutePlanning.h"
#include <math.h>

#define  IGNORESpace 0.001

static VGPoint getPointByIndex(const vector<VGLineSeg> segs, int idx)
{
    if (idx < 0 || idx >= (int)segs.size() * 2)
        return VGPoint();

    const VGLineSeg &seg = segs[idx / 2];
    return idx % 2 ? seg.GetEnd() : seg.GetBegin();
}

static VGPoint getPointByPos(const vector<vector<VGLineSeg>> segs, int nLine, int idx)
{
    if (nLine < 0 || nLine >= (int)segs.size())
        return VGPoint();

    return getPointByIndex(segs[nLine], idx);
}

static bool isContains(const vector<int> v, int p)
{
    for (int itr:v)
    {
        if (itr == p)
            return true;
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//VGBoudary
///////////////////////////////////////////////////////////////////////////////////////////////
VGBoudary::VGBoudary(const VGPolygon &outline, const vector<VGPolygon> &blocks)
: VGPolygon(outline), m_blocks(blocks), m_blockExpand(0), m_space(2)
, m_widthShrink(0), m_longShrink(0), m_bBlockValid(false)
{
}

VGBoudary::VGBoudary(const VGBoudary &oth) : VGPolygon(oth)
, m_blocks(oth.m_blocks), m_bBlockValid(false)
{
}

void VGBoudary::SetOutLine(const VGPolygon &outline)
{
    m_points = outline.Points();
}

void VGBoudary::SetOrientation(double radius)
{
    m_orin = VGVertex(true, radius);
}

void VGBoudary::AddBlock(const VGPolygon &plg)
{
    if (plg.IsValid())
        m_blocks.push_back(plg);
}

void VGBoudary::SetSpaceValue(double f)
{
    m_space = f;
}

void VGBoudary::ShrinkWidthValue(double f)
{
    m_widthShrink = f;
}

void VGBoudary::ShrinkLongValue(double f)
{
    m_longShrink = f;
}

void VGBoudary::SetBlockExpandValue(double f)
{
    if (m_blockExpand == f)
        return;
    m_blockExpand = f;
    m_bBlockValid = false;
}

void VGBoudary::_blockExpand()
{
    m_bBlockValid = true;

    if (DoubleEqual(m_blockExpand, 0))
    {
        m_expBlocks = m_blocks;
        return;
    }

    m_expBlocks.clear();
    for (size_t i = 0; i < m_blocks.size(); i++)
    {
        m_expBlocks.push_back(m_blocks[i].GetExpand(m_blockExpand));
    }
}

double VGBoudary::_getSafeRouteLength(const VGPoint &pnt1, const VGPoint &pnt2) const
{
    VGPolyLine pl;
    GenerateSafeRoute(pl, pnt1, pnt2);
    return pl.Length();
}

void VGBoudary::Shrink()
{
    if (m_edgeChanges.size() && m_edgeChanges.at(0).index == -1)
        m_outline = GetExpand(m_edgeChanges.at(0).edgeShrink);
    else if (m_edgeChanges.size())
        m_outline = GetExpand(m_edgeChanges);
    else
        m_outline = *this;
    _blockExpand();
}

void VGBoudary::SetSupportPoint(const VGPoint &pnt)
{
    m_pntSupport = pnt;
}

double VGBoudary::_generateOpSeg(vector<vector<VGLineSeg>> &segs)
{
    VGVertex v = m_orin.CrossMultiply();
    VGPoint point1;
    VGPoint point2;
    m_outline.GetTipPoint(m_orin, point1, point2);
    double var = v.DotMultiply(VGVertex(point1, point2)) / v.UnitLength();
    double fStep = (var > 0 ? m_space : -m_space) / v.UnitLength();
    double l = (var - fStep*(int)((var - IGNORESpace) / fStep)) / 2;
    double lenSeg = 0;
    segs.clear();
    while (l*l <= var*var)
    {
        vector<VGLineSeg> resLineSeg;
        VGLine line(m_orin, point1 + v*l);
        l += fStep;
        lenSeg += _calculate1LineSegs(line, resLineSeg);
        if (resLineSeg.size() > 0)
        {
            if (m_bAnti)
                segs.insert(segs.begin(), resLineSeg);
            else
                segs.push_back(resLineSeg);
        }
    }
    return lenSeg;
}

double VGBoudary::_calculate1LineSegs(const VGLine &line, vector<VGLineSeg> &resLineSeg)const
{
    map<double, VGPoint> ints = line.IntersectionsXY(m_outline);
    if (ints.size() < 2)
        return 0;
    map<double, VGPoint>::iterator itr = ints.begin();
    VGPoint pntLast = (itr++)->second;
    for (; itr != ints.end(); itr++)
    {
        VGLineSeg segTmp(pntLast, itr->second);
        if (m_outline.IsContains(segTmp))
            resLineSeg.push_back(segTmp);
        pntLast = itr->second;
    }

    for (const VGPolygon plg : m_expBlocks)
    {
        vector<VGLineSeg> tmpSegs;
        for (const VGLineSeg &seg : resLineSeg)
        {
            plg.CutSeg(seg, tmpSegs);
        }
        resLineSeg = tmpSegs;
    }

    double lenSeg = 0;
    for (size_t i=0; i<resLineSeg.size(); i++)
    {
        lenSeg += resLineSeg[i].Length();
    }
    return lenSeg;
}

bool VGBoudary::PlanRoute(RoutePlanning *pl, VGRoute &plg)
{
    if (pl)
    {
        vector<vector<VGLineSeg>> segs;
        plg.OperateVoyage() = _generateOpSeg(segs);
        _generateRout(plg, segs);
        return true;
    }
    return false;
}

const vector<VGPolygon> &VGBoudary::ExpandBlocks()const
{
    return m_expBlocks;
}

const VGPolygon &VGBoudary::ShrinkBoudary() const
{
    return m_outline;
}

void VGBoudary::ClearBlocks()
{
    m_blocks.clear();
    m_expBlocks.clear();
    m_bBlockValid = false;
}

void VGBoudary::SetAnti(bool b)
{
    m_bAnti = b;
}

void VGBoudary::SetEdgeChange(double sh, int idx)
{
    if (idx >= (int)m_points.size())
        return;

    if (idx > 0 && m_edgeChanges.size() && m_edgeChanges.at(0).index<0)
        _removeChangeEdge(-1);

    _removeChangeEdge(idx);
    if (idx == -1)
        m_edgeChanges.insert(m_edgeChanges.begin(), EdgeChange(idx, sh));
    else
        m_edgeChanges.push_back(EdgeChange(idx, sh));
}

bool VGBoudary::_generateRout(VGPolyLine &rt, vector<vector<VGLineSeg>> &segs)const
{
    if (segs.size() <= 0 || segs.front().size() <= 0)
        return false;

    const VGLineSeg &segTmp = segs[0][0];
    rt.AddRoutePoint(segTmp.GetBegin(), VGRoutePoint::Enter);

    int idxLineLast = 0;
    int idxPntLast = 0;
    while (segs.size())
    {
        if (!_linkRoute(rt, segs, idxLineLast, idxPntLast))
            return false;
    }

    return true;
}

bool VGBoudary::GenerateSafeRoute(VGPolyLine &rt, const VGPoint &pnt1, const VGPoint &pnt2) const
{
    bool ret;
    VGPoint pntLast = pnt1;
    for (int i=0; i<1000; i++)
    {
        int segPos = _getSafeRoute(pntLast, pnt2, rt, &pntLast);
        if (segPos)
        {
            ret = segPos > 0;
            break;
        }
    }

    if (!ret)
    {
        rt.Clear();
        rt.AddRoutePoint(pnt1);
        rt.AddRoutePoint(pnt2, VGRoutePoint::Unreach);
    }
    return ret;
}

bool VGBoudary::IsSafeRoute(const VGPoint &pnt1, const VGPoint &pnt2) const
{
    map<double, const VGPolygon *> blocks;
    VGLineSeg seg(pnt1, pnt2);
    _blockDistance(blocks, seg);
    return blocks.size() == 0;
}

int VGBoudary::_getSafeRoute(const VGPoint &pnt1, const VGPoint &pnt2, VGPolyLine &rt, VGPoint *last)const
{
    if (m_outline.IsContains(VGLineSeg(pnt1, pnt2)))
    {
        rt.CalculateNearerWay(pnt1);
        map<double, const VGPolygon *> blocks;
        VGLineSeg seg(pnt1, pnt2);
        _blockDistance(blocks, seg);
        if (blocks.size())
        {
            double tmp = blocks.begin()->first;
            const VGPolygon &plg = *blocks.begin()->second;
            vector<VGPoint> pnts = plg.GetCornerPoint(pnt1, pnt2, m_outline);
            if (pnts.size() == 0)
                return -1;

            if (pnts.size() == 1)
            {
                if (pnts.front() == pnt2)
                {
                    rt.AddRoutePoint(pnt2);
                    return 1;
                }
                if (last)
                    *last = pnts.front();

                return 0;
            }

            VGPoint pntLast = _getPropertyCorner(rt, pnt2, blocks.begin()->second->GetCornerPoint(pnt1, pntLast, m_outline));
            map<double, const VGPolygon *> blocksTmp;
            _blockDistance(blocksTmp, VGLineSeg(pnt1, pntLast));
            if (blocksTmp.size() > 0 && pnts.size() > 1)
            {
                pntLast = pnts.back();
                _blockDistance(blocksTmp, VGLineSeg(pnt1, pntLast));
                if (blocksTmp.size() > 0)
                    pntLast = seg.GetPoint(tmp);
            }
            else if(blocksTmp.size() > 0)
            {
                pntLast = seg.GetPoint(tmp);
            }

            if (last)
                *last = pntLast;
            return 0;
        }
        rt.AddRoutePoint(pnt2);
        return 1;
    }

    if (m_outline.RoundRoute(pnt1, pnt2, rt))
        return 1;

    return -1;
}

bool VGBoudary::_linkRoute(VGPolyLine &rt, vector<vector<VGLineSeg>> &segs, int &nLine, int &nIdx)const
{
    if (nLine < 0 || nIdx < 0 || nLine >= (int)segs.size() || nIdx >= (int)segs[nLine].size() * 2)
        return false;

    VGLineSeg seg = segs[nLine][nIdx / 2];
    bool bBeg = nIdx % 2 == 0;
    VGPoint pntTra = bBeg ? seg.GetEnd() : seg.GetBegin();
    segs[nLine].erase(segs[nLine].begin() + nIdx / 2);
    if (segs[nLine].size() == 0)
    {
        segs.erase(segs.begin() + nLine);
        if (nLine == 0)
            nLine = -1;
    }

    if (segs.size() == 0)
    {
        rt.AddRoutePoint(pntTra, VGRoutePoint::Return);
        return true;
    }
    VGPolyLine pll;
    nIdx = _getNearestSeg(segs, nLine, pntTra, !bBeg, pll);
    for (const VGRoutePoint &pnt: pll.RoutePoints())
    {
        if (&pnt == &pll.RoutePoints().front())
        {
            rt.AddRoutePoint(pnt, VGRoutePoint::Transform);
        }
        else if (pnt.GetType() == VGRoutePoint::Unreach)
        {
            rt.AddRoutePoint(pnt, VGRoutePoint::Unreach);
            return false;
        }
        else if (&pnt != &pll.RoutePoints().back())
        {
            rt.AddRoutePoint(pnt, VGRoutePoint::Route);
        }
        else
        {
            rt.AddRoutePoint(pnt, VGRoutePoint::Operation);
        }
    }
    return true;
}

int VGBoudary::_getNearestSeg(const vector<vector<VGLineSeg>> &segs, int &nLine, const VGPoint &pnt, bool bBeg, VGPolyLine &rt)const
{
    const int nLineCur = nLine;
    nLine = segs.size() - 1;
    rt.Clear();
    int nRet = -1;
	vector<int> serchs;
	double dis = -1;
    if (nLineCur + 1 > (int)segs.size())
	{
		serchs.push_back(nLine);
    }
    else if (nLineCur < 0)
    {
		nLine = 0;
        serchs.push_back(nLine);
        if (segs.size()>1)
            serchs.push_back(1);
    }
    else
    {
        nRet = bBeg ? segs[nLineCur].size() * 2 - 1 : 0;
		serchs = _calcSearchs(segs, pnt, nLineCur, bBeg, nLine, rt);
        if (rt.CountPoint() > 1 && ((VGRoutePoint*)&(rt.Last()))->GetType() != VGRoutePoint::Unreach)
            dis = rt.Length();
        else
            nRet = -1;
	}

    int tmp = 0;
	for (int ss : serchs)
	{
		const vector<VGLineSeg> &line = segs[ss];
		for (size_t i = 0; i < line.size(); ++i)
		{
			const VGLineSeg &segTmp = line.at(i);
			VGPolyLine tmp1;
			VGPolyLine tmp2;
			if (!GenerateSafeRoute(tmp1, pnt, segTmp.GetBegin()))
                tmp |= 1;
            if (!GenerateSafeRoute(tmp2, pnt, segTmp.GetEnd()))
                tmp |= 2;
            if (tmp == 3)
                continue;

			double disTmp1 = tmp == 1 ? -1 : tmp1.Length();
            double disTmp2 = tmp == 2 ? -1 : tmp2.Length();
            bool bBegLess = true;
            if (disTmp1 < 0 || (disTmp2 > 0 && disTmp2 < disTmp1))
                bBegLess = false;
            double disTmp = bBegLess ? disTmp1 : disTmp2;
			if (DoubleLess(disTmp, dis) || dis < 0)
			{
				dis = bBegLess ? disTmp1 : disTmp2;
				nLine = ss;
				nRet = bBegLess ? i * 2 : i * 2 + 1;
				rt = bBegLess ? tmp1 : tmp2;
			}
		}
	}

    tmp = _checkOtherLine(segs, serchs, nLine, rt);
    if (tmp >= 0)
        return tmp;

    return nRet;
}

vector<int> VGBoudary::_calcSearchs(const vector<vector<VGLineSeg>> &segs, const VGPoint &pnt, int nLineCur, bool &bBeg, int &nLine, VGPolyLine &rt) const
{
    const VGLineSeg &seg = segs[nLineCur].front();
    vector<int> serchs;
    if (VGLine(seg).IsOnLine(pnt))
    {
        bBeg = GenerateSafeRoute(rt, pnt, bBeg ? segs[nLineCur].back().GetEnd() : segs[nLineCur].front().GetBegin());
        nLine = nLineCur;
        if (nLineCur - 1 >= 0)
            serchs.push_back(nLineCur - 1);

        serchs.push_back(nLineCur);
        if (nLineCur + 1 < (int)segs.size())
            serchs.push_back(nLineCur + 1);
    }
    else
    {
        if (nLineCur - 1 >= 0 && fabs(VGLine(segs[nLineCur - 1].front()).DistanceToPoint(pnt)) < 1.5*m_space)
        {
            serchs.push_back(nLineCur-1);
            serchs.push_back(nLineCur);
        }
        else if (nLineCur + 1 <= nLine && fabs(VGLine(segs[nLineCur + 1].front()).DistanceToPoint(pnt)) < 1.5*m_space)
        {
            serchs.push_back(nLineCur);
            serchs.push_back(nLineCur + 1);
        }
        else
        {
            serchs.push_back(nLineCur);
        }
    }
    return serchs;
}

int VGBoudary::_checkOtherLine(const vector<vector<VGLineSeg>> &segs, const vector<int> &serchs, int &nLine, VGPolyLine &rt)const
{
    int nRet = -1;
    for (size_t ss = 0; ss < segs.size(); ++ss)
    {
        if (isContains(serchs, ss))
            continue;
        const vector<VGLineSeg> &lines = segs[ss];
        for (size_t i = 0; i < lines.size(); ++i)
        {
            const VGLineSeg &segTmp = lines.at(i);
            if (rt.CutBeging(segTmp.GetBegin()))
            {
                nLine = ss;
                nRet = i * 2;
            }
            else if (rt.CutBeging(segTmp.GetEnd()))
            {
                nLine = ss;
                nRet = i * 2 + 1;
            }
        }
    }
    while (rt.CountLineSeg() > 1)
    {
        VGLineSeg sg(rt.RoutePoints().front(), rt.RoutePoints().at(2));
        if (!m_outline.IsContains(sg))
            return nRet;
        map<double, const VGPolygon *> blocks;
        _blockDistance(blocks, sg);
        if (blocks.size())
            return nRet;

        rt.RemoveAt(1);
    }
    return nRet;
}

void VGBoudary::_removeChangeEdge(int idx)
{
    idx = idx < 0 ? -1 : idx;
    int nCount = m_edgeChanges.size();
    for (int i = 0; i < nCount; ++i)
    {
        if (idx == m_edgeChanges[i].index)
        {
            m_edgeChanges.erase(m_edgeChanges.begin() + i);
            break;
        }
    }
}

VGPoint VGBoudary::_getPropertyCorner(const VGPolyLine &rt, const VGPoint &end, const vector<VGPoint> &pnts)const
{
    if (pnts.size() == 0)
        return VGPoint();

    if (rt.CountPoint() == 0)
        return pnts.front();

    VGLine line(rt.First(), end);
    double t = -1;
    VGPoint ret;
    for (const VGPoint &itr : pnts)
    {
        if (DoubleEqual(t, -1))
        {
            t = line.GetParam(itr);
            ret = itr;
            continue;
        }

        double tmp = line.GetParam(itr);
        if (tmp > t)
        {
            t = tmp;
            ret = itr;
        }
    }
    return ret;
}

void VGBoudary::_blockDistance(map<double, const VGPolygon*> &blocks, const VGLineSeg &seg)const
{
    VGLine line(seg);
    blocks.clear();
    for (const VGPolygon &plg : m_expBlocks)
    {
        if (!plg.IsIntersect(seg))
            continue;

        map<double, VGPoint> pnts = line.IntersectionsXY(plg);//平行不算交点，
		map<double, VGPoint>::iterator itr = pnts.begin();
		int nRemain = pnts.size();
		for (; itr != pnts.end(); itr++)
		{
			--nRemain;
			if ((itr->first > 0 && itr->first < 1) || (DoubleEqual(0, itr->first) && nRemain>0))
			{
				blocks[pnts.begin()->first] = &plg;
				break;
			}
		}
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
//RoutePlanning
///////////////////////////////////////////////////////////////////////////////////////////////
VGBoudary &RoutePlanning::Boudary()
{
    return m_boundary;
}

bool RoutePlanning::Palnning(VGRoute &res)
{   
    return m_boundary.PlanRoute(this, res);
}
