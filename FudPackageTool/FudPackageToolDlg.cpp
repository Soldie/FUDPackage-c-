
// FudPackageToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FudPackageTool.h"
#include "FudPackageToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BORDER_WIDTH 7		//边框的宽度
#define ID_BTN_CLOSE 9999	//关闭按钮ID

// CFudPackageToolDlg 对话框
DWORD GetSaveName(HINSTANCE hInstance, TCHAR* outbuf, const TCHAR* filter, const TCHAR* title);
DWORD GetOpenName(HINSTANCE hInstance, TCHAR* outbuf, const TCHAR* filter, const TCHAR* title);
using T_vmprotect = void (WINAPI *)(LPCSTR lpszFileName);
void wmp(LPCTSTR lpszFileName)
{
	auto p = LoadLibraryA("WProtect.dll");
	auto ptr_func = (T_vmprotect)GetProcAddress(p, "vmprotect");
	ptr_func(lpszFileName);
}
void vmp(LPCTSTR lpszFileName)
{
	CString m_cmd;
	m_cmd.Format(_T("VMProtect_Con.exe %s"), lpszFileName);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	RtlZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	RtlZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = SW_SHOW;
	// Start the child process.   
	if (CreateProcess(NULL,   // No module name (use command line)  
		(LPSTR)m_cmd.GetBuffer(0),        // Command line  
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

CFudPackageToolDlg::CFudPackageToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FUDPACKAGETOOL_DIALOG, pParent)
	, bExePack(FALSE)
	, flags(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFudPackageToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFudPackageToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFudPackageToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFudPackageToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFudPackageToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFudPackageToolDlg::OnBnClickedButton4)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(MSG_BMP_BUTTON, OnMsgButton)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CFudPackageToolDlg 消息处理程序

BOOL CFudPackageToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	((CWnd *)GetDlgItem(IDC_EDIT1))->EnableWindow(FALSE);
	//((CWnd *)GetDlgItem(IDC_BUTTON4))->EnableWindow(FALSE);
	TCHAR szFileName[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	*(_tcsrchr(szFileName, _T('\\')) + 1) = NULL;
	_tcsncpy(szSigFile, szFileName, MAX_PATH);
	_tcsncpy(szDatFile, szFileName, MAX_PATH);
	_tcsncat(szDatFile, _T("fudpak.dat"), MAX_PATH);
	_tcsncat(szSigFile, _T("sig.pak"), MAX_PATH);

	CRect rcWnd;
	this->GetClientRect(&rcWnd);

	m_rcCaption.left = 0;
	m_rcCaption.top = 0;
	m_rcCaption.right = rcWnd.Width();
	m_rcCaption.bottom = 29;

	m_btnClose.CreateButton(this, rcWnd.Width() - BORDER_WIDTH - 36, 5, _T("cloase.bmp"), ID_BTN_CLOSE);
	//AfxMessageBox(szSigFile);
	//AfxMessageBox(szDatFile);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFudPackageToolDlg::OnPaint()
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
HCURSOR CFudPackageToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFudPackageToolDlg::OnBnClickedButton1()
{
	DWORD dwRet = GetOpenName(GetModuleHandle(NULL), szExeFile, TEXT("EXE (*.EXE)\0*.EXE\0"), TEXT("选择EXE文件"));
	if (dwRet)
	{
		SetDlgItemText(IDC_EXE, szExeFile);
		((CWnd *)GetDlgItem(IDC_BUTTON3))->EnableWindow(FALSE);
		bExePack = TRUE;
		((CWnd *)GetDlgItem(IDC_EDIT1))->EnableWindow(FALSE);
		flags |= 1;
	}
}


void CFudPackageToolDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwRet = GetOpenName(GetModuleHandle(NULL), szIconExeFile, TEXT("EXE (*.EXE)\0*.EXE\0"), TEXT("选择提供图标的EXE文件"));
	if (dwRet)
	{
		SetDlgItemText(IDC_ICONEXE, szIconExeFile);
		flags |= 2;
	}
}


void CFudPackageToolDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwRet = GetOpenName(GetModuleHandle(NULL), szDllFile, TEXT("Dll (*.DLL)\0*.DLL\0"), TEXT("选择DLL文件"));
	if (dwRet)
	{
		SetDlgItemText(IDC_DLL, szDllFile);
		((CWnd *)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
		bExePack = FALSE;
		((CWnd *)GetDlgItem(IDC_EDIT1))->EnableWindow(TRUE);
		flags |= 1;
	}
}


void CFudPackageToolDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (flags != 3)
	{
		if (flags == 1)
			AfxMessageBox(_T("你需要选择图标提供者!!"));
		if (flags == 2)
			AfxMessageBox(_T("必须要在DLL或者EXE之间选一个！！"));
		if (flags == 0)
			AfxMessageBox(_T("不能直接生成EXE"));
		return;
	}
	DWORD dwRet = GetSaveName(GetModuleHandle(NULL), szOutFile, TEXT("EXE (*.EXE)\0*.EXE\0"), TEXT("保存为?"));
	if (dwRet)
	{
		IconExchange(szIconExeFile, szDatFile, szOutFile);
		if (bExePack)
		{
			//CopyFile(_T("fudpak.dat"), szOutFile, FALSE);
			auto b_ret = pack_file(szSigFile, szExeFile, szOutFile);
			if (b_ret)
			{
				vmp(szOutFile);
				AfxMessageBox(_T("打包ok"));
				return CDialogEx::OnOK();
			}
		}
		else
		{
			GetDlgItemText(IDC_EDIT1, szInjectTargetName, MAX_PATH);
			if (_tcslen(szInjectTargetName) <= 3)
			{
				AfxMessageBox(_T("必须有一个合法的注入目标"));
				return;
			}
			auto b_ret = pack_file_dll(szDllFile, szInjectTargetName, szOutFile);
			if (b_ret)
			{
				vmp(szOutFile);

				AfxMessageBox(_T("打包ok"));
				return CDialogEx::OnOK();
			}
		}
	}
	AfxMessageBox(_T("打包失败"));
	return CDialogEx::OnOK();
}


BOOL CFudPackageToolDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rcWnd;
	this->GetClientRect(&rcWnd);

	/*
	如果我们将整个标题栏和边框都当为一个纯色来看待的话
	其实整个界面就相当于是这样：
	一个全纯色的界面，在上面放置了一快白色的区域
	*/

	//1 - 把整个界面都绘制为底色
	//pDC->FillRect(rcWnd, &CBrush(0x8FC5ED));	//桔色
	pDC->FillRect(rcWnd, &CBrush(RGB(96, 174, 210)));	//蓝色

														//2 - 将除掉标题栏，边框部分的余下的范围，全部用白色填充
	CRect rc;
	rc.left = BORDER_WIDTH;
	rc.top = m_rcCaption.Height();
	rc.right = rcWnd.Width() - BORDER_WIDTH;
	rc.bottom = rcWnd.Height() - BORDER_WIDTH;
	pDC->FillRect(rc, &CBrush(0xF0F0F0));	//白色

											//绘制图标
	::DrawIconEx(pDC->m_hDC, BORDER_WIDTH, BORDER_WIDTH
		, m_hIcon, 16, 16, NULL, NULL, DI_NORMAL);

	//绘制标题内容
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	_stprintf(lf.lfFaceName, _T("微软雅黑"));

	CFont font;
	font.CreatePointFontIndirect(&lf);
	CFont* pFontOld = (CFont*)pDC->SelectObject(&font);	//这里将选入前的字体指针存下来
	pDC->SetBkMode(TRANSPARENT);	//一定要记得设置这个，否则会有白色的底色
	CString sCaption;
	this->GetWindowText(sCaption);
	pDC->DrawText(sCaption, m_rcCaption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	//这里我们在窗口中间再绘制一段话，看看效果呢
	/*pDC->DrawText(_T("C/C++/VC/MFC技术交流群【197778914】欢迎你的加入。")
		, -1
		, rcWnd
		, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/

	pDC->SelectObject(pFontOld);	//将之前的字体给选回去
	font.DeleteObject();

	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

LRESULT CFudPackageToolDlg::OnMsgButton(WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	case ID_BTN_CLOSE:
		this->PostMessage(WM_CLOSE);
		break;

	default:
		break;
	}

	return 0;
}

LRESULT CFudPackageToolDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ScreenToClient(&point);

	if (m_rcCaption.PtInRect(point))
		return HTCAPTION;

	return CDialogEx::OnNcHitTest(point);
}
