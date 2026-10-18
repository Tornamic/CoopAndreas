#include "network/packet_types.h"
#include "stdafx.h"
#include "network/packet_handler.h"

PACKET_HANDLER(ePacketType::PLAYER_CHAT_MESSAGE, Packets::System::ChatMessage* pChatMessage, CNetworkPlayer* pNetworkPlayer)
{
	pChatMessage->senderPlayerid = pNetworkPlayer->m_playerId;
	GetPacketFactory().SendToAll(*pChatMessage, pNetworkPlayer);
}

PACKET_HANDLER(
    ePacketType::HORSESHOE_STATE, Packets::System::HorseshoeState* pHorseshoeState, CNetworkPlayer* pNetworkPlayer)
{
    GetPacketFactory().SendToAll(*pHorseshoeState, pNetworkPlayer);
}