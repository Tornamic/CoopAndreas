#include "stdafx.h"
#include "CCustomMenuManager.h"
#include "CConfigLoader.h"

#define SCR_NICKNAME_PORT_IP (70)

#define ROW_UNSELECTED (-1)
#define ROW_NICKNAME (0)
#define ROW_IP (1)
#define ROW_PORT (2)
#define ROW_BACK (3)
#define ROW_CONNECT (4)

static const int MAX_ROWS = 5;
static char* aRows[MAX_ROWS] = { "Nickname:", "IP:", "Port:", "Back", "Connect" };
static const int MAX_INPUT_ROWS = 3;
static int iLastHovered = ROW_NICKNAME;
static int iActiveInputRow = ROW_UNSELECTED;
static char aRowContent[MAX_INPUT_ROWS][32+1] = { "", "", ""};

static inline void SetFontColor(bool bActive)
{
	RwRGBA c;
	if (bActive)
	{
		CFont::SetColor(*plugin::CallMethodAndReturn<CRGBA*, 0x58FEA0>((void*)0xBAB22C, &c, (char)3));
		CFont::SetDropColor(*plugin::CallMethodAndReturn<CRGBA*, 0x58FEA0>((void*)0xBAB22C, &c, (char)5));
	}
	else
	{
		CFont::SetColor(*plugin::CallMethodAndReturn<CRGBA*, 0x58FEA0>((void*)0xBAB22C, &c, (char)12));
		CFont::SetDropColor(*plugin::CallMethodAndReturn<CRGBA*, 0x58FEA0>((void*)0xBAB22C, &c, (char)5));
	}
}

