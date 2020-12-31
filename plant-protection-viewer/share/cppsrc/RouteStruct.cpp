#include "RouteStruct.h"
#include "math.h"

using namespace std;

static int get_line_intersection(const VGPoint &p0,const VGPoint &p1,
    const VGPoint &p2, const VGPoint &p3, VGPoint &intsect)
{
    const double s10_x = p1.GetX() - p0.GetX();
    const double s10_y = p1.GetY() - p0.GetY();
    const double s32_x = p3.GetX() - p2.GetX();
    const double s32_y = p3.GetY() - p2.GetY();

    const double denom = s10_x * s32_y - s32_x * s10_y;
    if (DoubleEqual(denom, 0))
        return DoubleEqual(VGLine(p0, p1).DistanceToPoint(p2), 0) ? 1 : -1; // Collinear

    const double s02_x = p0.GetX() - p2.GetX();
    const double s02_y = p0.GetY() - p2.GetY();
    const double t_numer = s32_x * s02_y - s32_y * s02_x;

    intsect = p0 + VGPoint(s10_x, s10_y)*(t_numer / denom);

    return 0;
}
////////////////////////////////////////////////////////////////
//VGPoint
////////////////////////////////////////////////////////////////
VGPoint::VGPoint(double x, double y, double z)
    :m_x(x), m_y(y), m_z(z)
{
}

VGPoint::VGPoint(const VGPoint &oth)
    : m_x(oth.m_x), m_y(oth.m_y), m_z(oth.m_z)
{
}

double & VGPoint::X()
{
    return m_x;
}

double & VGPoint::Y()
{
    return m_y;
}

double & VGPoint::Z()
{
    return m_z;
}

double VGPoint::GetX() const
{
    return m_x;
}

double VGPoint::GetY() const
{
    return m_y;
}

double VGPoint::GetZ() const
{
    return m_z;
}

