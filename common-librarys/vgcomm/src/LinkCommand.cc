#include "LinkCommand.h"
#ifndef __ios__
#include "SerialLink.h"
#endif
#include "UDPLink.h"
#ifdef QGC_ENABLE_BLUETOOTH
#include "BluetoothLink.h"
#endif

#define LINK_SETTING_ROOT "LinkCommand"

LinkCommand::LinkCommand(QObject *p):QObject(p), m_link(NULL)
, m_autoConnect(false), m_stLink(St_Disconnected)
{
    delete m_link;
}

void LinkCommand::onLinkConnected(bool b)
{
    SetLinkStat(b ? St_Connected : St_Disconnected);
}

LinkCommand::~LinkCommand()
{
    if (m_link)
	{
		emit linkDestroy(m_link);
		m_link->deleteLater();
	}
}

LinkInterface* LinkCommand::link(void)
{
    return m_link;
}


LinkCommand::Link_Stat LinkCommand::GetLinkStat() const
{
    return m_stLink;
}


void LinkCommand::SetLinkStat(Link_Stat st)
{
    if (st != m_stLink)
    {
        m_stLink = st;
        emit connectedChanged();
    }
}

void LinkCommand::connectLink()
{
    if (!m_link)
    {
        if (m_link = CreateLink())
            connect(m_link, &LinkInterface::linkConnected, this, &LinkCommand::onLinkConnected);
    }
    else
    {
        m_link->_disconnect();
    }

    SetLinkStat(St_Connecting);
    if (m_link)
        m_link->_connect();
}

void LinkCommand::disconnectLink()
{
    if (m_link)
    {
        SetLinkStat(St_Connecting);
        SetAutoConnect(false);
        m_link->_disconnect();
    }
}

LinkCommand *LinkCommand::createSettings(int type)
{
    LinkCommand *ret = NULL;
    switch(type) {
#ifndef __ios__
        case LinkCommand::TypeSerial:
            ret = new SerialCommand();
            break;
#endif
        case LinkCommand::TypeUdp:
            ret = new UDPCommand();
            break;
#ifdef QGC_ENABLE_BLUETOOTH
    case LinkCommand::TypeBluetooth:
        ret = new BluetoothCommand();
        break;
#endif
    }
    return ret;
}

bool LinkCommand::IsAutoConnect() const
{
    return m_autoConnect;
}

void LinkCommand::SetAutoConnect(bool autoc /*= true*/)
{
    m_autoConnect = autoc;
    emit autoConnectChanged();
}

bool LinkCommand::isConnect() const
{
    return m_stLink == St_Connected;
}

void LinkCommand::write(const char *buff, unsigned len)
{
    if (m_link && m_link->isConnected())
        m_link->writeBytes(QByteArray(buff, len));
}
