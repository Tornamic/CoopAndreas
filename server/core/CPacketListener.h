#pragma once

#ifndef _CPACKERLISTENER_H_
	#define _CPACKERLISTENER_H_

#include <vector>
#include <algorithm>
#include "../thirdparty-libraries/enet/enet.h"


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
		~CPacketListener();
	
};

#endif