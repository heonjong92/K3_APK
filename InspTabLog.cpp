// InspTabLog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "InspTabLog.h"
#include "afxdialogex.h"

#include "APKView.h"
#include "VisionSystem.h"

// CInspTabLog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspTabLog, CDialogEx)

CInspTabLog::CInspTabLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInspTabLog::IDD, pParent)
{
	m_dwBodyColor = RGB(220,220,220);
	m_BrushBody.CreateSolidBrush(m_dwBodyColor);
}

CInspTabLog::~CInspTabLog()
{
}

void CInspTabLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

 	DDX_Control(pDX, IDC_LIST_RESULT, m_wndListResult);
}


BEGIN_MESSAGE_MAP(CInspTabLog, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CInspTabLog 메시지 처리기입니다.
BOOL CInspTabLog::OnInitDialog()
{
	CDialog::OnInitDialog();

	using namespace UIExt;
	using namespace VisionDevice;

	if (!m_InitLogMessageTypes.IsEmpty())
	{
		for (INT_PTR nI=0 ; nI<m_InitLogMessageTypes.GetCount() ; nI++)
		{
			CString strLog = m_InitLogMessages.GetAt(nI);
			SYSTEMTIME st = m_InitLogSysTimes.GetAt(nI);

			CString strTime;
			strTime.Format( _T("[%02d/%02d %02d:%02d:%02d:%03d]"), st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 

			CString strTimeLog;
			strTimeLog.Format( _T("%s %s"), strTime, strLog );
			eLogType _type = m_InitLogMessageTypes.GetAt(nI);
			switch (_type)
			{
			default:
			case LogTypeNormal:
				m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeNormal );
				break;
			case LogTypeNotification:
				m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeNotification );
				break;
			case LogTypeError:
				m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeError );
				break;
			}
		}
		m_InitLogMessages.RemoveAll();
		m_InitLogMessageTypes.RemoveAll();
		m_InitLogSysTimes.RemoveAll();
	}

	return TRUE;
}

HBRUSH CInspTabLog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
	}

	return hbr;
}

BOOL CInspTabLog::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	
	int nOffsetXY = 3;
	m_wndListResult.MoveWindow( nOffsetXY, nOffsetXY, rc.Width()-(nOffsetXY*2), rc.Height()-(nOffsetXY*2) );

	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	return TRUE;
}

void CInspTabLog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		UpdateData(FALSE);
	}
}

void CInspTabLog::AddLogMessage(LPCTSTR lpszLog, eLogType type)
{
	if (!m_wndListResult)
	{
		m_InitLogMessageTypes.Add(type);
		m_InitLogMessages.Add(lpszLog);

		SYSTEMTIME st;
		::GetLocalTime(&st);
		m_InitLogSysTimes.Add(st);
		return;
	}
	if (!m_InitLogMessageTypes.IsEmpty())
	{
		for (INT_PTR nI=0 ; nI<m_InitLogMessageTypes.GetCount() ; nI++)
		{
			CString strLog = m_InitLogMessages.GetAt(nI);
			SYSTEMTIME st = m_InitLogSysTimes.GetAt(nI);
			CString strTime;	
			strTime.Format( _T("[%02d/%02d %02d:%02d:%02d:%03d]"), st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

			CString strTimeLog;
			strTimeLog.Format( _T("%s %s"), strTime, strLog );
			eLogType _type = m_InitLogMessageTypes.GetAt(nI);
			switch (_type)
			{
			default:
			case LogTypeNormal:
				m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeNormal );
				break;
			case LogTypeNotification:
				m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeNotification );
				break;
			case LogTypeError:
				m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeError );
				break;
			}
		}
		m_InitLogMessages.RemoveAll();
		m_InitLogMessageTypes.RemoveAll();
		m_InitLogSysTimes.RemoveAll();
	}

	SYSTEMTIME st;
	::GetLocalTime(&st);
	CString strCurrentTime;	
	strCurrentTime.Format( _T("[%02d/%02d %02d:%02d:%02d:%03d]"), st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );

	CString strTimeLog;
	strTimeLog.Format( _T("%s %s"), strCurrentTime, lpszLog );

	switch (type)
	{
	default:
	case LogTypeNormal:
		m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeNormal );
		break;
	case LogTypeNotification:
		m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeNotification );
		break;
	case LogTypeError:
		m_wndListResult.AddString( strTimeLog, UIExt::COutputListBox::ItemTypeError );
		break;
	}
}

BOOL CInspTabLog::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(msg);
}
