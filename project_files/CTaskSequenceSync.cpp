#include "stdafx.h"
#include "CTaskSequenceSync.h"
#include <game_sa/CTaskSequences.h>
#include <COpCodeSync.h>

SSyncedOpCode m_syncedTasks[] = // TODO
{
	{COMMAND_TASK_PLAY_ANIM_NON_INTERRUPTABLE, true, eSyncedParamType::PED},
	{COMMAND_TASK_GO_STRAIGHT_TO_COORD, true, eSyncedParamType::PED},
	{COMMAND_TASK_ACHIEVE_HEADING, true, eSyncedParamType::PED},
	{COMMAND_TASK_JUMP, true, eSyncedParamType::PED},
	{COMMAND_TASK_TIRED, true, eSyncedParamType::PED},
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
	return 
		opcode == COMMAND_PERFORM_SEQUENCE_TASK ||
		opcode == COMMAND_CLEAR_SEQUENCE_TASK ||
		(IsOpCodeTaskSynced(opcode) && m_bSequenceOpened);
}

void OpenSequence()
{
	m_sequenceTaskCount = 0;
	m_bSequenceOpened   = true;

	int sequenceId = CTaskSequences::ms_iActiveSequence;

	for (int i = 0; i < CTaskSequences::NUM_TASKS; ++i) 
	{
		m_serializedSequences[sequenceId][i].clear();
		m_serializedSequences[sequenceId][i].shrink_to_fit(); // i hate C++/STL
	}
}

void CloseSequence()
{
	m_sequenceTaskCount = 0;
	m_bSequenceOpened   = false;
}

void ClearSequence()
{
	int sequenceId = CTheScripts::GetActualScriptThingIndex(COpCodeSync::scriptParamsBuffer[0].value, SCRIPT_THING_SEQUENCE_TASK);

	for (int i = 0; i < CTaskSequences::NUM_TASKS; ++i)
	{
		m_serializedSequences[sequenceId][i].clear();
		m_serializedSequences[sequenceId][i].shrink_to_fit();
	}
}

