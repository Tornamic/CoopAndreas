#pragma once
#include "../CCustomCommand.h"
class CCommandRemoveCarBlipForNetworkPlayer : public CCustomCommand
{
	// Inherited via CCustomCommand
	void Process(CRunningScript* script) override;
};

