#if !defined(_COOPSERVER_NETWORK_PACKETLISTENER_H_) || !defined(_COOPSERVER_CPACKETLISTENER_H_)
#define _COOPSERVER_NETWORK_PACKETLISTENER_H_
#define _COOPSERVER_CPACKETLISTENER_H_

#include <vector>
#include <algorithm>

#include "../include/enet/enet.h"


class CPacketListener
{
	public:
		unsigned short m_iPacketID;
		void(*m_callback)(ENetPeer*, void*, int);

		CPacketListener(unsigned short var_id, void(*callback)(ENetPeer*, void*, int))
		{
			m_iPacketID = var_id;
			m_callback = callback;
		}
		~CPacketListener() { }
	
};

#endif