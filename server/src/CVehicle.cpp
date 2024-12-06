
#include "../core/CVehicle.h"

CVehicle::CVehicle(int vehicleid, unsigned short model, CVector pos, float rot)
{
	m_nVehicleId = vehicleid;
	m_nModelId = model;
	m_vecPosition = pos;
	m_vecRotation = CVector( 0, 0, 0 );
}