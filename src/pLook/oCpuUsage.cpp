///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. �v���X�^�[
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

#include "oCpuUsage.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CCpuUsageForNT::CCpuUsageForNT()
{
	liOldIdleTime.LowPart = 0;
	liOldIdleTime.HighPart = 0;
	liOldIdleTime.u.LowPart = 0;
	liOldIdleTime.u.HighPart = 0;
	liOldIdleTime.QuadPart = 0;

	liOldSystemTime = liOldIdleTime;

	m_pProcNTQSI			= (PROCNTQSI)GetNTQSIProcAdrs();
	m_NumberOfProcessors	= GetNumberOfProcessors();
}

CCpuUsageForNT::~CCpuUsageForNT()
{
	ReleaseProc();
}

///////////////////////////////////////////////////////////////////////////////
//�@NtQuerySystemInformation �ւ̊֐��|�C���^�� NTDLL.DLL ���瓾��
///////////////////////////////////////////////////////////////////////////////
// ����:
//	�Ȃ�
//
// �߂�l:
//	PROCNTQSI	NtQuerySystemInformation �ւ̊֐��|�C���^
//
PROCNTQSI CCpuUsageForNT::GetNTQSIProcAdrs()
{
	PROCNTQSI NtQuerySystemInformation;
	HMODULE hModule;

	if ((hModule = GetModuleHandle("ntdll")) == NULL)
	{
		return (PROCNTQSI)NULL;
	}

	m_hModuleForNTDLL = hModule; // �����o�ϐ���DLL�̃n���h�����R�s�[����

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(hModule, "NtQuerySystemInformation");

    if (!NtQuerySystemInformation)
        return (PROCNTQSI)NULL;
	else
		return (PROCNTQSI)NtQuerySystemInformation;
}


///////////////////////////////////////////////////////////////////////////////
//�@NtQuerySystemInformation �ւ̊֐��|�C���^���������
///////////////////////////////////////////////////////////////////////////////
// ����:
//	�Ȃ�
//
// �߂�l:
//	�Ȃ�
//
void CCpuUsageForNT::ReleaseProc()
{
	if (m_hModuleForNTDLL != NULL)
	{
		FreeLibrary(m_hModuleForNTDLL);

		// �g���I������ϐ��͈ꉞ NULL �ɃZ�b�g���Ă���
		m_hModuleForNTDLL = (HMODULE)NULL;
		m_pProcNTQSI = (PROCNTQSI)NULL;
		m_NumberOfProcessors = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//�@�V�X�e���ɓ��ڂ���Ă��� CPU �̐��𓾂܂��B
///////////////////////////////////////////////////////////////////////////////
// ����:
//	�Ȃ�
//
// �߂�l:
//	int					0 �G���[
//				
//			CPU �̌�	1 - single processor
//						2 - dual processor
//
BYTE CCpuUsageForNT::GetNumberOfProcessors()
{
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;
    LONG                           status;

    // get number of processors in the system
    status = m_pProcNTQSI(SystemBasicInformation, &SysBaseInfo, sizeof SysBaseInfo, NULL);

    if (status != NO_ERROR)
	{
        return 0;
	}

	return SysBaseInfo.bKeNumberProcessors;
}


///////////////////////////////////////////////////////////////////////////////
//�@���݂�CPU���p���𓾂܂�
///////////////////////////////////////////////////////////////////////////////
// ����:
//	�Ȃ�
//
// �߂�l:
//	int		0-100 [%] ��CPU���p�� (single processor �̂ݑΉ����Ă���)
//
//			0 ���A������̂̓G���[�̉\��������
//
int CCpuUsageForNT::GetCpuUsageForNT()
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo = { 0 };
	SYSTEM_TIME_INFORMATION SysTimeInfo = { 0 };
    double dbIdleTime = 0.0;
    double dbSystemTime = 0.0;

	int cpuUsage = 0; // CPU �g�p�� [%]

	// get new system time
	LONG status = m_pProcNTQSI(SystemTimeInformation, &SysTimeInfo, sizeof SysTimeInfo, 0);
	if (status != NO_ERROR)
	{
		return 0;
	}

	// get new CPU's idle time
	status = m_pProcNTQSI(SystemPerformanceInformation, &SysPerfInfo, sizeof SysPerfInfo, NULL);
	if (status != NO_ERROR)
	{
		return 0;
	}

	// if it's a first call - skip it
	if (liOldIdleTime.QuadPart != 0)
	{
		// CurrentValue = NewValue - OldValue
		dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
		dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

		// CurrentCpuIdle = IdleTime / SystemTime
		dbIdleTime = dbIdleTime / dbSystemTime;

		// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)m_NumberOfProcessors + 0.5;
		
		//printf("\b\b\b\b%3d%%",(UINT)dbIdleTime);
		cpuUsage = (int)dbIdleTime;
	}

	// store new CPU's idle and system time
	liOldIdleTime = SysPerfInfo.liIdleTime;
	liOldSystemTime = SysTimeInfo.liKeSystemTime;

	return cpuUsage;
}


///////////////////////////////////////////////////////////////////////////////
//�@NTDLL.DLL ���J�������ǂ������ׂ܂��B�O������̓���m�F�ɂ��g����������
///////////////////////////////////////////////////////////////////////////////
// ����:
//	�Ȃ�
//
// �߂�l:
//	BOOL	TRUE	�������Ă���BCPU �̗��p�����擾�ł���B
//			FALSE	���s���Ă���BCPU �̗��p�����擾�ł��Ȃ��B
//
BOOL CCpuUsageForNT::IsOpenDLL()
{
	return (m_hModuleForNTDLL != NULL) ? TRUE: FALSE;
}
