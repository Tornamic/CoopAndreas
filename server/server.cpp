#include <iostream>
#include <enet/enet.h>
#include <algorithm>
#include <cstring>
#include <string>

#include "core/CControllerState.h"
#include "core/CNetwork.h"
#include "core/CPackets.h"
#include "core/CPed.h"
#include "core/CPedManager.h"
#include "core/CPlayer.h"
#include "core/CPlayerManager.h"
#include "core/CVector.h"
#include "core/CVehicle.h"
#include "core/CVehicleManager.h"
#include "core/eNetworkEntityType.h"
#include "core/VehicleDoorState.h"



#include "src/CNetwork.cpp"
#include "src/CPed.cpp"
#include "src/CPedManager.cpp"
#include "src/CPlayer.cpp"
#include "src/CPlayerManager.cpp"
#include "src/CVehicle.cpp"
#include "src/CVehicleManager.cpp"


int main(int argc, char **argv)
{
    CNetwork::Init(6767);
    return 0;
}
