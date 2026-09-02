#include "LinkInterface.h"
#include "LinkCommand.h"
#include <QDateTime>
#include <QDebug>

/*////////////////////////////////////////////////////////////////////////////////
LinkInterface
*/////////////////////////////////////////////////////////////////////////////////
LinkInterface::LinkInterface(LinkCommand *cmd) :
QObject(cmd), m_bDecodedMavlink(false)
, m_bSetMavCh(false)
, m_active(false)
, m_nMavCh(0)
{
	qRegisterMetaType<LinkInterface *>("LinkInterface*");
}

bool LinkInterface::IsActive(void) const
{
    return m_active;
}

void LinkInterface::SetActive(bool active)
{
    m_active = active;
}

LinkCommand *LinkInterface::getLinkCommand() const
{
    return dynamic_cast<LinkCommand*>(parent());
}

QString LinkInterface::devName() const
{
    if (LinkCommand *cmd = dynamic_cast<LinkCommand*>(parent()))
        return cmd->getName();

    return QString();
}

bool LinkInterface::isLogReplay(void) const
{
    return false;
}

uint8_t LinkInterface::getMavlinkChannel(void) const
{
    if (!m_bSetMavCh)
        qWarning() << "Call to LinkInterface::mavlinkChannel with _mavlinkChannelSet == false";

    return m_nMavCh;
}

int LinkInterface::linkType() const
{
	if (LinkCommand *lc = getLinkCommand())
		return lc->type();

	return LinkCommand::TypeLast;
}

bool LinkInterface::IsDecodedMavlink() const
{
    return m_bDecodedMavlink;
}

void LinkInterface::SetDecodedMavlink(bool b)
{
    m_bDecodedMavlink = b;
}

/// Sets the mavlink channel to use for this link
void LinkInterface::_setMavlinkChannel(uint8_t channel)
{
    if (m_bSetMavCh)
        qWarning() << "Mavlink channel set multiple times";

    m_bSetMavCh = true;
    m_nMavCh = channel;
}
