// FlatButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FlatButton.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using namespace UIExt;

// CFlatButton

IMPLEMENT_DYNAMIC(CFlatButton, CButton)

CFlatButton::CFlatButton()
{
	m_pBitmap = NULL;
	m_dwBackgroundColor = RGB(255, 255, 255);
	m_dwNormalColor = RGB(80, 106, 126);
	m_dwHoverColor = RGB(80, 106, 126);
	m_dwBorderColor = (DWORD)-1;
	m_dwActiveColor = RGB(21, 131, 203);
	m_dwDisabledColor = RGB(180, 180, 180);
	m_dwTextColor = RGB(255, 255, 255);
	m_bIsHovered = FALSE;
	m_bIsChecked = FALSE;
	m_bShowFocusRect = FALSE;
	m_bIsBold = TRUE;

	m_nBorderWidth = 0;
	m_nFontHeight = 12;
	m_nIconIndex = -1;

	m_pImageIcon = NULL;
	m_bInvertIconColor = FALSE;

	m_bIgnoreDblClick = FALSE;
}

CFlatButton::~CFlatButton()
{
	if ( m_pBitmap )
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	if ( m_pImageIcon )
	{
		delete m_pImageIcon;
		m_pImageIcon = NULL;
	}
}


BEGIN_MESSAGE_MAP(CFlatButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_MESSAGE (BM_GETCHECK, OnGetCheck)
	ON_MESSAGE (BM_SETCHECK, OnSetCheck)
	ON_WM_GETDLGCODE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, &CFlatButton::OnBnClicked)
	//ON_CONTROL_REFLECT(BN_CLICKED, &CFlatButton::OnBnClicked)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CFlatButton::PreSubclassWindow()
{
	m_nButtonType = GetButtonStyle() & BS_TYPEMASK;
	ModifyStyle( 0, BS_OWNERDRAW );

	CButton::PreSubclassWindow();
}

void CFlatButton::SetBackgroundColor( DWORD dwColor )
{
	if (m_dwBackgroundColor == dwColor)
		return;
	m_dwBackgroundColor = dwColor;
	if ( GetSafeHwnd() )
		Invalidate();
}

void CFlatButton::SetBold( BOOL bBold )
{
	if (m_bIsBold == bBold)
		return;
	m_bIsBold = bBold;
	if ( GetSafeHwnd() )
		Invalidate();
}

void CFlatButton::SetFontHeight( int nHeight/*=0*/ )
{
	if (m_nFontHeight == nHeight)
		return;

	m_nFontHeight = nHeight;
	if (GetSafeHwnd())
		Invalidate();
}

void CFlatButton::SetColor( DWORD dwNormalColor, DWORD dwHoverColor, DWORD dwActiveColor, DWORD dwDisabledColor, DWORD dwTextColor, DWORD dwBorderColor/*=(DWORD)-1*/ )
{
	m_dwNormalColor = dwNormalColor;
	m_dwBorderColor = dwBorderColor;
	m_dwHoverColor = dwHoverColor;
	m_dwActiveColor = dwActiveColor;
	m_dwTextColor = dwTextColor;
	m_dwDisabledColor = dwDisabledColor;
	if ( GetSafeHwnd() )
		Invalidate();
}

void CFlatButton::SetButtonColor( DWORD dwNormalColor, DWORD dwActiveColor/*=(DWORD)-1*/ )
{
	m_dwNormalColor = dwNormalColor;
	m_dwHoverColor = m_dwNormalColor;
	if (dwActiveColor == (DWORD)-1)
	{
		m_dwActiveColor = m_dwNormalColor;
	}
	else
	{
		m_dwActiveColor = dwActiveColor;
	}
	if ( GetSafeHwnd() )
		Invalidate();
}

void CFlatButton::SetBorderWidth(int nWidth)
{
	m_nBorderWidth = nWidth;
	if ( GetSafeHwnd() )
		Invalidate();
}

