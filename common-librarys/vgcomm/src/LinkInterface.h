/*=====================================================================

PIXHAWK Micro Air Vehicle Flying Robotics Toolkit

(c) 2009 PIXHAWK PROJECT  <http://pixhawk.ethz.ch>

This file is part of the PIXHAWK project

PIXHAWK is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PIXHAWK is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PIXHAWK. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
* @file
*   @brief Brief Description
*
*   @author Lorenz Meier <mavteam@student.ethz.ch>
*
*/

#ifndef _LINKINTERFACE_H_
#define _LINKINTERFACE_H_

#include <QObject>
#include <QMutex>
#include "vgcomm_global.h"

class LinkCommand;
class VGCOMMSHARED_EXPORT LinkInterface : public QObject
{
    Q_OBJECT
public:
    // Property accessors
    bool IsActive(void)const;
    void SetActive(bool active);
	LinkCommand *getLinkCommand()const;
    virtual void requestReset() = 0;
    virtual bool isConnected() const = 0;
    virtual qint64 getConnectionSpeed() const = 0;
    virtual bool isLogReplay(void)const;
	uint8_t getMavlinkChannel(void) const;

	int linkType()const;
    bool IsDecodedMavlink()const;
    void SetDecodedMavlink(bool b);
    QString devName()const;
public slots:
    virtual void writeBytes(const QByteArray &array) = 0;
protected:
	LinkInterface(LinkCommand *cmd);
    virtual bool _connect(void) = 0;
    virtual void _disconnect(void) = 0;
signals:
    void bytesReceived(LinkInterface* link, QByteArray data);
    void linkConnected(bool b);
    void communicationError(const QString& title, const QString& error);
private:
    /// Sets the mavlink channel to use for this link
    void _setMavlinkChannel(uint8_t channel);
private:
    friend class LinkCommand;
    friend class LinkManager;

    bool        m_bSetMavCh;        ///< true: _mavlinkChannel has been set
    uint8_t     m_nMavCh;           ///< mavlink channel to use for this link, as used by mavlink_parse_char
    bool        m_active;           ///< true: link is actively receiving mavlink messages
    bool        m_bDecodedMavlink;

    mutable QMutex _dataRateMutex;  /// Mutex for accessing the data rate member variables
};

#endif // _LINKINTERFACE_H_
