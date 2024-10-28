#pragma once
#include "CPlayer.h"

class CPacketListener
{
public:
	unsigned short m_iPacketID;
	void(*m_callback)(ENetPeer*, void*, int);

	CPacketListener(unsigned short id, void(*callback)(ENetPeer*, void*, int))
	{
		m_iPacketID = id;
		m_callback = callback;
	}
};

class CNetwork
{
public:
	static std::vector<CPacketListener*> m_packetListeners;

	static bool Init(unsigned short port);
	static void InitListeners();
	static void SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag);
	static void SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith);
	static void SendPacketStreamFor(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, CPlayer* player);
	static void SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith);
private:
	static void HandlePlayerConnected(ENetEvent& event);
	static void HandlePlayerDisconnected(ENetEvent& event);
	static void HandlePacketReceive(ENetEvent& event);
	static void AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int));
};

