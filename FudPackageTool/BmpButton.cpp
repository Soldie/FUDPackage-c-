#include "stdafx.h"
#include "BmpButton.h"


//按钮状态数量
#define STATE_COUNT	4


//按钮状态
enum
{
	STATE_NONE = 0,	//当前鼠标不再按键上（常态）
	STATE_MOVE,		//当前鼠标在按键上
	STATE_DOWN,		//当前鼠标在按键上按下
	STATE_DISABLE,	//当前按键禁用
};

//鼠标状态
enum
{
	MOUSE_FLAGS_NONE = 0,			//初始状态
	MOUSE_FLAGS_LBUTTONDOWN_IN,		//鼠标左键按下, 且在按钮上状态
	MOUSE_FLAGS_LBUTTONDOWN_OUT,	//鼠标左键按下, 不在按钮上状态
};


//IMPLEMENT_DYNAMIC(CBmpButton, CWnd)
CBmpButton::CBmpButton()
{
	this->m_uState = 0;
	this->m_uFlags = 0;

	this->m_hButtonDC = NULL;
	this->m_hButtonBmp = NULL;
}

CBmpButton::~CBmpButton()
{
	if(this->m_hButtonDC){::DeleteDC(this->m_hButtonDC); this->m_hButtonDC=NULL;}
	if(this->m_hButtonBmp){::DeleteObject(this->m_hButtonBmp);this->m_hButtonBmp=NULL;}
}

//根据图片创建按钮，按钮大小根据图片大小自行计算
BOOL CBmpButton::CreateButton(CWnd*  pParentWnd		//父窗口句柄
				  , int x, int y					//按钮的x,y坐标
				  , LPCTSTR lpBmpPath				//图片地址（该版本只支持BMP格式）
				  , UINT uID)						//按钮ID
{
	//根据传入的图片地址，加载图片
	this->m_hButtonBmp = (HBITMAP)::LoadImage(NULL, lpBmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	return Create(NULL, NULL, WS_VISIBLE, CRect(x, y, x+1, y+1), pParentWnd, uID);
}


BEGIN_MESSAGE_MAP(CBmpButton, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


int CBmpButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HDC hDC = ::GetDC(this->m_hWnd);
	this->m_hButtonDC = ::CreateCompatibleDC(hDC);
	::SelectObject(this->m_hButtonDC, this->m_hButtonBmp);
	::ReleaseDC(this->m_hWnd, hDC);

	//获取图片大小
	BITMAP bm;
	::GetObject(this->m_hButtonBmp, sizeof(bm), &bm);
	this->m_szButton.cx = bm.bmWidth/STATE_COUNT;
	this->m_szButton.cy = bm.bmHeight;

	//不移动X,Y坐标，不改变Z序，不重绘，仅改变窗口大小
	SetWindowPos(NULL, 0, 0, this->m_szButton.cx, this->m_szButton.cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);

	return 0;
}


BOOL CBmpButton::OnEraseBkgnd(CDC *pDC)
{
	CRect rcWnd;
	this->GetClientRect(rcWnd);

	::BitBlt(pDC->m_hDC
		, 0, 0, this->m_szButton.cx, this->m_szButton.cy
		, this->m_hButtonDC
		, this->m_uState * this->m_szButton.cx
		, 0 ,SRCCOPY);

	return TRUE;
}

void CBmpButton::OnMouseMove(UINT ,CPoint pt)
{
	//这里需先判断当前鼠标是否在按钮上
	//因为按钮会设置“捕获”，所以可能鼠标没在按钮上也会触发该消息
	//只有WM_MOUSEMOVE 和 WM_LBUTTONUP 会有可能在鼠标不在按钮上会触发，余下的消息不会

	CRect rcClient;
	this->GetClientRect(rcClient);
	if( ::PtInRect(rcClient,pt) )
	{
		//首次移动到按钮上，需重绘
		if(STATE_NONE == this->m_uState && MOUSE_FLAGS_NONE == this->m_uFlags)
		{
			this->m_uState = STATE_MOVE;
			this->m_uFlags = MOUSE_FLAGS_NONE;
			this->Invalidate();
			return;
		}

		//鼠标在按钮上，鼠标左键又是按下状态
		//说明当前情况是，鼠标在“捕获”情况下，按下了按钮，未弹起鼠标左键，移动出了按钮范围，又再次移动回来
		//根据Windows按钮设计规范，此时按钮应重置为按下状态
		if(STATE_NONE == this->m_uState && MOUSE_FLAGS_LBUTTONDOWN_OUT == this->m_uFlags)
		{
			this->m_uState = STATE_DOWN;
			this->Invalidate();
			return;
		}
	}
	else if(STATE_DOWN == this->m_uState)
	{
		//鼠标不在按钮上，但是状态是左键按下，却又触发了这里
		//说明当前情况是，鼠标在按钮上按下后，未弹起，移动出按钮范围了
		//根据Windows按钮设计规范，此时按钮应显示为常态样式
		this->m_uFlags = MOUSE_FLAGS_LBUTTONDOWN_OUT;
		this->m_uState = STATE_NONE;
		this->Invalidate();
	}
	else if(STATE_NONE != this->m_uState)
	{
		this->m_uState = STATE_NONE;
		this->Invalidate();
	}

	//设置鼠标移出消息（必须设置，否则无法响应WM_MOUSELEAVE消息
	TRACKMOUSEEVENT te = {sizeof(TRACKMOUSEEVENT) ,TME_LEAVE ,m_hWnd, 1};
	::_TrackMouseEvent(&te);

}

void CBmpButton::OnMouseLeave()
{
	//鼠标离开了按钮范围
	//重要：设置捕获后，如果鼠标是按下状态移开按钮范围，不会触发这里
	this->m_uState = STATE_NONE;
	this->m_uFlags = MOUSE_FLAGS_NONE;

	this->Invalidate();
}

void CBmpButton::OnLButtonDown(UINT, CPoint pt)
{
	//设置鼠标捕获
	this->SetCapture();

	this->m_uState = STATE_DOWN;
	this->m_uFlags = MOUSE_FLAGS_LBUTTONDOWN_IN;

	this->Invalidate();
}

void CBmpButton::OnLButtonUp(UINT ,CPoint pt)
{
	//取消捕获
	::ReleaseCapture();

	CRect rcClient;
	this->GetClientRect(rcClient);

	//根据Windows按钮设计规范，按钮消息的投递应是在鼠标在按钮范围内弹起的时候投递
	//故此处需先判断下当前鼠标位置
	if(::PtInRect(rcClient,pt) && STATE_DOWN == this->m_uState)
	{
		::PostMessage(::GetParent(this->m_hWnd), MSG_BMP_BUTTON, this->GetDlgCtrlID(), NULL);
	}

	//所有状态置回默认值
	this->m_uState = STATE_NONE;
	this->m_uFlags = MOUSE_FLAGS_NONE;

	this->Invalidate();
}