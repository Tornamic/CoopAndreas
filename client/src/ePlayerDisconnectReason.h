#pragma once
#include "stdafx.h"

enum ePlayerDisconnectReason : uint8_t
{
	PLAYER_DISCONNECT_REASON_VERSION_MISMATCH = 1,
	PLAYER_DISCONNECT_REASON_NAME_TAKEN
};