void DrawCustomScreen_NicknamePortIp(CMenuManager* This)
{

	CFont::SetBackground(0, 0);
	CFont::SetProportional(1);
	CFont::SetWrapx(RsGlobal.maximumWidth - 10);
	CFont::SetRightJustifyWrap(10.0f);
	CFont::SetCentreSize(RsGlobal.maximumWidth);
	CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
	CFont::SetFontStyle(FONT_GOTHIC);
	CFont::SetEdge(1);
	SetFontColor(true);

	CFont::SetScale(This->StretchX(1.3f), This->StretchY(2.1f));

	CFont::PrintString(This->StretchX(28.0f), This->StretchY(40.0f), "Connect to a server");

	float fSizeX = This->StretchX(250.0f);
	float fSizeY = This->StretchY(20.0f);
	float fGap = This->StretchY(5.0f);
	float fPosX = RsGlobal.maximumWidth / 2;
	float fPosY = RsGlobal.maximumHeight / 2 - (fSizeY + fSizeY);

	CRect rect{};
	rect.left = fPosX - fSizeX;
	rect.right = fPosX + fSizeX;

	bool bHovered = false;

	if (This->CheckFrontEndUpInput())
	{
		if (--iLastHovered < 0)
		{
			if (iActiveInputRow == ROW_UNSELECTED)
			{
				iLastHovered = MAX_INPUT_ROWS;
				AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
			}
			else
			{
				iLastHovered = 0;
			}
		}
		if (iActiveInputRow != ROW_UNSELECTED)
		{
			iActiveInputRow = iLastHovered;
		}
		This->m_bShowMouse = false;
	}

	if (This->CheckFrontEndDownInput())
	{
		if (++iLastHovered > MAX_ROWS - 1)
		{
			if (iActiveInputRow == ROW_UNSELECTED)
			{
				iLastHovered = 0;
				AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
			}
			else 
			{
				iLastHovered = MAX_INPUT_ROWS - 1;
			}
		}
		if (iActiveInputRow != ROW_UNSELECTED)
		{
			if (iLastHovered >= MAX_INPUT_ROWS)
			{
				iActiveInputRow = ROW_UNSELECTED;
				iLastHovered = MAX_INPUT_ROWS;
			}
			else
			{
				iActiveInputRow = iLastHovered;
			}
		}

		This->m_bShowMouse = false;
	}

	if (iActiveInputRow == ROW_UNSELECTED)
	{

		if (This->CheckFrontEndRightInput() || This->CheckFrontEndLeftInput())
		{
			if (iLastHovered == ROW_BACK)
			{
				iLastHovered = ROW_CONNECT;
				AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
			}
			else if (iLastHovered == ROW_CONNECT)
			{
				iLastHovered = ROW_BACK;
				AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
			}
			This->m_bShowMouse = false;
		}
		if (This->m_bShowMouse)
		{
			for (size_t i = 0; i < MAX_INPUT_ROWS; i++)
			{
				rect.bottom = fPosY + (i * (fSizeY + fSizeY + fGap)) + fSizeY;
				rect.top = fPosY + (i * (fSizeY + fSizeY + fGap)) - fSizeY;

				if (This->m_nMousePosY >= rect.top && This->m_nMousePosY < rect.bottom && iLastHovered != i)
				{
					AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
					iLastHovered = i;
				}
			}

			CFont::SetFontStyle(FONT_MENU);
			CFont::SetScale(This->StretchX(0.7f), This->StretchY(1.0f));
			CFont::SetEdge(1);
			CRect rectText{};
			CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
			CFont::GetTextRect(&rectText, rect.left + fGap, rect.bottom + fSizeY + fSizeY, "Back");
			rectText.right = RsGlobal.maximumWidth / 2;

			if (This->m_nMousePosX >= rectText.left && This->m_nMousePosX <= rectText.right &&
				This->m_nMousePosY >= rectText.bottom && This->m_nMousePosY <= rectText.top &&
				iLastHovered != ROW_BACK)
			{
				AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
				iLastHovered = ROW_BACK;
			}

			CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
			CFont::GetTextRect(&rectText, rect.right, rect.bottom + fSizeY + fSizeY, "Connect");
			rectText.left = RsGlobal.maximumWidth / 2;

			if (This->m_nMousePosX >= rectText.left && This->m_nMousePosX <= rectText.right &&
				This->m_nMousePosY >= rectText.bottom && This->m_nMousePosY <= rectText.top &&
				iLastHovered != ROW_CONNECT)
			{
				AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0f, 1.0f);
				iLastHovered = ROW_CONNECT;
			}

		}
	}
	else
	{
		This->m_bShowMouse = false;
	}

	for (size_t i = 0; i < MAX_INPUT_ROWS; i++)
	{
		rect.bottom = fPosY + (i * (fSizeY + fSizeY + fGap)) + fSizeY;
		rect.top = fPosY + (i * (fSizeY + fSizeY + fGap)) - fSizeY;

		SetFontColor(i == iLastHovered);

		CFont::SetScale(This->StretchX(1.3f), This->StretchY(2.1f));
		CFont::SetFontStyle(FONT_GOTHIC);
		CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
		CFont::PrintString(rect.left, rect.top - fGap, aRows[i]);

		CFont::SetScale(This->StretchX(1.3f / 2.0f), This->StretchY(2.1f / 2.0f));
		CFont::SetFontStyle(FONT_SUBTITLES);
		CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
		if (aRowContent[i][0] == '\0')
		{
			if (iActiveInputRow != i)
			{
				CFont::PrintString(rect.right, rect.top + fSizeY / 2, "~r~???");
			}
		}
		else
		{	
			CFont::PrintString(rect.right, rect.top + fSizeY / 2, aRowContent[i]);
		}
		if (iActiveInputRow == i && GetTickCount() % 1000 < 500)
		{
			CFont::SetScale(This->StretchX(0.25f), This->StretchY(2.1f / 2.0f));
			CFont::PrintString(rect.right, rect.top + fSizeY / 2, "l");
		}
	}

	CFont::SetFontStyle(FONT_MENU);
	CFont::SetScale(This->StretchX(0.7f), This->StretchY(1.0f));
	CFont::SetEdge(1);

	SetFontColor(iLastHovered == ROW_BACK);
	CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
	CFont::PrintString(rect.left + fGap, rect.bottom + fSizeY + fSizeY, "Back");

	SetFontColor(iLastHovered == ROW_CONNECT);
	CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
	CFont::PrintString(rect.right, rect.bottom + fSizeY + fSizeY, "Connect");

	/*if (iActiveInputRow == -1)
	{*/
		if (plugin::CallAndReturn<bool, 0x57C330>() || plugin::CallAndReturn<bool, 0x57C3C0>()) // CPad::IsEnterJustReleased() || CPad::IsLMBJustReleased()
		{
			if (iLastHovered == ROW_BACK)
			{
				CConfigLoader::Save();
				This->SwitchToNewScreen(This->m_bGameNotLoaded ? 0x22 : 0x29);
			}
			else if (iLastHovered == ROW_CONNECT)
			{
				if (CNetwork::m_nPort != 0 && strlen(CLocalPlayer::m_Name) && CNetwork::IsValidIP(CNetwork::m_IpAddress))
				{
					CConfigLoader::Save();
					This->SwitchToNewScreen(1);
				}
			}
			if (iActiveInputRow == ROW_UNSELECTED)
			{
				if (iLastHovered < MAX_INPUT_ROWS)
				{
					iActiveInputRow = iLastHovered;
					This->m_bShowMouse = false;
				}
			}
			else
			{
				iActiveInputRow = ROW_UNSELECTED;
				This->m_bShowMouse = true;
			}
		}
	//}
}

