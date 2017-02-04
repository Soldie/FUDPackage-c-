#pragma once
class CPeFile
{
public:
	CPeFile(void);
	~CPeFile(void);
	BOOL CalcCheckSum(LPCTSTR lpFileName);
	ULONG GetConfigOffset(LPCTSTR lpFileName);
	BOOL MyUpdateResource(LPCTSTR lpFileName, LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage, LPVOID lpData, DWORD cbData);
	BOOL _MyUpdateResource(LPCTSTR lpFileName, LPVOID lpData, DWORD cbData);
	BOOL _MyUpdateResource2(LPCTSTR lpFileName, ULONG uOffset, LPVOID lpData,DWORD cbData);
	BOOL RestoreOverlayData(LPCTSTR lpFileName, LPCTSTR lpBackFileName);
};

