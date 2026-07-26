#include "stdafx.h"
#include "CEntryExitMarkerSync.h"
#include "CEntryExitManager.h"

void CEntryExitMarkerSync::Send()
{
    Packets::Scripts::EnExSync packet{};
    packet.bDisabled = CEntryExitManager::ms_bDisabled;
    packet.bBurglary = CEntryExitManager::ms_bBurglaryHousesEnabled;
    packet.count = 0;

    for (auto pEntryExit : CEntryExitManager::mp_poolEntryExits)
    {
        packet.enexes[packet.count].areaId = pEntryExit->m_nArea;
        packet.enexes[packet.count].flags = pEntryExit->m_nFlags;
        packet.enexes[packet.count].rectLeft = static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.left));
        packet.enexes[packet.count].rectBottom = static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.bottom));

        packet.count++;
    }

    GetPacketFactory().Send(packet);

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("CEntryExitMarkerSync::Send %d %d %d", CEntryExitManager::ms_bDisabled,
        CEntryExitManager::ms_bBurglaryHousesEnabled, packet.count);
#endif
}

void CEntryExitMarkerSync::Receive(const Packets::Scripts::EnExSync& packet)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("CEntryExitMarkerSync::Receive %d %d %d", CEntryExitManager::ms_bDisabled,
        CEntryExitManager::ms_bBurglaryHousesEnabled, packet.count);
#endif

    ms_lastData = packet;
    CEntryExitManager::ms_bDisabled = packet.bDisabled;
    CEntryExitManager::ms_bBurglaryHousesEnabled = packet.bBurglary;
    for (size_t i = 0; i < packet.count; i++)
    {
        for (auto pEntryExit : CEntryExitManager::mp_poolEntryExits)
        {
            if (static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.left)) == packet.enexes[i].rectLeft &&
                static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.bottom)) == packet.enexes[i].rectBottom &&
                pEntryExit->m_nArea == packet.enexes[i].areaId)
            {
                pEntryExit->m_nFlags = packet.enexes[i].flags;
            }
        }
    }
}
