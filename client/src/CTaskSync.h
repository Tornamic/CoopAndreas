#pragma once
class CTaskSync
{
public:
	static void* SerializeTask(CTask* t, CNetworkPed* owner, bool bPrimary, unsigned char taskSlot, size_t* dataSize);
	static void DeSerializeTask(void* data);
};

