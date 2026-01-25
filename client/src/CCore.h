#pragma once
#include <semver.h>

class CCore
{
public:
	static void Init();
	static void AllocateConsole();

	static semver_t Version;
	static inline injector::game_version_manager gvm{};
};

