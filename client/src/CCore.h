#pragma once
#include <semver.h>

class CCore
{
public:
	static void Init();
	static void AllocateConsole();

	static inline bool ms_bDebug = false;
	static inline uint8_t ms_nRunIndex;

	static semver_t Version;
	static inline injector::game_version_manager gvm{};
};

