/*********************************************************************************************
这是基于CWnd类重写的按钮类，该版本仅支持BMP文件的图片资源
该类实现的图片按钮，遵循微软的按钮标准
图片资源格式：需横向排列，大小相等的4个按钮状态（可参考示例代码中的资源文件）
按钮状态：普通-鼠标移上-鼠标按钮-禁用
该类为演示教学用，仅实现了最基本的内容，后续内容可自行增加
**********************************************************************************************/


/*********************************************************************************************

C/C++/MFC/VC技术交流学习群【197778914】欢迎您的加入！

**********************************************************************************************/


#pragma once


#define MSG_BMP_BUTTON	(WM_USER + 1001)	//按钮消息


//////////////////////////////////////////////////////////////////////////////////////////////////
//Class's
class CBmpButton : public CWnd
{
public:
	CBmpButton();
	virtual ~CBmpButton();

public:
	//根据图片创建按钮，按钮大小根据图片大小自行计算
	BOOL CreateButton(CWnd*  pParentWnd	//父窗口句柄
		, int x, int y					//按钮的x,y坐标
		, LPCTSTR lpBmpPath				//图片地址（该版本只支持BMP格式）
		, UINT uID);					//按钮ID

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT ,CPoint);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()

private:
	UINT    m_uState;		//当前按钮状态
	UINT    m_uFlags;		//当前鼠标状态

	SIZE	m_szButton;		//按钮大小

	HDC     m_hButtonDC;	//按钮图片DC
	HBITMAP m_hButtonBmp;	//按钮图片BMP
};