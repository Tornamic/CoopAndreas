#pragma once
class CNetworkEntityBlip
{
public:
	static void UpdateEntityBlip(CPackets::UpdateEntityBlip* packet);
	static void RemoveEntityBlip(CPackets::RemoveEntityBlip* packet);
	static void ClearEntityBlips();
};

