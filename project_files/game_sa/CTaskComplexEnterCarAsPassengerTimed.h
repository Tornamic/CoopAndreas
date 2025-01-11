#pragma once


class PLUGIN_API CTaskComplexEnterCarAsPassengerTimed : public CTaskComplex
{
public:

	CTaskComplexEnterCarAsPassengerTimed(CVehicle* vehicle, int doorFrameId, int time, bool a5);

	CVehicle* m_pVehicle;
	int m_nMaxTimeInitial;
	int m_nDoorFrameId;
	unsigned char byte18;
	unsigned char gap19[3];
	unsigned int dword1C_6;
	unsigned int m_nTimeWhenStarted;
	unsigned int m_nMaxTime;
	bool m_bTimerStarted;
	bool m_bRestartTimer;
};

VALIDATE_SIZE(CTaskComplexEnterCarAsPassengerTimed, 0x2A);
