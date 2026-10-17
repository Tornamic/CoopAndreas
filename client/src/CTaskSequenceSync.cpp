#include "stdafx.h"
#include "CTaskSequenceSync.h"
#include <game_sa/CTaskSequences.h>
#include <COpCodeSync.h>

// TODO: SET_SEQUENCE_TO_REPEAT

SSyncedOpCode m_syncedTasks[] =  // TODO
    {
        {COMMAND_TASK_PLAY_ANIM_NON_INTERRUPTABLE, true, eSyncedParamType::PED},
        {COMMAND_TASK_GO_STRAIGHT_TO_COORD, true, eSyncedParamType::PED},
        {COMMAND_TASK_ACHIEVE_HEADING, true, eSyncedParamType::PED},
        {COMMAND_TASK_JUMP, true, eSyncedParamType::PED},
        {COMMAND_TASK_TIRED, true, eSyncedParamType::PED},
        {COMMAND_TASK_PLAY_ANIM_SECONDARY, true, eSyncedParamType::PED},
        {COMMAND_TASK_CHAR_SLIDE_TO_COORD_AND_PLAY_ANIM, true, eSyncedParamType::PED},
        {COMMAND_TASK_PLAY_ANIM, true, eSyncedParamType::PED},
        {COMMAND_TASK_TURN_CHAR_TO_FACE_CHAR, true, eSyncedParamType::PED, eSyncedParamType::PED},
        {COMMAND_TASK_SHOOT_AT_COORD, true, eSyncedParamType::PED},
        {COMMAND_TASK_LOOK_AT_CHAR, true, eSyncedParamType::PED, eSyncedParamType::PED},
        {COMMAND_CLEAR_CHAR_TASKS, true, eSyncedParamType::PED},
        {COMMAND_CLEAR_LOOK_AT, true, eSyncedParamType::PED},
        {COMMAND_TASK_STAND_STILL, true, eSyncedParamType::PED},
        {COMMAND_TASK_LOOK_AT_COORD, true, eSyncedParamType::PED},
        {COMMAND_TASK_LEAVE_CAR, true, eSyncedParamType::PED, eSyncedParamType::VEHICLE},
        {COMMAND_TASK_DIE_NAMED_ANIM, true, eSyncedParamType::PED},
        {COMMAND_TASK_PLAY_ANIM_WITH_FLAGS, true, eSyncedParamType::PED},
        {COMMAND_TASK_LEAVE_ANY_CAR, true, eSyncedParamType::PED},
        {COMMAND_TASK_LOOK_ABOUT, true, eSyncedParamType::PED},
        {COMMAND_TASK_WANDER_STANDARD, true, eSyncedParamType::PED},
        {COMMAND_TASK_PAUSE, true, eSyncedParamType::PED},
        {COMMAND_TASK_ENTER_CAR_AS_PASSENGER, true, {eSyncedParamType::PED, eSyncedParamType::VEHICLE}},
        {COMMAND_TASK_ENTER_CAR_AS_DRIVER, true, {eSyncedParamType::PED, eSyncedParamType::VEHICLE}},
        {COMMAND_TASK_KILL_CHAR_ON_FOOT, true, {eSyncedParamType::PED, eSyncedParamType::PED}},
        {COMMAND_TASK_DRIVE_BY, true, {eSyncedParamType::PED, eSyncedParamType::PED, eSyncedParamType::VEHICLE}},
        {COMMAND_TASK_CLIMB, true, {eSyncedParamType::PED}},
        
};

std::vector<uint8_t> m_serializedSequences[CTaskSequences::NUM_SEQUENCES][CTaskSequences::NUM_TASKS];
int m_sequenceTaskCount = 0;
bool m_bSequenceOpened = false;

bool CTaskSequenceSync::IsOpCodeTaskSynced(eScriptCommands opcode)
{
    for (int i = 0; i < ARRAY_SIZE(m_syncedTasks); i++)
    {
        if (m_syncedTasks[i].m_wOpCode == opcode)
        {
            return true;
        }
    }
    return false;
}

