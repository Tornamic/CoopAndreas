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
};

