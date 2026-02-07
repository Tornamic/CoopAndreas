#pragma once

class CWantedLevelSync
{
public:
	static void Trigger();
	static void Recalculate();
	static void ResetLocal();
	static void HandlePacket(void* data, int size);
	static void OnPlayerDisconnected();
	static void OnPlayerRespawned(CNetworkPlayer* player);

private:
	static uint8_t m_nLocalOwnWantedLevel;
	static uint8_t m_nLastSentOwnLevel;
	static uint8_t m_nSharedMaxApplied;
};
