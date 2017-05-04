///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

// pLicMakerDlg.cpp : �����t�@�C��
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

// CpLicMakerDlg �_�C�A���O

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


// CpLicMakerDlg ���b�Z�[�W �n���h��

BOOL CpLicMakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	{
		int err = 0;

		// �X�e�[�^�X�o�[������
		if (!err) if (!m_statusbar.Create(this)) err = 1;
		if (!err) RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		//m_statusbar.SetPaneText(0, _T("�X�e�[�^�X�o�["),1);
	}

	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO1);
	pCmb->InsertString(0, "Trial");
	pCmb->InsertString(1, "Silver");
	pCmb->InsertString(2, "Gold");
	pCmb->InsertString(3, "Platinum");

	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	pCmb2->InsertString(0, "��ʊ��");
	pCmb2->InsertString(1, "�l�b�g�J�t�F");

	CWnd* myWnd = GetDlgItem(IDC_BUTTON4);
	myWnd->EnableWindow(FALSE);

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CpLicMakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
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

// �c�胉�C�Z���X���Ԃ̌v�Z���ʂ�Ԃ�
CoString check_exp_days(CoString& data)
{
	// �c�胉�C�Z���X���Ԃ̌v�Z���ʂ�Ԃ�
	CTime    cTime;
	int      year, month, day;

	cTime = CTime::GetCurrentTime();    // ���ݎ���
	year  = cTime.GetYear();            // �N
	month = cTime.GetMonth();           // ��
	day   = cTime.GetDay();             // ��

	int year2  = ((data[0] - '0') * 1000) + ((data[1] - '0') * 100) + ((data[2] - '0') * 10) + (data[3] - '0');
	int month2 = ((data[4] - '0') * 10) + (data[5] - '0');
	int day2   = ((data[6] - '0') * 10) + (data[7] - '0');

	COleDateTime cOleTime1( year, month, day, 12, 0, 0 );  // ���ݓ���
	COleDateTime cOleTime2( year2, month2,  day2, 12, 0, 0 );  // �I����
	COleDateTimeSpan cOleTms = cOleTime2 - cOleTime1;

	CString exp_days;
	exp_days.Format(_T("%ld"), cOleTms.GetDays());

	return exp_days;
}

// ���C�Z���X�쐬���̏���
void CpLicMakerDlg::OnBnClickedButton2()
{
	// �X�ܖ�
	CoString text1;
	GetDlgItemText(IDC_EDIT1, text1);

	// ���C�Z���X��
	CoString text2;
	GetDlgItemText(IDC_EDIT2, text2);

	// �X�܃R�[�h
	CoString text3;
	GetDlgItemText(IDC_EDIT3, text3);

	// �����œX�܃R�[�h�̃`�F�b�N���s��
	// �X�܃R�[�h�͊�{�I�ɂ͑啶���p�̂ݍő�10��
	

	// ���C�Z���X������
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

	// �L������
	CoString text5;
	GetDlgItemText(IDC_EDIT5, text5);

	// �����ŗL�������̃`�F�b�N���s��
	// YYYYMMDD�`��

	// ���C�Z���X�̎c��������v�Z
	CString exp_days = check_exp_days(text5);
	SetDlgItemText(IDC_STATIC_EXP_DAYS,exp_days);

	// �A�b�v�O���[�h�Ǘ�������
	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	CoString lab_name2;
	int index2 = pCmb2->GetCurSel();
	if (index2 == -1)
	{
		// �s���Ȃ��l�̏ꍇ�̓l�b�g�J�t�F(1)��ݒ�
		pCmb2->SetCurSel(1);
		index2 = 1;
	}

	// �Í������ꂽ������
	CoString text4;

	// Format "���j�[�N������","�L������","�A�b�v�O���[�h�Ǘ�������","���C�Z���X������","�X�ܖ�","�N���C�A���g��"
	text4.Format("License:\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\"",
		text3, text5, index2, lab_name, text1, text2);

	CoString base64;
	base64_encode(text4, base64);

	SetDlgItemText(IDC_EDIT4, base64);
	SetDlgItemText(IDC_EDIT6, text4);

	// �ۑ��{�^���̗L����
	CWnd* myWnd = GetDlgItem(IDC_BUTTON4);
	myWnd->EnableWindow(TRUE);
}