void __fastcall CMenuManager__DrawStandardMenu_Hook(CMenuManager* This, SKIP_EDX, char a1)
{

	This->DrawStandardMenu(a1);
	
#if DEBUG
	char* names[] = {
		"SCR_STATS",
		"SCR_GAME",
		"SCR_BRIEF",
		"SCR_AUDIO",
		"SCR_DISPLAY",
		"SCR_MAP",
		"SCR_NEWGAME",
		"SCR_MISSIONPACKLIST",
		"SCR_LOADMISSION",
		"SCR_LOADGAME",
		"SCR_DELETEGAME",
		"SCR_SURETOLOAD",
		"SCR_SURETODELETE",
		"SCR_LOAD_WARNING",
		"SCR_DELETE_WARNING",
		"SCR_DELETE_SUCCESS",
		"SCR_SAVEGAMELIST",
		"SCR_SAVEGAMEPROCEED",
		"SCR_SAVE_WARNING_SCR",
		"SCR_SAVE_SUCCESS",
		"SCR_LOAD_INFO",
		"SCR_SAVE_INFO",
		"SCR_SAVE_AND_CHEAT_SCR",
		"SCR_DEFAULT_DISPLAY",
		"SCR_DEFAULT_AUDIO",
		"SCR_DEFAULT_CONTROLS",
		"SCR_AUDIO_MP3",
		"SCR_DISPLAY_ADVANCED",
		"SCR_LANGUAGE",
		"SCR_ERRORDELETING",
		"SCR_ERRORSAVE_PC",
		"SCR_ERRORLOAD_PC",
		"SCR_ERROR_LOAD_CORRUPT",
		"SCR_OPTIONS",
		"SCR_MAINMENU",
		"SCR_QUIT",
		"SCR_GAMEOPT",
		"SCR_CONTROLLER_CONFIG_PC",
		"SCR_REDEFINE_CHOICE",
		"SCR_CONTROLLER_REDEFINE",
		"SCR_CONTROLLER_MOUSESETTINGS",
		"SCR_CONTROLLER_JOYPADSETTINGS",
		"SCR_PAUSEMENU",
		"SCR_MAIN",
		"SCR_QUITTING_TO_OS",
		"SCR_MARKETING",
		"SCR_SHOP",
		"SCR_CREDITS",
		"SCR_RATEAPP",
		"SCR_RETRYMISSION",
		"SCR_LINK",
		"NUM_MENU_PAGES"
	};

	CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
	CFont::SetFontStyle(1);
	CFont::SetColor(CRGBA(255, 255, 0, 255));
	CFont::SetBackground(false, false);
	CFont::SetDropColor(CRGBA(0, 0, 0, 255));
	CFont::SetDropShadowPosition(1);
	CFont::SetScale(This->StretchX(0.7f), This->StretchY(0.7f));
	if (This->m_nCurrentMenuPage == SCR_NICKNAME_PORT_IP)
	{
		sprintf(gString, "%s %d %d", "SCR_NICKNAME_PORT_IP", This->m_nCurrentMenuPage, iActiveInputRow);
		CFont::PrintString(This->StretchX(10.0f), This->StretchY(10.0f), gString);
	}
	else
	{
		sprintf(gString, "%s %d", names[This->m_nCurrentMenuPage], This->m_nCurrentMenuPage);
		CFont::PrintString(This->StretchX(10.0f), This->StretchY(10.0f), gString);
	}
#endif

	if (This->m_nCurrentMenuPage == SCR_NICKNAME_PORT_IP)
	{
		DrawCustomScreen_NicknamePortIp(This);
	}
}

