#include "stdafx.h"
#include "CDiscordRPCMgr.h"
#include <CDiscordRPC.h>
#include <CTheZones.h>
#include <CGangWars.h>
#include <CGame.h>

constexpr size_t RPC_UPDATE_TICKRATE = 15'000;
size_t m_nLastUpdateTickRate;

constexpr int MAX_FREEROAM_STATES = 8;
int m_nLastStateFreeroam = 0;
std::string GetNextState()
{
	CPlayerPed* player = FindPlayerPed(0);
	
	if (!player)
		return "";

	m_nLastStateFreeroam++;
	m_nLastStateFreeroam %= MAX_FREEROAM_STATES;

	switch (m_nLastStateFreeroam)
	{
	case 0:
	{
		int wantedLevel = player->GetWantedLevel();

		if (!wantedLevel)
			return GetNextState();

		std::string result = "Wanted Level: ";

		for (int i = 0; i < wantedLevel; i++)
		{
			result += '\xE2';
			result += '\x98';
			result += '\x85';
		}

		return result;
	}
	case 1:
	{
		int health = (int)(player->m_fHealth / player->m_fMaxHealth * 100);

		std::string result = "Health: " + std::to_string(health) + "%";

		if (player->m_fArmour)
		{
			result += " | Armour: " + std::to_string((int)player->m_fArmour) + "%";
		}

		return result;
	}
	case 2:
	{
		std::string result = std::to_string(player->GetPlayerInfoForThisPlayerPed()->m_nMoney);

		for (int i = result.length() - 3; i > 0; i -= 3) 
			result.insert(i, ",");

		return "Money: $" + result;
	}
	case 3:
	{
		eWeaponType weaponType = player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

		if(weaponType == eWeaponType::WEAPON_UNARMED)
			return GetNextState();

		return "Current Weapon: " + CUtil::GetWeaponName(weaponType);
	}
	case 4:
	{
		CVehicle* vehicle = player->m_pVehicle;

		if(!vehicle || !player->m_nPedFlags.bInVehicle)
			return GetNextState();

		std::string modelName = TheText.Get(reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex])->m_szGameName);
		int speed = (int)(vehicle->m_vecMoveSpeed.Magnitude() * 175);
		return "Vehicle " + modelName + " | Speed: " + std::to_string(speed) + " km/h";
	}
	case 5:
	{
		static const std::string levelName[] =
		{
			"Country Side",
			"Los Santos",
			"San Fierro",
			"Las Venturas"
		};

		return (player->m_nPedFlags.bInVehicle ? ("Driving through ") : ("Walking around ")) + levelName[CTheZones::m_CurrLevel];
	}
	case 6:
	{
		CVector pos = player->GetPosition();

		if (
			pos.z >= 54.0f &&
			pos.x >= -2900.0f && pos.x <= -1980.0f &&
			pos.y >= -2190.0f && pos.y <= -940.0f
			)
		{
			return "Exploring Mount Chiliad";
		}
		return "Location: " + std::string(CHud::m_pLastZoneName);
	}
	case 7:
	{
		int captured = (int)CStats::GetStatValue(STAT_TERRITORIES_TAKEN_OVER);
		if (!captured)
			return GetNextState();

		return "Territory captured: " + std::to_string(captured) + "%";
	}
	}

	return "";
}

class InteriorPlacement2D 
{
public:
	CVector2D m_vecPos;
	uint8_t m_nAreaId;
	float m_fCheckRadius;

	inline bool IsFit(InteriorPlacement2D* ot)
	{
		return
			ot->m_nAreaId == this->m_nAreaId &&
			(ot->m_vecPos - this->m_vecPos).Magnitude() <= this->m_fCheckRadius;
	}
};

bool IsPlacementInRangeOfAnyGym(InteriorPlacement2D placement)
{
	static InteriorPlacement2D gymPos[] =
	{
		{{768.0f, 5.8f}, 5, 100.0f},
		{{774.0f, 47.9f}, 6, 100.0f},
		{{774.2f, 76.0f}, 7, 100.0f},
		{{1234.3f, -763.4f}, 5, 12.5f}, // Madd Dog's house 
	};

	for (uint8_t i = 0; i < sizeof(gymPos) / sizeof(InteriorPlacement2D); i++)
	{
		if (gymPos[i].IsFit(&placement))
		{
			return true;
		}
	}

	return false;
}

