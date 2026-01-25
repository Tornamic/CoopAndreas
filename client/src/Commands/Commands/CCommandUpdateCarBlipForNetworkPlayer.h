#pragma once
#include "../CCustomCommand.h"
class CCommandUpdateCarBlipForNetworkPlayer : public CCustomCommand
{
	// Inherited via CCustomCommand
	void Process(CRunningScript* script) override;
};