void __fastcall CMenuManager__SwitchToNewScreen_Hook(CMenuManager* This, SKIP_EDX, char page)
{
	CPad::NewMouseControllerState.lmb = false;
	CPad::OldMouseControllerState.lmb = false;
	CPad::NewKeyState.enter = false;
	CPad::OldKeyState.enter = false;
	CPad::NewKeyState.extenter = false;
	CPad::OldKeyState.extenter = false;

#if DEBUG
	printf("page %d\n", page);
#endif
	if (page == 1 && (This->m_nCurrentMenuPage == 34 || This->m_nCurrentMenuPage == 41))
	{
		This->m_nPreviousMenuPage = This->m_nCurrentMenuPage;
		This->field_1AEA = 0;
		This->ResetHelperText();
		This->m_nCurrentMenuEntry = 0;
		This->m_nCurrentMenuPage = SCR_NICKNAME_PORT_IP;
		return;
	}
	if (This->m_nCurrentMenuPage == SCR_NICKNAME_PORT_IP)
	{
		if (page == -2)
		{
			if (iActiveInputRow == ROW_UNSELECTED)
			{
				This->SwitchToNewScreen(This->m_bGameNotLoaded ? 0x22 : 0x29);
			}
			else
			{
				iActiveInputRow = ROW_UNSELECTED;
				This->m_bShowMouse = true;
			}
			return;
		}
		This->SwitchToNewScreen(1);
		return;
	}

	if (This->m_nCurrentMenuPage == 1 && page == -2)
	{
		This->m_nPreviousMenuPage = This->m_nCurrentMenuPage;
		This->field_1AEA = 0;
		This->ResetHelperText();
		This->m_nCurrentMenuEntry = 0;
		This->m_nCurrentMenuPage = SCR_NICKNAME_PORT_IP;
		return;
	}
	This->SwitchToNewScreen(page);
}

void __fastcall CMenuManager__UserInput_Hook(CMenuManager* This)
{
	if (This->m_nCurrentMenuPage != SCR_NICKNAME_PORT_IP)
	{
		This->UserInput();
	}
}

void CCustomMenuManager::Init()
{
	CConfigLoader::Init();

	strncpy_s(aRowContent[ROW_NICKNAME], 32, CLocalPlayer::m_Name, 32);
	strncpy_s(aRowContent[ROW_IP], 15, CNetwork::m_IpAddress, 15);
	_itoa_s(static_cast<int>(CNetwork::m_nPort), aRowContent[ROW_PORT], 10);

	patch::RedirectCall(0x57BA58, CMenuManager__DrawStandardMenu_Hook);
	//patch::SetUChar(0x57D6B8 + 1, SCR_NICKNAME_PORT_IP);
	
	patch::RedirectCall(0x576AE9, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x577244, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x577266, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x5772C2, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x5772E5, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57730F, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x578E26, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x578EA1, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57B73A, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57C4AB, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57C4B3, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57C4BB, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57C4C3, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D2D9, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D306, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D315, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D335, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D379, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D38A, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D6AB, CMenuManager__SwitchToNewScreen_Hook);
	patch::RedirectCall(0x57D6BA, CMenuManager__SwitchToNewScreen_Hook);

	//patch::RedirectCall(0x57B457, CMenuManager__UserInput_Hook);

	patch::SetUChar(0x580130, 0x75);
	patch::SetUChar(0x58013A, 0x75);
	patch::SetUChar(0x580144, 0x75);
	patch::SetUChar(0x58014E, 0x74);
}

