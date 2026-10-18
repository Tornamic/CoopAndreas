#include "stdafx.h"
#include "CCutsceneSkipVote.h"
#include <set>
#include "CHudColours.h"

// shared state, shown to everyone
static int nVotes = 0;
static int nVoters = 0;
static bool bVoted = false;

// host only
static bool bSkipped = false;
static std::set<playerid_t> voters;
static std::set<playerid_t> votes;

static void BroadcastState()
{
    nVotes = votes.size();
    nVoters = voters.size();

    Packets::Scripts::CutsceneSkipVoteState packet{};
    packet.votes = nVotes;
    packet.voters = nVoters;
    GetPacketFactory().Send(packet);
}

static void AddVote(playerid_t playerid)
{
    if (voters.count(playerid) && votes.insert(playerid).second)
    {
        BroadcastState();
    }
}

static bool RemoveDisconnected(std::set<playerid_t>& players)
{
    size_t oldSize = players.size();
    for (auto it = players.begin(); it != players.end();)
    {
        if (*it != CNetworkPlayerManager::m_localPlayerId && !CNetworkPlayerManager::GetPlayer(*it))
        {
            it = players.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return players.size() != oldSize;
}

static bool CCutsceneMgr__IsCutsceneSkipButtonBeingPressed_Hook()
{
    if (!bVoted && CCutsceneMgr::IsCutsceneSkipButtonBeingPressed())
    {
        bVoted = true;

        if (CLocalPlayer::m_bIsHost)
        {
            AddVote(CNetworkPlayerManager::m_localPlayerId);
        }
        else
        {
            Packets::Scripts::CutsceneSkipVote packet{};
            GetPacketFactory().Send(packet);
        }
    }

    if (!CLocalPlayer::m_bIsHost)
    {
        return false;
    }

    if (!bSkipped)
    {
        bool bVotersChanged = RemoveDisconnected(voters);
        bool bVotesChanged = RemoveDisconnected(votes);
        if (bVotersChanged || bVotesChanged)
        {
            BroadcastState();
        }

        bSkipped = votes.size() == voters.size();
        if(CCore::ms_bDebug)
        {
            if(votes.size() > 0)
            {
                bSkipped = true;
            }
        }
    }

    return bSkipped;
}

static void CCutsceneMgr__StartCutscene_Hook()
{
    nVotes = 0;
    nVoters = 0;
    bVoted = false;
    bSkipped = false;
    votes.clear();
    voters.clear();

    if (CLocalPlayer::m_bIsHost)
    {
        for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
        {
            voters.insert(pNetworkPlayer->m_playerId);
        }
        voters.insert(CNetworkPlayerManager::m_localPlayerId);
        BroadcastState();
    }

    CCutsceneMgr::StartCutscene();
}

PACKET_HANDLER(ePacketType::CUTSCENE_SKIP_VOTE, Packets::Scripts::CutsceneSkipVote* pCutsceneSkipVote)
{
    if (CLocalPlayer::m_bIsHost)
    {
        AddVote(pCutsceneSkipVote->senderPlayerid.value);
    }
}

PACKET_HANDLER(ePacketType::CUTSCENE_SKIP_VOTE_STATE, Packets::Scripts::CutsceneSkipVoteState* pCutsceneSkipVoteState)
{
    nVotes = pCutsceneSkipVoteState->votes;
    nVoters = pCutsceneSkipVoteState->voters;
}

static uintptr_t DrawWidescreen_ptr = 0x0;
static void* DrawWidescreen_Hook(int a1)
{
    if (CCutsceneMgr::ms_running && nVotes > 0)
    {
        char szText[64];
        sprintf(szText, "Skip? (%d/%d)%s", nVotes, nVoters, bVoted ? " - Voted" : "");

        CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        CFont::SetEdge(1);
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GREY));
        CFont::SetScale(CUtil::HUD_X(0.3f), CUtil::HUD_Y(0.4f));
        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        CFont::SetJustify(false);

        CFont::PrintString(CUtil::HUD_X(640.0f - 10.0f), CUtil::HUD_Y(448.0f - 32.0f), szText);
    }

    return plugin::CallAndReturnDyn<void*, int>(DrawWidescreen_ptr, a1);
}

void CCutsceneSkipVote::InjectHooks()
{
    patch::RedirectCall(0x48072B, CCutsceneMgr__StartCutscene_Hook);
    patch::RedirectCall(0x5B1947, CCutsceneMgr__IsCutsceneSkipButtonBeingPressed_Hook);

    DrawWidescreen_ptr = injector::GetBranchDestination(0x53E2BC).as_int();
    patch::RedirectCall(0x53E2BC, DrawWidescreen_Hook);
}
