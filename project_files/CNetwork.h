#pragma once
class CNetwork
{
public:
	static ENetHost* m_pClient;

	static DWORD WINAPI InitAsync(LPVOID);
};