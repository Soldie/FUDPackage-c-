#include "stdafx.h"
#include "Base.h"
BOOL ReadInFile(LPCTSTR lpszFile, PVOID *outBuffer, UINT *outFileSize)
{
	PVOID m_FileData;
	UINT m_FileSize;
	BOOL bRet = FALSE;
	HANDLE hFile = CreateFile(lpszFile, FILE_ALL_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		//把文件读出来
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		DWORD dwRet = 0;
		m_FileData = (PVOID)new BYTE[dwFileSize];
		m_FileSize = dwFileSize;
		if (ReadFile(hFile, m_FileData, dwFileSize, &dwRet, NULL))
		{
			bRet = TRUE;
			*outFileSize = m_FileSize;
			*outBuffer = m_FileData;
		}
		CloseHandle(hFile);
	}
	return bRet;
}
BOOL WriteToFile(LPCTSTR lpszFileName, PVOID Buffer, UINT nBufferSize)
{
	VMProtectBegin(__FUNCTION__);
	BOOL bRet = FALSE;
	HANDLE hFile = CreateFile(lpszFileName, FILE_ALL_ACCESS, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		if (WriteFile(hFile, Buffer, nBufferSize, &dwRet, NULL))
		{
			bRet = TRUE;
		}
		CloseHandle(hFile);
	}
	VMProtectEnd();
	return bRet;
	
}