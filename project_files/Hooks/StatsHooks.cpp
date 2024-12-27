#include "stdafx.h"
#include "StatsHooks.h"
#include "../CStatsSync.h"

void __declspec(naked) CStats__DisplayScriptStatUpdateMessage_Hook()
{
	__asm
	{
		pushad
	}

	if (CNetwork::m_bConnected)
	{
		CStatsSync::NotifyChanged();
	}

	__asm
	{
		popad

		// code cave
		mov     cl, [eax + 117h]

		push	0x55B98F
		ret
	}
}

void CStats__SetStatValue_Hook(eStats statID, float value)
{
	if (CNetwork::m_bConnected)
	{
		switch (statID)
		{
		case STAT_FAT:
		case STAT_STAMINA:
		case STAT_MUSCLE:
			CStatsSync::NotifyChanged();
		}
	}
	CStats::SetStatValue(statID, value);
}

CPed* ped;
eWeaponType weaponType;
CNetworkPlayer* networkPlayer;
char weaponSkill;
void __declspec(naked) CPed__GetWeaponSkill_Hook()
{
	__asm
	{
		pushad

		mov ped, ecx
		
		mov eax, [esp+4]
		mov weaponType, eax
	}

	// if is not a network player
	if (ped->m_nPedType > 3 || ped == FindPlayerPed(0))
	{
		goto exit;
	}

	networkPlayer = CNetworkPlayerManager::GetPlayer(ped);
	if (networkPlayer == nullptr)
	{
		goto exit;
	}

	weaponSkill = networkPlayer->GetWeaponSkill(weaponType);

	__asm
	{
		popad
		mov al, weaponSkill
		retn 4
	}

exit:
	__asm
	{
		popad

		push    esi
		mov     esi, [esp + 8]
		
		push 0x5E3B65
		ret
	}
}

void __fastcall CPed__Dress_Hook(CPed* This, int)
{
	This->Dress();

	if (CNetwork::m_bConnected)
	{
		if (This == FindPlayerPed(0))
		{
			CChat::AddMessage("CPacketHandler::RebuildPlayer__Trigger()");
			CPacketHandler::RebuildPlayer__Trigger();
		}
	}
}

void StatsHooks::InjectHooks()
{
	patch::RedirectJump(0x55B989, CStats__DisplayScriptStatUpdateMessage_Hook);

	const std::vector<int> CStats__SetStatValue_Refs = {
		0x00439117, 0x00439157, 0x00439194, 0x0043919D, 0x00439937, 0x00439947, 0x00439953, 0x0043995F, 0x0043996B, 0x00439977,
		0x00439983, 0x0043998F, 0x0043999B, 0x004399AA, 0x004399B6, 0x004399C2, 0x004399DA, 0x004399E9, 0x004399F8, 0x00439A07,
		0x0044230E, 0x00443E80, 0x00443EC2, 0x0044AE79, 0x00478EBC, 0x00480D3D, 0x00480DDA, 0x00489D73, 0x0048EE20, 0x004921A4,
		0x004921E6, 0x00562D16, 0x005A83F7, 0x005A8417, 0x005D3B60, 0x006E2922
	};

	patch::RedirectCall(CStats__SetStatValue_Refs, CStats__SetStatValue_Hook);

	patch::RedirectJump(0x5E3B60, CPed__GetWeaponSkill_Hook);

	patch::RedirectCall(0x5A8357, CPed__Dress_Hook);
}
