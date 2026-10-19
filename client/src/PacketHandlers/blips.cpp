#include "network/packets/scripts.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CNetworkCheckpoint.h>

PACKET_HANDLER(ePacketType::UPDATE_CHECKPOINT, Packets::Blips::UpdateCheckpoint* pUpdateCheckpoint)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	CNetworkCheckpoint::Update(pUpdateCheckpoint->position, pUpdateCheckpoint->radius);
}

PACKET_HANDLER(ePacketType::REMOVE_CHECKPOINT, Packets::Blips::RemoveCheckpoint* pRemoveCheckpoint)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	CNetworkCheckpoint::Remove();
}
