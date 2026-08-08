#include "MissionRunner.h"
#include <imgui.h>

namespace
{
struct SMissionEntry
{
    const char* m_szName;
    int m_nId;
};

constexpr SMissionEntry MISSIONS[] = {
    {"Big Smoke", 11},
    {"Ryder", 12},
    {"Tagging Up Turf", 13},
    {"Cleaning The Hood", 14},
    {"Drive-Thru", 15},
    {"Nines And AK's", 16},
    {"Drive-By", 17},
    {"Sweet's Girl", 18},
    {"Cesar Vialpando", 19},
    {"OG Loc", 27},
    {"Running Dog", 28},
    {"Wrong Side Of The Tracks", 29},
    {"Just Business", 30},
    {"Home Invasion", 24},
    {"Catalyst", 25},
    {"Robbing Uncle Sam", 26},
    {"Life's A Beach", 31},
    {"Madd Dogg's Rhymes", 32},
    {"Management Issues", 33},
    {"House Party", 34},
    {"High Stakes, Low Rider", 36},
    {"Burning Desire", 22},
    {"Gray Imports", 23},
    {"Doberman", 21},
    {"Los Sepulcros", 20},
    {"Reuniting The Families", 37},
    {"The Green Sabre", 38},
    {"Badlands", 39},
    {"Tanker Commander", 43},
    {"Body Harvest", 46},
    {"Local Liquor Store", 41},
    {"Against All Odds", 44},
    {"Small Town Bank", 42},
    {"Wu Zi Mu / Farewell, My Love... (progression-dependent)", 48},
    {"Are You Going To San Fierro?", 47},
    {"Wear Flowers In Your Hair", 49},
    {"555 WE TIP", 51},
    {"Deconstruction", 50},
    {"Air Raid", 72},
    {"Supply Lines...", 73},
    {"New Model Army", 74},
    {"Photo Opportunity", 58},
    {"Jizzy", 59},
    {"T-Bone Mendez", 65},
    {"Mike Toreno", 66},
    {"Outrider", 60},
    {"Snail Trail", 52},
    {"Ice Cold Killa", 61},
    {"Pier 69", 64},
    {"Toreno's Last Flight", 62},
    {"Mountain Cloud Boys", 53},
    {"Ran Fa Li", 54},
    {"Lure", 55},
    {"Amphibious Assault", 56},
    {"The Da Nang Thang", 57},
    {"Yay Ka-Boom-Boom", 63},
    {"Zeroing In", 67},
    {"Test Drive", 68},
    {"Customs Fast Track", 69},
    {"Puncture Wounds", 70},
    {"Monster", 75},
    {"Highjack", 76},
    {"Interdiction", 77},
    {"Verdant Meadows", 78},
    {"N.O.E.", 79},
    {"Stowaway", 80},
    {"Black Project", 81},
    {"Green Goo", 82},
    {"Fender Ketchup", 84},
    {"Explosive Situation", 85},
    {"You've Had Your Chips", 86},
    {"Don Peyote", 88},
    {"Architectural Espionage", 96},
    {"Key To Her Heart", 97},
    {"Dam And Blast", 98},
    {"Cop Wheels", 99},
    {"Up, Up And Away!", 100},
    {"Breaking the Bank at Caligula's", 101},
    {"Intensive Care", 89},
    {"The Meat Business", 90},
    {"Fish In A Barrel", 87},
    {"Misappropriation", 93},
    {"Madd Dogg", 95},
    {"Freefall", 91},
    {"High Noon", 94},
    {"Saint Mark's Bistro", 92},
    {"A Home In The Hills", 102},
    {"Vertical Bird", 103},
    {"Home Coming", 104},
    {"Beat Down On B Dup", 106},
    {"Grove 4 Life", 107},
    {"Cut Throat Business", 105},
    {"Riot", 108},
    {"Los Desperados", 109},
    {"End Of The Line - Part 1", 110},
    {"End Of The Line - Part 2", 111},
    {"End Of The Line - Part 3", 112},
};

const char* GetLaunchUnavailableReason()
{
    if (!CNetwork::m_bAuthenticated)
    {
        return "Connect and authenticate with a server to launch missions.";
    }

    if (!CLocalPlayer::m_bIsHost)
    {
        return "Only the host can launch missions.";
    }

    CPlayerPed* pPlayerPed = FindPlayerPed(0);
    if (!pPlayerPed)
    {
        return "The local player is not available.";
    }

    if (CTheScripts::IsPlayerOnAMission() || CTheScripts::bAlreadyRunningAMissionScript)
    {
        return "A mission is already active.";
    }

    if (CGame::currArea != AREA_MAIN_MAP || pPlayerPed->m_nAreaCode != AREA_MAIN_MAP)
    {
        return "Missions can only be launched outside interiors.";
    }

    return nullptr;
}
}

bool MissionRunner::DrawUI()
{
    ImGui::SetNextWindowSize(ImVec2(300.0f, 500.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Missions");

    const char* szUnavailableReason = GetLaunchUnavailableReason();
    if (szUnavailableReason)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.25f, 1.0f), "%s", szUnavailableReason);
        ImGui::Separator();
    }

    ImGui::BeginChild("##mission_list", ImVec2(0.0f, 0.0f));

    bool bMissionLaunched = false;
    for (const SMissionEntry& mission : MISSIONS)
    {
        bool bValidMissionId = mission.m_nId >= 0 && mission.m_nId < CTheScripts::NumberOfMissionScripts;
        ImGui::BeginDisabled(szUnavailableReason || !bValidMissionId);

        if (ImGui::Button(mission.m_szName, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)) && bValidMissionId)
        {
            Command<Commands::LOAD_AND_LAUNCH_MISSION_INTERNAL>(mission.m_nId);
            bMissionLaunched = true;
        }

        ImGui::EndDisabled();

        if (bMissionLaunched)
        {
            break;
        }
    }

    ImGui::EndChild();
    ImGui::End();
    return bMissionLaunched;
}
