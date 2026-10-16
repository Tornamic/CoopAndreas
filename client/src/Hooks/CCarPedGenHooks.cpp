#include "CCarPedGenHooks.h"

CMatrix* Invert(CMatrix* a1, CMatrix* out)
{
    out->pos.x = 0.0;
    out->pos.y = 0.0;
    out->pos.z = 0.0;
    out->right.x = a1->right.x;
    out->right.y = a1->up.x;
    out->right.z = a1->at.x;
    out->up.x = a1->right.y;
    out->up.y = a1->up.y;
    out->up.z = a1->at.y;
    out->at.x = a1->right.z;
    out->at.y = a1->up.z;
    out->at.z = a1->at.z;
    out->pos.x = a1->pos.x * out->right.x;
    out->pos.y = out->right.y * a1->pos.x;
    out->pos.z = out->right.z * a1->pos.x;
    out->pos.x = a1->pos.y * out->up.x + out->pos.x;
    out->pos.y = out->up.y * a1->pos.y + out->pos.y;
    out->pos.z = out->up.z * a1->pos.y + out->pos.z;
    out->pos.x = out->at.x * a1->pos.z + out->pos.x;
    out->pos.y = a1->pos.z * out->at.y + out->pos.y;
    out->pos.z = out->at.z * a1->pos.z + out->pos.z;
    out->pos.x = out->pos.x * -1.0;
    out->pos.y = out->pos.y * -1.0;
    out->pos.z = out->pos.z * -1.0;
    return out;
}

bool CCarPedGenHooks::IsSphereVisibleForNetworkPlayer(CVector* pOrigin, float fRadius, CNetworkPlayer* pNetworkPlayer)
{

    bool bResult = false;
    const Packets::Players::PlayerCameraSync& camSnapshot = pNetworkPlayer->m_cameraSnapshot;
    CMatrix matrix{};

    matrix.GetPosition() = camSnapshot.source;
    matrix.GetForward() = camSnapshot.front;
    matrix.GetUp() = camSnapshot.up;
    matrix.GetRight().Cross(matrix.GetUp(), matrix.GetForward());

    CMatrix invMat;
    Invert(&matrix, &invMat);

    return TheCamera.IsSphereVisible(*pOrigin, fRadius, (RwMatrixTag*)&invMat);
}

bool IsSphereVisibleForAnyNetworkPlayer(CVector* pOrigin, float fRadius)
{
    for (CNetworkPlayer* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (CCarPedGenHooks::IsSphereVisibleForNetworkPlayer(pOrigin, fRadius, pNetworkPlayer))
        {
            return true;
        }
    }
    return false;
}

bool __fastcall CCamera__IsSphereVisible_Hook(CCamera* This, SKIP_EDX, CVector* pOrigin, float fRadius)
{
    if (This->IsSphereVisible(*pOrigin, fRadius))
    {
        return true;
    }

    return IsSphereVisibleForAnyNetworkPlayer(pOrigin, fRadius);
}

bool __fastcall CEntity__GetIsOnScreen_Hook(CEntity* This, SKIP_EDX)
{
    if (This->GetIsOnScreen())
    {
        return true;
    }
    float fRadius;
    CVector origin;
    This->TransformFromObjectSpace(
        origin, CModelInfo::ms_modelInfoPtrs[This->m_nModelIndex]->m_pColModel->m_boundSphere.m_vecCenter);
    fRadius = CModelInfo::ms_modelInfoPtrs[This->m_nModelIndex]->m_pColModel->m_boundSphere.m_fRadius;
    return IsSphereVisibleForAnyNetworkPlayer(&origin, fRadius);
}

void CCarPedGenHooks::InjectHooks()
{
    // TODO: camera checks are not enough, hook distance checks too
    // TODO 0x425053 take into account player dists too
    // TODO add CGangWars checks once syncing

    // CCarCtrl::GenerateCarCreationCoors2
    patch::RedirectCall(0x42484E, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x42498E, CCamera__IsSphereVisible_Hook);

    // CCarCtrl::PossiblyRemoveVehicle
    patch::RedirectCall(0x425057, CEntity__GetIsOnScreen_Hook);
    patch::RedirectCall(0x4251A0, CEntity__GetIsOnScreen_Hook);
    patch::RedirectCall(0x4252B3, CEntity__GetIsOnScreen_Hook);
    patch::RedirectCall(0x4253B1, CEntity__GetIsOnScreen_Hook);

    // CCarCtrl::CreatePoliceChase
    patch::RedirectCall(0x42C4A0, CEntity__GetIsOnScreen_Hook);
    patch::RedirectCall(0x42C4B8, CCamera__IsSphereVisible_Hook);

    // CPathFind::GeneratePedCreationCoors
    patch::RedirectCall(0x44EB5E, CCamera__IsSphereVisible_Hook);

    // ped related,
    // TODO: there are also distance checks, take into account network players dists too
    patch::RedirectCall(0x5FCA07, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x5FCEF6, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x5FD3A6, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x613DB0, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x614C10, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x615278, CCamera__IsSphereVisible_Hook);
    patch::RedirectCall(0x615B36, CCamera__IsSphereVisible_Hook);

    // CPopulation::ManagePed
    patch::RedirectCall(0x6121C4, CEntity__GetIsOnScreen_Hook);
    patch::RedirectCall(0x612157, CEntity__GetIsOnScreen_Hook);
}
