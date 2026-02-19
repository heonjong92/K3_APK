// FlatTabWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FlatTabWnd.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ID_TIMER_CHECKHOVER	(100)
#define ID_TIMER_BLINK		(200)

namespace UIExt
{
const UINT WM_FLAT_TABWND_SELCHANGE = ::RegisterWindowMessage( _T("FLAT_TABWND_SELCHANGE") );
const UINT WM_FLAT_TABWND_SELCHANGED = ::RegisterWindowMessage( _T("FLAT_TABWND_SELCHANGED") );
}

using namespace UIExt;

#define OFFSET_XY		(0)

// CFlatTabWnd

IMPLEMENT_DYNAMIC(CFlatTabWnd, CGdipStatic)
CFlatTabWnd::CFlatTabWnd()
{
	m_bIsHovered = FALSE;
	m_bIsMouseDown = FALSE;

	m_nActiveTabIdx = -1;
	m_nHoverTabIdx = -1;

	m_szTabHeader.cx = 100;
	m_szTabHeader.cy = 30;

	m_nBarOffset = 0;
	m_nBarWidth = 0;

	m_eTabDirection = TabDirectionHorzLeftTop;

	m_bNotifyTab = FALSE;
	m_aryTabInfo.RemoveAll();
	m_nBlinkCount = 0;

	m_dwBackgroundColor = RGB(255,255,255);

	m_bEnableTabChange = TRUE;

	m_nTextHeight = 12;
}

CFlatTabWnd::~CFlatTabWnd()
{
}


BEGIN_MESSAGE_MAP(CFlatTabWnd, CGdipStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_SETFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()


void CFlatTabWnd::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	Color colorBackground;
	colorBackground.SetFromCOLORREF(m_dwBackgroundColor);
	g.FillRectangle( &SolidBrush(colorBackground), rect );
}

void CFlatTabWnd::SetBackgroundColor(DWORD dwColor)
{
	m_dwBackgroundColor = dwColor;
	Invalidate();
}

void CFlatTabWnd::OnDraw( Graphics& g, Rect rect )
{
	//g.SetSmoothingMode( SmoothingModeAntiAlias );

	DrawTabs( g, rect );
}

CSize CFlatTabWnd::CalcTabSize( Graphics& g, const TabInfo& ti )
{
	BSTR bstrCaption = ti.strCaption.AllocSysString();
	Gdiplus::Font fntCaption( CResourceManager::SystemFontFamily(), (float)m_nTextHeight, FontStyleRegular, UnitPixel );

	RectF rectText;
	rectText.X = rectText.Y = 0.f;

	if ( IsHorzTab() )
	{
		rectText.Width = 0.f;
		rectText.Height = (float)m_szTabHeader.cy;
	}
	else
	{
		rectText.Width = (float)m_szTabHeader.cx;
		rectText.Height = 0.f;
	}

	RectF rectBB;
	g.MeasureString( bstrCaption, -1, &fntCaption, rectText, &rectBB );

	::SysFreeString( bstrCaption );

	float fImageOffset = 0.f;
	if ( ti.pIconImage )
	{
		fImageOffset = ti.pIconImage->GetWidth() + 5.f;
	}

	if ( IsHorzTab() )
	{
		rectBB.Width += fImageOffset + 15.f;
		if (m_nBarWidth != 0)
			rectBB.Width = (float)m_nBarWidth;
	}
	else
	{
		rectBB.Width += fImageOffset;
		rectBB.Height += 15.f;
		if (m_nBarWidth != 0)
			rectBB.Height = (float)m_nBarWidth;
	}


	return CSize( (int)(rectBB.Width + .5f), (int)(rectBB.Height + .5f) );
}

void CFlatTabWnd::SetTabText( INT_PTR nI, LPCTSTR lpszNewText )
{
	if ( m_aryTabInfo.GetCount() <= nI || nI < 0 )
	{
		return;
	}

	m_aryTabInfo.GetAt(nI).strCaption = lpszNewText;
}

INT_PTR CFlatTabWnd::IsNotifyTab( INT_PTR nI )
{
	for ( INT_PTR i=0 ; i<m_aryNotifyTabIdx.GetCount() ; i++ )
	{
		if ( m_aryNotifyTabIdx.GetAt(i) == nI )
			return i;
	}
	return -1;
}

