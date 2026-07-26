#pragma once

enum eNetworkEntityType : uint8_t;

class CNetworkEntitySerializer
{
public:
	eNetworkEntityType entityType = (eNetworkEntityType)1;
	int entityId = 0;

#ifdef COOP_CLIENT
	CEntity* GetEntity();
	void SetEntity(CEntity* entity);
#endif

	template <typename Stream>
	bool Serialize(Stream& stream)
	{
		serialize_int(stream, (int&)entityType, NETWORK_ENTITY_TYPE_PLAYER, NETWORK_ENTITY_TYPE_NOTINPOOLS);

		int maxValue = 0;
		switch (entityType)
		{
		case NETWORK_ENTITY_TYPE_PLAYER:
			maxValue = Config::MAX_SERVER_PLAYERS;
			break;
		case NETWORK_ENTITY_TYPE_VEHICLE:
			maxValue = Config::MAX_SERVER_VEHICLES;
			break;
		case NETWORK_ENTITY_TYPE_PED:
			maxValue = Config::MAX_SERVER_PEDS;
			break;
		}
		
		if (maxValue == 0)
		{
			return true;
		}

		serialize_int(stream, entityId, 0, maxValue - 1);
		return true;
	}
};

