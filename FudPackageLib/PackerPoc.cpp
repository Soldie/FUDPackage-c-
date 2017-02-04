#include "stdafx.h"
#include "Base.h"

BOOL bind_file(LPTSTR lpszExeFile1, LPCTSTR lpszExeFile2, LPCTSTR lpszOutFile)
{
	BOOL bRet = FALSE;
	PVOID pExeData1 = NULL;
	UINT nExeSize1 = 0;
	PVOID pExeData2 = NULL;
	UINT nExeSize2 = 0;
	if (ReadInFile(lpszExeFile1, &pExeData1, &nExeSize1)
		&& ReadInFile(lpszExeFile2, &pExeData2, &nExeSize2))
	{
		PVOID pBuffer = NULL;
		PVOID pExeLdrData1 = NULL;
		UINT nExeLdrSize1 = 0;
		PVOID pExeLdrData2 = NULL;
		UINT nExeLdrSize2 = 0;
		if (Pe2LdrFile(pExeData1, &pExeLdrData1, nExeSize1, &nExeLdrSize1)
			&& Pe2LdrFile(pExeData2, &pExeLdrData2, nExeSize2, &nExeLdrSize2))
		{
			PVOID pakExe1 = NULL;
			SIZE_T npakExeSize1 = 0;
			PVOID pakExe2 = NULL;
			SIZE_T npakExeSize2 = 0;
			if (pak_compress(pExeLdrData1, nExeLdrSize1, &pakExe1, &npakExeSize1)
				&& pak_compress(pExeLdrData2, nExeLdrSize2, &pakExe2, &npakExeSize2))
			{
				CONFIG_DATA data = { 0 };
				data.TotalSize = sizeof(CONFIG_DATA) + npakExeSize1 + npakExeSize2;
				data.PakSize1 = npakExeSize1;
				data.PakSize2 = npakExeSize2;
				data.dwCmd = CMD_BINDER;
				auto pBuffer = malloc(data.TotalSize);
				if (pBuffer)
				{
					RtlCopyMemory(pBuffer, &data, sizeof(CONFIG_DATA));
					RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA)), pakExe1, npakExeSize1);
					RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA) + npakExeSize1), pakExe2, npakExeSize2);
					//继续working
					CConfigData cfg;
					bRet = cfg.SaveConfigData(lpszOutFile, pBuffer, data.TotalSize);
					free(pBuffer);
				}
			}
			if (pakExe1)
				VirtualFree(pakExe1, 0, MEM_RELEASE);
			if (pakExe2)
				VirtualFree(pakExe2, 0, MEM_RELEASE);
		}
		if (pExeLdrData1)
			free(pExeLdrData1);
		if (pExeLdrData2)
			free(pExeLdrData2);

	}
	if (pExeData1)
		free(pExeData1);
	if (pExeData2)
		free(pExeData2);
	return bRet;
}

