#pragma once
#include "stdint.h"

class CLocalPlayer
{
public:
	static float m_fLastAimX;
	static float m_fLastAimY;
	static char m_name[32 + 1];
	static bool m_bIsHost;
	static uint16_t m_nMyId;
};

