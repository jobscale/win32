///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#if !defined(__OFOUNDATIONCLASSLIBRARY_H__)
#define __OFOUNDATIONCLASSLIBRARY_H__

#pragma warning(disable : 4996)

#include <atlbase.h>
#include <afxtempl.h>

#undef OFCL_API
#ifdef _OFC_EXPORT_
#define OFCL_API __declspec(dllexport)
#else
#define OFCL_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// CoRect

class OFCL_API CoRect : public CRect
{
public:
	static CoRect* Cast(CRect* pRect);
	operator CoRect*();
	CPoint TopRight();
	CPoint BottomLeft();

};

/////////////////////////////////////////////////////////////////////////////
// CoRgn

class OFCL_API CoRgn : public CRgn
{
public:
	static CoRgn* Cast(CRgn* pRgn);
	operator CoRgn*();

};

/////////////////////////////////////////////////////////////////////////////
// CoBitmap

class OFCL_API CoBitmap : public CBitmap
{
public:
	static CoBitmap* Cast(CBitmap* pBitmap);
	operator CoBitmap*();
	HBITMAP Detach();
	BOOL LoadImage(LPCTSTR lpszBitmap, UINT fuLoad = LR_DEFAULTCOLOR);
	CoBitmap& operator=(CoBitmap& other);
	BOOL MakeRgn(CoRgn& rgn);
	template<class T>
	BOOL _makeRgn(CoRgn& rgn, T* pT);

};

/////////////////////////////////////////////////////////////////////////////
// CoBrush

class OFCL_API CoBrush : public CBrush
{
public:
	static CoBrush* Cast(CBrush* pBrush);
	BOOL LoadImage(LPCTSTR lpszBitmap, UINT fuLoad = LR_DEFAULTCOLOR);
	operator CoBrush*();
	operator CoBitmap*();

};

/////////////////////////////////////////////////////////////////////////////
// CoMenu

struct MENUPOS
{
	CMenu* pMenu;
	UINT nPos;
	CMenu* pCurrentMenu;
	UINT nCurrentPos;
	MENUPOS(CMenu* _pMenu = NULL, UINT _nPos = 0, CMenu* _pCurrentMenu = NULL, UINT _nCurrentPos = 0)
		: pMenu(_pMenu)
		, nPos(_nPos)
		, pCurrentMenu(_pCurrentMenu)
		, nCurrentPos(_nCurrentPos)
	{
	}
};

class OFCL_API CoMenu : public CMenu
{
public:
	static CoMenu* Cast(CMenu* pMenu);
	operator CoMenu*();
	BOOL EnableMenuItemParent(UINT nIDItem, UINT nEnable);
	BOOL DeleteMenuParent(UINT nIDItem);
	BOOL FindItemParent(UINT nIDItem, MENUPOS& result);
};

/////////////////////////////////////////////////////////////////////////////
// CoFont

#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#endif

class OFCL_API CoFont : public CFont
{
public:
	static CoFont* Cast(CFont* pFont);
	operator CoFont*();
	BOOL CreateArealFont(BYTE lfItalic = 0, BYTE lfQuality = DEFAULT_QUALITY, LONG lfHeight = 0, BYTE charset = -1);
	BOOL CreateSystemFont(BYTE lfItalic = 0, BYTE lfQuality = DEFAULT_QUALITY, LONG lfHeight = 0, BYTE charset = -1);
	BOOL CreateDefaultFont(BYTE lfItalic = 0, BYTE lfQuality = DEFAULT_QUALITY, LONG lfHeight = 0, BYTE charset = -1);
	BOOL CreateCopyFont(CoFont* pFont, BYTE lfItalic = 0, BYTE lfQuality = DEFAULT_QUALITY, LONG lfHeight = 0, BYTE charset = -1);
};

/////////////////////////////////////////////////////////////////////////////
// CoGuiFont
class OFCL_API CoGuiFont
{
protected:
	CDC* m_pDC;
	CFont* m_pFont;

public:
	CoGuiFont(CDC* pDC);
	virtual ~CoGuiFont();
};

