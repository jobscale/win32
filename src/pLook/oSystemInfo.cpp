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

#include "../oCommon/oTools.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 

// CPU���
#define SDFGH \
	CoString regProcessorNameString; \
	if (1) \
	{ \
		LPCTSTR p = NULL; \
		p = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"; \
		HKEY hKey; \
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p, 0, KEY_READ, &hKey) != ERROR_SUCCESS) \
		{ \
			DEBUG_OUT_DEFAULT("failed."); \
			return -1; \
		} \
		DWORD dwType; \
		DWORD dwSize = _MAX_PATH; \
		if (RegQueryValueEx(hKey, "ProcessorNameString", 0, &dwType, (LPBYTE)regProcessorNameString.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS) \
		{ \
			RegCloseKey(hKey); \
			DEBUG_OUT_DEFAULT("failed."); \
			return -1; \
		} \
		regProcessorNameString.ReleaseBuffer(); \

#define SDFGH_END \
		RegCloseKey(hKey); \
	} \

// �O���t�B�b�N���
#define ASDFG \
	CoString Display_DriverDesc; \
	if (1) \
	{ \
		LPCTSTR p = "SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0001"; \
		LPCTSTR p2 = "SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000"; \
		HKEY hKey; \
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p, 0, KEY_READ, &hKey) != ERROR_SUCCESS) \
		{ \
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p2, 0, KEY_READ, &hKey) != ERROR_SUCCESS) \
			{ \
				DEBUG_OUT_DEFAULT("failed."); \
				return -1; \
			} \
		} \
		DWORD dwType; \
		DWORD dwSize = _MAX_PATH; \
		if (RegQueryValueEx(hKey, "DriverDesc", 0, &dwType, (LPBYTE)Display_DriverDesc.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS) \
		{ \
			RegCloseKey(hKey); \
			DEBUG_OUT_DEFAULT("failed."); \
			return -1; \
		} \
		Display_DriverDesc.ReleaseBuffer(); \

#define ASDFG_END \
		RegCloseKey(hKey); \
	} \

// CPU���A�O���t�B�b�N�{�[�h�����擾
INT GetHardwareInformation(CoString& textData)
{
	CRITICALTRACE(systeminformation, GetHardwareInformation);

	OSVERSIONINFO info_ver = { sizeof OSVERSIONINFO };
	if (!GetVersionEx(&info_ver))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	// ���W�X�g������CPU�����擾����
	SDFGH
	SDFGH_END

	textData = "{\"name\": \"CPU���\",\"val\":\""+ regProcessorNameString +"\"},";

	// ���W�X�g������O���{�����擾����
	ASDFG
	ASDFG_END

	textData += "{\"name\": \"�O���t�B�b�N�h���C�o�[��\",\"val\":\""+ Display_DriverDesc +"\"},";

	return 0;
}

