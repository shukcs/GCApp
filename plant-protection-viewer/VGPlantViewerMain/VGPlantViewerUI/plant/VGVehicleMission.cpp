#include "VGVehicleMission.h"
#include "MissionItem.h"
#include "VGApplication.h"
#include "VGVehicle.h"
#include "VGMapManager.h"
#include "VGFlyRoute.h"
#include "VGCoordinate.h"
#include "VGPlantManager.h"
#include "VGLandPolyline.h"
#include "VGGlobalFun.h"
#include "VGLandManager.h"
#include "VGPlanningWorker.h"
#include "VGPlantInformation.h"
#include "VGSupportPolyline.h"
#include "VGToolBox.h"

#define SeqWaitCol QColor("#8FFF00")
#define SeqPassCol QColor("#FF8000")
#define SeqCurCol QColor("#00A000")

#define RefreshSupport(s, f, id) \
if (s) \
{\
    s->SetAltitude(f);\
    s->SetId(id);\
    _sendSupport(true, s);\
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VGMissionItem
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VGMissionItem::VGMissionItem(const MissionItem &item, QObject *parent) : MapAbstractItem(parent)
, m_item(new MissionItem(item)), m_color(SeqWaitCol)
{
}

VGMissionItem::VGMissionItem(MissionItem *ref, QObject *parent, int id) : MapAbstractItem(parent, id)
, m_item(ref), m_color(SeqWaitCol)
{
}

VGMissionItem::~VGMissionItem()
{
    delete m_item;
}

QColor VGMissionItem::GetColor() const
{
    return m_color;
}

void VGMissionItem::SetColor(const QColor &col)
{
    if (m_color == col)
        return;
    m_color = col;
    emit colorChanged(col);
}

MapAbstractItem::MapItemType VGMissionItem::ItemType() const
{
    return MapAbstractItem::Type_SequencePoint;
}

quint16 VGMissionItem::GetCommand() const
{
    if (m_item)
        return m_item->command();

    return MAV_CMD_ENUM_END;
}

void VGMissionItem::SetCommand(quint16 cmd)
{
    if (m_item)
        m_item->setCommand((MAV_CMD)cmd);
}

quint16 VGMissionItem::GetFrame() const
{
    if (m_item)
        return m_item->frame();

    return MAV_FRAME_ENUM_END;
}

void VGMissionItem::SetFrame(quint16 fr)
{
    if (m_item)
        m_item->setFrame((MAV_FRAME)fr);
}

int VGMissionItem::GetSequence() const
{
    if (m_item)
        return m_item->sequenceNumber()+1;

    return -1;
}

void VGMissionItem::SetSequence(int seq)
{
    if (m_item)
        return m_item->setSequenceNumber(seq);
}

QVariant VGMissionItem::GetParam1() const
{
    if (m_item)
        return m_item->param1();

    return QVariant();
}

void VGMissionItem::SetParam1(const QVariant &p)
{
    if (m_item)
        m_item->setParam1(p);
}

QVariant VGMissionItem::GetParam2() const
{
    if (m_item)
        return m_item->param2();

    return QVariant();
}

void VGMissionItem::SetParam2(const QVariant &p)
{
    if (m_item)
        m_item->setParam2(p);
}

QVariant VGMissionItem::GetParam3() const
{
    if (m_item)
        return m_item->param3();

    return QVariant();
}

void VGMissionItem::SetParam3(const QVariant &p)
{
    if (m_item)
        m_item->setParam3(p);
}

QVariant VGMissionItem::GetParam4() const
{
    if (m_item)
        return m_item->param4();

    return QVariant();
}

void VGMissionItem::SetParam4(const QVariant &p)
{
    if (m_item)
        m_item->setParam4(p);
}

QGeoCoordinate VGMissionItem::GetCoordinate() const
{
    if (m_item)
        return m_item->coordinate();

    return QGeoCoordinate();
}

void VGMissionItem::SetCoordinate(const QGeoCoordinate &c)
{
    if (m_item)
        m_item->setCoordinate(c);
    emit coordinateChanged(c);
}

void VGMissionItem::SetRelativeAtitude(double h)
{
    if (m_item)
        m_item->setParam7(h);
}

MissionItem * VGMissionItem::GetMissionItem() const
{
    return m_item;
}

bool VGMissionItem::operator==(const MapAbstractItem &item) const
{
    if (item.ItemType() != ItemType())
        return false;

    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//VGVehicleMission
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VGVehicleMission::VGVehicleMission(QObject *parent, const QList<MissionItem*> &items): MapAbstractItem(parent), m_bExecute(false), m_hasSuspend(false)
,m_msSuspend(false), m_supportEnter(NULL), m_supportReturn(NULL), m_beg(-1), m_end(-1), m_begTip(NULL), m_enterHeight(5.), m_returnHeight(5.)
, m_curRidge(0), m_segEnter(NULL), m_endTip(NULL), m_opHeight(-1), m_segReturn(NULL), m_length(-1), m_dOpVoyage(-1), m_speed(5)
, m_return(new MissionItem(0, MAV_CMD_NAV_RETURN_TO_LAUNCH, MAV_FRAME_MISSION, 0, 0, 0, 0, 0, 0, 0, false, false, MAV_MISSION_TYPE_MISSION, this))
, m_bShowSeq(false), m_bdrStruct(NULL)
{
    SetBorderColor(QColor("#00FF10"));
    SetMissionItems(items);
    SetWidth(1);
    _calculatLength();
}

VGVehicleMission::VGVehicleMission(VGFlyRoute *fr) : MapAbstractItem(fr), m_bExecute(false), m_hasSuspend(false), m_msSuspend(false), m_supportEnter(NULL)
, m_supportReturn(NULL), m_beg(fr?1:-1), m_end(-1), m_begTip(NULL), m_endTip(NULL), m_opHeight(fr ? fr->GetOperationHeight():-1), m_enterHeight(5)
, m_returnHeight(5), m_curRidge(0), m_segEnter(NULL), m_segReturn(NULL), m_speed(5)
, m_return(new MissionItem(0, MAV_CMD_NAV_RETURN_TO_LAUNCH, MAV_FRAME_MISSION, 0, 0, 0, 0, 0, 0, 0, false, false, MAV_MISSION_TYPE_MISSION, this))
, m_bShowSeq(false), m_bdrStruct(NULL)
{
    SetBorderColor(QColor("#00FF10"));
    _generateMission(fr);
    SetWidth(1);
    m_length = fr->GetTotalVoyage();
}

VGVehicleMission::~VGVehicleMission()
{
    delete m_bdrStruct;
}

bool VGVehicleMission::operator==(const MapAbstractItem &item)const
{
    if (item.ItemType() != ItemType())
        return false;

    return true;
}
MapAbstractItem::MapItemType VGVehicleMission::ItemType() const
{
    return Type_VehicleMission;
}

VGFlyRoute *VGVehicleMission::GetFlyRoute() const
{
    return qobject_cast<VGFlyRoute *>(m_parentItem);
}

void VGVehicleMission::SetCurrentExecuteItem(int idx)
{
    if (idx < 0 || idx == m_iCurExecuteItem)
        return;

    for (VGMissionItem *itr : m_missionItems)
    {
        int seq = itr->GetSequence()-1;
        if (seq == idx)
            itr->SetColor(SeqCurCol);
        else if (seq < idx)
            itr->SetColor(SeqPassCol);  
        else
            itr->SetColor(SeqWaitCol);
    }
}

void VGVehicleMission::showSquences(bool b)
{
    foreach(VGMissionItem *itr, m_missionItems)
    {
        itr->Show(b);
    }
    m_bShowSeq = b;
}

void VGVehicleMission::AtachByPlant()
{
    if (VGFlyRoute *fr = GetFlyRoute())
        fr->DetachVm(this);
}

void VGVehicleMission::SetSelected(bool b)
{
    SetVisible(b);
    MapAbstractItem::SetSelected(b);
    if(b && qvgApp->mapManager()->IsMissionPage())
    {
        if (VGFlyRoute *fr = GetFlyRoute())
            fr->SetSelected(true);
    }
}

uint32_t VGVehicleMission::CountBlock() const
{
    if (VGFlyRoute *fr = GetFlyRoute())
        return fr->CountBlock();

    return 0;
}

VGCoordinate * VGVehicleMission::GetSupportEnter() const
{
    return m_supportEnter;
}

VGCoordinate * VGVehicleMission::GetSupportReturn() const
{
    return m_supportReturn;
}

QString VGVehicleMission::GetInfo() const
{
    if (VGFlyRoute *fr = GetFlyRoute())
    {
        DescribeMap dsc;
        if (!VGGlobalFunc::initialItemDescribe(dsc, *fr, false))
            return QString();
        QByteArray ba;
        char buff[32] = {};
        QString str = dsc.find("surveyUser").value().toString();
        strcpy(buff, str.toUtf8().data());
        ba.append(buff, sizeof(buff));
        *(qint64*)buff = dsc.find("surveyTime").value().value<qint64>();
        ba.append(buff, sizeof(qint64));
        *(qint64*)buff = dsc.find("editTime").value().value<qint64>();
        ba.append(buff, sizeof(qint64));
        *(qint64*)buff = dsc.find("routeTime").value().value<qint64>();
        ba.append(buff, sizeof(qint64));
        *(int*)buff = m_beg;
        ba.append(buff, 4);
        *(int*)buff = m_end;
        ba.append(buff, 4);
        return QString(ba.toBase64().data());
    }
    return QString();
}

void VGVehicleMission::addSupport(const QGeoCoordinate &coor, bool bEnter, bool bRcv)
{
    if (!bRcv && (!_checkSupport(coor)||!_checkSupportHeight(bEnter ? m_enterHeight : m_returnHeight)))
        return;

    VGCoordinate *&support = bEnter ? m_supportEnter : m_supportReturn;
    int id = bEnter ? (bRcv ? VGCoordinate::SupportEnter : VGCoordinate::SetSupportEnter)
        : (bRcv ? VGCoordinate::SupportReturn : VGCoordinate::SetSupportReturn);

    double alt = bRcv ? coor.altitude() : (bEnter ? m_enterHeight : m_returnHeight);

    if (!support)
        support = new VGCoordinate(coor, id, this);
    else
        support->SetCoordinate(coor);

    support->SetId(id);
    support->SetAltitude(alt);
    support->Show(GetVisible());

    if (bEnter)
        emit hasEnterSupChanged(id == VGCoordinate::SupportEnter);
    else
        emit hasReturnSupChanged(id == VGCoordinate::SupportReturn);

    if (!bRcv)
        _sendSupport(bEnter, support);

    _supportSegChanged(bEnter);
}

VGLandBoundary *VGVehicleMission::GetBelongBoundary() const
{
    if (VGFlyRoute *fr = GetFlyRoute())
        return fr->GetBelongedBoundary();
    return NULL;
}

float VGVehicleMission::MaxWorkAlt() const
{
    return m_returnHeight > m_enterHeight ? m_returnHeight : m_enterHeight;
}

void VGVehicleMission::clearSupport(bool bEnter, bool bRcv)
{
    VGCoordinate *&support = bEnter ? m_supportEnter : m_supportReturn;
    VGSupportPolyline *seg = bEnter ? m_segEnter : m_segReturn;
    int id = bEnter ? VGCoordinate::SetSupportEnter : VGCoordinate::SetSupportReturn;

    if (bRcv && (!support || support->GetId()==id))
        return;

    support->deleteLater();
    support = NULL;
    if (seg)
        seg->SetValid(false);

    if(bEnter)
        emit hasEnterSupChanged(false);
    else
        emit hasReturnSupChanged(false);

    if (!bRcv)
        _sendSupport(bEnter, NULL);
}

VGVehicleMission *VGVehicleMission::fromInfo(const QString &info)
{
    QByteArray ba = QByteArray::fromBase64(info.toUtf8());
    if(ba.size() < 64)
        return NULL;

    DescribeMap dsc;
    dsc["surveyUser"] = QString::fromUtf8(ba.mid(0, 32));
    dsc["surveyTime"] = QVariant::fromValue<qint64>(*(qint64*)ba.mid(32, 8).data());
    dsc["editTime"] = QVariant::fromValue<qint64>(*(qint64*)ba.mid(40, 8).data());
    dsc["routeTime"] = QVariant::fromValue<qint64>(*(qint64*)ba.mid(48, 8).data());
    VGFlyRoute *rtTmp = NULL;
    for (VGFlyRoute *rt : qvgApp->landManager()->FlyRoutes())
    {
        DescribeMap tmp;
        VGGlobalFunc::initialItemDescribe(tmp, *rt, false);
        if (MapAbstractItem::IsContain(tmp, dsc))
        {
            rtTmp = rt;
            break;
        }
    }
    VGVehicleMission *ret = rtTmp ? rtTmp->beginOperation() : NULL;
    if (ret)
    {      
        rtTmp->DetachVm(ret);
        int nTmp = *(int*)ba.mid(56, 4).data();
        ret->SetBegin(nTmp);
        nTmp = *(int*)ba.mid(60, 4).data();
        ret->SetEnd(nTmp);
    }
    return ret;
}

bool VGVehicleMission::canSync2Vehicle() const
{
    if (m_missionItems.count() <= 0)
        return false;

    VGToolBox *tb = qvgApp->toolbox();
    if (!tb)
        return false;
    if (VGVehicle *vk = tb->activeVehicle())
        return vk->isMissionIdle();

    return false;
}

void VGVehicleMission::sync2Vehicle()
{
    if (m_missionItems.count()<=0 || m_beg<0)
        return;

    if (VGPlantManager *mgr = qvgApp->plantManager())
        mgr->syncPlanFlight(this);
}

int VGVehicleMission::GetBegin() const
{
    return m_beg;
}

void VGVehicleMission::SetBegin(int beg)
{
    if (m_beg == beg || m_msSuspend)
        return;

    m_beg = beg;
    emit beginChanged(beg);
    _generateMission(GetFlyRoute());
    if (m_segEnter && m_begTip)
        m_segEnter->SetBegin(m_begTip->GetCoordinate());
}

int VGVehicleMission::GetEnd() const
{
    return m_end;
}

double VGVehicleMission::GetOpVoyage() const
{
    return m_dOpVoyage;
}

QString VGVehicleMission::GetFlyRouteID() const
{
    if (auto fr = GetFlyRoute())
        return fr->GetActId();

    return QString();
}

void VGVehicleMission::SetSuspend(int ridge, const QGeoCoordinate &c)
{
    bool bHas = 0<=ridge && (!GetFlyRoute() || ridge<GetFlyRoute()->CountRidges());
    if (bHas != m_hasSuspend)
    {
        m_hasSuspend = bHas;
        emit hasSuspendChanged();
    }
    m_curRidge = ridge;
    if (bHas)
        m_suspend = c;
}

void VGVehicleMission::UpdateMissionItem()
{
    VGFlyRoute *fr = GetFlyRoute();
    double rRoit = qvgApp->plantManager()->GetRoitRadius(fr ? fr->GetSprinkleWidth() : 1);
    double tmRoit = qvgApp->plantManager()->GetRoitTime();
    for (VGMissionItem *itr : m_missionItems)
    {
        int id = itr->GetId();
        if (id>VGCoordinate::RouteBegin && id<VGCoordinate::RouteEnd)
        {
            itr->SetParam1(tmRoit);
            itr->SetParam2(rRoit);
        }
    }
}

void VGVehicleMission::SetEnd(int end)
{
    if (m_end == end)
        return;

    m_end = end;
    emit endChanged(end);
    _generateMission(GetFlyRoute());
    if (m_segReturn && m_endTip)
        m_segReturn->SetBegin(m_endTip->GetCoordinate());
}

double VGVehicleMission::GetOpHeight() const
{
    return m_opHeight;
}

void VGVehicleMission::SetOpHeight(double f)
{
    if (m_opHeight == f)
        return;

    if (f > m_returnHeight || f > m_returnHeight)
    {
        qvgApp->SetQmlTip(tr("Operation height mast be lower than support height"), true);//"作业高度不能大于辅助点高度！"
        return;
    }

    m_opHeight = f;
    emit opHeightChanged(f);
    if (VGFlyRoute *fr = GetFlyRoute())
        fr->SetOperationHeight(f);

    for (VGMissionItem *itr : m_missionItems)
    {
        int id = itr->GetId();
        if (id >= VGCoordinate::RouteBegin && id<=VGCoordinate::RouteEnd)
            itr->SetRelativeAtitude(f);
    }
}

float VGVehicleMission::GetMedPerAcre() const
{
    if (VGFlyRoute *fr = GetFlyRoute())
        return fr->GetMedPerAcre();

    return -1;
}

void VGVehicleMission::SetMedPerAcre(float f)
{
    if (VGFlyRoute *fr = GetFlyRoute())
    {
        if (fr->GetMedPerAcre() == f)
            return;

        fr->SetMedPerAcre(f);
        emit medPerAChanged(f);
    }
}

bool VGVehicleMission::HasEnterSup() const
{
    if(m_supportEnter)
        return m_supportEnter->GetId() == VGCoordinate::SupportEnter;

    return false;
}

bool VGVehicleMission::HasReturnSup() const
{
    if(m_supportReturn)
        return m_supportReturn->GetId() == VGCoordinate::SupportReturn;

    return false;
}

double VGVehicleMission::GetEnterHeight() const
{
    return m_enterHeight;
}

void VGVehicleMission::SetEnterHeight(double f)
{
    if (m_enterHeight == f || !_checkSupportHeight(f))
        return;

    RefreshSupport(m_supportEnter, f, VGCoordinate::SetSupportEnter);
    m_enterHeight = f;
    emit enterHeightChanged(f);
}

double VGVehicleMission::GetReturnHeight() const
{
    return m_returnHeight;
}

void VGVehicleMission::SetReturnHeight(double f)
{
    if (m_returnHeight == f || !_checkSupportHeight(f))
        return;

    RefreshSupport(m_supportEnter, f, VGCoordinate::SetSupportReturn);
    m_returnHeight = f;
    emit returnHeightChanged(f);
}

double VGVehicleMission::GetLength() const
{
    return m_length;
}

VGLandInformation *VGVehicleMission::GetLandInformation() const
{
    if (VGFlyRoute *rt = GetFlyRoute())
        return rt->GetBelongedLand();
    return NULL;
}

void VGVehicleMission::processSaveReslt(const DescribeMap &result)
{
    if(VGFlyRoute *fr = GetFlyRoute())
    {
        DescribeMap::const_iterator itr = result.find("ActualId");
        if (itr != result.end())
        {
            QString actId = itr.value().toString();
            fr->SetActId(actId);
        }
    }
}

int VGVehicleMission::CountItems() const
{
    int ret = m_missionItems.count();
    if (ret == 0 || m_return)
        return 0;

    return ret + 1;
}

void VGVehicleMission::SetSpeed(double f)
{
    if (m_speed == f)
        return;
    m_speed = f;
    emit speedChanged();
}

int VGVehicleMission::GetCountOperationLine() const
{
    if (VGFlyRoute *fr = GetFlyRoute())
        return fr->CountRidges();

    return 0;
}

bool VGVehicleMission::HasSuspend() const
{
    return m_hasSuspend;
}

bool VGVehicleMission::IsMissionSuspend() const
{
    return m_msSuspend;
}

void VGVehicleMission::SetMissionSuspend(bool b)
{
    if (!m_hasSuspend || m_msSuspend == b)
        return;

    m_msSuspend = b;
    emit msSuspendChanged();

    if (m_curRidge > 0)
    {
        m_beg = b ? m_curRidge+1 : 1;
        emit beginChanged(m_beg);
        _generateMission(GetFlyRoute());
        if (m_segEnter && m_begTip)
            m_segEnter->SetBegin(m_begTip->GetCoordinate());
    }
}

double VGVehicleMission::GetSpeed() const
{
    return m_speed;
}

void VGVehicleMission::onItemDestoyed(QObject *obj)
{
    if (VGMissionItem *item = (VGMissionItem *)obj)
    {
        int idx = m_missionItems.indexOf(item);
        if (idx >= 0)
        {
            m_missionItems.removeAt(idx);
            m_path.removeAt(idx);
            _adjustSequence(idx);
        }
    }
}

void VGVehicleMission::_generateMission(VGFlyRoute *fr)
{
    VGLandPolyline *ol = fr->allMissionRoute();
    if (ol && ol->coordinateCount()>0 && m_beg)
    {
        if (m_end <= 0)
            m_end = fr->CountRidges();

        int idx_beg = _getLinePntIndex(m_beg);
        int idx_end = _getLinePntIndex(m_end, false);
        if (idx_beg < idx_end)
        {
            _genMissionItem(ol->GetCoordinates().mid(idx_beg, idx_end-idx_beg+1));
            _calculatLength();
            _genTipPoint(ol, idx_beg, idx_end);
        }
    }
}

void VGVehicleMission::_genMissionItem(const QList<VGCoordinate*> &coors)
{
    if (coors.size() < 2)
        return;

    qDeleteAll(m_missionItems);
    m_missionItems.clear();
    m_path.clear();

    VGFlyRoute *fr = GetFlyRoute();
    double tmRoit = qvgApp->plantManager()->GetRoitTime();
    double rRoit = qvgApp->plantManager()->GetRoitRadius(fr ? fr->GetSprinkleWidth() : 1);
    float angle = fr ? fr->GetAngle() : VGGlobalFunc::checkAngle(coors);   //机头方向
    int seqNum = m_missionItems.count();

    float bSprink = 0;//是否喷药bInital
    QGeoCoordinate c;
    if (m_msSuspend && m_suspend.isValid())
        c = m_suspend;

    if (c.isValid())
    {
        MissionItem* item = new MissionItem(seqNum++,
            MAV_CMD_NAV_WAYPOINT,                   //航迹点
            MAV_FRAME_GLOBAL_RELATIVE_ALT,          //相对高度
            .1, .5, bSprink, angle,
            c.latitude(), c.longitude(), GetOpHeight(),
            true, false);
        m_missionItems << new VGMissionItem(item, this, VGCoordinate::RouteBegin);
        m_path << QVariant::fromValue(c);
        bSprink = 1;
    }

    for (VGCoordinate *coor : coors)
    {
        VGMapManager::CalcBoundaryByCoor(m_boundarys, coor->GetCoordinate());
        MissionItem* item = new MissionItem(seqNum++,
            MAV_CMD_NAV_WAYPOINT,                   //航迹点
            MAV_FRAME_GLOBAL_RELATIVE_ALT,          //相对高度
            tmRoit, rRoit, bSprink, angle,
            coor->GetLatitude(), coor->GetLongitude(), GetOpHeight(),
            true, false);
        m_missionItems << new VGMissionItem(item, this, coor->GetId());
        m_path << QVariant::fromValue(coor->GetCoordinate());
        bSprink = VGGlobalFunc::checkSprink(*coor);
    }

    c = coors.last()->GetCoordinate();
    if (c.isValid())
    {
        MissionItem* item = new MissionItem(seqNum++,
            MAV_CMD_NAV_WAYPOINT,                   //航迹点
            MAV_FRAME_GLOBAL_RELATIVE_ALT,          //相对高度
            .1, .5, bSprink, angle,
            c.latitude(), c.longitude(), GetOpHeight(),
            true, false);
        m_missionItems << new VGMissionItem(item, this, VGCoordinate::RouteEnd);
        m_path << QVariant::fromValue(c);
    }
    emit pathChanged(m_path);
    emit countItemChanged();
}

int VGVehicleMission::_getLinePntIndex(int nline, bool bStart /*= true*/)
{

    VGLandPolyline *ol = NULL;
    if (VGFlyRoute *fr = GetFlyRoute())
        ol = fr->allMissionRoute();

    if (!ol)
        return -1;

    int ret = 0;
    foreach(VGCoordinate *itr, ol->GetCoordinates())
    {
        int id = itr->GetId();
        bool bOp = VGCoordinate::RouteBegin == id || VGCoordinate::RouteOperate==id;
        if (bOp)
            nline--;

        if (nline == 0 && (bStart || !bOp))
            return ret;

        ret++;
    }
    return -1;
}

void VGVehicleMission::_adjustSequence(int beg)
{
    if (beg >= m_missionItems.count() || beg<0)
        return;

    QList<VGMissionItem*>::iterator itr = m_missionItems.begin() + beg;
    for (int i = beg; i<m_missionItems.count(); ++itr)
    {
        (*itr)->SetSequence(i++);
    }
}

void VGVehicleMission::_supportSegChanged(bool bEnter)
{
    VGCoordinate *tip = bEnter ? m_begTip : m_endTip;
    VGCoordinate *support = bEnter ? m_supportEnter : m_supportReturn;
    if (!tip || !support)
        return;

    VGSupportPolyline *&seg = bEnter ? m_segEnter : m_segReturn;
    if (!seg)
    {
        seg = new VGSupportPolyline(this);
        seg->SetBorderColor(bEnter ? Qt::black : Qt::red);
        seg->AddPoint(tip->GetCoordinate());
        seg->AddPoint(support->GetCoordinate());
    }
    else
    {
        seg->SetEnd(support->GetCoordinate());
    }
    seg->SetValid(true);
    seg->Show(GetVisible());
}

void VGVehicleMission::_calculatLength()
{
    VGMissionItem *itmLast = NULL;
    m_length = 0;
    m_dOpVoyage = 0;
    for (VGMissionItem *itr : m_missionItems)
    {
        if (itmLast)
            m_length += itr->GetCoordinate().distanceTo(itmLast->GetCoordinate());

        if (itmLast && DoubleEqu(itr->GetParam3().toFloat(), 1))
            m_dOpVoyage += itr->GetCoordinate().distanceTo(itmLast->GetCoordinate());
        itmLast = itr;
    }
    m_dOpVoyage /= 1000;
    m_length /= 1000;
    emit lengthChanged(m_length);
}

void VGVehicleMission::_genTipPoint(VGLandPolyline *ol, int beg, int end)
{
    if (0<=beg && beg<end && end<ol->coordinateCount())
    {
        if (!m_begTip)
            m_begTip = new VGCoordinate(QGeoCoordinate(), VGCoordinate::Route_Start, this);

        if (m_msSuspend && m_suspend.isValid())
            m_begTip->SetCoordinate(m_suspend);
        else
            m_begTip->SetCoordinate(ol->GetCoordinates().at(beg)->GetCoordinate());
        if (!m_endTip)
            m_endTip = new VGCoordinate(ol->GetCoordinates().at(end)->GetCoordinate(), VGCoordinate::Route_End, this);
        else
            m_endTip->SetCoordinate(ol->GetCoordinates().at(end)->GetCoordinate());
    }
    if (m_begTip)
        m_begTip->Show(GetVisible());

    if (m_endTip)
        m_endTip->Show(GetVisible());
}

void VGVehicleMission::_sendSupport(bool bEnter, VGCoordinate *pnt)
{
    if (VGPlantInformation *info = qvgApp->plantManager()->GetCurrentPlant())
        info->SendSupport(bEnter, pnt);
}

bool VGVehicleMission::_checkSupport(const QGeoCoordinate &c)
{
    if (!m_bdrStruct)
    {
        VGFlyRoute *fr = GetFlyRoute();
        if (!fr)
        {
            qvgApp->SetQmlTip(tr("Land data error!"), true);//"地块数据出错！"
            return false;
        }
        m_bdrStruct = new VGPlanningWorker();
        VGPlanningWorker::GenMissionBoundary(fr, m_bdrStruct, c);
    }
    VGPlanningWorker::PointPosType tp = m_bdrStruct->GetCoordinatePos(c);
    if (tp == VGPlanningWorker::OK)
        return true;

    if (tp == VGPlanningWorker::InBlock)
        qvgApp->SetQmlTip(tr("Support can not be in block!"), true);//"辅助点不能设置在障碍物中！"
    if (tp == VGPlanningWorker::OutBoundary)
        qvgApp->SetQmlTip(tr("Support can not be out boundary!"), true);//"辅助点不能设置在地块外！"
    return false;
}

bool VGVehicleMission::_checkSupportHeight(double f)
{
    if (m_opHeight > f)
    {
        qvgApp->SetQmlTip(tr("Support height mast higher than operate height!"), true);//"辅助点高度不能小于作业高度！"
        return false;
    }
    return true;
}

void VGVehicleMission::SetMissionItems(const QList<MissionItem*> &items, bool bRef)
{
    qDeleteAll(m_missionItems);
    m_missionItems.clear();
    m_path.clear();
    m_boundarys.clear();
    foreach(MissionItem *itr, items)
    {
        VGMissionItem *item = bRef ? new VGMissionItem(itr, this) : new VGMissionItem(*itr, this);
        m_missionItems << item;
        item->Show(GetVisible() && m_beg<=0);
        if (itr->coordinate().isValid())
        {
            QGeoCoordinate coor = itr->coordinate();
            m_path << QVariant::fromValue(coor);
            VGMapManager::CalcBoundaryByCoor(m_boundarys, coor);
        }
    }
	if (m_path.count())
		SetExecutable(true);
    
    emit pathChanged(m_path);
    Show(true);
}

const QList<VGMissionItem*> &VGVehicleMission::VGVehicleMissionItems()const
{
    return m_missionItems;
}

QList<MissionItem*> VGVehicleMission::MissionItems()const
{
    QList<MissionItem*> ret;
    foreach(VGMissionItem *itr, m_missionItems)
    {
        ret << itr->GetMissionItem();
    }

    if (ret.count() > 0)
    {
        MissionItem *last = ret.last();
        if (m_return && last)
        {
            m_return->setSequenceNumber(last->sequenceNumber() + 1);
            ret << m_return;
        }
    }
    return ret;
}

QList<MissionItem*> VGVehicleMission::BoundaryItems() const
{
    if (VGFlyRoute *rt = GetFlyRoute())
        return rt->GetSafeBoudaryItems();

    return QList<MissionItem*>();
}

void VGVehicleMission::showContent(bool b)
{
    if (m_supportEnter)
        m_supportEnter->Show(b);
    if (m_supportReturn)
        m_supportReturn->Show(b);

    if (m_begTip)
        m_begTip->Show(b);
    if (m_endTip)
        m_endTip->Show(b);
    if (m_segEnter && m_segEnter->IsValide())
        m_segEnter->Show(b);
    if (m_segReturn && m_segReturn->IsValide())
        m_segReturn->Show(b);

    if (m_bShowSeq)
        showSquences(b);

    _show(b);
}

QVariantList VGVehicleMission::GetPath() const
{
    return m_path;
}

void VGVehicleMission::Show(bool b)
{
    MapAbstractItem::Show(b);
    if (b && GetFlyRoute()==NULL)
        Monitor();
}

void VGVehicleMission::SetExecutable(bool b)
{
	m_bExecute = b;
}

bool VGVehicleMission::GetExecutable() const
{
	return m_bExecute;
}

void VGVehicleMission::Monitor()
{
    VGMapManager *mgr = qvgApp->mapManager();
    if (mgr && m_boundarys.count())
        mgr->SetMissionBoundary(m_boundarys);
}