// �f�R�[�h�{�^���N���b�N���̏���
void CpLicMakerDlg::OnBnClickedButton3()
{
	CoString text1;
	GetDlgItemText(IDC_EDIT4, text1);
	
	CoString text2;
	base64_decode(text1, text2);

	SetDlgItemText(IDC_EDIT6, text2);

	text2DlgItems(text2);

	// �ۑ��{�^���̗L����
	CWnd* myWnd = GetDlgItem(IDC_BUTTON4);
	myWnd->EnableWindow(TRUE);
}


void CpLicMakerDlg::OnBnClickedButton4()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	// �X�ܖ�
	CoString text1;
	GetDlgItemText(IDC_EDIT1, text1);

	// ���C�Z���X��
	CoString text2;
	GetDlgItemText(IDC_EDIT2, text2);

	// �X�܃R�[�h
	CoString text3;
	GetDlgItemText(IDC_EDIT3, text3);

	// �����œX�܃R�[�h�̃`�F�b�N���s��
	// �X�܃R�[�h�͊�{�I�ɂ͑啶���p�̂ݍő�10��
	

	// ���C�Z���X������
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

	// �L������
	CoString text5;
	GetDlgItemText(IDC_EDIT5, text5);

	// �����ŗL�������̃`�F�b�N���s��
	// YYYYMMDD�`��

	// ���C�Z���X�̎c��������v�Z
	CString exp_days = check_exp_days(text5);
	SetDlgItemText(IDC_STATIC_EXP_DAYS,exp_days);

	// �A�b�v�O���[�h�Ǘ�������
	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	CoString lab_name2;
	int index2 = pCmb2->GetCurSel();
	if (index2 == -1)
	{
		// �s���Ȃ��l�̏ꍇ�̓l�b�g�J�t�F(1)��ݒ�
		pCmb2->SetCurSel(1);
		index2 = 1;
	}

	// �Í������ꂽ������
	CoString text4;

	// Format "���j�[�N������","�L������","�A�b�v�O���[�h�Ǘ�������","���C�Z���X������","�X�ܖ�","�N���C�A���g��"
	text4.Format("License:\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\"",
		text3, text5, index2, lab_name, text1, text2);

	CoString base64;
	base64_encode(text4, base64);

	//�ۑ���I��
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("ini�t�@�C��(*.ini)\0*.ini\0")
		TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = "ini";
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	GetSaveFileName(&ofn);

	// INI�t�@�C���ɏ���ۑ�����
	WritePrivateProfileString(SEC_PLM, KEY_BEFORE, text4, szFile);
	WritePrivateProfileString(SEC_PLM, KEY_AFTER, base64, szFile);

	SetDlgItemText(IDC_EDIT4, base64);
	SetDlgItemText(IDC_EDIT6, text4);
}

void CpLicMakerDlg::OnBnClickedButton5()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("ini�t�@�C��(*.ini)\0*.ini\0")
		TEXT("���ׂẴt�@�C��(*.*)\0*.*\0\0");
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

	SetDlgItemText(IDC_EDIT1, shopName);  // �X�ܖ�
	SetDlgItemText(IDC_EDIT2, massLic);   // ���C�Z���X��
	SetDlgItemText(IDC_EDIT3, unique);   // �X�܃R�[�h
	SetDlgItemText(IDC_EDIT5, expireLic);  // �L������

	// ���C�Z���X�̎c��������v�Z
	CString exp_days = check_exp_days(expireLic);
	SetDlgItemText(IDC_STATIC_EXP_DAYS,exp_days);

	// ���C�Z���X�`��
	CComboBox* pCmb = (CComboBox*)GetDlgItem(IDC_COMBO1);
	SetDlgItemText(IDC_COMBO1, assessment_name);

	// �A�b�v�O���[�h�Ǘ�������
	CComboBox* pCmb2 = (CComboBox*)GetDlgItem(IDC_COMBO2);
	pCmb2->SetCurSel((unsigned char)atoi(gradeText));
}
