// FlatLabel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FlatLabel.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
// CFlatLabel

IMPLEMENT_DYNAMIC(CFlatLabel, CGdipStatic)

CFlatLabel::CFlatLabel()
{
	m_pImageIcon = NULL;
	m_dwTextColor = RGB(255,255,255);
	m_dwBorderColor = (DWORD)-1;
	m_nBorderWidth = 1;
	m_dwBodyColor = RGB(89,101,117);
	m_nFontHeight = 0;
	m_bIsBold = FALSE;
}

CFlatLabel::~CFlatLabel()
{
}


BEGIN_MESSAGE_MAP(CFlatLabel, CGdipStatic)
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()

void CFlatLabel::SetColor( COLORREF dwBodyColor, COLORREF dwTextColor, COLORREF dwBorderColor/*=-1*/ )
{
	m_dwBodyColor = dwBodyColor;
	m_dwTextColor = dwTextColor;
	m_dwBorderColor = dwBorderColor;
	if (GetSafeHwnd())
		Invalidate();
}

void CFlatLabel::SetBorderWidth( int nWidth )
{
	if (m_nBorderWidth == nWidth)
		return;

	m_nBorderWidth = nWidth;
	if (GetSafeHwnd())
		Invalidate();
}

void CFlatLabel::SetBold( BOOL bBold )
{
	if (m_bIsBold == bBold)
		return;

	m_bIsBold = bBold;
	if (GetSafeHwnd())
		Invalidate();
}

void CFlatLabel::SetFontHeight( int nHeight/*=0*/ )
{
	if (m_nFontHeight == nHeight)
		return;

	m_nFontHeight = nHeight;
	if (GetSafeHwnd())
		Invalidate();
}

