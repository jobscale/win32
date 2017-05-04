///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//
// $Date: $
// $Rev: $
// $Author: $
// $HeadURL: $
//
// $Id: $
//

// pLicMakerDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "time.h"
#include "pLicMaker.h"
#include "pLicMakerDlg.h"

#include "../oCommon/oEthernet.h"
#include "../oCommon/oBase64.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CpLicMakerDlg ダイアログ

CpLicMakerDlg::CpLicMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpLicMakerDlg::IDD, pParent)
{
	m_hIcon = (HICON)0;
}

void CpLicMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CpLicMakerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CpLicMakerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CpLicMakerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CpLicMakerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CpLicMakerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CpLicMakerDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CpLicMakerDlg メッセージ ハンドラ

BOOL CpLicMakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	{
		int err = 0;

		// ステータスバー初期化
		if (!err) if (!m_statusbar.Create(this)) err = 1;
		if (!err) RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		//m_statusbar.SetPaneText(0, _T("ステータスバー"),1);
	}

	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO1);
	pCmb->InsertString(0, "Trial");
	pCmb->InsertString(1, "Silver");
	pCmb->InsertString(2, "Gold");
	pCmb->InsertString(3, "Platinum");

	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	pCmb2->InsertString(0, "一般企業");
	pCmb2->InsertString(1, "ネットカフェ");

	CWnd* myWnd = GetDlgItem(IDC_BUTTON4);
	myWnd->EnableWindow(FALSE);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CpLicMakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CpLicMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CpLicMakerDlg::OnOK()
{
}

void CpLicMakerDlg::OnCancel()
{
}

void CpLicMakerDlg::OnBnClickedButton1()
{
	CDialog::OnCancel();
}

// 残りライセンス期間の計算結果を返す
CoString check_exp_days(CoString& data)
{
	// 残りライセンス期間の計算結果を返す
	CTime    cTime;
	int      year, month, day;

	cTime = CTime::GetCurrentTime();    // 現在時刻
	year  = cTime.GetYear();            // 年
	month = cTime.GetMonth();           // 月
	day   = cTime.GetDay();             // 日

	int year2  = ((data[0] - '0') * 1000) + ((data[1] - '0') * 100) + ((data[2] - '0') * 10) + (data[3] - '0');
	int month2 = ((data[4] - '0') * 10) + (data[5] - '0');
	int day2   = ((data[6] - '0') * 10) + (data[7] - '0');

	COleDateTime cOleTime1( year, month, day, 12, 0, 0 );  // 現在日時
	COleDateTime cOleTime2( year2, month2,  day2, 12, 0, 0 );  // 終了日
	COleDateTimeSpan cOleTms = cOleTime2 - cOleTime1;

	CString exp_days;
	exp_days.Format(_T("%ld"), cOleTms.GetDays());

	return exp_days;
}

// ライセンス作成時の処理
void CpLicMakerDlg::OnBnClickedButton2()
{
	// 店舗名
	CoString text1;
	GetDlgItemText(IDC_EDIT1, text1);

	// ライセンス数
	CoString text2;
	GetDlgItemText(IDC_EDIT2, text2);

	// 店舗コード
	CoString text3;
	GetDlgItemText(IDC_EDIT3, text3);

	// ここで店舗コードのチェックを行う
	// 店舗コードは基本的には大文字英のみ最大10桁
	

	// ライセンス文字列
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO1);
	CoString lab_name;
	int index = pCmb->GetCurSel();
	if (index == -1)
	{
		GetDlgItemText(IDC_COMBO1, lab_name);
	}
	else
	{
		pCmb->GetLBText(index, lab_name);
	}

	// 有効期限
	CoString text5;
	GetDlgItemText(IDC_EDIT5, text5);

	// ここで有効期限のチェックを行う
	// YYYYMMDD形式

	// ライセンスの残り期限を計算
	CString exp_days = check_exp_days(text5);
	SetDlgItemText(IDC_STATIC_EXP_DAYS,exp_days);

	// アップグレード管理文字列
	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	CoString lab_name2;
	int index2 = pCmb2->GetCurSel();
	if (index2 == -1)
	{
		// 不正ない値の場合はネットカフェ(1)を設定
		pCmb2->SetCurSel(1);
		index2 = 1;
	}

	// 暗号化された文字列
	CoString text4;

	// Format "ユニーク文字列","有効期限","アップグレード管理文字列","ライセンス文字列","店舗名","クライアント数"
	text4.Format("License:\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\"",
		text3, text5, index2, lab_name, text1, text2);

	CoString base64;
	base64_encode(text4, base64);

	SetDlgItemText(IDC_EDIT4, base64);
	SetDlgItemText(IDC_EDIT6, text4);

	// 保存ボタンの有効化
	CWnd* myWnd = GetDlgItem(IDC_BUTTON4);
	myWnd->EnableWindow(TRUE);
}