void PerformSequence()
{
	int pedHandle = COpCodeSync::scriptParamsBuffer[0].value;
	auto ped = CPools::GetPed(pedHandle);
	
	if (!ped)
		return;

	int pedid = -1;

	if (!ped->IsPlayer())
	{
		pedid = CNetworkPedManager::GetPed(ped)->m_nPedId;
	}

	int sequenceId = CTheScripts::GetActualScriptThingIndex(COpCodeSync::scriptParamsBuffer[1].value, SCRIPT_THING_SEQUENCE_TASK);

	/*
	packet structure:
	// header:
	int playerid
	int pedid
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
		packetSize += sizeof(uint8_t) + m_serializedSequences[sequenceId][i].size(); // size + task
	}

	if (taskCount == 0)
		return;

	packetSize += sizeof(int) + sizeof(int) + sizeof(uint8_t); // playerid + pedid + taskCount

	std::vector<uint8_t> packet;
	packet.clear();
	packet.reserve(packetSize);
	
	int playerid = -1; // let the server fill this variable
	packet.insert(packet.end(), (uint8_t*)&playerid, (uint8_t*)&playerid + sizeof(int));

	packet.insert(packet.end(), (uint8_t*)&pedid, (uint8_t*)&pedid + sizeof(int)); // if pedid == -1 then perfom the sequence on the player

	packet.push_back(taskCount);

	for (int i = 0; i < taskCount; i++)
	{
		packet.push_back((uint8_t)m_serializedSequences[sequenceId][i].size());
		packet.insert(packet.end(), m_serializedSequences[sequenceId][i].begin(), m_serializedSequences[sequenceId][i].end());
	}

	CNetwork::SendPacket(CPacketsID::PERFORM_TASK_SEQUENCE, packet.data(), packetSize, ENET_PACKET_FLAG_RELIABLE);

	CChat::AddMessage("Created packet tcount %d, size %d", taskCount, packetSize);
}

void AddNewTask(eScriptCommands opcode)
{
	int sequenceId = CTaskSequences::ms_iActiveSequence;
	int idx = -1;
	COpCodeSync::IsOpcodeSyncable(opcode, &idx, true);

	assert(idx != -1 && "an opcode exist in 'm_serializedSequences' but not found in 'syncedOpcodes'");

	int dataSize = 0;
	m_serializedSequences[sequenceId][m_sequenceTaskCount] = COpCodeSync::SerializeOpcode(idx, dataSize);
	CChat::AddMessage("opcode %d size %d", opcode, dataSize);

	m_sequenceTaskCount++;
}


bool CTaskSequenceSync::OnOpCodeExecuted(eScriptCommands opcode)
{
	if (opcode == COMMAND_OPEN_SEQUENCE_TASK)
	{
		//CChat::AddMessage("OpenSequence");
		OpenSequence();
		return false;
	}

	if (opcode == COMMAND_CLOSE_SEQUENCE_TASK)
	{
		//CChat::AddMessage("CloseSequence");
		CloseSequence();
		return false;
	}

	if (opcode == COMMAND_CLEAR_SEQUENCE_TASK)
	{
		//CChat::AddMessage("ClearSequence");
		ClearSequence();
		return false;
	}

	if (opcode == COMMAND_PERFORM_SEQUENCE_TASK)
	{
		//CChat::AddMessage("PerformSequence");
		PerformSequence();
		return false;
	}

	if (CTaskSequences::ms_iActiveSequence != -1 
		&& m_bSequenceOpened 
		&& IsOpCodeTaskSynced(opcode) 
		&& COpCodeSync::scriptParamsBuffer[0].value == -1)
	{
		//CChat::AddMessage("AddNewTask");
		AddNewTask(opcode);
		return false;
	}

	return true;
}

void CTaskSequenceSync::HandlePacket(void* data, int size)
{
	std::ofstream outFile("packet_dump.bin", std::ios::binary);
	if (outFile.is_open()) {
		outFile.write(reinterpret_cast<char*>(data), size);
		outFile.close();
	}

	CChat::AddMessage("CTaskSequenceSync::HandlePacket");

    if (size < 9)
    {
        return;
    }

	CChat::AddMessage("1. size %d", size);

	int playerid = *(int*)data;
	int pedid = *(int*)((int)data + 4);
	uint8_t count = *(uint8_t*)((int)data + 8);

	if (count == 0) return;

	CChat::AddMessage("2");

	int sequenceId = -1;
	Command<Commands::OPEN_SEQUENCE_TASK>(&sequenceId);

	if (sequenceId == -1)
	{
		CChat::AddMessage("CTaskSequenceSync::HandlePacket: can't open a new sequence");
		return;
	}

	CChat::AddMessage("3. seq %d", sequenceId);

	CPed* ped = nullptr;

	if (pedid == -1)
	{
		ped = CNetworkPlayerManager::GetPlayer(playerid)->m_pPed;
	}
	else
	{
		ped = CNetworkPedManager::GetPed(pedid)->m_pPed;
	}

	int ptr = 9;
	for (int i = 0; i < count; i++)
	{
		uint8_t len = *(uint8_t*)((int)data + ptr);
		ptr++;

		COpCodeSync::ms_bProcessingTaskSequence = true;
		CChat::AddMessage("4. %d", i);
		COpCodeSync::HandlePacket((uint8_t*)((int)data + ptr), len);
		COpCodeSync::ms_bProcessingTaskSequence = false;

		ptr += len;
	}

	CChat::AddMessage("11");
	Command<Commands::CLOSE_SEQUENCE_TASK>(sequenceId);
	Command<Commands::PERFORM_SEQUENCE_TASK>(CPools::GetPedRef(ped), sequenceId);
	Command<Commands::CLEAR_SEQUENCE_TASK>(sequenceId);
	CChat::AddMessage("12");
}