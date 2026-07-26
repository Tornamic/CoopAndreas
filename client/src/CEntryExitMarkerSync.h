#pragma once
#include <vector>
class CEntryExitMarkerSync
{
public:
	static inline Packets::Scripts::EnExSync ms_lastData;
	static inline bool ms_bUpdateAfterProcessingScripts = false;
	static inline bool ms_bUpdateAfterProcessingThisFrame = false;
	static inline uint32_t ms_nLastUpdate = 0;
	static inline std::unordered_map<CEntryExit*, uint32_t> ms_mapLastEnExUpdate{};

	static void Send();
	static void Receive(const Packets::Scripts::EnExSync& packet);
};