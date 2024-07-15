#pragma once
class CUtil
{
public:
	static bool CUtil::CompareControllerStates(const CControllerState& state1, const CControllerState& state2);
	static void CUtil::CopyExceptShockButtonL(CControllerState& destination, const CControllerState& source);
	static bool CUtil::IsDucked(CPed* ped);
	static bool CUtil::isDifferenceGreaterThanPercent(float value1, float value2, int percent);
};

