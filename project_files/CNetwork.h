#pragma once
class CPacketListener
{
public:
	unsigned short m_iPacketID;
	void(*m_callback)(void*, int);

	CPacketListener(unsigned short id, void(*callback)(void*, int))
	{
		m_iPacketID = id;
		m_callback = callback;
	}
};

class CNetwork
{
public:
	static std::unordered_map<unsigned short, CPacketListener*> m_packetListeners;
	static ENetHost* m_pClient;
	static ENetPeer* m_pPeer;
	static bool CNetwork::m_bConnected;
	static char CNetwork::m_IpAddress[128 + 1];
	static unsigned short CNetwork::m_nPort;
	static inline uint32_t ms_nBytesReceivedThisSecond;
	static inline uint32_t ms_nBytesReceivedThisSecondCounter;
	static inline uint32_t ms_nBytesSentThisSecond;
	static inline uint32_t ms_nBytesSentThisSecondCounter;

	static DWORD WINAPI InitAsync(LPVOID);
	static void Disconnect();
	static void SendPacket(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag = (ENetPacketFlag)0);
	static void InitListeners();
	static void HandlePacketReceive(ENetEvent& event);
	static void AddListener(unsigned short id, void(*callback)(void*, int));
};
uint8_t get_channelID(uint8_t packetType);