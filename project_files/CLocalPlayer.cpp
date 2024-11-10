#include "CLocalPlayer.h"

float CLocalPlayer::m_fLastAimX = 0.0f;
float CLocalPlayer::m_fLastAimY = 0.0f;
char CLocalPlayer::m_name[32 + 1] = { '\0' };
bool CLocalPlayer::m_bIsHost = false;
uint16_t CLocalPlayer::m_nMyId = 0;