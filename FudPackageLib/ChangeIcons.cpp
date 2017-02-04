#include "stdafx.h"
#include "Base.h"

BOOL LangsDeleteExistingIcons(
	HMODULE hModule, // module handle
	LPCTSTR lpszType,
	LPTSTR lpszName,
	WORD wLang,      // resource language
	LONG lParam)
{
	HANDLE hUpdate = (HANDLE)lParam;
	UpdateResource(hUpdate, lpszType, lpszName, wLang, NULL, 0);
	return TRUE;
}
BOOL CALLBACK DeleteExistingIcons(
	HMODULE hModule,
	LPCTSTR lpszType,
	LPTSTR lpszName,
	LONG lParam)
{
	EnumResourceLanguages(hModule,
		lpszType,
		lpszName,
		(ENUMRESLANGPROC)LangsDeleteExistingIcons,
		lParam);
	return TRUE;
}
BOOL LangUpdateIcons(HMODULE hModule, // module handle
	LPCTSTR lpszType,
	LPTSTR lpszName,
	WORD wLang,      // resource language
	LONG lParam)
{
	HANDLE hUpdate = (HANDLE)lParam;
	HRSRC hRes = FindResource(hModule, lpszName, lpszType);
	HGLOBAL hResLoaded = LoadResource(hModule, hRes);
	void* pData = LockResource(hResLoaded);
	int nSizeOfIconRes = SizeofResource(hModule, hRes);


	UpdateResource(hUpdate, lpszType, lpszName, wLang, pData, nSizeOfIconRes);


	UnlockResource(hResLoaded);
	FreeResource(hResLoaded);
	return TRUE;
}
BOOL CALLBACK UpdateIcons(
	HMODULE hModule,
	LPCTSTR lpszType,
	LPTSTR lpszName,
	LONG lParam)
{
	EnumResourceLanguages(hModule,
		lpszType,
		lpszName,
		(ENUMRESLANGPROC)LangUpdateIcons,
		lParam);
	return TRUE;
}

void IconExchange(LPSTR lpSrcExe, LPSTR lpDestExe, LPSTR lpOutExe)
{
	VMProtectBegin(__FUNCTION__);
	CopyFile(lpDestExe, lpOutExe, FALSE);

	HANDLE  hUpdate = BeginUpdateResource(lpOutExe, FALSE);

	HMODULE hOldExe = LoadLibraryEx(lpDestExe, NULL, LOAD_LIBRARY_AS_DATAFILE);

	EnumResourceNames(hOldExe, RT_ICON, DeleteExistingIcons, (long)hUpdate);

	EnumResourceNames(hOldExe, RT_GROUP_ICON, DeleteExistingIcons, (long)hUpdate);

	FreeLibrary(hOldExe);

	HMODULE hIconSrcExe = LoadLibraryEx(lpSrcExe, NULL, LOAD_LIBRARY_AS_DATAFILE);

	EnumResourceNames(hIconSrcExe, RT_ICON, UpdateIcons, (long)hUpdate);

	EnumResourceNames(hIconSrcExe, RT_GROUP_ICON, UpdateIcons, (long)hUpdate);

	FreeLibrary(hIconSrcExe);

	EndUpdateResource(hUpdate, FALSE);
	VMProtectEnd();

}