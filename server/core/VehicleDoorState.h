#if !defined(_COOPSERVER_VEHICLE_VEHICLEDOORSTATE_H_) || !defined(_COOPSERVER_CVEHICLEDOORSTATE_H_)
#define _COOPSERVER_CVEHICLEDOORSTATE_H_
#define _COOPSERVER_VEHICLE_VEHICLEDOORSTATE_H_

struct VehicleDoorState
{
	unsigned char bonnet = 0;
	unsigned char boot = 0;
	unsigned char leftFront = 0;
	unsigned char rightFront = 0;
	unsigned char leftRear = 0;
	unsigned char rightRear = 0;
};
#endif