#include "network/packets/world.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CWeatherSync.h>
#include <game_sa/CTagManager.h>

PACKET_HANDLER(ePacketType::GAME_WEATHER_TIME, Packets::World::GameWeatherTime* pGameWeatherTime)
{
	CWeatherSync::HandlePacket(pGameWeatherTime);
}

PACKET_HANDLER(ePacketType::ADD_EXPLOSION, Packets::World::AddExplosion* pAddExplosion)
{
	//unused for now
	//CExplosion::AddExplosion(nullptr, nullptr, pAddExplosion->type, pAddExplosion->pos, pAddExplosion->time, pAddExplosion->usesSound, pAddExplosion->cameraShake, pAddExplosion->isVisible);
}

PACKET_HANDLER(ePacketType::TAG_UPDATE, Packets::World::TagUpdate* pTagUpdate)
{
    for (auto& tagDesc : CTagManager::ms_tagDesc)
    {
		if (!tagDesc.m_pEntity)
		{
			continue;
		}

		auto& pos = tagDesc.m_pEntity->GetPosition();

		if (static_cast<int16_t>(floor(pos.x)) == pTagUpdate->payload.pos_x &&
			static_cast<int16_t>(floor(pos.y)) == pTagUpdate->payload.pos_y &&
			static_cast<int16_t>(floor(pos.z)) == pTagUpdate->payload.pos_z)
		{
			CTagManager::SetAlpha(tagDesc.m_pEntity, pTagUpdate->payload.alpha);
		}
	}
}

PACKET_HANDLER(ePacketType::UPDATE_ALL_TAGS, Packets::World::UpdateAllTags* pUpdateAllTags)
{
	// a hack to not draw "TAGS SPRAYED 1 of 100", look at 0x49CF4B
	bool saved = TheCamera.m_bWideScreenOn;
	TheCamera.m_bWideScreenOn = true;
	for (auto& tag : pUpdateAllTags->tags)
	{
		for (auto& tagDesc : CTagManager::ms_tagDesc)
		{
			if (!tagDesc.m_pEntity)
			{
				continue;
			}

			auto& pos = tagDesc.m_pEntity->GetPosition();

			if (static_cast<int16_t>(floor(pos.x)) == tag.pos_x &&
				static_cast<int16_t>(floor(pos.y)) == tag.pos_y &&
				static_cast<int16_t>(floor(pos.z)) == tag.pos_z)
			{
				CTagManager::SetAlpha(tagDesc.m_pEntity, tag.alpha);
			}
		}
	}
	TheCamera.m_bWideScreenOn = saved;
}