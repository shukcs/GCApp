#ifndef __VG_EDGE_TIP_H__
#define __VG_EDGE_TIP_H__

#include <QGeoCoordinate>
#include "MapAbstractItem.h"

class VGCoordinate;
class VGEdgeTip : public MapAbstractItem
{
    Q_OBJECT
    Q_PROPERTY(QString tip READ GetTip NOTIFY changed)
    Q_PROPERTY(QGeoCoordinate coor READ GetCoordinate NOTIFY changed)
    Q_PROPERTY(double rotation READ GetRotation NOTIFY changed)
    Q_PROPERTY(bool valid READ IsValid NOTIFY changed)
public:
    explicit VGEdgeTip(VGCoordinate *beg, VGCoordinate *end, QObject *parent = 0);
    explicit VGEdgeTip(const VGEdgeTip &oth);
    ~VGEdgeTip();

    QString GetTip()const;
    const QGeoCoordinate GetCoordinate()const;
    double GetRotation()const;
    bool IsValid()const;
protected:
    MapAbstractItem::MapItemType ItemType() const;
    bool operator==(const MapAbstractItem &item)const;
private:
    void prcsSignals();
signals:
    void changed();
private slots:
    void onTipDestroied();
private:
    VGCoordinate *m_beg;
    VGCoordinate *m_end;
};

#endif //__VG_EDGE_TIP_H__