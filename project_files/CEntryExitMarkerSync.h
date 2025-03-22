#pragma once
#include <vector>
class CEntryExitMarkerSync
{
public:
	static inline std::vector<uint8_t> ms_vLastData;
	static inline bool ms_bNeedToUpdateAfterProcessingScripts = false;
	static inline bool ms_bNeedToUpdateAfterProcessingThisFrame = false;
	static inline uint32_t ms_nLastUpdate = 0;

	static void Send();
	static void Receive(void* data, size_t size);
};