#pragma once
class CCarPedGenHooks
{
public:
    static bool IsSphereVisibleForNetworkPlayer(CVector* pOrigin, float fRadius, CNetworkPlayer* pNetworkPlayer);
    static void InjectHooks();
};
