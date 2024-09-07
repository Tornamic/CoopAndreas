#pragma once
#include "stdafx.h"

#pragma pack(1)
typedef struct _CPassengerAim
{
	CVector Front;
	CVector Source;
	CVector SourceBeforeLookBehind;
	CVector Up;

} CPassengerAim;