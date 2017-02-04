#include "stdafx.h"
#include "Base.h"

PTEB_ACTIVE_FRAME FindThreadFrameByContext(ULONG_PTR Context)
{
	PTEB_ACTIVE_FRAME Frame;

	Frame = pRtlGetFrame();
	while (Frame != NULL && Frame->Context != Context)
		Frame = Frame->Previous;

	return Frame;
}

LOAD_MEM_DLL_INFO* GetLoadMemDllInfo()
{
	return (LOAD_MEM_DLL_INFO *)FindThreadFrameByContext(LOAD_MEM_DLL_INFO_MAGIC);
}
