#pragma once

#include "Windows.h"

class CCrashLog
{
public:
	static LONG __stdcall ExceptionHandler(_EXCEPTION_POINTERS* exceptionInfo);
};

