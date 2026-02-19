// StopWatchCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "StopWatchCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace UIExt;

#define ID_TIMER_COUNTER			(500)

/////////////////////////////////////////////////////////////////////////////
// CStopWatchCtrl

CStopWatchCtrl::CStopWatchCtrl()
{
	m_nCounter = 0;
	m_uTimer = 0;
	m_strText = _T("0");
	m_bPaused = FALSE;
	//m_nCharacterNumber = 8;

	//m_colForeColor = Color.White;

	m_bIsCacheBuild = FALSE;
	m_nBorderWidth = 1;
	m_colBorderColor = 0xFF303030;//Color.Gray;
	m_nCornerRadius = 5;
	m_nCharacterNumber = 6;
	m_fBevelRate = 0.25f;
	m_colFadedColor = 0xFF202020;//Color.DimGray;
	//m_colCustomBk1 = Color::Black;
	//m_colCustomBk2 = 0xFF202020;//Color.DimGray;
	m_colCustomBk1 = Color(35, 47, 56);
	m_colCustomBk2 = Color(39, 53, 63);
	m_fWidthSegWidthRatio = 0.2f;
	m_fWidthIntervalRatio = 0.05f;
	m_enumAlign = TA_Right;
	m_bRoundRect = FALSE;
	m_bGradientBackground = FALSE;
	m_bShowHighlight = FALSE;
	m_nHighlightOpaque = 50;
	m_colForeColor = Color(200,200,200);
	m_colBackColor = Color::Transparent;

	SetMode(TimerModeSecond);
}


CStopWatchCtrl::~CStopWatchCtrl()
{
}


BEGIN_MESSAGE_MAP(CStopWatchCtrl, CLedTextCtrl)
	//{{AFX_MSG_MAP(CStopWatchCtrl)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStopWatchCtrl message handlers

void CStopWatchCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	if ( nIDEvent == ID_TIMER_COUNTER )
	{
		if ( m_bPaused )
		{
			CLedTextCtrl::OnTimer(nIDEvent);
			return ;
		}

		switch ( m_eTimerMode )
		{
		case TimerModeHourMinuteSecond:
			++m_nSecond;
			if (m_nSecond%60 == 0)
			{
				m_nMinute++;
				m_nSecond = 0;
				if (m_nMinute%60 == 0)
				{
					m_nHour++;
					m_nMinute = 0;
				}
			}
			break;
		case TimerModeSecond:
			++m_nCounter;
			break;
		}
		InvalidateText();
	}


	CLedTextCtrl::OnTimer(nIDEvent);
}

void CStopWatchCtrl::InvalidateText()
{
	CString strText;
	switch ( m_eTimerMode )
	{
	case TimerModeHourMinuteSecond:
		if ( m_nHour > 0 )
		{
			strText.Format(_T("%4d:%02d:%02d"), m_nHour, m_nMinute, m_nSecond);
		}
		else
		{
			if ( m_nMinute > 0 )
				strText.Format(_T("     %02d:%02d"), m_nMinute, m_nSecond);
			else
				strText.Format(_T("        %02d"), m_nSecond);
		}
		break;
	case TimerModeSecond:
		strText.Format( _T("%d"), m_nCounter );
		break;
	case TimerModeYield:
		strText.Format( _T("%d.%d"), m_nMinute, m_nSecond );
		break;
	}

	SetText( strText );
}

void CStopWatchCtrl::SetMode( TimerMode eMode )
{
	if ( m_eTimerMode == eMode ) return;

	m_eTimerMode = eMode;

	m_nHour = m_nMinute = m_nSecond = m_nCounter = 0;
	m_nCharacterNumber = 10;

	switch ( eMode )
	{
	case TimerModeHourMinuteSecond:
		m_strText = _T("----:--:--");
		break;
	case TimerModeSecond:
		m_strText = _T("0");
		break;
	case TimerModeYield:
		m_strText = _T("0.0");
		break;
	}

	if (GetSafeHwnd())
		SetText( m_strText );
}


void CStopWatchCtrl::Start( BOOL bReset /*=TRUE*/ )
{
	if ( m_uTimer )
		KillTimer( m_uTimer );

	m_bPaused = FALSE;

	if ( bReset )
	{
		m_nCounter = 0;
		m_nHour = 0;
		m_nMinute = 0;
		m_nSecond = 0;
	}

	InvalidateText();
	
	m_uTimer = SetTimer( ID_TIMER_COUNTER, 1000, NULL );
}

void CStopWatchCtrl::Pause()
{
	m_bPaused = TRUE;
}

void CStopWatchCtrl::Resume()
{
	m_bPaused = FALSE;
}

BOOL CStopWatchCtrl::IsStarted()
{
	return m_uTimer == 0 ? FALSE : TRUE;
}

void CStopWatchCtrl::Stop()
{
	if ( m_uTimer )
	{
		KillTimer( m_uTimer );
	}
}

void CStopWatchCtrl::Reset()
{
	m_nCounter = 0;
	m_nHour = 0;
	m_nMinute = 0;
	m_nSecond = 0;

	InvalidateText();
}

void CStopWatchCtrl::SetTime( UINT nDays, UINT nHours, UINT nMinutes, UINT nSeconds )
{
	switch ( m_eTimerMode )
	{
	case TimerModeHourMinuteSecond:
		m_nHour = nDays*24 + nHours;
		m_nMinute = nMinutes;
		m_nSecond = nSeconds;
		break;
	case TimerModeSecond:
		m_nCounter = nDays*24*60*60 + nHours*60*60 + nMinutes*60 + nSeconds;
		break;
	case TimerModeYield:
		m_nMinute = nMinutes;
		m_nSecond = nSeconds;
		break;
	}

	InvalidateText();
}

CxTimeSpan CStopWatchCtrl::GetElapsedTime()
{
	switch ( m_eTimerMode )
	{
	case TimerModeHourMinuteSecond:
		{
			UINT nHour = m_nHour % 24;
			UINT nDays = m_nHour / 24;
			return CxTimeSpan( nDays, nHour, m_nMinute, m_nSecond );
		}
		break;
	case TimerModeSecond:
		{
			UINT nSecond = m_nCounter % 60;
			UINT nMinute = m_nCounter / 60;
			UINT nHour = nMinute / 60;
			nMinute %= 60;
			UINT nDays = nHour / 24;
			nHour %= 24;

			return CxTimeSpan( nDays, nHour, nMinute, nSecond );
		}
		break;
	}

	return CxTimeSpan( 0, 0, 0, 0 );
}