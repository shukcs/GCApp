#ifndef __VGSURVEYMONITOR_H__
#define __VGSURVEYMONITOR_H__

#include <QObject>
#include <QGeoCoordinate>

class VGPlantInformation;
class VGSurveyMonitor : public QObject
{
    Q_OBJECT
    Q_ENUMS(Survey_Dev)
    Q_PROPERTY(Survey_Dev dev READ GetDevType NOTIFY devChanged)
    Q_PROPERTY(bool connected READ IsConnected NOTIFY connectedChanged)
    Q_PROPERTY(QGeoCoordinate coordinate READ GetCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(double lat READ GetLatitude NOTIFY coordinateChanged)
    Q_PROPERTY(double lon READ GetLongitude NOTIFY coordinateChanged)
    Q_PROPERTY(double relativeAltitude READ RelativeAltitude NOTIFY relativeAltitudeChanged)
    Q_PROPERTY(double altitude READ Altitude NOTIFY altitudeChanged)
    Q_PROPERTY(int satNum READ GetSatNum NOTIFY satNumChanged)
    Q_PROPERTY(double powerPercent READ PowerPercent NOTIFY powerPercentChanged)
    Q_PROPERTY(double precision READ Precision NOTIFY precisionChanged)
    Q_PROPERTY(QString posTip READ GetPositionTip  NOTIFY posTipChanged)
    Q_PROPERTY(bool posValid READ IsPositionValid WRITE SetPositionValid NOTIFY positionValidChanged)
    Q_PROPERTY(float compass READ GetCompass NOTIFY compassChanged)
    Q_PROPERTY(bool sysMode READ GetSysMode WRITE SetSysMode NOTIFY sysModeChanged)
    Q_PROPERTY(bool p900Mode READ GetP900Mode WRITE SetP900Mode NOTIFY p900ModeChanged)
    Q_PROPERTY(bool unitAddr READ GetUnitAddr NOTIFY unitAddrChanged)
    Q_PROPERTY(int destAddr READ GetDestAddr WRITE SetDestAddr NOTIFY destAddrChanged)
public:
    enum Survey_Dev {
        Dev_Baidu,
        Dev_CQCDQ,
        Dev_Plant,
    };
public:
    explicit VGSurveyMonitor(QObject *parent = NULL);
    ~VGSurveyMonitor();
public:
    bool IsPositionValid()const;
    void SetPositionValid(bool bValid);
    void SetLatAndLon(double lat, double lon, bool bHasGGa=false);
    void SetRelativeAltitude(double altitude);
    void SetAltitude(double altitude);
    void SetSatNum(int n);
    void SetPowerPercent(double percent);
    void SetPrecision(double f);
    void SetConnected(bool connected, Survey_Dev dev);
    Survey_Dev GetDevType()const;
    void SetPosMode(int mode);
    void SetFixType(int type);
    void SetCompass(float f);
    void DealParam(const QString &id, const QVariant &var);
public:
    static double correctValue(double v, const QString &flag);
    static double toGGAValue(double v);
protected:
    //qml property
    QString GetPositionTip()const;
    QGeoCoordinate GetCoordinate()const;
    bool IsConnected()const;
    double RelativeAltitude()const;
    double Altitude()const;
    int GetSatNum()const;
    double PowerPercent()const;
    double Precision()const;
    double GetLatitude()const;
    double GetLongitude()const;
    bool GetSysMode()const;
    void SetSysMode(bool b);
    bool GetP900Mode()const;
    void SetP900Mode(bool b);
    bool GetUnitAddr()const;
    int GetDestAddr()const;
    void SetDestAddr(int v);
    float GetCompass()const;
    //overload
    void timerEvent(QTimerEvent *event);
protected slots:
    void onReceiveRtcm(const QList<uint16_t> idxs, const QList<QByteArray> &rtcms);
signals:
    void connectedChanged(bool b);
    void relativeAltitudeChanged(double altitude);
    void altitudeChanged(double altitude);
    void distanceChanged(double distance);
    void satNumChanged(int n);
    void powerPercentChanged(double pow);
    void compassAngleChanged(int angle);
    void rollAngleChanged(int angle);
    void pitchAngleChanged(int angle);
    void precisionChanged(int angle);
    void positionValidChanged(bool isPositionValid);
    void coordinateChanged();
    void posTipChanged(const QString &tip);
    void compassChanged();

    void sysModeChanged();
    void p900ModeChanged();
    void unitAddrChanged();
    void destAddrChanged();
    void devChanged();
private:
    void _initial();
    void _setSysMode(bool b, bool setSuv=false);
    void _setP900Mode(bool b, bool setSuv=false);
    void _setUnitAddr(bool b);
    void _setDestAddr(int v, bool setSuv=false);
    void _setParam(const QString &id, const QVariant &v);
    void _readParam();
private:
    double      m_relativeAltitude;
    double      m_altitude;
    double      m_powerPercent;
    double      m_fPrecision;
    int         m_satNum;
    float       m_compass;

    int         m_idTimer;
    int         m_posMode;
    int         m_countTimeout;
    bool        m_bConnected;
    bool        m_bPositionValid;
    bool        m_bSysMode;
    bool        m_bP900Mode;
    bool        m_bUnitAddr;
    int         m_nDestAddr;
    Survey_Dev  m_dev;
    QGeoCoordinate  m_pos;
};

#endif // __VGSURVEYMONITOR_H__
