// SplashWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "SplashWnd.h"
#include "UIExt/ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSplashWnd
CSplashWnd* CSplashWnd::m_pSplashWnd = NULL;

IMPLEMENT_DYNAMIC(CSplashWnd, UIExt::CGdipBaseWnd)

CSplashWnd::CSplashWnd()
{
	m_pImageBackground = NULL;
}

CSplashWnd::~CSplashWnd()
{
	if (m_pImageBackground)
	{
		delete m_pImageBackground;
		m_pImageBackground = NULL;
	}

	ASSERT(m_pSplashWnd == this);
	m_pSplashWnd = NULL;

	m_nProgressStep = m_nProgressMax = 0;
}


BEGIN_MESSAGE_MAP(CSplashWnd, UIExt::CGdipBaseWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CSplashWnd::OnDraw( Graphics& g, Rect rect )
{
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	if (m_pImageBackground)
	{
		g.DrawImage(m_pImageBackground, Rect(0, 0, rect.Width, rect.Height), 
			0, 0, m_pImageBackground->GetWidth(), m_pImageBackground->GetHeight(), UnitPixel);
	}
	else
	{
		g.FillRectangle( &SolidBrush(Color::White), rect );
	}

	SolidBrush brushText(Color(100, 100, 100));

	RectF rectMessageF;
	rectMessageF.X = 10.f;
	rectMessageF.Y = 260.f;
	rectMessageF.Width = 400.f;
	rectMessageF.Height = 130.f;

	//g.DrawRectangle( &Pen(Color::Red), rectMessageF );

	Rect rectInner = rect;
	rectInner.Width--;
	rectInner.Height--;
	rectInner.X++;
	rectInner.Y++;
	g.DrawRectangle( &Pen(Color::White), rectInner );
	Rect rectBody = rect;
	rectBody.Width--;
	rectBody.Height--;
	g.DrawRectangle( &Pen(Color::DarkGray), rectBody );

	FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();
	Gdiplus::Font font(pFontFamily, 12.f, FontStyleRegular, UnitPixel);
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	RectF rectTextF = rectMessageF;
	rectTextF.Height = 18.f;
	int nMaxMessages = (int)(rectMessageF.Height / rectTextF.Height);
	int nMsgCnt = 0;
	std::list<MessageType>::reverse_iterator iter2 = m_Types.rbegin();
	int nAlpha = 255;
	int nAlphaStep = 100 / nMaxMessages;
	const WCHAR* wstrMessage;
	for (Messages::reverse_iterator iter = m_Messages.rbegin() ; iter != m_Messages.rend() && iter2 != m_Types.rend() && nMsgCnt < nMaxMessages ; iter++, iter2++, nMsgCnt++)
	{
		MsgString& strMessage = (*iter);
		MessageType& type = (*iter2);
		wstrMessage = strMessage.data();

		if (type == MessageTypeNormal)
		{
			SolidBrush brushMessageNormal(Color(nAlpha, 80, 80, 80));
			g.DrawString( wstrMessage, -1, &font, rectTextF, &stringFormat, &brushMessageNormal );
		}
		else
		{
			SolidBrush brushMessageError(Color(nAlpha, 200, 80, 80));
			g.DrawString( wstrMessage, -1, &font, rectTextF, &stringFormat, &brushMessageError );
		}

		rectTextF.Y += rectTextF.Height;
		nAlpha -= nAlphaStep;

	}

	Rect rectProgress = rectInner;
	const int nProgressHeight = 16;
	rectProgress.Y = rectProgress.GetBottom() - nProgressHeight;
	rectProgress.Height = nProgressHeight;
	rectProgress.Width = rectProgress.Width / 2;
	rectProgress.Inflate(-2, -3);
	if (m_nProgressMax != 0)
		DrawProgress(g, rectProgress);
}

void CSplashWnd::DrawProgress( Graphics& g, Rect& rectProgress )
{
	int nProgressLength = rectProgress.Width * m_nProgressStep / m_nProgressMax;

	SolidBrush brushProgress( Color(100, 66, 99, 145) );
	g.FillRectangle( &brushProgress, rectProgress.GetLeft(), rectProgress.GetTop(), nProgressLength, rectProgress.Height );
}

void CSplashWnd::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{

}

BOOL CSplashWnd::Create(CWnd* pParentWnd)
{
	m_pImageBackground = ::GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_SPLASH );
	if (!m_pImageBackground)
		return FALSE;

	WNDCLASSEX wcx; 

#ifndef AW_HIDE
	#define AW_HIDE 0x00010000
	#define AW_BLEND 0x00080000
#endif

#ifndef CS_DROPSHADOW
	#define CS_DROPSHADOW   0x00020000
#endif

	wcx.cbSize = sizeof(wcx);
	wcx.lpfnWndProc = AfxWndProc;
	wcx.style = CS_DBLCLKS|CS_SAVEBITS;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = AfxGetInstanceHandle();
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground=::GetSysColorBrush(COLOR_WINDOW);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = _T("UIExt::SplashWndClass");
	wcx.hIconSm = NULL;
	wcx.style |= CS_DROPSHADOW;

	ATOM classAtom = RegisterClassEx(&wcx);

	if (!classAtom)
	{
		wcx.style &= ~CS_DROPSHADOW;
		classAtom = RegisterClassEx(&wcx);
	}

	if (!CreateEx(WS_EX_TOOLWINDOW, _T("UIExt::SplashWndClass"), 
			NULL, WS_POPUP|WS_VISIBLE, 0, 0, 
			m_pImageBackground->GetWidth(), m_pImageBackground->GetHeight(), 
			pParentWnd->GetSafeHwnd(), NULL))
		return FALSE;

	return TRUE;
}

