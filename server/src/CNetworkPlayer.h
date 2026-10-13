#pragma once

#include "enet/enet.h"

#include "CVector.h"
#include "CNetwork.h"
#include <string>
#include <network/ids.h>
#include <network/packets/players.h>
#include <CPedClothesDesc.h>
class CNetworkPed;
class CNetworkPlayer
{
public:
    CNetworkPlayer(ENetPeer* peer, playerid_t playerid);

    std::string GetName();

    ENetPeer* m_pPeer;
    playerid_t m_playerId;
    char m_Name[32 + 1] = {0};
    bool m_bIsHost = false;
    int8_t m_nSeatId = -1;
    vehicleid_t m_vehicleId{};
    bool m_bCorrectVersion = false;
    float m_afStats[14]{};
    Packets::Players::PlayerPlaceWaypoint m_waypointState{};
    CPedClothesDesc m_pPedClothesDesc{};

    struct
    {
        uint8_t bStatsModified : 1;
        uint8_t bClothesModified : 1;
    } m_ucSyncFlags;
    std::vector<CNetworkPed*> m_vPedClaims;

    void RemoveFromVehicle();

    ~CNetworkPlayer();
};