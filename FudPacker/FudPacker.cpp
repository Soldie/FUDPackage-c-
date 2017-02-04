// FudPacker.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "FudPacker.h"
#include "..\FudPackageLib\Base.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	cmd_analyer();
	return 0;
}