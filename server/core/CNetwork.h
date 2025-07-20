#if !defined(_COOPSERVER_NETWORK_NETWORK_H_) || !defined(_COOPSERVER_CNETWORK_H_)
#define _COOPSERVER_NETWORK_NETWORK_H_
#define _COOPSERVER_CNETWORK_H_  



#include <cstddef>
#include <unordered_map>
#include <atomic>
#include <thread>

#include "../include/enet/enet.h"
#include "CPacketListener.h"

static int max_server_slots = 0;
static char *COOPANDREAS_VERSION = "0.1.1-alpha";

class CNetwork
{
	public:
        static inline std::atomic<bool> shared_loop_value = false;
  
		CNetwork();
		static std::unordered_map<unsigned short, CPacketListener*> m_packetListeners;
		static bool Init(char hostname[], unsigned short &port, int max_slots = 4);
		static void InitListeners();
		static void SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0);
		static void SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0, ENetPeer* dontShareWith = nullptr);
		static void SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0, ENetPeer* dontShareWith = nullptr);
  //HandleServerPackets(ENetHost*, EnetEvent*, )
  static void* HandleServerPacketsThread(ENetHost* p_server, ENetEvent* p_event, void (*p_HandlePlayerConnected)(ENetEvent&), void (*p_HandlePlayerDisconneted)(ENetEvent&), void (*p_HandlePacketReceive)(ENetEvent&));
		~CNetwork();

	private:
		static void HandlePlayerConnected(ENetEvent& event);
		static void HandlePlayerDisconnected(ENetEvent& event);
		static void HandlePacketReceive(ENetEvent& event);
		static void AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int));		
	
};

#endif
