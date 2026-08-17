#pragma once
class CPassengerEnter
{
public:
	static void Process();
	static bool HasFoundNearbyVehiclePassengerDoor();
	static void UpdatePassengerDoorHint();
private:
	inline static bool bDisplayNearPassegnerDoorMessage = false;
};

