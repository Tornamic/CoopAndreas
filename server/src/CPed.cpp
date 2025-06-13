#include "../core/CPed.h"

CPed::CPed(int pedid, CPlayer* syncer, short modelId, unsigned char pedType, CVector pos, unsigned char createdBy)
{
    m_nPedId = pedid;
    m_pSyncer = syncer;
    m_nModelId = modelId;
    m_nPedType = pedType;
    m_vecPos = pos;
    m_nCreatedBy = createdBy;
}
