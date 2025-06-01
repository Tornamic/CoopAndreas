#pragma once

#include "Windows.h"

class CCrashLog
{
public:
	static inline char ms_szCrashMessage[32768];
	static inline _EXCEPTION_POINTERS* ms_pExceptionPointers;
	static inline tm ms_exceptionTime;
	static inline bool ms_bSuccessSavedLog = false;
	static inline LPTOP_LEVEL_EXCEPTION_FILTER ms_lpPreviousFilter;
	static LONG __stdcall ExceptionHandler(_EXCEPTION_POINTERS* exceptionInfo);
};

