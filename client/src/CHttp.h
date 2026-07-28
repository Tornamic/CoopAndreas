#pragma once

class CHttp
{
public:
	// blocks the thread!
	static std::string Get(const wchar_t* host, const wchar_t* path); 
	// blocks the thread!
	static std::string Post(const wchar_t* host, const wchar_t* path, const std::string& data); 
};