#pragma once
#include "../CCustomCommand.h"
class CCommandSetBlipVisibleTo : public CCustomCommand
{
	// Inherited via CCustomCommand
	void Process(CRunningScript* script) override;
};
