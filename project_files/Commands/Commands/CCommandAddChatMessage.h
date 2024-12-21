#pragma once

#include "../CCustomCommand.h"

class CCommandAddChatMessage : public CCustomCommand
{
public:
	CCommandAddChatMessage() {}
	void Process(CRunningScript* script) override;
};

