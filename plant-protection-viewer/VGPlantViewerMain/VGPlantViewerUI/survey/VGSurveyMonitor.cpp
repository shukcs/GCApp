#include "VGSurveyMonitor.h"
#include <QTimerEvent>
#include "VGGlobalFun.h"
#include "VGApplication.h"
#include "VGLandManager.h"
#include "VGLandInformation.h"
#include "VGQXManager.h"
#include "VGBDLocation.h"
#include "VGVehicleManager.h"
#include "VGVehicleManager.h"
#include "VGVehicle.h"

VGSurveyMonitor::VGSurveyMonitor(QObject *parent) : QObject(parent), m_relativeAltitude(0)
, m_altitude(0), m_fPrecision(0), m_powerPercent(0), m_satNum(0), m_compass(0), m_idTimer(-1)
, m_countTimeout(0), m_bConnected(false), m_bPositionValid(false), m_posMode(0), m_bSysMode(false)
, m_bP900Mode(false), m_bUnitAddr(false), m_nDestAddr(0), m_dev(Dev_Baidu)
{
    connect(qvgApp->qxManager(), &VGQXManager::rtcmRead, this, &VGSurveyMonitor::onReceiveRtcm);
}

VGSurveyMonitor::~VGSurveyMonitor()
{
}

bool VGSurveyMonitor::IsPositionValid() const
{
    return m_bPositionValid;
}

void VGSurveyMonitor::SetPositionValid(bool bValid)
{
    if (m_bPositionValid == bValid)
        return;

    m_bPositionValid = bValid;
    emit positionValidChanged(bValid);
    emit posTipChanged(GetPositionTip());
}

void VGSurveyMonitor::SetLatAndLon(double lat, double lon, bool bHasGGa)
{
    SetPositionValid(true);
    m_countTimeout = 0;
    if (!bHasGGa)
        qvgApp->qxManager()->SendGGA(VGQXManager::GenerateGGa(QGeoCoordinate(lat, lon, m_altitude), m_relativeAltitude, m_satNum, m_fPrecision));

    ShareFunction::gpsCorrect(lat, lon);
    m_pos.setLatitude(lat);
    m_pos.setLongitude(lon);
    emit coordinateChanged();
}

void VGSurveyMonitor::SetRelativeAltitude(double altitude)
{
    if (m_relativeAltitude == altitude)
        return;

    m_relativeAltitude = altitude;
    emit relativeAltitudeChanged(altitude);
}

void VGSurveyMonitor::SetAltitude(double altitude)
{
    if (m_altitude == altitude)
        return;
    m_altitude = altitude;
    emit altitudeChanged(altitude);
}

void VGSurveyMonitor::SetSatNum(int n)
{
    if (m_satNum == n)
        return;
    m_satNum = n;
    emit satNumChanged(n);
}

void VGSurveyMonitor::SetPowerPercent(double percent)
{
    if (m_powerPercent == percent)
        return;

    m_powerPercent = percent;
    emit powerPercentChanged(percent);
}

void VGSurveyMonitor::SetPrecision(double f)
{
    if (m_fPrecision == f)
        return;

    m_fPrecision = f;
    emit precisionChanged(f);
}

void VGSurveyMonitor::SetConnected(bool connected, Survey_Dev dev)
{
    if (m_bConnected != connected)
    {
        m_bConnected = connected;
        emit connectedChanged(connected);
        if (connected && m_idTimer <= 0)
        {
            _initial();
            m_idTimer = startTimer(2000);
            emit posTipChanged(GetPositionTip());
        }
        else if (!connected)
        {
            if (m_idTimer > 0)
            {
                killTimer(m_idTimer);
                m_idTimer = -1;
            }
        }
    }
    if (dev != m_dev)
    {
        m_dev = m_dev;
        emit devChanged();
    }
    _readParam();
}

void VGSurveyMonitor::SetPosMode(int mode)
{
    if (m_posMode == mode)
        return;

    m_posMode = mode;
    emit posTipChanged(GetPositionTip());
}

void VGSurveyMonitor::SetFixType(int type)
{
    int acc = 0;
    int gps = 0;
    int tmp = VGGlobalFunc::GetPosTypeByMavFix(type, acc, gps);
    if (tmp > 7 || tmp < 2)
        tmp = gps;
    SetPosMode(tmp);
}

void VGSurveyMonitor::SetCompass(float f)
{
    if (!DoubleEqu(m_compass, f))
    {
        m_compass = f;
        emit compassChanged();
    }
}

void VGSurveyMonitor::DealParam(const QString &id, const QVariant &var)
{
    if (id == "SYS_MODE")
        _setSysMode(var.toInt() == 1);
    else if (id == "P900_MODE")
        _setP900Mode(var.toInt() == 1);
    else if (id == "P900_UNITADDR")
        _setUnitAddr(var.toInt() == 1);
    else if (id == "P900_DESTADDR")
        _setDestAddr(var.toInt());
}

