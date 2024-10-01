#pragma once
class CTaskSync
{
public:
	static void* SerializeTask(CTask* t, CNetworkPed* owner, bool bPrimary, size_t* dataSize);
	static void DeSerializeTask(void* data);
};

