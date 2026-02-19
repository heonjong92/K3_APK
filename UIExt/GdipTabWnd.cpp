// GdipTabWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GdipTabWnd.h"
#include "GdiplusExt.h"

#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ID_TIMER_CHECKHOVER	(100)
#define ID_TIMER_BLINK		(200)

#define OFFSET_XY		(1)

namespace UIExt
{
const UINT WM_GDIP_TABWND_SELCHANGE = ::RegisterWindowMessage( _T("GDIP_TABWND_SELCHANGE") );
const UINT WM_GDIP_TABWND_SELCHANGED = ::RegisterWindowMessage( _T("GDIP_TABWND_SELCHANGED") );
}

using namespace UIExt;

// CGdipTabWnd

IMPLEMENT_DYNAMIC(CGdipTabWnd, CGdipStatic)
CGdipTabWnd::CGdipTabWnd()
{
	m_bIsHovered = FALSE;
	m_bIsMouseDown = FALSE;

	m_nActiveTabIdx = -1;
	m_nHoverTabIdx = -1;

	m_szTabHeader.cx = 90;
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

CGdipTabWnd::~CGdipTabWnd()
{
}


BEGIN_MESSAGE_MAP(CGdipTabWnd, CWnd)
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
END_MESSAGE_MAP()


void CGdipTabWnd::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	Color colorBackground;
	colorBackground.SetFromCOLORREF(m_dwBackgroundColor);
	g.FillRectangle( &SolidBrush(colorBackground), rect );
}

void CGdipTabWnd::SetBackgroundColor(DWORD dwColor)
{
	m_dwBackgroundColor = dwColor;
	Invalidate();
}

void CGdipTabWnd::OnDraw( Graphics& g, Rect rect )
{
	g.SetSmoothingMode( SmoothingModeAntiAlias );

	DrawTabs( g, rect );
}

CSize CGdipTabWnd::CalcTabSize( Graphics& g, const TabInfo& ti )
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

void CGdipTabWnd::SetTabText( INT_PTR nI, LPCTSTR lpszNewText )
{
	if ( m_aryTabInfo.GetCount() <= nI || nI < 0 )
	{
		return;
	}

	m_aryTabInfo.GetAt(nI).strCaption = lpszNewText;
}

INT_PTR CGdipTabWnd::IsNotifyTab( INT_PTR nI )
{
	for ( INT_PTR i=0 ; i<m_aryNotifyTabIdx.GetCount() ; i++ )
	{
		if ( m_aryNotifyTabIdx.GetAt(i) == nI )
			return i;
	}
	return -1;
}

void CGdipTabWnd::CalcChildArea( CRect rc )
{
	if ( IsHorzTab() )		// Horz, Top인 경우
	{
		m_rcChildArea.left = rc.left;
		m_rcChildArea.top = rc.top+m_szTabHeader.cy;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( OFFSET_XY*2, OFFSET_XY, OFFSET_XY, OFFSET_XY );
	}
	else // Vert, Left인 경우
	{
		m_rcChildArea.left = rc.left + m_szTabHeader.cx;
		m_rcChildArea.top = rc.top;
		m_rcChildArea.right = rc.right;
		m_rcChildArea.bottom = rc.bottom;
		m_rcChildArea.DeflateRect( OFFSET_XY*2, OFFSET_XY, OFFSET_XY, OFFSET_XY );
	}
}

