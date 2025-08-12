#include "stdafx.h"
#include "StatsHooks.h"
#include "../CStatsSync.h"

void CHud__SetHelpMessageStatUpdate_Hook(char bIncrease, short statId, float statUpdate, float statMax)
{
	if (CNetwork::m_bConnected)
	{
		if(CStatsSync::GetSyncIdByInternal((eStats)statId) != -1)
			CStatsSync::NotifyChanged();
	}

	CHud::SetHelpMessageStatUpdate(bIncrease, statId, statUpdate, statMax);
}


int8_t __fastcall CPed__GetWeaponSkill_Hook(CPed* This, int, eWeaponType weaponType)
{
	eStats SkillStatIndex;
	int reqStatLevel2;
	int reqStatLevel1;

	if (weaponType < WEAPON_PISTOL || weaponType > WEAPON_TEC9)
	{
		return 1;
	}
	if (This->m_nPedType <= PED_TYPE_PLAYER2)
	{
		if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(This))
		{
			return networkPlayer->GetWeaponSkill(weaponType);
		}

		SkillStatIndex = plugin::CallAndReturn<eStats, 0x743CD0>(weaponType);//CWeaponInfo::GetSkillStatIndex(weaponType);
		reqStatLevel2 = CWeaponInfo::GetWeaponInfo(weaponType, 2)->m_nReqStatLevel;
		if (reqStatLevel2 <= CStats::GetStatValue(SkillStatIndex))
		{
			return 2;
		}
		reqStatLevel1 = CWeaponInfo::GetWeaponInfo(weaponType, 1)->m_nReqStatLevel;
		return reqStatLevel1 <= CStats::GetStatValue(SkillStatIndex);
	}
	if (weaponType == WEAPON_PISTOL && This->m_nPedType == PED_TYPE_COP)
	{
		return 3;
	}
	else
	{
		return This->m_nWeaponSkill;
	}
}

void CStats__SetStatValue_Hook(eStats statID, float value)
{
	if (CNetwork::m_bConnected)
	{
		if (CStatsSync::GetSyncIdByInternal(statID) != -1)
			CStatsSync::NotifyChanged();
	}
	CStats::SetStatValue(statID, value);
}

void __fastcall CPed__Dress_Hook(CPed* This, int)
{
	This->Dress();

	if (CNetwork::m_bConnected)
	{
		if (This == FindPlayerPed(0))
		{
			CPacketHandler::RebuildPlayer__Trigger();
		}
	}
}

void StatsHooks::InjectHooks()
{
	patch::RedirectCall(0x55BA93, CHud__SetHelpMessageStatUpdate_Hook); // inside CStats::DisplayScriptStatUpdateMessage_Hook
	patch::RedirectJump(0x5E3B60, CPed__GetWeaponSkill_Hook);
	patch::RedirectCall(0x5A8357, CPed__Dress_Hook);

	const std::vector<int> CStats__SetStatValue_Refs = {
		0x00439117, 0x00439157, 0x00439194, 0x0043919D, 0x00439937, 0x00439947, 0x00439953, 0x0043995F, 0x0043996B, 0x00439977,
		0x00439983, 0x0043998F, 0x0043999B, 0x004399AA, 0x004399B6, 0x004399C2, 0x004399DA, 0x004399E9, 0x004399F8, 0x00439A07,
		0x0044230E, 0x00443E80, 0x00443EC2, 0x0044AE79, 0x00478EBC, 0x00480D3D, 0x00480DDA, 0x00489D73, 0x0048EE20, 0x004921A4,
		0x004921E6, 0x00562D16, 0x005A83F7, 0x005A8417, 0x005D3B60, 0x006E2922
	};

	patch::RedirectCall(CStats__SetStatValue_Refs, CStats__SetStatValue_Hook);
}