BOOL pack_file(LPTSTR lpszExeFile1, LPCTSTR lpszExeFile2, LPCTSTR lpszOutFile)
{
	BOOL bRet = FALSE;
	PVOID pExeData1 = NULL;
	UINT nExeSize1 = 0;
	PVOID pExeData2 = NULL;
	UINT nExeSize2 = 0;
	if (ReadInFile(lpszExeFile1, &pExeData1, &nExeSize1) 
		&& ReadInFile(lpszExeFile2, &pExeData2, &nExeSize2))
	{
		PVOID pBuffer = NULL;
		PVOID pExeLdrData1 = NULL;
		UINT nExeLdrSize1 = 0;
		PVOID pExeLdrData2 = NULL;
		UINT nExeLdrSize2 = 0;
		if (Pe2LdrFile(pExeData1, &pExeLdrData1, nExeSize1, &nExeLdrSize1)
			&& Pe2LdrFile(pExeData2, &pExeLdrData2, nExeSize2, &nExeLdrSize2))
		{
			PVOID pakExe1 = NULL;
			SIZE_T npakExeSize1 = 0;
			PVOID pakExe2 = NULL;
			SIZE_T npakExeSize2 = 0;
			if (pak_compress(pExeLdrData1, nExeLdrSize1, &pakExe1, &npakExeSize1)
				&& pak_compress(pExeLdrData2, nExeLdrSize2, &pakExe2, &npakExeSize2))
			{
				CONFIG_DATA data = { 0 };
				data.TotalSize = sizeof(CONFIG_DATA) + npakExeSize1 + npakExeSize2;
				data.PakSize1 = npakExeSize1;
				data.PakSize2 = npakExeSize2;
				data.dwCmd = CMD_EXE;
				auto pBuffer = malloc(data.TotalSize);
				if (pBuffer)
				{
					RtlCopyMemory(pBuffer, &data, sizeof(CONFIG_DATA));
					RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA)), pakExe1, npakExeSize1);
					RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA) + npakExeSize1), pakExe2, npakExeSize2);
					//继续working
					CConfigData cfg;
					bRet = cfg.SaveConfigData(lpszOutFile, pBuffer, data.TotalSize);
					free(pBuffer);
				}
			}
			if (pakExe1)
				VirtualFree(pakExe1,0,MEM_RELEASE);
			if (pakExe2)
				VirtualFree(pakExe2,0,MEM_RELEASE);
		}
		if(pExeLdrData1)
			free(pExeLdrData1);
		if(pExeLdrData2)
			free(pExeLdrData2);
		
	}
	if(pExeData1)
		free(pExeData1);
	if(pExeData2)
		free(pExeData2);
	return bRet;
}
BOOL pack_file_dll(LPTSTR lpszDllFile, LPCTSTR lpszTarget, LPCTSTR lpszOutFile)
{
	BOOL bRet = FALSE;
	PVOID pExeData1 = NULL;
	UINT nExeSize1 = 0;
	if (ReadInFile(lpszDllFile, &pExeData1, &nExeSize1))
	{
		PVOID pBuffer = NULL;
		PVOID pExeLdrData1 = NULL;
		UINT nExeLdrSize1 = 0;
		if (Pe2LdrFile(pExeData1, &pExeLdrData1, nExeSize1, &nExeLdrSize1))
		{
			PVOID pakExe1 = NULL;
			SIZE_T npakExeSize1 = 0;
			if (pak_compress(pExeLdrData1, nExeLdrSize1, &pakExe1, &npakExeSize1))
			{
				CONFIG_DATA data = { 0 };
				data.TotalSize = sizeof(CONFIG_DATA) + npakExeSize1;
				data.PakSize1 = npakExeSize1;
				data.PakSize2 = 0;
				data.dwCmd = CMD_DLL;
				strncpy(data.szInjectTaget, lpszTarget,MAX_PATH);
				auto pBuffer = malloc(data.TotalSize);
				if (pBuffer)
				{
					RtlCopyMemory(pBuffer, &data, sizeof(CONFIG_DATA));
					RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA)), pakExe1, npakExeSize1);
					//RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA) + npakExeSize1), pakExe2, npakExeSize2);
					//继续working
					CConfigData cfg;
					bRet = cfg.SaveConfigData(lpszOutFile, pBuffer, data.TotalSize);
					free(pBuffer);
				}
			}
			if (pakExe1)
				VirtualFree(pakExe1, 0, MEM_RELEASE);
		}
		if (pExeLdrData1)
			free(pExeLdrData1);
	}
	if (pExeData1)
		free(pExeData1);
	return bRet;
}
BOOL unpack_file_test(LPCTSTR lpszPackFile)
{
	CConfigData cfg;
	CONFIG_DATA cfgdata;
	BOOL bRet = FALSE;
	if (cfg.LoadConfig(lpszPackFile, &cfgdata, sizeof(CONFIG_DATA)))
	{
		if (cfgdata.TotalSize > sizeof(CONFIG_DATA))
		{
			auto pBuffer = malloc(cfgdata.TotalSize);
			if (pBuffer)
			{
				PVOID pakFile1 = NULL;
				PVOID pakFile2 = NULL;
				if (cfg.LoadConfig(lpszPackFile, pBuffer, cfgdata.TotalSize))
				{
					pakFile1 = (PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA));
					pakFile2 = (PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA) + cfgdata.PakSize1);
					PVOID ldrfile1 = NULL;
					PVOID ldrfile2 = NULL;
					SIZE_T ldrfile_size1 = 0;
					SIZE_T ldrfile_size2 = 0;
					if (pak_decompress(pakFile1, cfgdata.PakSize1, &ldrfile1, &ldrfile_size1)
						&& pak_decompress(pakFile2, cfgdata.PakSize2, &ldrfile2, &ldrfile_size2))
					{
						PVOID pefile1 = NULL;
						UINT pefile_size1 = 0;
						PVOID pefile2 = NULL;
						UINT pefile_size2 = 0;
						if (Ldr2PeFile(ldrfile1,&pefile1,ldrfile_size1,&pefile_size1)
							&& Ldr2PeFile(ldrfile2, &pefile2, ldrfile_size2, &pefile_size2))
						{
							WriteToFile(_T("1.exe"), pefile1, pefile_size1);
							WriteToFile(_T("2.exe"), pefile2, pefile_size2);
							bRet = TRUE;
						}
						if (pefile1)
						{
							free(pefile1);
						}
						if (pefile2)
						{
							free(pefile2);
						}
					}
					if (ldrfile1)
					{
						VirtualFree(ldrfile1, 0, MEM_RELEASE);
					}
					if (ldrfile2)
					{
						VirtualFree(ldrfile2, 0, MEM_RELEASE);
					}
				}
				free(pBuffer);
			}
		}
	}
	return bRet;
}
//BOOL PackFile(LPCWSTR lpszDllFile, LPCWSTR lpszExeFile, LPCWSTR lpszTargetFile)
//{
//	PVOID pBuffer;
//	PVOID pExeLdrData;
//	UINT nExeLdrSize;
//	PVOID pDllLdrData;
//	UINT nDllLdrSize;
//	PVOID pExeData;
//	UINT nExeSize;
//	PVOID pDllData;
//	UINT nDllSize;
//	CConfigData cfg;
//	CONFIG_DATA data;
//	BOOL bRet = FALSE;
//	if (ReadInFile(lpszDllFile, &pDllData, &nDllSize) && ReadInFile(lpszExeFile, &pExeData, &nExeSize))
//	{
//		OutputDebugStringA("ok read file\r\n");
//		if (Pe2LdrFile(pExeData, &pExeLdrData, nExeSize, &nExeLdrSize) && Pe2LdrFile(pDllData, &pDllLdrData, nDllSize, &nDllLdrSize))
//		{
//			OutputDebugStringA("ok conver file\r\n");
//			data.dwTotalSize = sizeof(CONFIG_DATA) + nDllLdrSize + nExeLdrSize;
//			data.dwDllFileSize = nDllLdrSize;
//			data.dwExeFileSize = nExeLdrSize;
//			pBuffer = malloc(data.dwTotalSize);
//			if (pBuffer)
//			{
//				RtlCopyMemory(pBuffer, &data, sizeof(CONFIG_DATA));
//				RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA)), pDllLdrData, nDllLdrSize);
//				RtlCopyMemory((PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA) + nDllLdrSize), pExeLdrData, nExeLdrSize);
//				//继续working
//				bRet = cfg.SaveConfigData(lpszTargetFile, pBuffer, data.dwTotalSize);
//				OutputDebugStringA("work with pe\r\n");
//				free(pBuffer);
//			}
//			free(pDllLdrData);
//			free(pExeLdrData);
//		}
//		free(pExeData);
//		free(pDllData);
//	}
//	return bRet;
//}