// �V�X�e�������擾����
INT GetSystemInformation(CoString& textData)
{
	CRITICALTRACE(systeminformation, GetSystemInformation);

	OSVERSIONINFO info_ver = { sizeof OSVERSIONINFO };
	if (!GetVersionEx(&info_ver))
	{
		DEBUG_OUT_DEFAULT("failed.");
		return -1;
	}

	// ���W�X�g������CPU�����擾����
	CoString regProcessorIdentifier;
	CoString regProcessorVendor;

	SDFGH
		// CPU�t�@�~���[���擾����
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "Identifier", 0, &dwType, (LPBYTE)regProcessorIdentifier.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		regProcessorIdentifier.ReleaseBuffer();

		// �v���_�N�g�L�[���擾����
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "VendorIdentifier", 0, &dwType, (LPBYTE)regProcessorVendor.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		regProcessorVendor.ReleaseBuffer();
	SDFGH_END

	textData = "{\"name\": \"CPU�x���_�[�R�[�h\",\"val\":\""+ regProcessorVendor +"\"},";
	textData += "{\"name\": \"CPU�t�@�~���[\",\"val\":\""+ regProcessorIdentifier +"\"},";
	textData += "{\"name\": \"CPU���\",\"val\":\""+ regProcessorNameString +"\"},";

	//�b�o�t�̏�� 
	//�b�o�t�̏��𓾂�ɂ� Windows-API �ł��� GetSystemInfo ���Ăяo���܂��B
	//����ɂ���ē�������ɂ͎��̂悤�ȕ�������܂��B
	//�v���Z�b�T�[�A�[�L�e�N�`��
	//  �b�o�t�� Intel / MIPS / Alpha / PowerPC ���̉��ꂩ��������܂��B
	//�v���Z�b�T�[�^�C�v
	//  �b�o�t�̋�̓I�Ȍ^�Ԃ�������x��������܂��B
	//    ��jIntel Pentium / Alpha 21064 / PowerPC 602 ��
	//�v���Z�b�T�[��
	//  �V�X�e���Ɏ�������Ă���b�o�t�̐��B
	//--------------------------------------------------------------------------------
	CoString pwork;
	CoString processor;
	{
		SYSTEM_INFO info_sys;
		GetSystemInfo(&info_sys);
		switch (info_sys.dwProcessorType)
		{
		case PROCESSOR_INTEL_386 : processor = _T("PROCESSOR_INTEL_386"); break;
		case PROCESSOR_INTEL_486 : processor = _T("PROCESSOR_INTEL_486"); break;
		case PROCESSOR_INTEL_PENTIUM : processor = _T("PROCESSOR_INTEL_PENTIUM"); break;
		case PROCESSOR_MIPS_R4000 : processor = _T("PROCESSOR_MIPS_R4000"); break;
		case PROCESSOR_ALPHA_21064 : processor = _T("PROCESSOR_ALPHA_21064"); break;
		case PROCESSOR_PPC_601 : processor = _T("PROCESSOR_PPC_601"); break;
		case PROCESSOR_PPC_603 : processor = _T("PROCESSOR_PPC_603"); break;
		case PROCESSOR_PPC_604 : processor = _T("PROCESSOR_PPC_604"); break;
		case PROCESSOR_PPC_620 : processor = _T("PROCESSOR_PPC_620"); break;
		case PROCESSOR_HITACHI_SH3 : processor = _T("PROCESSOR_HITACHI_SH3"); break;
		case PROCESSOR_HITACHI_SH3E : processor = _T("PROCESSOR_HITACHI_SH3E"); break;
		case PROCESSOR_HITACHI_SH4 : processor = _T("PROCESSOR_HITACHI_SH4"); break;
		case PROCESSOR_MOTOROLA_821 : processor = _T("PROCESSOR_MOTOROLA_821"); break;
		case PROCESSOR_SHx_SH3 : processor = _T("PROCESSOR_SHx_SH3"); break;
		case PROCESSOR_SHx_SH4 : processor = _T("PROCESSOR_SHx_SH4"); break;
		case PROCESSOR_STRONGARM : processor = _T("PROCESSOR_STRONGARM"); break;
		case PROCESSOR_ARM720 : processor = _T("PROCESSOR_ARM720"); break;
		case PROCESSOR_ARM820 : processor = _T("PROCESSOR_ARM820"); break;
		case PROCESSOR_ARM920 : processor = _T("PROCESSOR_ARM920"); break;
		case PROCESSOR_ARM_7TDMI : processor = _T("PROCESSOR_ARM_7TDMI"); break;
		default : processor = _T("unknown"); break;
		}

		pwork.Format("{\"name\":\"CPU���r�W����\", \"val\":\"%d\"},"
			"{\"name\":\"CPU�R�A��\", \"val\":\"%d\"},"
			"{\"name\":\"�y�[�W�T�C�Y\", \"val\":\"%d\"},",
			info_sys.wProcessorRevision,info_sys.dwNumberOfProcessors, info_sys.dwPageSize);
		//processor += work;
	}
	textData += "{\"name\": \"CPU�R�[�h\",\"val\": \""+ processor +"\"},";
	textData += pwork;
 
	//�������̏��
	//�������̏��𓾂�ɂ� Windows-API �ł���GlobalMemoryStatus���Ăяo���܂��B
	//����ɂ���ē�������ɂ͎��̂悤�ȕ�������܂��B
	//�����������̗ʁi�󂫁^�S�́j
	//���z�������̗ʁi�󂫁^�S�́j
	//�X���b�v�t�@�C���̑傫���i�y�[�W���O�T�C�Y�^�S�́j
	//--------------------------------------------------------------------------------
	CoString memory;
	{
		MEMORYSTATUS info_mem = { sizeof MEMORYSTATUS };
		GlobalMemoryStatus(&info_mem);
		memory.Format("{\"name\": \"����������\",\"val\": \"%d MB\"},{\"name\": \"���z������\",\"val\": \"%d MB\"},",
			(int)(info_mem.dwTotalPhys / 1024 / 1024), (int)(info_mem.dwTotalVirtual / 1024 / 1024));
	}
	textData += memory;
 
	//Windows �̃o�[�W���� 
	//--------------------------------------------------------------------------------
	CoString windows;
	{
		DWORD dwBuild = info_ver.dwBuildNumber;
		switch(info_ver.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s :
			windows = _T("Windows 3.1");
			break;
		case VER_PLATFORM_WIN32_WINDOWS :
			windows = _T("Windows 95/98/ME");
			dwBuild = LOWORD(info_ver.dwBuildNumber);
			break;
		case VER_PLATFORM_WIN32_NT :
			windows = _T("Windows NT");
			break;
		default :
			windows = _T("unknown");
			break;
		}

		//  ���W���[�o�[�W�����E�}�C�i�[�o�[�W����
		if (info_ver.dwMajorVersion == 5 && info_ver.dwMinorVersion == 0)
		{
			windows = "Windows 2000";
		}
		else if (info_ver.dwMajorVersion == 5 && info_ver.dwMinorVersion == 1)
		{
			windows = "Windows XP";
		}
		else if (info_ver.dwMajorVersion == 5 && info_ver.dwMinorVersion == 2)
		{
			windows = "Windows 2003";
		}
		else if (info_ver.dwMajorVersion == 6 && info_ver.dwMinorVersion == 0)
		{
			windows = "Windows VISTA";
		}
		else if (info_ver.dwMajorVersion == 7 && info_ver.dwMinorVersion == 0)
		{
			windows = "Windows 7";
		}
		else
		{
			CoString work;
			work.Format(" (%ld.%ld.%ld)", info_ver.dwMajorVersion, info_ver.dwMinorVersion, dwBuild);
			windows += work;
		}
	}
	textData += "{\"name\": \"OS\",\"val\": \""+ windows +"\"},";
 
	//Windows ���C���X�g�[������Ă���t�H���_ 
	//Windows ���C���X�g�[������Ă���t�H���_���擾����ɂ� Windows-API �ł���GetWindowsDirectory ���Ăяo���܂��B
	//�܂��V�X�e���t�H���_���擾����ɂ́A������GetSystemDirectory ���Ăяo���܂��B
 	//--------------------------------------------------------------------------------
	CoString windowsFolder;
	CoString systemFolder;
	{
		// Windows�ިڸ�؂̎擾
		if (GetWindowsDirectory(windowsFolder.GetBuffer(_MAX_PATH), _MAX_PATH) > _MAX_PATH)
		{
			windowsFolder = _T("�擾�ł��܂���");
		}
		else
		{
			windowsFolder.ReleaseBuffer();
			windowsFolder.Replace("\\", "\\\\");
		}

		// �����ިڸ�؂̎擾
		if (GetSystemDirectory(systemFolder.GetBuffer(_MAX_PATH), _MAX_PATH) > _MAX_PATH)
		{
			systemFolder = _T("�擾�ł��܂���");
		}
		else
		{
			systemFolder.ReleaseBuffer();
			systemFolder.Replace("\\", "\\\\");
		}
	}
	textData += "{\"name\": \"WINDOWS�t�H���_\",\"val\": \""+ windowsFolder +"\"},";
	textData += "{\"name\": \"SYSTEM�t�H���_\",\"val\": \""+ systemFolder +"\"},";
	//textData += "WINDOWS�t�H���_: ";
	//textData += windowsFolder + "\n";
	//textData += "SYSTEM�t�H���_: ";
	//textData += systemFolder + "\n";

 	//���O�C�����Ă��郆�[�U�[�̏�� 
	//���O�C�����Ă��郆�[�U�[�̏��̓��W�X�g������擾���܂��B
	//���W�X�g���̏��� Windows 3.1/95/98 �� Windows NT �ł̓��W�X�g���L�[���Ⴄ���߁A��L�̂悤�ȒP���ȏ����ł̓��[�U�[���𓾂邱�Ƃ͏o���܂���B 
	//Windows 3.1/95/98 �̃��W�X�g���L�[
	//SOFTWARE\MICROSOFT\Windows\CurrentVersion 
	//Windows NT �̃��W�X�g���L�[
	//Software\Microsoft\Windows NT\CurrentVersion 
	CoString registeredOrganization;
	CoString registeredOwner;
	CoString registeredProductId;
	//--------------------------------------------------------------------------------
	{
		// ڼ޽�؏����������邽�߂�Windows���ް�ޮݏ����擾����
		if (!GetVersionEx(&info_ver))
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		// Windows�̎�ʂɂ���ĊJ���T�u�L�[��ς���
		LPCTSTR p = NULL;
		if (info_ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			p = "Software\\Microsoft\\Windows NT\\CurrentVersion"; // WindowsNT
		}
		else
		{
			p = "SOFTWARE\\MICROSOFT\\Windows\\CurrentVersion"; // Windows3.1/95/98
		}
		// ڼ޽�؏�����݂���
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, p, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		{
			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		// ��Ж����擾����
		DWORD dwType;
		DWORD dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "RegisteredOrganization", 0, &dwType, (LPBYTE)registeredOrganization.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		registeredOrganization.ReleaseBuffer();
		// հ�ް�����擾����
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "RegisteredOwner", 0, &dwType, (LPBYTE)registeredOwner.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		registeredOwner.ReleaseBuffer();

		// �v���_�N�g�L�[���擾����
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "ProductId", 0, &dwType, (LPBYTE)registeredProductId.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		registeredProductId.ReleaseBuffer();
		RegCloseKey(hKey);
	}
	textData += "{\"name\": \"OS�o�^�Җ�\",\"val\":\""+ registeredOwner +"\"},";
	textData += "{\"name\": \"OS�o�^��Ɩ�\",\"val\":\""+ registeredOrganization +"\"},";
	textData += "{\"name\": \"�v���_�N�g�ԍ�\",\"val\":\""+ registeredProductId +"\"},";

	//textData += "�g�p��: ";
	//textData += registeredOwner + "\n";
	//textData += "���: ";
	//textData += registeredOrganization + "\n";

	//�ڑ�����Ă���@��̏�� 
	//�p�\�R���ɐڑ�����Ă���@��̏��𓾂�ɂ� Windows-API �ł���GetSystemMetrics ���Ăяo���܂��B
	//GetSystemMetrics ����͗l�X�ȏ��𓾂邱�Ƃ��o���A���̎�ȕ��Ƃ��Ă͎��̂��̂�����܂��B
	//SM_CXSCREEN �X�N���[���̂w�T�C�Y�i�h�b�g�j 
	//SM_CYSCREEN �X�N���[���̂x�T�C�Y�i�h�b�g�j 
	CoString screen;
	screen.Format("{\"name\": \"�X�N���[���T�C�Y\", \"val\": \"%d x %d\"},", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	textData += screen;

	// ���W�X�g������O���{�����擾����
	CoString Display_DriverVersion;
	CoString Display_ProviderName;

	ASDFG
		// CPU�t�@�~���[���擾����
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "DriverVersion", 0, &dwType, (LPBYTE)Display_DriverVersion.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		Display_DriverVersion.ReleaseBuffer();

		// �v���_�N�g�L�[���擾����
		dwSize = _MAX_PATH;
		if (RegQueryValueEx(hKey, "ProviderName", 0, &dwType, (LPBYTE)Display_ProviderName.GetBuffer(_MAX_PATH), &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);

			DEBUG_OUT_DEFAULT("failed.");
			return -1;
		}
		Display_ProviderName.ReleaseBuffer();
	ASDFG_END

	textData += "{\"name\": \"�O���t�B�b�N�h���C�o�[�x���_�[\",\"val\":\""+ Display_ProviderName +"\"},";
	textData += "{\"name\": \"�O���t�B�b�N�h���C�o�[��\",\"val\":\""+ Display_DriverDesc +"\"},";
	textData += "{\"name\": \"�O���t�B�b�N�h���C�o�[�o�[�W����\",\"val\":\""+ Display_DriverVersion +"\"},";

	//SM_MOUSEPRESENT �}�E�X�̐ڑ���� 
	//SM_CMOUSEBUTTONS �}�E�X�{�^���̐� 
	//CoString mouse;
	//BOOL isMouse = GetSystemMetrics(SM_MOUSEPRESENT);
	//if (isMouse)
	//{
	//	mouse.Format("%d �{�^���}�E�X Enable", GetSystemMetrics(SM_CMOUSEBUTTONS));
	//}
	//else
	//{
	//	mouse.Format("�ڑ�����Ă��܂���");
	//}
	//textData += "�}�E�X: ";
	//textData += mouse + "\n";

	//SM_NETWORK �l�b�g���[�N�����݂��邩�ǂ��� 
	CoString network = GetSystemMetrics(SM_NETWORK) ? "�L��" : "����";
	textData += "{\"name\": \"�l�b�g���[�N\",\"val\":\""+ network +"\"},";

	//textData += "�l�b�g���[�N: ";
	//textData += network + "\n";

	//�h���C�u�̏��
	//--------------------------------------------------------------------------------
	CoString drives;
	{
		//=====	Windows ���F�����Ă���h���C�u���擾���� ========
		DWORD dwLogicalDrives = GetLogicalDrives();		// �F���ł��Ă���h���C�u�Q���擾����

		//=====	�b�h���C�u��菇�ԂɌ�������(�`�C�a�̓t���b�s�[�Ȃ̂ŏȂ�) =======
		CoString strDrive;
		for (UINT nDrive = 0; nDrive < 26; nDrive++)
		{
			strDrive.Format("%c", nDrive + 'A');

			// �h���C�u�Ƃ��ĔF�����Ă��Ȃ��h���C�u�͌����Ώۂ���͂���
			if ((dwLogicalDrives & (1 << nDrive)) == 0)
			{
				continue;
			}

			// ���[�g�t�H���_
			CoString work;
			CoString rootDrive = strDrive + ":\\";
			UINT uiVal = GetDriveType(rootDrive);
			switch (uiVal)
			{
			case DRIVE_UNKNOWN : work = _T("�s���ȃh���C�u"); break;
			case DRIVE_REMOVABLE : work = _T("�����[�o�u���h���C�u"); break;
			case DRIVE_FIXED : work = _T("�Œ�h���C�u"); break;
			case DRIVE_REMOTE : work = _T("�����[�g�h���C�u"); break;
			case DRIVE_CDROM : work = _T("CD-ROM�h���C�u"); break;
			case DRIVE_RAMDISK : work = _T("RAM�h���C�u"); break;
			default : work = _T("���ڑ��h���C�u"); break;
			}

			drives += "{\"name\": \"�h���C�u(" + strDrive + ")\",";
			drives += "\"val\": \"" + work + "\"},";

			{
				// �t�@�C���V�X�e���`�F�b�N
				char volumeName[256] = { 0 };		// �{�����[���̖��O
				DWORD volumeNameSize = 256;			// �{�����[�����̃T�C�Y
				DWORD volumeSerialMunber;			// �{�����[���̒ʂ��ԍ�
				DWORD maxNameLength;				// �t�@�C�����̍ő咷
				DWORD fileSystemFlags;				// �t�@�C���V�X�e���̏��t���O
				char fileSystemName[256] = { 0 };	// �t�@�C���V�X�e���̖��O�iFAT,NTFS���j
				DWORD fileSystemNameSize = 256;		// �t�@�C���V�X�e���̖��O�̃T�C�Y

				// �{�����[�����x���������ꍇ�͕�����̍X�V���s���Ȃ��̂ŗ\�ߏ��������Ă���
				wsprintf(volumeName, ""); // ���{�����[�����x���̏�����
				if (GetVolumeInformation(rootDrive, volumeName, volumeNameSize, &volumeSerialMunber,
									&maxNameLength, &fileSystemFlags, (LPTSTR)fileSystemName, fileSystemNameSize))
				{
					work.Format("{\"name\" : \"�{�����[���̖��O(%s)\", \"val\": \"%s\"},", strDrive,(LPCTSTR)volumeName);
					drives += work;
					work.Format("{\"name\" : \"�{�����[���̃V���A���ԍ�(%s)\", \"val\" :\"%X\"},",strDrive, volumeSerialMunber);
					drives += work;
					work.Format("{\"name\" : \"�t�@�C���V�X�e���̖��O(%s)\", \"val\" :\"%s\"},", strDrive,fileSystemName);
					drives += work;

					ULARGE_INTEGER ulFreeBytesAvailable = { 0 };
					ULARGE_INTEGER ulTotalNumberOfBytes = { 0 };
					ULARGE_INTEGER ulTotalNumberOfFreeBytes = { 0 };
					GetDiskFreeSpaceEx(rootDrive, &ulFreeBytesAvailable, &ulTotalNumberOfBytes, &ulTotalNumberOfFreeBytes);
					work.Format("{\"name\" : \"���f�B�A�e��(%s)\", \"val\" :\"%d MB\"}, {\"name\": \"�󂫃X�y�[�X(%s)\", \"val\" :\"%d MB\"},", 
						strDrive,
						(int)(ulTotalNumberOfBytes.QuadPart / 1024 / 1024),
						strDrive,
						(int)(ulTotalNumberOfFreeBytes.QuadPart / 1024 / 1024)
					);
					drives += work;
				}
				else
				{
					drives += "";
				}
			}
		}
	}
	textData += drives;

	return 0;
}
