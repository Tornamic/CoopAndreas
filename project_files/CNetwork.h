#pragma once
class CNetwork
{
public:
	static ENetHost* m_pClient;
	static ENetPeer* m_pPeer;
	static bool CNetwork::m_bConnected;

	static DWORD WINAPI InitAsync(LPVOID);
	static void Disconnect();
	static void SendPacket(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag);
};