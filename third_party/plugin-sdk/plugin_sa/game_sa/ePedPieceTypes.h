/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

enum ePedPieceTypes {
	PED_COL_SPHERE_LEG,
	PED_COL_SPHERE_MID,
	PED_COL_SPHERE_HEAD,
	PED_SPHERE_CHEST,
	PED_SPHERE_MIDSECTION,
	PED_SPHERE_UPPERARM_L,
	PED_SPHERE_UPPERARM_R,
	PED_SPHERE_LEG_L,
	PED_SPHERE_LEG_R,
	PED_SPHERE_HEAD
};

extern inline const char* ePedPieceTypesToString(ePedPieceTypes type)
{
	static const char* str[] = {
		"PED_COL_SPHERE_LEG",
		"PED_COL_SPHERE_MID",
		"PED_COL_SPHERE_HEAD",
		"PED_SPHERE_CHEST",
		"PED_SPHERE_MIDSECTION",
		"PED_SPHERE_UPPERARM_L",
		"PED_SPHERE_UPPERARM_R",
		"PED_SPHERE_LEG_L",
		"PED_SPHERE_LEG_R",
		"PED_SPHERE_HEAD"
	};
	if (type < PED_COL_SPHERE_LEG || type > PED_SPHERE_HEAD)
	{
		return "(null)";
	}
	return str[type];
}