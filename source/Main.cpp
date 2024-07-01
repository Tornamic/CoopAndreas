#include "../project_files/stdafx.h"

using namespace plugin;

class CoopAndreas {
public:
    CoopAndreas() {
		//------------------------------------
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		//------------------------------------

		CCore::Init();
	};
} CoopAndreasPlugin;
