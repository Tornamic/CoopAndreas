#pragma once

#include "CCustomCommand.h"

class CCustomCommandMgr
{
public:
	static constexpr uint16_t MIN_CUSTOM_COMMAND = 0xF00;
	static constexpr uint16_t MAX_CUSTOM_COMMAND = 0xFFF;
	static constexpr uint16_t MAX_CUSTOM_COMMAND_COUNT = MAX_CUSTOM_COMMAND - MIN_CUSTOM_COMMAND;

	static inline CCustomCommand* m_commands[MAX_CUSTOM_COMMAND_COUNT];

	static void RegisterCommand(uint16_t opcode, CCustomCommand* command)
	{
		assert(opcode >= MIN_CUSTOM_COMMAND);
		assert(opcode <= MAX_CUSTOM_COMMAND);

		size_t idx = opcode - MIN_CUSTOM_COMMAND;

		assert(m_commands[idx] == nullptr);
		m_commands[idx] = command;
	}

	static void ProcessCommand(uint16_t opcode, CRunningScript* script)
	{
		assert(opcode >= MIN_CUSTOM_COMMAND);
		assert(opcode <= MAX_CUSTOM_COMMAND);

		size_t idx = opcode - MIN_CUSTOM_COMMAND;

		assert(m_commands[idx] != nullptr);
		m_commands[idx]->Process(script);
	}
};

