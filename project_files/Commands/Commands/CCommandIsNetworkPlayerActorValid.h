#pragma once
#include "../CCustomCommand.h"
class CCommandIsNetworkPlayerActorValid : public CCustomCommand
{
	void Process(CRunningScript* script) override;
};

