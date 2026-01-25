#pragma once

#ifndef _PLAYERDISCONNECTREASON_H_
#define _PLAYERDISCONNECTREASON_H_
#include <cstdint>

enum ePlayerDisconnectReason : uint8_t
{
	PLAYER_DISCONNECT_REASON_VERSION_MISMATCH = 1,
	PLAYER_DISCONNECT_REASON_NAME_TAKEN
};

#endif