#include "../project_files/stdafx.h"
int i = 0;
class CoopAndreas {
public:
    CoopAndreas() {
		Events::shutdownRwEvent += []
			{
				if (CNetwork::m_bConnected)
				{
					enet_peer_disconnect(CNetwork::m_pPeer, 0);
					enet_peer_reset(CNetwork::m_pPeer);
				}
			};
		Events::gameProcessEvent += []
			{
				if (GetAsyncKeyState(VK_F8))
				{
					CNetwork::Disconnect();
				}
			};
		CCore::Init();
	};
} CoopAndreasPlugin;
