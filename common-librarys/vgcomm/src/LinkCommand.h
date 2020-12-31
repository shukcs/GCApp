/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2015 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#ifndef LINKCONFIGURATION_H
#define LINKCONFIGURATION_H

#include "vgcomm_global.h"
#include <QObject>

class LinkInterface;
class QSettings;

/// Interface holding link specific settings.

class VGCOMMSHARED_EXPORT LinkCommand : public QObject
{
    Q_OBJECT
    Q_ENUMS(LinkType)
    Q_PROPERTY(LinkType linkType            READ type                                   CONSTANT)
    Q_PROPERTY(QString  name                READ getName        WRITE setName           NOTIFY nameChanged)
    Q_PROPERTY(bool     autoConnect         READ IsAutoConnect  WRITE SetAutoConnect    NOTIFY autoConnectChanged)
    Q_PROPERTY(bool     connected           READ isConnect                              NOTIFY connectedChanged)
public:
    enum LinkType {
#ifndef __ios__
        TypeSerial,     ///< Serial Link
#endif
        TypeUdp,        ///< UDP Link
        //TypeTcp,        ///< TCP Link
#ifdef QGC_ENABLE_BLUETOOTH
        TypeBluetooth,  ///< Bluetooth Link
#endif
#ifdef QT_DEBUG
       //TypeMock,       ///< Mock Link for Unitesting
#endif
#ifndef __mobile__
      //TypeLogReplay,
#endif
        TypeLast  // Last type value (type >= TypeLast == invalid)
    };
    enum Link_Stat{
        St_Disconnected,
        St_Connecting,
        St_Connected,
    };
public:
    virtual ~LinkCommand();
    LinkInterface *link(void);
    Link_Stat GetLinkStat()const;
    void SetLinkStat(Link_Stat st);

    bool IsAutoConnect()const;
    void SetAutoConnect(bool autoc = true);
    bool isConnect()const;
    void write(const char *buff, unsigned len);

    virtual LinkType type()const = 0;
    virtual QString getName()const = 0;
    virtual void setName(const QString &s) = 0;
    void connectLink();
    void disconnectLink();
public:
    static LinkCommand *createSettings(int type);
protected:
    virtual LinkInterface *CreateLink() = 0;
    LinkCommand(QObject *p = NULL);
protected slots:
    void onLinkConnected(bool b);
signals:
    void nameChanged (const QString &name);
    void linkDestroy(LinkInterface *link);
    void autoConnectChanged();
    void connectedChanged();
protected:
    LinkInterface *m_link; ///< Link currently using this configuration (if any)
private:
    bool        m_autoConnect;   ///< This connection is started automatically at boot
    Link_Stat   m_stLink;       ///可以连接和断开
};

#endif // LINKCONFIGURATION_H
