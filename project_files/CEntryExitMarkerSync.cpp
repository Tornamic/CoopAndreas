#include "stdafx.h"
#include "CEntryExitMarkerSync.h"
#include "CEntryExitManager.h"
#include <unordered_set>

void CEntryExitMarkerSync::Send()
{
    std::vector<uint8_t> buffer;
    uint16_t count = 0;
    size_t estimatedSize = sizeof(uint16_t);

    for (auto entryExit : CEntryExitManager::mp_poolEntryExits)
    {
        count++;
        estimatedSize += sizeof(float) + sizeof(float) + sizeof(uint16_t);
    }

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
        buffer.insert(buffer.end(), (uint8_t*)&entryExit->m_recEntrance.left, (uint8_t*)&entryExit->m_recEntrance.left + sizeof(float));
        buffer.insert(buffer.end(), (uint8_t*)&entryExit->m_recEntrance.bottom, (uint8_t*)&entryExit->m_recEntrance.bottom + sizeof(float));
        buffer.insert(buffer.end(), (uint8_t*)&entryExit->m_nFlags, (uint8_t*)&entryExit->m_nFlags + sizeof(uint16_t));

        if (*(uint16_t*)&entryExit->m_nFlags == 0xFDFD)
        {
            CChat::AddMessage("flags == 0xFDFD");
        }
    }

    std::cout << "Final buffer size: " << estimatedSize << " bytes" << std::endl;

    CNetwork::SendPacket(CPacketsID::ENEX_SYNC, buffer.data(), estimatedSize, ENET_PACKET_FLAG_RELIABLE);
}

void CEntryExitMarkerSync::Receive(void* data, size_t size)
{
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

    int i = 0;
    for (size_t i = 0; i < count; ++i)
    {
        float x, y;

        memcpy(&x, buffer + offset, sizeof(float));
        offset += sizeof(float);

        memcpy(&y, buffer + offset, sizeof(float));
        offset += sizeof(float);

        uint16_t flags;
        memcpy(&flags, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        for (auto entryExit : CEntryExitManager::mp_poolEntryExits)
        {
            if(entryExit->m_recEntrance.left == x 
                && entryExit->m_recEntrance.bottom == y)
            {
                memcpy(&entryExit->m_nFlags, &flags, 2);
                if (flags == 0xFDFD)
                {
                    CChat::AddMessage("flags == 0xFDFD #%d", i);
                }
                i++;
            }
        }
    }
}
