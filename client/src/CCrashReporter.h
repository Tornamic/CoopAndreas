#pragma once
#include <thread>

class CCrashReporter
{
public:
	static inline bool ms_bReportingEnabled = true;
	static inline std::atomic<bool> ms_bReady = false;
	static inline std::string ms_sAddress = "";
	static inline std::thread ms_postThread;

	static void FetchIP();
	static void PostCrashLog(std::string& content);
};