void CFlatButton::SetIcon( HMODULE hRes, LPCTSTR lpszResType, UINT nId, int nIconIndex/*=-1*/ )
{
	if (m_pImageIcon)
		delete m_pImageIcon;
	m_pImageIcon = ::GdipLoadImageFromRes( hRes, lpszResType, nId );
	if ( GetSafeHwnd() )
		Invalidate();
	m_nIconIndex = nIconIndex;
}

void CFlatButton::ShowFocusRect( BOOL bShow )
{
	m_bShowFocusRect = bShow;
}

void CFlatButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rcBody = lpDrawItemStruct->rcItem;
	
	CDC dc;
	dc.Attach( lpDrawItemStruct->hDC );

	if ( m_MemDC.GetSafeHdc() == NULL )
	{
		m_MemDC.CreateCompatibleDC( &dc );
	}

	if ( !m_pBitmap )
	{
		m_pBitmap = new CBitmap;
		ASSERT( m_pBitmap );
		VERIFY( m_pBitmap->CreateCompatibleBitmap( &dc, rcBody.Width(), rcBody.Height() ) );
	}

	ASSERT( m_pBitmap );
	CBitmap* pOldBitmap = m_MemDC.SelectObject( m_pBitmap );
	m_MemDC.FillSolidRect( rcBody, m_dwBackgroundColor );

	BOOL bEnabled = lpDrawItemStruct->itemState & ODS_DISABLED ? FALSE : TRUE;
	BOOL bFocused = lpDrawItemStruct->itemState & ODS_FOCUS ? TRUE : FALSE;
	BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED ? TRUE : FALSE;
	BOOL bChecked = m_bIsChecked;

	DrawButton( &m_MemDC, rcBody, bEnabled, bFocused, bChecked, bSelected );

	m_MemDC.SetMapMode( MM_TEXT );
	m_MemDC.SetViewportOrg( 0, 0 );

	dc.BitBlt( rcBody.left, rcBody.top, rcBody.Width(), rcBody.Height(), 
		&m_MemDC, 0, 0, SRCCOPY );

	m_MemDC.SelectObject( pOldBitmap );	

	dc.Detach();
}



void CFlatButton::DrawFocusRect( Graphics& g, Rect& rectBody )
{
	g.SetSmoothingMode( SmoothingModeDefault );
	Rect rectFocus;
	Color colorFocus( 120, 255, 255, 255 );

	rectFocus = rectBody;
	rectFocus.Inflate( -2, -2 );

	Pen penFocus( colorFocus, 1 );
	penFocus.SetDashStyle(DashStyleDashDot);
	g.DrawRectangle( &penFocus, rectFocus );
}

