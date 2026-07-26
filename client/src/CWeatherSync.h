#pragma once

class CWeatherSync
{
public:
	static void SyncCurrentState();
	static void HandlePacket(Packets::World::GameWeatherTime* pGameWeatherTime);
	static void ServerTimeRecalculated(server_time_t serverTime);
};