#pragma once

#ifndef _CNETWORK_H_
	#define _CNETWORK_H_ 

#define COOPANDREAS_VERSION "0.2.2-alpha"

#include <cstddef>
#include <unordered_map>
#include "../thirdparty-libraries/enet/enet.h"

#include "CPacketListener.h"

#define DEFAULT_SERVER_PORT (6767)
#define MAX_SERVER_PLAYERS (8)

class CNetwork
{
	public:
		CNetwork();
		
		static std::unordered_map<unsigned short, CPacketListener*> m_packetListeners;
		static bool Init(unsigned short port);
		static void InitListeners();
		static void SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0);
		static void SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0, ENetPeer* dontShareWith = nullptr);
		static void SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0, ENetPeer* dontShareWith = nullptr);
		static void HandlePlayerConnected(ENetPeer* peer, void* data, int size);
		~CNetwork();
	private:
		static void HandlePeerConnected(ENetEvent& event);
		static void HandlePlayerDisconnected(ENetEvent& event);
		static void HandlePacketReceive(ENetEvent& event);
		static void AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int));

	
};
uint8_t get_channelID(uint8_t packetType)


#endif