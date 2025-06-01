#pragma once
class CLocalPlayer
{
public:
	static inline float m_vecLastAimX;
	static inline float m_vecLastAimY;
	static inline char m_Name[32 + 1];
	static inline bool m_bIsHost;

	static void BuildTaskPacket(eTaskType type, bool toggle = true);
};

