
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "APK.h"

#include "MainFrm.h"
#include "LanguageInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	m_bFullScreenMode = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bFullScreenMode = TRUE;
	CLanguageInfo::Instance()->ReadLanguageIni();

	SetMenu( NULL );
	ModifyStyle( WS_OVERLAPPEDWINDOW , WS_MAXIMIZE|WS_POPUP );
	ModifyStyleEx( WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED );
	
	SIZE szWnd;
	szWnd.cx = 1280; // ::GetSystemMetrics(SM_CXSCREEN);
	szWnd.cy = 1024; // ::GetSystemMetrics(SM_CYSCREEN);
/*
#ifdef _DEBUG
	szWnd.cx = 1280;		szWnd.cy = 1024;
#endif
*/
	SetWindowPos( NULL, 0, 0, szWnd.cx, szWnd.cy, SWP_FRAMECHANGED );
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	
	cs.style = WS_MAXIMIZE|WS_POPUP;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame 메시지 처리기
void CMainFrame::SwitchFullScreenMode()
{
	if ( m_bFullScreenMode )
	{
		NormalScreen();
	}
	else
	{
		FullScreen();
	}
}

void CMainFrame::FullScreen()
{
	if (m_bFullScreenMode)
		return;

	ModifyStyle( WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE, WS_MAXIMIZE|WS_POPUP );
	SIZE szWnd;
	szWnd.cx = 1280; // ::GetSystemMetrics(SM_CXSCREEN);
	szWnd.cy = 1024; // ::GetSystemMetrics(SM_CYSCREEN);
/*
#ifdef _DEBUG
szWnd.cx = 1280;		szWnd.cy = 1024;
#endif
*/
	SetWindowPos( NULL, 0, 0, szWnd.cx, szWnd.cy, SWP_FRAMECHANGED );
	RecalcLayout();

	SetForegroundWindow();

	m_bFullScreenMode = TRUE;
}

void CMainFrame::NormalScreen()
{
	if (!m_bFullScreenMode)
		return;

	ModifyStyle( WS_MAXIMIZE|WS_POPUP, WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE );
	SIZE szWnd;
	szWnd.cx = 1280; // ::GetSystemMetrics(SM_CXSCREEN);
	szWnd.cy = 1024; // ::GetSystemMetrics(SM_CYSCREEN);
/*
#ifdef _DEBUG
szWnd.cx = 1280;		szWnd.cy = 1024;
#endif
*/
	SetWindowPos( NULL, 0, 0, szWnd.cx, szWnd.cy, SWP_FRAMECHANGED );
	RecalcLayout();
		
	m_bFullScreenMode = FALSE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_F11)
		{
			SwitchFullScreenMode();
		}
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}