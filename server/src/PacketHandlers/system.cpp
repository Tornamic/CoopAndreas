#include "network/packet_types.h"
#include "stdafx.h"
#include "network/packet_handler.h"

PACKET_HANDLER(ePacketType::PLAYER_PING, Packets::System::PlayerPing* pPlayerPing, CNetworkPlayer* pNetworkPlayer)
{
    if (pPlayerPing->playerCount != 1)
    {
        logger::warn("Player %d sent an invalid ping report count: %d", pNetworkPlayer->m_iPlayerId,
            pPlayerPing->playerCount);
        return;
    }

    pNetworkPlayer->m_nPing = pPlayerPing->ping[0];
}

PACKET_HANDLER(ePacketType::PLAYER_CHAT_MESSAGE, Packets::System::ChatMessage* pChatMessage, CNetworkPlayer* pNetworkPlayer)
{
	pChatMessage->playerid = pNetworkPlayer->m_iPlayerId;
	GetPacketFactory().SendToAll(*pChatMessage, pNetworkPlayer);
}
