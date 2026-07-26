#include "stdafx.h"

void CLocalPlayer::BuildTaskPacket(eTaskType type, bool toggle)
{
	CPlayerPed* pPlayerPed = FindPlayerPed(0);
	Packets::Players::SetPlayerTask packet{};
	packet.taskType = type;
	packet.vecPos = pPlayerPed->GetPosition();
	packet.currentRotation = pPlayerPed->m_fCurrentRotation;
	packet.aimingRotation = pPlayerPed->m_fAimingRotation;
	packet.toggle = toggle;
	GetPacketFactory().Send(packet);
}

bool CLocalPlayer::GetIsHostingEntity(CEntity* pEntity)
{
    if (pEntity->m_nType == ENTITY_TYPE_PED)
    {
        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pEntity);
        if (pNetworkPlayer)
        {
            return false;
        }

        CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pEntity);
        if (!pNetworkPed)
        {
            return true;
        }
        if (pNetworkPed->m_bSyncing)
        {
            return true;
        }
    }
    else if (pEntity->m_nType == ENTITY_TYPE_VEHICLE)
    {
        CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pEntity);
        if (!pNetworkVehicle)
        {
            return true;
        }
        if (pNetworkVehicle && pNetworkVehicle->m_bSyncing)
        {
            return true;
        }
    }
    else
    {
        return true;
    }

    return false;
}