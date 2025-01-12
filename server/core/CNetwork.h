#pragma once

#ifndef _CNETWORK_H_
	#define _CNETWORK_H_ 

#define COOPANDREAS_VERSION "0.1.0-alpha"

#include <cstddef>
#include "../thirdparty-libraries/enet/enet.h"

#include "CPacketListener.h"

#define MAX_SERVER_PLAYERS (16)

class CNetwork
{
	public:
		CNetwork();
		
		static std::vector<CPacketListener*> m_packetListeners;
		static bool Init(unsigned short port);
		static void InitListeners();
		static void SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag);
		static void SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith);
		static void SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith);
		~CNetwork();
	private:
		static void HandlePlayerConnected(ENetEvent& event);
		static void HandlePlayerDisconnected(ENetEvent& event);
		static void HandlePacketReceive(ENetEvent& event);
		static void AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int));		
	
};

#endif