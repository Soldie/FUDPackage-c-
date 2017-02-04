
// BinderUIDlg.h : 头文件
//

#pragma once


// CBinderUIDlg 对话框
class CBinderUIDlg : public CDialogEx
{
// 构造
public:
	CBinderUIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BINDERUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
public:
	TCHAR szExeFile[MAX_PATH];
	TCHAR szOutFile[MAX_PATH];
	TCHAR szSrcExeFile[MAX_PATH];
	TCHAR szDatFile[MAX_PATH];
	TCHAR szBackDoorFile[MAX_PATH];
	int flags;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
};
