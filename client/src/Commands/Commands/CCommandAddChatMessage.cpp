#include "stdafx.h"
#include "CCommandAddChatMessage.h"

void CCommandAddChatMessage::Process(CRunningScript* script)
{
	char text[128];
	script->ReadTextLabelFromScript(text, 128);
	text[127] = '\0';

	CChat::AddMessage(text);
}