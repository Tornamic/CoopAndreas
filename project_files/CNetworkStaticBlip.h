#pragma once

class CNetworkStaticBlip
{
public:
	//static inline std::vector<SMissionMarker> ms_vMissionMarkers;
	static inline bool ms_bMassUpdateJustReceived = false;
	static inline bool ms_bNeedToSendAfterThisFrame = true;

	static void Create(CVector& position, eRadarSprite sprite, eBlipDisplay display, eBlipType type, bool trackingBlip, bool shortRange);
	static void Send();

	static inline bool IsAllowedSyncingRadarSprite(eRadarSprite sprite)
	{
		return (sprite >= 5 && sprite <= 40) || (sprite >= 42 && sprite <= 55) || (sprite >= 58 && sprite <= 63);
	}
};

