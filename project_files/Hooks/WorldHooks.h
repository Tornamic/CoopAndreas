#pragma once
class WorldHooks
{
public: 
	static void InjectHooks();

	static std::unordered_map<CPed*, std::vector<void*>> m_aPedStackMap;
};