bool IsPlacementInRangeOfAnyClothesShop(InteriorPlacement2D placement)
{
	static InteriorPlacement2D clothesPos[] =
	{
		{{225.0f, -9.1f}, 5, 100.0f}, // Victim
		{{204.1f, -46.8f}, 1, 100.0f}, // Sub Urban
		{{161.4f, -94.2f}, 18, 100.0f}, // Zip
		{{204.1f, -165.7f}, 14, 100.0f}, // Didier Sachs
		{{207.2f, -109.7f}, 15, 100.0f}, // Binco
		{{206.4f, -137.7f}, 3, 100.0f}, // Pro-Laps
	};

	for (uint8_t i = 0; i < sizeof(clothesPos) / sizeof(InteriorPlacement2D); i++)
	{
		if (clothesPos[i].IsFit(&placement))
		{
			return true;
		}
	}

	return false;
}

bool IsPlacementInRangeOfAnyAmmunation(InteriorPlacement2D placement)
{
	static InteriorPlacement2D ammuPos[] =
	{
		{{315.2f, -140.8f}, 7, 100.0f}, // Ammu-nation 1
		{{285.8f, -39.0f}, 1, 100.0f}, // Ammu-nation 2
		{{291.7f, -80.1f}, 4, 100.0f}, // Ammu-nation 3
		{{297.1f, -109.8f}, 6, 100.0f}, // Ammu-nation 4
		{{316.5f, -167.6f}, 6, 100.0f} // Ammu-nation 5
	};

	for (uint8_t i = 0; i < sizeof(ammuPos) / sizeof(InteriorPlacement2D); i++)
	{
		if (ammuPos[i].IsFit(&placement))
		{
			return true;
		}
	}

	return false;
}

