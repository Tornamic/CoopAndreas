#include "scripting.h"
#include <Windows.h>

#define OPCODE extern "C" int __declspec(dllexport)

GAME_SCRIPT_THREAD* gst;
FARPROC ProcessOneCommand = (FARPROC)0x469EB0;
BYTE ScriptBuf[255];
DWORD* pdwParamVars[18];

DWORD dwScmOpcodeDebug = 0;
BOOL bScmLocalDebug = FALSE;
BOOL bExceptionDisplayed = FALSE;

int ExecuteScriptBuf()
{
	__asm
	{

		mov eax, OFFSET ScriptBuf	// Move our script buffer base into eax.

		mov ecx, gst				// Move GAME_SCRIPT_THREAD structure into ecx.
			mov[ecx + 0x14], eax			// Move eax into the gst->dwScriptIP.

			call ProcessOneCommand		// Call the game's script opcode processor.
	}
	return gst->dwIfFlag;
}

extern "C" int __declspec(dllexport) ScriptCommand(WORD opcode, const char args[], ...)
{
	va_list ap;
	const char* p = args;	// Get parameter string.
	va_start(ap, args);			// Initialize varargs.
	memcpy(&ScriptBuf, &opcode, 2);	// Copy opcode to script buf.
	int buf_pos = 2;	// Position in buffer after opcode.
	WORD var_pos = 0;	// Init var_pos.

	// (aru) Reset all vars before proceeding (To fix some nasty 0x00465CB4 crashes)
	// May cause problems with functions that use local vars returned from previous calls

	for (int i = 0; i < 18; i++)
		gst->dwLocalVar[i] = 0;

	dwScmOpcodeDebug = opcode;

	while (*p)			// While we've not ran out of parameters...
	{
		switch (*p)		// Switch current parameter.
		{
		case 'i':	// If integer...
		{
			int i = va_arg(ap, int);			// Grab an int off the stack.
			ScriptBuf[buf_pos] = 0x01;			// Variable data type = 0x01 (4b int).
			buf_pos++;;							// Increment buffer position.
			memcpy(&ScriptBuf[buf_pos], &i, 4);	// Insert the int.
			buf_pos += 4;						// Increment buffer by 4b.
			break;
		}
		case 'f':	// If float...
		{
			/*	Take note, MSVC++ puts a double on the stack
				even if you typecase as a float. <3 Microsoft.	*/
			float f = (float)va_arg(ap, double);	// Get float off the stack.
			ScriptBuf[buf_pos] = 0x06;				// Variable data type = 0x06 (float).
			buf_pos++;								// Increment buffer position.
			memcpy(&ScriptBuf[buf_pos], &f, 4);		// Copy float into script buf.
			buf_pos += 4;							// Increment buffer by 4b.
			break;
		}
		case 'v':	// If variable...
		{
			DWORD* v = va_arg(ap, DWORD*);				// Get the pointer to the passed variable.
			ScriptBuf[buf_pos] = 0x03;					// Variable data type = 0x03 (Local Var).
			buf_pos++;									// Increment buffer position.
			pdwParamVars[var_pos] = v;					// Save pointer to passed variable.
			gst->dwLocalVar[var_pos] = *v;				// Put value of passed variable into local.
			memcpy(&ScriptBuf[buf_pos], &var_pos, 2);	// Copy the offset into the script.
			buf_pos += 2;								// Increment buffer by 2b.
			var_pos++;									// Increment the variable position.
			break;
		}
		case 's':	// If string... Updated 13th Jan 06.. (kyeman) SA string support
		{
			char* sz = va_arg(ap, char*);
			unsigned char aLen = strlen(sz);
			ScriptBuf[buf_pos] = 0x0E;
			buf_pos++;
			ScriptBuf[buf_pos] = aLen;
			buf_pos++;
			memcpy(&ScriptBuf[buf_pos], sz, aLen);
			buf_pos += aLen;
			break;
		}
		case 'z':	// If the params need zero-terminating...
		{
			ScriptBuf[buf_pos] = 0x00;
			buf_pos++;
			break;
		}
		}
		++p;		// Next parameter
	}
	va_end(ap);		// End varargs.

	// Execute script stub.
	
	int result = 0;
	
	try { // REENABLE BEFORE RELEASE!!
		result = ExecuteScriptBuf();
		if (var_pos)	// if we've used a variable...
		{
			for (int i = 0; i < var_pos; i++)		// For every passed variable...
			{
				*pdwParamVars[i] = gst->dwLocalVar[i];	// Retrieve variable from local var.
			}
		}
	}
	catch (...) {
		if (!bExceptionDisplayed) {

			bExceptionDisplayed = TRUE;
		}
	}
	
	return result;
}
void InitScripting()
{
	gst = new GAME_SCRIPT_THREAD;
	ZeroMemory(gst, sizeof(GAME_SCRIPT_THREAD));
}

OPCODE opcode_create_actor(int pedType, int model, float x, float y, float z, DWORD &id)
{
	return ScriptCommand(0x009A, "iifffv", pedType, model, x, y, z, &id);
}

OPCODE opcode_get_actor_pos(DWORD id, float &x, float &y, float &z)
{
	return ScriptCommand(0x00A0, "ivvv", id, &x, &y, &z);
}

OPCODE opcode_get_actor_ptr(DWORD id, DWORD& ptr)
{
	return ScriptCommand(0x0A96, "iv", id, &ptr);
}
OPCODE opcode_create_player(DWORD &iPlayerNum, float fX, float fY, float fZ, DWORD &dwPlayerActorID)
{
	return ScriptCommand(0x0053, "vfffv", &iPlayerNum, fX, fY, fZ, &dwPlayerActorID);
}
OPCODE opcode_create_actor_from_player(DWORD &iPlayerNum, DWORD &dwPlayerActorID)
{
	return ScriptCommand(0x01F5, "vv", &iPlayerNum, &dwPlayerActorID);
}