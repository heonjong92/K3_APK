#if !defined(AFX_STOPWATCHWND_H__DFD193FF_ADBA_4FF2_9A9E_334533C71B25__INCLUDED_)
#define AFX_STOPWATCHWND_H__DFD193FF_ADBA_4FF2_9A9E_334533C71B25__INCLUDED_

#include "LedTextCtrl.h"
#include <XUtil/xTime.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StopWatchWnd.h : header file
//
namespace UIExt {
/////////////////////////////////////////////////////////////////////////////
// CStopWatchCtrl window

class CStopWatchCtrl : public CLedTextCtrl
{
// Construction
public:
	CStopWatchCtrl();

	enum TimerMode { TimerModeHourMinuteSecond, TimerModeSecond, TimerModeYield};

// Attributes
private:
	UINT_PTR			m_uTimer;

	UINT				m_nCounter;
	
	UINT				m_nHour;
	UINT				m_nMinute;
	UINT				m_nSecond;

	TimerMode			m_eTimerMode;

	BOOL				m_bPaused;

// Operations
public:
	BOOL IsStarted();

	void Start( BOOL bReset = TRUE );
	void Stop();

	void Pause();
	void Resume();

	void Reset();

	void SetMode( TimerMode eMode );

	void SetTime( UINT nDays, UINT nHours, UINT nMinutes, UINT nSeconds );

	CxTimeSpan GetElapsedTime();

	void InvalidateText();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStopWatchCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStopWatchCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStopWatchCtrl)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STOPWATCHWND_H__DFD193FF_ADBA_4FF2_9A9E_334533C71B25__INCLUDED_)
