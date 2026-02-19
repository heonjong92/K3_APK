// HeartBeatCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "HeartBeatCtrl.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CHeartBeatCtrl
using namespace UIExt;
using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CHeartBeatCtrl, CGdipBaseWnd)

float s_BeatPos[] = { 0.f, 0.1f, -0.3f, /*0.3f,*/ 0.9f, /*0.f,*/ -0.9f, /*-0.4f,*/ 0.1f, 0.f };

CHeartBeatCtrl::CHeartBeatCtrl()
{
	m_nPosX = 0;
	m_nPosStep = 2;
	m_bBeat = TRUE;
	m_nBeatStep = 0;
	m_bDelayBeat = FALSE;
}

CHeartBeatCtrl::~CHeartBeatCtrl()
{
}


BEGIN_MESSAGE_MAP(CHeartBeatCtrl, CGdipBaseWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL CHeartBeatCtrl::Create(CWnd* pParentWnd /*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("HeartBeatCtrl"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1 );
}

void CHeartBeatCtrl::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.FillRectangle( &SolidBrush(Color(40, 0,0,0)), rect );

	g.SetSmoothingMode( SmoothingModeAntiAlias );

	int nPosY1 = rect.Y + rect.Height/2;
	int nPosY2 = rect.Y + rect.Height/2;
	int nAlpha = 255;
	if (m_bBeat && (m_nBeatStep!=0))
	{
		nPosY1 = (int)(rect.Y + rect.Height/2 - s_BeatPos[m_nBeatStep] * rect.Height * 0.4f);
		nPosY2 = (int)(rect.Y + rect.Height/2 - s_BeatPos[m_nBeatStep+1] * rect.Height * 0.4f);
	}
	Pen penLine(Color(nAlpha, 100,255,100), 2.0f);
	penLine.SetEndCap(LineCapRound);
	g.DrawLine(&penLine, m_nPosX, nPosY1, m_nPosX+m_nPosStep, nPosY2);
}

void CHeartBeatCtrl::OnGdipEraseBkgnd(Gdiplus::Graphics& g, Rect rect)
{
	g.FillRectangle( &SolidBrush(Color(0,0,0)), rect );
}

void CHeartBeatCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		CRect rc;
		GetClientRect( rc );
		if (rc.IsRectEmpty())
			return;
		m_nPosX += m_nPosStep;
		if (m_bBeat)
		{
goto_Beat:
			int nPosCnt = rc.Width() / m_nPosStep;
			int nBeatWidth = (int)(sizeof(s_BeatPos)/sizeof(float));
			int nHalf = nPosCnt/2 - nBeatWidth/4;
			if (nHalf == m_nPosX/m_nPosStep)
			{
				m_nBeatStep = 1;
			}
			
			if (m_nBeatStep > 0)
			{
				m_nBeatStep++;
				if (m_nBeatStep >= (int)sizeof(s_BeatPos) / sizeof(float))
				{
					m_nBeatStep = 0;
					m_bBeat = FALSE;
				}
			}
		}
		else
		{
			if (m_bDelayBeat)
			{
				m_bDelayBeat = FALSE;
				m_bBeat = TRUE;
				m_nBeatStep = 0;
				goto goto_Beat;
			}
		}
		if (m_nPosX > rc.right-1)
		{
			m_nPosX = rc.left;
		}

		Invalidate(FALSE);
	}

	CGdipBaseWnd::OnTimer(nIDEvent);
}

int CHeartBeatCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGdipBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer( 1, 50, NULL );

	return 0;
}

void CHeartBeatCtrl::Beat()
{
	if (!m_bBeat)
	{
		m_nBeatStep = 0;
		m_bBeat = TRUE;
	}
	else
	{
		m_bDelayBeat = TRUE;
	}
}