void CGdipTabWnd::DrawTabs( Graphics& g, Rect rc )
{
#define TAB_INTERVAL	(0)
#define TAB_MARGIN		(10)
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

		CalcChildArea( rc );

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

		if (IsHideTab(nI) >= 0)
		{
			ti.rcArea.left = 0;
			ti.rcArea.top = 0;
			ti.rcArea.right = 0;
			ti.rcArea.bottom = 0;
			continue;
		}

		DrawTab( g, rectTab, ti, 
			m_nHoverTabIdx == nI ? TRUE : FALSE, 
			m_nActiveTabIdx == nI ? TRUE : FALSE, IsNotifyTab(nI) < 0 ? FALSE : TRUE, IsDisabledTab(nI) < 0 ? FALSE : TRUE );

		if (IsDisabledTab(nI) < 0)
		{
			ti.rcArea.left = rectTab.X;
			ti.rcArea.top = rectTab.Y;
			ti.rcArea.right = rectTab.GetRight();
			ti.rcArea.bottom = rectTab.GetBottom();
		}
		else
		{
			ti.rcArea.left = 0;
			ti.rcArea.top = 0;
			ti.rcArea.right = 0;
			ti.rcArea.bottom = 0;
		}

		if ( m_eTabDirection == TabDirectionHorzLeftTop )
		{
			rectTab.X += rectTab.Width + TAB_INTERVAL;		// interval: 5px
		}
		if ( m_eTabDirection == TabDirectionVertLeftTop )
		{
			rectTab.Y += rectTab.Height + TAB_INTERVAL;
		}
	}

	Pen BodyPen( Color(160, 160, 160), 1.f);
	Pen BodyShadowPen( Color(150, 160, 160, 160), 2.f);

	if ( IsHorzTab() )
	{
		int nTop = rectTab.GetBottom();
		if ( m_nActiveTabIdx >= 0 )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
			ti.rcArea;
			g.DrawLine( &BodyShadowPen, rc.X, nTop, ti.rcArea.left, nTop );
			g.DrawLine( &BodyShadowPen, ti.rcArea.right, nTop, rc.GetRight(), nTop );

		}
		else
		{
			g.DrawLine( &BodyShadowPen, rc.X, nTop, rc.GetRight(), nTop );
		}

		g.DrawLine( &BodyShadowPen, rc.GetRight(), nTop, rc.GetRight(), rc.GetBottom() );
		g.DrawLine( &BodyShadowPen, rc.GetRight(), rc.GetBottom(), rc.X, rc.GetBottom() );
		g.DrawLine( &BodyShadowPen, rc.X, rc.GetBottom(), rc.X, nTop );
	}
	else
	{
		int nLeft = rectTab.GetRight();
		if ( m_nActiveTabIdx >= 0 )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
			g.DrawLine( &BodyShadowPen, nLeft, rc.Y, nLeft, ti.rcArea.top );
			g.DrawLine( &BodyShadowPen, nLeft, ti.rcArea.bottom, nLeft, rc.GetBottom() );
		}
		else
		{
			g.DrawLine( &BodyShadowPen, nLeft, rc.Y, nLeft, rc.GetBottom() );
		}

		g.DrawLine( &BodyShadowPen, nLeft, rc.GetBottom(), rc.GetRight(), rc.GetBottom() );
		g.DrawLine( &BodyShadowPen, rc.GetRight(), rc.GetBottom(), rc.GetRight(), rc.Y );
		g.DrawLine( &BodyShadowPen, rc.GetRight(), rc.Y, nLeft, rc.Y );
	}

	if ( IsHorzTab() )
	{
		int nTop = rectTab.GetBottom();
		if ( m_nActiveTabIdx >= 0 )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
			ti.rcArea;
			g.DrawLine( &BodyPen, rc.X, nTop, ti.rcArea.left, nTop );
			g.DrawLine( &BodyPen, ti.rcArea.right, nTop, rc.GetRight(), nTop );

		}
		else
		{
			g.DrawLine( &BodyPen, rc.X, nTop, rc.GetRight(), nTop );
		}

		g.DrawLine( &BodyPen, rc.GetRight(), nTop, rc.GetRight(), rc.GetBottom() );
		g.DrawLine( &BodyPen, rc.GetRight(), rc.GetBottom(), rc.X, rc.GetBottom() );
		g.DrawLine( &BodyPen, rc.X, rc.GetBottom(), rc.X, nTop );
	}
	else
	{
		int nLeft = rectTab.GetRight();
		if ( m_nActiveTabIdx >= 0 )
		{
			TabInfo& ti = m_aryTabInfo.GetAt(m_nActiveTabIdx);
			g.DrawLine( &BodyPen, nLeft, rc.Y, nLeft, ti.rcArea.top );
			g.DrawLine( &BodyPen, nLeft, ti.rcArea.bottom, nLeft, rc.GetBottom() );
		}
		else
		{
			g.DrawLine( &BodyPen, nLeft, rc.Y, nLeft, rc.GetBottom() );
		}

		g.DrawLine( &BodyPen, nLeft, rc.GetBottom(), rc.GetRight(), rc.GetBottom() );
		g.DrawLine( &BodyPen, rc.GetRight(), rc.GetBottom(), rc.GetRight(), rc.Y );
		g.DrawLine( &BodyPen, rc.GetRight(), rc.Y, nLeft, rc.Y );
	}

}

