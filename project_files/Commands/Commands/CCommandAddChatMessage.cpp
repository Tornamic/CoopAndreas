#include "stdafx.h"
#include "CCommandAddChatMessage.h"

void CCommandAddChatMessage::Process(CRunningScript* script)
{
	char text[8];
	script->ReadTextLabelFromScript(text, 8);
	text[7] = '\0';

	CChat::AddMessage(text);
}
