#include "stdafx.h"

bool CUtil::CompareControllerStates(const CControllerState & state1, const CControllerState & state2) {
    return state1.LeftStickX == state2.LeftStickX &&
        state1.LeftStickY == state2.LeftStickY &&
        //state1.RightStickX == state2.RightStickX &&
        //state1.RightStickY == state2.RightStickY &&
        state1.LeftShoulder1 == state2.LeftShoulder1 &&
        state1.LeftShoulder2 == state2.LeftShoulder2 &&
        state1.RightShoulder1 == state2.RightShoulder1 &&
        state1.RightShoulder2 == state2.RightShoulder2 &&
        state1.DPadUp == state2.DPadUp &&
        state1.DPadDown == state2.DPadDown &&
        state1.DPadLeft == state2.DPadLeft &&
        state1.DPadRight == state2.DPadRight &&
        state1.Start == state2.Start &&
        state1.Select == state2.Select &&
        state1.ButtonSquare == state2.ButtonSquare &&
        state1.ButtonTriangle == state2.ButtonTriangle &&
        state1.ButtonCross == state2.ButtonCross &&
        state1.ButtonCircle == state2.ButtonCircle &&
        state1.ShockButtonL == state2.ShockButtonL &&
        state1.ShockButtonR == state2.ShockButtonR &&
        state1.m_bChatIndicated == state2.m_bChatIndicated &&
        state1.m_bPedWalk == state2.m_bPedWalk &&
        state1.m_bVehicleMouseLook == state2.m_bVehicleMouseLook &&
        state1.m_bRadioTrackSkip == state2.m_bRadioTrackSkip;
}

void CUtil::CopyControllerState(CControllerState& destination, const CControllerState& source) {
    destination.LeftStickX = source.LeftStickX;
    destination.LeftStickY = source.LeftStickY;
    destination.RightStickX = source.RightStickX;
    destination.RightStickY = source.RightStickY;

    destination.LeftShoulder1 = 0;
    destination.LeftShoulder2 = 0;

    destination.RightShoulder1 = source.RightShoulder1;
    destination.RightShoulder2 = source.RightShoulder2;

    destination.DPadUp = source.DPadUp;
    destination.DPadDown = source.DPadDown;
    destination.DPadLeft = source.DPadLeft;
    destination.DPadRight = source.DPadRight;

    destination.Start = source.Start;
    destination.Select = source.Select;

    destination.ButtonSquare = source.ButtonSquare;
    destination.ButtonTriangle = source.ButtonTriangle;
    destination.ButtonCross = source.ButtonCross;
    destination.ButtonCircle = source.ButtonCircle;

    destination.ShockButtonR = source.ShockButtonR;

    destination.m_bChatIndicated = source.m_bChatIndicated;
    destination.m_bPedWalk = source.m_bPedWalk;
    destination.m_bVehicleMouseLook = source.m_bVehicleMouseLook;
    destination.m_bRadioTrackSkip = source.m_bRadioTrackSkip;
}

bool CUtil::IsDucked(CPed* ped)
{
    CTaskSimpleDuck* task = ped->m_pIntelligence->GetTaskDuck(true);

    if (task == nullptr)
        return false;
    if (task->m_bIsFinished || task->m_bIsAborting)
        return false;

    return true;
}

bool CUtil::isDifferenceGreaterThanPercent(float value1, float value2, int percent) {
    float difference = std::abs(value1 - value2);
    float average = (std::abs(value1) + std::abs(value2)) / 2;
    return (difference / average) > (percent / 100);
}

bool CUtil::IsPositionUpdateNeeded(CVector pos, CVector update, int percent)
{
    return
        CUtil::isDifferenceGreaterThanPercent(pos.x, update.x, percent) ||
        CUtil::isDifferenceGreaterThanPercent(pos.y, update.y, percent) ||
        CUtil::isDifferenceGreaterThanPercent(pos.z, update.z, percent);
}

int CUtil::GetWeaponModelById(unsigned char id)
{
    return CWeaponInfo::GetWeaponInfo((eWeaponType)id, 1)->m_nModelId1;
}

bool CUtil::IsMeleeWeapon(unsigned char id)
{
    return CWeaponInfo::GetWeaponInfo((eWeaponType)id, 1)->m_nWeaponFire == 0;
}