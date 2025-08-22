#pragma once
enum eNetworkEntityType : uint8_t;

class CNetworkEntitySerializer
{
public:
	eNetworkEntityType entityType;
	int entityId;
	CEntity* GetEntity();
	void SetEntity(CEntity* entity);
};

