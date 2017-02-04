#include "stdafx.h"
#include "Base.h"
#include "ConfigData.h"
#include "PeFile.h"

USHORT
	ChkSum(
	ULONG PartialSum,
	PUSHORT Source,
	ULONG Length
	)

{

	while (Length--) {
		PartialSum += *Source++;
		PartialSum = (PartialSum >> 16) + (PartialSum & 0xffff);
	}


	return (USHORT)(((PartialSum >> 16) + PartialSum) & 0xffff);
}

PIMAGE_NT_HEADERS
	CheckSumMappedFile (
	LPVOID BaseAddress,
	DWORD FileLength,
	LPDWORD HeaderSum,
	LPDWORD CheckSum
	)
{

	PUSHORT AdjustSum;
	PIMAGE_NT_HEADERS NtHeaders;
	USHORT PartialSum;

	//
	// Compute the checksum of the file and zero the header checksum value.
	//

	*HeaderSum = 0;
	PartialSum = ChkSum(0, (PUSHORT)BaseAddress, (FileLength + 1) >> 1);

	//
	// If the file is an image file, then subtract the two checksum words
	// in the optional header from the computed checksum before adding
	// the file length, and set the value of the header checksum.
	//

	__try {
		PIMAGE_DOS_HEADER	pDosH;

		pDosH = (PIMAGE_DOS_HEADER)BaseAddress;

		NtHeaders = ( PIMAGE_NT_HEADERS)( (ULONG)pDosH + pDosH->e_lfanew );

	} __except(EXCEPTION_EXECUTE_HANDLER) {
		NtHeaders = NULL;
	}

	if ((NtHeaders != NULL) && (NtHeaders != BaseAddress)) {
		if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
			*HeaderSum = ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.CheckSum;
			AdjustSum = (PUSHORT)(&((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.CheckSum);
		} else
			if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
				*HeaderSum = ((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.CheckSum;
				AdjustSum = (PUSHORT)(&((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.CheckSum);
			} else {
				return(NULL);
			}
			PartialSum -= (PartialSum < AdjustSum[0]);
			PartialSum -= AdjustSum[0];
			PartialSum -= (PartialSum < AdjustSum[1]);
			PartialSum -= AdjustSum[1];
	}

	//
	// Compute the final checksum value as the sum of the paritial checksum
	// and the file length.
	//

	*CheckSum = (DWORD)PartialSum + FileLength;
	return NtHeaders;
}

CPeFile::CPeFile(void)
{
}


CPeFile::~CPeFile(void)
{
}

BOOL CPeFile::CalcCheckSum(LPCTSTR lpFileName)
{
	HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	CloseHandle(hFile);

	if ( hFileMap == NULL )
	{
		return FALSE;
	}

	LPVOID lpFileData = MapViewOfFile(hFileMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFileMap);

	if ( lpFileData == NULL )
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	DWORD dwHeaderSum, dwCheckSum;

	PIMAGE_DOS_HEADER lpDosHeader = (PIMAGE_DOS_HEADER)lpFileData;
	PIMAGE_NT_HEADERS lpNtHeader = (PIMAGE_NT_HEADERS)((ULONG)lpFileData + lpDosHeader->e_lfanew);

	lpNtHeader->OptionalHeader.CheckSum = 0;
	//	lpNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress = 0;
	//	lpNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size = 0;

	if ( CheckSumMappedFile(lpFileData, dwFileSize, &dwHeaderSum, &dwCheckSum) )
	{
		lpNtHeader->OptionalHeader.CheckSum = dwCheckSum;

		bResult = TRUE;
	}

	UnmapViewOfFile(lpFileData);
	return bResult;
}

ULONG CPeFile::GetConfigOffset(LPCTSTR lpFileName)
{
	ULONG uOffset = 0;

	HMODULE hModule = LoadLibraryEx(lpFileName, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hModule)
	{
		HRSRC hRes = FindResourceEx(hModule, CFG_RESTYPE, CFG_RESID, CFG_RESLANGID);
		if(hRes)
		{
			DWORD dwSize = SizeofResource(hModule, hRes);
			if(dwSize!=0)
			{
				HGLOBAL hResLoad = LoadResource(hModule, hRes);
				if(hResLoad)
				{
					LPVOID lpBuffer = LockResource(hResLoad);
					if(lpBuffer)
					{
						uOffset = ((DWORD)lpBuffer - ((DWORD)hModule & 0xFFFFFF00));
					}
				}
			}
		}

		FreeLibrary(hModule);
	}

	return uOffset;
}

BOOL  CPeFile::MyUpdateResource(LPCTSTR lpFileName, LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage, LPVOID lpData, DWORD cbData)
{
	BOOL bRet = FALSE;
	HANDLE hUpdate = BeginUpdateResource(lpFileName, FALSE);

	if(hUpdate)
	{
		bRet = UpdateResource(hUpdate, lpType, lpName, wLanguage, lpData, cbData);
		EndUpdateResource(hUpdate, !bRet);
	}

	return bRet;
}

BOOL CPeFile::_MyUpdateResource(LPCTSTR lpFileName, LPVOID lpData, DWORD cbData)
{
	return MyUpdateResource(lpFileName, CFG_RESTYPE, CFG_RESID, CFG_RESLANGID, lpData, cbData);
}

BOOL CPeFile::RestoreOverlayData(LPCTSTR lpFileName, LPCTSTR lpBackFileName)
{
	BOOL bRet = FALSE;
	HANDLE hBakFile = CreateFile(lpBackFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( hBakFile != INVALID_HANDLE_VALUE )
	{
		IMAGE_DOS_HEADER DosHeader;
		IMAGE_NT_HEADERS NtHeader;
		IMAGE_SECTION_HEADER SectHeader;
		DWORD dwBytes, dwFileSize;
		ULONG lPointer;

		ReadFile(hBakFile, &DosHeader, sizeof (DosHeader), &dwBytes, NULL);
		SetFilePointer(hBakFile, DosHeader.e_lfanew, NULL, FILE_BEGIN);
		ReadFile(hBakFile, &NtHeader, sizeof (NtHeader), &dwBytes, NULL);

		SetFilePointer(hBakFile, NtHeader.FileHeader.SizeOfOptionalHeader - \
			sizeof (IMAGE_OPTIONAL_HEADER) + sizeof (IMAGE_SECTION_HEADER) * (NtHeader.FileHeader.NumberOfSections - 1), NULL, FILE_CURRENT);

		ReadFile(hBakFile, &SectHeader, sizeof (IMAGE_SECTION_HEADER), &dwBytes, NULL);
		lPointer = SectHeader.PointerToRawData + SectHeader.SizeOfRawData;

		dwFileSize = GetFileSize(hBakFile, NULL);
		if ( lPointer < dwFileSize )
		{
			// ´æÔÚ Overlaydata	
			HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if ( hFile != INVALID_HANDLE_VALUE )
			{
				BYTE cBuffer[512];

				SetFilePointer(hBakFile, lPointer, NULL, FILE_BEGIN);
				SetFilePointer(hFile, 0, NULL, FILE_END);

				while ( lPointer < dwFileSize &&
					ReadFile(hBakFile, cBuffer, 512, &dwBytes, NULL) &&
					WriteFile(hFile, cBuffer, dwBytes, &dwBytes, NULL)
					)
				{
					lPointer += dwBytes;
				}

				CloseHandle(hFile);

				bRet = TRUE;
			}
		}
		else
		{
			bRet = TRUE;
		}

		CloseHandle(hBakFile);
	}

	return bRet;
}

BOOL CPeFile::_MyUpdateResource2(LPCTSTR lpFileName, ULONG uOffset, LPVOID lpData,DWORD cbData)
{
	BOOL bRet = FALSE;
	HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		if( INVALID_SET_FILE_POINTER != SetFilePointer(hFile, uOffset, NULL, FILE_BEGIN) )
		{
			DWORD dwBytes = 0;
			if( WriteFile(hFile, lpData, cbData, &dwBytes, NULL) &&
				dwBytes == cbData )
			{
				bRet = TRUE;
			}
		}

		CloseHandle(hFile);
	}

	return bRet;
}