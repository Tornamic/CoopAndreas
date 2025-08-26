#include "stdafx.h"
#include "CDiscordRPC.h"

void CDiscordRPC::Init()
{
	DiscordEventHandlers handlers;

	memset(&handlers, 0, sizeof(handlers));
	memset(&presence, 0, sizeof(presence));

	presence.state = state.c_str();
	presence.details = details.c_str();
	presence.startTimestamp = std::time(nullptr);
	presence.largeImageKey = "icon";
	presence.largeImageText = "CoopAndreas";

	Discord_Initialize("1324128258672951317", &handlers, 1, NULL);
	Discord_UpdatePresence(&presence);
}

void CDiscordRPC::Destroy()
{
	Discord_Shutdown();
}

void CDiscordRPC::SetDetailsAndState(std::string details, std::string state)
{
	CDiscordRPC::details = details;
	presence.details = details.c_str();
	CDiscordRPC::state = state;
	presence.state = state.c_str();
	Discord_UpdatePresence(&presence);
}