//BOOL UnPackFile(LPCWSTR lpszFileName, LPCWSTR lpszOutExeFile, LPCWSTR lpszOutDllFile)
//{
//	CConfigData cfg;
//	CONFIG_DATA cfgdata;
//	PVOID pBuffer;
//	PVOID pDllLdrBuffer;
//	PVOID pExeLdrBuffer;
//	PVOID pExePeBuffer;
//	PVOID pDllPeBuffer;
//	UINT nExePeSize;
//	UINT nDllPeSize;
//	BOOL bRet = FALSE;
//	if (cfg.LoadConfig(lpszFileName, &cfgdata, sizeof(CONFIG_DATA)))
//	{
//		if (cfgdata.TotalSize > sizeof(CONFIG_DATA))
//		{
//			//存在DLL
//			pBuffer = malloc(cfgdata.TotalSize);
//			if (pBuffer)
//			{
//				if (cfg.LoadConfig(lpszFileName, pBuffer, cfgdata.dwTotalSize))
//				{
//					pDllLdrBuffer = (PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA));
//					pExeLdrBuffer = (PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA) + cfgdata.dwDllFileSize);
//					if (Ldr2PeFile(pDllLdrBuffer, &pDllPeBuffer, cfgdata.dwDllFileSize, &nDllPeSize) && Ldr2PeFile(pExeLdrBuffer, &pExePeBuffer, cfgdata.dwExeFileSize, &nExePeSize))
//					{
//						WriteToFile(lpszOutDllFile, pDllPeBuffer, nDllPeSize);
//						WriteToFile(lpszOutExeFile, pExePeBuffer, nExePeSize);
//						free(pExePeBuffer);
//						free(pDllPeBuffer);
//						bRet = TRUE;
//					}
//				}
//				free(pBuffer);
//			}
//		}
//	}
//	return bRet;
//}