void CGdipTabWnd::DrawTab( Graphics& g, Rect& rectTab, const TabInfo& ti, BOOL bHover, BOOL bActive, BOOL bNotify, BOOL bDisabled )
{
	GraphicsPath TabPath;
	GraphicsPath TabS;

	g.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

	int nDiameter = 3 << 1;
	Rect arcRect( 0, 0, nDiameter, nDiameter );

	if ( IsHorzTab() )
	{
		// Horz-Tab
		TabPath.AddLine( rectTab.X, rectTab.GetBottom(), rectTab.X, rectTab.Y+nDiameter );
		arcRect.X = rectTab.X;
		arcRect.Y = rectTab.Y;
		TabPath.AddArc( arcRect, 180, 90 );
		TabS.AddArc( arcRect, 180, 90 );
		arcRect.X = rectTab.GetRight()-nDiameter;
		arcRect.Y = rectTab.Y;
		TabPath.AddArc( arcRect, 270, 90 );
		TabS.AddArc( arcRect, 270, 90 );
		TabPath.AddLine( rectTab.GetRight(), rectTab.Y+nDiameter, rectTab.GetRight(), rectTab.GetBottom() );
	}
	else
	{
		// Vert-Tab
		TabPath.AddLine( rectTab.GetRight(), rectTab.Y, rectTab.X+nDiameter, rectTab.Y );
		arcRect.X = rectTab.X;
		arcRect.Y = rectTab.Y;
		TabPath.AddArc( arcRect, 270, -90 );
		TabS.AddArc( arcRect, 270, -90 );
		arcRect.X = rectTab.X;
		arcRect.Y = rectTab.GetBottom()-nDiameter;
		TabPath.AddArc( arcRect, 180, -90 );
		TabS.AddArc( arcRect, 180, -90 );
		TabPath.AddLine( rectTab.GetLeft()+nDiameter, rectTab.GetBottom(), rectTab.GetRight(), rectTab.GetBottom() );
	}

	Color colorBody;

	if ( bNotify && (m_nBlinkCount % 2 == 0) )
	{
		colorBody = Color(252, 209, 211);//Color(237, 28, 36);
	}
	else
	{
		if ( bActive )
			colorBody = Color(255, 255, 255);
		else
			colorBody = Color::WhiteSmoke;
	}

	if (bDisabled)
		colorBody = Color(220,220,220);

	g.FillPath( &SolidBrush( colorBody ), &TabPath );

	Color colorStart = bActive ? ti.colorTab : (bHover ? Color(200, 255, 190, 60) :Color(200, 255, 255, 255));
	Color colorEnd = Color(200, 255, 255, 255);
	Rect rectTabTip;
	TabS.GetBounds( &rectTabTip );
	LinearGradientBrush TabTipBrush( rectTabTip, colorStart, colorEnd, IsHorzTab() ? 90.f : 180.f );

	g.DrawPath( &Pen(Color(100, 160, 160, 160), 2.f), &TabPath );
	g.DrawPath( &Pen( Color(160, 160, 160), 1.f), &TabPath );

	g.FillPath( &TabTipBrush, &TabS );
	if ( bActive || bHover )
		g.DrawPath( &Pen( colorStart, 1.f), &TabS );

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

	Gdiplus::Font fntCaption( CResourceManager::SystemFontFamily(), (float)m_nTextHeight, bActive ? FontStyleBold : FontStyleRegular, UnitPixel );

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
	g.DrawString( bstrCaption, -1, &fntCaption, rectText, &stringFormat, &SolidBrush( Color(80, 80, 80) ) );

	::SysFreeString( bstrCaption );
}

void CGdipTabWnd::OnMouseMove(UINT nFlags, CPoint point) 
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

void CGdipTabWnd::OnMouseEnter()
{
	Invalidate();
}