void CFlatTabWnd::DrawTabs( Graphics& g, Rect rc )
{
#define TAB_INTERVAL	(1)
#define TAB_MARGIN		(0)
	Rect rectTab;

	switch ( m_eTabDirection )
	{
	case TabDirectionHorzLeftTop:
		rectTab.Y = rc.Y;
		rectTab.X = rc.X + m_nBarOffset;
		break;
	case TabDirectionHorzRightTop:
		rectTab.Y = rc.Y;
		rectTab.X = rc.GetRight() - m_nBarOffset + TAB_INTERVAL;
		break;
	case TabDirectionVertLeftTop:
		rectTab.X = rc.X;
		rectTab.Y = rc.Y + m_nBarOffset;
		break;
	case TabDirectionVertLeftBottom:
		rectTab.X = rc.X;
		rectTab.Y = rc.GetBottom() - m_nBarOffset + TAB_INTERVAL;
		break;
	}

	if (m_aryTabInfo.GetCount() == 0)
		return;

	CSize szTab = CalcTabSize( g, m_aryTabInfo.GetAt(0) );
	BOOL bModifyTabSize = FALSE;
	if ( IsHorzTab() )
	{
		if (szTab.cx != m_szTabHeader.cx)
			bModifyTabSize = TRUE;
		m_szTabHeader.cx = szTab.cx;
		rectTab.Height = m_szTabHeader.cy;
	}
	else
	{
		if (szTab.cy != m_szTabHeader.cy)
			bModifyTabSize = TRUE;
		m_szTabHeader.cy = szTab.cy;
		rectTab.Width = m_szTabHeader.cx;
	}

	if (bModifyTabSize)
	{
		CRect rc;
		GetClientRect( rc );

		if ( IsHorzTab() )		// Horz, Top인 경우
		{
			m_rcChildArea.left = rc.left;
			m_rcChildArea.top = rc.top+m_szTabHeader.cy;
			m_rcChildArea.right = rc.right;
			m_rcChildArea.bottom = rc.bottom;
			m_rcChildArea.DeflateRect( OFFSET_XY, 0, OFFSET_XY, OFFSET_XY );
		}
		else // Vert, Left인 경우
		{
			m_rcChildArea.left = rc.left + m_szTabHeader.cx;
			m_rcChildArea.top = rc.top;
			m_rcChildArea.right = rc.right;
			m_rcChildArea.bottom = rc.bottom;
			m_rcChildArea.DeflateRect( 0, OFFSET_XY, OFFSET_XY, OFFSET_XY );
		}

		if ( m_nActiveTabIdx >= 0 )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
			if ( ti.pChildWnd )
			{
				ti.pChildWnd->MoveWindow( m_rcChildArea );
				ti.pChildWnd->RedrawWindow();
			}
		}
	}

	for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI ++ )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(nI);
		szTab = CalcTabSize( g, ti );

		if ( m_eTabDirection == TabDirectionHorzRightTop )
		{
			rectTab.X -= szTab.cx + TAB_INTERVAL + TAB_MARGIN;
		}
		if ( m_eTabDirection == TabDirectionVertLeftBottom )
		{
			rectTab.Y -= szTab.cy + TAB_INTERVAL + TAB_MARGIN;
		}

		if ( IsHorzTab() )
		{
			rectTab.Width = szTab.cx + TAB_MARGIN;	// margin x: 5px
		}
		else
		{
			rectTab.Height = szTab.cy + TAB_MARGIN;
		}

		DrawTab( g, rectTab, ti, 
			m_nHoverTabIdx == nI ? TRUE : FALSE, 
			m_nActiveTabIdx == nI ? TRUE : FALSE, IsNotifyTab(nI) < 0 ? FALSE : TRUE );

		ti.rcArea.left = rectTab.X;
		ti.rcArea.top = rectTab.Y;
		ti.rcArea.right = rectTab.GetRight();
		ti.rcArea.bottom = rectTab.GetBottom();

		if ( m_eTabDirection == TabDirectionHorzLeftTop )
		{
			rectTab.X += rectTab.Width + TAB_INTERVAL;		// interval: 5px
		}
		if ( m_eTabDirection == TabDirectionVertLeftTop )
		{
			rectTab.Y += rectTab.Height + TAB_INTERVAL;
		}
	}

	//rectTab.Width += TAB_INTERVAL;

	Color colorBody;
	colorBody.SetFromCOLORREF(RGB(230,230,230));

	if ( IsHorzTab() )
	{
		int nTop = rectTab.GetBottom();
		g.FillRectangle( &SolidBrush(colorBody), Rect(rc.X, nTop, rc.Width, rc.GetBottom()-nTop) );
	}
	else
	{
		int nLeft = rectTab.GetRight();
		g.FillRectangle( &SolidBrush(colorBody), Rect(nLeft, rc.Y, rc.GetRight()-nLeft, rc.Height) );
	}

}

