// pLicMakerDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxext.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

// CpLicMakerDlg ダイアログ
class CpLicMakerDlg : public CDialog
{
// コンストラクション
public:
	CpLicMakerDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_PLICMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CStatusBar m_statusbar;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();

private:
	void text2DlgItems(const CoString &);
};
