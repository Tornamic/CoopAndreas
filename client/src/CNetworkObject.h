#pragma once
class CNetworkObject
{
private:
    CNetworkObject() {}

public:
    objectid_t m_objectId{};
    CObject* m_pObject = nullptr;
    int m_nModelId = 0;
    bool m_bSyncing = false;

    CVector m_vecLastPos{};
    CVector m_vecLastRight{};
    CVector m_vecLastUp{};
    CVector m_vecLastMoveSpeed{};
    CVector m_vecLastTurnSpeed{};
    CPhysical* m_pLastAttachedTo = nullptr;
    CVector m_vecLastAttachOffset{};
    CVector m_vecLastAttachTilt{};
    CQuaternion m_lastAttachRotation{};
    uint32_t m_nLastUpdateTime = 0;

    inline static bool ms_bApplyingNetworkDamage = false;

    ~CNetworkObject();
    CNetworkObject(const Packets::Objects::ObjectSpawn& packet);
    bool CreateObject(const Packets::Objects::ObjectSpawn& packet);
    bool HasChangedSinceLastUpdate();
    void StoreLastUpdate();
    void SendDamageIfChanged(const Packets::Objects::SObjectDamageState& oldState, float damage, CVector* colPos,
        CVector* colNormal, CEntity* damager, eWeaponType weapon);
    static CNetworkObject* CreateHosted(CObject* object);
    static void CallObjectDamage(CObject* object, float damage, CVector* colPos, CVector* colNormal, CEntity* damager,
        eWeaponType weapon, bool bIgnoreOnlyDamagedByPlayer);
    static void SetMatrix(CObject* object, const CVector& pos, const CVector& right, const CVector& up);
    static void GetAttachState(CObject* object, Packets::Objects::SObjectAttachState& state);
    static bool ApplyAttachState(CObject* object, const Packets::Objects::SObjectAttachState& state);
    static bool IsAttachedToNetworkEntity(CObject* object);
    static void GetDamageState(CObject* object, Packets::Objects::SObjectDamageState& state);
    static void ApplyDamageState(CObject* object, const Packets::Objects::SObjectDamageState& state);
};
