// FilteredEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilteredEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;

#include <float.h>

// CFilteredEdit

const UINT UIExt::WM_FILTERED_EDIT_MODIFIED = ::RegisterWindowMessage( _T("UIEXT_FILTERED_EDIT_MODIFIED") );

IMPLEMENT_DYNAMIC(CFilteredEdit, CEdit)
CFilteredEdit::CFilteredEdit() : m_dwBorderColor( RGB(150, 150, 150) ),
					 m_dwTextColor( RGB(0, 0, 0) ),
					 m_dwActiveTextColor( RGB(255, 0, 0) ),
					 m_dwBgColor( RGB(240, 240, 240) )

{
	m_BgBrush.CreateSolidBrush( m_dwBgColor );

	m_bNowEditing = FALSE;

	m_strAllowString = _T("0123456789.-");
	m_strNoneAllowString.Empty();
}

CFilteredEdit::~CFilteredEdit()
{
}


BEGIN_MESSAGE_MAP(CFilteredEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
END_MESSAGE_MAP()

HBRUSH CFilteredEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	DrawEditFrame( pDC );

	return m_BgBrush;
}

void CFilteredEdit::OnPaint()
{
	CEdit::OnPaint();
}

void CFilteredEdit::SetColor( COLORREF dwBorderColor, COLORREF dwBgColor )
{
	m_dwBgColor = dwBgColor;
	m_dwBorderColor = dwBorderColor;
	if( m_BgBrush.GetSafeHandle() )
		m_BgBrush.DeleteObject();

	m_BgBrush.CreateSolidBrush( dwBgColor );

	if ( IsWindow(GetSafeHwnd()) )
		RedrawWindow();
}

void CFilteredEdit::SetFilter( LPCTSTR lpszFilter )
{
	m_strAllowString = lpszFilter;
	m_strNoneAllowString.Empty();
}

void CFilteredEdit::SetInvertFilter( LPCTSTR lpszFilter )
{
	m_strNoneAllowString = lpszFilter;
	m_strAllowString.Empty();
}

void CFilteredEdit::DrawEditFrame( CDC* pDC )
{	
	CRect rcItem;
	GetClientRect( &rcItem );

	if ( m_bNowEditing )
		pDC->SetTextColor( m_dwActiveTextColor );
	else
		pDC->SetTextColor( m_dwTextColor );

	pDC->SetBkColor( m_dwBgColor );
}

void CFilteredEdit::OnSetfocus() 
{
	m_bNowEditing = TRUE;

	RedrawWindow();
}

void CFilteredEdit::OnKillfocus()
{
	m_bNowEditing = FALSE;
	RedrawWindow();

	GetParent()->PostMessage( WM_FILTERED_EDIT_MODIFIED, (WPARAM)GetDlgCtrlID(), (LPARAM)0 );
}

BOOL CFilteredEdit::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_CHAR )
	{
		if ( !m_strAllowString.IsEmpty() )
		{
			TCHAR chCharCode = (TCHAR)pMsg->wParam;
			if ( chCharCode == VK_BACK )
				return CEdit::PreTranslateMessage(pMsg);
			if ( m_strAllowString.Find( chCharCode ) < 0 )
			{
				return TRUE;
			}
		}
		if ( !m_strNoneAllowString.IsEmpty() )
		{
			TCHAR chCharCode = (TCHAR)pMsg->wParam;
			if ( chCharCode == VK_BACK )
				return CEdit::PreTranslateMessage(pMsg);
			if ( m_strNoneAllowString.Find( chCharCode ) >= 0 )
			{
				return TRUE;
			}
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}
