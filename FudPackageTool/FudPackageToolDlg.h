
// FudPackageToolDlg.h : 头文件
//

#pragma once
#include "BmpButton.h"

// CFudPackageToolDlg 对话框
class CFudPackageToolDlg : public CDialogEx
{
// 构造
public:
	CFudPackageToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUDPACKAGETOOL_DIALOG };
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
	afx_msg void OnBnClickedButton2();
public:
	TCHAR szExeFile[MAX_PATH];
	TCHAR szDllFile[MAX_PATH];
	TCHAR szOutFile[MAX_PATH];
	TCHAR szIconExeFile[MAX_PATH];
	TCHAR szInjectTargetName[MAX_PATH];
	TCHAR szSigFile[MAX_PATH];
	TCHAR szDatFile[MAX_PATH];
	afx_msg void OnBnClickedButton3();
	BOOL bExePack;
	afx_msg void OnBnClickedButton4();
	int flags;
	CRect m_rcCaption;	//标题栏的范围
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT OnMsgButton(WPARAM, LPARAM);
	CBmpButton m_btnClose;	//关闭按钮
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
