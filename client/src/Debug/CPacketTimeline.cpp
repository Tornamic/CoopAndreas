#include "CPacketTimeline.h"
#include <imgui.h>
#include <CServerTime.h>
#include <imgui_internal.h>

enum
{
    DIRECTION_ALL,
    DIRECTION_INBOUND,
    DIRECTION_OUTBOUND
};

enum
{
    PACKET_TYPES_ALL,
    PACKET_TYPES_CHANNEL_SYSTEM,
    PACKET_TYPES_CHANNEL_SYNC,
    PACKET_TYPES_CHANNEL_EVENT,
    PACKET_TYPES_CHANNEL_SCRIPT,
    PACKET_TYPES_CUSTOM
};

static int ms_nDirection = DIRECTION_ALL;
static int ms_nPacketTypes = PACKET_TYPES_CUSTOM;
static bool ms_abPacketsEnabled[(int)ePacketType::PACKET_ID_MAX]{};
static int ms_nSecondRange = 10;

static ImVec4 GetPacketColor(ePacketType packetType)
{
    const char* typeString = ePacketType_ToString(packetType);

    return ImVec4(((typeString[0] * 17) % 256) / 256.0f, ((typeString[1] * 23) % 256) / 256.0f,
        ((typeString[strlen(typeString) - 1] * 11) % 256) / 256.0f, 255.0f);
}

