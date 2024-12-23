#pragma once
#include <cstdint>
class CCustomCommand
{
protected:
	CCustomCommand() {}

public:
	virtual void Process(CRunningScript* script) = 0;

	virtual ~CCustomCommand() = default;
};