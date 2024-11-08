#include "../../CUtil.h"
#include "../Manager/Types/CNetworkPlayerManager.h"
#include "CNetworkPlayer.h"
#include "../../stdafx.h"

CNetworkPlayer::~CNetworkPlayer()
{
	if(this->GetEntity())
		this->Destroy();
}

void CNetworkPlayer::Create()
{
	uint32_t actorId = 0;
	int32_t playerId = this->GetId() + 2;

	auto& syncData = this->GetSyncData();

	plugin::Command<Commands::CREATE_PLAYER>(playerId, syncData.m_vecPosition.x, syncData.m_vecPosition.y, syncData.m_vecPosition.z, &actorId);
	plugin::Command<Commands::GET_PLAYER_CHAR>(playerId, &actorId);

	this->m_pEntity = (CPlayerPed*)CPools::GetPed(actorId);

	this->m_pEntity->SetOrientation(0.0f, 0.0f, 0.0f);

	plugin::Command<Commands::SET_CHAR_PROOFS>(actorId, 0, 1, 1, 0, 0);
}

void CNetworkPlayer::Destroy()
{
	if (this->m_pEntity == nullptr) return;

	if (this->m_pEntity->m_pVehicle != nullptr)
	{
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(this->m_pEntity), 0.f, 0.f, 0.f);
	}

	CWorld::Remove(m_pEntity);
	delete m_pEntity; // possible crash (todo: test)
}

int CNetworkPlayer::GetInternalId()
{
	byte playerNumber = 0;

	for (; playerNumber < CNetworkPlayerManager::Instance().GetMaxCount() + 2; playerNumber++)
	{
		if (this->m_pEntity == CWorld::Players[playerNumber].m_pPed)
		{
			return playerNumber;
		}
	}

	return -1;
}

char* CNetworkPlayer::GetName()
{
	char* buffer = new char[32 + 1];

	if (m_name[0] == '\0')
		sprintf(buffer, "player %d", this->GetId());
	else
		strcpy(buffer, m_name);

	return buffer;
}

void CNetworkPlayer::StreamIn()
{
	if (this->GetEntity() == nullptr)
	{
		this->Create();
	}

	auto ped = this->GetEntity();
	auto& syncData = this->GetSyncData();

	ped->m_vecMoveSpeed = syncData.m_vecVelocity;
	ped->m_fCurrentRotation = ped->m_fAimingRotation = syncData.m_fRotation;
	ped->m_fHealth = syncData.m_nHealth;
	ped->m_fArmour = syncData.m_nArmour;
	CUtil::GiveWeaponByPacket(this, syncData.m_nCurrentWeapon, syncData.m_nAmmoInClip);
	CUtil::SetPlayerJetpack(this, syncData.m_bHasJetpack);
}

void CNetworkPlayer::StreamOut()
{
	if (this->GetEntity())
	{
		this->Destroy();
	}
}