double VGSurveyMonitor::correctValue(double v, const QString &flag)
{
    double lat = (int(v*0.01) + (v*0.01 - int(v*0.01))*100.0 / 60.0);
    if (flag == "S" || flag == "W")
        lat = -lat;

    return lat;
}

double VGSurveyMonitor::toGGAValue(double v)
{
    int degree = v;
    return degree*100 + (v-degree)*60;
}

VGSurveyMonitor::Survey_Dev VGSurveyMonitor::GetDevType() const
{
    return m_dev;
}

QString VGSurveyMonitor::GetPositionTip() const
{
    if (IsConnected())
        return VGGlobalFunc::GetPosTypeTip(m_posMode);

    return tr("N/A");
}

QGeoCoordinate VGSurveyMonitor::GetCoordinate() const
{
    return m_pos;
}

bool VGSurveyMonitor::IsConnected() const
{
    return m_bConnected;
}

double VGSurveyMonitor::RelativeAltitude() const
{
    return m_relativeAltitude;
}

double VGSurveyMonitor::Altitude() const
{
    return m_altitude;
}

int VGSurveyMonitor::GetSatNum() const
{
    return m_satNum;
}

double VGSurveyMonitor::PowerPercent() const
{
    return m_powerPercent;
}

double VGSurveyMonitor::Precision() const
{
    return m_fPrecision;
}

double VGSurveyMonitor::GetLatitude() const
{
    return m_pos.latitude();
}

double VGSurveyMonitor::GetLongitude() const
{
    return m_pos.longitude();
}

bool VGSurveyMonitor::GetSysMode() const
{
    return m_bSysMode;
}

void VGSurveyMonitor::SetSysMode(bool b)
{
    _setSysMode(b, true);
}

bool VGSurveyMonitor::GetP900Mode() const
{
    return m_bP900Mode;
}

void VGSurveyMonitor::SetP900Mode(bool b)
{
    _setP900Mode(b, true);
}

bool VGSurveyMonitor::GetUnitAddr() const
{
    return m_bUnitAddr;
}

int VGSurveyMonitor::GetDestAddr() const
{
    return m_nDestAddr;
}

void VGSurveyMonitor::SetDestAddr(int v)
{
    _setDestAddr(v, true);
}

float VGSurveyMonitor::GetCompass() const
{
    return m_compass;
}

void VGSurveyMonitor::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_idTimer)
    {
        if (++m_countTimeout >= 3)
            SetPositionValid(false);
        return;
    }
    QObject::timerEvent(event);
}

void VGSurveyMonitor::onReceiveRtcm(const QList<uint16_t> idxs, const QList<QByteArray> &rtcms)
{
    if (VGVehicle *v = qvgApp->vehicleManager()->surveyVehicle())
        v->sendRTCM(idxs, rtcms);

    if (VGBDLocation *bd = qvgApp->GetBDLocation())
        bd->SendRTCM(idxs, rtcms);
}

void VGSurveyMonitor::_initial()
{
    SetPositionValid(false);
    SetPrecision(0);
    SetSatNum(0);
    SetPowerPercent(0);
}

void VGSurveyMonitor::_setSysMode(bool b, bool setSuv)
{
    if (m_bSysMode != b)
    {
        m_bSysMode = b;
        emit sysModeChanged();
        if (setSuv)
            _setParam("SYS_MODE", b ? 1 : 0);
    }
}

void VGSurveyMonitor::_setP900Mode(bool b, bool setSuv)
{
    if (m_bP900Mode != b)
    {
        m_bP900Mode = b;
        emit p900ModeChanged();
        if (setSuv)
            _setParam("P900_MODE", b ? 1 : 0);
    }
}

void VGSurveyMonitor::_setUnitAddr(bool b)
{
    if (m_bUnitAddr != b)
    {
        m_bUnitAddr = b;
        emit unitAddrChanged();
    }
}

void VGSurveyMonitor::_setDestAddr(int v, bool setSuv)
{
    if (m_nDestAddr != v)
    {
        m_nDestAddr = v;
        emit destAddrChanged();
        if (setSuv)
            _setParam("P900_DESTADDR", v);
    }
}

void VGSurveyMonitor::_setParam(const QString &id, const QVariant &p)
{
    if (VGVehicle *v = qvgApp->vehicleManager()->surveyVehicle())
        v->prcsVehicleParameter(id, p);
}

void VGSurveyMonitor::_readParam()
{
    if (m_bConnected && m_dev==Dev_CQCDQ)
    {
        if (VGVehicle *v = qvgApp->vehicleManager()->surveyVehicle())
        {
            v->prcsVehicleParameter("SYS_MODE");
            v->prcsVehicleParameter("P900_MODE");
            v->prcsVehicleParameter("P900_UNITADDR");
            v->prcsVehicleParameter("P900_DESTADDR");
        }
    }
}
