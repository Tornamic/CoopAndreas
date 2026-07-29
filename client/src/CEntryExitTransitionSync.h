#pragma once

#include <cstdint>

class CEntryExit;
class CPed;

namespace Packets::Scripts
{
class EnExTransition;
}

class CEntryExitTransitionSync
{
public:
    static void OnTransitionStarted(CEntryExit* entryExit, CPed* ped, bool started);
    static void OnTransitionFinished(CEntryExit* entryExit, CPed* ped, bool finished);
    static void Receive(const Packets::Scripts::EnExTransition& packet);
    static void Process();

private:
    static inline CEntryExit* ms_pLocalAnimatedTransition = nullptr;
    static inline uint8_t ms_nLocalEnExAreaId = 0;
    static inline int16_t ms_nLocalRectLeft = 0;
    static inline int16_t ms_nLocalRectBottom = 0;

    static CEntryExit* FindEntryExit(int16_t rectLeft, int16_t rectBottom, uint8_t areaId);
    static void Send(CEntryExit* entryExit, CPed* ped, bool finished);
};
