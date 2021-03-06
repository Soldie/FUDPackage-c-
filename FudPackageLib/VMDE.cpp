#include "stdafx.h"
extern "C"
{
#include "global.h"
#include "detect.h"
};
#define VMP 0
#if VMP
#include "../3rd/vmp/VMProtectSDK.h"
#ifdef _WIN64
#pragma comment(lib, "../3rd/vmp/VMProtectSDK64.lib")
#else
#pragma comment(lib, "../3rd/vmp/VMProtectSDK32.lib")
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
	// protection
#define VMProtectBegin(_) 
#define VMProtectBeginVirtualization(_) 
#define VMProtectBeginMutation(_)
#define VMProtectBeginUltra(_)
#define VMProtectBeginVirtualizationLockByKey(_)
#define VMProtectBeginUltraLockByKey(_)
#define VMProtectEnd()

#ifdef __cplusplus
}
#endif

#endif
#include <process.h>

#define PROGRAM_NAME TEXT("Virtual Machine Detector")
#define VERSION_NAME TEXT("VMD 1.0 build 09 19/02/2014")

//output data
#define VM_VPC TEXT("VirtualPC VM")
#define VM_VMWARE TEXT("VMWare VM")
#define VM_PARALLELS TEXT("Parallels VM")
#define VM_VBOX TEXT("VirtualBox VM")
#define VM_UNKNOWN  TEXT("Unknown VM")
#define VM_MSHV TEXT("Microsoft VMM")
#define VM_VMWAREHV TEXT("VMWare VMM")
#define VM_PRLHV TEXT("Parallels VMM")
#define VM_SANDBOXIE TEXT("Sandboxie present")
#define VM_SANDBOXIE_INSIDE TEXT("Inside Sandboxie")
#define VM_SANDBOXED TEXT("Running inside Sandboxie")
#define VM_UNKNOWNHV TEXT("Unknown VMM")
#define VM_HYPER_V TEXT("Microsoft Hyper-V")

BOOL g_IsWow64 = FALSE;
BOOL g_IsWin64 = FALSE;
RTL_OSVERSIONINFOW g_osver;
SYSTEM_INFO g_siSysInfo;

/*
* DetectSystemInfo
*
* Purpose:
*
* Remember system version and system info to global variables.
*
*/
VOID DetectSystemInfo(
	VOID
	)
{
	NTSTATUS Status;
	VMProtectBegin(__FUNCTION__);
	g_IsWin64 = supIs64BitWindows(&g_IsWow64);
	if (g_IsWow64) {
		GetNativeSystemInfo(&g_siSysInfo);
	}
	else {
		GetSystemInfo(&g_siSysInfo);
	}

	RtlSecureZeroMemory(&g_osver, sizeof(g_osver));
	g_osver.dwOSVersionInfoSize = sizeof(g_osver);

	Status = RtlGetVersion(&g_osver);
	if (NT_SUCCESS(Status)) {
		if (g_osver.dwMajorVersion < 6) {
			supEnablePrivilege(SE_DEBUG_PRIVILEGE, TRUE);
		}
	}
	VMProtectEnd();
}

/*
* DetectVMS
*
* Purpose:
*
* Execute detection methods.
*
*/
BOOL DetectVMS(
	VOID
	)
{
	VMProtectBegin(__FUNCTION__);
	BOOL cond = FALSE, bFound = FALSE;
	TCHAR szBuffer[MAX_PATH];

	RtlSecureZeroMemory(szBuffer, sizeof(szBuffer));

	do {

		if (IsSandboxiePresent()) 
		{
			bFound = TRUE;

			_strcpy(szBuffer, VM_SANDBOXIE);
			if (AmISandboxed()) 
			{
				_strcat(szBuffer, TEXT("\n"));
				_strcat(szBuffer, VM_SANDBOXIE_INSIDE);
			}
#ifndef _FULLOUT
			break;
#endif
		}

		if (IsHypervisor()) {
			bFound = TRUE;
			switch (GetHypervisorType()) {

			case 1:
				_strcpy(szBuffer, VM_MSHV);
				break;
			case 2:
				_strcpy(szBuffer, VM_VMWAREHV);
				break;
			case 3:
				_strcpy(szBuffer, VM_PRLHV);
				break;

			case 0:
			default:
				_strcpy(szBuffer, VM_UNKNOWNHV);
				break;

			}
#ifndef _FULLOUT
			break;
#endif
		}

		if (IsParallels()) {
			bFound = TRUE;
			_strcpy(szBuffer, VM_PARALLELS);
#ifndef _FULLOUT
			break;
#endif
		}

		if (IsVmWare()) {
			bFound = TRUE;
			_strcpy(szBuffer, VM_VMWARE);
#ifndef _FULLOUT
			break;
#endif
		}

		//there is no x64 guests support
#ifndef _WIN64
		if (IsVirtualPC()) {
			bFound = TRUE;
			_strcpy(szBuffer, VM_VPC);
#ifndef _FULLOUT
			break;
#endif
		}
#endif

		if (IsVirtualBox()) {
			bFound = TRUE;
			_strcpy(szBuffer, VM_VBOX);
#ifndef _FULLOUT
			break;
#endif
		}

		if (IsUnknownVM()) {
			bFound = TRUE;
			_strcpy(szBuffer, VM_UNKNOWN);
#ifndef _FULLOUT
			break;
#endif
		}

	} while (cond);

	/*if (bFound) {
		DisplayText(szBuffer, MB_ICONWARNING);
	}
	else {
		DisplayText(TEXT("Nothing detected"), MB_ICONINFORMATION);
	}*/
	
	return bFound;
	VMProtectEnd();
}

BOOL IsRunInVM()
{
	VMProtectBegin(__FUNCTION__);
	BOOL bRet = FALSE;
	DetectSystemInfo();

#ifdef _DEBUG
	DumpFirmwareTable();
#endif

	InitializeListHead(&VendorsListHead);

	EnumPCIDevsReg();

	bRet = DetectVMS();
	vFreeList();

	return bRet;
	VMProtectEnd();
}