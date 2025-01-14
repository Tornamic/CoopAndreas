#include "stdafx.h"
#include <CTaskSimpleCarSetPedOut.h>
#include <CCarEnterExit.h>
#include <CTaskSimpleCarSetPedInAsPassenger.h>
#include <CTaskComplexEnterCarAsPassenger.h>

CPlayerPed* m_pPed = nullptr;
int m_iPlayerId;

// last sync data
CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
CPackets::PlayerOnFoot* m_oOnFoot = nullptr;

signed short m_oShockButtonL = 0;
signed short m_lShockButtonL = 0;

CVector* m_vecWaypointPos = nullptr;
bool m_bWaypointPlaced = false;

char m_Name[32 + 1] = { 0 };

CNetworkPlayer::~CNetworkPlayer()
{
	delete m_lOnFoot;

	if (m_pPed == nullptr) return;

	this->DestroyPed();
}

CNetworkPlayer::CNetworkPlayer(int id, CVector position)
{
	m_iPlayerId = id;

	m_pPedClothesDesc.SetTextureAndModel("VEST", "VEST", 0);
	m_pPedClothesDesc.SetTextureAndModel("JEANSDENIM", "JEANS", 2);
	m_pPedClothesDesc.SetTextureAndModel("SNEAKERBINCBLK", "SNEAKER", 3);
	m_pPedClothesDesc.SetTextureAndModel("PLAYER_FACE", "HEAD", 1);

	CreatePed(id, position);


	m_lOnFoot = new CPackets::PlayerOnFoot();
}

void CNetworkPlayer::CreatePed(int id, CVector position)
{
	unsigned int actorId = 0;
	int playerId = id + 2;

	plugin::Command<Commands::CREATE_PLAYER>(playerId, position.x, position.y, position.z, &actorId);
	plugin::Command<Commands::GET_PLAYER_CHAR>(playerId, &actorId);

	m_pPed = (CPlayerPed*)CPools::GetPed(actorId);

	m_pPed->SetOrientation(0.0f, 0.0f, 0.0f);

	// set player immunies, he now dont cares about pain
	Command<Commands::SET_CHAR_PROOFS>(actorId, 0, 1, 1, 0, 0);

	*m_pPed->m_pPlayerData->m_pPedClothesDesc = m_pPedClothesDesc;

	CClothes::RebuildPlayer(m_pPed, false);
}

void CNetworkPlayer::DestroyPed()
{
	if (m_pPed->m_pVehicle)
	{
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(m_pPed), 0.f, 0.f, 0.f);
	}

	uintptr_t pedPtr = (uintptr_t)m_pPed;
	if (CUtil::IsValidEntityPtr(m_pPed))
	{
		CWorld::Remove(m_pPed);

		// destroy the ped
		__asm
		{
			mov ecx, pedPtr
			mov ebx, [ecx] // vtable addr
			push 1 // unused arg
			call[ebx] // call destructor
		}
	}
}

void CNetworkPlayer::Respawn()
{
	if (m_pPed)
	{
		this->DestroyPed();
	}

	CVector pos{};

	if (m_lOnFoot)
	{
		pos = m_lOnFoot->position;
	}

	this->CreatePed(m_iPlayerId, pos);
}

int CNetworkPlayer::GetInternalId() // most used for CWorld::PlayerInFocus
{
	byte playerNumber = 0;

	for (; playerNumber < MAX_SERVER_PLAYERS + 2; playerNumber++)
	{
		if (m_pPed == CWorld::Players[playerNumber].m_pPed)
		{
			return playerNumber;
		}
	}

	return -1;
}

char* CNetworkPlayer::GetName()
{
	char* buffer = new char[32 + 1];

	if (m_Name[0] == '\0')
		sprintf(buffer, "player %d", m_iPlayerId);
	else
		strcpy(buffer, m_Name);

	return buffer;
}

char CNetworkPlayer::GetWeaponSkill(eWeaponType weaponType)
{
	if (weaponType < WEAPON_PISTOL || weaponType > WEAPON_TEC9)
		return 1;

	eStats weaponStatId = plugin::CallAndReturn<eStats, 0x743CD0>(weaponType); // CWeaponInfo::GetSkillStatIndex
	int statSyncId = CStatsSync::GetSyncIdByInternal(weaponStatId);
	float weaponStat = m_stats[weaponStatId];

	if (CWeaponInfo::GetWeaponInfo(weaponType, 2)->m_nReqStatLevel <= weaponStat)
		return 2;

	return CWeaponInfo::GetWeaponInfo(weaponType, 1)->m_nReqStatLevel <= weaponStat;
}


void CNetworkPlayer::WarpIntoVehiclePassenger(CVehicle* vehicle, int seatid)
{
	assert(m_pPed != nullptr);

	if (!CUtil::IsValidEntityPtr(vehicle) || !CUtil::IsValidEntityPtr(m_pPed))
	{
		return;
	}

	if (m_pPed->m_nPedFlags.bInVehicle && m_pPed->m_pVehicle)
	{
		RemoveFromVehicle(m_pPed->m_pVehicle);
	}

	m_pPed->m_pIntelligence->FlushImmediately(false);

	m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1; // 1 - never
	
	int doorId = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(vehicle, seatid);
	auto task = CTaskSimpleCarSetPedInAsPassenger(vehicle, doorId, nullptr);
	task.m_bWarpingInToCar = true;
	task.ProcessPed(m_pPed);
}

void CNetworkPlayer::EnterVehiclePassenger(CVehicle* vehicle, int seatid)
{
	assert(m_pPed != nullptr);

	if (!CUtil::IsValidEntityPtr(vehicle) || !CUtil::IsValidEntityPtr(m_pPed))
	{
		return;
	}

	if (m_pPed->m_nPedFlags.bInVehicle && m_pPed->m_pVehicle)
	{
		RemoveFromVehicle(m_pPed->m_pVehicle);
	}

	m_pPed->m_pIntelligence->FlushImmediately(false);

	m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1; // 1 - never

	int doorId = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(vehicle, seatid);
	auto task = new CTaskComplexEnterCarAsPassenger(vehicle, doorId, false);
	m_pPed->m_pIntelligence->m_TaskMgr.SetTask(task, 3, false);
}

void CNetworkPlayer::RemoveFromVehicle(CVehicle* vehicle)
{
	assert(m_pPed != nullptr);

	if (!CUtil::IsValidEntityPtr(vehicle) || !CUtil::IsValidEntityPtr(m_pPed))
	{
		return;
	}

	m_pPed->m_pIntelligence->m_TaskMgr.SetTask(nullptr, TASK_PRIMARY_PRIMARY, false);

	m_pPed->m_nPedFlags.CantBeKnockedOffBike = 2; // 2 - normal

	auto task = CTaskSimpleCarSetPedOut(vehicle, 1, false);
	task.m_bWarpingOutOfCar = true;
	task.ProcessPed(m_pPed);
}

void CNetworkPlayer::UpdateHeading(float heading)
{
	m_pPed->m_fAimingRotation = heading;

	if (fabs(m_pPed->m_fCurrentRotation - heading) > M_PI_2)
	{
		m_pPed->m_fCurrentRotation = heading;
	}
}