#include "stdafx.h"

bool CNetwork::Init(const char* ip, int port)
{
    if (enet_initialize() != 0) // try to init enet
        return false;


	m_pClient = enet_host_create(nullptr, 1, 2, 0, 0); // create enet client

	if (m_pClient == nullptr) // check client
		return false;

	ENetAddress address; // connection address

	enet_address_set_host(&address, ip); // set address ip

	address.port = port; // set address port

	ENetPeer* peer = enet_host_connect(m_pClient, &address, 2, 0); // connect to the server

	if (peer == nullptr) // if not connected
		return false;
	
	return true;
}