#ifndef __VGGLOBALFUN_H__
#define __VGGLOBALFUN_H__

#include <QByteArray>
#include <QGeoCoordinate>

class MissionItem;
class VGOutline;
class VGCoordinate;

namespace VGGlobalFunc {
    bool IsBigEndian(void);
    //火星地图转GPS位置转
    void gpsCorrect(double &lat, double &lon);
    void toGps(double &lat, double &lon);
    int bytesToInt(const QByteArray &src);
    unsigned short bytesToUShort(const QByteArray &src);
    QByteArray intToBytes(int value);

    QGeoCoordinate gpsCorrect(const QGeoCoordinate &coordinate);
    QGeoCoordinate toGps(const QGeoCoordinate &coordinate);
    quint32 crc32(const QByteArray &data);
}

#endif // __VGGLOBALFUN_H__

