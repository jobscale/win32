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

#define _OFC_EXPORT_
#include "oFoundationClassLibrary.h"
#include "oString.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
// CoRect

CoRect* CoRect::Cast(CRect* pRect)
{
	return (CoRect*)pRect;
}

CoRect::operator CoRect*()
{
	return this;
}

CPoint CoRect::TopRight()
{
	return CPoint(top, right);
}

CPoint CoRect::BottomLeft()
{
	return CPoint(bottom, left);
}

/////////////////////////////////////////////////////////////////////////////
// CoRgn

CoRgn* CoRgn::Cast(CRgn* pRgn)
{
	return (CoRgn*)pRgn;
}

CoRgn::operator CoRgn*()
{
	return this;
}

/////////////////////////////////////////////////////////////////////////////
// CoBitmap

CoBitmap* CoBitmap::Cast(CBitmap* pBitmap)
{
	return (CoBitmap*)pBitmap;
}

CoBitmap::operator CoBitmap*()
{
	return this;
}

HBITMAP CoBitmap::Detach()
{
	return (HBITMAP)CBitmap::Detach();
}

#include <direct.h>

// 2004.4.19 WINAPI の LoadImage は WindowsXP Hyper Threading で動作異常あり
// ファイルからイメージの取得
HBITMAP OFCL_API oLoadImage(LPCTSTR lpszBitmap, UINT fuLoad = LR_DEFAULTCOLOR)
{
	// ファイルのオープン
	CFile f;
	if (!f.Open(lpszBitmap, CFile::modeRead | CFile::shareDenyNone))
	{
		// 存在しないファイルを意図的にロードれさる場合がある
		return NULL;
	}

	// ファイルのリード
	INT length = (INT)f.GetLength();
	CoString data;
	if (f.Read(data.GetBufferSetLength(length), length) != (DWORD)length)
	{
		ASSERT(FALSE);
		return NULL;
	}

	// BITMAPFILEHEADER 構造体の取得
	LPBITMAPFILEHEADER pBfh = (LPBITMAPFILEHEADER)(LPBYTE)data.GetBuffer(0);
	// BITMAPINFO 構造体の取得
	LPBITMAPINFO pBi = (LPBITMAPINFO)((LPBYTE)pBfh + sizeof BITMAPFILEHEADER);
	// データ
	LPVOID pData = ((LPBYTE)pBfh + pBfh->bfOffBits);

	// メモリの取得
	CoBitmapDC bitmap;
	if (!bitmap.CreateBitmapDC(NULL, CPoint(0, 0), CSize(pBi->bmiHeader.biWidth, pBi->bmiHeader.biHeight)))
	{
		ASSERT(FALSE);
		return NULL;
	}

	// 画像のコピー
	INT result = SetDIBitsToDevice(bitmap, 0, 0, pBi->bmiHeader.biWidth, pBi->bmiHeader.biHeight, 0, 0, 0, pBi->bmiHeader.biHeight, pData, pBi, DIB_RGB_COLORS);
	// WindowsME の一部の環境で戻り値が常に 1 であることを確認している
	// MSDN には「関数が成功すると、設定された走査行の数が返ります」と記述があるが必ずしもそうではない
	if (result == 0 || result == GDI_ERROR)
	{
		ASSERT(FALSE);
		return NULL;
	}

	// 加工処理
	if (fuLoad & LR_LOADTRANSPARENT)
	{
		CoBitmapDC transparentBitmap;
		if (!transparentBitmap.CreateBitmapDC(&bitmap, CPoint(0, 0), CSize(bitmap.Width(), bitmap.Height())))
		{
			ASSERT(FALSE);
			return NULL;
		}
		transparentBitmap.FillSolidRect(0, 0, bitmap.Width(), bitmap.Height(), GetSysColor(COLOR_WINDOW));
		transparentBitmap.Draw(bitmap.GetBitmap());
		return transparentBitmap.GetBitmap().Detach();
	}

	return bitmap.GetBitmap().Detach();
}

