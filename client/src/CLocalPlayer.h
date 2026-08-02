#pragma once
class CLocalPlayer
{
public:
	static inline float m_vecLastAimX;
	static inline float m_vecLastAimY;
	static inline char m_Name[Config::MAX_NICKNAME_LENGTH + 1];
	static inline uint16_t m_nPing;
	static inline bool m_bIsHost;

	static void BuildTaskPacket(eTaskType type, bool toggle = true);

	static bool GetIsHostingEntity(CEntity* pEntity);
};

