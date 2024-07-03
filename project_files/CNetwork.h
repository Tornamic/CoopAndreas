#pragma once
class CNetwork
{
public:
	static ENetHost* m_pClient;
	static ENetPeer* m_pPeer;
	static DWORD WINAPI InitAsync(LPVOID);
	static void SendPacket(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag);
};