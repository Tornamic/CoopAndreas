#pragma once

#ifndef _CPEDMANAGER_H_
	#define _CPEDMANAGER_H_
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include "CPed.h"

class CPedManager
{
public:
	static std::vector<CPed*> m_pPeds;
	static void Add(CPed* ped);
	static void Remove(CPed* ped);
	static CPed* GetPed(int pedid);
	static int GetFreeId();
	static void RemoveAllHostedAndNotify(CPlayer* player);

	static inline char ms_aszAllowedSpecialActors[52][8] =
	{
		"ANDRE", "BBTHIN", "BB","CAT","CESAR","COPGRL1","COPGRL2","COPGRL3",
		"CLAUDE","CROGRL1","CROGRL2","CROGRL3","DWAYNE","EMMET","FORELLI","GANGRL1",
		"GANGRL2","GANGRL3","GUNGRL1","GUNGRL2","GUNGRL3", "HERN","JANITOR","JETHRO",
		"JIZZY","KENDL","MACCER","MADDOGG","MECGRL1","MECGRL2","MECGRL3","NURGRL1",
		"NURGRL2","NURGRL3","OGLOC","PAUL","PULASKI","ROSE","RYDER1","RYDER2",
		"RYDER3","SINDACO","SMOKE","SMOKEV","SUZIE","SWEET","TBONE","TENPEN",
		"TORINO","TRUTH","WUZIMU","ZERO"
	};
};

#endif