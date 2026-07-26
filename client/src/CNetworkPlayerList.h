#pragma once
class CNetworkPlayerList
{
public:
	static constexpr float BOX_WIDTH = 250.0f;
	static constexpr float BOX_HEIGHT = 300.0f;
	static constexpr float COLUMN_HEIGHT = 35.0f;

	static constexpr float NAME_OFFSET_X = 7.0f; // 16.0f
	static constexpr float NAME_OFFSET_Y = 13.0f;

	static constexpr float BAR_OFFSET_X = 14.0f;
	static constexpr float BAR_OFFSET_Y = 23.0f;
	static constexpr float BAR_WIDTH = 40.0f;
	static constexpr float BAR_HEIGHT = 8.0f;

	static constexpr uint8_t PING_STRIPES = 4;
	static constexpr float PING_OFFSET_X = 4.0f;
	static constexpr float PING_OFFSET_Y = 27.0f;
	static constexpr float PING_SCALE_X = 1.0f;
	static constexpr float PING_SCALE_Y = 2.5f;
	static constexpr float PING_SPACE_X = 2.5f;
	static constexpr float PING_ADD_SCALE_Y = 2.0f;

	static constexpr float PING_COUNT_SCALE_X = 0.5f;
	static constexpr float PING_COUNT_SCALE_Y = 0.9f;
	static constexpr float PING_COUNT_OFFSET_X = 4.0f;
	static constexpr float PING_COUNT_OFFSET_Y = 13.0f;

	static constexpr float SEPARATOR_PADDING_X = 5.0f;
	static constexpr float SEPARATOR_OFFSET_Y = 0.5f;
	static constexpr float SEPARATOR_WIDTH = -10.0f;
	static constexpr float SEPARATOR_HEIGHT = 1.0f;

	static constexpr float BAR_ARMOUR_OFFSET_Y = 10.0f;

	static constexpr float MAX_NAME_OFFSET_Y = 2.0f;
	static constexpr float MIN_NAME_SCALE_X = 0.2f;
	static constexpr float MIN_NAME_SCALE_Y = 0.5f;
	static constexpr float MAX_NAME_SCALE_X = 0.4f;
	static constexpr float MAX_NAME_SCALE_Y = 0.8f;

	static void Draw();
	static void DrawBox(float fX, float fY);
	static void DrawPing(CNetworkPlayer* pNetworkPlayer, float fX, float fY);
	static void DrawName(CNetworkPlayer* pNetworkPlayer, float fX, float fY);
	static void DrawBars(CPlayerPed* pPlayerPed, float fX, float fY);
	static void DrawWeaponIcon(CPlayerPed* pPlayerPed, float fX, float fY);
	static void DrawSeparator(float fCenterBoxX, float fCenterBoxY, float fColumnY);
};

