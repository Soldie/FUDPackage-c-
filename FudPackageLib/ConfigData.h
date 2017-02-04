#pragma once
class CConfigData
{
public:
	CConfigData();
	~CConfigData();
	BOOL LoadConfig(LPCTSTR lpszFileName, PVOID outBuffer, UINT nOutSize);
	BOOL SaveConfigData(LPCTSTR lpszFileName, PVOID InBuffer, UINT nInSize);
};

