#pragma once
class CNetworkCheckpoint
{
public:
	static inline bool ms_bEnabled{};
	static inline CVector ms_vecPosition{};
	static inline CVector ms_vecRadius{};

	static void Process();
	static void Update(CVector& vecPosition, CVector& vecRadius);
	static void Remove();
};

