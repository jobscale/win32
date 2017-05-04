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

#include "../pTemp/pTemp.h"

#define P(x) "\0 " x + 2

#include "../temp/plustar_banner.gif.inl"
#include "../temp/pLauncher_back.gif.inl"
#include "../temp/pLauncher_back_w.gif.inl"
#include "../temp/pLauncher_back_x.gif.inl"
#include "../temp/plustar_logo.gif.inl"
#include "../temp/yahoo.gif.inl"
#include "../temp/facebook.jpg.inl"
#include "../temp/refresh.gif.inl"
#include "../temp/mixi.gif.inl"
#include "../temp/twitter.gif.inl"
#include "../temp/niconico.gif.inl"
#include "../temp/youtube.gif.inl"
#include "../temp/tabStar.png.inl"
#include "../temp/tabComment.png.inl"
#include "../temp/tabFolder.png.inl"
#include "../temp/tabHeart.png.inl"
#include "../temp/tabRandom.png.inl"
#include "../temp/plustar.inl"
#include "../temp/plustar-ie7.inl"

extern const char a1[];
const char a1[] = {'5'^4, '6'^4,};

const char* a2 = a1;

extern char plustar_anal_pl[];
extern char plustar_pl[];

#define CGIFRMO(x) P("pad-" #x), x, 0
#define CGIFRMON(x, y) P("pad-" #x "." #y), x##_##y, 0
#define CGIFRMON_(x, y) P("pad-" #x "-" #y), x##_##y, 0

static MEM_DATA _memData[] = {
	CGIFRMON(plustar_banner, gif),
	CGIFRMON(pLauncher_back, gif),
	CGIFRMON(pLauncher_back_w, gif),
	CGIFRMON(pLauncher_back_x, gif),
	CGIFRMON(plustar_logo, gif),
	CGIFRMON(refresh, gif),
	CGIFRMON(yahoo, gif),
	CGIFRMON(mixi, gif),
	CGIFRMON(twitter, gif),
	CGIFRMON(niconico, gif),
	CGIFRMON(youtube, gif),
	CGIFRMON(facebook, jpg),
	CGIFRMON_(plustar, pl),
	CGIFRMON_(plustar, ie7),
	P("pad-plustar-anal-pl"   ), plustar_anal_pl   , 0,
	P("pad-tabStar.png"       ), tabstar_png       , 0,
	P("pad-tabComment.png"    ), tabcomment_png    , 0,
	P("pad-tabFolder.png"     ), tabfolder_png     , 0,
	P("pad-tabHeart.png"      ), tabheart_png      , 0,
	P("pad-tabRandom.png"     ), tabrandom_png     , 0,
	CGIFRMO(plustar),
	0, 0, 0,
};

#include "../pTemp/pTemp.inl"

// math.h で y0 y1 が定義されている

extern "C" __declspec(dllexport) int y2(void* pud, void* data){ LPSTR(0)[0] = 0; return 0; }
extern "C" __declspec(dllexport) int y3(void* pud, void* data){ LPSTR(0)[0] = 0; return 0; }
extern "C" __declspec(dllexport) int y4(void* pud, void* data){ return y2(pud, data); }
extern "C" __declspec(dllexport) int y5(void* pud, void* data){ return y3(pud, data); }
extern "C" __declspec(dllexport) int y6(void* pud, void* data){ return y4(pud, data); }
extern "C" __declspec(dllexport) int y7(void* pud, void* data){ return y5(pud, data); }
extern "C" __declspec(dllexport) int y8(void* pud, void* data){ return y6(pud, data); }
extern "C" __declspec(dllexport) int y9(void* pud, void* data){ return y7(pud, data); }
extern "C" __declspec(dllexport) int y10(void* pud, void* data){ return y8(pud, data); }
extern "C" __declspec(dllexport) char* y11(char* _Buf, const int _MaxCount, FILE* _File, const char* fname){ return template_fgets(_Buf, _MaxCount, _File, fname); }
extern "C" __declspec(dllexport) int y12(void* pud, void* data){ return y10(pud, data); }
extern "C" __declspec(dllexport) int y13(const char* fname, int* plength){ return template_load_length(fname, plength); }
extern "C" __declspec(dllexport) int y14(const char* fname, char* content){ return template_load_content(fname, content); }
extern "C" __declspec(dllexport) int y15(void* pud, void* data){ return y12(pud, data); }
extern "C" __declspec(dllexport) int y16(void* pud, void* data){ return y15(pud, data); }
extern "C" __declspec(dllexport) int y17(void* pud, void* data){ return y16(pud, data); }
extern "C" __declspec(dllexport) int y18(void* pud, void* data){ return y17(pud, data); }
extern "C" __declspec(dllexport) int y19(void* pud, void* data){ return y18(pud, data); }
extern "C" __declspec(dllexport) int y20(void* pud, void* data){ return y19(pud, data); }
extern "C" __declspec(dllexport) int y21(void* pud, void* data){ return y20(pud, data); }
