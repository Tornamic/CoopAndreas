#include "stdafx.h"
#include <string>
#include "enet/enet.h"

bool CNetwork::Init(unsigned short port)
{

    if (enet_initialize() != 0) // try to init enet
    {
        printf("init failed");
        return false;
    }

    ENetAddress address;

    address.host = ENET_HOST_ANY; // bind server ip
    address.port = port; // bind server port

    ENetHost* server = enet_host_create(&address, MAX_SERVER_PLAYERS, 2, 0, 0); // create enet host

    if (server == NULL)
    {
        printf("creating failed");
        return false;
    }

    printf("Server stared on port %d\n", port);

    ENetEvent event;
    while (true) // waiting for event
    {
        enet_host_service(server, &event, 10);
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.peer->data,
                    event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("%s disconnected.\n", event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;
            }
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    std::cout << "Destroyed" << std::endl;
    return 0;
}