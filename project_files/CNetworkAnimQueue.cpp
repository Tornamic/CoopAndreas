#include "stdafx.h"
#include "CNetworkAnimQueue.h"
#include <COpCodeSync.h>
#include <CTaskSequenceSync.h>

void CNetworkAnimQueue::AddOpCode(const uint8_t* buffer, int bufferSize)
{
	//CChat::AddMessage("AddOpCode %x %d", buffer, bufferSize);

	if (buffer && bufferSize > 0)
		ms_vQueue.emplace_back(buffer, buffer + bufferSize);
}

void CNetworkAnimQueue::Process()
{
	bool processedAnim = false;
	for (auto v : ms_vQueue)
	{
		processedAnim = true;
		COpCodeSync::HandlePacket(v.data(), v.size());
		//CChat::AddMessage("Process %x %d", v.data(), v.size());
	}
	if (processedAnim)
	{
		ms_vQueue.clear();
		ms_vQueue.shrink_to_fit();
	}

	//

	bool processedSequence = false;
	for (auto v : CTaskSequenceSync::ms_vTaskSequences)
	{
		processedSequence = true;
		CTaskSequenceSync::HandlePacket(v.data(), v.size());
	}
	if (processedSequence)
	{
		CTaskSequenceSync::ms_vTaskSequences.clear();
		CTaskSequenceSync::ms_vTaskSequences.shrink_to_fit();
	}
}
