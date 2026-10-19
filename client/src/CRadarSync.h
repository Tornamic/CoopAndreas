#pragma once

class CRadarSync
{
public:
    // invalid (zero) means the blip is visible for everyone
    inline static playerid_t ms_aForWho[MAX_RADAR_TRACES]{};

    // created by the game for this player only (waypoint, airstrip, group members), never synced,
    // allocated from the end of the array to not collide with the host's blips
    inline static bool ms_abLocal[MAX_RADAR_TRACES]{};

    inline static bool ms_abDirty[MAX_RADAR_TRACES]{};

    // the last known entity of BLIP_CAR and BLIP_CHAR, used to resolve the local entity handle
    inline static CNetworkEntitySerializer ms_aEntity[MAX_RADAR_TRACES]{};

    // the last known entity position of BLIP_OBJECT and BLIP_PICKUP
    inline static CVector ms_avecEntityPosn[MAX_RADAR_TRACES]{};

    static void InjectHooks();

    static int SetLocalCoordBlip(eBlipType type, CVector vecPosn, unsigned int nColour, eBlipDisplay display);
    static int SetLocalEntityBlip(eBlipType type, int nEntityHandle, unsigned int nColour, eBlipDisplay display);
    static void ClearLocalBlip(int nBlipHandle);

    static void SetBlipVisibleTo(int nBlipHandle, CPed* pPed);
    static bool IsTraceVisible(int nIndex);

    static void MarkDirty(int nIndex);
    static void SendChangedTraces();
    static void SendAllTraces();
    static void HandlePacket(const Packets::Scripts::RadarTraceUpdate& packet);
    static void HandlePacket(const Packets::Scripts::UpdateAllRadarTraces& packet);
};
