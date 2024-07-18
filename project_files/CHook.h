#pragma once
class CHook
{
public:
	static void Init();
};

void __fastcall CTaskSimpleUseGun__SetPedPosition_Hook(CTaskSimpleUseGun* This, CPed* ped);