/////////////////////////////////////////////////////////////////////////////
// CoBitmapDC

enum BDCF_ENUM
{
	BDCF_TRANSPARENT,
	BDCF_PAINT
};

class OFCL_API CoBitmapDC : public CDC
{
protected:
	CBitmap* m_pBitmap;
	CoBitmap m_bitmap;
	BITMAP m_bm;

public:
	CoBitmapDC(CDC* pDC = NULL, CoBitmap* pBitmap = NULL);
	virtual ~CoBitmapDC();
	operator CoBitmapDC*();
	BOOL CreateLoadImage(CDC* pDC, LPCTSTR lpszPath);
	BOOL CreateIndirect(CDC* pDC, CoBitmap* pBitmap);
	BOOL CreateBitmapDC(CDC* pDC, CPoint point, CSize size);
	CoBitmap& GetBitmap();
	BOOL DeleteDC();
	INT Width();
	INT Height();
	INT GetBitCount();
	CSize Draw(CoBitmap& bitmap, CPoint point = CPoint(0, 0), BDCF_ENUM type = BDCF_TRANSPARENT);
	INT StretchDIBits(CoBitmap* pBitmap, CRect rect);

};

/////////////////////////////////////////////////////////////////////////////
// CoSpecialBitmap

class OFCL_API CoSpecialBitmap : public CoBitmap
{
protected:
	CArray<CRect, CRect&> m_rect;
public:
	CoSpecialBitmap();
	virtual ~CoSpecialBitmap();
	INT AddSpecial(CoBitmap& bitmap, BOOL bCheck = TRUE);
	BOOL CreateSpecial(INT nIndex, CoBitmap& bitmap);
	virtual BOOL DeleteObject();
};

/////////////////////////////////////////////////////////////////////////////
// CoDialog

class OFCL_API CoDialog : public CDialog
{
public:
	static BOOL OnInitDialogMultiLanguage(CWnd* pWnd);

};

/////////////////////////////////////////////////////////////////////////////
// CoUser32

#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif

#ifndef AW_HOR_POSITIVE
enum ANIMATION_ENUM
{
	AW_HOR_POSITIVE = 0x00000001,
	AW_HOR_NEGATIVE = 0x00000002,
	AW_VER_POSITIVE = 0x00000004,
	AW_VER_NEGATIVE = 0x00000008,
	AW_CENTER = 0x00000010,
	AW_HIDE = 0x00010000,
	AW_ACTIVATE = 0x00020000,
	AW_SLIDE = 0x00040000,
	AW_BLEND = 0x00080000,
};
#else
typedef DWORD ANIMATION_ENUM;
#endif

class OFCL_API CoUser32
{
protected:
	HMODULE m_module;

public:
	CoUser32();
	virtual ~CoUser32();

	typedef DWORD (WINAPI* SetLayeredWindowAttributesFunc)(HWND hWnd, DWORD crKey, BYTE bAlpha, DWORD dwFlags);
	SetLayeredWindowAttributesFunc SetLayeredWindowAttributes;

	typedef BOOL (WINAPI* AnimateWindowFunc)(HWND hWnd, DWORD dwTime, DWORD dwFlags);
	AnimateWindowFunc AnimateWindow;

};

/////////////////////////////////////////////////////////////////////////////
// Function

void OFCL_API SetDCBrushOrg(CDC* pDC, CWnd* pWnd = NULL);
OFCL_API CFile& operator<<(CFile& f, const LPCTSTR lpszText);
OFCL_API CFile& operator<<(CFile& f, const INT value);
OFCL_API CFile& operator<<(CFile& f, const LONG value);
OFCL_API CFile& operator<<(CFile& f, const DOUBLE value);
OFCL_API CFile& operator<<(CFile& f, const DWORD value);

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__OFOUNDATIONCLASSLIBRARY_H__)
