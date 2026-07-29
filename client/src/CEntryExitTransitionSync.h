#pragma once

class CEntryExit;
class CPed;

namespace Packets::Scripts
{
class EnExTransition;
}

class CEntryExitTransitionSync
{
public:
    static void OnTransitionStarted(CEntryExit* entryExit, CPed* ped);
    static void OnTransitionFinished(CEntryExit* entryExit, CPed* ped);
    static void Receive(const Packets::Scripts::EnExTransition& packet);
    static void Process();

private:
    static inline CEntryExit* ms_pLocalAnimatedTransition = nullptr;
};
