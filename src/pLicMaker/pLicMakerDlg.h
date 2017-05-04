// pLicMakerDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxext.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

// CpLicMakerDlg �_�C�A���O
class CpLicMakerDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CpLicMakerDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_PLICMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