void CFlatButton::DrawButton( CDC* pDC, CRect& rcBody, BOOL bEnabled, BOOL bFocused, BOOL bChecked, BOOL bSelected )
{
	Graphics g( pDC->GetSafeHdc() );

	//g.SetSmoothingMode( SmoothingModeAntiAlias );
	//g.SetCompositingQuality(CompositingQualityHighQuality);

	CString strCaption;
	GetWindowText( strCaption );

	int nOffset = 0;
	if (g.GetSmoothingMode() == SmoothingModeAntiAlias)
		nOffset = 1;
	Rect rectBody( rcBody.left, rcBody.top, rcBody.Width()-nOffset, rcBody.Height()-nOffset );

	int nAlphaOffset = 0;
	if ( m_bIsHovered || bChecked )
		nAlphaOffset = 35;

	DWORD dwColor;
	if ( !bEnabled )
	{
		dwColor = m_dwDisabledColor;
	}
	else
	{
		if ( bSelected || bChecked )
		{
			dwColor = m_dwActiveColor;
		}
		else
		{
			if (m_bIsHovered)
				dwColor = m_dwHoverColor;
			else
				dwColor = m_dwNormalColor;
		}
	}

	Color colorBody;
	colorBody.SetFromCOLORREF( dwColor );

	SolidBrush brushBody(colorBody);
	g.FillRectangle( &brushBody, rectBody );

	int nIconAreaWidth = 0;
	if (m_pImageIcon != NULL)
	{
		// draw icon area
		Rect rectIcon;
		rectIcon = rectBody;
		if (strCaption.IsEmpty())
		{
		}
		else
		{
			rectIcon.Width = (int)(rectBody.Height*1.1f+0.5f);
			Color colorIconArea( 70, 0, 0, 0 );
			g.FillRectangle( &SolidBrush(colorIconArea), rectIcon );
		}

		int nIconCount = 0;
		int nIconIndex = 0;
		int nIW = m_pImageIcon->GetWidth();
		int nIH = m_pImageIcon->GetHeight();
		if (m_nIconIndex >= 0)
		{
			nIconCount = nIW / nIH;
			nIconIndex = m_nIconIndex;
			if (nIconIndex < 0 || nIconIndex >= nIconCount)
				nIconIndex = 0;
			nIW = nIH;
		}

		int nIX = rectIcon.X + (rectIcon.Width-nIW)/2;
		int nIY = rectIcon.Y + (rectIcon.Height-nIH)/2;

		if (bEnabled)
		{
			if (m_bInvertIconColor)
			{
				Gdiplus::ImageAttributes ia;

				ColorMatrix matrix = {
					-1.0f, 0.0f,   0.0f,   0.0f,  0.0f,
					0.0f,   -1.0f, 0.0f,   0.0f,  0.0f,
					0.0f,   0.0f,   -1.0f, 0.0f,  0.0f,
					0.0f,   0.0f,   0.0f,   1.0f,  0.0f,
					1.0f,   1.0f,   1.0f,   0.0f,  1.0f
				};
				ia.SetColorMatrix(&matrix);

				if (bSelected || bChecked)
					g.DrawImage( m_pImageIcon, Rect(nIX, nIY, nIW, nIH), nIW*nIconIndex, 0, nIW, nIH, UnitPixel, &ia );
				else
					g.DrawImage( m_pImageIcon, Rect(nIX, nIY, nIW, nIH), nIW*nIconIndex, 0, nIW, nIH, UnitPixel );
			}
			else
			{
				g.DrawImage( m_pImageIcon, Rect(nIX, nIY, nIW, nIH), nIW*nIconIndex, 0, nIW, nIH, UnitPixel );
			}
		}
		else
		{
			ImageAttributes ia;
			ColorMatrix matrix = {
				0.299f, 0.299f, 0.299f,   0.0f,  0.0f,
				0.588f, 0.588f, 0.588f,   0.0f,  0.0f,
				0.111f, 0.111f, 0.111f,   0.0f,  0.0f,
				0.0f,   0.0f,   0.0f,   1.0f,  0.0f,
				0.0f,   0.0f,   0.0f,   0.0f,  1.0f
			};
			ia.SetColorMatrix(&matrix);

			g.DrawImage( m_pImageIcon, Rect(nIX, nIY, nIW, nIH), nIW*nIconIndex, 0, nIW, nIH, UnitPixel, &ia );
		}

		nIconAreaWidth = rectIcon.Width;
	}

	//

	if ( bEnabled )
	{
		if ( bSelected )
		{
			Color colorDarken( 20, 0, 0, 0 );
			g.FillRectangle( &SolidBrush(colorDarken), rectBody );
		}
		else if ( m_bIsHovered )
		{
			Color colorLighten( 20, 255, 255, 255 );
			g.FillRectangle( &SolidBrush(colorLighten), rectBody );
		}

		if ( m_bShowFocusRect && bFocused )
		{
			DrawFocusRect( g, rectBody );
		}
	}

	float fFontHeight = m_nFontHeight == 0 ? (float)rectBody.Height*0.30f : (float)m_nFontHeight;

	Gdiplus::Font fontCaption( CResourceManager::SystemFontFamily(), fFontHeight, m_bIsBold ? FontStyleBold : FontStyleRegular, UnitPixel );

	BSTR bstrCaption = strCaption.AllocSysString();

	Color colorCaption;
	colorCaption.SetFromCOLORREF( m_dwTextColor );
	SolidBrush brushCaption(colorCaption);

	RectF rectCaptionF( (float)rectBody.X+nIconAreaWidth, (float)rectBody.Y, (float)rectBody.Width-nIconAreaWidth, (float)rectBody.Height );
	rectCaptionF.Inflate( -rectCaptionF.Height/8, 0 );

	UINT nBtnStyle = GetStyle();
	StringFormat stringFormat;
	if ( (nBtnStyle & 0xF00) == BS_LEFT )
		stringFormat.SetAlignment( StringAlignmentNear );
	else if ( (nBtnStyle & 0xF00) == BS_RIGHT )
		stringFormat.SetAlignment( StringAlignmentFar );
	else
		stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	RectF rectCaptionBBF;
	g.MeasureString( bstrCaption, -1, &fontCaption, rectCaptionF, &stringFormat, &rectCaptionBBF );

	float fY = rectCaptionF.Y + (rectCaptionF.Height-rectCaptionBBF.Height) / 2.f;

	rectCaptionF.Y = fY;
	rectCaptionF.Height = rectCaptionBBF.Height;

	g.SetSmoothingMode( SmoothingModeAntiAlias );
	if (fFontHeight > 16.f)
		g.SetTextRenderingHint( TextRenderingHintAntiAlias );
	else
		g.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

	g.SetTextContrast(8);		// 0~12
	g.DrawString( bstrCaption, -1, &fontCaption, rectCaptionF, &stringFormat, &brushCaption );

	if (m_dwBorderColor != (DWORD)-1)
	{
		if (m_nBorderWidth > 0)
		{
			Color colorBorder;
			colorBorder.SetFromCOLORREF( m_dwBorderColor );
			rectBody.Width--;
			rectBody.Height--;
			rectBody.Inflate(-m_nBorderWidth/2, -m_nBorderWidth/2);
			g.DrawRectangle( &Pen(colorBorder, (float)m_nBorderWidth), rectBody );
		}
	}

	::SysFreeString( bstrCaption );
}

void CFlatButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);
}


void CFlatButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIgnoreDblClick)
	{
		m_bIgnoreDblClick = FALSE;
		return;
	}
	UINT nType = m_nButtonType;
	if ( BS_CHECKBOX == (BS_CHECKBOX & nType) || 
		BS_AUTOCHECKBOX == (BS_AUTOCHECKBOX & nType) ||
		BS_RADIOBUTTON == (BS_RADIOBUTTON & nType) || 
		BS_AUTORADIOBUTTON == (BS_AUTORADIOBUTTON & nType) ) 
	{
		CRect rc;
		GetWindowRect( rc );
		ScreenToClient(rc);
		if ( rc.PtInRect(point) )
			SetCheck( !GetCheck() );
	}

	CButton::OnLButtonUp(nFlags, point);
}

void CFlatButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bIgnoreDblClick = TRUE;

	CButton::OnLButtonDblClk(nFlags, point);
}

void CFlatButton::OnMouseMove(UINT nFlags, CPoint point)
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

	CButton::OnMouseMove(nFlags, point);
}


void CFlatButton::OnMouseLeave()
{
    m_bIsHovered = FALSE;

	Invalidate();

	CButton::OnMouseLeave();
}

void CFlatButton::OnMouseEnter()
{
	m_bIsHovered = TRUE;

	Invalidate();
}

