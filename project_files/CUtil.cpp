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