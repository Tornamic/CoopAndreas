#include "CImGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx9.h"
#include "CPacketTimeline.h"
#include <filesystem>
ImFont* pFont;

void InitFonts()
{
    char windowsDir[MAX_PATH];
    GetWindowsDirectoryA(windowsDir, MAX_PATH);
    std::string fontsDir = std::string(windowsDir) + "\\Fonts\\";

    ImGuiIO& io = ImGui::GetIO();
    pFont = nullptr;

    std::string segoePath = fontsDir + "segoeui.ttf";
    if (std::filesystem::exists(segoePath))
    {
        pFont = io.Fonts->AddFontFromFileTTF(segoePath.c_str(), 16.0f);
    }
    
    if (!pFont)
    {
        pFont = io.Fonts->AddFontDefault();
    }
}

void InitStyles()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);

    style.Colors[ImGuiCol_WindowBg] = ImColor(13, 19, 33, 255);
    style.Colors[ImGuiCol_ChildBg] = ImColor(24, 31, 47, 255);
    style.ChildRounding = 3.0f;
}

void CImGui::Init()
{
    Events::initRwEvent += []
    {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(RsGlobal.ps->window);
        ImGui_ImplWin32_EnableDpiAwareness();

        ImGui_ImplDX9_Init(static_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice()));

        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = false;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename = nullptr;

        InitStyles();
        InitFonts();

        ImGui_ImplDX9_InvalidateDeviceObjects();
        ImGui_ImplDX9_CreateDeviceObjects();
    };

    Events::drawAfterFadeEvent += []
    {
        if (CImGui::ms_bActive && !FrontEndMenuManager.m_bMenuActive)
        {
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
            {
                CPad::GetPad(0)->DisablePlayerControls |= 0x200;
            }
            else
            {
                CPad::GetPad(0)->DisablePlayerControls &= ~0x200;
            }

            ImGui::PushFont(pFont);
            ImGui::Begin("Debug", nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetWindowSize(ImVec2(0.0, 0.0f));

            ImGui::TextUnformatted("Enter 'D1212' as a cheat-code to de/activate debug menu.");

            static bool bPacketTimeline = false;
            ImGui::Checkbox("Packet Timeline", &bPacketTimeline);
            if (bPacketTimeline)
            {
                CPacketTimeline::DrawUI();
            }

            ImGui::End();
            ImGui::PopFont();

            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

            ImGui_ImplDX9_InvalidateDeviceObjects();
        }
    };

    Events::gameProcessEvent.after += []
    {
        const char ACTIVATE_DEBUG_CHEAT[] = "2121D";  // type `d1212` as a cheat code
        if (strncmp(ACTIVATE_DEBUG_CHEAT, CCheat::m_CheatString, ARRAY_SIZE(ACTIVATE_DEBUG_CHEAT) - 1) == 0)
        {
            CCheat::m_CheatString[0] = '\0';
            CImGui::ms_bActive = !CImGui::ms_bActive;

            if (CImGui::ms_bActive)
            {
                CPad::GetPad(0)->DisablePlayerControls |= 0x200;
            }
            else
            {
                CPad::GetPad(0)->DisablePlayerControls &= ~0x200;
            }

            static_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice())->ShowCursor(CImGui::ms_bActive);
            ImGui::GetIO().MouseDrawCursor = CImGui::ms_bActive;
        }
    };
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CImGui::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}
