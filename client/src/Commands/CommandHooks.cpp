#include "stdafx.h"
#include "CommandHooks.h"
#include "CCustomCommandMgr.h"
#include "Commands/CCommandAddChatMessage.h"

uint16_t nCommand = 0x0;
CRunningScript* pScript = nullptr;

bool ProcessCustomCommand()
{
	// if is in custom command range
	uint16_t commandNormalised = (nCommand & 0x7FFF);
	if (commandNormalised >= CCustomCommandMgr::MIN_CUSTOM_COMMAND && commandNormalised <= CCustomCommandMgr::MAX_CUSTOM_COMMAND)
	{
		pScript->m_pCurrentIP += 2;
		pScript->m_bNotFlag = (nCommand & 0x8000) != 0;
		CCustomCommandMgr::ProcessCommand(commandNormalised, pScript); // process it
		return true;
	}
	return false;
}


void __declspec(naked) CRunningScript__Process_Hook() 
{
	__asm
	{
		mov ax, [ecx]

		mov nCommand, ax
		mov pScript, esi

		push eax
		push ecx
		push esi

		call ProcessCustomCommand

		test al, al
		jz process_orig_opcode 

		pop esi
		pop ecx
		pop eax
		push 0x469FB0
		ret

	process_orig_opcode:
		pop esi
		pop ecx
		pop eax
		push 0x469FBF
		ret
	}
}

//uintptr_t CRunningScript__ProcessOneCommand_Ret = 0x469EBF;
//uintptr_t CRunningScript__ProcessOneCommand_Exit = 0x469EF8;
//void __declspec(naked) CRunningScript__ProcessOneCommand_Hook()
//{
//	__asm
//	{
//		xor eax, eax
//		mov ax, [edx]
//
//		mov nCommand, ax
//		mov pScript, ecx
//
//		pushad
//		pushfd
//	}
//
//	if (ProcessCustomCommand(nCommand, pScript))
//	{
//		__asm
//		{
//			popfd
//			popad
//
//			jmp CRunningScript__ProcessOneCommand_Exit
//		}
//	}
//
//	__asm
//	{
//		popfd
//		popad
//
//		jmp CRunningScript__ProcessOneCommand_Ret
//	}
//}

void CommandHooks::InjectHooks()
{
	patch::RedirectJump(0x469FBA, CRunningScript__Process_Hook);
	///patch::RedirectJump(0x469EBA, CRunningScript__ProcessOneCommand_Hook);
}