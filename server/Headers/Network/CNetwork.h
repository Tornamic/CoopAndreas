#if !defined(_COOPSERVER_NETWORK_NETWORK_H_) || !defined(_COOPSERVER_CNETWORK_H_)
#define _COOPSERVER_NETWORK_NETWORK_H_
#define _COOPSERVER_CNETWORK_H_  



#include <cstddef>
#include <unordered_map>
#include <future>

#include "../../include/enet/enet.h"
#include "CPacketListener.h"

#define MAX_SERVER_PLAYERS (4) 
static char *COOPANDREAS_VERSION = "0.1.1-alpha";

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
		~CNetwork();

	private:
		static void HandlePlayerConnected(ENetEvent& event);
		static void HandlePlayerDisconnected(ENetEvent& event);
		static void HandlePacketReceive(ENetEvent& event);
		static void AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int));		
	
};

#endif