LRESULT CGdipTabWnd::OnMouseLeave(WPARAM, LPARAM)
{
    //ASSERT( m_bIsHovered );
    m_bIsHovered = FALSE;
	m_nHoverTabIdx = -1;

	HCURSOR hNormalCursor = ::LoadCursor( NULL, IDC_ARROW );
	::SetCursor(hNormalCursor);

    Invalidate();
	return 0;
}

void CGdipTabWnd::NotifyTab( INT_PTR nI )
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

void CGdipTabWnd::ResetNotify()
{
	m_bNotifyTab = FALSE;
	m_aryNotifyTabIdx.RemoveAll();
	KillTimer( ID_TIMER_BLINK );
	Invalidate();
}

void CGdipTabWnd::EnableTabChange( BOOL bEnable )
{
	m_bEnableTabChange = bEnable;
}

INT_PTR CGdipTabWnd::IsDisabledTab( INT_PTR nI )
{
	for ( INT_PTR i=0 ; i<m_aryDisabledTabIdx.GetCount() ; i++ )
	{
		if ( m_aryDisabledTabIdx.GetAt(i) == nI )
			return i;
	}
	return -1;
}

INT_PTR CGdipTabWnd::IsHideTab( INT_PTR nI )
{
	for ( INT_PTR i=0 ; i<m_aryHideTabIdx.GetCount() ; i++ )
	{
		if ( m_aryHideTabIdx.GetAt(i) == nI )
			return i;
	}
	return -1;
}

void CGdipTabWnd::EnableTab( INT_PTR nI, BOOL bEnable/*=TRUE*/ )
{
	if (bEnable)
	{
		INT_PTR nN = 0;
		if ( (nN = IsDisabledTab(nI)) >= 0 )
		{
			m_aryDisabledTabIdx.RemoveAt(nN);
		}
	}
	else
	{
		INT_PTR nN = 0;
		if ( (nN = IsDisabledTab(nI)) >= 0 )
		{
		}
		else
		{
			m_aryDisabledTabIdx.Add(nI);
		}
	}

	if (GetSafeHwnd())
		Invalidate();
}

void CGdipTabWnd::ShowTab( INT_PTR nI, BOOL bShow/*=TRUE*/ )
{
	if (bShow)
	{
		INT_PTR nN = 0;
		if ( (nN = IsHideTab(nI)) >= 0 )
		{
			m_aryHideTabIdx.RemoveAt(nN);
		}
	}
	else
	{
		INT_PTR nN = 0;
		if ( (nN = IsHideTab(nI)) >= 0 )
		{
		}
		else
		{
			m_aryHideTabIdx.Add(nI);
		}
	}

	if (GetSafeHwnd())
		Invalidate();
}

void CGdipTabWnd::EnableAllTabs()
{
	BOOL bUpdate = m_aryDisabledTabIdx.GetCount() == 0 ? FALSE : TRUE;
	m_aryDisabledTabIdx.RemoveAll();

	if (bUpdate && GetSafeHwnd())
	{
		Invalidate();
	}
}

void CGdipTabWnd::ShowAllTabs()
{
	BOOL bUpdate = m_aryHideTabIdx.GetCount() == 0 ? FALSE : TRUE;
	m_aryHideTabIdx.RemoveAll();

	if (bUpdate && GetSafeHwnd())
	{
		Invalidate();
	}
}

void CGdipTabWnd::SelectTab( INT_PTR nI, BOOL bNotify/*=TRUE*/ )
{
	if ( !m_bEnableTabChange )
		return;

	if ( IsDisabledTab(nI) >= 0 )
		return;
	if ( IsHideTab(nI) >= 0 )
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
		if ( pParent && *pParent && bNotify )
			pParent->SendMessage( WM_GDIP_TABWND_SELCHANGE, MAKEWPARAM(nI, 0), 0 );

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
			ti.pChildWnd->RedrawWindow();
		}

		if ( pParent && *pParent && bNotify )
			pParent->SendMessage( WM_GDIP_TABWND_SELCHANGED, MAKEWPARAM(nI, 0), 0 );

		Invalidate();
	}
}

void CGdipTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CGdipTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
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

void CGdipTabWnd::OnKillFocus(CWnd* pNewWnd) 
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

