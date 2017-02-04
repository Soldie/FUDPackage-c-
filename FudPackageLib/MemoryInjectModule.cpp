#include "stdafx.h"
#include "Base.h"
#include "StealthInject.h"

bool MemoryInjectModule(DWORD dwProcessId, void* address)
{
	VMProtectBegin(__FUNCTION__);
	GetAllPrivilege();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessId);
	if (hProcess == NULL)
	{
		return false;
	}
	CStealthInject ss;
	auto ret = ss.StealthLoadLibraryMemory(hProcess, address);
	VMProtectEnd();
	return ret;
	
}