std::string GetNextDetails()
{
	CPlayerPed* player = FindPlayerPed(0);

	if (!player)
		return "";

	int wantedLevel = player->GetWantedLevel();

	if (wantedLevel)
	{
		if (wantedLevel <= 2)
			return "Breaking the law";

		return "Escaping from the cops";
	}

	// State of defending gang war (i.e. fighting for enemy zone)
	if (CGangWars::State)
	{
		return "Taking over enemy territory";
	}
	// State of attacking gang war (i.e. fighting for own zone)
	else if (CGangWars::State2 == 2) // PLAYER_CAME_TO_WAR
	{
		return "Defending the hood";
	}
	
	CVector pos = player->GetPosition();

	if (
		pos.z < 55.0f &&
		pos.x >= 125.0f && pos.x <= 1040.0f &&
		pos.y >= -2250.0f && pos.y <= -1780.0f
		)
	{
		if ((pos - CVector(662.9f, -1866.2f, 5.4f)).Magnitude() <= 15.0f) // gym on the santa maria beach 
		{
			return "Training on the beach";
		}

		return "Chilling on the beach";
	}

	if (
		pos.z < 40.0f &&
		pos.x >= 2442.0f && pos.x <= 2544.0f &&
		pos.y >= -1723.0f && pos.y <= -1630.0f
		)
	{
		return "Hanging out at Grove Street";
	}

	InteriorPlacement2D currentPlacement{};
	currentPlacement.m_vecPos = pos;
	currentPlacement.m_nAreaId = CGame::currArea;

	if (IsPlacementInRangeOfAnyAmmunation(currentPlacement))
	{
		return "Getting armed at Ammu-Nation";
	}

	if (IsPlacementInRangeOfAnyGym(currentPlacement))
	{
		return "Training at the gym";
	}

	if (IsPlacementInRangeOfAnyClothesShop(currentPlacement))
	{
		return "Shopping for clothes";
	}

	if (player->m_nPedFlags.bInVehicle && player->m_pVehicle)
	{
		CVehicle* vehicle = player->m_pVehicle;
		eVehicleType vehicleType = CUtil::GetVehicleType(vehicle);

		if (vehicle->m_nModelIndex == MODEL_TAXI || vehicle->m_nModelIndex == MODEL_CABBIE)
		{
			return "Playing as a taxi driver";
		}

		if(vehicle->m_nModelIndex >= MODEL_COPCARLA && vehicle->m_nModelIndex <= MODEL_COPCARRU)
		{
			return "Pretends to be a cop";
		}

		if (vehicle->m_nModelIndex == MODEL_AMBULAN)
		{
			return "Racing to save lives";
		}

		if (vehicle->m_nModelIndex == MODEL_FIRETRUK)
		{
			return "Firefighter in action";
		}

		if (vehicleType == eVehicleType::VEHICLE_PLANE)
		{
			return "Flying a plane over San Andreas";
		}

		if (vehicleType == eVehicleType::VEHICLE_HELI)
		{
			return "Flying a heli over San Andreas";
		}

		if (
			(vehicleType == eVehicleType::VEHICLE_AUTOMOBILE || vehicleType == eVehicleType::VEHICLE_BIKE) && 
			vehicle->m_nCurrentGear >= vehicle->m_pHandlingData->m_transmissionData.m_nNumberOfGears)
		{
			return "On a high-speed chase";
		}

		if (vehicleType == eVehicleType::VEHICLE_AUTOMOBILE)
		{
			return "Cruising through the streets";
		}
		
		if (vehicleType == eVehicleType::VEHICLE_BIKE)
		{
			return "Burning rubber on a bike";
		}

		if (vehicleType == eVehicleType::VEHICLE_MTRUCK)
		{
			return "Rolling on big wheels";
		}

		if (vehicleType == eVehicleType::VEHICLE_BOAT)
		{
			return "Cruising on a boat";
		}

		if (vehicleType == eVehicleType::VEHICLE_BMX)
		{
			return "Pedaling through the streets";
		}

		if (vehicleType == eVehicleType::VEHICLE_QUAD)
		{
			return "Riding off-road on a quad";
		}

		if (vehicleType == eVehicleType::VEHICLE_TRAIN)
		{
			return "Riding the rails";
		}

		if (vehicleType == eVehicleType::VEHICLE_TRAILER)
		{
			return "Kidnapped and thrown in the trailer";
		}
	}
	else
	{
		if (player->m_pIntelligence->GetUsingParachute())
		{
			return "Skydiving through the sky";
		}

		// I hope it will determine every weapon
		eCamMode camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
		switch (camMode)
		{
		case MODE_AIMING:
		case MODE_AIMWEAPON:
		case MODE_AIMWEAPON_ATTACHED:
		case MODE_AIMWEAPON_FROMCAR:
		case MODE_M16_1STPERSON:
		case MODE_SNIPER_RUNABOUT:
		case MODE_ROCKETLAUNCHER_RUNABOUT:
		case MODE_SNIPER:
		case MODE_ROCKETLAUNCHER:
		case MODE_ROCKETLAUNCHER_HS:
		case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
			return "Aiming at the target";
		}

		if (auto task = player->m_pIntelligence->GetTaskInAir())
		{
			if (player->m_ePedState != PEDSTATE_ABSEIL_FROM_HELI
				&& player->m_vecMoveSpeed.z * 50.0 <= -20.0)
			{
				return "Hates gravity";
			}
		}

		if (player->m_pIntelligence->GetTaskSwim())
		{
			return "Swimming like a fish";
		}

		switch (player->m_nMoveState)
		{
		case PEDMOVE_STILL:
			return "Idle, waiting in the city";
		case PEDMOVE_WALK:
			return "Walking through the city";
		case PEDMOVE_JOG:
			return "Jogging through the city";
		case PEDMOVE_RUN:
			return "Running through the city";
		case PEDMOVE_SPRINT:
			return "Sprinting through the city";
		}
	}
	
	return "None";
}

void CDiscordRPCMgr::Update()
{
	size_t tickCount = GetTickCount();

	if (tickCount < m_nLastUpdateTickRate + RPC_UPDATE_TICKRATE)
		return;

	m_nLastUpdateTickRate = tickCount;

	std::string details = GetNextDetails();
	std::string state = GetNextState();
	CDiscordRPC::SetDetailsAndState(details, state);
}