// デコードボタンクリック時の処理
void CpLicMakerDlg::OnBnClickedButton3()
{
	CoString text1;
	GetDlgItemText(IDC_EDIT4, text1);
	
	CoString text2;
	base64_decode(text1, text2);

	SetDlgItemText(IDC_EDIT6, text2);

	text2DlgItems(text2);

	// 保存ボタンの有効化
	CWnd* myWnd = GetDlgItem(IDC_BUTTON4);
	myWnd->EnableWindow(TRUE);
}


void CpLicMakerDlg::OnBnClickedButton4()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	// 店舗名
	CoString text1;
	GetDlgItemText(IDC_EDIT1, text1);

	// ライセンス数
	CoString text2;
	GetDlgItemText(IDC_EDIT2, text2);

	// 店舗コード
	CoString text3;
	GetDlgItemText(IDC_EDIT3, text3);

	// ここで店舗コードのチェックを行う
	// 店舗コードは基本的には大文字英のみ最大10桁
	

	// ライセンス文字列
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO1);
	CoString lab_name;
	int index = pCmb->GetCurSel();
	if (index == -1)
	{
		GetDlgItemText(IDC_COMBO1, lab_name);
	}
	else
	{
		pCmb->GetLBText(index, lab_name);
	}

	// 有効期限
	CoString text5;
	GetDlgItemText(IDC_EDIT5, text5);

	// ここで有効期限のチェックを行う
	// YYYYMMDD形式

	// ライセンスの残り期限を計算
	CString exp_days = check_exp_days(text5);
	SetDlgItemText(IDC_STATIC_EXP_DAYS,exp_days);

	// アップグレード管理文字列
	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	CoString lab_name2;
	int index2 = pCmb2->GetCurSel();
	if (index2 == -1)
	{
		// 不正ない値の場合はネットカフェ(1)を設定
		pCmb2->SetCurSel(1);
		index2 = 1;
	}

	// 暗号化された文字列
	CoString text4;

	// Format "ユニーク文字列","有効期限","アップグレード管理文字列","ライセンス文字列","店舗名","クライアント数"
	text4.Format("License:\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\"",
		text3, text5, index2, lab_name, text1, text2);

	CoString base64;
	base64_encode(text4, base64);

	//保存先選択
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("iniファイル(*.ini)\0*.ini\0")
		TEXT("すべてのファイル(*.*)\0*.*\0\0");
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = "ini";
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	GetSaveFileName(&ofn);

	// INIファイルに情報を保存する
	WritePrivateProfileString(SEC_PLM, KEY_BEFORE, text4, szFile);
	WritePrivateProfileString(SEC_PLM, KEY_AFTER, base64, szFile);

	SetDlgItemText(IDC_EDIT4, base64);
	SetDlgItemText(IDC_EDIT6, text4);
}

void CpLicMakerDlg::OnBnClickedButton5()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("iniファイル(*.ini)\0*.ini\0")
		TEXT("すべてのファイル(*.*)\0*.*\0\0");
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	GetOpenFileName(&ofn);

	char strBefore[1024];
	char strAfter[1024];
	GetPrivateProfileString(SEC_PLM, KEY_BEFORE, "", strBefore, sizeof(strBefore), szFile);
	GetPrivateProfileString(SEC_PLM, KEY_AFTER, "", strAfter, sizeof(strAfter), szFile);

	SetDlgItemText(IDC_EDIT4, strAfter);
	SetDlgItemText(IDC_EDIT6, strBefore);
	text2DlgItems(strBefore);
}

void CpLicMakerDlg::text2DlgItems(const CoString &text)
{
	INT pos = 0;
	CoString unique = text.AbstractSearchSubstance(pos);
	CoString expireLic = text.AbstractSearchSubstance(pos);
	CoString gradeText = text.AbstractSearchSubstance(pos);
	CoString assessment_name = text.AbstractSearchSubstance(pos);
	CoString shopName = text.AbstractSearchSubstance(pos);
	CoString massLic = text.AbstractSearchSubstance(pos);

	SetDlgItemText(IDC_EDIT1, shopName);  // 店舗名
	SetDlgItemText(IDC_EDIT2, massLic);   // ライセンス数
	SetDlgItemText(IDC_EDIT3, unique);   // 店舗コード
	SetDlgItemText(IDC_EDIT5, expireLic);  // 有効期限

	// ライセンスの残り期限を計算
	CString exp_days = check_exp_days(expireLic);
	SetDlgItemText(IDC_STATIC_EXP_DAYS,exp_days);

	// ライセンス形態
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO1);
	SetDlgItemText(IDC_COMBO1, assessment_name);

	// アップグレード管理文字列
	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	pCmb2->SetCurSel((unsigned char)atoi(gradeText));
}
