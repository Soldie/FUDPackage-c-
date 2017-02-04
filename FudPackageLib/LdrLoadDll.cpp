#include "stdafx.h"
#include "Base.h"

#ifndef __GNUC__
// disable warnings about pointer <-> DWORD conversions
#pragma warning( disable : 4311 4312 4390 4996)
#endif

#ifdef _WIN64
#define POINTER_TYPE ULONGLONG
#else
#define POINTER_TYPE DWORD
#endif

#define RVATOVA( base, offset )(((DWORD)(base) + (DWORD)(offset))) 

#define LDRP_RELOCATION_FINAL 0x2 


T_RtlInitUnicodeString pRtlInitUnicodeString;
T_NtUnmapViewOfSection pNtUnmapViewOfSection;
T_NtOpenFile pNtOpenFile;
T_NtOpenDirectoryObject pNtOpenDirectoryObject;
T_NtOpenSection pNtOpenSection;
T_NtMapViewOfSection pNtMapViewOfSection;
T_RtlNtStatusToDosError pRtlNtStatusToDosError;
T_NtClose pNtClose;
T_NtCreateFile pNtCreateFile;
T_NtCreateSection pNtCreateSection;
T_NtQuerySection pNtQuerySection;
T_LdrLoadDll pLdrLoadDll;
T_RtlCompareUnicodeString pRtlCompareUnicodeString;
T_RtlPushFrame pRtlPushFrame;
T_RtlGetFrame pRtlGetFrame;
T_RtlPopFrame pRtlPopFrame;
T_NtQueryAttributesFile pNtQueryAttributesFile;
T_RtlDosPathNameToNtPathName_U pRtlDosPathNameToNtPathName_U;


T_NtOpenFile OldNtOpenFile = NULL;
T_NtCreateSection OldNtCreateSection = NULL;
T_NtQuerySection OldNtQuerySection = NULL;
T_NtMapViewOfSection OldNtMapViewOfSection = NULL;
T_NtClose OldNtClose = NULL;
T_NtQueryAttributesFile OldNtQueryAttributesFile = NULL;

