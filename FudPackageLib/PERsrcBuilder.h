// PERsrcBuilder.h: interface for the CPERsrcBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERSRCBUILDER_H__160BB411_7EB7_4633_BD28_0BB8FAB3C2B1__INCLUDED_)
#define AFX_PERSRCBUILDER_H__160BB411_7EB7_4633_BD28_0BB8FAB3C2B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ATLASSERT
#define ATLASSERT(expr) _ASSERTE(expr)
#endif // ATLASSERT

class CPERsrc
{
public:
	CPERsrc();
	~CPERsrc();

	BOOL Open(LPCTSTR lpFileName);
	void Close();

	LPVOID GetBaseAddr() const 
	{
		//ATLASSERT(IsOpen());
		return (LPVOID)((ULONG)m_hModule & ~1);
	}

	BOOL HasRsrc() const
	{
		//ATLASSERT(IsOpen());
		return m_lpNtH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress != 0;
	}
	
	LPVOID GetRsrcBaseAddr() const
	{
		//ATLASSERT(IsOpen());
		return (LPVOID)((ULONG)GetBaseAddr() + \
			Rva2Raw(m_lpNtH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress));
	}
	
	UINT GetRsrcSize() const 
	{
		///ATLASSERT(IsOpen());
		return m_lpNtH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	}

	DWORD Rva2Raw(DWORD dwRva) const;
	
protected:
	BOOL ReLoad();
	
	BOOL IsOpen() const { return m_hModule != NULL; }

	TCHAR	m_szFilePath[MAX_PATH];

	HMODULE m_hModule;
	PIMAGE_NT_HEADERS m_lpNtH;
	PIMAGE_SECTION_HEADER m_lpSectH;
	
};

#endif // !defined(AFX_PERSRCBUILDER_H__160BB411_7EB7_4633_BD28_0BB8FAB3C2B1__INCLUDED_)
