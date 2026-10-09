#include "stdafx.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"

std::vector<CNetworkPed*> CNetworkPedManager::m_pPeds;
CNetworkPed* CNetworkPedManager::m_apTempPeds[255];

CNetworkPed* CNetworkPedManager::GetPed(int pedid)
{
    for (int i = 0; i != m_pPeds.size(); i++)
    {
        if (m_pPeds[i]->m_nPedId == pedid && m_pPeds[i]->HasValidPed())
        {
            return m_pPeds[i];
        }
    }
    return nullptr;
}

CNetworkPed* CNetworkPedManager::GetPed(CEntity* entity)
{
    if (entity == nullptr)
        return nullptr;

    for (int i = 0; i != m_pPeds.size(); i++)
    {
        if (m_pPeds[i]->m_pPed == entity && m_pPeds[i]->HasValidPed())
        {
            return m_pPeds[i];
        }
    }

    return nullptr;
}

bool CNetworkPedManager::IsPedTracked(CPed* pPed)
{
    if (!pPed)
        return false;

    for (CNetworkPed* pNetworkPed : m_pPeds)
    {
        if (pNetworkPed && pNetworkPed->m_pPed == pPed && pNetworkPed->HasValidPed())
            return true;
    }

    for (CNetworkPed* pNetworkPed : m_apTempPeds)
    {
        if (pNetworkPed && pNetworkPed->m_pPed == pPed && pNetworkPed->HasValidPed())
            return true;
    }

    return false;
}

void CNetworkPedManager::Add(CNetworkPed* ped)
{
    CNetworkPedManager::m_pPeds.push_back(ped);
}

void CNetworkPedManager::Remove(CNetworkPed* ped)
{
    auto it = std::find(m_pPeds.begin(), m_pPeds.end(), ped);
    if (it != m_pPeds.end())
    {
        m_pPeds.erase(it);
    }
}

void CNetworkPedManager::HandlePedDestruction(CPed* pPed)
{
    if (!pPed)
        return;

    // At the native destructor boundary, every wrapper retaining this address is stale,
    // including wrappers from earlier pool generations.
    for (auto it = m_pPeds.begin(); it != m_pPeds.end();)
    {
        CNetworkPed* pNetworkPed = *it;
        if (!pNetworkPed || pNetworkPed->m_pPed != pPed)
        {
            ++it;
            continue;
        }

        pNetworkPed->CancelClaim();
        pNetworkPed->DetachPed();
        it = m_pPeds.erase(it);
        delete pNetworkPed;
    }

    for (CNetworkPed* pNetworkPed : m_apTempPeds)
    {
        if (!pNetworkPed || pNetworkPed->m_pPed != pPed)
            continue;

        pNetworkPed->DetachPed();
    }
}

