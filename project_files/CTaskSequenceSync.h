#pragma once
class CTaskSequenceSync
{
public:
	static inline bool ms_bFailedToProcessSequence = false;
	static inline std::vector<std::vector<uint8_t>> ms_vTaskSequences;
	static bool OnOpCodeExecuted(eScriptCommands opcode);
	static void HandlePacket(void* data, int size);
	static bool IsNeededToCollectParametes(eScriptCommands opcode);
	static bool IsOpCodeTaskSynced(eScriptCommands opcode);
};