void CFlatLabel::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	using namespace Gdiplus;

	Rect rectBody = rect;

	BOOL bDisabled = IsWindowEnabled() ? FALSE : TRUE;

	int nIconAreaWidth = 0;
	if (m_pImageIcon != NULL)
	{
		// draw icon area
		Rect rectIcon;
		rectIcon = rectBody;
		rectIcon.Width = (int)(rectBody.Height*1.1f+0.5f);

		Color colorIconArea( 70, 0, 0, 0 );
		g.FillRectangle( &SolidBrush(colorIconArea), rectIcon );

		int nIW = m_pImageIcon->GetWidth();
		int nIH = m_pImageIcon->GetHeight();
		int nIX = rectIcon.X + (rectIcon.Width-nIW)/2;
		int nIY = rectIcon.Y + (rectIcon.Height-nIH)/2;

		if (!bDisabled)
		{
			g.DrawImage( m_pImageIcon, nIX, nIY, 0, 0, nIW, nIH, UnitPixel );
		}
		else
		{
			// gray scale
			ImageAttributes ia;
			ColorMatrix matrix = {
				0.299f, 0.299f, 0.299f,   0.0f,  0.0f,
				0.588f, 0.588f, 0.588f,   0.0f,  0.0f,
				0.111f, 0.111f, 0.111f,   0.0f,  0.0f,
				0.0f,   0.0f,   0.0f,   1.0f,  0.0f,
				0.0f,   0.0f,   0.0f,   0.0f,  1.0f
			};
			ia.SetColorMatrix(&matrix);
			g.DrawImage( m_pImageIcon, Rect(nIX, nIY, nIW, nIH), 0, 0, nIW, nIH, UnitPixel, &ia );
		}

		nIconAreaWidth = rectIcon.Width;
	}

	Gdiplus::Font fontCaption( 
		CResourceManager::SystemFontFamily(), 
		m_nFontHeight == 0 ? (float)rectBody.Height*0.60f : (float)m_nFontHeight, 
		m_bIsBold ? FontStyleBold : FontStyleRegular, 
		UnitPixel );

	CString strCaption;
	GetWindowText( strCaption );
	BSTR bstrCaption = strCaption.AllocSysString();

	Color colorCaption;
	if (!bDisabled)
	{
		colorCaption.SetFromCOLORREF( m_dwTextColor );
	}
	else
	{
		BYTE cGray = (GetRValue(m_dwTextColor)+GetGValue(m_dwTextColor)+GetBValue(m_dwTextColor)) / 3;
		DWORD dwGrayTextColor = RGB(cGray, cGray, cGray);
		colorCaption.SetFromCOLORREF( dwGrayTextColor );
	}
	SolidBrush brushCaption(colorCaption);

	RectF rectCaptionF( (float)rectBody.X+nIconAreaWidth, (float)rectBody.Y, (float)rectBody.Width-nIconAreaWidth, (float)rectBody.Height );
	rectCaptionF.Inflate( -rectCaptionF.Height/8, 0 );

	UINT nStyle = GetStyle();
	StringFormat stringFormat;
	if ( (nStyle & 0x00F) == SS_LEFT )
		stringFormat.SetAlignment( StringAlignmentNear );
	else if ( (nStyle & 0x00F) == SS_RIGHT )
		stringFormat.SetAlignment( StringAlignmentFar );
	else
		stringFormat.SetAlignment( StringAlignmentCenter );
	if ( (nStyle & 0xF00) == SS_CENTERIMAGE )
		stringFormat.SetLineAlignment( StringAlignmentCenter );
	else
		stringFormat.SetLineAlignment( StringAlignmentNear );
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	RectF rectCaptionBBF;
	g.MeasureString( bstrCaption, -1, &fontCaption, rectCaptionF, &stringFormat, &rectCaptionBBF );

	float fY = rectCaptionF.Y + (rectCaptionF.Height-rectCaptionBBF.Height) / 2.f;

	rectCaptionF.Y = fY;
	rectCaptionF.Height = rectCaptionBBF.Height;

	g.SetSmoothingMode( SmoothingModeAntiAlias );
	g.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

	g.SetTextContrast(8);		// 0~12
	g.DrawString( bstrCaption, -1, &fontCaption, rectCaptionF, &stringFormat, &brushCaption );

	if (m_dwBorderColor != (DWORD)-1)
	{
		if (m_nBorderWidth > 0)
		{
			Color colorBorder;
			if (!bDisabled)
			{
				colorBorder.SetFromCOLORREF( m_dwBorderColor );
			}
			else
			{
				BYTE cGray = (GetRValue(m_dwBorderColor)+GetGValue(m_dwBorderColor)+GetBValue(m_dwBorderColor)) / 3;
				DWORD dwGrayBorderColor = RGB(cGray, cGray, cGray);
				colorBorder.SetFromCOLORREF( dwGrayBorderColor );
			}
			rectBody.Width--;
			rectBody.Height--;
			rectBody.Inflate(-m_nBorderWidth/2, -m_nBorderWidth/2);
			g.DrawRectangle( &Pen(colorBorder, (float)m_nBorderWidth), rectBody );
		}
	}

	::SysFreeString( bstrCaption );
}


void CFlatLabel::OnGdipEraseBkgnd( Gdiplus::Graphics& g, Rect rect )
{
	using namespace Gdiplus;

	BOOL bDisabled = IsWindowEnabled() ? FALSE : TRUE;
	Color colorBody;
	if (!bDisabled)
	{
		colorBody.SetFromCOLORREF(m_dwBodyColor);
	}
	else
	{
		BYTE cGray = (GetRValue(m_dwBodyColor)+GetGValue(m_dwBodyColor)+GetBValue(m_dwBodyColor)) / 3;
		DWORD dwGrayBodyColor = RGB(cGray, cGray, cGray);
		colorBody.SetFromCOLORREF(dwGrayBodyColor);
	}
	g.FillRectangle( &SolidBrush(colorBody), rect );
}

void CFlatLabel::PreSubclassWindow()
{
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	CGdipStatic::PreSubclassWindow();
}


void CFlatLabel::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	if ( nAction != UIS_SET )
		CGdipStatic::OnUpdateUIState(nAction, nUIElement);
}
