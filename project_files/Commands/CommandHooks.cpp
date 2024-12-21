#include "stdafx.h"
#include "CommandHooks.h"
#include "CCustomCommandMgr.h"
#include "Commands/CCommandAddChatMessage.h"

bool ProcessCustomCommand(uint16_t command, CRunningScript* script)
{
	// if is in custom command range
	if ((command & 0x7FFF) >= CCustomCommandMgr::MIN_CUSTOM_COMMAND)
	{
		script->m_pCurrentIP += 2;
		script->m_bNotFlag = (command & 0x8000) != 0;
		CCustomCommandMgr::ProcessCommand(command & 0x7FFF, script); // process it
		return true;
	}
	return false;
}

uint16_t nCommand = 0x0;
CRunningScript* pScript = nullptr;

uintptr_t CRunningScript__Process_Ret = 0x469FBF;
uintptr_t CRunningScript__Process_Exit = 0x469FF7;
void __declspec(naked) CRunningScript__Process_Hook() 
{
	__asm
	{
		xor eax, eax
		mov ax, [ecx]

		mov nCommand, ax
		mov pScript, esi

		pushad
		pushfd
	}

	if (ProcessCustomCommand(nCommand, pScript))
	{
		__asm
		{
			popfd
			popad

			jmp CRunningScript__Process_Exit
		}
	}
	
	__asm
	{
		popfd
		popad

		jmp CRunningScript__Process_Ret
	}
}

uintptr_t CRunningScript__ProcessOneCommand_Ret = 0x469EBF;
uintptr_t CRunningScript__ProcessOneCommand_Exit = 0x469EF8;
void __declspec(naked) CRunningScript__ProcessOneCommand_Hook()
{
	__asm
	{
		xor eax, eax
		mov ax, [edx]

		mov nCommand, ax
		mov pScript, ecx

		pushad
		pushfd
	}

	if (ProcessCustomCommand(nCommand, pScript))
	{
		__asm
		{
			popfd
			popad

			jmp CRunningScript__ProcessOneCommand_Exit
		}
	}

	__asm
	{
		popfd
		popad

		jmp CRunningScript__ProcessOneCommand_Ret
	}
}

void CommandHooks::InjectHooks()
{
	patch::RedirectJump(0x469FBA, CRunningScript__Process_Hook);
	patch::RedirectJump(0x469EBA, CRunningScript__ProcessOneCommand_Hook);
}