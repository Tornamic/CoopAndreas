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
	static void SetState(std::string state);
	static void SetDetails(std::string details);
};