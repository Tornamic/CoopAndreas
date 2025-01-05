#include "stdafx.h"

#define PROPORION_X(value) (value * RsGlobal.maximumWidth / 1920)
#define PROPORION_Y(value) (value * RsGlobal.maximumHeight / 1080)

unsigned char GetHudAlpha(float distance) 
{
	if (distance <= 5.0f) 
	{
		return 255;
	}
	else if (distance >= 15.0f) 
	{
		return 0;
	}
	else 
	{
		float factor = (5.0f - distance) / 10.0f;
		return static_cast<unsigned char>(std::round(factor * 255.0f));
	}
}

void DrawNickName(float x, float y, unsigned char alpha, const char* name)
{
	CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
	CFont::SetFontStyle(3);
	CFont::SetColor(CRGBA(255, 255, 0, alpha));
	CFont::SetBackground(false, false);
	CFont::SetDropColor(CRGBA(0, 0, 0, alpha));
	CFont::SetDropShadowPosition(1);
	CFont::SetScale(PROPORION_X(0.4f), PROPORION_Y(0.7f));
	CFont::PrintString(x, y, name);
}

void DrawWeaponIcon(CPed* ped, int x, int y, unsigned char alpha) 
{
	const auto x0 = (float)x;
	const auto y0 = (float)y;
	const float width = CUtil::SCREEN_STRETCH_X(47.0f / 2.0f);
	const float height = CUtil::SCREEN_STRETCH_Y(58.0f / 2.0f);
	const float halfWidth = width / 2.0f;
	const float halfHeight = height / 2.0f;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

	auto modelId = CUtil::GetWeaponModelById(ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType);

	if (modelId <= 0) {
		CHud::Sprites[0].Draw({ x0, y0, width + x0, height + y0 }, CRGBA(255, 255, 255, alpha));
		return;
	}

	auto mi = CModelInfo::GetModelInfo(modelId);
	auto txd = CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex);
	if (!txd)
		return;

	auto texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(mi->m_nKey, "ICON"));
	if (!texture)
		return;

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(NULL));
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(texture)));
	CSprite::RenderOneXLUSprite(
		x0 + halfWidth, y0 + halfHeight, 1.0f,
		halfWidth, halfHeight,
		255u, 255u, 255u, alpha,
		1.0f,
		alpha,
		0, 0
	);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
}

void CNetworkPlayerNameTag::Process()
{

	for (auto player : CNetworkPlayerManager::m_pPlayers)
	{
		if (!player->m_pPed)
			continue;

		CVector localPlayerPos = FindPlayerCoors(0);
		CVector networkPlayerPos{};

		if (player->m_pPed->m_pRwClump)
			player->m_pPed->GetBonePosition(*(RwV3d*)&networkPlayerPos, 5, false);
		else
		{
			networkPlayerPos = player->m_pPed->GetPosition();
			networkPlayerPos.z += 0.5f;
		}

		unsigned char alpha = GetHudAlpha((localPlayerPos - networkPlayerPos).Magnitude());

		if (alpha == 0 || !player->m_pPed->IsVisible() || player->m_lOnFoot == nullptr)
			continue;

		networkPlayerPos.z += 0.3f;

		RwV3d out;
		float width, height;
		CSprite::CalcScreenCoors(*(RwV3d*)&networkPlayerPos, &out, &width, &height, false, false);	
		
		// draw health bar
		
		if (player->m_lOnFoot->health >= 10.0f || GetTickCount() % 500 > 150) // blinking, fps fixed
		{
			CSprite2d::DrawBarChart(
				(float)out.x,
				(float)out.y,
				PROPORION_X(100),
				PROPORION_Y(14),
				player->m_lOnFoot->health,
				false,
				false,
				true,
				CRGBA(180, 25, 29, alpha),
				CRGBA(0, 0, 0, 0)
			);
		}

		// draw armour bar
		if (player->m_lOnFoot->armour > 0.0f)
		{
			CSprite2d::DrawBarChart(
				(float)out.x,
				(float)out.y - PROPORION_X(12),
				PROPORION_X(100),
				PROPORION_Y(14),
				player->m_lOnFoot->armour,
				false,
				false,
				true,
				CRGBA(225, 225, 225, alpha),
				CRGBA(0, 0, 0, 0)
			);
		}
		
		int nicknameOffsetY = player->m_lOnFoot->armour > 0.0f ? 24 : 12;

		DrawNickName((float)out.x + PROPORION_X(5), (float)out.y - PROPORION_Y(nicknameOffsetY), alpha, player->GetName());

		DrawWeaponIcon(player->m_pPed, (int)out.x - PROPORION_X(70), (int)out.y - PROPORION_Y(50), alpha);
	}
}
