#pragma once

#include "network/ids.h"
#include "eNetworkEntityType.h"

class CNetworkEntitySerializer
{
public:
	eNetworkEntityType entityType = (eNetworkEntityType)1;

	// only the one matching `entityType` is used, may be invalid if the entity has no network instance
	playerid_t playerid{};
	vehicleid_t vehicleid{};
	pedid_t pedid{};
	objectid_t objectid{};

#ifdef COOP_CLIENT
	CEntity* GetEntity();
	void SetEntity(CEntity* entity);
#endif

	template <typename Stream>
	bool Serialize(Stream& stream)
	{
		serialize_int(stream, (int&)entityType, NETWORK_ENTITY_TYPE_PLAYER, NETWORK_ENTITY_TYPE_NOTINPOOLS);

		switch (entityType)
		{
		case NETWORK_ENTITY_TYPE_PLAYER:
			serialize_playerid(stream, playerid);
			break;
		case NETWORK_ENTITY_TYPE_VEHICLE:
			serialize_vehicleid(stream, vehicleid);
			break;
		case NETWORK_ENTITY_TYPE_PED:
			serialize_pedid(stream, pedid);
			break;
		case NETWORK_ENTITY_TYPE_OBJECT:
			serialize_objectid(stream, objectid);
			break;
		}

		return true;
	}
};
