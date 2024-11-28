#pragma once
class WorldHooks
{
public: 
	static void InjectHooks();

	static std::unordered_map<CVehicle*, std::vector<void*>> m_aVehicleStackMap;
};