bool CTaskSequenceSync::IsNeededToCollectParametes(eScriptCommands opcode)
{
    return !COpCodeSync::bProcessingNetworkOpcode &&
           (opcode == COMMAND_PERFORM_SEQUENCE_TASK || opcode == COMMAND_CLEAR_SEQUENCE_TASK ||
               (IsOpCodeTaskSynced(opcode) && m_bSequenceOpened));
}

void OpenSequence()
{
    m_sequenceTaskCount = 0;
    m_bSequenceOpened = true;

    int sequenceId = CTaskSequences::ms_iActiveSequence;

    for (int i = 0; i < CTaskSequences::NUM_TASKS; ++i)
    {
        m_serializedSequences[sequenceId][i].clear();
    }
}

void CloseSequence()
{
    m_sequenceTaskCount = 0;
    m_bSequenceOpened = false;
}

void ClearSequence()
{
    int sequenceId =
        CTheScripts::GetActualScriptThingIndex(COpCodeSync::scriptParamsBuffer[0].value, SCRIPT_THING_SEQUENCE_TASK);

    for (int i = 0; i < CTaskSequences::NUM_TASKS; ++i)
    {
        m_serializedSequences[sequenceId][i].clear();
    }
}

void PerformSequence()
{
    int pedHandle = COpCodeSync::scriptParamsBuffer[0].value;
    auto ped = CPools::GetPed(pedHandle);

    if (!ped)
        return;

    pedid_t pedid{};  // invalid means the player

    if (!ped->IsPlayer())
    {
        if (auto networkPed = CNetworkPedManager::GetPed(ped))
        {
            pedid = networkPed->m_pedId;
        }

        if (!pedid.valid())
            return;
    }

    int sequenceId =
        CTheScripts::GetActualScriptThingIndex(COpCodeSync::scriptParamsBuffer[1].value, SCRIPT_THING_SEQUENCE_TASK);

    /*
    packet structure:
    // header (Packets::Scripts::PerformTaskSequence::HEADER_SIZE):
    playerid_t playerid (4 bytes, filled by the server)
    pedid_t pedid (8 bytes, packed, invalid if the sequence is performed on the player)
    uint8_t taskCount
    // for each task:
    uint8_t size
    uint8_t serializedData[size]
    */

    int packetSize = 0;
    uint8_t taskCount = 0;

    for (int i = 0; i < CTaskSequences::NUM_TASKS; i++)
    {
        if (m_serializedSequences[sequenceId][i].empty())
            break;

        taskCount++;
        packetSize += sizeof(uint8_t) + m_serializedSequences[sequenceId][i].size();  // size + task
    }

    if (taskCount == 0)
        return;

    packetSize += Packets::Scripts::PerformTaskSequence::HEADER_SIZE;  // playerid + pedid + taskCount

    std::vector<uint8_t> vBuffer(Packets::Scripts::PerformTaskSequence::HEADER_SIZE);
    vBuffer.reserve(packetSize);

    ids::WritePlayerId(&vBuffer[0], playerid_t());  // let the server fill this variable
    ids::WriteEntityId(
        &vBuffer[ids::PLAYERID_SIZE], pedid);       // if pedid is invalid then perfom the sequence on the player
    vBuffer[ids::PLAYERID_SIZE + ids::ENTITYID_SIZE] = taskCount;

    for (int i = 0; i < taskCount; i++)
    {
        vBuffer.push_back((uint8_t)m_serializedSequences[sequenceId][i].size());
        vBuffer.insert(
            vBuffer.end(), m_serializedSequences[sequenceId][i].begin(), m_serializedSequences[sequenceId][i].end());
    }

    if (packetSize > Packets::Scripts::PerformTaskSequence::MAX_BUFFER_SIZE)
        return;

    static Packets::Scripts::PerformTaskSequence packet{};
    packet.size = packetSize;
    memcpy(packet.buffer, vBuffer.data(), packetSize);
    GetPacketFactory().Send(packet);
}

