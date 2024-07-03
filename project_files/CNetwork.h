#pragma once
class CNetwork
{
public:
	static ENetHost* m_pClient;

	static bool Init(const char* ip, int port);
};