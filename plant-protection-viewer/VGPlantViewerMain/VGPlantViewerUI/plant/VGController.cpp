#include <mavlink.h>
#include "VGController.h"
#include "VGPlantManager.h"
#include "VGPlantInformation.h"
#include "VGApplication.h"
#include "ParametersLimit.h"
#include "ParameterLimit.h"
#include <math.h>

static bool s_bInitRCParms = false;
static QStringList s_strLsRCParms;
static uint16_t s_trimCount[VGController::RC_CHNS] = { 0 };
static uint16_t s_trimAverage[VGController::RC_CHNS] = { 0 };
static const QStringList &getInitRCParms()
{
    if (s_bInitRCParms)
        return s_strLsRCParms;

    s_bInitRCParms = true;
    s_strLsRCParms += qvgApp->parameters()->GetAllInSameGroup("RC1_MIN");
    s_strLsRCParms += qvgApp->parameters()->GetAllInSameGroup("RC2_MIN");
    s_strLsRCParms += qvgApp->parameters()->GetAllInSameGroup("RC3_MIN");
    s_strLsRCParms += qvgApp->parameters()->GetAllInSameGroup("RC4_MIN");
    return s_strLsRCParms;
}
////////////////////////////////////////////////////////////////////////////////
//VGController
////////////////////////////////////////////////////////////////////////////////
VGController::VGController(QObject *p) :QObject(p)
, m_curUav(NULL), m_calibrat(Step_None)
{
    intitParam();
}

VGController::StepCalibrat VGController::GetCalibrat() const
{
    return m_calibrat;
}

int VGController::GetRoll() const
{
    return getChannelPar(RC_Roll);
}

int VGController::GetPitch() const
{
    return getChannelPar(RC_Pitch);
}

int VGController::GetThrottle() const
{
    return getChannelPar(RC_Throttle);
}

int VGController::GetYaw() const
{
    return getChannelPar(RC_Yaw);
}

QString VGController::GetRollStr() const
{
    return qvgApp->parameters()->GetGroup("RC1_MIN");
}

QString VGController::GetPitchStr() const
{
    return qvgApp->parameters()->GetGroup("RC2_MIN");
}

QString VGController::GetThrottleStr() const
{
    return qvgApp->parameters()->GetGroup("RC3_MIN");
}

QString VGController::GetYawStr() const
{
    return qvgApp->parameters()->GetGroup("RC4_MIN");
}

QString VGController::GetStepTip() const
{
    static QStringList s_strTips;
    if (s_strTips.isEmpty())
    {
        s_strTips << tr("Please move lever,active")//"请打动遥控杆，激活"
            << tr("Please move %1,%2,%3,%4 to the mid,wait 5 s")//"请把遥控器%1，%2，%3，%4打到对应中间，等待5秒"
            .arg(GetRollStr()).arg(GetPitchStr()).arg(GetYawStr()).arg(GetThrottleStr())
            << tr("Please move %1,%2,%3,%4 to the min and max,wait 5 s")//"请把遥控器%1，%2，%3，%4打到最大及最小，分别等待5秒"
            .arg(GetRollStr()).arg(GetPitchStr()).arg(GetYawStr()).arg(GetThrottleStr())
            << tr("Controller correct finish");//L"遥控器校准完成");
    }
    if (m_calibrat <= Step_Finish && m_calibrat >= Step_None)
        return s_strTips.at(m_calibrat);

    return QString();
}

void VGController::SetCurUav(VGPlantInformation *uav)
{
    if (m_curUav == uav)
        return;

    m_curUav = uav;
    if (m_curUav)
        intitParam();
}

float VGController::GetCalibratValue(uint32_t ch, int mask) const
{
    if (ch < RC_CHNS)
    {
        if (mask > 0)
            return m_maxCalibrat[ch];

        if (mask < 0)
            return m_minCalibrat[ch];

        if (ch == RC_Throttle)
            return m_minCalibrat[RC_Throttle];

        return (m_trimCalibrat[ch]>800 && m_trimCalibrat[ch]<2200)?m_trimCalibrat[ch]:1500;
    }
    return 0;
}

QString VGController::GetCalibratParamID(uint32_t ch, int mask) const
{
    int n = (mask > 0 ? 2 : (mask < 0 ? 0 : 1)) + 3 * ch;
    if (getInitRCParms().count() > n)
        return getInitRCParms().at(n);

    return QString();
}

