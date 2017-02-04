#pragma once
#include <process.h>
class CPassEmulator
{
public:
	CPassEmulator();
	~CPassEmulator();
	static	HHOOK hMouse;
	static	BOOL Clicked;
	static LRESULT CALLBACK MouseFunc(int nCode, WPARAM wParam, LPARAM lParam);
	static void __cdecl MouseThread(void* Args);
	static void ActivateAntiEmulator();
};

