#pragma once
class CUtil
{
public:
	static bool CompareControllerStates(const CControllerState& state1, const CControllerState& state2);
	static void CopyExceptShockButtonL(CControllerState& destination, const CControllerState& source);
	static bool IsDucked(CPed* ped);
	static bool isDifferenceGreaterThanPercent(float value1, float value2, int percent);
	static bool IsPositionUpdateNeeded(CVector pos, CVector update, int percent = 5);
};

