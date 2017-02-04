#include "stdafx.h"
#include "Base.h"
#include "PassEmulator.h"

 HHOOK CPassEmulator::hMouse = 0;
 BOOL CPassEmulator::Clicked = FALSE;
CPassEmulator::CPassEmulator()
{
	
}

CPassEmulator::~CPassEmulator()
{
}

 LRESULT CALLBACK CPassEmulator::MouseFunc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	if (pMouseStruct)
	{
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
			CPassEmulator::Clicked = TRUE;
			break;
		case WM_RBUTTONDOWN:
			CPassEmulator::Clicked = TRUE;
			break;
		}
	}

	return CallNextHookEx(CPassEmulator::hMouse, nCode, wParam, lParam);
}

 void __cdecl CPassEmulator::MouseThread(void* Args)
{
	CPassEmulator::hMouse = SetWindowsHookEx(WH_MOUSE_LL, CPassEmulator::MouseFunc, GetModuleHandle(NULL), NULL);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (CPassEmulator::Clicked)break;
	}
	_endthread();
}
 void CPassEmulator::ActivateAntiEmulator()
{
	VMProtectBegin(__FUNCTION__);
	CPassEmulator::Clicked = FALSE;
	int counting_stars = 0;
	for (int i = 1; i < INT_MAX; i++)
	{
		for (int s = 1; s < INT_MAX; s++)
			if (i % 2 == 0)
				counting_stars++;
			else
				counting_stars--;
	}
	HANDLE hMouseThread = (HANDLE)_beginthread(CPassEmulator::MouseThread, NULL, NULL);

	while (CPassEmulator::Clicked != 1)
		Sleep(1000);
	PostThreadMessage(GetThreadId(hMouseThread), NULL, NULL, NULL);
	Sleep(120000);
	VMProtectEnd();
}