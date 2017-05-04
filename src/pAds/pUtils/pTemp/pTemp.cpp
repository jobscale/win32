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

#include "pTemp.h"

#define P(x) "\0 " x + 2

#include "../temp/default.inl"
#include "../temp/pagerror.gif.inl"
#include "../temp/refresh.gif.inl"
#include "../temp/search.gif.inl"
#include "../temp/back.gif.inl"
#include "../temp/close.png.inl"
#include "../temp/blocked.inl"
#include "../temp/cgi-error-404.inl"
#include "../temp/adhoc.inl"
//#include "../temp/cgi-style.css.inl"
#include "../temp/connect-failed.inl"
#include "../temp/connection-timeout.inl"
#include "../temp/forwarding-failed.inl"
#include "../temp/no-server-data.inl"
#include "../temp/no-such-domain.inl"
#include "../temp/plustar-ad-header.inl"
#include "../temp/untrusted.inl"
//#include "../temp/url-info-osd.xml.inl"

extern const char a1[];
const char a1[] = {'5'^4, '6'^4,};

const char* a2 = a1;

extern char plustar_header[];
extern char plustar_anal[];
extern char plustar_adcore[]; //jquery
extern char plustar_adsdk[];
extern char plustar_showads[];
extern char plustar_sdk[];
extern char plustar_adcss[];
extern char plustar_adshow[];
extern char plustar_show[];

#define CGIFRMO(x) P("pad-" #x), x, 0
#define CGIFRMON(x, y) P("pad-" #x "." #y), x##_##y, 0
#define CGIFRMON_(x, y) P("pad-" #x "-" #y), x##_##y, 0
#define CGIFRMON__(x, y, z) P("pad-" #x "-" #y "-" #z), x##_##y##_##z, 0
#define CGIFRMON___(x, y, z) P("pad-" #x "-" #y "." #z), x##_##y##_##z, 0
#define CGIFRMON____(x, y, z, a) P("pad-" #x "-" #y "-" #z "." #a), x##_##y##_##z##_##a, 0

static MEM_DATA _memData[] = {
	CGIFRMON(pagerror, gif),
	CGIFRMON(refresh, gif),
	CGIFRMON(search, gif),
	CGIFRMON(back, gif),
	CGIFRMON(close,png),
	CGIFRMON_(plustar, header),
	CGIFRMON_(plustar, anal),
	CGIFRMON_(plustar, adcore), // jquery.js
	CGIFRMON_(plustar, adsdk),
	CGIFRMON_(plustar, showads),
	CGIFRMON_(plustar, sdk),
	CGIFRMON_(plustar, adshow),
	CGIFRMON_(plustar, show),
	CGIFRMON_(plustar, adcss),
	CGIFRMON__(plustar, ad, header),
	CGIFRMON_(connection, timeout),
	CGIFRMON_(forwarding, failed),
	CGIFRMO(adhoc),
	CGIFRMON__(cgi, error, 404),
	CGIFRMON_(connect, failed),
	CGIFRMON__(no, server, data),
	CGIFRMON__(no, such, domain),
//	CGIFRMON___(cgi, style, css),
//	CGIFRMON____(url, info, osd, xml),
	CGIFRMO(untrusted),
	CGIFRMO(default),
	0, 0, 0
};

#include "pTemp.inl"

extern "C" __declspec(dllexport) int t0(void* pud, void* data){ LPSTR(0)[0] = 0; return 0; }
extern "C" __declspec(dllexport) int t1(void* pud, void* data){ LPSTR(0)[0] = 0; return 0; }
extern "C" __declspec(dllexport) int t2(void* pud, void* data){ return t1(pud, data); }
extern "C" __declspec(dllexport) int t3(void* pud, void* data){ return t2(pud, data); }
extern "C" __declspec(dllexport) int t4(void* pud, void* data){ return t3(pud, data); }
extern "C" __declspec(dllexport) int t5(void* pud, void* data){ return t4(pud, data); }
extern "C" __declspec(dllexport) int t6(void* pud, void* data){ return t5(pud, data); }
extern "C" __declspec(dllexport) int t7(void* pud, void* data){ return t6(pud, data); }
extern "C" __declspec(dllexport) int t8(void* pud, void* data){ return t7(pud, data); }
extern "C" __declspec(dllexport) int t9(void* pud, void* data){ return t8(pud, data); }
extern "C" __declspec(dllexport) int t10(void* pud, void* data){ return t9(pud, data); }
extern "C" __declspec(dllexport) char* t11(char* _Buf, const int _MaxCount, FILE* _File, const char* fname){ return template_fgets(_Buf, _MaxCount, _File, fname); }
extern "C" __declspec(dllexport) int t12(void* pud, void* data){ return t10(pud, data); }
extern "C" __declspec(dllexport) int t13(const char* fname, int* plength){ return template_load_length(fname, plength); }
extern "C" __declspec(dllexport) int t14(const char* fname, char* content){ return template_load_content(fname, content); }
extern "C" __declspec(dllexport) int t15(void* pud, void* data){ return t12(pud, data); }
extern "C" __declspec(dllexport) int t16(void* pud, void* data){ return t15(pud, data); }
extern "C" __declspec(dllexport) int t17(void* pud, void* data){ return t16(pud, data); }
extern "C" __declspec(dllexport) int t18(void* pud, void* data){ return t17(pud, data); }
extern "C" __declspec(dllexport) int t19(void* pud, void* data){ return t18(pud, data); }
extern "C" __declspec(dllexport) int t20(void* pud, void* data){ return t19(pud, data); }
extern "C" __declspec(dllexport) int t21(void* pud, void* data){ return t20(pud, data); }