void CFlatTabWnd::DrawTab( Graphics& g, Rect& rectTab, const TabInfo& ti, BOOL bHover, BOOL bActive, BOOL bNotify )
{
	g.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

	int nDiameter = 3 << 1;
	Color colorBody;

	if ( bNotify && (m_nBlinkCount % 2 == 0) )
	{
		colorBody = Color(157, 100, 100);
	}
	else
	{
		if ( bActive )
		{
			colorBody = Color(80, 106, 126);
			if (!IsWindowEnabled())
				colorBody = Color(100, 100, 100);
		}
		else
			colorBody = Color(157, 157, 157);
	}

	g.FillRectangle( &SolidBrush(colorBody), rectTab );

	if (bHover)
	{
		Color colorLighten( 20, 255, 255, 255 );
		g.FillRectangle( &SolidBrush(colorLighten), rectTab );
	}

	RectF rectText;
	
	float fTextOffset = ti.pIconImage ? ti.pIconImage->GetWidth()+5.f : 0.f;

	if ( IsHorzTab() )
	{
		rectText.X = (float)rectTab.X + fTextOffset;
		rectText.Y = (float)rectTab.Y + nDiameter/2;
		rectText.Width = (float)rectTab.Width - fTextOffset;
		rectText.Height = (float)rectTab.Height - nDiameter/2;
	}
	else
	{
		rectText.X = (float)rectTab.X + nDiameter/2 + fTextOffset;
		rectText.Y = (float)rectTab.Y;
		rectText.Width = (float)rectTab.Width - nDiameter/2 - fTextOffset;
		rectText.Height = (float)rectTab.Height;
	}

	BSTR bstrCaption = ti.strCaption.AllocSysString();

	Gdiplus::Font fntCaption( CResourceManager::SystemFontFamily(), (float)m_nTextHeight, FontStyleBold, UnitPixel );

	if ( ti.pIconImage )
	{
		int nIW = ti.pIconImage->GetWidth();
		int nIH = ti.pIconImage->GetHeight();
		g.DrawImage( ti.pIconImage, (int)(rectText.X-fTextOffset), (int)(rectText.Y), 0, 0, nIW, nIH, UnitPixel );
	}

	StringFormat stringFormat;
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetAlignment( StringAlignmentCenter );
	rectText.Y -= 1.f;
	g.DrawString( bstrCaption, -1, &fntCaption, rectText, &stringFormat, &SolidBrush( Color(255, 255, 255) ) );

	::SysFreeString( bstrCaption );
}

void CFlatTabWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_bIsMouseDown && ::GetCapture() == m_hWnd )
	{
		for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI++ )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(nI);
			if ( ti.rcArea.PtInRect( point ) )
			{
				if ( m_nHoverTabIdx != nI )
				{
					m_nHoverTabIdx = nI;
					Invalidate();
				}
				break;
			}
		}
		if ( m_nHoverTabIdx != -1 )
		{
			m_nHoverTabIdx = -1;
			Invalidate();
		}
	}
	else 
	{
        if ( !m_bIsHovered ) 
		{
            TRACKMOUSEEVENT t = {
                sizeof(TRACKMOUSEEVENT),
					TME_LEAVE,
					m_hWnd,
					0
            };
			
            if ( ::_TrackMouseEvent(&t) ) 
			{
                m_bIsHovered = TRUE;
				OnMouseEnter();
            }
        }
		else
		{
			for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI++ )
			{
				TabInfo& ti = m_aryTabInfo.GetAt(nI);
				if ( ti.rcArea.PtInRect( point ) )
				{
					if ( m_nHoverTabIdx != nI )
					{
						m_nHoverTabIdx = nI;
						Invalidate();
					}
					return;
				}
			}

			if ( m_nHoverTabIdx != -1 )
			{
				m_nHoverTabIdx = -1;
				Invalidate();
			}
		}
    }

	CGdipStatic::OnMouseMove(nFlags, point);
}

void CFlatTabWnd::OnMouseEnter()
{
	Invalidate();
}

