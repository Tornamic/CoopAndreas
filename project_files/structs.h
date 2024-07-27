#pragma once
#include "stdafx.h"

#pragma pack(1)
typedef struct _CAMERA_AIM
{
	CVector Front;
	CVector Source;
	CVector SourceBeforeLookBehind;
	CVector Up;

} CAMERA_AIM;