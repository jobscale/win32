////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. プラスター
//
// ┌──┐┌┐    ┌┐┌┐┌──┐┌──┐┌──┐┌──┐
// └─┐│││    │││││┌─┘└┐┌┘│┌┐││┌┐│
// ┌─┘│││    │││││└─┐  ││  │└┘││└┴┘
// │┌─┘││    ││││└─┐│  ││  │┌┐││┌┬┐
// ││    │└─┐│└┘│┌─┘│  ││  ││││││││
// └┘    └──┘└──┘└──┘  └┘  └┘└┘└┘└┘
//
// Powered by jobscale
// Since 22 July 1973
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <wininet.h>
#pragma comment(lib, "wininet")

#define P(x) "\0 " x + 2

#include "../conf/config-pl.inl"
#include "../conf/match-all-pl.action.inl"
#include "../conf/default-pl.action.inl"
#include "../conf/default-pl.filter.inl"

#include "../../../oCommon/oTools.h"
#include "../../../oCommon/oSocketTools.h"
#include "../../../oCommon/oAtlHttpSocket.h"
#include "../../../oCommon/oCodeConv.h"

struct MEM_DATA
{
	const char* name;
	const char* org;
	char* pos;
	int length;
	int line;
};

static MEM_DATA _memData[] = {
	P("pad-config-pl.txt"), config_txt, config_txt, strlen(config_txt), 0,
	P(".\\pad-match-all-pl.action"), match_all_action, match_all_action, strlen(match_all_action), 0,
	P(".\\pad-default-pl.action"), default_action, default_action, strlen(default_action), 0,
	P(".\\pad-default-pl.filter"), default_filter, default_filter, strlen(default_filter), 0,
	0, 0, 0, 0, 0
};

int MemInitialize()
{
	for (int i = 0; _memData[i].name; i++)
	{
		char* p = (char*)_memData[i].org;
		int j = 0;
		for (; memcmp(&p[j], P("a-IR.o_F#ey,OU"), 14) != 0; j++)
		{
			p[j]^=87;
		}
		p[j] = 0;
	}

	return 0;
}

int read_file_line(FILE *fp, char **dest, int *newline, const int _MaxCount, const char* fname)
{
	// 0=OK, 3=EndOfFile

	for (int i = 0; _memData[i].name; i++)
	{
		if (strcmp(fname, _memData[i].name) == 0)
		{
			if (_memData[i].length == 0)
			{
				_memData[i].pos = (char*)_memData[i].org;
				_memData[i].length = strlen(_memData[i].pos);
				_memData[i].line = 0;

				return 3;
			}

			_memData[i].line++;
			*newline = _memData[i].line;

			const char* eol = strstr(_memData[i].pos, "\n");
			int line_length = eol ? (eol - _memData[i].pos) : _memData[i].length;

			if (line_length >= _MaxCount)
			{
				line_length = _MaxCount - 1;
			}

			memcpy((*dest), _memData[i].pos, line_length);
			(*dest)[line_length] = NULL;

			if (_memData[i].length > line_length)
			{
				_memData[i].pos += line_length + 1;
				_memData[i].length -= line_length;
			}
			else
			{
				_memData[i].pos = 0;
				_memData[i].length = 0;
			}

			return 0;
		}
	}

	return 3;
}

namespace initialize_this_one
{
	static int _memInitialize = MemInitialize();
}

int utl_initialize()
{
	LPCTSTR path = GetModuleFolder();
	SetCurrentDirectory(path);

#if 0
	InitializeInternet();
#endif

	return 0;
}

extern "C" __declspec(dllexport) int u0(const struct putil_data* pud, void* data){ LPSTR(0)[0] = 0; return 0; }
extern "C" __declspec(dllexport) int u1(const struct putil_data* pud, void* data){ LPSTR(0)[0] = 0; return 0; }
extern "C" __declspec(dllexport) int u2(const struct putil_data* pud, void* data){ return utl_initialize(); }
extern "C" __declspec(dllexport) int u3(const struct putil_data* pud, void* data){ return u2(pud, data); }
extern "C" __declspec(dllexport) int u4(const struct putil_data* pud, void* data){ return u3(pud, data); }
extern "C" __declspec(dllexport) int u5(const struct putil_data* pud, void* data){ return u4(pud, data); }
extern "C" __declspec(dllexport) int u6(const struct putil_data* pud, void* data){ return u5(pud, data); }
extern "C" __declspec(dllexport) int u7(const struct putil_data* pud, void* data){ return u6(pud, data); }
extern "C" __declspec(dllexport) int u8(){ return 0; }
extern "C" __declspec(dllexport) int u9(const struct putil_data* pud, void* data){ return u7(pud, data); }
extern "C" __declspec(dllexport) int u10(const struct putil_data* pud, void* data){ return 0; }
extern "C" __declspec(dllexport) int u11(FILE *fp, char **dest, int *newline, const int _MaxCount, const char* fname){ return read_file_line(fp, dest, newline, _MaxCount, fname); }
extern "C" __declspec(dllexport) int u12(const struct putil_data* pud, void* data){ return 0; }
extern "C" __declspec(dllexport) int u13(const struct putil_data* pud, void* data){ return u1(pud, data); }
extern "C" __declspec(dllexport) int u14(const struct putil_data* pud, void* data){ return u13(pud, data); }
extern "C" __declspec(dllexport) int u15(const struct putil_data* pud, void* data){ return u14(pud, data); }
extern "C" __declspec(dllexport) int u16(const struct putil_data* pud, void* data){ return u15(pud, data); }
extern "C" __declspec(dllexport) int u17(const struct putil_data* pud, void* data){ return u16(pud, data); }
extern "C" __declspec(dllexport) int u18(const struct putil_data* pud, void* data){ return u17(pud, data); }
extern "C" __declspec(dllexport) int u19(const struct putil_data* pud, void* data){ return u18(pud, data); }
extern "C" __declspec(dllexport) int u20(const struct putil_data* pud, void* data){ return u19(pud, data); }
extern "C" __declspec(dllexport) int u21(const struct putil_data* pud, void* data){ return u20(pud, data); }
