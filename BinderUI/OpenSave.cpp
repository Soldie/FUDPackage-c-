#include "stdafx.h"


DWORD GetOpenName(HINSTANCE hInstance, TCHAR* outbuf, const TCHAR* filter, const TCHAR* title)
{
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	TCHAR buf[MAX_PATH + 2];
	GetModuleFileName(hInstance, buf, 260);

	TCHAR* tmp = StrRChr(buf, NULL, L'\\');
	if (tmp != 0)
	{
		*tmp = 0;
		ofn.lpstrInitialDir = buf;
	}

	ofn.hInstance = hInstance;
	ofn.hwndOwner = NULL;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = outbuf;
	ofn.lpstrFile[0] = 0;
	ofn.lpstrFile[1] = 0;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST;

	return GetOpenFileName(&ofn);
}

DWORD GetSaveName(HINSTANCE hInstance, TCHAR* outbuf, const TCHAR* filter, const TCHAR* title)
{
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	TCHAR buf[MAX_PATH + 2];
	GetModuleFileName(hInstance, buf, 260);

	TCHAR* tmp = StrRChr(buf, NULL, L'\\');
	if (tmp != 0)
	{
		*tmp = 0;
		ofn.lpstrInitialDir = buf;
	}

	ofn.hInstance = hInstance;
	ofn.hwndOwner = NULL;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = outbuf;
	ofn.lpstrFile[0] = 0;
	ofn.lpstrFile[1] = 0;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST;

	return GetSaveFileName(&ofn);
}
