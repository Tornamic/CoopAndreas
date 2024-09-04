#pragma once
class CPacketHandler
{
public:
	static void PlayerConnected__Handle(void* data, int size);
	static void PlayerDisconnected__Handle(void* data, int size);
	static CPackets::PlayerOnFoot* PlayerOnFoot__Collect();
	static void PlayerOnFoot__Handle(void* data, int size);
	static void PlayerBulletShot__Handle(void* data, int size);
	static void PlayerHandshake__Handle(void* data, int size);
	static void PlayerPlaceWaypoint__Handle(void* data, int size);
	static void PlayerGetName__Handle(void* data, int size);
	static void PlayerSetHost__Handle(void* data, int size);
	static void AddExplosion__Handle(void* data, int size);
	static void VehicleSpawn__Handle(void* data, int size);
	static void VehicleRemove__Handle(void* data, int size);
	static CPackets::VehicleIdleUpdate* VehicleIdleUpdate__Collect(CNetworkVehicle* vehicle);
	static void VehicleIdleUpdate__Handle(void* data, int size);
	static CPackets::VehicleDriverUpdate* VehicleDriverUpdate__Collect(CNetworkVehicle* vehicle);
	static void VehicleDriverUpdate__Handle(void* data, int size);
	static void VehicleEnter__Handle(void* data, int size);
	static void VehicleExit__Handle(void* data, int size);
	static void VehicleDamage__Handle(void* data, int size);
	static void VehicleComponentAdd__Handle(void* data, int size);
	static void VehicleComponentRemove__Handle(void* data, int size);
	static CPackets::VehiclePassengerUpdate* VehiclePassengerUpdate__Collect(CNetworkVehicle* vehicle, CPlayerPed* localPlayer);
	static void VehiclePassengerUpdate__Handle(void* data, int size);
};

