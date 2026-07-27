#include "stdafx.h"
#include "CNetworkPlayerList.h"
#include "CHudColours.h"

void CNetworkPlayerList::DrawBox(float fX, float fY)
{
    CRect rect = CRect(CUtil::SCREEN_STRETCH_X(fX), CUtil::SCREEN_STRETCH_Y(fY),
        CUtil::SCREEN_STRETCH_X(fX + BOX_WIDTH), CUtil::SCREEN_STRETCH_Y(fY + BOX_HEIGHT));

    CSprite2d::DrawRect(rect, CRGBA(0, 0, 0, 190));

    CFont::SetColor(CRGBA(225, 225, 225, 255));

    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetFontStyle(0);

    CFont::SetScaleForCurrentlanguage(CUtil::SCREEN_STRETCH_X(0.6f), CUtil::SCREEN_STRETCH_Y(0.9f));

    CFont::PrintString(rect.left + CUtil::SCREEN_STRETCH_X(8.0f),
        fmin(rect.bottom, rect.top) - CUtil::SCREEN_STRETCH_Y(10.0f), "Players");
}

#if 0
void CNetworkPlayerList::DrawPing(CNetworkPlayer* pNetworkPlayer, float fX, float fY)
{
    static uint16_t nPing = rand() % 256;
    static DWORD lastUpdate = 0;

    if (CTimer::m_snTimeInMilliseconds - lastUpdate >= 1000)
    {
        nPing = rand() % 256;
        lastUpdate = CTimer::m_snTimeInMilliseconds;
    }

    CRGBA pingColor;

    if (nPing < 80)
    {
        pingColor = CRGBA(35, 176, 74, 255);
    }
    else if (nPing < 150)
    {
        pingColor = CRGBA(253, 197, 0, 255);
    }
    else
    {
        pingColor = CRGBA(255, 45, 45, 255);
    }

    CRect rect = CRect(CUtil::SCREEN_STRETCH_X(fX + PING_OFFSET_X), CUtil::SCREEN_STRETCH_Y(fY + PING_OFFSET_Y),
        CUtil::SCREEN_STRETCH_X(fX + PING_OFFSET_X + PING_SCALE_X),
        CUtil::SCREEN_STRETCH_Y(fY + PING_OFFSET_Y + PING_SCALE_Y));

    for (uint8_t i = 0; i < PING_STRIPES; i++)
    {
        CSprite2d::DrawRect(rect, pingColor);

        rect.left += CUtil::SCREEN_STRETCH_X(PING_SPACE_X);
        rect.right += CUtil::SCREEN_STRETCH_X(PING_SPACE_X);
        rect.top -= CUtil::SCREEN_STRETCH_Y(PING_ADD_SCALE_Y);
    }

    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetColor(CRGBA(255, 255, 255, 255));
    CFont::SetFontStyle(1);
    CFont::SetEdge(0);

    CFont::SetScale(PING_COUNT_SCALE_X, PING_COUNT_SCALE_Y);
    CFont::PrintString(CUtil::SCREEN_STRETCH_X(fX + PING_OFFSET_X + PING_COUNT_OFFSET_X),
        rect.top + CUtil::SCREEN_STRETCH_Y(PING_COUNT_OFFSET_Y), std::to_string(nPing).c_str());
}
#endif

void CNetworkPlayerList::DrawName(CNetworkPlayer* pNetworkPlayer, float fX, float fY)
{
    char szPlayerName[Config::MAX_NICKNAME_LENGTH + 1];

    if (pNetworkPlayer == nullptr)
    {
        strcpy_s(szPlayerName, CLocalPlayer::m_Name);
    }
    else
    {
        strcpy_s(szPlayerName, pNetworkPlayer->GetName().c_str());
    }

    float fNormalizedValue =
        static_cast<float>(strlen(szPlayerName) - 1) / static_cast<float>(sizeof(szPlayerName) / sizeof(char) - 1 - 1);

    CFont::SetScale(
        CUtil::SCREEN_STRETCH_X(MAX_NAME_SCALE_X - (MAX_NAME_SCALE_X - MIN_NAME_SCALE_X) * fNormalizedValue),
        CUtil::SCREEN_STRETCH_Y(MAX_NAME_SCALE_Y - (MAX_NAME_SCALE_Y - MIN_NAME_SCALE_Y) * fNormalizedValue));

    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetColor(CRGBA(255, 255, 255, 255));
    CFont::SetFontStyle(1);
    CFont::SetEdge(1);

    CFont::PrintString(CUtil::SCREEN_STRETCH_X(fX + NAME_OFFSET_X),
        CUtil::SCREEN_STRETCH_Y(fY + NAME_OFFSET_Y) +
            CUtil::SCREEN_STRETCH_Y(4.0f + MAX_NAME_OFFSET_Y * fNormalizedValue),
        szPlayerName);
}

