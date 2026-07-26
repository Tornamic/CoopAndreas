#pragma once
class CNetworkEntityBlip
{
public:
	static void UpdateEntityBlip(Packets::Blips::UpdateEntityBlip* packet);
	static void RemoveEntityBlip(Packets::Blips::RemoveEntityBlip* packet);
	static void ClearEntityBlips();
};

