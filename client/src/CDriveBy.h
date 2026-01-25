#pragma once
class CDriveBy
{
public:
	static void Process(CPlayerPed* player);
	static bool IsPedInDriveby(CPed* ped);
	static void StartDriveby(CPed* ped);
	static void StopDriveby(CPed* ped);
};

