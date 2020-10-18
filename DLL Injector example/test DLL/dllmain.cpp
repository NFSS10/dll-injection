#include "pch.h"
#include <Windows.h>


void runStuff() {
	MessageBox(NULL, L"Hello! I was injected in another process :)", L"Success!", NULL);
	Sleep(1000);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		runStuff();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}