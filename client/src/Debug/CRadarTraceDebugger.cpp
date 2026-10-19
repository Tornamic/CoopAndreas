#include "CRadarTraceDebugger.h"
#include <imgui.h>
#include "CRadarSync.h"

static const char* GetBlipTypeName(unsigned int nType)
{
    static const char* aszNames[] = { "NONE", "CAR", "CHAR", "OBJECT", "COORD", "CONTACT", "SPOTLIGHT", "PICKUP", "AIRSTRIP" };
    return nType < ARRAY_SIZE(aszNames) ? aszNames[nType] : "?";
}

static const char* GetBlipDisplayName(unsigned int nDisplay)
{
    static const char* aszNames[] = { "NEITHER", "MARKER", "BLIP", "BOTH" };
    return nDisplay < ARRAY_SIZE(aszNames) ? aszNames[nDisplay] : "?";
}

void CRadarTraceDebugger::DrawUI()
{
    ImGui::SetNextWindowSize(ImVec2(900.0f, 500.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Radar Traces"))
    {
        ImGui::End();
        return;
    }

    static bool bInUseOnly = true;
    ImGui::Checkbox("In use only", &bInUseOnly);

    int nInUse = 0;
    int nLocal = 0;
    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        if (CRadar::ms_RadarTrace[i].m_bInUse)
        {
            nInUse++;
            if (CRadarSync::ms_abLocal[i])
            {
                nLocal++;
            }
        }
    }
    ImGui::SameLine();
    ImGui::Text("| host: %s | in use: %d/%d | local: %d | my id: %s", CLocalPlayer::m_bIsHost ? "yes" : "no", nInUse,
        MAX_RADAR_TRACES, nLocal, to_string(CNetworkPlayerManager::m_localPlayerId).c_str());

    const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable;

    if (ImGui::BeginTable("traces", 14, flags))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Idx");
        ImGui::TableSetupColumn("Handle");
        ImGui::TableSetupColumn("Use");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Display");
        ImGui::TableSetupColumn("Sprite");
        ImGui::TableSetupColumn("Colour");
        ImGui::TableSetupColumn("Friendly");
        ImGui::TableSetupColumn("Entity");
        ImGui::TableSetupColumn("Network entity / posn");
        ImGui::TableSetupColumn("EnEx");
        ImGui::TableSetupColumn("ForWho");
        ImGui::TableSetupColumn("Local");
        ImGui::TableSetupColumn("Visible");
        ImGui::TableHeadersRow();

        for (int i = 0; i < MAX_RADAR_TRACES; i++)
        {
            tRadarTrace& trace = CRadar::ms_RadarTrace[i];
            if (bInUseOnly && !trace.m_bInUse)
            {
                continue;
            }

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%d", i);
            ImGui::TableNextColumn();
            ImGui::Text("0x%08X", i | (trace.m_nCounter << 16));
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(trace.m_bInUse ? "yes" : "no");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(GetBlipTypeName(trace.m_nBlipType));
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(GetBlipDisplayName(trace.m_nBlipDisplay));
            ImGui::TableNextColumn();
            ImGui::Text("%d", trace.m_nRadarSprite);
            ImGui::TableNextColumn();
            ImGui::Text("%u", trace.m_nColour);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(trace.m_bFriendly ? "yes" : "no");
            ImGui::TableNextColumn();
            ImGui::Text("%d", trace.m_nEntityHandle);

            ImGui::TableNextColumn();
            switch (trace.m_nBlipType)
            {
            case BLIP_CAR:
                ImGui::Text("vehicle %s", to_string(CRadarSync::ms_aEntity[i].vehicleid).c_str());
                break;
            case BLIP_CHAR:
                if (CRadarSync::ms_aEntity[i].entityType == NETWORK_ENTITY_TYPE_PLAYER)
                {
                    ImGui::Text("player %s", to_string(CRadarSync::ms_aEntity[i].playerid).c_str());
                }
                else
                {
                    ImGui::Text("ped %s", to_string(CRadarSync::ms_aEntity[i].pedid).c_str());
                }
                break;
            case BLIP_OBJECT:
            case BLIP_PICKUP:
                ImGui::Text("%.1f %.1f %.1f", CRadarSync::ms_avecEntityPosn[i].x, CRadarSync::ms_avecEntityPosn[i].y,
                    CRadarSync::ms_avecEntityPosn[i].z);
                break;
            default:
                ImGui::Text("%.1f %.1f %.1f", trace.m_vecPos.x, trace.m_vecPos.y, trace.m_vecPos.z);
                break;
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(trace.m_pEntryExit != nullptr ? "yes" : "");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(CRadarSync::ms_aForWho[i].valid() ? to_string(CRadarSync::ms_aForWho[i]).c_str() : "all");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(CRadarSync::ms_abLocal[i] ? "yes" : "");
            ImGui::TableNextColumn();
            if (trace.m_bInUse)
            {
                bool bVisible = CRadarSync::IsTraceVisible(i);
                ImGui::TextColored(bVisible ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f), bVisible ? "yes" : "no");
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();
}
