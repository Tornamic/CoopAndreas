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