UINT CFlatButton::OnGetDlgCode()
{
	UINT nCode = CButton::OnGetDlgCode();
	
	// radio support
    UINT nType = m_nButtonType;
	if (BS_RADIOBUTTON == (BS_RADIOBUTTON & nType) ||
		BS_AUTORADIOBUTTON == (BS_AUTORADIOBUTTON & nType)) 
	{
		nCode |= DLGC_RADIOBUTTON;
	}
	else 
	{
		//nCode |= DLGC_UNDEFPUSHBUTTON;
		// DLGC_DEFPUSHBUTTON - default button
    }
	
	return nCode;
}

void CFlatButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);

	if ( cx <= 0 || cy <= 0 ) return;

	if ( m_pBitmap != NULL ) delete m_pBitmap;
	m_pBitmap = new CBitmap;
	ASSERT( m_pBitmap );
	CClientDC dc(this);
	VERIFY( m_pBitmap->CreateCompatibleBitmap(&dc, cx, cy) );
}

BOOL CFlatButton::OnEraseBkgnd(CDC* pDC)
{
	if ( !m_pBitmap ) return CWnd::OnEraseBkgnd(pDC);
	if ( !m_MemDC.GetSafeHdc() ) return CWnd::OnEraseBkgnd(pDC);
	CBitmap * pOldBitmap = m_MemDC.SelectObject(m_pBitmap);
	CRect rect;
	GetClientRect(rect);
	m_MemDC.FillSolidRect(rect, m_dwBackgroundColor);
	m_MemDC.SelectObject(pOldBitmap);

	return TRUE;
}

LRESULT CFlatButton::OnGetCheck( WPARAM wParam, LPARAM lParam ) 
{
	UINT nType = m_nButtonType;
	if ( BS_CHECKBOX == (BS_CHECKBOX & nType) || 
		BS_AUTOCHECKBOX == (BS_AUTOCHECKBOX & nType) ||
		BS_RADIOBUTTON == (BS_RADIOBUTTON & nType) || 
		BS_AUTORADIOBUTTON == (BS_AUTORADIOBUTTON & nType) ) 
	{
		if ( FALSE == m_bIsChecked ) 
		{
			return BST_UNCHECKED;
		}
		else 
		{
			return BST_CHECKED;
		}
	}
	return 0;
}

LRESULT CFlatButton::OnSetCheck( WPARAM wParam, LPARAM lParam )
{
	int nCheck = (int)wParam;
	if ( BST_CHECKED == nCheck ) 
	{
		m_bIsChecked = TRUE;
	}
	else if ( BST_UNCHECKED == nCheck) 
	{
		m_bIsChecked = FALSE;
	}

	Invalidate();

	return 0;
}


BOOL CFlatButton::OnBnClicked()
{
	UINT nType = m_nButtonType;

	// radio support
	if( BS_RADIOBUTTON == (BS_RADIOBUTTON & nType) ||
		BS_AUTORADIOBUTTON == (BS_AUTORADIOBUTTON & nType) ) 
	{
        SendMessage( BM_SETCHECK, BST_CHECKED, 0 );

        // Start by searching forwards
        HWND hWndDlg = GetParent ()->m_hWnd;
        CWnd* pWnd = FromHandle( ::GetNextDlgGroupItem(hWndDlg, this->m_hWnd, FALSE) );
        while( NULL != pWnd && this != pWnd ) 
		{
            pWnd->SendMessage( BM_SETCHECK, BST_UNCHECKED, 0 );
            pWnd = FromHandle( ::GetNextDlgGroupItem(hWndDlg, pWnd->m_hWnd, FALSE) );
        }

        // Then search backwards
        pWnd = FromHandle( ::GetNextDlgGroupItem(hWndDlg, this->m_hWnd, TRUE) );
        while (NULL != pWnd && this != pWnd) 
		{
            pWnd->SendMessage( BM_SETCHECK, BST_UNCHECKED, 0 );
            pWnd = FromHandle( ::GetNextDlgGroupItem(hWndDlg, pWnd->m_hWnd, TRUE) );
        }
	}

	return FALSE;
}