LRESULT CFlatTabWnd::OnMouseLeave(WPARAM, LPARAM)
{
    //ASSERT( m_bIsHovered );
    m_bIsHovered = FALSE;
	m_nHoverTabIdx = -1;

	HCURSOR hNormalCursor = ::LoadCursor( NULL, IDC_ARROW );
	::SetCursor(hNormalCursor);

    Invalidate();
	return 0;
}

void CFlatTabWnd::NotifyTab( INT_PTR nI )
{
	if ( nI < 0 || nI >= m_aryTabInfo.GetCount() )
		return;
	if ( m_nActiveTabIdx == nI )
		return;

	for ( INT_PTR i=0 ; i<m_aryNotifyTabIdx.GetCount() ; i++ )
	{
		if ( m_aryNotifyTabIdx.GetAt(i) == nI )
			return;
	}

	m_bNotifyTab = TRUE;
	m_aryNotifyTabIdx.Add( nI );
	m_nBlinkCount = 0;

	SetTimer( ID_TIMER_BLINK, 500, NULL );
}

void CFlatTabWnd::ResetNotify()
{
	m_bNotifyTab = FALSE;
	m_aryNotifyTabIdx.RemoveAll();
	KillTimer( ID_TIMER_BLINK );
	Invalidate();
}

void CFlatTabWnd::EnableTabChange( BOOL bEnable )
{
	m_bEnableTabChange = bEnable;
}

static void EnableWindowWithAllChilds( CWnd* pWnd, BOOL bEnable, BOOL bFirst )
{
	if (!bFirst)
	{
		pWnd->EnableWindow(bEnable);
		pWnd->Invalidate();
	}
	CWnd* pWndC = pWnd->GetWindow(GW_CHILD);
	while ( pWndC && *pWndC )
	{
		EnableWindowWithAllChilds(pWndC, bEnable, FALSE);
		pWndC = pWndC->GetWindow(GW_HWNDNEXT);
	}
}

BOOL CFlatTabWnd::EnableWindow( BOOL bEnable )
{
	EnableWindowWithAllChilds(this, bEnable, TRUE);
	//RedrawWindow();
	if ( m_nActiveTabIdx >= 0 )
	{
		TabInfo& ti = m_aryTabInfo.GetAt( m_nActiveTabIdx );
		if ( ti.pChildWnd )
		{
			ti.pChildWnd->RedrawWindow();
		}
	}
	BOOL bRet = CGdipStatic::EnableWindow(bEnable);
	Invalidate();
	return bRet;
}

void CFlatTabWnd::SelectTab( INT_PTR nI )
{
	if ( !m_bEnableTabChange )
		return;

	if ( m_nActiveTabIdx != nI )
	{
		if ( m_bNotifyTab )
		{
			INT_PTR nN = 0;
			if ( (nN = IsNotifyTab(nI)) >= 0 )
			{
				m_aryNotifyTabIdx.RemoveAt(nN);
			}
			if ( m_aryNotifyTabIdx.GetCount() == 0 )
			{
				m_bNotifyTab = FALSE;
				KillTimer( ID_TIMER_BLINK );
			}
		}

		CWnd* pParent = GetParent();
		if ( pParent && *pParent )
			pParent->SendMessage( WM_FLAT_TABWND_SELCHANGE, MAKEWPARAM(nI, 0), 0 );

		for ( INT_PTR i = 0 ; i < m_aryTabInfo.GetCount() ; i ++ )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(i);
			if ( ti.pChildWnd )
			{
				ti.pChildWnd->ShowWindow( SW_HIDE );
			}
		}
		m_nActiveTabIdx = nI;
		TabInfo& ti = m_aryTabInfo.GetAt( m_nActiveTabIdx );
		if ( ti.pChildWnd )
		{
			ti.pChildWnd->MoveWindow( m_rcChildArea );
			ti.pChildWnd->ShowWindow( SW_SHOW );
		}

		if ( pParent && *pParent )
			pParent->SendMessage( WM_FLAT_TABWND_SELCHANGED, MAKEWPARAM(nI, 0), 0 );

		Invalidate();
	}
}

void CFlatTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( GetCapture() != this )
		SetCapture();

	if ( m_bIsHovered ) 
	{
        TRACKMOUSEEVENT t = {
            sizeof(TRACKMOUSEEVENT),
				TME_CANCEL | TME_LEAVE,
				m_hWnd, 0
		};
		
        if ( ::_TrackMouseEvent(&t) ) 
		{
            m_bIsHovered = FALSE;
        }
    }

	for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI++ )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(nI);
		if ( ti.rcArea.PtInRect( point ) )
		{
			SelectTab( nI );
		}
	}
	
	CGdipStatic::OnLButtonDown(nFlags, point);
    m_bIsMouseDown = TRUE;

	Invalidate();
}

void CFlatTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( GetCapture() == this )
		ReleaseCapture();

	/*
	if ( m_nHoverTabIdx != -1 )
	{
		m_nHoverTabIdx = -1;
		Invalidate();
	}
	*/

	m_bIsMouseDown = FALSE;

	for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI++ )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(nI);
		if ( ti.rcArea.PtInRect( point ) )
		{
			m_bIsHovered = TRUE;
			CGdipStatic::OnLButtonUp(nFlags, point);
			return;
		}
	}

	m_bIsHovered = FALSE;
	m_nHoverTabIdx = -1;
	OnMouseLeave(0,0);

	CGdipStatic::OnLButtonUp(nFlags, point);
}

void CFlatTabWnd::OnKillFocus(CWnd* pNewWnd) 
{
	if ( ::GetCapture() == m_hWnd ) 
	{
        ::ReleaseCapture();
		
        ASSERT( !m_bIsHovered );
        m_bIsMouseDown = FALSE;
		Invalidate();
    }

	CGdipStatic::OnKillFocus(pNewWnd);	
}

void CFlatTabWnd::OnSetFocus(CWnd* pOldWnd)
{
	CGdipStatic::OnSetFocus(pOldWnd);
}

BOOL CFlatTabWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( (m_bIsHovered||m_bIsMouseDown) && (m_nHoverTabIdx >= 0) )
	{
		HCURSOR hHandCursor = ::LoadCursor( NULL, IDC_HAND );
		::SetCursor(hHandCursor);
		return TRUE;
	}
	else
	{
		CPoint pt;
		GetCursorPos( &pt );
		ScreenToClient( &pt );
		if ( !m_rcChildArea.PtInRect(pt) )
		{
			HCURSOR hNormalCursor = ::LoadCursor( NULL, IDC_ARROW );
			::SetCursor(hNormalCursor);
			return TRUE;
		}
	}

	return CGdipStatic::OnSetCursor(pWnd, nHitTest, message);
}
void CFlatTabWnd::OnSize(UINT nType, int cx, int cy)
{
	CGdipStatic::OnSize(nType, cx, cy);

	if ( cx <= 0 || cy <= 0 ) return;

	CRect rc;
	GetClientRect( rc );

	if ( IsHorzTab() )		// Horz, Top인 경우
	{
		m_rcChildArea.left = rc.left;
		m_rcChildArea.top = rc.top+m_szTabHeader.cy;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( OFFSET_XY, 0, OFFSET_XY, OFFSET_XY );
	}
	else // Vert, Left인 경우
	{
		m_rcChildArea.left = rc.left + m_szTabHeader.cx;
		m_rcChildArea.top = rc.top;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( 0, OFFSET_XY, OFFSET_XY, OFFSET_XY );
	}

	for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI ++ )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(nI);
		if ( ti.pChildWnd )
		{
			ti.pChildWnd->MoveWindow( m_rcChildArea );
			ti.pChildWnd->RedrawWindow();
		}
	}

}

void CFlatTabWnd::SetTabDirection( TabDirection eDirection, int nBarHeight, int nBarOffset/*=0*/ ) 
{
	m_eTabDirection = eDirection;
	IsHorzTab() ? m_szTabHeader.cy = nBarHeight : m_szTabHeader.cx = nBarHeight;
	m_nBarOffset = nBarOffset;

	CRect rc;
	GetClientRect( rc );

	if ( IsHorzTab() )		// Horz, Top인 경우
	{
		m_rcChildArea.left = rc.left;
		m_rcChildArea.top = rc.top+m_szTabHeader.cy;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( OFFSET_XY, 0, OFFSET_XY, OFFSET_XY );
	}
	else // Vert, Left인 경우
	{
		m_rcChildArea.left = rc.left + m_szTabHeader.cx;
		m_rcChildArea.top = rc.top;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( 0, OFFSET_XY, OFFSET_XY, OFFSET_XY );
	}

	if ( m_nActiveTabIdx >= 0 )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
		if ( ti.pChildWnd )
		{
			ti.pChildWnd->MoveWindow( m_rcChildArea );
			ti.pChildWnd->RedrawWindow();
		}
	}
}

