#include "stdafx.h"
#include "CCommandTeleportPlayersToHostSafely.h"
#include <CGeneral.h>
#include <CPedPlacement.h>

static constexpr int MAX_TELEPORT_SLOTS = 3;
static constexpr float MIN_DIST = 1.0f;
static constexpr float MAX_DIST = 5.0f;
static void CalculateSafePositions(CVector& targetPos, float targetHeading, int playerCount, CVector* out) 
{
	assert(playerCount <= MAX_TELEPORT_SLOTS);
	
	if (playerCount == 0) return;

	for (int attempt = 0, pedIndex = 0; attempt < 2 && pedIndex < playerCount; attempt++)
	{
		for (float dist = MIN_DIST; dist <= MAX_DIST && pedIndex < playerCount; dist += 0.5f)
		{
			for (float angle = targetHeading + 2.35619f; angle < targetHeading + 6.28319f && pedIndex < playerCount; angle += attempt == 0 ? 0.7f : 0.35f)
			{
				float rad = CGeneral::LimitRadianAngle(angle);
				CVector offset(-sinf(rad), cosf(rad), 0.0f);
				CVector teleportPos = targetPos + offset * dist;

				if (CPedPlacement::FindZCoorForPed(&teleportPos))
				{
					if (!CWorld::TestSphereAgainstWorld(teleportPos, 0.35f, false, true, true, true, true, true, false)
						&& CWorld::GetIsLineOfSightClear(targetPos, teleportPos, true, true, false, true, true, false, false))
					{
						out[pedIndex++] = teleportPos;
					}
				}
			}
		}
	}
}

void CCommandTeleportPlayersToHostSafely::Process(CRunningScript* script)
{
	script->CollectParameters(3);

	assert(CLocalPlayer::m_bIsHost);
	
	int playerCount = 0;
	CNetworkPlayer* networkPlayers[MAX_TELEPORT_SLOTS];
	for (int i = 0; i < MAX_TELEPORT_SLOTS; i++)
	{
		if (ScriptParams[i] != -1)
		{
			if (auto ped = CPools::GetPed(ScriptParams[i]))
			{
				if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(ped))
				{
					networkPlayers[playerCount] = networkPlayer;
					playerCount++;
				}
			}
		}
	}

	if (playerCount == 0) return;

	CVector positions[MAX_TELEPORT_SLOTS];
	for (int i = 0; i < playerCount; i++)
		positions[i] = CVector(0.0f, 0.0f, 0.0f);

	CalculateSafePositions(FindPlayerCoors(0), FindPlayerPed(0)->m_fCurrentRotation, playerCount, positions);
	for (int i = 0; i < playerCount; i++)
	{
		if (positions[i].x == 0.0f && positions[i].y == 0.0f && positions[i].z == 0.0f)
		{
			positions[i] = FindPlayerCoors(0);
		}
		CPackets::TeleportPlayerScripted packet{};
		packet.playerid = networkPlayers[i]->m_iPlayerId;
		packet.pos = positions[i];
		packet.heading = FindPlayerPed(0)->m_fCurrentRotation;
		CNetwork::SendPacket(CPacketsID::TELEPORT_PLAYER_SCRIPTED, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
	}
}
