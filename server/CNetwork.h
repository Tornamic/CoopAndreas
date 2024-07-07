#pragma once
class CNetwork
{
public:
	static bool Init(unsigned short port);
	static void SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag);
	static void SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith);
};

