#include "stdafx.h"

ENetHost* CNetwork::m_pClient = nullptr;
ENetPeer* CNetwork::m_pPeer = nullptr;
bool CNetwork::m_bConnected = false;

std::vector<CPacketListener*> CNetwork::m_packetListeners;

char CNetwork::m_IpAddress[128 + 1];
unsigned short CNetwork::m_nPort;

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
	// init listeners
	CNetwork::InitListeners();

	if (enet_initialize() != 0) { // try to init enet
		CChat::AddMessage("{cecedb}[Network] {ff0000}Failed to enet_initialize.");
		return false;
	}
	else
	{
		std::cout << "Success to enet_initialize" << std::endl;
	}

	m_pClient = enet_host_create(NULL, 1, 2, 0, 0); // create enet client
	if (m_pClient == NULL) // check client
		return false;

	ENetAddress address; // connection address

	enet_address_set_host(&address, m_IpAddress); // set address ip
	address.port = m_nPort; // set address port

	m_pPeer = enet_host_connect(m_pClient, &address, 2, 0); // connect to the server
	if (m_pPeer == NULL) { // if not connected
		CChat::AddMessage("{cecedb}[Network] {ff0000}m_pPeer == NULL.");
		//std::cout << "Not Connected" << std::endl;
		return false;
	}

	CChat::AddMessage("{cecedb}[Network] Connecting to the server...");
	ENetEvent event;
	while (!m_bConnected)
	{
		if (enet_host_service(m_pClient, &event, 2000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			m_bConnected = true;
			CChat::AddMessage("{cecedb}[Network] {00ff00}Successfully {cecedb}connected to the server.");
			CPatch::RevertTemporaryPatches();
		}
		else
		{
			//enet_peer_reset(m_pPeer);
			CChat::AddMessage("{cecedb}[Network] Failed to connect. Retrying...");
		}
	}

	
	return true;
}

void CNetwork::SendPacket(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag)
{
	// 2 == sizeof(unsigned short)
	
	// packet size `id + data`
	size_t packetSize = 2 + dataSize;

	// create buffer
	char* packetData = new char[packetSize];

	// copy id
	memcpy(packetData, &id, 2);

	// copy data
	memcpy(packetData + 2, data, dataSize);

	// create packet
	ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

	// send packet
	enet_peer_send(m_pPeer, 0, packet);
}

void CNetwork::Disconnect()
{
	m_bConnected = false;
}

void CNetwork::InitListeners()
{
	CNetwork::AddListener(CPacketsID::PLAYER_ONFOOT, CPacketHandler::PlayerOnFoot__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_CONNECTED, CPacketHandler::PlayerConnected__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_DISCONNECTED, CPacketHandler::PlayerDisconnected__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_BULLET_SHOT, CPacketHandler::PlayerBulletShot__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_HANDSHAKE, CPacketHandler::PlayerHandshake__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_PLACE_WAYPOINT, CPacketHandler::PlayerPlaceWaypoint__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_GET_NAME, CPacketHandler::PlayerGetName__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_SET_HOST, CPacketHandler::PlayerSetHost__Handle);
	CNetwork::AddListener(CPacketsID::ADD_EXPLOSION, CPacketHandler::AddExplosion__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_SPAWN, CPacketHandler::VehicleSpawn__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_REMOVE, CPacketHandler::VehicleRemove__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_ENTER, CPacketHandler::VehicleEnter__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_EXIT, CPacketHandler::VehicleExit__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_DRIVER_UPDATE, CPacketHandler::VehicleDriverUpdate__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_IDLE_UPDATE, CPacketHandler::VehicleIdleUpdate__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_DAMAGE, CPacketHandler::VehicleDamage__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_ADD, CPacketHandler::VehicleComponentAdd__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_REMOVE, CPacketHandler::VehicleComponentRemove__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_PASSENGER_UPDATE, CPacketHandler::VehiclePassengerUpdate__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_CHAT_MESSAGE, CPacketHandler::PlayerChatMessage__Handle);
	CNetwork::AddListener(CPacketsID::PED_SPAWN, CPacketHandler::PedSpawn__Handle);
	CNetwork::AddListener(CPacketsID::PED_REMOVE, CPacketHandler::PedRemove__Handle);
	CNetwork::AddListener(CPacketsID::PED_ONFOOT, CPacketHandler::PedOnFoot__Handle);
	CNetwork::AddListener(CPacketsID::GAME_WEATHER_TIME, CPacketHandler::GameWeatherTime__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_KEY_SYNC, CPacketHandler::PlayerKeySync__Handle);
	CNetwork::AddListener(CPacketsID::PED_ADD_TASK, CPacketHandler::PedAddTask__Handle);
	CNetwork::AddListener(CPacketsID::PED_DRIVER_UPDATE, CPacketHandler::PedDriverUpdate__Handle);
	CNetwork::AddListener(CPacketsID::PED_SHOT_SYNC, CPacketHandler::PedShotSync__Handle);
	CNetwork::AddListener(CPacketsID::PED_PASSENGER_UPDATE, CPacketHandler::PedPassengerSync__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_AIM_SYNC, CPacketHandler::PlayerAimSync__Handle);
	CNetwork::AddListener(CPacketsID::VEHICLE_CONFIRM, CPacketHandler::VehicleConfirm__Handle);
	CNetwork::AddListener(CPacketsID::PED_CONFIRM, CPacketHandler::PedConfirm__Handle);
	CNetwork::AddListener(CPacketsID::PLAYER_STATS, CPacketHandler::PlayerStats__Handle);
	CNetwork::AddListener(CPacketsID::REBUILD_PLAYER, CPacketHandler::RebuildPlayer__Handle);
	CNetwork::AddListener(CPacketsID::RESPAWN_PLAYER, CPacketHandler::RespawnPlayer__Handle);
	CNetwork::AddListener(CPacketsID::ASSIGN_VEHICLE, CPacketHandler::AssignVehicleSyncer__Handle);
}

void CNetwork::HandlePacketReceive(ENetEvent& event)
{
	// get packet id
	unsigned short id;
	memcpy(&id, event.packet->data, 2);

	// get data
	char* data = new char[event.packet->dataLength - 2];
	memcpy(data, event.packet->data + 2, event.packet->dataLength - 2);

	// call listener's callback by id
	for (size_t i = 0; i < m_packetListeners.size(); i++)
	{
		if (m_packetListeners[i]->m_iPacketID == id)
		{
			m_packetListeners[i]->m_callback(data, event.packet->dataLength - 2);
		}
	}
}

void CNetwork::AddListener(unsigned short id, void(*callback)(void*, int))
{
	CPacketListener* listener = new CPacketListener(id, callback);
	m_packetListeners.push_back(listener);
}

