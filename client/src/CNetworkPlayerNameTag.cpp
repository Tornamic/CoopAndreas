#include "stdafx.h"

#define PROPORION_X(value) (value * RsGlobal.maximumWidth / 1920)
#define PROPORION_Y(value) (value * RsGlobal.maximumHeight / 1080)

uint8_t GetHudAlpha(float distance)
{
	if (distance < 45.0f)
	{
		return 255;
	}
	else if (distance > CNetworkPlayerNameTag::MAX_DRAW_NICKNAME_DISTANCE)
	{
		return 0;
	}
	else
	{
		return (uint8_t)((1.0f - (distance - 45.0f) / 5.0f) * 255.0f);
	}
}

void DrawNickName(float x, float y, float scale, unsigned char alpha, const char* name)
{
	CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
	CFont::SetFontStyle(1);
	CFont::SetColor(CRGBA(255, 255, 0, alpha));
	CFont::SetBackground(false, false);
	CFont::SetDropColor(CRGBA(0, 0, 0, alpha));
	CFont::SetDropShadowPosition(1);
	CFont::SetScale(PROPORION_X(0.6f * scale), PROPORION_Y(1.22f * scale));
	CFont::PrintString(x, y, name);
}

void DrawWeaponIcon(CPed* ped, float x, float y, float scale, unsigned char alpha)
{
	const float width = CUtil::SCREEN_STRETCH_X(47.0f / 2.0f) * scale;
	const float height = CUtil::SCREEN_STRETCH_Y(58.0f / 2.0f) * scale;
	const float halfWidth = width / 2.0f;
	const float halfHeight = height / 2.0f;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

	auto modelId = CUtil::GetWeaponModelById(ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType);

	if (modelId <= 0) {
		CHud::Sprites[0].Draw({ x, y, width + x, height + y }, CRGBA(255, 255, 255, alpha));
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
		x + halfWidth, y + halfHeight, 1.0f,
		halfWidth, halfHeight,
		255u, 255u, 255u, alpha,
		1.0f,
		alpha,
		0, 0
	);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
}

void DrawBarChartScale(float x, float y, uint16_t width, uint8_t height, float scale, float progress, CRGBA color)
{
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
	RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEFLAT));

	progress = std::clamp(progress, 0.0f, 100.0f);

	const float endX = x + (float)width;
	const float unclampedCurrX = x + (float)width * progress / 100.0f;
	const float currX = min(unclampedCurrX, endX);
	const auto fheight = (float)height;

	// Progress rect
	CSprite2d::DrawRect({ x, y, currX, y + fheight }, color);
	// Background (from currX to endX)
	CSprite2d::DrawRect({ currX, y, endX, y + fheight }, { uint8_t(color.r / 2.0f), uint8_t(color.g / 2.0f), uint8_t(color.b / 2.0f), color.a });

	const float w = CUtil::SCREEN_STRETCH_X(2.0f) * scale, h = CUtil::SCREEN_SCALE_Y(2.0f) * scale;
	const CRect rects[] = {
		//left,     top,              right,    bottom
		{ x,        y,                endX,     y + h       },       // Top
		{ x,        y + fheight - h,  endX,     y + fheight },       // Bottom
		{ x,        y,                x + w,    y + fheight },       // Left
		{ endX - w, y,                endX,     y + fheight }        // Right
	};

	const auto black = CRGBA{ 0, 0, 0, color.a };
	for (const CRect& rect : rects) {
		CSprite2d::DrawRect(rect, black);
	}
}


void CNetworkPlayerNameTag::Process()
{
	if (CCutsceneMgr::ms_running
		|| TheCamera.m_bWideScreenOn)
	{
		return;
	}

	for (auto player : CNetworkPlayerManager::m_pPlayers)
	{
		if (!player->m_pPed)
			continue;

		CVector localPlayerCamPos = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource;
		CVector networkPlayerPos{};

		if (player->m_pPed->m_pRwClump)
			player->m_pPed->GetBonePosition(*(RwV3d*)&networkPlayerPos, 5, false);
		else
		{
			networkPlayerPos = player->m_pPed->GetPosition();
			networkPlayerPos.z += 0.5f;
		}
		networkPlayerPos.z += 0.3f;

		float distance = (localPlayerCamPos - networkPlayerPos).Magnitude();
		uint8_t alpha = GetHudAlpha(distance);

		if (alpha == 0 || !player->m_pPed->IsVisible() || player->m_lOnFoot == nullptr)
			continue;

		if (!CWorld::GetIsLineOfSightClear(localPlayerCamPos, networkPlayerPos, true, false, false, true, false, false, false))
			continue;

		RwV3d out;
		float width, height;
		float normalizedDistance = distance / MAX_DRAW_NICKNAME_DISTANCE;
		CSprite::CalcScreenCoors(*(RwV3d*)&networkPlayerPos, &out, &width, &height, false, false);

		float scale = std::clamp(1.2f - normalizedDistance, 0.7f, 1.0f);
		
		// draw health bar
		if (player->m_lOnFoot->health >= 10.0f || GetTickCount() % 500 > 150) // blinking, fps fixed
		{
			DrawBarChartScale(
				out.x,
				out.y,
				(uint16_t)(PROPORION_X(100.0f * scale)),
				(uint8_t)(PROPORION_Y(14.0f * scale)),
				scale,
				player->m_lOnFoot->health,
				CRGBA(180, 25, 29, alpha)
			);
		}

		// draw armour bar
		if (player->m_lOnFoot->armour > 0.0f)
		{
			DrawBarChartScale(
				out.x,
				out.y - PROPORION_X(12) * scale,
				(uint16_t)(PROPORION_X(100) * scale),
				(uint8_t)(PROPORION_Y(14) * scale),
				scale,
				player->m_lOnFoot->armour,
				CRGBA(225, 225, 225, alpha)
			);
		}
		
		float nicknameOffsetY = (player->m_lOnFoot->armour > 0.0f ? 12.0f * scale + 12.0f * scale : 12.0f * scale);
		DrawNickName(
			out.x + PROPORION_X(4.8f),
			out.y - (PROPORION_Y(nicknameOffsetY) + PROPORION_Y(8.0f)),
			scale,
			alpha,
			player->GetName()
		);
		DrawWeaponIcon(
			player->m_pPed,
			(int)out.x - PROPORION_X(70.0f) * scale,
			(int)out.y - PROPORION_Y(44.0f) * scale,
			scale,
			alpha
		);
	}
}
