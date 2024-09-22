#include "sdk.h"
#include <time.h>

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

DWORD CBaseAPI::dwFindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern)
{
	const char* pat = szPattern;
	DWORD firstMatch = NULL;
	for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
			else pat += 2;
		}
		else {
			pat = szPattern;
			firstMatch = 0;
		}
	}
	return NULL;
}
//===================================================================================
HMODULE CBaseAPI::GetModuleHandleSafe( const char* pszModuleName )
{
	HMODULE hmModuleHandle = NULL;

	do
	{
		hmModuleHandle = GetModuleHandle( pszModuleName );
		Sleep( 1 );
	}
	while(hmModuleHandle == NULL);

	return hmModuleHandle;
}
//===================================================================================
void CBaseAPI::LogToFile( const char * pszMessage, ... )
{
	va_list va_alist;
	char szLogbuf[4096];
	FILE * fp;
	struct tm * current_tm;
	time_t current_time;

	time (&current_time);
	current_tm = localtime (&current_time);

	sprintf ( szLogbuf, "[%02d:%02d:%02d] ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec );

	va_start ( va_alist, pszMessage );
	_vsnprintf ( szLogbuf + strlen( szLogbuf ), sizeof( szLogbuf ) - strlen( szLogbuf ), pszMessage, va_alist);
	va_end ( va_alist );
	sprintf ( szLogbuf,  "%s\n", szLogbuf );
	if ( (fp = fopen ( "Darkstorm.log", "a") ) != NULL )
	{
		fprintf ( fp, "%s" , szLogbuf );
		fclose (fp);
	}
#ifdef DEBUG
	::WriteConsole( hDebugConsole, szLogbuf, strlen(szLogbuf), &dwMessage, NULL );
#endif
}
//===================================================================================
DWORD CBaseAPI::GetClientSignature(char* chPattern)
{
	static HMODULE hmModule = GetModuleHandleSafe("client.dll");
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return dwFindPattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, chPattern);
}
//===================================================================================
DWORD CBaseAPI::GetEngineSignature(char* chPattern)
{
	static HMODULE hmModule = GetModuleHandleSafe("engine.dll");
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return dwFindPattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, chPattern);
}
//===================================================================================
void CBaseAPI::ErrorBox ( char* error )
{
	string s = error;
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
	MessageBoxW(NULL, r.c_str(), 0, 0);
}
//===================================================================================
DWORD CBaseAPI::dwGetCallFunctionLocation(DWORD instructionPointer)
{
	DWORD jumpMask = instructionPointer + 1;
	return ((*(PDWORD)(jumpMask)) + jumpMask + 4);
}

CBaseAPI gBaseAPI;