BOOL CSplashWnd::PreTranslateMessage(MSG* pMsg)
{
	if (m_pSplashWnd == NULL)
		return FALSE;

	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		m_pSplashWnd->HideSplashScreen(100);
		return TRUE;	// message handled here
	}

	return CGdipBaseWnd::PreTranslateMessage(pMsg);
}

void CSplashWnd::PostNcDestroy()
{
	delete this;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd/*=NULL*/)
{
	if (m_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	m_pSplashWnd = new CSplashWnd;
	if (!m_pSplashWnd->Create(pParentWnd))
	{
		delete m_pSplashWnd;
		m_pSplashWnd = NULL;
	}
	else
	{
		m_pSplashWnd->UpdateWindow();
	}
}

void CSplashWnd::HideSplashScreen( int nDelay )
{
	if (!m_pSplashWnd)
		return;
	m_pSplashWnd->SetTimer( 1, nDelay, NULL );
}

#define TXT_BUFFER_SIZE 1024
void CSplashWnd::ShowMessage(MessageType type, LPCTSTR lpszFormat, ...)
{
	if (!m_pSplashWnd)
		return;

	if ( lpszFormat == NULL || m_pSplashWnd == NULL )
	{
		return;
	}
	
	va_list argList;
	va_start(argList, lpszFormat);
	
	TCHAR lptszBuffer[TXT_BUFFER_SIZE];
	VERIFY(_vsntprintf_s(lptszBuffer, TXT_BUFFER_SIZE, _TRUNCATE, lpszFormat, argList) <= TXT_BUFFER_SIZE );

	va_end(argList);

	USES_CONVERSION;

	
	if(FALSE)	// 신규 내용 위로 표시
	{
		m_pSplashWnd->m_Messages.push_back( T2W(lptszBuffer) );
		m_pSplashWnd->m_Types.push_back( type );
	}
	else		// 신규 내용 아래로 표시
	{
		int nMaxCount	= 7;
		int nViewCount	= (int)m_pSplashWnd->m_Messages.size();

		if(nMaxCount <= nViewCount)
		{
			int nCheck;			

			Messages Messages_Temp;
			Messages_Temp.clear();
			nCheck = 1;

			for( MsgString Buff : m_pSplashWnd->m_Messages )
			{
				if(nCheck != nMaxCount) Messages_Temp.push_back(Buff);		
				nCheck ++;
			}
			

			MessagesType MessagesType_Temp;
			MessagesType_Temp.clear();
			nCheck = 1;

			for( MessageType Buff : m_pSplashWnd->m_Types )
			{
				if(nCheck != nMaxCount) MessagesType_Temp.push_back(Buff);		
				nCheck ++;
			}


			m_pSplashWnd->m_Messages.clear();
			m_pSplashWnd->m_Types.clear();
			m_pSplashWnd->m_Messages = Messages_Temp;
			m_pSplashWnd->m_Types = MessagesType_Temp;
		}

		m_pSplashWnd->m_Messages.push_front( T2W(lptszBuffer) );
		m_pSplashWnd->m_Types.push_front( type );
	}

	m_pSplashWnd->RedrawWindow();
}

void CSplashWnd::SetProgress(int nStep, int nMax)
{
	if (!m_pSplashWnd)
		return;

	m_pSplashWnd->m_nProgressStep = nStep;
	m_pSplashWnd->m_nProgressMax = nMax;

	m_pSplashWnd->RedrawWindow();
}

void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 )
	{
		KillTimer(1);
		m_pSplashWnd->DestroyWindow();
		AfxGetMainWnd()->UpdateWindow();
		return;
	}

	CGdipBaseWnd::OnTimer(nIDEvent);
}


int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGdipBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterWindow();

	return 0;
}
