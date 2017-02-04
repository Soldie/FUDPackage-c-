#include "stdafx.h"
#include "Base.h"

BOOL pak_compress(PVOID lpInBuffer, SIZE_T InBufferSize, PVOID *lpOutBuffer, SIZE_T *OutBufferSize)
{
	BOOL bRet = FALSE;
	ULONG MaxSize = 0;
	MaxSize = compressBound(InBufferSize);
	if (MaxSize!=0)
	{
		auto MemBuffer = VirtualAlloc(NULL, MaxSize, MEM_COMMIT, PAGE_READWRITE);
		if (MemBuffer)
		{
			RtlZeroMemory(MemBuffer, MaxSize);
			auto z_ret =compress((BYTE *)MemBuffer, &MaxSize, (const BYTE*)lpInBuffer, InBufferSize);
			if (z_ret==Z_OK)
			{
				bRet = TRUE;
			}
		}
		if (bRet)
		{
			*lpOutBuffer = MemBuffer;
			*OutBufferSize = MaxSize;
		}
		else
		{
			if (MemBuffer)
			{
				VirtualFree(MemBuffer, 0, MEM_RELEASE);
			}
		}
	}
	return bRet;
}

BOOL pak_decompress(PVOID lpInBuffer, SIZE_T InBufferSize, PVOID *lpOutBuffer, SIZE_T *OutBuffSize)
{
	VMProtectBegin(__FUNCTION__);
	BOOL bRet = FALSE;
	SIZE_T MaxSize = 0;
	MaxSize = InBufferSize ;
	while(1)
	{
		MaxSize = MaxSize * 2;
		auto MemBuff = VirtualAlloc(NULL, MaxSize, MEM_COMMIT, PAGE_READWRITE);
		if (MemBuff)
		{
			RtlZeroMemory(MemBuff, MaxSize);
			auto SizeOut = MaxSize;
			auto z_ret = uncompress((BYTE*)MemBuff, &SizeOut, (const BYTE *)lpInBuffer, InBufferSize);
			if (z_ret==Z_OK)
			{
				*lpOutBuffer = MemBuff;
				*OutBuffSize = SizeOut;
				bRet = TRUE;
				break;
			}
			else if (z_ret==Z_BUF_ERROR)
			{
				VirtualFree(MemBuff, 0, MEM_RELEASE);
			}
			else
			{
				VirtualFree(MemBuff, 0, MEM_RELEASE);
				break;
			}
		}
		else
		{
			break;
		}
	}
	VMProtectEnd();
	return bRet;
	
}