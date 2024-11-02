#pragma once
#include "../../structs.h"
#include "../CNetworkEntity.h"
#include "../Data/PlayerSyncData.h"
#include <CPlayerPed.h>

class CNetworkPlayer : public CNetworkEntity<PlayerSyncData, CPlayerPed>
{
public:
	CNetworkPlayer(uint16_t networkId) :
		CNetworkEntity(networkId) {}

	
	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_PLAYER; }
	
	CVector m_vecWaypointPos;
	bool m_bWaypointPlaced;

	char m_name[32 + 1] = { 0 };

	CVector m_aPassengerAim;

	CControllerState m_oldControllerState;
	CControllerState m_newControllerState;

	ÑCompressedControllerState m_compressedControllerState;

	int GetInternalId();
	char* GetName();

	~CNetworkPlayer();

	void StreamIn() override;
	void StreamOut() override;
	void Create() override;
	void Destroy() override;
};

