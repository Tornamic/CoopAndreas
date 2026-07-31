#pragma once

class CEntryExit;
class CPed;

namespace Packets::Players
{
class EnExTransition;
}

class CEntryExitTransitionSync
{
public:
    static void OnTransitionStarted(CEntryExit* pEntryExit, CPed* pPed);
    static void OnTransitionFinished(CEntryExit* pEntryExit, CPed* pPed);
    static void Receive(const Packets::Players::EnExTransition& packet);
    static void Process();

private:
    static inline CEntryExit* ms_pLocalAnimatedTransition = nullptr;
};
