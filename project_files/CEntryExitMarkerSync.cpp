#include "stdafx.h"
#include "CEntryExitMarkerSync.h"
#include "CEntryExitManager.h"
#include <unordered_set>

struct SEntryExitFlags
{
    unsigned short bUnknownInterior : 1;
    unsigned short bUnknownPairing : 1;
    unsigned short bCreateLinkedPair : 1;
    unsigned short bRewardInterior : 1;
    unsigned short bUsedRewardEntrance : 1;
    unsigned short bCarsAndAircraft : 1;
    unsigned short bBikesAndMotorcycles : 1;
    unsigned short bDisableOnFoot : 1;

    unsigned short bAcceptNpcGroup : 1;
    unsigned short bFoodDateFlag : 1;
    unsigned short bUnknownBurglary : 1;
    unsigned short bDisableExit : 1;
    unsigned short bBurglaryAccess : 1;
    unsigned short bEnteredWithoutExit : 1;
    unsigned short bEnableAccess : 1;
    unsigned short bDeleteEnex : 1;
};

void CEntryExitMarkerSync::Send()
{
    std::vector<uint8_t> buffer;
    uint16_t count = 0;
    //                          disabled       burglary          count
    size_t estimatedSize = sizeof(bool) + sizeof(bool) + sizeof(uint16_t);

    for (auto entryExit : CEntryExitManager::mp_poolEntryExits)
    {
        count++;
        //                      areaId            rec.left         rec.bottom         flags
        estimatedSize += sizeof(uint8_t) + sizeof(int16_t) + sizeof(int16_t) + sizeof(uint16_t);
    }

    //CChat::AddMessage("count: %d", count);

    buffer.clear();
    buffer.reserve(estimatedSize);

    buffer.push_back(CEntryExitManager::ms_bDisabled);
    buffer.push_back(CEntryExitManager::ms_bBurglaryHousesEnabled);

    buffer.insert(buffer.end(), (uint8_t*)&count, (uint8_t*)&count + sizeof(uint16_t));

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("CEntryExitMarkerSync::Send %d %d", CEntryExitManager::ms_bDisabled, CEntryExitManager::ms_bBurglaryHousesEnabled);
#endif


    for (auto entryExit : CEntryExitManager::mp_poolEntryExits)
    {
        int16_t left   = (int16_t)std::floor(entryExit->m_recEntrance.left);
        int16_t bottom = (int16_t)std::floor(entryExit->m_recEntrance.bottom);

        buffer.push_back(entryExit->m_nArea);
        buffer.insert(buffer.end(), (uint8_t*)&left, (uint8_t*)&left + sizeof(int16_t));
        buffer.insert(buffer.end(), (uint8_t*)&bottom, (uint8_t*)&bottom + sizeof(int16_t));
        buffer.insert(buffer.end(), (uint8_t*)&entryExit->m_nFlags, (uint8_t*)&entryExit->m_nFlags + sizeof(uint16_t));
    }

    /*CChat::AddMessage("Final buffer size: es%d buf%d bytes", estimatedSize, buffer.size());

    std::ofstream outFile("file_test_s.bin", std::ios::binary | std::ios::out);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        outFile.close();
        CChat::AddMessage("Data successfully saved to file_test_s.bin");
    }
    else
    {
        CChat::AddMessage("Failed to open file for writing");
    }*/

    CNetwork::SendPacket(CPacketsID::ENEX_SYNC, buffer.data(), estimatedSize, ENET_PACKET_FLAG_RELIABLE);
}

void CEntryExitMarkerSync::Receive(void* data, size_t size)
{
    //CChat::AddMessage("Final buffer size: %d bytes", size);

    ms_vLastData.assign((uint8_t*)data, (uint8_t*)data + size);

    uint8_t* buffer = static_cast<uint8_t*>(data);
    size_t offset = 0;
        
    memcpy(&CEntryExitManager::ms_bDisabled, buffer + offset, sizeof(bool));
    offset += sizeof(bool);

    memcpy(&CEntryExitManager::ms_bBurglaryHousesEnabled, buffer + offset, sizeof(bool));
    offset += sizeof(bool);

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("CEntryExitMarkerSync::Receive %d %d", CEntryExitManager::ms_bDisabled, CEntryExitManager::ms_bBurglaryHousesEnabled);
#endif

    uint16_t count;
    memcpy(&count, buffer + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    //CChat::AddMessage("count: %d", count);

    int i = 0;
    for (size_t i = 0; i < count; ++i)
    {
        uint8_t area;
        int16_t left, bottom;

        memcpy(&area, buffer + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        memcpy(&left, buffer + offset, sizeof(int16_t));
        offset += sizeof(int16_t);

        memcpy(&bottom, buffer + offset, sizeof(int16_t));
        offset += sizeof(int16_t);

        SEntryExitFlags flags;
        memcpy(&flags, buffer + offset, sizeof(SEntryExitFlags));
        offset += sizeof(SEntryExitFlags);

        for (auto entryExit : CEntryExitManager::mp_poolEntryExits)
        {
            if ((int16_t)std::floor(entryExit->m_recEntrance.left) == left && (int16_t)std::floor(entryExit->m_recEntrance.bottom) == bottom && entryExit->m_nArea == area)
            {
                memcpy(&entryExit->m_nFlags, &flags, 2);
            }
        }
    }

    /*std::ofstream outFile("file_test_c.bin", std::ios::binary | std::ios::out);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<const char*>(data), size);
        outFile.close();
        CChat::AddMessage("Data successfully saved to file_test_c.bin");
    }
    else
    {
        CChat::AddMessage("Failed to open file for writing");
    }*/

}
