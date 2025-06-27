#pragma once
class CTaskSequenceSync
{
public:
	static bool OnOpCodeExecuted(eScriptCommands opcode);
	static void HandlePacket(void* data, int size);
	static bool IsNeededToCollectParametes(eScriptCommands opcode);
	static bool IsOpCodeTaskSynced(eScriptCommands opcode);
};

