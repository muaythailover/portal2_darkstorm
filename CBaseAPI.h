#pragma once

#include "SDK.h"

//Original CBaseAPI class by P47TR!CK

class CBaseAPI
{
public:
	DWORD dwFindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern);
	HMODULE GetModuleHandleSafe( const char* pszModuleName );
	void LogToFile( const char * pszMessage, ... );
	DWORD GetClientSignature ( char* chPattern );
	DWORD GetEngineSignature ( char* chPattern );
	void ErrorBox ( char* error );
	DWORD dwGetCallFunctionLocation(DWORD instructionPointer);
};

extern CBaseAPI gBaseAPI;