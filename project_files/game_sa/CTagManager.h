#pragma once

struct tTagDesc 
{
    CEntity* m_pEntity;
    uint8_t m_nAlpha;
    char _pad[3];
};

class CTagManager
{
public:
	static inline tTagDesc(&ms_tagDesc)[150] = *(tTagDesc(*)[150])0xA9A8C0;
    static inline int32_t& ms_numTags = *(int32_t*)0xA9AD70;
    static inline int32_t& ms_numTagged = *(int32_t*)0xA9AD74;

    static void SetAlpha(CEntity* entity, uint8_t ucAlpha);
    static bool IsTag(const CEntity* entity);
    static CVector* GetTagPos(int32_t i);
};

