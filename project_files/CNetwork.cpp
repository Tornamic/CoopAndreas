#include "stdafx.h"

ENetHost* CNetwork::m_pClient = nullptr;
ENetPeer* CNetwork::m_pPeer = nullptr;
bool CNetwork::m_bConnected = false;

DWORD WINAPI temp_send_onfoot(LPVOID)
{
	Sleep(2000);
	while (true)
	{
		CPackets::PlayerOnFoot* packet = {};

		packet->position = FindPlayerPed(0)->m_matrix->pos;
		packet->velocity = FindPlayerPed(0)->m_vecMoveSpeed;
		/*packet->rotation = FindPlayerPed(0)->m_fCurrentRotation;*/

		CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

		Sleep(100);
	}
}

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
	// wait some time
	Sleep(2000);

	if (enet_initialize() != 0) { // try to init enet
		std::cout << "Fail to enet_initialize" << std::endl;
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

	enet_address_set_host(&address, "127.0.0.1"); // set address ip
	address.port = 6767; // set address port

	m_pPeer = enet_host_connect(m_pClient, &address, 2, 0); // connect to the server
	if (m_pPeer == NULL) { // if not connected
		std::cout << "Not Connected" << std::endl;
		return false;
	}

	ENetEvent event;
	if (enet_host_service(m_pClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		m_bConnected = true;
		std::cout << "Connection succeeded." << std::endl;
	}
	else
	{
		enet_peer_reset(m_pPeer);
		std::cout << "Connection failed." << std::endl;
	}

	while (true) //This is taking the main thread to itself. We need to run the client in a different thread. Remove this loop to keep developing without receiving packets from the server.
	{
		enet_host_service(m_pClient, &event, 5000);
		switch (event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::cout << "Packet Received!" << std::endl;

				unsigned short id;

				// extract id
				memcpy(&id, event.packet->data, 2);

				// extract size
				size_t dataSize = event.packet->dataLength - 2;

				// allocate memory for data
				char* data = new char[dataSize];

				// extract data
				memcpy(data, event.packet->data + 2, dataSize);
				
				enet_packet_destroy(event.packet); //You should destroy after used it

				// todo: create packet handler and separate it in different functions
				if (id == CPacketsID::PLAYER_CONNECTED) 
				{
					// get packet struct
					CPackets::PlayerConnected* packet = (CPackets::PlayerConnected*)data;

					// create new player
					CNetworkPlayer* player = new CNetworkPlayer(packet->id, CVector(2246.506f, -1259.552f, 23.9531f));

					// add player to list
					CNetworkPlayerManager::Add(player);


				}
				else if (id == CPacketsID::PLAYER_ONFOOT)
				{
					// get packet struct
					CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

					// get player instance 
					CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->id);

					// check if player not created
					if (player == nullptr)
						break;

					player->m_pPed->m_matrix->pos = packet->position;
					player->m_pPed->m_vecMoveSpeed = packet->velocity;
					player->m_pPed->m_fCurrentRotation = packet->rotation;

					player->m_lOnFoot = packet;
				}
				break;

			}
			case ENET_EVENT_TYPE_NONE:
				break;
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