//////////////////////////////////////////////////////////////////////////
//³õÊ¼»¯
//////////////////////////////////////////////////////////////////////////
BOOL m_bLoadSection = FALSE;
BOOL initLdr()
{
	HMODULE hNtdll = GetModuleHandle(TEXT("ntdll.dll"));

	if (m_bLoadSection)
	{
		return m_bLoadSection;
	}
	if (!(pRtlInitUnicodeString = (T_RtlInitUnicodeString)GetProcAddress(hNtdll, "RtlInitUnicodeString"))) return FALSE;

	if (!(pNtUnmapViewOfSection = (T_NtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection")))	return FALSE;

	if (!(pNtOpenFile = (T_NtOpenFile)GetProcAddress(hNtdll, "NtOpenFile"))) return FALSE;

	if (!(pNtOpenDirectoryObject = (T_NtOpenDirectoryObject)GetProcAddress(hNtdll, "NtOpenDirectoryObject"))) return FALSE;

	if (!(pNtOpenSection = (T_NtOpenSection)GetProcAddress(hNtdll, "NtOpenSection"))) return FALSE;

	if (!(pNtMapViewOfSection = (T_NtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection"))) return FALSE;

	if (!(pRtlNtStatusToDosError = (T_RtlNtStatusToDosError)GetProcAddress(hNtdll, "RtlNtStatusToDosError"))) return FALSE;

	if (!(pNtClose = (T_NtClose)GetProcAddress(hNtdll, "NtClose"))) return FALSE;

	if (!(pNtCreateFile = (T_NtCreateFile)GetProcAddress(hNtdll, "NtCreateFile")))	return FALSE;

	if (!(pNtCreateSection = (T_NtCreateSection)GetProcAddress(hNtdll, "NtCreateSection"))) return FALSE;

	if (!(pNtQuerySection = (T_NtQuerySection)GetProcAddress(hNtdll, "NtQuerySection"))) return FALSE;

	if (!(pLdrLoadDll = (T_LdrLoadDll)GetProcAddress(hNtdll, "LdrLoadDll"))) return FALSE;

	if (!(pRtlCompareUnicodeString = (T_RtlCompareUnicodeString)GetProcAddress(hNtdll, "RtlCompareUnicodeString"))) return FALSE;

	if (!(pRtlPushFrame = (T_RtlPushFrame)GetProcAddress(hNtdll, "RtlPushFrame"))) return FALSE;

	if (!(pRtlGetFrame = (T_RtlGetFrame)GetProcAddress(hNtdll, "RtlGetFrame"))) return FALSE;

	if (!(pRtlPopFrame = (T_RtlPopFrame)GetProcAddress(hNtdll, "RtlPopFrame"))) return FALSE;

	if (!(pNtQueryAttributesFile = (T_NtQueryAttributesFile)GetProcAddress(hNtdll, "NtQueryAttributesFile"))) return FALSE;

	if (!(pRtlDosPathNameToNtPathName_U = (T_RtlDosPathNameToNtPathName_U)GetProcAddress(hNtdll, "RtlDosPathNameToNtPathName_U"))) return FALSE;

	//LdrSetDllManifestProber
	DWORD dwOld = 0;
	PVOID pLdrSetDllManifestProber = GetProcAddress(hNtdll, "LdrSetDllManifestProber");
	ULONG ptrClear = *(PULONG)((ULONG_PTR)pLdrSetDllManifestProber + 0x9);
	VirtualProtect((LPVOID)ptrClear, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOld);
	*(DWORD *)ptrClear = NULL;

	InlineHook((void *)pNtOpenFile, (void *)OnNtOpenFile, (void **)&OldNtOpenFile);
	InlineHook((void *)pNtCreateSection, (void *)OnNtCreateSection, (void **)&OldNtCreateSection);
	InlineHook((void *)pNtQuerySection, (void *)OnNtQuerySection, (void **)&OldNtQuerySection);
	InlineHook((void *)pNtMapViewOfSection, (void *)OnNtMapViewOfSection, (void **)&OldNtMapViewOfSection);
	InlineHook((void *)pNtClose, (void *)OnNtClose, (void **)&OldNtClose);
	InlineHook((void *)pNtQueryAttributesFile, (void *)OnNtQueryAttributesFile, (void **)&OldNtQueryAttributesFile);
	//InlineHook((void *)pNtUnmapViewOfSection,(void *)OnNtUnmapViewOfSection,(void **)&OldNtUnmapViewOfSection);
	m_bLoadSection = TRUE;
	return TRUE;
}

NTSTATUS __stdcall OnNtOpenFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
	)
{
	NTSTATUS            Status;
	LOAD_MEM_DLL_INFO  *MemDllInfo;

	MemDllInfo = GetLoadMemDllInfo();
	if (MemDllInfo == NULL ||
		pRtlCompareUnicodeString(ObjectAttributes->ObjectName, &MemDllInfo->MemDllFullPath, TRUE))
	{
		return OldNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	}

	ObjectAttributes->ObjectName = &MemDllInfo->Lz32Path;
	Status = OldNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	if (NT_SUCCESS(Status))
	{
		MemDllInfo->DllFileHandle = *FileHandle;
	}
	return Status;
}

NTSTATUS __stdcall OnNtCreateSection(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,  // Optional
	IN PLARGE_INTEGER MaximumSize,           // Optional
	IN ULONG SectionPageProtection,
	IN ULONG AllocationAttributes,
	IN HANDLE FileHandle                     // Optional
	)
{
	BOOL                IsDllHandle;
	NTSTATUS            Status;
	LARGE_INTEGER       SectionSize;
	LOAD_MEM_DLL_INFO  *MemDllInfo;

	IsDllHandle = FALSE;
	MemDllInfo = NULL;

	if (FileHandle != NULL)
	{
		MemDllInfo = GetLoadMemDllInfo();
		if (MemDllInfo != NULL && MemDllInfo->DllFileHandle == FileHandle)
		{
			//            if (MaximumSize == NULL)
			MaximumSize = &SectionSize;

			MaximumSize->QuadPart = MemDllInfo->ViewSize;
			DesiredAccess = SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE;
			SectionPageProtection = PAGE_EXECUTE_READWRITE;
			AllocationAttributes = SEC_COMMIT;
			FileHandle = NULL;
			IsDllHandle = TRUE;
		}
	}

	Status = OldNtCreateSection(
		SectionHandle,
		DesiredAccess,
		ObjectAttributes,
		MaximumSize,
		SectionPageProtection,
		AllocationAttributes,
		FileHandle
		);

	if (!NT_SUCCESS(Status) || !IsDllHandle)
	{
		return Status;
	}

	MemDllInfo->SectionHandle = *SectionHandle;

	return Status;
}

NTSTATUS __stdcall OnNtQuerySection(
	IN HANDLE SectionHandle,
	IN SECTION_INFORMATION_CLASS SectionInformationClass,
	OUT PVOID SectionInformation,
	IN ULONG SectionInformationLength,
	OUT PULONG ResultLength OPTIONAL
	)
{
	PIMAGE_DOS_HEADER           DosHeader;
	PIMAGE_NT_HEADERS           NtHeaders;
	PIMAGE_OPTIONAL_HEADER      OptionalHeader;
	LOAD_MEM_DLL_INFO          *MemDllInfo;
	SECTION_IMAGE_INFORMATION  *ImageInfo;
	SECTION_BASIC_INFORMATION  *BasicInfo;

	MemDllInfo = GetLoadMemDllInfo();
	if (MemDllInfo == NULL || SectionHandle == NULL || MemDllInfo->SectionHandle != SectionHandle)
		goto DEFAULT_PROC;

	DosHeader = (PIMAGE_DOS_HEADER)MemDllInfo->MemDllBase;
	NtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)DosHeader + DosHeader->e_lfanew);
	OptionalHeader = &NtHeaders->OptionalHeader;

	switch (SectionInformationClass)
	{
	case SectionBasicInformation:
		BasicInfo = (SECTION_BASIC_INFORMATION *)SectionInformation;
		BasicInfo->BaseAddress = MemDllInfo->MappedBase;
		BasicInfo->Attributes = 0;
		BasicInfo->Size.QuadPart = MemDllInfo->ViewSize;
		break;

	case SectionImageInformation:
		if (ResultLength != NULL)
			*ResultLength = sizeof(*ImageInfo);

		if (SectionInformationLength < sizeof(*ImageInfo))
			return STATUS_BUFFER_TOO_SMALL;

		if (SectionInformation == NULL)
			break;

		ImageInfo = (SECTION_IMAGE_INFORMATION *)SectionInformation;
		ImageInfo->TransferAddress = (PVOID)((ULONG_PTR)DosHeader + OptionalHeader->AddressOfEntryPoint);
		ImageInfo->ZeroBits = 0;
		ImageInfo->MaximumStackSize = OptionalHeader->SizeOfStackReserve;
		ImageInfo->CommittedStackSize = OptionalHeader->SizeOfStackCommit;
		ImageInfo->SubSystemType = OptionalHeader->Subsystem;
		ImageInfo->SubSystemMinorVersion = OptionalHeader->MinorSubsystemVersion;
		ImageInfo->SubSystemMajorVersion = OptionalHeader->MajorSubsystemVersion;
		ImageInfo->GpValue = 0;
		ImageInfo->ImageCharacteristics = NtHeaders->FileHeader.Characteristics;
		ImageInfo->DllCharacteristics = OptionalHeader->DllCharacteristics;
		ImageInfo->Machine = NtHeaders->FileHeader.Machine;
		ImageInfo->ImageContainsCode = 0; // OptionalHeader->SizeOfCode;
		ImageInfo->LoaderFlags = OptionalHeader->LoaderFlags;
		ImageInfo->ImageFileSize = MemDllInfo->DllBufferSize;
		ImageInfo->CheckSum = OptionalHeader->CheckSum;
		break;

	case SectionRelocationInformation:
		if (SectionInformation != NULL)
			*(PULONG_PTR)SectionInformation = (ULONG_PTR)MemDllInfo->MappedBase - (ULONG_PTR)OptionalHeader->ImageBase;

		if (ResultLength != NULL)
			*ResultLength = sizeof(ULONG_PTR);

		break;

	default:
		goto DEFAULT_PROC;
	}

	return STATUS_SUCCESS;

DEFAULT_PROC:
	return OldNtQuerySection(SectionHandle, SectionInformationClass, SectionInformation, SectionInformationLength, ResultLength);
}
NTSTATUS __stdcall OnNtMapViewOfSection(
	IN HANDLE  SectionHandle,
	IN HANDLE  ProcessHandle,
	IN OUT PVOID  *BaseAddress,
	IN ULONG  ZeroBits,
	IN ULONG  CommitSize,
	IN OUT PLARGE_INTEGER  SectionOffset,	// optional
	IN OUT PULONG  ViewSize,
	IN SECTION_INHERIT  InheritDisposition,
	IN ULONG  AllocationType,
	IN ULONG  Protect
	)
{
	NTSTATUS                    Status;
	LOAD_MEM_DLL_INFO          *MemDllInfo;
	PIMAGE_DOS_HEADER           DosHeader;
	PIMAGE_NT_HEADERS           NtHeader;
	PIMAGE_SECTION_HEADER       SectionHeader;
	PBYTE                       DllBase, ModuleBase;
	ULONG						NewViewSize;
	if (SectionHandle == NULL)
		goto CALL_OLD_PROC;

	MemDllInfo = GetLoadMemDllInfo();
	if (MemDllInfo == NULL)
		goto CALL_OLD_PROC;

	if (SectionHandle != MemDllInfo->SectionHandle)
		goto CALL_OLD_PROC;

	if (SectionOffset != NULL)
		SectionOffset->QuadPart = 0;
	if (ViewSize == NULL)
	{
		ViewSize = &NewViewSize;
	}
	//if(ViewSize)
	*ViewSize = MemDllInfo->ViewSize;

	Status = OldNtMapViewOfSection(
		SectionHandle,
		ProcessHandle,
		BaseAddress,
		0,
		0,
		NULL,
		ViewSize,
		ViewShare,
		0,
		PAGE_EXECUTE_READWRITE
		);
	if (!NT_SUCCESS(Status))
		return Status;

	MemDllInfo->MappedBase = *BaseAddress;

	ModuleBase = (PBYTE)*BaseAddress;
	DllBase = (PBYTE)MemDllInfo->MemDllBase;

	if (FLAG_ON(MemDllInfo->Flags, LMD_MAPPED_DLL))
	{
		CopyMemory(ModuleBase, DllBase, MemDllInfo->ViewSize);
	}
	else
	{
		DosHeader = (PIMAGE_DOS_HEADER)DllBase;
		NtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBase + DosHeader->e_lfanew);
		SectionHeader = (PIMAGE_SECTION_HEADER)((ULONG_PTR)&NtHeader->OptionalHeader + NtHeader->FileHeader.SizeOfOptionalHeader);
		for (ULONG NumberOfSections = NtHeader->FileHeader.NumberOfSections; NumberOfSections; ++SectionHeader, --NumberOfSections)
		{
			CopyMemory(
				ModuleBase + SectionHeader->VirtualAddress,
				DllBase + SectionHeader->PointerToRawData,
				SectionHeader->SizeOfRawData
				);
		}

		//        CopyMemory(ModuleBase, DllBase, (ULONG_PTR)SectionHeader - (ULONG_PTR)DllBase);
		CopyMemory(ModuleBase, DllBase, MEMORY_PAGE_SIZE);
	}

	Status = (ULONG_PTR)ModuleBase != NtHeader->OptionalHeader.ImageBase ? STATUS_IMAGE_NOT_AT_BASE : STATUS_SUCCESS;
	return Status;

CALL_OLD_PROC:
	return OldNtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Protect);
}
NTSTATUS __stdcall OnNtClose(
	IN HANDLE Handle
	)
{
	LOAD_MEM_DLL_INFO *MemDllInfo;

	MemDllInfo = GetLoadMemDllInfo();
	if (MemDllInfo != NULL && Handle != NULL)
	{
		if (MemDllInfo->DllFileHandle == Handle)
			MemDllInfo->DllFileHandle = NULL;
		else if (MemDllInfo->SectionHandle == Handle)
			MemDllInfo->SectionHandle = NULL;
	}
	return OldNtClose(Handle);
}

NTSTATUS __stdcall OnNtQueryAttributesFile(
	IN POBJECT_ATTRIBUTES     ObjectAttributes,
	IN PFILE_BASIC_INFORMATION FileInformation
	)
{
	LOAD_MEM_DLL_INFO  *MemDllInfo;

	MemDllInfo = GetLoadMemDllInfo();

	if (MemDllInfo == NULL || pRtlCompareUnicodeString(ObjectAttributes->ObjectName, &MemDllInfo->MemDllFullPath, TRUE))
	{
		goto PassThr;
	}

	return STATUS_SUCCESS;
PassThr:
	return OldNtQueryAttributesFile(ObjectAttributes, FileInformation);
}

NTSTATUS
LoadDllFromMemory(
	IN PVOID           DllBuffer,
	IN ULONG           DllBufferSize,
	IN PUNICODE_STRING ModuleFileName,
	OUT PVOID*          ModuleHandle OPTIONAL,
	IN ULONG           Flags OPTIONAL
	)
{
	NTSTATUS            Status = STATUS_UNSUCCESSFUL;
	PVOID               ModuleBase;
	LOAD_MEM_DLL_INFO   MemDllInfo;
	PIMAGE_DOS_HEADER   DosHeader;
	PIMAGE_NT_HEADERS   NtHeader;
	WCHAR               Lz32DosPath[MAX_PATH];
	WCHAR               SystemPath[MAX_PATH];
	ZeroMemory(&MemDllInfo, sizeof(MemDllInfo));
	MemDllInfo.Context = LOAD_MEM_DLL_INFO_MAGIC;
	{
		pRtlPushFrame(&MemDllInfo);

		Status = STATUS_UNSUCCESSFUL;
		do
		{
			if (!pRtlDosPathNameToNtPathName_U(ModuleFileName->Buffer, &MemDllInfo.MemDllFullPath, NULL, NULL))
				break;

			/*Length = Nt_GetSystemDirectory(Lz32DosPath, countof(Lz32DosPath));
			*(PULONG64)(Lz32DosPath + Length)       = TAG4W('lz32');
			*(PULONG64)(Lz32DosPath + Length + 4)   = TAG4W('.dll');
			Lz32DosPath[Length + 8]                 = 0;*/
			GetSystemDirectoryW(SystemPath, MAX_PATH);
			wcsncpy(Lz32DosPath, SystemPath, MAX_PATH);
			wcsncat(Lz32DosPath, L"\\Lz32.dll", MAX_PATH);
			;//;//;//;//;//OutputDebugStringW(Lz32DosPath);
			if (!pRtlDosPathNameToNtPathName_U(Lz32DosPath, &MemDllInfo.Lz32Path, NULL, NULL))
				break;

			MemDllInfo.Flags = Flags;
			MemDllInfo.MemDllBase = DllBuffer;
			MemDllInfo.DllBufferSize = DllBufferSize;
			DosHeader = (PIMAGE_DOS_HEADER)DllBuffer;
			NtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBuffer + DosHeader->e_lfanew);
			MemDllInfo.ViewSize = NtHeader->OptionalHeader.SizeOfImage;
			Status = pLdrLoadDll(NULL, 0, ModuleFileName, &ModuleBase);
			if (!NT_SUCCESS(Status) && !FLAG_ON(Flags, LMD_MAPPED_DLL))
			{
				break;
			}

			if (ModuleHandle != NULL)
				*ModuleHandle = (HANDLE)ModuleBase;
		} while (0);

		pRtlPopFrame(&MemDllInfo);
		return Status;
	}
	return STATUS_MEMORY_NOT_ALLOCATED;
}

BOOL LdrLoadMemDll(PVOID DllMem, SIZE_T dllSize, LPCWSTR szDllName, HMODULE *pModule)
{
	if (initLdr())
	{
		UNICODE_STRING unDllName;
		pRtlInitUnicodeString(&unDllName, szDllName);
		if (NT_SUCCESS(LoadDllFromMemory(DllMem, dllSize, &unDllName, (PVOID *)pModule, 0)))
		{
			return TRUE;
		}
	}
	return FALSE;
}