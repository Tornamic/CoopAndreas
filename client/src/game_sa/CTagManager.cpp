#include "stdafx.h"
#include "CTagManager.h"

void CTagManager::SetAlpha(CEntity* entity, uint8_t ucAlpha)
{
	plugin::Call<0x49CEC0>(entity, ucAlpha);
}

bool CTagManager::IsTag(const CEntity* entity)
{
	return plugin::CallAndReturn<bool, 0x49CCE0>(entity);
}

CVector* CTagManager::GetTagPos(int32_t i)
{
	return plugin::CallAndReturn<CVector*, 0x49CEA0>(i);
}

void CTagSync::SyncCurrentState()
{
	Packets::World::UpdateAllTags packet{};

	for (int i = 0; i < ARRAY_SIZE(packet.tags); i++)
	{
		auto& tagDesc = CTagManager::ms_tagDesc[i];
		if (tagDesc.m_pEntity == nullptr)
		{
			continue;
		}
		auto& pos = tagDesc.m_pEntity->GetPosition();
		packet.tags[i].pos_x = static_cast<int16_t>(floor(pos.x));
		packet.tags[i].pos_y = static_cast<int16_t>(floor(pos.y));
		packet.tags[i].pos_z = static_cast<int16_t>(floor(pos.z));
		packet.tags[i].alpha = tagDesc.m_nAlpha;
		/*if (packet.tags[i].alpha != 0)
		{
			CChat::AddMessage("%d %d %d %d",
				packet.tags[i].pos_x,
				packet.tags[i].pos_y,
				packet.tags[i].pos_z,
				packet.tags[i].alpha);
		}*/
	}

	GetPacketFactory().Send(packet);
}