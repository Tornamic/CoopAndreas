#pragma once

#include "discord_register.h"
#include "discord_rpc.h"
#include <string>
#include <ctime>

class CDiscordRPC
{
public:
	inline static std::string state;
	inline static std::string details;
	inline static DiscordRichPresence presence;

	static void Init();
	static void Destroy();
	static void SetDetailsAndState(std::string details, std::string state);
};