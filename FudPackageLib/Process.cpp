#include "stdafx.h"
#include "Base.h"
#include <strsafe.h>
VOID GetProcessNameById(DWORD ProcessId, LPTSTR lpszOutName)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,
		FALSE, ProcessId);

	// Get the process name.

	if (NULL != hProcess)
	{
		DWORD dwSize = sizeof(szProcessName);
		if (!QueryFullProcessImageName(hProcess, 0, szProcessName, &dwSize))
		{
			//printf("Get Image Name %d\r\n", GetLastError());
		}
	}
	else
	{
		//printf("SomeThing pid = %d,%d\r\n", ProcessId, GetLastError());
	}


	//_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);
	StringCbCopy(lpszOutName, MAX_PATH, szProcessName);

	//LogPrint("ImageName = %ws\r\n", lpszOutName);
	CloseHandle(hProcess);
}
void GetAllProcess(LPCTSTR lpszName,std::map<DWORD,DWORD> &pid_map)
{
	DWORD aProcess[0x1024] = { 0 };
	DWORD cbNeed = 0;
	DWORD dwRet = (DWORD)-1;
	if (EnumProcesses(aProcess, sizeof(aProcess), &cbNeed))
	{
		DWORD Count = 0;
		Count = cbNeed / sizeof(DWORD);
		for (DWORD i = 0; i < Count; i++)
		{
			if (aProcess[i])
			{
				TCHAR szName[MAX_PATH] = { 0 };
				LPTSTR szName2 = NULL;
				GetProcessNameById(aProcess[i], szName);
				szName2 = PathFindFileName(szName);
				//OutputDebugString(szName2);
				//OutputDebugString(TEXT("\r\n"));
				if (_tcsicmp(szName2, lpszName) == 0)
				{
					dwRet = aProcess[i];
					if (pid_map.find(dwRet) == pid_map.end())
					{
						//printf("%d\r\n",dwRet);
						pid_map[dwRet] = 1;
					}
					//break;
				}
			}
		}
	}
	return;
}