// ファイルからイメージの取得
BOOL CoBitmap::LoadImage(LPCTSTR lpszBitmap, UINT fuLoad/* = LR_DEFAULTCOLOR*/)
{
	if (!lpszBitmap)
	{
		return FALSE;
	}
	return Attach(oLoadImage(lpszBitmap, fuLoad));
}

// イメージのコピー
CoBitmap& CoBitmap::operator=(CoBitmap& other)
{
	if (!other.GetSafeHandle())
	{
		return *this;
	}

	BITMAP bm;
	VERIFY(other.GetBitmap(&bm) == sizeof BITMAP);
	DeleteObject();
	VERIFY(CreateBitmapIndirect(&bm));

	DWORD size = bm.bmWidthBytes * bm.bmHeight;
	CoString buffer;
	LPVOID pBits = buffer.GetBuffer(size);
	VERIFY(other.GetBitmapBits(size, pBits) == size);
	VERIFY(SetBitmapBits(size, pBits) == size);

	return *this;
}

// リージョンの作成
BOOL CoBitmap::MakeRgn(CoRgn& rgn)
{
	BITMAP bm;
	VERIFY(GetBitmap(&bm) == sizeof BITMAP);
	if (bm.bmBitsPixel == 32)
	{
		_makeRgn(rgn, (LPRGBQUAD)NULL);
	}
	else if (bm.bmBitsPixel == 24)
	{
		_makeRgn(rgn, (RGBTRIPLE*)NULL);
	}
	else if (bm.bmBitsPixel == 16)
	{
		_makeRgn(rgn, (LPWORD)NULL);
	}
	return FALSE;
}

