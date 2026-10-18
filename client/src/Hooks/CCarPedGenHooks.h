#pragma once
class CCarPedGenHooks
{
public:
    static bool IsSphereVisibleForNetworkPlayer(CVector* pOrigin, float fRadius, CNetworkPlayer* pNetworkPlayer);
    static bool IsSphereVisibleForAnyNetworkPlayer(CVector* pOrigin, float fRadius);
    static bool IsSphereVisibleForAnyPlayer(CVector* pOrigin, float fRadius);
    static bool IsEntityOnAnyPlayersScreen(CEntity* pEntity);
    static void InjectHooks();
};
