#ifndef __VG_CONTROLLER_H__
#define __VG_CONTROLLER_H__

#include <QObject>

class VGPlantInformation;
typedef struct __mavlink_rc_channels_t mavlink_rc_channels_t;

class VGController : public QObject
{
    Q_OBJECT
    Q_ENUMS(StepCalibrat)
    Q_PROPERTY(StepCalibrat step READ GetCalibrat WRITE SetCalibrat NOTIFY stepChanged)
    Q_PROPERTY(int roll READ GetRoll NOTIFY rollChanged)//-100--100
    Q_PROPERTY(int pitch READ GetPitch NOTIFY pitchChanged)
    Q_PROPERTY(int throttle READ GetThrottle NOTIFY throttleChanged)
    Q_PROPERTY(int yaw READ GetYaw NOTIFY yawChanged)
    Q_PROPERTY(QString rollStr READ GetRollStr CONSTANT)
    Q_PROPERTY(QString pitchStr READ GetPitchStr CONSTANT)
    Q_PROPERTY(QString throttleStr READ GetThrottleStr CONSTANT)
    Q_PROPERTY(QString yawStr READ GetYawStr CONSTANT)
    Q_PROPERTY(QString stepTip READ GetStepTip NOTIFY stepTipChanged)
public:

    enum {
        RC_Roll = 0,
        RC_Pitch,
        RC_Throttle,
        RC_Yaw,
        RC_CHNS = 4,
        RC_MIN = 800,
        RC_TRIM = 1500,
        RC_MAX = 2200,
    };
    enum StepCalibrat {
        Step_None = 0,
        Step_Trim,
        Step_Tip,
        Step_Finish,
    };
public:
    VGController(QObject *p=NULL);

    StepCalibrat GetCalibrat()const;
    int GetRoll()const;
    int GetPitch()const;
    int GetThrottle()const;
    int GetYaw()const;

    QString GetRollStr()const;
    QString GetPitchStr()const;
    QString GetThrottleStr()const;
    QString GetYawStr()const;
    QString GetStepTip()const;
    void SetCurUav(VGPlantInformation *uav);
    float GetCalibratValue(uint32_t ch, int mask)const;
    QString GetCalibratParamID(uint32_t ch, int mask)const;
public slots:
    void OnRecvRCChannels(const QString &id, const mavlink_rc_channels_t &rc);
    void OnParamChanged(const QString &id);
protected:
    void SetCalibrat(StepCalibrat st);
private:
    void intitParam();
    void initCalibrat();
    int getChannelPar(uint32_t ch)const;
    bool changeParams(uint32_t ch, int mask, uint32_t val);
    void notifyChange(uint32_t ch);
    void calibrat(uint32_t ch, int val);
signals:
    void stepChanged();
    void rollChanged();
    void pitchChanged();
    void throttleChanged();
    void yawChanged();
    void stepTipChanged();
private:
    VGPlantInformation  *m_curUav;
    StepCalibrat        m_calibrat;
    uint16_t            m_min[RC_CHNS];
    uint16_t            m_max[RC_CHNS];
    uint16_t            m_trim[RC_CHNS];
    uint16_t            m_minCalibrat[RC_CHNS];
    uint16_t            m_maxCalibrat[RC_CHNS];
    uint16_t            m_trimCalibrat[RC_CHNS];
    uint16_t            m_curVal[RC_CHNS];
};

#endif // __VG_CONTROLLER_H__
