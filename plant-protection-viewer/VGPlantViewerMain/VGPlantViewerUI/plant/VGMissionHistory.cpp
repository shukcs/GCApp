#include "VGMissionHistory.h"
#include "VGMissionRecord.h"
#include "VGNetManager.h"
#include "QmlObjectListModel.h"
#include "das.pb.h"
#include "VGApplication.h"

#define PtbClassName(_cls) QString(_cls::descriptor()->full_name().c_str())
using namespace das::proto;

VGMissionHistory::VGMissionHistory(QObject *p/*=NULL*/) : QObject(p)
, m_records(new QmlObjectListModel(this)), m_areaAll(-1)
, m_valid(false), m_seq(1)
{
}

QmlObjectListModel *VGMissionHistory::GetHistories() const
{
    return m_records;
}

void VGMissionHistory::InitialNet(VGNetManager *nm)
{
    if (nm)
        connect(nm, &VGNetManager::receiveMissionInfo, this, &VGMissionHistory::OnRecvRecord, Qt::UniqueConnection);
}

void VGMissionHistory::OnRecvRecord(const google::protobuf::Message &msg)
{
    QString name = QString(msg.GetTypeName().c_str());
    if (name == PtbClassName(AckUavMissionAcreage))
    {
        m_areaAll = ((AckUavMissionAcreage*)&msg)->acreage();
        m_msTip = tr("Mission acreage: ") + QString::number(m_areaAll);
        emit allAreaChanged();
        emit msTipChanged();
        if(fabs(m_areaAll-0) > 1e-7)
        {
            m_valid = true;
            emit validChanged();
        }
    }
    else if(name == PtbClassName(AckUavMission))
    {
        while (m_records->count())
        {
            delete m_records->removeAt(0);
        }
        const AckUavMission &um = *(AckUavMission*)&msg;
        for (int i = 0; i<um.routes_size(); ++i)
        {
            const UavRoute &ur = um.routes(i);
            VGMissionRecord *mr = new VGMissionRecord(ur.uav().c_str(),
                                  ur.plan().c_str(),ur.land().c_str(),
                                  ur.user().c_str(),ur.optm(), ur.acreage(),
                                  ur.beg(),ur.end(), QGeoCoordinate(ur.continiulat(), ur.continiulon()));
            m_records->append(mr);
        }
    }
}

QString VGMissionHistory::GetBeginTime() const
{
    if (!m_begTime.isValid())
        return QDateTime::currentDateTime().addDays(-30).toString("yyyy-MM-dd");

    return m_begTime.toString("yyyy-MM-dd");
}

void VGMissionHistory::SetBeginTime(const QString &t)
{
    if (!m_endTime.isValid())
        m_endTime = QDateTime::currentDateTime();
    QDateTime tm = QDateTime::fromString(t+" 00:00:00", "yyyy-MM-dd hh:mm:ss");
    if (tm > m_endTime)
        return;

    m_begTime = tm;
    emit begTimeChanged();
}

QString VGMissionHistory::GetEndinTime() const
{
    if (!m_endTime.isValid())
        return QDateTime::currentDateTime().toString("yyyy-MM-dd");

    return m_endTime.toString("yyyy-MM-dd");
}

void VGMissionHistory::SetEndinTime(const QString &t)
{
    QDateTime tm = QDateTime::fromString(t+ " 23:59:59", "yyyy-MM-dd hh:mm:ss");
    if (tm < m_begTime)
        return;
    m_endTime = tm;
    emit endTimeChanged();
}

float VGMissionHistory::GetAllArea() const
{
    return m_areaAll;
}

QString VGMissionHistory::GetUav() const
{
    return m_uav;
}

void VGMissionHistory::SetUav(const QString &uav)
{
    m_uav = uav;
    emit uavChanged();
}

bool VGMissionHistory::IsValid() const
{
    return m_valid;
}

QString VGMissionHistory::GetMsTip() const
{
    return m_msTip;
}

void VGMissionHistory::reqMissionArea()
{
    if (auto nm = qvgApp->netManager())
    {
        m_msTip = QString();
        emit msTipChanged();
        while (auto itr = m_records->removeAt(0))
        {
            itr->deleteLater();
        }
        RequestUavMissionAcreage req;
        req.set_seqno(m_seq++);
        if (!m_uav.isEmpty())
            req.set_uav(m_uav.toUtf8().data());

        QDateTime tmp = m_begTime.isValid() ? m_begTime : QDateTime::currentDateTime().addDays(-30);
        req.set_beg(tmp.toMSecsSinceEpoch());
        tmp = m_endTime.isValid() ? m_endTime : QDateTime::currentDateTime();
        req.set_end(tmp.toMSecsSinceEpoch());
        nm->SendNetWork(req);
    }
}

void VGMissionHistory::reqDetail()
{
    if (!m_valid)
        return;

    m_valid = false;
    emit validChanged();
    if (auto nm = qvgApp->netManager())
    {
        RequestUavMission req;
        req.set_seqno(m_seq++);
        if (!m_uav.isEmpty())
            req.set_uav(m_uav.toUtf8().data());
        QDateTime tmp = m_begTime.isValid() ? m_begTime : QDateTime::currentDateTime().addDays(-30);
        req.set_beg(tmp.toMSecsSinceEpoch());
        tmp = m_endTime.isValid() ? m_endTime : QDateTime::currentDateTime();
        req.set_end(tmp.toMSecsSinceEpoch());

        nm->SendNetWork(req);
    }
}