void AddNewTask(eScriptCommands opcode)
{
    int sequenceId = CTaskSequences::ms_iActiveSequence;
    int idx = -1;
    COpCodeSync::IsOpcodeSyncable(opcode, &idx, true);

    assert(idx != -1 && "an opcode exists in 'm_serializedSequences' but not found in 'syncedOpcodes'");

    int dataSize = 0;
    m_serializedSequences[sequenceId][m_sequenceTaskCount] = COpCodeSync::SerializeOpcode(idx, dataSize);

    m_sequenceTaskCount++;
}

bool CTaskSequenceSync::OnOpCodeExecuted(eScriptCommands opcode)
{
    if (opcode == COMMAND_OPEN_SEQUENCE_TASK)
    {
        // CChat::AddMessage("OpenSequence");
        OpenSequence();
        return false;
    }

    if (opcode == COMMAND_CLOSE_SEQUENCE_TASK)
    {
        // CChat::AddMessage("CloseSequence");
        CloseSequence();
        return false;
    }

    if (opcode == COMMAND_CLEAR_SEQUENCE_TASK)
    {
        // CChat::AddMessage("ClearSequence");
        ClearSequence();
        return false;
    }

    if (opcode == COMMAND_PERFORM_SEQUENCE_TASK)
    {
        // CChat::AddMessage("PerformSequence");
        PerformSequence();
        return false;
    }

    if (CTaskSequences::ms_iActiveSequence != -1 && m_bSequenceOpened && IsOpCodeTaskSynced(opcode) &&
        COpCodeSync::scriptParamsBuffer[0].value == -1)
    {
        // CChat::AddMessage("AddNewTask");
        AddNewTask(opcode);
        return false;
    }

    return true;
}

void CTaskSequenceSync::HandlePacket(void* pData, int size)
{
    if (size < Packets::Scripts::PerformTaskSequence::HEADER_SIZE)
    {
        return;
    }

    const uint8_t* pHeader = (const uint8_t*)pData;
    playerid_t playerid = ids::ReadPlayerId(pHeader);
    pedid_t pedid = ids::ReadEntityId<pedid_t>(pHeader + ids::PLAYERID_SIZE);
    uint8_t count = pHeader[ids::PLAYERID_SIZE + ids::ENTITYID_SIZE];

    if (count == 0)
        return;

    int sequenceId = -1;
    Command<Commands::OPEN_SEQUENCE_TASK>(&sequenceId);

    if (sequenceId == -1)
    {
        // CChat::AddMessage("CTaskSequenceSync::HandlePacket: can't open a new sequence");
        return;
    }

    CPed* ped = nullptr;

    if (!pedid.valid())
    {
        if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(playerid))
        {
            ped = networkPlayer->m_pPed;
        }
    }
    else
    {
        if (auto networkPed = CNetworkPedManager::GetPed(pedid))
        {
            ped = networkPed->m_pPed;
        }
    }

    if (ped == nullptr)
    {
        if (playerid != CNetworkPlayerManager::m_localPlayerId)
        {
            return;
        }
        ped = FindPlayerPed(0);
    }

    CTaskSequenceSync::ms_bFailedToProcessSequence = false;

    int ptr = Packets::Scripts::PerformTaskSequence::HEADER_SIZE;
    for (int i = 0; i < count; i++)
    {
        uint8_t len = *(uint8_t*)((int)pData + ptr);
        ptr++;

        COpCodeSync::ms_bProcessingTaskSequence = true;
        COpCodeSync::HandlePacket((uint8_t*)((int)pData + ptr), len);

        if (CTaskSequenceSync::ms_bFailedToProcessSequence == true)
        {
            if (pData && size > 0)
            {
                ms_vTaskSequences.emplace_back((uint8_t*)pData, (uint8_t*)pData + size);
            }
            Command<Commands::CLOSE_SEQUENCE_TASK>(sequenceId);
            Command<Commands::CLEAR_SEQUENCE_TASK>(sequenceId);
            return;
        }

        COpCodeSync::ms_bProcessingTaskSequence = false;

        ptr += len;
    }

    Command<Commands::CLOSE_SEQUENCE_TASK>(sequenceId);
    Command<Commands::PERFORM_SEQUENCE_TASK>(CPools::GetPedRef(ped), sequenceId);
    Command<Commands::CLEAR_SEQUENCE_TASK>(sequenceId);
}