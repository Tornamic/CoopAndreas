#include "CHeadMovement.h"

void IKChainManager_c__LookAt(const char* purpose, CPed* pPed, CEntity* lookAtEntity, int time, int offsetBone, 
					CVector* offset, bool useTorso, float fSpeed, int blendTime, int priority, bool bForceLooking)
{
	// IKChainManager_c::LookAt
	plugin::CallMethod<0x618970, DWORD, const char*, CPed*, CEntity*, int, int, CVector*, bool, float, int, int, bool>(0xC15448, purpose, 
			pPed, lookAtEntity, time, offsetBone, offset, useTorso, fSpeed, blendTime, priority, bForceLooking);
}

void CHeadMovement::Process()
{
	Packets::Players::HeadMovement packet{};

	CVector posn = FindPlayerPed(0)->m_matrix->pos;
	CCam& cam = TheCamera.m_aCams[TheCamera.m_nActiveCam];

	CVector LookAt = (cam.m_vecFront * 20.0f) + posn;
	IKChainManager_c__LookAt("FollowPedSA", FindPlayerPed(0), nullptr, 2000, -1, &LookAt, false, 0.1f, 500, 3, false);

	packet.direction = cam.m_vecFront;
	GetPacketFactory().Send(packet);
}

void CHeadMovement::HandlePacket(Packets::Players::HeadMovement* pHeadMovement, CNetworkPlayer* pNetworkPlayer)
{
	CVector posn = pNetworkPlayer->m_pPed->m_matrix->pos;
	CVector LookAt = (pHeadMovement->direction * 20.0f) + posn;
	IKChainManager_c__LookAt("FollowPedSA", pNetworkPlayer->m_pPed, nullptr, 1000, -1, &LookAt, false, 0.1f, 500, 3, false);
}
