#include "ShareFunction.h"
#include <math.h>

#define ER  6378245.0
#define EE  0.00669342162296594323
#define CountCal 4

#ifndef M_PI/* pi */
#define	M_PI 3.14159265358979323846
#endif

union FlagEnddian
{
    int     nFlag;
    char    cData;
};

static bool outOfChina(double lat, double lon)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return false;
}

static double transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * M_PI) + 40.0 * sin(y / 3.0 * M_PI)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * M_PI) + 320 * sin(y * M_PI / 30.0)) * 2.0 / 3.0;
    return ret;
}

static double transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * M_PI) + 40.0 * sin(x / 3.0 * M_PI)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * M_PI) + 300.0 * sin(x / 30.0 * M_PI)) * 2.0 / 3.0;
    return ret;
}

static void calculateOffset(double inLat, double inLon, double &latOffset, double &lonOffset)
{
    latOffset = transformLat(inLon - 105.0, inLat - 35.0);
    lonOffset = transformLon(inLon - 105.0, inLat - 35.0);
    double radLat = inLat / 180.0 * M_PI;
    double magic = sin(radLat);
    magic = 1 - EE * magic * magic;
    double sqrtMagic = sqrt(magic);

    latOffset = (latOffset * 180.0) / ((ER * (1 - EE)) / (magic * sqrtMagic) * M_PI);
    lonOffset = (lonOffset * 180.0) / (ER / sqrtMagic * cos(radLat) * M_PI);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//namespace ShareFunction
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool ShareFunction::IsBigEndian(void)
{
    const static FlagEnddian sFlag = { 1 };
    return sFlag.cData == sFlag.nFlag;
}

void ShareFunction::gpsCorrect(double &inLat, double &inLon)
{
    if (outOfChina(inLat, inLon))
        return;

    double dLat;
    double dLon;
    calculateOffset(inLat, inLon, dLat, dLon);
    inLat += dLat;
    inLon += dLon;
}

void ShareFunction::toGps(double &lat, double &lon)
{
    if (outOfChina(lat, lon))
        return;

    double dLat;
    double dLon;
    double tmpLat = lon;
    double tmplon = lon;
    for (int i = 0; ; ++i)//根据区域收敛反向求gps多次计算约接近
    {
        calculateOffset(tmpLat, tmplon, dLat, dLon);
        if (i >= CountCal)
            break;

        tmpLat = lat - dLat;
        tmplon = lon - dLon;
    }

    lat -= dLat;
    lon -= dLon;
}

void ShareFunction::gcjTranceBaidu(double &lat, double &lon)
{
    double x = lon;
    double y = lat;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * M_PI);
    double theta = atan2(y, x) + 0.000003 * cos(x * M_PI);
    lon = z * cos(theta) + 0.0065;
    lat = z * sin(theta) + 0.006;
}
