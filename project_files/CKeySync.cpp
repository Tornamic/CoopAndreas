#include "stdafx.h"
#include "CKeySync.h"

CControllerState storedOldState{};
CControllerState storedNewState{};

void CKeySync::ApplyNetworkPlayerContext(CNetworkPlayer* player)
{
	// store local key state
	CPad* pad = CPad::GetPad(0);
	storedOldState = pad->OldState;
	storedNewState = pad->NewState;

	pad->OldState = player->m_oldControllerState;
	pad->NewState = player->m_newControllerState;
}

void CKeySync::ApplyLocalContext()
{
	// restore local key state
	CPad* pad = CPad::GetPad(0);
	pad->OldState = storedOldState;
	pad->NewState = storedNewState;
}

#define PROCESS_STICK(stick);                                   \
    if (player->m_newControllerState.stick &&                   \
        cState.stick)                                           \
    {                                                           \
        player->m_newControllerState.stick =                    \
            cState.stick;                                       \
        player->m_oldControllerState.stick =                    \
            cState.stick;                                       \
    }                                                           \
    else if (!player->m_newControllerState.stick &&             \
                cState.stick)                                   \
    {                                                           \
        player->m_newControllerState.stick =                    \
            cState.stick;                                       \
        player->m_oldControllerState.stick = 0;                 \
    }                                                           \
    else if (!cState.stick)                                     \
    {                                                           \
        player->m_newControllerState.stick = 0;                 \
        player->m_oldControllerState.stick = 0;                 \
    } 

#define PROCESS_KEY(key);                                       \
    if (cState.key)                                             \
    {                                                           \
        if (player->m_newControllerState.key)                   \
        {                                                       \
            player->m_newControllerState.key = 255;             \
            player->m_oldControllerState.key = 255;             \
        }                                                       \
        else                                                    \
        {                                                       \
            player->m_newControllerState.key = 255;             \
            player->m_oldControllerState.key = 0;               \
        }                                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        player->m_newControllerState.key = 0;                   \
        player->m_oldControllerState.key = 0;                   \
    }
    
void CKeySync::ProcessPlayer(CNetworkPlayer* player)
{
    ÑCompressedControllerState cState = player->m_compressedControllerState;
    PROCESS_STICK(LeftStickX);
    PROCESS_STICK(LeftStickY);

    PROCESS_KEY(LeftShoulder1);
    PROCESS_KEY(LeftShoulder2);
    PROCESS_KEY(RightShoulder1);
    PROCESS_KEY(RightShoulder2);
    PROCESS_KEY(DPadUp);
    PROCESS_KEY(DPadDown);
    PROCESS_KEY(DPadLeft);
    PROCESS_KEY(DPadRight);
    PROCESS_KEY(Start);
    PROCESS_KEY(Select);
    PROCESS_KEY(ButtonSquare);
    PROCESS_KEY(ButtonTriangle);
    PROCESS_KEY(ButtonCross);
    PROCESS_KEY(ButtonCircle);
    PROCESS_KEY(ShockButtonL);
    PROCESS_KEY(ShockButtonR);
    PROCESS_KEY(m_bChatIndicated);
    PROCESS_KEY(m_bPedWalk);
    PROCESS_KEY(m_bVehicleMouseLook);
    PROCESS_KEY(m_bRadioTrackSkip);
}

#undef PROCESS_KEY
#undef PROCESS_STICK