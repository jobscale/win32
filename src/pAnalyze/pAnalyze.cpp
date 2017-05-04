// a_dll.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include <atlcomtime.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#include "pAnalyze.h"

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

HANDLE EXPAPI ca_Create()
{
	return (HANDLE)new CAnalyze;
}

void EXPAPI ca_Release(HANDLE hCA)
{
	delete (CAnalyze*)hCA;
}

size_t EXPAPI ca_Length(HANDLE mod, BSTR str)
{
	return ((CAnalyze*)mod)->Length(str);
}

LRESULT EXPAPI Interface(DWORD type, DWORD argc, LPCTSTR argv)
{
	switch(type)
	{
	case 0:
		return (LRESULT)new CAnalyze;
	case 1:
		delete (CAnalyze*)argv;
		return 0;
	case 2:
		return (LRESULT)((CAnalyze*)argc)->Length((BSTR)argv);
	}

	return 0;
}

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CAnalyze::CAnalyze()
{
}

CAnalyze::~CAnalyze()
{
}

size_t CAnalyze::Length(BSTR str)
{
	return wcslen(str);
}
