#pragma once
class CDriveBy
{
public:
	static void Process(CPlayerPed* pPlayerPed);
	static bool IsPedInDriveby(CPed* ped);
	static CTask* StartDriveby(CPed* ped);
	static void StopDriveby(CPed* ped);
	static void InitHooks();
};

