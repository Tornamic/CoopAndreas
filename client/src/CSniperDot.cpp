#include "stdafx.h"
#include "CSniperDot.h"
#include <CCoronas.h>

static constexpr float DOT_RANGE = 250.0f;
static constexpr float DOT_SIZE = 0.12f;
static constexpr unsigned int CORONA_ID_BASE = 0xC00A0000;  // + the player's internal id, clear of the game's ids

void CSniperDot::Process()
{
    for (CNetworkPlayer* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        CPlayerPed* pPed = pNetworkPlayer->m_pPed;
        if (!pPed || pPed->m_nPedFlags.bInVehicle || pPed->GetWeapon().m_eWeaponType != WEAPON_SNIPERRIFLE)
        {
            continue;
        }

        const Packets::Players::PlayerCameraSync& camera = pNetworkPlayer->m_cameraSnapshot;
        if (camera.cameraMode != MODE_SNIPER)
        {
            continue;
        }

        CVector front = camera.front;
        front.Normalise();
        CVector start = camera.source;
        CVector end = start + front * DOT_RANGE;

        CColPoint colPoint{};
        CEntity* pHit = nullptr;
        CEntity* pIgnored = CWorld::pIgnoreEntity;
        CWorld::pIgnoreEntity = pPed;
        bool bHit = CWorld::ProcessLineOfSight(start, end, colPoint, pHit, true, true, true, true, false, false, false, true);
        CWorld::pIgnoreEntity = pIgnored;
        if (!bHit)
        {
            continue;
        }

        // a little off the surface, or the corona sinks into it
        CVector dot = colPoint.m_vecPoint - front * 0.05f;
        CCoronas::RegisterCorona(CORONA_ID_BASE + pNetworkPlayer->GetInternalId(), nullptr, 255, 0, 0, 255, dot, DOT_SIZE,
            DOT_RANGE, CORONATYPE_SHINYSTAR, FLARETYPE_NONE, false, false, 0, 0.0f, false, 0.1f, 0, 15.0f, false, false);
    }
}
