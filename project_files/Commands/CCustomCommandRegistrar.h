#pragma once

#include "CCustomCommandMgr.h"
#include "Commands/CCommandAddChatMessage.h"

class CCustomCommandRegistrar
{
public:
	static void Register()
	{
		CCustomCommandMgr::RegisterCommand(0xF00, new CCommandAddChatMessage());
	}
};