void VGController::OnRecvRCChannels(const QString &id, const mavlink_rc_channels_t &rc)
{
    if (m_curUav && m_curUav->planeId() == id)
    {
        calibrat(RC_Roll, rc.chan1_raw);
        calibrat(RC_Pitch, rc.chan2_raw);
        calibrat(RC_Throttle, rc.chan3_raw);
        calibrat(RC_Yaw, rc.chan4_raw);
    }
}

void VGController::OnParamChanged(const QString &id)
{
    if (m_curUav && m_curUav==qvgApp->plantManager()->GetCurrentPlant())
    {
        int idx = getInitRCParms().indexOf(id);
        if (idx>=0)
        {
            uint32_t value = m_curUav->GetParamValue(id).toFloat();
            if (changeParams(idx / 3, idx % 3 - 1, value))
                notifyChange(idx / 3);
        }
    }
}

void VGController::SetCalibrat(StepCalibrat st)
{
    if (m_calibrat != st)
    {
        m_calibrat = st;
        if (Step_Trim == st)
            initCalibrat();
        emit stepChanged();
        emit stepTipChanged();
        if (Step_Finish == st)
            qvgApp->plantManager()->ControllerCalibrat();
    }
}

void VGController::intitParam()
{
    initCalibrat();
    for (int i = 0; i < RC_CHNS; ++i)
    {
        m_min[i] = RC_MIN;
        m_max[i] = RC_MAX;
        m_trim[i] = RC_TRIM;
        m_curVal[i] = RC_TRIM;
    }
    if (m_curUav)
    {
        for (const QString &itr : getInitRCParms())
        {
            OnParamChanged(itr);
        }
    }
}

void VGController::initCalibrat()
{
    for (int i = 0; i < RC_CHNS; ++i)
    {
        m_minCalibrat[i] = 0;
        m_maxCalibrat[i] = 0;
        m_trimCalibrat[i] = 0;
        s_trimCount[i] = 0;
        s_trimAverage[i] = 0;
    }
}

int VGController::getChannelPar(uint32_t ch)const
{
    if (ch >= RC_CHNS)
        return 0;

    if (m_curVal[ch] >= RC_MIN && m_curVal[ch] <= RC_MAX)
    {
        int n = m_curVal[ch] - m_trim[ch];
        if (m_curVal[ch] > m_trim[ch])
            return (100 * n) / (RC_MAX - m_trim[ch]);

        return (100 * n) / (m_trim[ch]-RC_MIN);
    }
    return 0;
}

bool VGController::changeParams(uint32_t ch, int mask, uint32_t val)
{
    if (ch >= RC_CHNS || val > RC_MAX ||val< RC_MIN)
        return false;

    if (mask < 0)
    {
        if (m_min[ch] != val)
        {
            m_min[ch] = val;
            return true;
        }
    }
    else if (mask > 0)
    {
        if (m_max[ch] != val)
        {
            m_max[ch] = val;
            return true;
        }
    }
    else
    {
        if (m_trim[ch] != val && ch!=RC_Throttle)
        {
            m_trim[ch] = val;
            return true;
        }
    }
    return false;
}

void VGController::notifyChange(uint32_t ch)
{
    switch (ch)
    {
    case RC_Roll:
        emit rollChanged();
        break;
    case RC_Pitch:
        emit pitchChanged();
        break;
    case RC_Throttle:
        emit throttleChanged();
        break;
    case RC_Yaw:
        emit yawChanged();
        break;
    default:
        break;
    }
}

void VGController::calibrat(uint32_t ch, int val)
{
    if (ch<RC_CHNS && val>=RC_MIN && val<=RC_MAX)
    {
        if (m_calibrat==Step_Trim && ch!=RC_Throttle)
        {
            int ave = s_trimAverage[ch];
            uint16_t &count = s_trimCount[ch];
            ave = count>0 ? (ave*count+val)/(count+1) : val;
            if (abs(ave-val) < 50)
            {
                m_trimCalibrat[ch] = val;
                s_trimAverage[ch] = ave;
                count++;
            }
            else if(abs(val-m_curVal[ch])<50)
            {
                m_trimCalibrat[ch] = val;
                s_trimAverage[ch] = (m_curVal[ch]+val)/2;
                count = 2;
            }
        }
        else if(m_calibrat==Step_Tip)
        {
            if (val > m_maxCalibrat[ch])
                m_maxCalibrat[ch] = val;
            if (val < m_minCalibrat[ch] || 0 == m_minCalibrat[ch])
                m_minCalibrat[ch] = val;
        }

        if(m_curVal[ch] != val)
        {
            m_curVal[ch] = val;
            notifyChange(ch);
        }
    }
}
