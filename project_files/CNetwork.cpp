#include "CNetwork.h" //Declaring it BEFORE the stdafx.h to garantee this Source will get the fully compiled ENET library
#include "stdafx.h"

ENetHost* CNetwork::m_pClient;
bool CNetwork::Init(const char* ip, int port)
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

	enet_address_set_host(&address, ip); // set address ip
	address.port = port; // set address port

	ENetPeer* peer = enet_host_connect(m_pClient, &address, 2, 0); // connect to the server
	if (peer == NULL) { // if not connected
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
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		std::cout << "Connection failed." << std::endl;
	}
	
	return true;
}