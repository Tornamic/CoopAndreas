#include "stdafx.h"
#include "CNetworkCheckpoint.h"

void CNetworkCheckpoint::Process()
{
	if (ms_bEnabled)
	{
		float fToY = ms_vecRadius.y + ms_vecPosition.y;
		float fToX = ms_vecRadius.x + ms_vecPosition.x;
		float fFromY = ms_vecPosition.y - ms_vecRadius.y;
		float fFromX = ms_vecPosition.x - ms_vecRadius.x;
		CTheScripts::HighlightImportantArea(*(int*)&ms_vecRadius.x, fFromX, fFromY, fToX, fToY, ms_vecPosition.z);
	}
}

void CNetworkCheckpoint::Update(CVector& vecPosition, CVector& vecRadius)
{
	//CChat::AddMessage("CNetworkCheckpoint::Update(CVector(%f, %f, %f), CVector(%f, %f, %f))",
		//vecPosition.x, vecPosition.y, vecPosition.z,
		//vecRadius.x, vecRadius.y, vecRadius.z);
	ms_vecPosition = vecPosition;
	ms_vecRadius = vecRadius;
	ms_bEnabled = true;
}

void CNetworkCheckpoint::Remove()
{
	//CChat::AddMessage("CNetworkCheckpoint::Remove()");
	ms_bEnabled = false;
}
