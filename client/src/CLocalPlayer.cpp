#include "stdafx.h"

void CLocalPlayer::BuildTaskPacket(eTaskType type, bool toggle)
{
	CPackets::SetPlayerTask packet{};
	packet.taskType = type;
	packet.position = FindPlayerCoors(0);
	packet.rotation = FindPlayerPed(0)->m_fCurrentRotation;
	packet.toggle = toggle;
	CNetwork::SendPacket(CPacketsID::SET_PLAYER_TASK, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
