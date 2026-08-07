#include "CMoonSync.h"
#include <CCoronas.h>

void CMoonSync::SyncCurrentState()
{
    Packets::World::UpdateMoonSize packet{};
    packet.moonSize = CCoronas::MoonSize;
	GetPacketFactory().Send(packet);
}

void CMoonSync::HandlePacket(Packets::World::UpdateMoonSize* pUpdateMoonSize)
{
    CCoronas::MoonSize = pUpdateMoonSize->moonSize;
}