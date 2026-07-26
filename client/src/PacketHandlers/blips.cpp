#include "network/packets/scripts.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CNetworkEntityBlip.h>
#include <CNetworkCheckpoint.h>
#include <CNetworkStaticBlip.h>

PACKET_HANDLER(ePacketType::UPDATE_ENTITY_BLIP, Packets::Blips::UpdateEntityBlip* pUpdateEntityBlip)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	CNetworkEntityBlip::UpdateEntityBlip(pUpdateEntityBlip);
}

PACKET_HANDLER(ePacketType::REMOVE_ENTITY_BLIP, Packets::Blips::RemoveEntityBlip* pRemoveEntityBlip)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	CNetworkEntityBlip::RemoveEntityBlip(pRemoveEntityBlip);
}

PACKET_HANDLER(ePacketType::CLEAR_ENTITY_BLIPS, Packets::Blips::ClearEntityBlips* pClearEntityBlips)
{
	CNetworkEntityBlip::ClearEntityBlips();
}

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

PACKET_HANDLER(ePacketType::CREATE_STATIC_BLIP, Packets::Blips::StaticBlipsSnapshot* pCreateStaticBlip)
{
	if (CLocalPlayer::m_bIsHost)
	{
		return;
	}

	CNetworkStaticBlip::Create(*pCreateStaticBlip);
}