// リージョンの作成
template<class T>
BOOL CoBitmap::_makeRgn(CoRgn& rgn, T* pT)
{
	ASSERT(!rgn.GetSafeHandle());

	VERIFY(rgn.CreateRectRgnIndirect(CRect(CPoint(0, 0), CSize(0, 0))));

	BITMAP bm;
	VERIFY(GetBitmap(&bm) == sizeof BITMAP);

	RGBQUAD first;
	*LPDWORD(&first) = -1;
	DWORD count = bm.bmWidthBytes * bm.bmHeight;
	CoString buffer;
	T* pBits = (T*)buffer.GetBuffer(count);
	VERIFY(GetBitmapBits(count, pBits) == count);
	T* pWorkBits = pBits;
	for (INT y = 0; y < bm.bmHeight; y++)
	{
		pWorkBits = (T*)(LPBYTE(pBits) + bm.bmWidthBytes * y);
		for (INT x = 0; x < bm.bmWidth; x++)
		{
			if (sizeof pWorkBits[x] == 4 || sizeof pWorkBits[x] == 3)
			{
				RGBTRIPLE* pTriple = (RGBTRIPLE*)&pWorkBits[x];
				RGBQUAD q = {
					pTriple->rgbtBlue,
					pTriple->rgbtGreen,
					pTriple->rgbtRed,
					0
				};
				if (*LPDWORD(&first) == -1)
				{
					first = q;
				}
				if (*LPDWORD(&first) != *LPDWORD(&q))
				{
					CoRgn pointRgn;
					VERIFY(pointRgn.CreateRectRgnIndirect(CRect(CPoint(x, y), CSize(1, 1))));
					rgn.CombineRgn(&rgn, &pointRgn, RGN_OR);
				}
			}
			else if (sizeof pWorkBits[x] == 2)
			{
				// 5-6-5 16-bit image, where the blue mask is 0x001F, the green mask is 0x07E0, and the red mask is 0xF800
				// 5-5-1-5 16-bit image, where the blue mask is 0x001F, the green mask is 0x07C0, and the red mask is 0xF800
				LPWORD pTriple = (LPWORD)&pWorkBits[x];
				INT b = (*pTriple & 0x001F);
				INT g = (*pTriple & 0x07C0) >> 6;
				INT r = (*pTriple & 0xF800) >> 11;

				RGBQUAD q = {
					b,
					g,
					r,
					0
				};
				if (*LPDWORD(&first) == -1)
				{
					first = q;
				}
				if (*LPDWORD(&first) != *LPDWORD(&q))
				{
					CoRgn pointRgn;
					VERIFY(pointRgn.CreateRectRgnIndirect(CRect(CPoint(x, y), CSize(1, 1))));
					rgn.CombineRgn(&rgn, &pointRgn, RGN_OR);
				}
			}
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CoMenu

// 型変換
CoMenu* CoMenu::Cast(CMenu* pMenu)
{
	return (CoMenu*)pMenu;
}

// 型変換
CoMenu::operator CoMenu*()
{
	return this;
}

// 親アイテムの有効設定
BOOL CoMenu::EnableMenuItemParent(UINT nIDItem, UINT nEnable)
{
	ASSERT(this);

	MENUPOS pos;
	if (!FindItemParent(nIDItem, pos))
	{
		return FALSE;
	}
	pos.pMenu->EnableMenuItem(pos.nPos, nEnable | MF_BYPOSITION);

	return TRUE;
}

// 親アイテムの削除
BOOL CoMenu::DeleteMenuParent(UINT nIDItem)
{
	ASSERT(this);

	MENUPOS pos;
	if (!FindItemParent(nIDItem, pos))
	{
		return FALSE;
	}
	pos.pMenu->DeleteMenu(pos.nPos, MF_BYPOSITION);

	return TRUE;
}

// 親アイテムの検索
BOOL CoMenu::FindItemParent(UINT nIDItem, MENUPOS& result)
{
	ASSERT(this);

	UINT count = GetMenuItemCount();
	for (UINT i = 0; i < count; i++)
	{
		UINT nID = GetMenuItemID(i);
		if (nID == 0)
		{
		}
		else if (nID == -1)
		{
			result.pMenu = this;
			result.nPos = i;
			if (((CoMenu*)GetSubMenu(i))->FindItemParent(nIDItem, result))
			{
				return TRUE;
			}
		}
		else if (nID == nIDItem)
		{
			result.pCurrentMenu = this;
			result.nCurrentPos = i;
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CoFont

// 型変換
CoFont* CoFont::Cast(CFont* pFont)
{
	return (CoFont*)pFont;
}

// 型変換
CoFont::operator CoFont*()
{
	return this;
}

// 共通フォントの作成
BOOL CoFont::CreateArealFont(BYTE lfItalic/* = 0*/, BYTE lfQuality/* = 0*/, LONG lfHeight/* = 0*/, BYTE charset/* = -1*/)
{
	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof LOGFONT, &lf, 0);

	strcpy(lf.lfFaceName, "Areal");
	lf.lfItalic = lfItalic;
	lf.lfQuality = lfQuality;
	lf.lfHeight -= lfHeight;
	if (charset != OEM_CHARSET) lf.lfCharSet = charset;
	return CreateFontIndirect(&lf);
}

// システムフォントの作成
BOOL CoFont::CreateSystemFont(BYTE lfItalic/* = 0*/, BYTE lfQuality/* = 0*/, LONG lfHeight/* = 0*/, BYTE charset/* = -1*/)
{
	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof LOGFONT, &lf, 0);

	lf.lfItalic = lfItalic;
	lf.lfQuality = lfQuality;
	lf.lfHeight -= lfHeight;
	if (charset != OEM_CHARSET) lf.lfCharSet = charset;
	return CreateFontIndirect(&lf);
}

// 標準フォントの作成
BOOL CoFont::CreateDefaultFont(BYTE lfItalic/* = 0*/, BYTE lfQuality/* = 0*/, LONG lfHeight/* = 0*/, BYTE charset/* = -1*/)
{
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	CFont* pFont = CoFont::FromHandle(hFont);
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	lf.lfItalic = lfItalic;
	lf.lfQuality = lfQuality;
	lf.lfHeight -= lfHeight;
	if (charset != OEM_CHARSET) lf.lfCharSet = charset;
	return CreateFontIndirect(&lf);
}

// コピーフォントの作成
BOOL CoFont::CreateCopyFont(CoFont* pFont, BYTE lfItalic/* = 0*/, BYTE lfQuality/* = 0*/, LONG lfHeight/* = 0*/, BYTE charset/* = -1*/)
{
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	lf.lfItalic = lfItalic;
	lf.lfQuality = lfQuality;
	lf.lfHeight -= lfHeight;
	if (charset != OEM_CHARSET) lf.lfCharSet = charset;
	return CreateFontIndirect(&lf);
}

/////////////////////////////////////////////////////////////////////////////
// CoGuiFont

CoGuiFont::CoGuiFont(CDC* pDC)
	: m_pDC(pDC)
{
	m_pFont = m_pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
}

CoGuiFont::~CoGuiFont()
{
	m_pDC->SelectObject(m_pFont);
}

/////////////////////////////////////////////////////////////////////////////
// CoBrush

// 型変換
CoBrush* CoBrush::Cast(CBrush* pBrush)
{
	return (CoBrush*)pBrush;
}

// 型変換
CoBrush::operator CoBrush*()
{
	return this;
}

// 型変換
CoBrush::operator CoBitmap*()
{
	LOGBRUSH lb;
	GetLogBrush(&lb);
	return (CoBitmap*)CoBitmap::FromHandle((HBITMAP)lb.lbHatch);
}

// ファイルからイメージの取得
BOOL CoBrush::LoadImage(LPCTSTR lpszBitmap, UINT fuLoad/* = LR_DEFAULTCOLOR*/)
{
	LOGBRUSH lb;
	lb.lbStyle = BS_PATTERN;
	lb.lbColor = RGB(0xFF, 0xFF, 0xFF);
	lb.lbHatch = (ULONG_PTR)oLoadImage(lpszBitmap, fuLoad);
	return CreateBrushIndirect(&lb);
}

/////////////////////////////////////////////////////////////////////////////
// CoBitmapDC

CoBitmapDC::CoBitmapDC(CDC* pDC/* = NULL*/, CoBitmap* pBitmap/* = NULL*/)
	: m_pBitmap(NULL)
{
	ZeroMemory(&m_bm, sizeof BITMAP);

	if (pBitmap)
	{
		VERIFY(CreateIndirect(pDC, pBitmap));
	}
}

CoBitmapDC::~CoBitmapDC()
{
	DeleteDC();
}

CoBitmapDC::operator CoBitmapDC*()
{
	return this;
}

BOOL CoBitmapDC::CreateLoadImage(CDC* pDC, LPCTSTR lpszPath)
{
	DeleteDC();

	if (!CreateCompatibleDC(pDC))
	{
		return FALSE;
	}

	if (!m_bitmap.LoadImage(lpszPath))
	{
		return FALSE;
	}

	VERIFY(m_bitmap.GetBitmap(&m_bm) == sizeof BITMAP);

	m_pBitmap = SelectObject(&m_bitmap);

	return TRUE;
}

BOOL CoBitmapDC::CreateIndirect(CDC* pDC, CoBitmap* pBitmap)
{
	DeleteDC();

	if (!CreateCompatibleDC(pDC))
	{
		return FALSE;
	}

	VERIFY(pBitmap->GetBitmap(&m_bm) == sizeof BITMAP);

	if (!m_bitmap.CreateCompatibleBitmap(pDC, m_bm.bmWidth, m_bm.bmHeight))
	{
		return FALSE;
	}

	m_pBitmap = SelectObject(&m_bitmap);

	DWORD size = m_bm.bmWidthBytes * m_bm.bmHeight;
	CoString buffer;
	LPBYTE pBits = (LPBYTE)buffer.GetBuffer(size);
	VERIFY(pBitmap->GetBitmapBits(size, pBits) == size);
	VERIFY(m_bitmap.SetBitmapBits(size, pBits) == size);

	return TRUE;
}

BOOL CoBitmapDC::CreateBitmapDC(CDC* pDC, CPoint point, CSize size)
{
	DeleteDC();

	CDC dc;
	if (!pDC)
	{
		pDC = &dc;
		VERIFY(pDC->CreateDC(_T("DISPLAY"), NULL, NULL, NULL));
	}

	if (!CreateCompatibleDC(pDC))
	{
		return FALSE;
	}

	if (!m_bitmap.CreateCompatibleBitmap(pDC, size.cx, size.cy))
	{
		return FALSE;
	}

	VERIFY(m_bitmap.GetBitmap(&m_bm) == sizeof BITMAP);

	m_pBitmap = SelectObject(&m_bitmap);

	if (!BitBlt(0, 0, size.cx, size.cy, pDC, point.x, point.y, SRCCOPY))
	{
		return FALSE;
	}

	return TRUE;
}

CoBitmap& CoBitmapDC::GetBitmap()
{
	return m_bitmap;
}

BOOL CoBitmapDC::DeleteDC()
{
	if (GetSafeHdc())
	{
		SelectObject(m_pBitmap);
		m_pBitmap = NULL;
	}
	if (m_bitmap.GetSafeHandle())
	{
		m_bitmap.DeleteObject();
	}
	return CDC::DeleteDC();
}

INT CoBitmapDC::Width()
{
	return m_bm.bmWidth;
}

INT CoBitmapDC::Height()
{
	return m_bm.bmHeight;
}

INT CoBitmapDC::GetBitCount()
{
	return m_bm.bmBitsPixel;
}

CSize CoBitmapDC::Draw(CoBitmap& bitmap, CPoint point/* = CPoint(0, 0)*/, BDCF_ENUM type/* = BDCF_TRANSPARENT*/)
{
	ASSERT(GetSafeHdc());
	// ASSERT(m_bitmap.GetSafeHandle());

	if (type == BDCF_TRANSPARENT)
	{
		CoRgn rgn;
		bitmap.MakeRgn(rgn);
		rgn.OffsetRgn(point);
		SelectClipRgn(&rgn);
	}

	CoBitmapDC memDC(this, &bitmap);
	VERIFY(BitBlt(point.x, point.y, memDC.Width(), memDC.Height(), &memDC, 0, 0, SRCCOPY));

	if (type == BDCF_TRANSPARENT)
	{
		SelectClipRgn(NULL, RGN_COPY);
	}

	return CSize(memDC.Width(), memDC.Height());
}

INT CoBitmapDC::StretchDIBits(CoBitmap* pBitmap, CRect rect)
{
	BITMAP bm;
	pBitmap->GetBitmap(&bm);
	BITMAPINFO bmi = { sizeof BITMAPINFO };
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = bm.bmBitsPixel;
	bmi.bmiHeader.biCompression = BI_RGB;

	DWORD size = bm.bmWidthBytes * bm.bmHeight;
	CoString buffer;
	LPBYTE pBits = (LPBYTE)buffer.GetBuffer(size);
	VERIFY(pBitmap->GetBitmapBits(size, pBits) == size);
	SetStretchBltMode(COLORONCOLOR);
	return ::StretchDIBits(GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), 0, 0, bm.bmWidth, bm.bmHeight, pBits, &bmi, DIB_RGB_COLORS, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
// CoSpecialBitmap

CoSpecialBitmap::CoSpecialBitmap()
{
}

CoSpecialBitmap::~CoSpecialBitmap()
{
}

// 多層画像の追加
INT CoSpecialBitmap::AddSpecial(CoBitmap& bitmap, BOOL bCheck/* = TRUE*/)
{
	if (!bitmap.GetSafeHandle())
	{
		return (INT)m_rect.Add(CRect(0, 0, 0, 0));
	}

	BITMAP bm;
	VERIFY(bitmap.GetBitmap(&bm) == sizeof BITMAP);
	BITMAP beforeBM = { 0 };
	CoBitmap beforeBitmap;
	if (GetSafeHandle())
	{
		beforeBitmap.Attach(Detach());
		VERIFY(beforeBitmap.GetBitmap(&beforeBM) == sizeof BITMAP);
	}
	else
	{
		ASSERT(beforeBM.bmWidth != 1);
	}
	BITMAP afterBM = beforeBM;
	if (!afterBM.bmWidth)
	{
		afterBM.bmWidth = bm.bmWidth; // 幅の決定
	}
	afterBM.bmHeight += bm.bmHeight; // 高さを加算
	if (bCheck)
	{
		ASSERT(afterBM.bmWidth == bm.bmWidth); // 幅が違う
	}
	if (afterBM.bmWidth < bm.bmWidth)
	{
		afterBM.bmWidth = bm.bmWidth; // 幅が広くなる
	}

	CoBitmapDC dummyDC;
	VERIFY(dummyDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL));

	CoBitmapDC specialDC;
	VERIFY(specialDC.CreateCompatibleDC(dummyDC));
	VERIFY(CreateCompatibleBitmap(dummyDC, afterBM.bmWidth, afterBM.bmHeight));
	CBitmap* pSpecialBitmap = specialDC.SelectObject(this);
	ASSERT(pSpecialBitmap);

	CoBitmapDC dc;
	VERIFY(dc.CreateCompatibleDC(dummyDC));
	if (beforeBM.bmHeight)
	{
		CBitmap* pBitmap = dc.SelectObject(&beforeBitmap);
		ASSERT(pBitmap);
		VERIFY(specialDC.BitBlt(0, 0, beforeBM.bmWidth, beforeBM.bmHeight, dc, 0, 0, SRCCOPY));
		dc.SelectObject(pBitmap);
	}
	CBitmap* pBitmap = dc.SelectObject(&bitmap);
	ASSERT(pBitmap);
	VERIFY(specialDC.BitBlt(0, beforeBM.bmHeight, bm.bmWidth, bm.bmHeight, dc, 0, 0, SRCCOPY));
	dc.SelectObject(pBitmap);

	specialDC.SelectObject(pSpecialBitmap);

	CRect rect(CPoint(0, beforeBM.bmHeight), CSize(bm.bmWidth, bm.bmHeight));
	return (INT)m_rect.Add(rect);
}

// 多層画像の取得
BOOL CoSpecialBitmap::CreateSpecial(INT nIndex, CoBitmap& bitmap)
{
	ASSERT(GetSafeHandle());

	if (nIndex >= m_rect.GetSize())
	{
		return FALSE;
	}
	CRect rect = m_rect[nIndex];

	if (rect.IsRectEmpty())
	{
		return FALSE;
	}

	CoBitmapDC dummyDC;
	VERIFY(dummyDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL));

	CoBitmapDC specialDC;
	VERIFY(specialDC.CreateCompatibleDC(dummyDC));
	CBitmap* pSpecialBitmap = specialDC.SelectObject(this);
	ASSERT(pSpecialBitmap);

	CoBitmapDC dc;
	VERIFY(dc.CreateCompatibleDC(dummyDC));
	VERIFY(bitmap.CreateCompatibleBitmap(dummyDC, rect.Width(), rect.Height()));
	CBitmap* pBitmap = dc.SelectObject(&bitmap);
	ASSERT(pBitmap);
	VERIFY(dc.BitBlt(0, 0, rect.Width(), rect.Height(), specialDC, 0, rect.top, SRCCOPY));
	dc.SelectObject(pBitmap);

	specialDC.SelectObject(pSpecialBitmap);

	return TRUE;
}

// オブジェクトの開放
BOOL CoSpecialBitmap::DeleteObject()
{
	m_rect.RemoveAll();
	return CoBitmap::DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CoDialog

/*static */BOOL CoDialog::OnInitDialogMultiLanguage(CWnd* pWnd)
{
	// マルチランゲージ対応（ダイアログフォントの設定）
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	CFont* pFont = CFont::FromHandle(hFont);
	if (!pFont)
	{
		return FALSE;
	}
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	// 日本語、ANSI
	if (lf.lfCharSet == SHIFTJIS_CHARSET || lf.lfCharSet == ANSI_CHARSET)
	{
	}

	// 中国、韓国、西ヨーロッパ
	else if (lf.lfCharSet == CHINESEBIG5_CHARSET || lf.lfCharSet == GB2312_CHARSET ||
		lf.lfCharSet == HANGUL_CHARSET || lf.lfCharSet == JOHAB_CHARSET)
	{
		// フォントサイズの調整
		static CoFont font;
		CFont* pArialFont = pWnd->GetFont();
		if (!font.GetSafeHandle() && pArialFont)
		{
			pArialFont->GetLogFont(&lf);
			lf.lfHeight += 2;
			VERIFY(font.CreateFontIndirect(&lf));
		}

		pWnd->SendMessageToDescendants(WM_SETFONT, (WPARAM)font.GetSafeHandle(), MAKELPARAM(TRUE, 0));

		for (CWnd* pChild = pWnd->GetWindow(GW_CHILD); pChild; pChild = pChild->GetWindow(GW_HWNDNEXT))
		{
			CoString className;
			GetClassName(pChild->GetSafeHwnd(), className.GetBuffer(_MAX_PATH), _MAX_PATH);
			className.ReleaseBuffer();
			if (!className.Compare(WC_LISTVIEW) || !className.Compare(WC_TREEVIEW) || !className.Compare(_T("EDIT")) || !className.Compare(_T("SysMonthCal32")))
			{
				pChild->SetFont(pFont);
			}
		}
	}

	// デフォルトフォントのセット
	else
	{
		pWnd->SendMessageToDescendants(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CoUser32

CoUser32::CoUser32()
	: m_module(LoadLibrary(PP(user32.dll)))
{
	SetLayeredWindowAttributes = (SetLayeredWindowAttributesFunc)GetProcAddress(m_module, P("SetLayeredWindowAttributes"));
	AnimateWindow = (AnimateWindowFunc)GetProcAddress(m_module, P("AnimateWindow"));
}

CoUser32::~CoUser32()
{
	FreeLibrary(m_module);
}

/////////////////////////////////////////////////////////////////////////////
// Function

// ブラシ原点の設定
void OFCL_API SetDCBrushOrg(CDC* pDC, CWnd* pWnd/* = NULL*/)
{
	CWnd* pDCWnd = (pWnd) ? pWnd : pDC->GetWindow();
	CWnd* pBaseWnd = AfxGetMainWnd();
	if (!pBaseWnd) pBaseWnd = CWnd::GetDesktopWindow();
	CRect rect;
	pDCWnd->GetClientRect(rect);
	pDCWnd->ClientToScreen(rect);
	pBaseWnd->ScreenToClient(rect);
	pDC->SetBrushOrg(CPoint(-rect.left, -rect.top));
}

OFCL_API CFile& operator<<(CFile& f, LPCTSTR lpszText)
{
	f.Write(lpszText, (UINT)strlen(lpszText));

	return f;
}

OFCL_API CFile& operator<<(CFile& f, const INT value)
{
	CoString text;
	text.Format("%d", value);
	f.Write(text, text.GetLength());

	return f;
}

OFCL_API CFile& operator<<(CFile& f, const LONG value)
{
	CoString text;
	text.Format("%d", value);
	f.Write(text, text.GetLength());

	return f;
}

OFCL_API CFile& operator<<(CFile& f, const DOUBLE value)
{
	CoString text;
	text.Format("%f", value);
	f.Write(text, text.GetLength());

	return f;
}

OFCL_API CFile& operator<<(CFile& f, const DWORD value)
{
	CoString text;
	text.Format("0x%08X", value);
	f.Write(text, text.GetLength());

	return f;
}

/////////////////////////////////////////////////////////////////////////////
