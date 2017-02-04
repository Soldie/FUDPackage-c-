// PERsrcBuilder.cpp: implementation of the CPERsrcBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Base.h"
#include "PERsrcBuilder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CPERsrc

CPERsrc::CPERsrc()
{
	m_hModule = NULL;
	m_lpNtH = NULL;
	m_lpSectH = NULL;

	m_szFilePath[0] = 0;
}

CPERsrc::~CPERsrc()
{
	Close();
}

BOOL CPERsrc::Open(LPCTSTR lpFileName)
{
	if ( m_hModule == NULL )
	{
		_tcscpy_s(m_szFilePath, lpFileName);
		return ReLoad();
	}
	
	return FALSE;
}

BOOL CPERsrc::ReLoad()
{
	if ( m_hModule == NULL )
	{
		m_hModule = LoadLibraryEx(m_szFilePath, NULL, LOAD_LIBRARY_AS_DATAFILE);
		if ( m_hModule != NULL )
		{
			PIMAGE_DOS_HEADER lpDosH = (PIMAGE_DOS_HEADER)GetBaseAddr();

			m_lpNtH = (PIMAGE_NT_HEADERS)((ULONG)lpDosH + lpDosH->e_lfanew);
			m_lpSectH = IMAGE_FIRST_SECTION(m_lpNtH);

			return TRUE;
		}
	}
	
	return FALSE;
}

void CPERsrc::Close()
{
	if ( m_hModule != NULL )
	{
		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}

	m_lpNtH = NULL;
	m_lpSectH = NULL;
}

DWORD CPERsrc::Rva2Raw(DWORD dwRva) const
{
//	ATLASSERT(m_lpNtH != NULL);
//	ATLASSERT(m_lpSectH != NULL);

	if ( dwRva < m_lpSectH->VirtualAddress )
	{
		return dwRva;
	}

	for ( DWORD i = 0; i < m_lpNtH->FileHeader.NumberOfSections; i++ )
	{
		DWORD dwLimitAddr;	
		if ( i + 1 == m_lpNtH->FileHeader.NumberOfSections ) {
			dwLimitAddr = m_lpNtH->OptionalHeader.SizeOfImage;
		}
		else
		{
			dwLimitAddr = m_lpSectH[i + 1].VirtualAddress;
		}

		if ( dwRva >= m_lpSectH[i].VirtualAddress && dwRva < dwLimitAddr )
		{
			return dwRva - m_lpSectH[i].VirtualAddress + m_lpSectH[i].PointerToRawData;
		}
	}

	return (DWORD)-1;
}