void CGdipTabWnd::OnSetFocus(CWnd* pOldWnd)
{
	CGdipStatic::OnSetFocus(pOldWnd);
}

BOOL CGdipTabWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

void CGdipTabWnd::OnSize(UINT nType, int cx, int cy)
{
	CGdipStatic::OnSize(nType, cx, cy);

	if ( cx <= 0 || cy <= 0 ) return;

	if ( m_pBitmap != NULL ) delete m_pBitmap;
	m_pBitmap = new CBitmap;
	ASSERT( m_pBitmap );
	CClientDC dc(this);
	VERIFY( m_pBitmap->CreateCompatibleBitmap(&dc, cx, cy) );

	CRect rc;
	GetClientRect( rc );

	CalcChildArea( rc );

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

void CGdipTabWnd::SetTabDirection( TabDirection eDirection, int nBarHeight, int nBarOffset/*=0*/ ) 
{
	m_eTabDirection = eDirection;
	IsHorzTab() ? m_szTabHeader.cy = nBarHeight : m_szTabHeader.cx = nBarHeight;
	m_nBarOffset = nBarOffset;

	CRect rc;
	GetClientRect( rc );

	CalcChildArea( rc );

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

void CGdipTabWnd::SetTabWidth( int nBarWidth/*=0*/ )	// 0: variable, 
{
	m_nBarWidth = nBarWidth;

	CRect rc;
	GetClientRect( rc );

	CalcChildArea( rc );

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

INT_PTR CGdipTabWnd::AddTab( LPCTSTR lpszCaption, CWnd* pChildWnd, Color colorTab/*=Color(200, 255, 60, 60)*/, Image* pIconImage/*=NULL*/ )
{
	if ( pChildWnd )
		pChildWnd->ModifyStyle( 0, WS_CLIPSIBLINGS );
	TabInfo ti;
	ti.pChildWnd = pChildWnd;
	ti.strCaption = lpszCaption;
	ti.pIconImage = NULL;
	ti.colorTab = colorTab;
	return m_aryTabInfo.Add( ti );
}

void CGdipTabWnd::RemoveTab( INT_PTR nI )
{
	m_aryTabInfo.RemoveAt(nI);
	m_nActiveTabIdx = -1;
	m_nHoverTabIdx = -1;
	m_bIsHovered = FALSE;
	m_bIsMouseDown = FALSE;
	for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI++ )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(nI);
		ti.pChildWnd->ShowWindow( SW_HIDE );
	}
}

void CGdipTabWnd::ResetTab()
{
	m_aryTabInfo.RemoveAll();
	m_aryHideTabIdx.RemoveAll();
	m_aryDisabledTabIdx.RemoveAll();
	m_nActiveTabIdx = -1;
	m_nHoverTabIdx = -1;
	m_bIsHovered = FALSE;
	m_bIsMouseDown = FALSE;
	for ( INT_PTR nI = 0 ; nI < m_aryTabInfo.GetCount() ; nI++ )
	{
		TabInfo& ti = m_aryTabInfo.GetAt(nI);
		ti.pChildWnd->ShowWindow( SW_HIDE );
	}
}

int CGdipTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle( 0, WS_CLIPCHILDREN|SS_NOTIFY );
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	SetTimer( ID_TIMER_CHECKHOVER, 100, NULL );

	return 0;
}

void CGdipTabWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CGdipStatic::OnShowWindow(bShow, nStatus);

	//m_nHoverIndex = -1;
}


void CGdipTabWnd::OnDestroy()
{
	CGdipStatic::OnDestroy();

	m_aryTabInfo.RemoveAll();
	m_nActiveTabIdx = -1;
	m_nHoverTabIdx = -1;
	m_bIsHovered = FALSE;
	m_bIsMouseDown = FALSE;
}

void CGdipTabWnd::OnTimer(UINT_PTR nIDEvent)
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

void CGdipTabWnd::PreSubclassWindow()
{
	ModifyStyle( 0, WS_CLIPCHILDREN|SS_NOTIFY );
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	SetTimer( ID_TIMER_CHECKHOVER, 100, NULL );

	CRect rc;
	GetClientRect( rc );

	CalcChildArea( rc );

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
