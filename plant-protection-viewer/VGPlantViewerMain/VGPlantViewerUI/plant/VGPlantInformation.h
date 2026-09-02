#ifndef __VGMYPLANEINFORMATION_H__
#define __VGMYPLANEINFORMATION_H__

#include <QObject>
#include <QGeoCoordinate>
#include <QDateTime>
#include <MapAbstractItem.h>

#define Imitation_Mode      "MPC_ALT_MODE"
#define Imitation_Precision "MPC_Z_P"
#define SPS_Permision       "COM_ARM_SPS"
#define QX_UserSdk          "RTCM_SOURCE"
class VGVehicle;
class VGFlyRoute;
class VGVehicleMission;
class VGCoordinate;
class VGLandPolyline;
class VGLandInformation;
class QVariant;
class MissionItem;
class VGSupportPolyline;
class VGBlock;

class VGPlantInformation : public MapAbstractItem
{
    Q_OBJECT
    Q_ENUMS(PlantStatus)
    Q_ENUMS(QxwzStatus)

    Q_PROPERTY(VGVehicleMission *vm READ GetVehicleMission NOTIFY missionChanged)
    Q_PROPERTY(QString flightMode READ GetFlightMode WRITE SetFlightMode NOTIFY flightModeChanged)
    Q_PROPERTY(bool arm READ IsSetArm WRITE SetArm NOTIFY armChanged)

    Q_PROPERTY(QString planeId READ planeId NOTIFY planeIdChanged)
    Q_PROPERTY(float voltage READ GetVoltage NOTIFY voltageChanged)

