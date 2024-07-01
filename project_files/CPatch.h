#pragma once
class CPatch
{
public:
	static void ApplyPatches();
private:
	static void PatchPlayers();
	static void PatchPools();
	static void FixCrashes();
	static void PatchStreaming();
};