void CPacketTimeline::DrawUI()
{
    ImGui::Begin("##packet_timeline", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowSize(ImVec2(800, 500), ImGuiCond_Once);

    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Network Packet Timeline");
    ImGui::SetWindowFontScale(1.0f);

    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();

    SYSTEMTIME st;
    GetLocalTime(&st);
    ImGui::TextDisabled("Time: %02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);

    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::TextDisabled("-");
    ImGui::SameLine();
    ImGui::Spacing();

    uint32_t totalSeconds = CTimer::m_snTimeInMilliseconds / 1000;
    uint32_t hours = totalSeconds / 3600;
    uint32_t minutes = (totalSeconds / 60) % 60;
    uint32_t seconds = totalSeconds % 60;
    ImGui::SameLine();
    ImGui::TextDisabled("Uptime: %02u:%02u:%02u", hours, minutes, seconds);

    char latency[32], jitter[32];
    sprintf_s(latency, "Latency: %d ms", CNetwork::GetRTT());
    sprintf_s(jitter, "Jitter: %d ms", CNetwork::GetRTTVariance());

    ImGui::SameLine(
        ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(latency).x - ImGui::CalcTextSize(jitter).x - 44);

    // latency
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(p.x + 4, p.y + ImGui::GetTextLineHeight() * 0.5f), 4.0f, IM_COL32(0, 255, 0, 255));
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
    ImGui::TextUnformatted(latency);

    // jitter
    ImGui::SameLine();
    p = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(p.x + 4, p.y + ImGui::GetTextLineHeight() * 0.5f), 4.0f, IM_COL32(255, 255, 0, 255));
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
    ImGui::TextUnformatted(jitter);

    ImGui::Spacing();

    constexpr float fFiltersWidth = 150.0f;

    {
        float filtersMaxHeight = ImGui::GetStyle().ItemSpacing.y * 5.0f + ImGui::GetFrameHeight();

        ImGui::BeginChild("##filters", ImVec2(fFiltersWidth, -filtersMaxHeight), ImGuiChildFlags_Borders);

        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("Filters");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Spacing();

        static char searchText[32];
        ImGui::PushItemWidth(-1.0f);
        ImGui::InputTextWithHint("##search", "Search packet type...", searchText, sizeof(searchText),
            ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_CharsUppercase);
        ImGui::PopItemWidth();
        ImGui::Spacing();

        ImGui::Text("Packet Types");
        if (ImGui::RadioButton("All##PacketTypes", &ms_nPacketTypes, PACKET_TYPES_ALL))
        {
            std::fill_n(ms_abPacketsEnabled, ARRAY_SIZE(ms_abPacketsEnabled), true);
        }
        auto FilterByChannel = [](ePacketChannel packetChannel) -> void
        {
            for (size_t i = 0; i < ARRAY_SIZE(ms_abPacketsEnabled); i++)
            {
                Packet* pProtPacket = GetPacketFactory().m_map[static_cast<ePacketType>(i)];
                if (pProtPacket && pProtPacket->GetChannel() == packetChannel)
                {
                    ms_abPacketsEnabled[i] = true;
                }
                else
                {
                    ms_abPacketsEnabled[i] = false;
                }
            }
        };

        ImGui::SameLine();
        if (ImGui::RadioButton("System", &ms_nPacketTypes, PACKET_TYPES_CHANNEL_SYSTEM))
        {
            FilterByChannel(ePacketChannel::SYSTEM);
        }
        if (ImGui::RadioButton("Sync", &ms_nPacketTypes, PACKET_TYPES_CHANNEL_SYNC))
        {
            FilterByChannel(ePacketChannel::SYNC);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Event", &ms_nPacketTypes, PACKET_TYPES_CHANNEL_EVENT))
        {
            FilterByChannel(ePacketChannel::EVENT);
        }
        if (ImGui::RadioButton("Script", &ms_nPacketTypes, PACKET_TYPES_CHANNEL_SCRIPT))
        {
            FilterByChannel(ePacketChannel::SCRIPT);
        }
        ImGui::SameLine();
        ImGui::RadioButton("Custom", &ms_nPacketTypes, PACKET_TYPES_CUSTOM);

        ImGui::Spacing();

        {
            ImGui::Text("Packet Types (%d/%d)",
                std::count(ms_abPacketsEnabled, ms_abPacketsEnabled + ARRAY_SIZE(ms_abPacketsEnabled), true),
                ARRAY_SIZE(ms_abPacketsEnabled));

            float packetListMaxHeight = ImGui::GetStyle().ItemSpacing.y * 2.0f + 2.0f + ImGui::GetFrameHeight();

            ImGui::BeginChild("PacketList", ImVec2(0.0f, -packetListMaxHeight), ImGuiChildFlags_Borders);

            ImGui::SetWindowFontScale(0.8f);
            float boxSize = ImGui::GetFrameHeight();
            for (int i = 0; i < ARRAY_SIZE(ms_abPacketsEnabled); i++)
            {
                const char* typeString = ePacketType_ToString((ePacketType)i);
                if (searchText[0] && !strstr(typeString, searchText))
                {
                    continue;
                }
                if (ImGui::Checkbox(typeString, &ms_abPacketsEnabled[i]))
                {
                    ms_nPacketTypes = PACKET_TYPES_CUSTOM;
                }
            }
            ImGui::SetWindowFontScale(1.0f);

            ImGui::EndChild();
        }

        ImGui::Separator();

        if (ImGui::Button("Reset Filters", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            ms_nPacketTypes = PACKET_TYPES_CUSTOM;
            ms_nDirection = DIRECTION_ALL;
            std::fill_n(ms_abPacketsEnabled, ARRAY_SIZE(ms_abPacketsEnabled), false);
            searchText[0] = '\0';
        }

        ImGui::EndChild();
    }

    ImGui::SameLine();

    {
        ImGui::BeginChild("##toolbar", ImVec2(0, ImGui::GetFrameHeight() * 2.0f), ImGuiChildFlags_Borders);

        ImGui::Text("Time Range");

        ImGui::SameLine();

        ImGui::SetNextItemWidth(200);

        ImGui::SliderInt("##timerange", &ms_nSecondRange, 1, 60, "%d s");

        ImGui::SameLine();
        ImGui::Spacing();

        ImGui::SameLine();
        ImGui::Text("Direction");

        ImGui::SameLine();
        ImGui::RadioButton("All##Direction", &ms_nDirection, DIRECTION_ALL);

        ImGui::SameLine();
        ImGui::RadioButton("Inbound", &ms_nDirection, DIRECTION_INBOUND);

        ImGui::SameLine();
        ImGui::RadioButton("Outbound", &ms_nDirection, DIRECTION_OUTBOUND);

        ImGui::EndChild();
    }

    {
        ImGui::SetCursorPos(ImVec2(fFiltersWidth + ImGui::GetStyle().ItemSpacing.x * 2.0f,
            ImGui::GetFrameHeight() * 3.5f + ImGui::GetStyle().ItemSpacing.y * 4.0f));

        float filtersMaxHeight = ImGui::GetStyle().ItemSpacing.y * 5.0f + ImGui::GetFrameHeight();

        ImGui::BeginChild("##Timeline", ImVec2(0, -filtersMaxHeight), ImGuiChildFlags_Borders);

        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("Timeline");
        ImGui::SetWindowFontScale(1.0f);

        float cursorY = ImGui::GetCursorPosY();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetWindowFontScale(0.8f);

        {
            float height = ImGui::GetFrameHeight();
            for (int i = 0; i < ARRAY_SIZE(ms_abPacketsEnabled); i++)
            {
                if (!ms_abPacketsEnabled[i])
                {
                    continue;
                }
                const char* typeString = ePacketType_ToString((ePacketType)i);

                ImVec4 checkColour = GetPacketColor((ePacketType)i);

                ImGui::ColorButton(typeString + 1, checkColour,
                    ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(height, height));

                ImGui::SameLine();

                ImGui::Text(typeString);
            }
        }

        {
            ImGui::SetCursorPos(ImVec2(200.0f, cursorY + ImGui::GetStyle().ItemSpacing.y));

            ImVec2 cursorGlobalPos = ImGui::GetCursorScreenPos();
            ImVec2 cursorLocalPos = ImGui::GetCursorPos();

            ImDrawList* pDrawList = ImGui::GetWindowDrawList();

            // draw horizontal lines
            const float gapSizeY = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;
            int count = 1;
            for (int i = 0; i < ARRAY_SIZE(ms_abPacketsEnabled); i++)
            {
                if (!ms_abPacketsEnabled[i])
                {
                    continue;
                }

                pDrawList->AddLine(ImVec2(cursorGlobalPos.x - ImGui::GetStyle().ItemSpacing.x * 3.0f,
                                       cursorGlobalPos.y + gapSizeY * count),
                    ImVec2(cursorGlobalPos.x + ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight(),
                        cursorGlobalPos.y + gapSizeY * count),
                    IM_COL32(255, 255, 255, 30), 1.0f);

                count++;
            }

            // draw vertical lines
            const float lineCount = 10.0f;  // dont change please
            const float gapSizeX = (ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight()) / lineCount;
            for (float i = 0; i <= lineCount; i++)
            {
                ImGui::SetCursorPos(ImVec2(cursorLocalPos.x + gapSizeX * i, cursorLocalPos.y));
                if (i == lineCount)
                {
                    ImGui::TextColored(ImColor(255, 0, 0, 255), "NOW");
                    pDrawList->AddLine(
                        ImVec2(cursorGlobalPos.x + gapSizeX * i, cursorGlobalPos.y + ImGui::GetStyle().ItemSpacing.y),
                        ImVec2(cursorGlobalPos.x + gapSizeX * i, cursorGlobalPos.y + 9999.9f), IM_COL32(255, 0, 0, 255),
                        1.0f);
                }
                else
                {
                    float mult = lineCount / static_cast<float>(ms_nSecondRange);
                    ImGui::Text("-%.1fs", (lineCount - i) / mult);

                    pDrawList->AddLine(
                        ImVec2(cursorGlobalPos.x + gapSizeX * i, cursorGlobalPos.y + ImGui::GetStyle().ItemSpacing.y),
                        ImVec2(cursorGlobalPos.x + gapSizeX * i, cursorGlobalPos.y + 9999.9f),
                        IM_COL32(255, 255, 255, 30), 1.0f);
                }
            }

            count = 1;
            bool bDrewToolTip = false;
            for (int i = 0; i < ARRAY_SIZE(ms_abPacketsEnabled); i++)
            {
                if (!ms_abPacketsEnabled[i])
                {
                    continue;
                }
                float prevCircleX = -999.0f;
                for (const SPacketRecord& packetRecord : GetPacketFactory().m_records[i])
                {
                    if (ms_nDirection != DIRECTION_ALL &&
                        (ms_nDirection != DIRECTION_INBOUND || !packetRecord.m_bInbound) &&
                        (ms_nDirection != DIRECTION_OUTBOUND || !packetRecord.m_bOutbound))
                    {
                        continue;
                    }

                    float timeDiff = static_cast<float>(g_serverTime - packetRecord.m_serverTime);
                    if (timeDiff >= 0.0f && timeDiff < 1000.0f * static_cast<float>(ms_nSecondRange))
                    {
                        float radius = 3.0f;
                        timeDiff /= 1000.0f * static_cast<float>(ms_nSecondRange);

                        float a = cursorGlobalPos.x;
                        float b = cursorGlobalPos.x + gapSizeX * lineCount;
                        float circleX = Lerp(timeDiff, b, a);

                        if (std::abs(prevCircleX - circleX) < 1.0f)
                        {
                            continue;
                        }
                        prevCircleX = circleX;

                        ImVec4 checkColour = GetPacketColor((ePacketType)i);

                        ImVec2 center = ImVec2(circleX, cursorGlobalPos.y + gapSizeY * count);
                        pDrawList->AddCircleFilled(center, radius, ImGui::ColorConvertFloat4ToU32(checkColour));

                        if (!bDrewToolTip)
                        {
                            ImVec2 mouse = ImGui::GetIO().MousePos;
                            float dx = mouse.x - center.x;
                            float dy = mouse.y - center.y;

                            radius = 9.0f;
                            if (dx * dx + dy * dy <= radius * radius)
                            {
                                bDrewToolTip = true;
                                ImGui::BeginTooltip();

                                ImGui::ColorButton("##tooltip", checkColour,
                                    ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
                                    ImVec2(radius, radius));

                                ImGui::SameLine();

                                const char* typeString = ePacketType_ToString((ePacketType)i);

                                ImGui::Text(typeString);

                                ImGui::Spacing();

                                ImGui::TextDisabled("Direction:");
                                ImGui::SameLine();
                                if (packetRecord.m_bInbound)
                                {
                                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Inbound");
                                }
                                else if (packetRecord.m_bOutbound)
                                {
                                    ImGui::TextColored(ImVec4(0.7f, 0.4f, 1.0f, 1.0f), "Outbound");
                                }

                                ImGui::TextDisabled("Time:");
                                ImGui::SameLine();
                                ImGui::Text("-%.3f s", (g_serverTime - packetRecord.m_serverTime) / 1000.0f);

                                ImGui::TextDisabled("Size:");
                                ImGui::SameLine();
                                ImGui::Text("%d bytes", packetRecord.m_nSize);

                                if (packetRecord.m_sContent.length() != 0)
                                {
                                    ImGui::TextDisabled("Content:");
                                    ImGui::Text(packetRecord.m_sContent.c_str());
                                }

                                ImGui::EndTooltip();
                            }
                        }
                    }
                }

                count++;
            }
        }
        ImGui::SetWindowFontScale(1.0f);

        ImGui::EndChild();
    }

    ImGui::Spacing();

    {
        ImGui::BeginChild("##Footer", ImVec2(0, 0), ImGuiChildFlags_Borders);

        ImGui::TextDisabled("Packets: %d", CNetwork::GetReceivedCount() + CNetwork::GetSentCount());

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextDisabled("-");
        ImGui::SameLine();
        ImGui::Spacing();

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "In: %d", CNetwork::GetReceivedCount());

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextDisabled("-");
        ImGui::SameLine();
        ImGui::Spacing();

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.4f, 1.0f, 1.0f), "Out: %d", CNetwork::GetSentCount());

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextDisabled("-");
        ImGui::SameLine();
        ImGui::Spacing();

        ImGui::SameLine();
        ImGui::TextDisabled("Loss: %d (%.2f%%)", CNetwork::GetDroppedCount(), CNetwork::GetDroppedRatio());

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        float cursorX = ImGui::GetContentRegionMax().x -
                        ImGui::CalcTextSize("Bandwidth:In 000.0 KB/sOut 000.0 KB/s").x -
                        ImGui::GetStyle().ItemSpacing.x * 4.0f;
        ImGui::SetCursorPosX(cursorX);

        ImGui::TextDisabled("Bandwidth:");

        ImGui::SameLine();
        ImGui::TextColored(
            ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "In %.1f KB/s", CNetwork::ms_nBytesReceivedThisSecond / 1024.0f);

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextDisabled("-");
        ImGui::SameLine();
        ImGui::Spacing();

        ImGui::SameLine();
        ImGui::TextColored(
            ImVec4(0.7f, 0.4f, 1.0f, 1.0f), "Out %.1f KB/s", CNetwork::ms_nBytesSentThisSecond / 1024.0f);

        ImGui::EndChild();
    }
    ImGui::End();
}
