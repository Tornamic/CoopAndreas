#pragma once
class COpCodeSync
{
public:
	static inline bool ms_bSyncingEnabled = true;
	static inline bool ms_bLoadingCutscene = true;
	static inline bool ms_abLoadingMissionAudio[4];
	static inline std::vector<CRunningScript*> ms_vSyncedScripts;

	static void Init();
	static void HandlePacket(const uint8_t* buffer, int bufferSize);
};

