#include "stdafx.h"
#include "GameHooks.h"
#include "CKeySync.h"

static void __cdecl CMenuManager__DrawFrontEnd_FixChat_Hook(float alpha)
{
    CFont::SetAlphaFade(alpha);

    if (CChat::m_bInputActive)
        CChat::ToggleInput(false);
}

static void __cdecl CClock__RestoreClock_Hook()
{
    CClock::RestoreClock();
    CPacketHandler::GameWeatherTime__Trigger();
}

static void __cdecl CClock__SetGameClock_Hook(unsigned char h, unsigned char m, unsigned char d)
{
    CClock::SetGameClock(h, m, d);
    CPacketHandler::GameWeatherTime__Trigger();
}

DWORD ProcessCheat_Hook_Ret = 0x438589;
static void __declspec(naked) ProcessCheat_Hook()
{
    // finally figured out how to hook functions without masturbating RedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCall
    __asm
    {
        call eax; call orig code
        pop edi
        pop esi
        mov byte ptr ds : [0x969110] , bl

        pushad; store registers
    }

    // sync time and weather after processing cheat code
    CPacketHandler::GameWeatherTime__Trigger();

    __asm
    {
        popad; restore registers

        jmp ProcessCheat_Hook_Ret; jump to function continuation
    }
}

CEntity* pEntity = nullptr;
DWORD dwJmpAddress = 0x0;
DWORD CRenderer__AddEntityToRenderList_Hook_Continue = 0x5534B5;
DWORD CRenderer__AddEntityToRenderList_Hook_Return = 0x553533;
static void __declspec(naked) CRenderer__AddEntityToRenderList_Hook()
{
    __asm
    {
        mov pEntity, ecx
        pushad
    }
    if ((RwObject*)((DWORD)pEntity + 0x18) == nullptr)
    {
        __asm
        {
            popad
            retn
        }
    }
    else
    {
        __asm
        {
            popad
            push esi
            mov esi, [esp + 4 + 4]
            jmp CRenderer__AddEntityToRenderList_Hook_Continue
        }
    }
}

uintptr_t CTheZones__Update_Dest = 0x0;
CControllerState oldControllerState;

static void __cdecl CTheZones__Update_Hook()
{
    plugin::CallDyn(CTheZones__Update_Dest);

    if (!CNetwork::m_bConnected)
    {
        return;
    }

    // process network players keys
    for (auto player : CNetworkPlayerManager::m_pPlayers)
    {
        CKeySync::ProcessPlayer(player);
    }

    CPad* pad = CPad::GetPad(0);
    CControllerState newState = pad->NewState;

    if (CUtil::CompareControllerStates(oldControllerState, newState))
        return;
    
    oldControllerState = newState;

    // send local player keys
    CPackets::PlayerKeySync packet{};

    packet.newState = CCompressedControllerState(newState, pad->DisablePlayerControls);
    
    CNetwork::SendPacket(CPacketsID::PLAYER_KEY_SYNC, &packet, sizeof packet);
}

void GameHooks::InjectHooks()
{
    patch::RedirectCall(0x57C2A3, CMenuManager__DrawFrontEnd_FixChat_Hook);

    patch::RedirectCall(0x47F1C7, CClock__RestoreClock_Hook);
    patch::RedirectCall(0x441534, CClock__SetGameClock_Hook);
    patch::RedirectJump(0x43857F, ProcessCheat_Hook);

    // CRenderer::RenderEverythingBarRoads => CVisibilityPlugins::GetClumpAlpha crash fix
    patch::RedirectJump(0x5534B0, CRenderer__AddEntityToRenderList_Hook);

    // no, here we will not do anything with zones, here we will get and send player keys
    // it is necessary for the menu to be processed correctly
    CTheZones__Update_Dest = injector::GetBranchDestination(0x53BF49).as_int();
    patch::RedirectCall(0x53BF49, CTheZones__Update_Hook);
}
