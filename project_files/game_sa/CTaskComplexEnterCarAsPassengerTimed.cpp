#include "stdafx.h"
#include "CTaskComplexEnterCarAsPassengerTimed.h"


// 0x63B030
CTaskComplexEnterCarAsPassengerTimed::CTaskComplexEnterCarAsPassengerTimed(CVehicle* vehicle, int doorFrameId, int time, bool a5)
{
	plugin::CallMethod<0x63B030, CTaskComplexEnterCarAsPassengerTimed*>(this, vehicle, doorFrameId, time, a5);
}