void CFlatTabWnd::SetTabWidth( int nBarWidth/*=0*/ )	// 0: variable, 
{
	m_nBarWidth = nBarWidth;

	CRect rc;
	GetClientRect( rc );

	if ( IsHorzTab() )		// Horz, Top인 경우
	{
		m_rcChildArea.left = rc.left;
		m_rcChildArea.top = rc.top+m_szTabHeader.cy;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( OFFSET_XY, 0, OFFSET_XY, OFFSET_XY );
	}
	else // Vert, Left인 경우
	{
		m_rcChildArea.left = rc.left + m_szTabHeader.cx;
		m_rcChildArea.top = rc.top;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( 0, OFFSET_XY, OFFSET_XY, OFFSET_XY );
	}

	if ( m_nActiveTabIdx >= 0 )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
		if ( ti.pChildWnd )
		{
			ti.pChildWnd->MoveWindow( m_rcChildArea );
			ti.pChildWnd->RedrawWindow();
		}
	}
}

INT_PTR CFlatTabWnd::AddTab( LPCTSTR lpszCaption, CWnd* pChildWnd, Color colorTab/*=Color(200, 255, 60, 60)*/, Image* pIconImage/*=NULL*/ )
{
	if ( pChildWnd )
		pChildWnd->ModifyStyle( 0, WS_CLIPSIBLINGS );
	TabInfo ti;
	ti.pChildWnd = pChildWnd;
	ti.pChildWnd->ShowWindow(SW_HIDE);
	ti.strCaption = lpszCaption;
	ti.pIconImage = NULL;
	ti.colorTab = colorTab;
	return m_aryTabInfo.Add( ti );
}

void CFlatTabWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CGdipStatic::OnShowWindow(bShow, nStatus);

	//m_nHoverIndex = -1;
}


void CFlatTabWnd::OnDestroy()
{
	CGdipStatic::OnDestroy();

	m_aryTabInfo.RemoveAll();
	m_nActiveTabIdx = -1;
	m_nHoverTabIdx = -1;
	m_bIsHovered = FALSE;
	m_bIsMouseDown = FALSE;
}

void CFlatTabWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch ( nIDEvent )
	{
	case ID_TIMER_CHECKHOVER:
		{
			CPoint ptCur;
			if ( GetCursorPos(&ptCur) )
			{
				HWND hWndCur = ::WindowFromPoint(ptCur);
				if ( hWndCur != GetSafeHwnd() )
				{
					if ( m_bIsHovered || m_bIsMouseDown )
					{
						m_bIsHovered = FALSE;
						m_bIsMouseDown = FALSE;
						m_nHoverTabIdx = -1;
						Invalidate();
					}
				}
			}
		}
		break;
	case ID_TIMER_BLINK:
		if ( m_nBlinkCount < 20 )
		{
			m_nBlinkCount++;
			Invalidate();
		}
		break;
	}

	CGdipStatic::OnTimer(nIDEvent);
}

int CFlatTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGdipStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS|SS_NOTIFY );
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	SetTimer( ID_TIMER_CHECKHOVER, 100, NULL );

	return 0;
}


void CFlatTabWnd::PreSubclassWindow()
{
	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS|SS_NOTIFY );
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	SetTimer( ID_TIMER_CHECKHOVER, 100, NULL );

	CRect rc;
	GetClientRect( rc );

	if ( IsHorzTab() )		// Horz, Top인 경우
	{
		m_rcChildArea.left = rc.left;
		m_rcChildArea.top = rc.top+m_szTabHeader.cy;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( OFFSET_XY, 0, OFFSET_XY, OFFSET_XY );
	}
	else // Vert, Left인 경우
	{
		m_rcChildArea.left = rc.left + m_szTabHeader.cx;
		m_rcChildArea.top = rc.top;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( 0, OFFSET_XY, OFFSET_XY, OFFSET_XY );
	}

	if ( m_nActiveTabIdx >= 0 )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
		if ( ti.pChildWnd )
		{
			ti.pChildWnd->MoveWindow( m_rcChildArea );
			ti.pChildWnd->RedrawWindow();
		}
	}

	CGdipStatic::PreSubclassWindow();
}


void CFlatTabWnd::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	if ( nAction != UIS_SET )
		CGdipStatic::OnUpdateUIState(nAction, nUIElement);
}