double VGPoint::DistanceTo(const VGPoint &oth)const
{
    double dx = oth.m_x - m_x;
    double dy = oth.m_y - m_y;
    double dz = oth.m_z - m_z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

double VGPoint::DistanceSqureTo(const VGPoint &oth) const
{
    double dx = oth.m_x - m_x;
    double dy = oth.m_y - m_y;
    double dz = oth.m_z - m_z;
    return dx*dx + dy*dy + dz*dz;
}

VGPoint VGPoint::operator-(const VGPoint &oth) const
{
    return VGPoint(m_x - oth.m_x, m_y - oth.m_y, m_z - oth.m_z);
}

VGPoint & VGPoint::operator-=(const VGPoint &oth)
{
    m_x -= oth.m_x;
    m_y -= oth.m_y;
    m_z -= oth.m_z;
    return *this;
}

VGPoint VGPoint::operator-(const VGVertex &oth) const
{
    return VGPoint(m_x - oth.GetX(), m_y - oth.GetY(), m_z - oth.GetZ());
}

VGPoint & VGPoint::operator-=(const VGVertex &oth)
{
    m_x -= oth.GetX();
    m_y -= oth.GetY();
    m_z -= oth.GetZ();
    return *this;
}

VGPoint VGPoint::operator+(const VGPoint &oth) const
{
    return VGPoint(m_x + oth.m_x, m_y + oth.m_y, m_z + oth.m_z);
}

VGPoint & VGPoint::operator+=(const VGPoint &oth)
{
    m_x += oth.m_x;
    m_y += oth.m_y;
    m_z += oth.m_z;
    return *this;
}

VGPoint VGPoint::operator+(const VGVertex &oth) const
{
    return VGPoint(m_x + oth.GetX(), m_y + oth.GetY(), m_z + oth.GetZ());
}

VGPoint & VGPoint::operator+=(const VGVertex &oth)
{
    m_x += oth.GetX();
    m_y += oth.GetY();
    m_z += oth.GetZ();
    return *this;
}

VGPoint VGPoint::operator*(double f) const
{
    return VGPoint(m_x * f, m_y * f, m_z * f);
}

VGPoint &VGPoint::operator*=(double f)
{
    m_x *= f;
    m_y *= f;
    m_z *= f;
    return *this;
}

VGPoint VGPoint::operator/(double f) const
{
    return VGPoint(m_x / f, m_y / f, m_z / f);
}

VGPoint & VGPoint::operator/=(double f)
{
    m_x /= f;
    m_y /= f;
    m_z /= f;
    return *this;
}

bool VGPoint::operator!=(const VGPoint &oth)const
{
    return !operator==(oth);
}

bool VGPoint::operator==(const VGPoint &oth)const
{
    return DoubleEqual(m_x, oth.m_x) && DoubleEqual(m_y, oth.m_y) && DoubleEqual(m_z, oth.m_z);
}

////////////////////////////////////////////////////////////////
//VGVertex
////////////////////////////////////////////////////////////////
VGVertex::VGVertex(double x, double y, double z)
    :m_x(x), m_y(y), m_z(z)
{
}

VGVertex::VGVertex(bool b2d, double angleY, double angleXY /*= 0*/)
{
    m_x = sin(angleY);
    m_y = cos(angleY);
	
	m_z = b2d ?  0 : sin(angleXY);
}

VGVertex::VGVertex(const VGPoint &beg, const VGPoint &end)
{
    m_x = end.GetX() - beg.GetX();
    m_y = end.GetY() - beg.GetY();
    m_z = end.GetZ() - beg.GetZ();
}

VGVertex::VGVertex(const VGVertex &oth)
    : m_x(oth.m_x), m_y(oth.m_y), m_z(oth.m_z)
{
}

double & VGVertex::X()
{
    return m_x;
}

double & VGVertex::Y()
{
    return m_y;
}

double & VGVertex::Z()
{
    return m_z;
}

double VGVertex::GetX() const
{
    return m_x;
}

double VGVertex::GetY() const
{
    return m_y;
}

double VGVertex::GetZ() const
{
    return m_z;
}

VGVertex VGVertex::operator-(const VGVertex &oth) const
{
    return VGVertex(m_x - oth.m_x, m_y - oth.m_y, m_z - oth.m_z);
}

VGVertex & VGVertex::operator-=(const VGVertex &oth)
{
    m_x -= oth.m_x;
    m_y -= oth.m_y;
    m_z -= oth.m_z;
    return *this;
}

VGVertex VGVertex::operator+(const VGVertex &oth) const
{
    return VGVertex(m_x + oth.m_x, m_y + oth.m_y, m_z + oth.m_z);
}

VGVertex & VGVertex::operator+=(const VGVertex &oth)
{

    m_x += oth.m_x;
    m_y += oth.m_y;
    m_z += oth.m_z;
    return *this;
}

VGVertex VGVertex::operator*(double f) const
{
    return VGVertex(m_x * f, m_y * f , m_z * f);
}

VGVertex & VGVertex::operator*=(double f)
{
    m_x *= f;
    m_y *= f;
    m_z *= f;
    return *this;
}

VGVertex VGVertex::operator/(double f) const
{
    return VGVertex(m_x / f, m_y / f, m_z / f);
}

VGVertex & VGVertex::operator/=(double f)
{
    m_x /= f;
    m_y /= f;
    m_z /= f;
    return *this;
}

VGVertex VGVertex::CrossMultiply(const VGVertex &v) const
{
    return VGVertex(CrossX(v), CrossY(v), CrossZ(v));
}

double VGVertex::DotMultiply(const VGVertex &v) const
{
    return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
}

bool VGVertex::IsValid() const
{
    return !DoubleEqual(m_x, 0) || !DoubleEqual(m_y, 0) || !DoubleEqual(m_z, 0);
}

bool VGVertex::operator==(const VGVertex &oth) const
{
    bool bValid = IsValid();
    bool bValidOth = oth.IsValid();
    if (bValid && bValidOth)
        return !CrossMultiply(oth).IsValid();

    return !bValid && !bValidOth;
}

bool VGVertex::operator!=(const VGVertex &oth) const
{
    return !operator==(oth);
}

double VGVertex::UnitLength() const
{
    return sqrt(UnitLengthSquare());
}

double VGVertex::UnitLengthSquare() const
{
    return m_x*m_x + m_y*m_y + m_z*m_z;
}

VGVertex VGVertex::UnitVertex() const
{
    if (!IsValid())
        return *this;

    return *this / UnitLength();
}

double VGVertex::CrossX(const VGVertex &v /*= VGVertex(0.0, 0, 1)*/) const
{
	return m_y*v.m_z - m_z*v.m_y;
}

double VGVertex::CrossY(const VGVertex &v /*= VGVertex(0.0, 0, 1)*/) const
{
	return v.m_x*m_z - v.m_z*m_x;
}

double VGVertex::CrossZ(const VGVertex &v /*= VGVertex(0.0, 0, 1)*/) const
{
	return m_x*v.m_y - m_y*v.m_x;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//VGRoutePoint
////////////////////////////////////////////////////////////////////////////////////////////////////////
VGRoutePoint::VGRoutePoint(double x, double y, double z, int tp) :VGPoint(x,y,z)
, m_type(tp)
{
}

VGRoutePoint::VGRoutePoint(const VGPoint &pnt, int tp /*= 0*/) : VGPoint(pnt)
, m_type(tp)
{
}

VGRoutePoint::VGRoutePoint(const VGRoutePoint &pnt) : VGPoint(pnt)
, m_type(pnt.m_type)
{
}

void VGRoutePoint::SetType(int t)
{
    m_type = t;
}

int VGRoutePoint::GetType() const
{
    return m_type;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//VGLineSeg
///////////////////////////////////////////////////////////////////////////////////////////////
VGLineSeg::VGLineSeg(const VGPoint &beg /*= VGPoint()*/, const VGPoint &end /*= VGPoint()*/)
    :m_beg(beg), m_end(end)
{
}

VGLineSeg::VGLineSeg(const VGLineSeg &oth)
    : m_beg(oth.m_beg), m_end(oth.m_end)
{
}

double VGLineSeg::Length() const
{
    return m_end.DistanceTo(m_beg);
}

VGPoint VGLineSeg::NearestPoint(const VGPoint &point) const
{
    VGVertex v = GetVertex();
    double t = v.DotMultiply(VGVertex(m_beg, point)) / v.UnitLengthSquare();
    if (t <= 0)
        return m_beg;

    if (t >= 1)
        return m_end;

    return m_beg + v*t;
}

double VGLineSeg::NearestDistance(const VGPoint &point) const
{
    VGVertex v = GetVertex();
    double t = v.DotMultiply(VGVertex(m_beg, point)) / v.UnitLengthSquare();
    if (t <= 0)
        return point.DistanceTo(m_beg);

    if (t >= 1)
        return point.DistanceTo(m_end);

    return point.DistanceTo(m_beg + v*t);
}

VGVertex VGLineSeg::GetVertex()
{
    return VGVertex(m_beg, m_end);
}

VGVertex VGLineSeg::GetVertex() const
{
    return VGVertex(m_beg, m_end);
}

bool VGLineSeg::IsValid() const
{
    return m_beg != m_end;
}

double VGLineSeg::LengthSqure() const
{
    return m_beg.DistanceSqureTo(m_end);
}

VGLineSeg VGLineSeg::Expand(const VGPoint &point, double dis) const
{
    VGLine ln1 = VGLine(*this);
    VGPoint pnt = ln1.RectPoint(point);
    VGLine ln2 = ln1.GetRectLineFrom(point);
    if (point == pnt)
    {
        VGVertex vC = ln2.Vertex()*(dis / ln2.Vertex().UnitLength());
        return VGLineSeg(m_beg + vC, m_end + vC);
    }
    VGPoint pntA = ln2.GetDistancePoint(pnt, dis);
    VGLine line1(point, m_beg);
    VGLine line2(point, m_end);
    double dis1 = dis * point.DistanceTo(m_beg) / point.DistanceTo(pnt);
    double dis2 = dis * point.DistanceTo(m_end) / point.DistanceTo(pnt);
    return VGLineSeg(line1.GetDistancePoint(m_beg, dis1), line2.GetDistancePoint(m_end, dis2));
}

VGPoint VGLineSeg::GetPoint(double t) const
{
    VGVertex v = GetVertex();
    if (!v.IsValid())
        return m_beg;
    return m_beg + v * t;
}

double VGLineSeg::GetParam(const VGPoint &point) const
{
    return VGLine(*this).GetParam(point);
}

bool VGLineSeg::IsOnSeg(const VGPoint &point)const
{
    if (m_beg == point || m_end == point)
        return true;

    VGVertex v(m_beg, point);
    VGVertex v2 = GetVertex();
    if (v != v2)
        return false;

    double lenSquare = LengthSqure();
    double dotM = v.DotMultiply(v);
    return dotM > 0 && dotM <= lenSquare;
}

VGLine VGLineSeg::GetRectLineFrom(const VGPoint &pnt, const VGVertex &vC) const
{
    VGVertex v = GetVertex().CrossMultiply(vC);
    return VGLine(v.UnitVertex(), pnt);
}

VGPoint VGLineSeg::GetBegin() const
{
    return m_beg;
}

VGPoint VGLineSeg::GetEnd() const
{
    return m_end;
}

VGPoint VGLineSeg::GetMid() const
{
    return (m_beg + m_end) / 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//VGLine
///////////////////////////////////////////////////////////////////////////////////////////////
VGLine::VGLine(const VGPoint &beg, const VGPoint &end) : m_vertex(beg, end)
, m_piont(beg)
{
}

VGLine::VGLine(const VGVertex &v, const VGPoint &pnt) : m_vertex(v)
, m_piont(pnt)
{
}

VGLine::VGLine(const VGLineSeg &seg) : m_vertex(seg.GetVertex())
, m_piont(seg.GetBegin())
{
}

VGLine::VGLine(const VGLine &oth) : m_vertex(oth.m_vertex)
, m_piont(oth.m_piont)
{
}

VGLine::VGLine()
{
}

VGPoint VGLine::GetOnePoint() const
{
    return m_piont;
}

void VGLine::SetOnePoint(const VGPoint &p)
{
    m_piont = p;
}

VGVertex & VGLine::Vertex()
{
    return m_vertex;
}

VGVertex VGLine::GetVertex() const
{
    return m_vertex;
}

void VGLine::SetVertex(const VGVertex &v)
{
    m_vertex = v;
}

vector<VGPoint> VGLine::IntersectionsXY(const VGLineSeg &seg) const
{
    VGPoint pnt;
    vector<VGPoint> ret;
    int nRet = get_line_intersection(m_piont, m_piont + m_vertex, seg.GetBegin(), seg.GetEnd(), pnt);

    if (nRet == 0)
    {
        double t = seg.GetParam(pnt);
        if (t>=0 && t<=1)
            ret.push_back(pnt);
    }
    else if (nRet > 0)
    {
        ret.push_back(seg.GetBegin());
        ret.push_back(seg.GetEnd());
    }

    return ret;
}

vector<VGPoint> VGLine::IntersectionsXY(const VGLine &pll) const
{
    VGPoint pnt;
    vector<VGPoint> ret;
    int nRet = get_line_intersection(m_piont, m_piont + m_vertex, pll.m_piont, pll.m_piont + pll.m_vertex, pnt);
    if (nRet == 0)
    {
        ret.push_back(pnt);
    }
    else if (nRet > 0)
    {
        ret.push_back(m_piont);
        ret.push_back(m_piont + m_vertex);
    }

    return ret;
}

map<double, VGPoint> VGLine::IntersectionsXY(const VGPolygon &plg) const
{
    map<double, VGPoint> ret;
    int nCount = plg.CountLineSeg();
    for (int i = 0; i < nCount; ++i)
    {
        VGLineSeg seg = plg.LineSegAt(i);
        vector<VGPoint> tmp = IntersectionsXY(seg);
        if (tmp.size()<=0)
            continue;

        if (tmp.size() == 1)
            ret[GetParam(tmp.front())] = tmp.front();
    }
    return ret;
}

/*
	dis^2 = VGVertex(pnt-m_piont-t*m_vertex).UnitLengthSquare()
	min(dis^2) => diff(dis^2) = 0
	m_vertex.x*(pnt-m_piont).x+m_vertex.y*(pnt-m_piont).y+m_vertex.z*(pnt-m_piont)z = t * m_vertex.UnitLengthSquare
*/
double VGLine::DistanceToPoint(const VGPoint &pnt) const
{
	if (!m_vertex.IsValid())
		return m_piont.DistanceTo(pnt);

	VGVertex v(m_piont, pnt);
	double t = m_vertex.DotMultiply(v) / m_vertex.UnitLengthSquare();

    double ret = GetPoint(t).DistanceTo(pnt);
    return m_vertex.CrossZ(v) < 0? ret:-ret;
}

VGLine VGLine::GetRectLineFrom(const VGPoint &pnt) const
{
	if (!m_vertex.IsValid())
		return VGLine(m_piont, pnt);

	double t = m_vertex.DotMultiply(VGVertex(m_piont, pnt)) / m_vertex.UnitLengthSquare();
    return VGLine(m_piont + m_vertex*t, pnt);
}

VGPoint VGLine::GetDistancePoint(const VGPoint &pnt, double dis)const
{
    return pnt + m_vertex * (dis / m_vertex.UnitLength());
}

VGPoint VGLine::RectPoint(const VGPoint &point) const
{
	if (!m_vertex.IsValid())
		return m_piont;

    double t = m_vertex.DotMultiply(VGVertex(m_piont, point)) / m_vertex.UnitLengthSquare();
    return m_piont + m_vertex*t;
}

VGPoint VGLine::GetPoint(double t) const
{
    if (!m_vertex.IsValid())
        return m_piont;

    return m_piont + m_vertex * t;
}

double VGLine::GetParam(const VGPoint &point)const
{
    VGVertex v(m_piont, point);
    if (!m_vertex.IsValid() || !v.IsValid())
        return 0;

    if (point == m_piont + m_vertex)
        return 1;

    return m_vertex.DotMultiply(v) / m_vertex.UnitLengthSquare();
}

bool VGLine::IsOnLine(const VGPoint &poin) const
{
    VGVertex v(m_piont, poin);
    if (!v.IsValid())
        return true;

    return m_vertex == v;
}

bool VGLine::operator!=(const VGLine &oth) const
{
    return !operator==(oth);
}

bool VGLine::operator==(const VGLine &oth) const
{
    VGVertex v(m_piont, oth.m_piont);
    return m_vertex == oth.m_vertex && (!v.IsValid() || v == m_vertex);
}
////////////////////////////////////////////////////////////////
//VGPoint
////////////////////////////////////////////////////////////////
VGSinAngleArea::VGSinAngleArea(const VGSinAngle &an)
{
    AdjustByContains(an);
}

bool VGSinAngleArea::IsValid()const
{
    return m_angleBeg != m_angleEnd;
}

bool VGSinAngleArea::Contains(const VGSinAngle &an)const
{

    if (an == m_angleBeg || an == m_angleEnd)
        return true;
    if (m_angleBeg < m_angleEnd && m_angleBeg < an && an < m_angleEnd)
        return true;
    if (m_angleEnd < m_angleBeg && (m_angleBeg < an || an < m_angleEnd))
        return true;

    return false;
}

int VGSinAngleArea::AdjustByContains(const VGSinAngle &an)
{
    int ret = 0;
    if (an == m_angleBeg)
    {
        ret = 1;
    }
    else if (an == m_angleEnd)
    {
        ret = -1;
    }
    else if (an.GetSinAngle()>0 && an>m_angleEnd)
    {
        m_angleEnd = an;
        ret = -2;
    }
    else if (an.GetSinAngle()<0 && (an<m_angleBeg || m_angleBeg.IsZero()))
    {
        m_angleBeg = an;
        ret = 2;
    }
    return ret;
}
const VGSinAngle &VGSinAngleArea::GetBegin()const
{
    return m_angleBeg;
}
const VGSinAngle &VGSinAngleArea::GetEnd()const
{
    return m_angleEnd;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//VGSinAngle
///////////////////////////////////////////////////////////////////////////////////////////////
VGSinAngle::VGSinAngle(const VGPoint &pnt1, const VGPoint &pnt2, const VGPoint &pnt3)
    : m_sin(_calculateSin(VGLine(pnt2, pnt1), pnt3))
    , m_cos(_calculateCos(VGLine(pnt2, pnt1), pnt3))
{
}

VGSinAngle::VGSinAngle(const VGVertex &v1, const VGVertex &v2)
    : m_sin(_calculateSin(v1, VGPoint() + v2))
    , m_cos(_calculateCos(v1, VGPoint() + v2))
{
}

VGSinAngle::VGSinAngle(const VGLine &line, const VGPoint &pnt)
    : m_sin(_calculateSin(line, pnt))
    , m_cos(_calculateCos(line, pnt))
{
}

VGSinAngle::VGSinAngle() : m_sin(0), m_cos(1)
{
}

double VGSinAngle::GetSinAngle() const
{
    return m_sin;
}

bool VGSinAngle::IsAcute() const
{
    return m_cos > 0;
}

double VGSinAngle::GetCos() const
{
    return m_cos;
}

bool VGSinAngle::operator!=(const VGSinAngle &oth) const
{
    return !operator==(oth);
}

bool VGSinAngle::operator==(const VGSinAngle &oth) const
{
    return DoubleEqual(m_cos, oth.m_cos) && DoubleEqual(m_sin, oth.m_sin);
}

double VGSinAngle::_calculateSin(const VGLine &line, const VGPoint &pnt)
{
    VGVertex v = VGVertex(line.GetOnePoint(), pnt);
    if (!v.IsValid())
        return 0;

    double dis =line.DistanceToPoint(pnt) / v.UnitLength();

    return dis;
}

double VGSinAngle::_calculateCos(const VGLine &line, const VGPoint &pnt)
{
    VGVertex v1 = line.GetVertex();
    VGVertex v2(line.GetOnePoint(), pnt);
    if (!v1.IsValid() || !v2.IsValid())
        return 1;

    double unit = v1.UnitLength()*v2.UnitLength();

    return v1.DotMultiply(v2) / unit;
}

bool VGSinAngle::operator<(const VGSinAngle &oth) const
{
    if (m_sin >= 0)//[0-180]
    {
        if (oth.m_sin < 0)
            return true;
        if (IsAcute() && !oth.IsAcute())
            return true;
        if (!IsAcute() && oth.IsAcute())
            return false;
    }
    else
    {
        if (oth.m_sin >= 0)
            return false;
        if (IsAcute() && !oth.IsAcute())
            return false;
        if (!IsAcute() && oth.IsAcute())
            return true;
    }
    return IsAcute() ? m_sin < oth.m_sin : m_sin > oth.m_sin;
}

bool VGSinAngle::operator>(const VGSinAngle &oth) const
{
    if (operator==(oth))
        return false;

    return !operator<(oth);
}

VGSinAngle VGSinAngle::operator+(const VGSinAngle &oth) const
{
    VGSinAngle ret;
    ret.m_sin = m_sin*oth.GetCos() + GetCos() * oth.m_sin;
    ret.m_cos = GetCos()*oth.GetCos() - m_sin * oth.m_sin;
    return ret;
}

VGSinAngle VGSinAngle::operator-(const VGSinAngle &oth) const
{
    VGSinAngle ret;
    ret.m_sin = m_sin*oth.GetCos() - GetCos() * oth.m_sin;
    ret.m_cos = GetCos()*oth.GetCos() + m_sin * oth.m_sin;
    return ret;
}

VGSinAngle VGSinAngle::TranceTriAngle() const
{
    VGSinAngle ret(*this);
    if (ret.m_sin < 0)
        ret.m_sin = -ret.m_sin;

    return ret;
}

bool VGSinAngle::IsZero() const
{
    return DoubleEqual(m_sin, 0);
}
///////////////////////////////////////////////////////////////////////////////////////////////
//VGPolyLine
///////////////////////////////////////////////////////////////////////////////////////////////
VGPolyLine::VGPolyLine(const vector<VGRoutePoint> &points) :m_points(points)
{
}

VGPolyLine::VGPolyLine(const VGPolyLine &oth) : m_points(oth.m_points)
{
}

double VGPolyLine::Length() const
{
    double ret = 0;
    for (int i = 0; i < CountLineSeg(); i++)
    {
        ret += LineSegAt(i).Length();
    }

    return ret;
}

VGLineSeg VGPolyLine::LineSegAt(int idx) const
{
    if (idx < 0 && idx >= (int)m_points.size())
        return VGLineSeg();

    return VGLineSeg(m_points.at(idx), m_points.at(idx+1));
}

int VGPolyLine::CountLineSeg() const
{
    if (m_points.size() < 2)
        return 0;

    return m_points.size() - 1;
}

int VGPolyLine::CountPoint() const
{
    return m_points.size();
}

void VGPolyLine::AddFrontPoint(const VGPoint &pnt, int type /*= VGRoutePoint::Route*/)
{
    m_points.insert(m_points.begin(), VGRoutePoint(pnt, type));
}

void VGPolyLine::AddRoutePoint(const VGPoint &pnt, int type)
{
    m_points.push_back(VGRoutePoint(pnt, type));
}

bool VGPolyLine::CalculateNearerWay(const VGPoint &pnt, int type)
{
    if (m_points.size()==0)
    {
        m_points.push_back(VGRoutePoint(pnt, type));
        return false;
    }
    bool ret = false;
    VGLine line(m_points.back(), pnt);
    int nCount = (int)m_points.size() - 2;
    for (int i = 0; i < nCount; ++i)
    {
        vector<VGPoint> pnts = line.IntersectionsXY(VGLineSeg(m_points[i], m_points[i+1]));
        if (pnts.size())
        {
            double t = line.GetParam(pnts.back());
            if (DoubleEqual(t, 0))
            {
                m_points.erase(m_points.begin() + i + 2, m_points.end());
                ret = true;
                break;
            }
            else if (t>0 && t<1 )
            {
                _setPoint(i + 1, pnts.back(), m_points[i].GetType());
                m_points.erase(m_points.begin() + i + 2, m_points.end());
                ret = true;
                break;
            }
        }
    }
    AddRoutePoint(pnt, type);
    return ret;
}

bool VGPolyLine::CutBeging(const VGPoint &pnt, int/*= VGRoutePoint::Route*/)
{
    if (m_points.size() < 2)
        return false;

    int nCount = CountLineSeg();
    for (int i = 0; i < nCount; ++i)
    {
        if (LineSegAt(i).IsOnSeg(pnt))
        {
            m_points.erase(m_points.begin()+(i+1), m_points.end());
            if (m_points.back() != pnt)
                m_points.push_back(pnt);
            return true;
        }
    }

    return false;
}

void VGPolyLine::Clear()
{
    m_points.clear();
}

const VGPoint & VGPolyLine::First() const
{
    return m_points.front();
}

const VGPoint &VGPolyLine::Last() const
{
    return m_points.back();
}

VGPolyLine VGPolyLine::Revert() const
{
    VGPolyLine ret;
    vector<VGRoutePoint> &pnts = ret.m_points;
    pnts.resize(m_points.size());
    for (size_t i = 0; i<m_points.size();++i)
    {
        pnts[i] = m_points[m_points.size() - i - 1];
    }
    return ret;
}

void VGPolyLine::RemoveAt(int i)
{
    if (i < 0 || i >= int(m_points.size()))
        return;
    m_points.erase(m_points.begin() + i);
}

void VGPolyLine::_setPoint(int idx, const VGPoint &pnt, int type /*= 5*/)
{
    if (idx > int(m_points.size()) || idx < 0)
        m_points.push_back(VGRoutePoint(pnt, type));
    else
        m_points[idx] = VGRoutePoint(pnt, type);
}

const vector<VGRoutePoint> &VGPolyLine::RoutePoints()const
{
    return m_points;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//VGPolygon
///////////////////////////////////////////////////////////////////////////////////////////////
VGPolygon::VGPolygon()
{
}

VGPolygon::VGPolygon(const VGPolygon &oth) : m_points(oth.m_points)
{
}

double VGPolygon::Length() const
{
    double ret = 0;
    for (int i = 0; i < CountLineSeg(); i++)
    {
        ret += LineSegAt(i).Length();
    }

    return ret;
}

VGLineSeg VGPolygon::LineSegAt(int idx) const
{
    if (idx < 0 && idx > (int)m_points.size())
        return VGLineSeg();

    return VGLineSeg(m_points.at(idx), m_points.at((idx + 1) % CountLineSeg()));
}

int VGPolygon::CountLineSeg() const
{
    if (m_points.size() < 2)
        return 0;

    return m_points.size();
}

bool VGPolygon::IsValid() const
{
    return CountLineSeg() >= 3;
}

VGPolygon VGPolygon::GetExpand(double f) const
{
    if (DoubleEqual(f, 0))
        return *this;

    vector<EdgeChange> edges;
    int nCount = m_points.size();
    for (int i = 0; i < nCount; ++i) 
    {
        edges.push_back(EdgeChange(i, f));
    }

    return GetExpand(edges);
}

VGPolygon VGPolygon::GetExpand(const vector<EdgeChange> &edges) const
{
    if (edges.size()==0)
        return *this;

    vector<VGLine> linesExp;
    linesExp.resize(edges.size());
    for (const EdgeChange &ec : edges)
    {
        if (ec.index < 0 || ec.index >= (int)edges.size())
            return *this;

        VGLineSeg seg = LineSegAt(ec.index);
        VGLine line = seg.GetRectLineFrom(seg.GetMid());
        map<double, VGPoint> intsects = line.IntersectionsXY(*this);
        map<double, VGPoint>::iterator itr = intsects.begin();
        bool bF = true;
        for (; itr != intsects.end(); itr++)
        {
            if (itr->first > 0)
                break;
            if (itr->first < 0)
                bF = !bF;
        }
        linesExp[ec.index] = VGLine(seg.GetVertex(), m_points[ec.index] + line.Vertex()*(bF ? -ec.edgeShrink : ec.edgeShrink));
    }
    VGPolygon ret;
    for (size_t i = 0; i < linesExp.size(); ++i)
    {
        int nLast = (i + linesExp.size() - 1) % linesExp.size();
        vector<VGPoint> ps = linesExp[nLast].IntersectionsXY(linesExp[i]);
        if (ps.size() == 1)
            ret.m_points.push_back(ps.front());
    }
    return ret;
}

void VGPolygon::SetPoint(double x, double y, double z, int idx)
{
    SetPoint(VGPoint(x,y,z), idx);
}

void VGPolygon::SetPoint(const VGPoint &pnt, int idx/*=-1*/)
{
    if (idx < 0 || idx >= (int)m_points.size())
        m_points.push_back(pnt);
    else
        m_points[idx] = pnt;
}

void VGPolygon::Remove(int idx)
{
    if (idx < 0 || idx >= (int)m_points.size())
        return;

    m_points.erase(m_points.begin() + idx);
}

bool VGPolygon::IsContains(const VGPoint &pnt, bool bContainsOnBoard)const
{
    if (!IsValid())
        return false;

    VGLine ln(VGVertex(0.0, 1, 0), pnt);
    map<double, VGPoint> ic = ln.IntersectionsXY(*this);
    map<double, VGPoint>::iterator itr = ic.begin();
    bool bRet = false;
    for (; itr != ic.end(); ++itr)
    {
        if (DoubleEqual(itr->first, 0))
            return bContainsOnBoard;
        else if (itr->first < 0)
            bRet = !bRet;
        else
            return bRet;
    }
    return false;
}

bool VGPolygon::IsContains(const VGLineSeg &seg) const
{
    map<double, VGPoint> intersections = VGLine(seg).IntersectionsXY(*this);
    if (intersections.size() == 0)
        return false;

    map<double, VGPoint>::iterator itr = intersections.begin();
    if (itr->first > 0 || (--intersections.end())->first < 1)
        return false;

    for (; itr != intersections.end(); ++itr)
    {
        if (itr->first > 0 && itr->first < 1)
            return false;
    }

    return IsContains(seg.GetMid());
}

bool VGPolygon::IsContains(const VGPolyLine &pl, bool bOnBoard) const
{
    for (const VGPoint &pnt : pl.RoutePoints())
    {
        if (!IsContains(pnt, bOnBoard))
            return false;
    }
    return true;
}

const vector<VGPoint> &VGPolygon::Points()const
{
    return m_points;
}

void VGPolygon::GetTipPoint(const VGVertex &v, VGPoint &pntMin, VGPoint &pntMax)
{
    double fDisMax = 0;
    double fDisMin = 0;
    VGLine line(v, VGPoint());
    for (size_t i = 0; i < m_points.size(); i++)
    {
        double tmp = line.DistanceToPoint(m_points[i]);
        if (0 == i)
        {
            fDisMax = tmp;
            fDisMin = tmp;
            pntMax = m_points[i];
            pntMin = m_points[i];
        }
        else if (fDisMax < tmp)
        {
            fDisMax = tmp;
            pntMax = m_points[i];
        }
        else if (fDisMin > tmp)
        {
            fDisMin = tmp;
            pntMin = m_points[i];
        }
    }
}

void VGPolygon::Clear()
{
    m_points.clear();
}

double VGPolygon::NearestDistance(const VGPoint &pnt) const
{
    if (IsValid())
        return -1;
    double distance;
    int nCount = m_points.size();
    for (int i = 0; i < nCount; i++)
    {
        if (i==0)
        {
            distance = VGLineSeg(m_points.at(i), m_points.at((i + 1) % nCount)).NearestDistance(pnt);
            continue;
        }
        double tmp = VGLineSeg(m_points.at(i), m_points.at((i + 1) % nCount)).NearestDistance(pnt);
        if (tmp < distance)
            distance = tmp;
    }

    return distance;
}

double VGPolygon::Distance2LineSeg(const VGLineSeg &seg) const
{
    if (m_points.size() == 0)
        return 0;

    int nCount = m_points.size();
    VGLine line(seg);
    bool bRetPl = false;
    map<double, VGPoint>interSs = line.IntersectionsXY(*this);
    if (interSs.size() == 0)
        bRetPl = true;
    else if (interSs.begin()->first>=1)
        return interSs.begin()->second.DistanceTo(seg.GetEnd());
    else if ((--interSs.end())->first <= 0)
        return (--interSs.end())->second.DistanceTo(seg.GetBegin());

    double disMax = line.DistanceToPoint(m_points.front());
    double disMin = disMax;
    for (int i = 1; i < nCount; ++i)
    {
        double tmp = line.DistanceToPoint(m_points[i]);
        if (tmp > disMax)
            disMax = tmp;
        else if (tmp < disMin)
            disMin = tmp;
    }
    disMin = fabs(disMin);
    disMax = fabs(disMin);

    double ret = disMin<disMax ? disMin : disMax;
    return bRetPl ? ret : -ret;
}

vector<VGPoint> VGPolygon::GetCornerPoint(const VGPoint &pnt1, const VGPoint &pnt2, const VGPolygon &outLine)const
{
    vector<VGPoint> ret;
    int nCount = m_points.size();
    VGPoint pnt;
    if (_findRoundPnt(pnt1, pnt2, outLine, pnt))
    {
        ret.push_back(pnt);
        return ret;
    }
    if (nCount == 0)
        return ret;

    VGSinAngleArea aa(VGSinAngle(pnt2, pnt1, m_points.front()));
    int idxMin = 0;
    int idxMax = 0;
    for (int i = 1; i < nCount; i++)
    {
        int ankTmp = aa.AdjustByContains(VGSinAngle(pnt2, pnt1, m_points[i]));
        if (ankTmp == -2)
            idxMax = i;
        else if (ankTmp == -1 && pnt1.DistanceTo(m_points[i]) > pnt1.DistanceTo(m_points[idxMax]))
            idxMax = i;
        else if (ankTmp == 1 && pnt1.DistanceTo(m_points[i]) > pnt1.DistanceTo(m_points[idxMax]))
            idxMin = i;
        else if (ankTmp == 2)
            idxMin = i;
    }
    VGSinAngle maxAnk = aa.GetEnd().TranceTriAngle();
    VGSinAngle minAnk = aa.GetBegin().TranceTriAngle();
    bool bMax;
    if (minAnk < maxAnk)
        bMax=true;
    else if (maxAnk > maxAnk)
        bMax = false;
    else
        bMax = VGSinAngle(pnt1, m_points[idxMax], pnt2).TranceTriAngle() > VGSinAngle(pnt1, m_points[idxMin], pnt2).TranceTriAngle();

    const VGPoint &pntA = bMax ? m_points[idxMax] : m_points[idxMin];
    const VGPoint &pntZ = bMax ? m_points[idxMin] : m_points[idxMax];

    if (outLine.IsContains(VGLineSeg(pnt1, pntA)))
        ret.push_back(pntA);
    if (outLine.IsContains(VGLineSeg(pnt1, pntZ)))
        ret.push_back(pntZ);
    return ret;
}

bool VGPolygon::IsIntersect(const VGLine &line, bool bSkipByBord /*= true*/)const
{
    int nCount = m_points.size();
    if (nCount < 3)
        return false;

    for (int i = 0; i < nCount; ++i)
    {
        vector<VGPoint> tmp;
        VGLineSeg seg = LineSegAt(i);
        double t1 = line.DistanceToPoint(seg.GetBegin());
        double t2 = line.DistanceToPoint(seg.GetEnd());
        if (t1*t2 > 0)
            continue;
        if (DoubleEqual(t1, 0) && bSkipByBord)
            continue;
        if (DoubleEqual(t2, 0) && bSkipByBord)
            continue;

        return true;
    }
    return false;
}

VGPoint VGPolygon::GetCenter(bool bAverage/*=true*/) const
{
    VGPoint ret;
    int nCount = m_points.size();
    if (nCount == 0)
        return ret;
    if (nCount == 1)
        return m_points[0];

    double lenth = 0;
    for (int i = 0; i < nCount; i++)
    {
        double tmp = LineSegAt(i).Length();
        lenth += (bAverage ? tmp : tmp*tmp);
        ret += (m_points[i] + m_points[(i+1) % nCount])*tmp;
    }

    return ret / (2 * lenth);
}

double VGPolygon::CalculatePolygonArea() const //格林公式求面积
{
    if (m_points.size() < 3)
        return 0;

    double area = 0;
    for (size_t i = 1; i < m_points.size(); ++i)
    {
        const VGPoint &pntBeg = m_points.at(i-1);
        const VGPoint &pntEnd = m_points.at(i);
        area += (pntBeg.GetY() + pntEnd.GetY())*(pntEnd.GetX() - pntBeg.GetX()) / 2;
    }
    const VGPoint &pntBeg = m_points.back();
    const VGPoint &pntEnd = m_points.front();
    area += (pntBeg.GetY() + pntEnd.GetY())*(pntEnd.GetX() - pntBeg.GetX()) / 2;

    return area > 0 ? area : -area;
}

bool VGPolygon::IsSegOut(const VGLineSeg &seg) const
{
    map<double, VGPoint> intersections = VGLine(seg).IntersectionsXY(*this);
    if (intersections.size() == 0)
        return true;

    map<double, VGPoint>::iterator itr = intersections.begin();
    if (itr->first >= 1 || (--intersections.end())->first <= 0)
        return true;

    for (; itr!=intersections.end(); ++itr)
    {
        if (itr->first > 0 && itr->first < 1)
            return false;
    }

    return !IsContains(seg.GetMid(), false);
}

bool VGPolygon::RoundRoute(const VGPoint &pnt1, const VGPoint &pnt2, VGPolyLine &rt) const
{
    int idx1 = _getOnBoarder(pnt1);
    int idx2 = _getOnBoarder(pnt2);
    if (idx1 < 0 || idx2 < 0)
        return false;

    VGPolyLine pll1;
    VGPolyLine pll2;
    if (_getRoundRoute(pnt1, pnt2, pll1, pll2))
    {
        rt = (pll1.Length() < pll2.Length()) ? pll1 : pll2;
        return true;
    }

    return false;
}

void VGPolygon::CutSeg(const VGLineSeg &seg, vector<VGLineSeg> &segs) const
{
    if (IsContains(seg))
        return;
    map<double, VGPoint> tmp = VGLine(seg).IntersectionsXY(*this);
    if (tmp.size() < 2 || tmp.begin()->first>=1 || (--tmp.end())->first<=0)
    {
        segs.push_back(seg);
        return;
    }

    VGPoint lastPnt = seg.GetBegin();
    double tCur = 0;
    for (map<double, VGPoint>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
    {
        tCur = itr->first;
        if (tCur > 0 && tCur<1)
        {
            VGLineSeg segTmp(lastPnt, itr->second);
            if (!IsContains(segTmp))
                segs.push_back(segTmp);
            lastPnt = itr->second;
        }
        else if (tCur>=1)
        {
            VGLineSeg segTmp(lastPnt, seg.GetEnd());
            if (!IsContains(segTmp))
                segs.push_back(segTmp);
            break;
        }
    }
    if (tCur < 1)
    {
        VGLineSeg segTmp(lastPnt, seg.GetEnd());
        if (!IsContains(segTmp))
            segs.push_back(segTmp);
    }
}

bool VGPolygon::_findRoundPnt(const VGPoint &pnt1, const VGPoint &pnt2, const VGPolygon &outLine, VGPoint &pnt)const
{
    VGPolyLine pll1;
    VGPolyLine pll2;
    if (_getRoundRoute(pnt1, pnt2, pll1, pll2))
    {
        if (pll1.CountPoint() < 2 && pll2.CountPoint() < 2)
            return false;

        int idx = pll1.Length() <= pll2.Length() ? 0 : 1;
        if (idx == 0 && pll1.CountPoint() < 2)
            idx = 1;
        else if (idx == 1 && pll2.CountPoint() < 2)
            idx = 0;

        if (outLine.IsValid())
        {
            if (0 == idx && !outLine.IsContains(pll1))
                idx = 1;
            else if (1 == idx && !outLine.IsContains(pll2))
                idx = 0;

            if ((0 == idx && pll1.CountPoint() < 2) || (1 == idx && pll2.CountPoint() < 2))
                return false;
        }
        if (idx == 0)
            pnt = pll1.RoutePoints()[1];
        else
            pnt = pll2.RoutePoints()[1];
        return true;
    }
    return false;
}

int VGPolygon::_getOnBoarder(const VGPoint &pnt)const
{
    if (!IsValid())
        return -1;

    int nCount = m_points.size();
    for (int i = 0; i < nCount; ++i)
    {
        if (VGLineSeg(m_points[i], m_points[(i + 1) % nCount]).IsOnSeg(pnt))
            return i;
    }
    return -1;
}

int VGPolygon::_getLongestLineSegIndex() const
{
    if (!IsValid())
        return -1;

    int nCount = m_points.size();
    int idx = 0;
    double lengthMax = m_points[idx].DistanceSqureTo(m_points[(idx + 1) % nCount]);
    for (int i = 1; i < nCount; ++i)
    {
        double tmp = m_points[i].DistanceSqureTo(m_points[(i + 1) % nCount]);
        if (tmp > lengthMax)
        {
            lengthMax = tmp;
            idx = i;
        }
    }
    return idx;
}

bool VGPolygon::_getRoundRoute(const VGPoint &pnt1, const VGPoint &pnt2, VGPolyLine &pll1, VGPolyLine &pll2) const
{
    int idx1 = _getOnBoarder(pnt1);
    int idx2 = _getOnBoarder(pnt2);
    if (idx1 < 0 || idx2 < 0)
        return false;

    bool bPl1 = true;
    for (size_t i = 1; i <= m_points.size(); ++i)
    {
        int idx = (idx1 + i) % m_points.size();
        const VGPoint &pntTmp = m_points.at(idx);
        if (bPl1)
            pll1.AddRoutePoint(pntTmp);
        else
            pll2.AddRoutePoint(pntTmp);

        if (idx == idx2)
            bPl1 = false;
    }
    if (pll1.CountPoint() <= 0 || pll1.First() != pnt1)
        pll1.AddFrontPoint(pnt1);
    if (pll1.CountPoint() <= 0 || pll1.Last() != pnt2)
        pll1.AddRoutePoint(pnt2);

    if (pll2.CountPoint()<=0 || pll2.First() != pnt2)
        pll2.AddFrontPoint(pnt2);
    if (pll2.CountPoint() <= 0 || pll2.Last() != pnt1)
        pll2.AddRoutePoint(pnt1);

    pll2  = pll2.Revert();

    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//VGRoute
///////////////////////////////////////////////////////////////////////////////////////////////
VGRoute::VGRoute(const vector<VGRoutePoint> &points) :VGPolyLine(points)
, m_operateVoyage(0)
{
}

VGRoute::VGRoute(const VGRoute &oth) : VGPolyLine(oth)
, m_operateVoyage(oth.m_operateVoyage)
{
}

double &VGRoute::OperateVoyage()
{
    return m_operateVoyage;
}

double VGRoute::TotalVoyage() const
{
    return Length();
}

double VGRoute::TransVoyage() const
{
    return Length() - m_operateVoyage;
}

double VGRoute::GetOperateVoyage() const
{
    return m_operateVoyage;
}

const vector<VGRoutePoint> &VGRoute::GetRoutePoints()const
{
    return RoutePoints();
}