    Q_PROPERTY(PlantStatus status READ GetStatus NOTIFY statusChanged)
    Q_PROPERTY(bool binded READ IsBinded NOTIFY bindedChanged)
    Q_PROPERTY(QString master READ master  NOTIFY masterChanged)
    Q_PROPERTY(QString posTip READ GetPosTip NOTIFY posTipChanged)
    Q_PROPERTY(bool directLink READ isDirectLink WRITE setDirectLink NOTIFY directLinkChanged)
    Q_PROPERTY(bool landed READ IsLanded NOTIFY landedChanged)
    Q_PROPERTY(double compass READ GetCompassAngle  NOTIFY compassAngleChanged)
    Q_PROPERTY(double roll READ GetRollAngle  NOTIFY rollAngleChanged)
    Q_PROPERTY(double pitch READ GetPitchAngle  NOTIFY pitchAngleChanged)
    Q_PROPERTY(double horSpeed READ GetSpeedHor NOTIFY horSpeedChanged)
    Q_PROPERTY(double powPercent READ GetPowerPercent NOTIFY powPercentChanged)
    Q_PROPERTY(double medSpeed READ GetMedicineSpeed NOTIFY medSpeedChanged)
    Q_PROPERTY(double medVol READ GetMedicineVol NOTIFY medVolChanged)
    Q_PROPERTY(double precision READ GetPrecision NOTIFY precisionChanged)
    Q_PROPERTY(QGeoCoordinate coordinate READ lastCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(double lat READ GetLat NOTIFY coordinateChanged)
    Q_PROPERTY(double lon READ GetLon NOTIFY coordinateChanged)
    Q_PROPERTY(double heigth READ GetHeigth NOTIFY heigthChanged)
    Q_PROPERTY(int satlateNumb READ GetSatlateNumb NOTIFY satlateNumbChanged)
    Q_PROPERTY(bool monitor READ IsMonitor NOTIFY monitorChanged)
    Q_PROPERTY(bool missionValible READ IsMissionValible NOTIFY missionValibleChanged)
    Q_PROPERTY(QString accTip READ GetAccTip NOTIFY accTipChanged)
    Q_PROPERTY(QString gpsTip READ GetGpsTip NOTIFY gpsTipChanged)
    Q_PROPERTY(bool wlcLose READ IsWlcLose NOTIFY wlcLoseChanged)
    Q_PROPERTY(bool restrain READ IsRestrain NOTIFY restrainChanged)
    Q_PROPERTY(QString qxacc READ GetQXAcc NOTIFY qxaccChanged)
    Q_PROPERTY(QString qxpswd READ GetQXPswd NOTIFY qxpswdChanged)
    Q_PROPERTY(QString appkey READ GetAppkey NOTIFY appkeyChanged)
    Q_PROPERTY(QString appsecret READ GetAppsecret NOTIFY appsecretChanged)
    Q_PROPERTY(QString devid READ GetDevid NOTIFY devidChanged)
    Q_PROPERTY(int devtype READ GetDevtype NOTIFY devtypeChanged)
    Q_PROPERTY(QString qxwzStat READ GetQxwzStat  NOTIFY qxwzStatChanged)
public:
    enum PlantStatus
    {
        UnConnect,
        Connected,
    };
    enum QxwzStatus
    {
        QXWZSOCK_UNINIT_ST = 0,
        QXWZTASK_BEGIN_ST,
        QXWZ_READGPS_ST,
        QXWZ_NBEGIN_ST = 10,
        QXWZ_NGETACC_ST,
        QXWZ_NNOSOCK_ST,
        QXWZ_NCONNECT_ST,
        QXWZ_NLOGIN_ST,
        QXWZ_NUNAUTHORIZD_ST,
        QXWZ_NRECRTCM_ST,
        QXWZ_NSENDGGA_ST,
        QXWZ_SDKBEGIN_ST  = 20,
        QXWZ_SDKGETACC_ST ,
        QXWZ_SDKNOSOCK_ST,
        QXWZ_SDKCONNECT_ST,
        QXWZ_SDKLOGIN_ST,
        QXWZ_SDKUNAUTHORIZD_ST,
        QXWZ_SDKEXPIRETIME_ST,
        QXWZ_SDKRECRTCM_ST,
        QXWZ_SDKSENDGGA_ST ,
    };
    class BlockInfo {
    public:
        BlockInfo(int32_t lat, int32_t lon, int32_t angle, uint32_t distance);
        QGeoCoordinate GetCoordinate()const;
        float GetAngle()const;
        float GetDistance()const;
    protected:
        float m_angle;          //角度
        float m_distance;       //距离
        QGeoCoordinate  m_coor;
    };
public:
    explicit VGPlantInformation(QObject *parent = 0);    
    VGPlantInformation(const VGPlantInformation &oth);
    ~VGPlantInformation();

    QString planeId()const;
    void setPlaneId(const QString &id);

    PlantStatus GetStatus()const;
    void SetStatus(PlantStatus status);

    bool IsBinded()const;
    void SetBinded(bool status);

    QString master()const;
    void setMaster(const QString &master);

    qint64 lastTime()const;
    void setLastTime(qint64 time);
    void setLastConnected();
    uint32_t getConnectedTime()const;

    qint64 bindTime()const;
    void setBindTime(qint64 time);

    bool isDirectLink()const;
    void setDirectLink(bool isDirectLink);

    bool IsLanded()const;
    void SetLanded(bool isLanded);

    void SetQxwzStat(int stat);
    QString GetQxwzStat() const;

    void setCoordinate(const QGeoCoordinate &coor);
    QGeoCoordinate lastCoordinate()const;
    double distance()const;
    void setDistance(double distance);
    double GetHeigth();
    void SetRelativeAltitude(double f);
    void SetAltitude(double f);
    int GetSatlateNumb()const;
    void SetSatlateNumb(int n);
    double GetSpeedHor()const;
    void SetSpeedHor(double f);
    double GetSpeedVer()const;
    void SetSpeedVer(double f);
    double GetCompassAngle()const;
    void SetCompassAngle(double f);
    double GetRollAngle()const;
    void SetRollAngle(double f);
    double GetPitchAngle()const;
    void SetPitchAngle(double f);
    double GetPowerPercent()const;
    void SetPowerPercent(double f, bool alarm = false);
    double GetMedicineSpeed()const;
    void SetMedicineSpeed(double f);
    double GetMedicineVol()const;
    void SetMedicineVol(double f);
    double GetPrecision()const;
    void SetPrecision(double f);
    void SetMonitor(bool b, bool bCur=false);
    void CheckConnect(qint64 curMs);
    void syncPlanFlight(VGVehicleMission *vm);
    bool operator==(const MapAbstractItem &item)const;
    MapItemType ItemType()const;

    void PrcsCommandRes(unsigned short cmd, bool res);
    QString GetFlightMode()const;
    void ChangeFlightMode(const QString &mod);
    void SetFlightMode(const QString &mod);
    void SetMissionValible(bool b, int tp=0);
    void SetCurrentExecuteItem(int idx);
    void SetFixType(unsigned char fixType);
    void LoadFromVehicle();
    void TestPump();
	void TestMotor(int idx, int ms, int speedd);
    void Shutdown();
    void SetPlantParammeter(const QString &key, const QVariant &v);
    void GetPlantParammeter(const QString &key);
    void SychFinish(bool suc);
    void Disarm();
    void SetBaseMode(unsigned short bm);
    void SetSelected(bool);
    void SetSychBegin();
	void SetMedicineGrade(int t);
	void SetVoltErr(int t);
    bool DealPlantParameter(const QString &key, const QVariant &v);
    void SetGndHeight(double height);
    QVariant GetParamValue(const QString &key);
    bool IsImitateGround();
    bool IsSpsPermission();
    void SetVoltage(uint16_t mv);
    float GetVoltage()const;
    void SendSupport(bool bEnter, VGCoordinate *spt);
    void SetSysStatus(uint8_t st);
    bool IsWlcLose()const;
    void SetMissionCount(int c, bool finish);
    void SetMissionItem(const MissionItem &item, bool finish);
    void SetDownMisson(bool b);
    void SendSetHome(const QGeoCoordinate &c=QGeoCoordinate());
    void SetHome(const QGeoCoordinate &c, bool bRcv = false);
    void ReceiveSupports(const QGeoCoordinate &beg, const QGeoCoordinate &end, int st);
    void SetQxAccount(const QString &acc, const QString &pswd, bool bSend=true);
    void GetQxAccount();
    void ClearQxAccount(bool bSend=true);
    void ProcessSuspend(int ridge, const QString &plan, const QGeoCoordinate &c);
	void SetContinueCoor(const QGeoCoordinate &c, int type);
    void GetQXSdkAccount();
    void SetQxSdkAccount(const QString &key, const QString &secret,const QString &id, int type, bool bSend=true);
    void ClearQxSdkAccount(bool bSend=true);
    void PrcsBlocks(const QList<BlockInfo> &blocks);
    VGVehicleMission *GetPrepareMission()const;
    void ClearWay();
public:
    static QString GetFlightModeDscb(const QString &str);
    static QString GetQxStatDscb(int stat);
public slots:
    void onMissionChange();
protected:
    bool IsMonitor()const;
    bool IsSetArm()const;
    void SetArm(bool b);
    bool IsMissionValible()const;
    QString GetPosTip()const;
    QString GetAccTip()const;
    QString GetGpsTip()const;
    void   showContent(bool b);
    void _arm(bool bArm);
    void _sysFlightMode();
    void SetPosType(int t);
    void SetAccType(int t);
    void SetGpsType(int t);
    VGVehicleMission *GetVehicleMission()const;
    void reqMissionFromNet(int idx);
    void SetAPointCoor(const QGeoCoordinate &a, bool clear = false);
    void SetBPointCoor(const QGeoCoordinate &b, bool clear = false);
    QString GetQXAcc()const;
    QString GetQXPswd()const;
    QString GetAppkey() const;
    QString GetAppsecret() const;
    QString GetDevid() const;
    int GetDevtype() const;
    bool IsRestrain()const;
    bool IsShowWay(const QGeoCoordinate &coor, uint32_t tm)const;
    double GetLat()const;
    double GetLon()const;
    Q_INVOKABLE VGVehicleMission *beginOperation(VGFlyRoute *fr);
signals:
    void flightModeChanged();
    void planeIdChanged(const QString &id);
    void medVolChanged(double f);
    void precisionChanged(double f);
    void posTipChanged();
    void accTipChanged();
    void gpsTipChanged();
    void qxwzStatChanged();
    void satlateNumbChanged(int n);

    void bindedChanged(bool bBind);
    void statusChanged();
    void masterChanged(const QString &master);
    void directLinkChanged(bool isDirectLink);
    void landedChanged();
    void powPercentChanged(double per);
    void medSpeedChanged(double per);
    void compassAngleChanged(double f);
    void rollAngleChanged(double f);
    void pitchAngleChanged(double f);
    void coordinateChanged();
    void heigthChanged(double f);
    void horSpeedChanged(double f);
    void verSpeedChanged(double f);
    void monitorChanged(bool b);
    void armChanged();
    void wlcLoseChanged();
    void restrainChanged();
    void missionValibleChanged(bool b);
    void missionChanged(VGVehicleMission *vm);
    void missionRun();
    void voltageChanged();
    void qxaccChanged();
    void qxpswdChanged();
    void appkeyChanged();
    void appsecretChanged();
    void devidChanged();
    void devtypeChanged();
protected slots:
    void onSysFinish(const QString &id);
    void onContentDestroied(QObject *o);
private:
    void setMissionRun();
    void setParameter(const QString &str, const QVariant &var=QVariant(), bool doAnyway=false);
    void saveMission();
    void loadMission();
    void sendCommand(int cmd, float f1 = 0, float f2 = 0, float f3 = 0, float f4 = 0, float f5 = 0, float f6 = 0, float f7 = 0);
    void sendSetHome(bool uav, const QGeoCoordinate &c=QGeoCoordinate());
    void prcsSupportAndHome();
    void requestFromUav(uint32_t mavType);

    void _prcsArmOrDisarm(bool);
    void _prcsFligtMode(bool);
    void _prcsSetAssist(int, bool);
    void _prcsSetHome(bool);

    void _checkParams();
    void _showContinue(bool b = true);
    void _showBlocks(bool b);
private:
    bool            m_bDirectLink;  //是否直连
    bool            m_bLanded;      //是否降落
    PlantStatus     m_status;
    bool            m_bBind;        //0 未绑定 1 绑定
    bool            m_bSychUav;
    bool            m_bSys;

    QGeoCoordinate  m_lastPosition; //地图位置
    double          m_lat;          //GPS
    double          m_lon;          //GPS
    double          m_relativeAltitude;
    double          m_gndHeight;
    int             m_satNum;
    double          m_horSpeed;
    double          m_verSpeed;
    double          m_distance;
    double          m_compassAngle;
    double          m_rollAngle;
    double          m_pitchAngle;
    double          m_powerPercent;
	int				m_powerGrade;
    double          m_medicineSpeed;
    double          m_medicineVol;
	int				m_medicineGrade;
    double          m_precision; //航向精度
    int             m_tpPos;
    int             m_acc;
    int             m_gps;
    bool            m_bMonitor;
    bool            m_bMissionValible;
    bool            m_bArmOp;
    bool            m_bWlcLose;
    bool            m_bSetArm;
    float           m_voltage;
    int             m_missionCount;
    bool            m_bDownMs;
    bool            m_bRestrain;
    QString         m_qxwzStat;

    qint64          m_bindTime;         //解绑、绑定时间
    qint64          m_lastTime;         //最后在线时间
    qint64          m_lastLacalTime;    //无数据计时
    qint64          m_lastPosChgTm;    //无数据计时

    QString         m_planeId;          //飞机ID
    QString         m_master;           //绑定者
    QString         m_flightMode;       //飞行模式
    QString         m_flightModeSet;    //飞行模式设置
    QString         m_strQxAcc;         //千寻账号
    QString         m_strQxPswd;        //千寻密码
    QString         m_strAppkey;
    QString         m_strAppsecret;
    QString         m_devid;
    int             m_devType;
    VGVehicleMission        *m_prepareVm; //计划上传mission
    VGVehicleMission        *m_uploadVm;  //上传mission
    VGCoordinate            *m_homeCoor;  //home
    VGCoordinate            *m_cntnCoor;  //中断点
    VGCoordinate            *m_aPointCoor;  //AB点的A
    VGCoordinate            *m_bPointCoor;  //AB点的B
    VGLandPolyline               *m_planMissionLine;
    unsigned                m_baseMode;
    uint32_t                m_statSupport;
    VGSupportPolyline       *m_rtl;
    QMap<QString, QVariant> m_params;
    QMap<QString, QVariant> m_strLsReqParams;
    QList<VGBlock *>        m_blocks;
};

#endif // VGMYPLANEINFORMATION_H