void CNetworkPlayerList::DrawBars(CPlayerPed* pPlayerPed, float fX, float fY)
{
    float fBarOffsetX = CUtil::SCREEN_STRETCH_X(fX + BAR_OFFSET_X + BOX_WIDTH / 2.0f);
    float fBarOffsetY = CUtil::SCREEN_STRETCH_Y(fY + BAR_OFFSET_Y);

    uint16_t barWidth = static_cast<uint16_t>(CUtil::SCREEN_STRETCH_X(BAR_WIDTH));
    uint8_t barHeight = static_cast<uint8_t>(CUtil::SCREEN_STRETCH_Y(BAR_HEIGHT));

    if (pPlayerPed->m_fArmour != 0.0f)
    {
        CSprite2d::DrawBarChart(fBarOffsetX + barWidth, fBarOffsetY - BAR_OFFSET_Y - BAR_ARMOUR_OFFSET_Y, barWidth,
            barHeight, pPlayerPed->m_fArmour, 0, 0, 1, HudColour.GetRGBA(HUD_COLOUR_WHITE), CRGBA(0, 0, 0, 0));
    }

    CSprite2d::DrawBarChart(fBarOffsetX + barWidth, fBarOffsetY, barWidth, barHeight, pPlayerPed->m_fHealth, 0, 0, 1,
        HudColour.GetRGBA(HUD_COLOUR_RED), CRGBA(0, 0, 0, 0));
}

void CNetworkPlayerList::DrawWeaponIcon(CPlayerPed* pPlayerPed, float fX, float fY)
{
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    int nModelId = CUtil::GetWeaponModelById(pPlayerPed->GetWeapon().m_eWeaponType);

    float fWidth = CUtil::SCREEN_STRETCH_X(47.0f / 2.0f);
    float fHeight = CUtil::SCREEN_STRETCH_Y(58.0f / 2.0f);
    float fHalfWidth = fWidth / 2.0f;
    float fHalfHeight = fHeight / 2.0f;

    float fOffsetX = CUtil::SCREEN_STRETCH_X(fX + BOX_WIDTH) - fWidth - CUtil::SCREEN_SCALE_X(7.0f);
    float fOffsetY = CUtil::SCREEN_STRETCH_Y(fY + NAME_OFFSET_Y) - fHalfHeight / 2.0f;

    if (nModelId <= 0)
    {
        CHud::Sprites[0].Draw({fOffsetX, fOffsetY, fWidth + fOffsetX, fHeight + fOffsetY}, CRGBA(255, 255, 255, 255));
        return;
    }

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(nModelId);
    TxdDef* txd = CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex);
    if (txd == nullptr)
    {
        return;
    }

    RwTexture* texture =
        RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(mi->m_nKey, "ICON"));
    if (texture == nullptr)
    {
        return;
    }

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(texture)));

    CSprite::RenderOneXLUSprite(fOffsetX + fHalfWidth, fOffsetY + fHalfHeight, 1.0f, fHalfWidth, fHalfHeight, 255, 255,
        255, 255, 1.0f, 255, 0, 0);

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
}

void CNetworkPlayerList::DrawSeparator(float fCenterBoxX, float fCenterBoxY, float fColumnY)
{
    float fSeparatorX = fCenterBoxX + SEPARATOR_PADDING_X;
    float fSeparatorY = fColumnY + fCenterBoxY / 2.0f + SEPARATOR_OFFSET_Y;

    CSprite2d::DrawRect(CRect(CUtil::SCREEN_STRETCH_X(fSeparatorX), CUtil::SCREEN_STRETCH_Y(fSeparatorY),
                            CUtil::SCREEN_STRETCH_X(fSeparatorX + BOX_WIDTH + SEPARATOR_WIDTH),
                            CUtil::SCREEN_STRETCH_Y(fSeparatorY + SEPARATOR_HEIGHT)),
        CRGBA(169, 169, 169, 130));
}

void CNetworkPlayerList::Draw()
{
    if (CPad::NewKeyState.tab == 0)
    {
        return;
    }

    float fBoxX = CUtil::SCREEN_BASE_WIDTH / 2.0f - BOX_WIDTH / 2.0f;
    float fBoxY = CUtil::SCREEN_BASE_HEIGHT / 2.0f - BOX_HEIGHT / 2.0f;

    uint8_t nPlayerCount = CNetworkPlayerManager::m_pPlayers.size();
    std::vector<int> vPlayerId;

    vPlayerId.reserve(nPlayerCount + 1);

    for (uint8_t i = 0; i < nPlayerCount; i++)
    {
        vPlayerId.push_back(CNetworkPlayerManager::m_pPlayers[i]->m_iPlayerId);
    }

    vPlayerId.push_back(CNetworkPlayerManager::m_nMyId);
    std::sort(vPlayerId.begin(), vPlayerId.end());

    DrawBox(fBoxX, fBoxY);

    for (uint8_t i = 0; i < vPlayerId.size() && i < Config::MAX_SERVER_PLAYERS; i++)
    {
        CNetworkPlayer* pNetworkPlayer = nullptr;
        CPlayerPed* pPlayerPed = nullptr;

        if (vPlayerId[i] == CNetworkPlayerManager::m_nMyId)
        {
            pPlayerPed = FindPlayerPed(0);
        }
        else
        {
            for (CNetworkPlayer* pPlayer : CNetworkPlayerManager::m_pPlayers)
            {
                if (pPlayer->m_iPlayerId == vPlayerId[i])
                {
                    pNetworkPlayer = pPlayer;
                    pPlayerPed = pPlayer->m_pPed;
                    break;
                }
            }
        }

        float fColumnY = fBoxY + i * COLUMN_HEIGHT;

#if 0
        DrawPing(pNetworkPlayer, fBoxX, fColumnY);
#endif
        DrawName(pNetworkPlayer, fBoxX, fColumnY);
        DrawBars(pPlayerPed, fBoxX, fColumnY);
        DrawWeaponIcon(pPlayerPed, fBoxX, fColumnY);

        if (i == nPlayerCount)
        {
            continue;
        }

        DrawSeparator(fBoxX, fBoxY, fColumnY);
    }
}
