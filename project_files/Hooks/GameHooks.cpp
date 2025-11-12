#include "stdafx.h"
#include "GameHooks.h"
#include "CKeySync.h"
#include <CCutsceneMgr.h>

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

    packet.newState = CCompressedControllerState(newState);
    packet.newState.bCamera = pad->unk1;
    packet.newState.unk2 = pad->unk2;
    packet.newState.bPlayerAwaitsInGarage = pad->bPlayerAwaitsInGarage;
    packet.newState.bPlayerOnInteriorTransition = pad->bPlayerOnInteriorTransition;
    packet.newState.unk3 = pad->unk3;
    packet.newState.bPlayerSafe = pad->bPlayerSafe;
    packet.newState.bPlayerTalksOnPhone = pad->bPlayerTalksOnPhone;
    packet.newState.bPlayerSafeForCutscene = pad->bPlayerSafeForCutscene;
    packet.newState.bPlayerSkipsToDestination = pad->bPlayerSkipsToDestination;
    packet.newState.bDisablePlayerEnterCar = pad->bDisablePlayerEnterCar;
    packet.newState.bDisablePlayerDuck = pad->bDisablePlayerDuck;
    packet.newState.bDisablePlayerFireWeapon = pad->bDisablePlayerFireWeapon;
    packet.newState.bDisablePlayerFireWeaponWithL1 = pad->bDisablePlayerFireWeaponWithL1;
    packet.newState.bDisablePlayerCycleWeapon = pad->bDisablePlayerCycleWeapon;
    packet.newState.bDisablePlayerJump = pad->bDisablePlayerJump;
    packet.newState.bDisablePlayerDisplayVitalStats = pad->bDisablePlayerDisplayVitalStats;

    CNetwork::SendPacket(CPacketsID::PLAYER_KEY_SYNC, &packet, sizeof packet, (ENetPacketFlag)0);
}

void CCutsceneMgr__StartCutscene_Hook()
{
    CCutsceneMgr::StartCutscene();
    if (CLocalPlayer::m_bIsHost)
    {
        CPackets::StartCutscene packet{};
        packet.currArea = CGame::currArea;
        strncpy_s(packet.name, CCutsceneMgr::ms_cutsceneName, 8);
        CNetwork::SendPacket(CPacketsID::START_CUTSCENE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
}

bool CCutsceneMgr__IsCutsceneSkipButtonBeingPressed_Hook()
{
    bool result = plugin::CallAndReturn<bool, 0x4D5D10>();

    if (result)
    {
        CPackets::SkipCutscene packet{};
        CNetwork::SendPacket(CPacketsID::SKIP_CUTSCENE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }

    return result;
}

int __purecall_Hook()
{
    *(char**)0xDEAD  = "This hook is needed for a more detailed crash log when calling an unimplemented virtual function";
    *(char**)0xDEAD2 = "we will get a full backtrace instead of a single msgbox";
    __asm
    {
        mov eax, 0
        push 0x0
        ret
    }

    return 0;
}


#ifdef DEBUG
bool __fastcall CPCKeyboard__GetKeyDown_Hook(int, int, uint16_t key_code, uint8_t use_mode, char* usage)
{
    return GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(key_code);
}

bool __fastcall CPCKeyboard__GetJustKeyDown_Hook(int, int, uint16_t key_code, uint8_t use_mode, char* usage)
{
    return GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(key_code) & 0x1;
}
#endif

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

    //patch::RedirectCall(0x48072B, CCutsceneMgr__StartCutscene_Hook);
    
    //patch::RedirectCall(0x5B1947, CCutsceneMgr__IsCutsceneSkipButtonBeingPressed_Hook);
   // patch::RedirectCall(0x469F0E, CCutsceneMgr__IsCutsceneSkipButtonBeingPressed_Hook);
   // patch::RedirectCall(0x475459, CCutsceneMgr__IsCutsceneSkipButtonBeingPressed_Hook);

    patch::RedirectJump(PURECALL, __purecall_Hook);

#ifdef DEBUG
    /*patch::ReplaceFunction(0x571980, CPCKeyboard__GetKeyDown_Hook);
    patch::ReplaceFunction(0x571970, CPCKeyboard__GetJustKeyDown_Hook);*/
#endif
}
