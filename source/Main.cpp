#include "../project_files/stdafx.h"

bool created = false;

CNetworkPlayer* player = nullptr;
class CoopAndreas {
public:
    CoopAndreas() {
		CCore::Init();
		Events::gameProcessEvent += []
			{
				// test
				if (KeyPressed(VK_F8) && !created)
				{
					created = true;
					player = new CNetworkPlayer(0, FindPlayerCoors(0));
				}
			};
	};
} CoopAndreasPlugin;
