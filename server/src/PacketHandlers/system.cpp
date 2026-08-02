#include "network/packet_types.h"
#include "stdafx.h"
#include "network/packet_handler.h"

PACKET_HANDLER(ePacketType::PLAYER_CHAT_MESSAGE, Packets::System::ChatMessage* pChatMessage, CNetworkPlayer* pNetworkPlayer)
{
	pChatMessage->playerid = pNetworkPlayer->m_iPlayerId;
	GetPacketFactory().SendToAll(*pChatMessage, pNetworkPlayer);
}