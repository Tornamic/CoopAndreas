#pragma once
class CNetworkAnimQueue
{
private:
	static inline std::vector<std::vector<uint8_t>> ms_vQueue;

public:
	static void AddOpCode(const uint8_t* buffer, int bufferSize);
	static void Process();
};

