#pragma once
class CChat
{
public:
	static std::vector<std::string> CChat::m_aMessages;
	static void CChat::AddMessage(const std::string& message);
	static void Draw();
};