void CNetworkPedManager::Update()
{
    CNetworkPedManager::RemoveInvalidPeds();

    for (CNetworkPed* pNetworkPed : m_pPeds)
    {
        if (!pNetworkPed->m_bSyncing)
            continue;

        CPed* pPed = pNetworkPed->m_pPed;
        if (!pPed)
            continue;

        CVehicle* pVehicle = pPed->m_pVehicle;
        CNetworkVehicle* pNetworkVehicle = pVehicle ? CNetworkVehicleManager::GetVehicle(pVehicle) : nullptr;

        if (pNetworkVehicle && pPed->m_nPedFlags.bInVehicle)
        {
            bool isDriver = (pVehicle->m_pDriver == pPed);

            if (isDriver)
            {
                Packets::Peds::PedDriverUpdate packet{};

                packet.vehicleSubType = static_cast<eVehicleType>(pVehicle->m_nVehicleSubType);

                packet.pedid = pNetworkPed->m_nPedId;
                packet.vehicleid = pNetworkVehicle->m_nVehicleId;

                packet.pos = pVehicle->m_matrix->pos;
                packet.roll = pVehicle->m_matrix->right;
                packet.rot = pVehicle->m_matrix->up;
                packet.velocity = pVehicle->m_vecMoveSpeed;
                packet.turnSpeed = pVehicle->m_vecTurnSpeed;

                packet.pedHealth.iHealth = static_cast<uint8_t>(std::clamp(pPed->m_fHealth, 0.0f, 255.0f));
                packet.pedHealth.iArmour = static_cast<uint8_t>(std::clamp(pPed->m_fArmour, 0.0f, 255.0f));

                CWeapon& weapon = pPed->GetWeapon();
                packet.pedWeapon.iWeaponType = weapon.m_eWeaponType;
                packet.pedWeapon.iWeaponState = weapon.m_nState;
                packet.pedWeapon.nAmmo = weapon.m_nAmmoInClip;

                packet.color1 = pVehicle->m_nPrimaryColor;
                packet.color2 = pVehicle->m_nSecondaryColor;

                packet.health = pVehicle->m_fHealth;

                packet.paintjob = pVehicle->GetRemapIndex();

                if (pVehicle->m_nVehicleType == VEHICLE_BIKE)
                {
                    CBike* pBike = (CBike*)pVehicle;
                    packet.bikeLean = pBike->m_rideAnimData.m_fDesiredLeanAngle;
                }
                if (pVehicle->m_nVehicleSubType == VEHICLE_PLANE)
                {
                    CPlane* pPlane = (CPlane*)pVehicle;
                    packet.planeGearState = pPlane->m_fLandingGearStatus;
                }
                if (pVehicle->m_nVehicleSubType == eVehicleType::VEHICLE_BMX)
                {
                    CBmx* pBmx = (CBmx*)pVehicle;
                    packet.controlPedaling = pBmx->m_fControlPedaling;
                }
                if (pVehicle->m_nVehicleType == VEHICLE_AUTOMOBILE)
                {
                    CAutomobile* pAutomobile = (CAutomobile*)pVehicle;
                    packet.bikeLean = pAutomobile->m_wMiscComponentAngle;
                }
                packet.locked = pVehicle->m_eDoorLock;
                packet.gasPedal = pVehicle->m_fGasPedal;
                packet.breakPedal = pVehicle->m_fBreakPedal;
                packet.steerAngle = pVehicle->m_fSteerAngle;

                GetPacketFactory().Send(packet);
            }
            else
            {
                Packets::Peds::PedPassengerSync packet{};
                packet.pedid = pNetworkPed->m_nPedId;
                packet.vehicleid = pNetworkVehicle->m_nVehicleId;

                packet.healthSnapshot.iHealth = static_cast<uint8_t>(std::clamp(pPed->m_fHealth, 0.0f, 255.0f));
                packet.healthSnapshot.iArmour = static_cast<uint8_t>(std::clamp(pPed->m_fArmour, 0.0f, 255.0f));

                CWeapon& weapon = pPed->GetWeapon();
                packet.weaponSnapshot.iWeaponType = weapon.m_eWeaponType;
                packet.weaponSnapshot.iWeaponState = weapon.m_nState;
                packet.weaponSnapshot.nAmmo = weapon.m_nAmmoInClip;

                for (int i = 0; i < pNetworkVehicle->m_pVehicle->m_nMaxPassengers; i++)
                {
                    if (pNetworkVehicle->m_pVehicle->m_apPassengers[i] == pNetworkPed->m_pPed)
                    {
                        packet.seatid = i;
                        break;
                    }
                }
                GetPacketFactory().Send(packet);
            }
        }
        else
        {
            Packets::Peds::PedOnFoot packet{};

            packet.pedid = pNetworkPed->m_nPedId;
            packet.pos = pPed->m_matrix->pos;
            packet.velocity = pPed->m_vecMoveSpeed;

            packet.healthSnapshot.iHealth = static_cast<uint8_t>(std::clamp(pPed->m_fHealth, 0.0f, 255.0f));
            packet.healthSnapshot.iArmour = static_cast<uint8_t>(std::clamp(pPed->m_fArmour, 0.0f, 255.0f));

            CWeapon& weapon = pPed->GetWeapon();
            packet.weaponSnapshot.iWeaponType = weapon.m_eWeaponType;
            packet.weaponSnapshot.iWeaponState = weapon.m_nState;
            packet.weaponSnapshot.nAmmo = weapon.m_nAmmoInClip;

            packet.aimingRotation = pPed->m_fAimingRotation;
            packet.currentRotation = pPed->m_fCurrentRotation;
            packet.lookDirection = pPed->m_fLookDirection;
            packet.moveState = (eMoveState)pPed->m_nMoveState;

            packet.bDucked = CUtil::IsDucked(pPed);

            CTaskSimpleUseGun* useGun = pPed->m_pIntelligence->GetTaskUseGun();

            if (useGun)
            {
                packet.bAiming = true;
                packet.weaponAim = useGun->m_pTarget && (useGun->m_vecTarget.x == 0.f || useGun->m_vecTarget.y == 0.f)
                                       ? useGun->m_pTarget->GetPosition()
                                       : useGun->m_vecTarget;
            }

            packet.fightingStyle = pPed->m_nFightingStyle;

            GetPacketFactory().Send(packet);
        }
    }
}

void CNetworkPedManager::Process()
{
    for (auto networkPed : m_pPeds)
    {
        if (!networkPed->HasValidPed())
            continue;

        if (networkPed->m_bSyncing)
            continue;

        CPed* ped = networkPed->m_pPed;

        if (ped == nullptr)
            continue;

        if (!ped->m_nPedFlags.bInVehicle)
        {
            ped->m_fAimingRotation = networkPed->m_fAimingRotation;
            ped->m_fCurrentRotation = networkPed->m_fCurrentRotation;
            ped->m_fLookDirection = networkPed->m_fLookDirection;
        }
    }
}

void CNetworkPedManager::AssignHost()
{
    /*for (auto networkPed : m_pPeds)
    {
        CPed* ped = networkPed->m_pPed;

        if (ped)
        {
            ped->SetCharCreatedBy(networkPed->m_nCreatedBy);

            CTaskComplexWander* task = plugin::CallAndReturn<CTaskComplexWander*, 0x673D00>(ped); //
    GetWanderTaskByPedType ped->m_pIntelligence->m_TaskMgr.SetTask(task, 0, false);
        }
    }*/
}

unsigned char CNetworkPedManager::AddToTempList(CNetworkPed* networkPed)
{
    for (unsigned char i = 0; i < 255; i++)
    {
        if (m_apTempPeds[i] == nullptr)
        {
            m_apTempPeds[i] = networkPed;
            return i;
        }
    }

    return 255;
}

void CNetworkPedManager::RemoveInvalidPeds()
{
    for (auto it = CNetworkPedManager::m_pPeds.begin(); it != CNetworkPedManager::m_pPeds.end();)
    {
        CNetworkPed* pNetworkPed = *it;
        if (!pNetworkPed->HasValidPed())
        {
            pNetworkPed->CancelClaim();
            pNetworkPed->DetachPed();
            it = m_pPeds.erase(it);
            delete pNetworkPed;
            continue;
        }
        ++it;
    }

    for (CNetworkPed* pNetworkPed : m_apTempPeds)
    {
        if (!pNetworkPed || !pNetworkPed->m_pPed || pNetworkPed->HasValidPed())
            continue;

        pNetworkPed->DetachPed();
    }
}