static const int MAX_LENGTH_PER_ROW[MAX_INPUT_ROWS] = { 24, 15, 5 }; // nickname, ip, port

void InsertCharToRow(char c)
{
	int length = strnlen_s(aRowContent[iActiveInputRow], 32);
	if (length >= MAX_LENGTH_PER_ROW[iActiveInputRow])
	{
		return;
	}
	switch (iActiveInputRow)
	{
	case ROW_NICKNAME:
	{
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '[' || c == ']')
		{
			aRowContent[iActiveInputRow][length] = c;
			strncpy_s(CLocalPlayer::m_Name, aRowContent[iActiveInputRow], MAX_LENGTH_PER_ROW[iActiveInputRow]);
		}
		break;
	}
	case ROW_IP:
	{
		if (length == 0 && c == '.')
		{
			return;
		}

		if ((c >= '0' && c <= '9') || c == '.')
		{

			if (c == '.')
			{
				int dots = 0;
				for (int i = 0; i < length; i++)
				{
					if (aRowContent[iActiveInputRow][i] == '.')
					{
						if (++dots >= 3)
						{
							return;
						}
					}
				}
			}

			if (c == '.' && aRowContent[iActiveInputRow][length - 1] == '.')
			{
				return;
			}

			if (c != '.')
			{
				int segValue = 0;
				int i = length - 1;

				while (i >= 0 && aRowContent[iActiveInputRow][i] != '.')
				{
					i--;
				}

				for (int j = i + 1; j < length; j++)
				{
					segValue = segValue * 10 + (aRowContent[iActiveInputRow][j] - '0');
				}
				segValue = segValue * 10 + (c - '0');
				if (segValue > 255)
				{
					return;
				}
			}

			aRowContent[iActiveInputRow][length] = c;
			strncpy_s(CNetwork::m_IpAddress, aRowContent[iActiveInputRow], MAX_LENGTH_PER_ROW[iActiveInputRow]);
		}
		break;
	}

	case ROW_PORT:
	{
		if (length == 0 && c == '0')
		{
			return;
		}
		if (c >= '0' && c <= '9')
		{
			aRowContent[iActiveInputRow][length] = c;
			CNetwork::m_nPort = std::stoi(aRowContent[iActiveInputRow]);
		}
		break;
	}
	}
}

void CCustomMenuManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if(FrontEndMenuManager.m_nCurrentMenuPage != SCR_NICKNAME_PORT_IP)
	{ 
		return;
	}

	if (message == WM_CHAR)
	{
		if (iActiveInputRow == ROW_UNSELECTED && iLastHovered < MAX_INPUT_ROWS)
		{
			iActiveInputRow = iLastHovered;
			FrontEndMenuManager.m_bShowMouse = false;
		}

		if (iActiveInputRow == ROW_UNSELECTED)
		{
			return;
		}

		int length = strnlen_s(aRowContent[iActiveInputRow], 32);
		char c = (char)wParam;
		if (c == VK_BACK)
		{
			if (length > 0)
			{
				aRowContent[iActiveInputRow][length - 1] = '\0';
			}
			return;
		}

		if (length >= MAX_LENGTH_PER_ROW[iActiveInputRow])
		{
			return;
		}

		InsertCharToRow(c);
	}
	else if(message == WM_KEYDOWN)
	{
		bool isControlV = (GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'V';
		bool isShiftInsert = (GetKeyState(VK_SHIFT) & 0x8000) && wParam == VK_INSERT;

		if (isControlV || isShiftInsert)
		{
			if (!OpenClipboard(nullptr))
				return;

			HANDLE hData = GetClipboardData(CF_TEXT);

			CloseClipboard();

			if (!hData)
				return;

			char* clipboardText = (char*)GlobalLock(hData);

			GlobalUnlock(hData);

			if (!clipboardText)
				return;

			int length = strnlen_s(clipboardText, MAX_LENGTH_PER_ROW[iActiveInputRow]);
			for (int i = 0; i < length; i++)
			{
				InsertCharToRow(clipboardText[i]);
			}
		}
	}
}
