#include "stdafx.h"

ENetHost* CNetwork::m_pClient = nullptr;
ENetPeer* CNetwork::m_pPeer = nullptr;

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
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
				std::cout << "Packet Received!" << std::endl;
				enet_packet_destroy(event.packet); //You should destroy after used it
				break;
			case ENET_EVENT_TYPE_NONE:
				break;
		}

	}
	
	return true;
}

void CNetwork::SendPacket(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag)
{
	// packet size `id + data`
	size_t packetSize = sizeof(id) + dataSize;

	// create buffer
	char* packetData = new char[packetSize];

	// copy id
	memcpy(packetData, &id, sizeof(id));

	// copy data
	memcpy(packetData + sizeof(id), data, dataSize);

	// create packet
	ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

	// send packet
	enet_peer_send(m_pPeer, 0, packet);

	// free buffer
	delete[] packetData;
}