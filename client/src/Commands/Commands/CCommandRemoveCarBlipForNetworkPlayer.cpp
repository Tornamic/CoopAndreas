#include "stdafx.h"
#include "CCommandRemoveCarBlipForNetworkPlayer.h"

void CCommandRemoveCarBlipForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(2);

	auto ped = CPools::GetPed(ScriptParams[0]);
	assert(ped && "opcode 0xF05: invalid CPed*");

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(ped);
	assert(networkPlayer && "opcode 0xF05: invalid CNetworkPlayer*");

	auto vehicle = CPools::GetVehicle(ScriptParams[1]);
	assert(vehicle && "opcode 0xF05: invalid CVehicle*");

	auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle);
	assert(networkVehicle && "opcode 0xF05: invalid CNetworkVehicle*");

	Packets::Blips::RemoveEntityBlip packet{};
	packet.forWhoPlayerId = networkPlayer->m_iPlayerId;
	packet.entity.SetEntity(vehicle);

	GetPacketFactory().Send(packet);
}