// pSpice.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define EXPAPI __declspec(dllexport)

extern "C"
{

	HANDLE EXPAPI cs_Create()
	{
		return NULL;
	}

	void EXPAPI cs_Release(HANDLE hCS)
	{
	}

}
