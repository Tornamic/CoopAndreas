#include "../project_files/stdafx.h"

bool created = false;
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
					CNetworkPlayer* player = new CNetworkPlayer(0, FindPlayerCoors(0));
				}
			};
	};
} CoopAndreasPlugin;
