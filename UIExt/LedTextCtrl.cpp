// LedTextCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LedTextCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace Gdiplus;
using namespace UIExt;

/////////////////////////////////////////////////////////////////////////////
// CLedTextCtrl

CLedTextCtrl::CLedTextCtrl() : WIDTHHEIGHTRATIO(0.5f)
{
	m_bIsCacheBuild = FALSE;
	m_nBorderWidth = 1;
	m_colBorderColor = 0xFF303030;//Color.Gray;
	m_nCornerRadius = 5;
	m_nCharacterNumber = 6;
	m_fBevelRate = 0.25f;
	m_colFadedColor = 0xFF202020;//Color.DimGray;
	m_colCustomBk1 = Color::Black;
	m_colCustomBk2 = 0xFF202020;//Color.DimGray;
	m_fWidthSegWidthRatio = 0.2f;
	m_fWidthIntervalRatio = 0.05f;
	m_enumAlign = TA_Right;
	m_bRoundRect = FALSE;
	m_bGradientBackground = TRUE;
	m_bShowHighlight = TRUE;
	m_nHighlightOpaque = 50;
	m_colForeColor = Color::LightGreen;
	m_colBackColor = Color::Transparent;
	m_strText = _T("");
}

CLedTextCtrl::~CLedTextCtrl()
{
	DestoryCache();
}


BEGIN_MESSAGE_MAP(CLedTextCtrl, UIExt::CGdipStatic)
	//{{AFX_MSG_MAP(CLedTextCtrl)
	//ON_WM_ERASEBKGND()
	//ON_WM_PAINT()
	//ON_WM_CREATE()
	//ON_WM_DESTROY()
	//ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLedTextCtrl message handlers
void CLedTextCtrl::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	g.FillRectangle( &SolidBrush(m_colCustomBk1), rect );
}

void CLedTextCtrl::SetForeColor( DWORD dwColor )
{
	if ( dwColor == m_colForeColor.ToCOLORREF() )
		return;
	m_colForeColor.SetFromCOLORREF( dwColor );

	Invalidate();
}

void CLedTextCtrl::SetBorderWidth( int nBorderWidth )
{
	if ( m_nBorderWidth == nBorderWidth )
		return;
	if ( nBorderWidth >= 0 && nBorderWidth <= 5 )
	{
		m_nBorderWidth = nBorderWidth;
		Invalidate();
	}
}

void CLedTextCtrl::SetBorderColor( DWORD dwColor )
{
	if ( dwColor == m_colBorderColor.ToCOLORREF() )
		return;
	m_colBorderColor.SetFromCOLORREF(dwColor);
	
	Invalidate();
}

void CLedTextCtrl::SetHighlightOpaque( int nHighlightOpaque )
{
	if ( nHighlightOpaque <= 100 )
	{
		if ( m_nHighlightOpaque == nHighlightOpaque )
			return;
		m_nHighlightOpaque = nHighlightOpaque;
		
		Invalidate();
	}
}

void CLedTextCtrl::SetShowHighlight( BOOL bShow )
{
	if ( m_bShowHighlight == bShow )
		return;
	m_bShowHighlight = bShow;
	
	Invalidate();	
}

void CLedTextCtrl::SetCornerRadius( int nCornerRadius )
{
	if ( nCornerRadius >= 1 && nCornerRadius <= 10 )
	{
		if ( m_nCornerRadius == nCornerRadius )
			return;
		m_nCornerRadius = nCornerRadius;
		
		Invalidate();
	}
}

void CLedTextCtrl::SetGradientBackground( BOOL bSet )
{
	if ( m_bGradientBackground == bSet )
		return;
	m_bGradientBackground = bSet;
	
	Invalidate();
}

void CLedTextCtrl::SetBackColor1( DWORD dwColor )
{
	m_colCustomBk1.SetFromCOLORREF( dwColor );

	Invalidate();
}

void CLedTextCtrl::SetBackColor2( DWORD dwColor )
{
	m_colCustomBk2.SetFromCOLORREF( dwColor );

	Invalidate();
}

void CLedTextCtrl::SetRoundCorner( BOOL bSet )
{
	if ( m_bRoundRect == bSet )
		return;
	m_bRoundRect = bSet;
	
	Invalidate();
}

void CLedTextCtrl::SetSegmentIntervalRatio( int nSeqInterRatio )
{
	if ( nSeqInterRatio >= 0 && nSeqInterRatio <= 100 )
	{
		m_fWidthIntervalRatio = 0.01f + nSeqInterRatio * 0.001f;
		
		// need rebuild cache
		m_bIsCacheBuild = FALSE;
		Invalidate();
		
	}
}

void CLedTextCtrl::SetTextAlignment( TextAlignment eAlign )
{
	m_enumAlign = eAlign;
	
	Invalidate();
}

void CLedTextCtrl::SetSegmentWidthRatio( int nSeqWidthRatio )
{
	if ( nSeqWidthRatio >= 0 && nSeqWidthRatio <= 100 )
	{
		m_fWidthSegWidthRatio = 0.1f + nSeqWidthRatio * 0.002f;
		
		// need rebuild cache
		m_bIsCacheBuild = FALSE;
		Invalidate();
	}
}

void CLedTextCtrl::SetTotalCharCount( int nCharCount )
{
	if ( nCharCount >= 2 )
	{
		m_nCharacterNumber = nCharCount;
		
		Invalidate();
	}
}

void CLedTextCtrl::SetBevelRate( float fBevelRate )
{
	if ( fBevelRate >= 0.f && fBevelRate <= 1.f )
	{
		m_fBevelRate = fBevelRate / 2;
		
		// need rebuild cache
		m_bIsCacheBuild = FALSE;
		Invalidate();
	}
}

void CLedTextCtrl::SetFadedColor( DWORD dwColor )
{
	if ( m_colFadedColor.ToCOLORREF() == dwColor )
		return;
	m_colFadedColor.SetFromCOLORREF( dwColor );
	
	Invalidate();
}

void CLedTextCtrl::SetText( LPCTSTR lpszText )
{
	m_strText = lpszText;
	m_strText.MakeUpper();

	//m_nCharacterNumber = m_strText.GetLength();

	Invalidate();
}

void CLedTextCtrl::CalculateDrawingParams( const CRect& rcClientRect, float &fSegmentWidth, float &fSegmentInterval )
{
	float subRectWidth = rcClientRect.Height() * WIDTHHEIGHTRATIO;
	fSegmentWidth = subRectWidth * m_fWidthSegWidthRatio;
	fSegmentInterval = subRectWidth * m_fWidthIntervalRatio;
}

void CLedTextCtrl::DrawHighlight( const CRect& rcClientRect, Graphics& graphics )
{
	if ( m_bShowHighlight )
	{
		Rect topRect( rcClientRect.left, rcClientRect.top, rcClientRect.Width(), rcClientRect.Height() );
		topRect.Height = (topRect.Height >> 1);
		topRect.Inflate( -2, -2 );
		Color col1(100, 255, 255, 255);
		Color col2(m_nHighlightOpaque, 255, 255, 255);
		DrawRoundRect( graphics, topRect, 
			float(m_nCornerRadius - 1 > 1 ? m_nCornerRadius - 1 : 1), 
			col1, col2, Color(), 0, TRUE, FALSE);
	}
}

void CLedTextCtrl::DrawBackground( const CRect& rcClientRect, Graphics& graphics )
{
	Rect rect( rcClientRect.left, rcClientRect.top, rcClientRect.Width(), rcClientRect.Height() );

	if ( m_bRoundRect )
	{
		DrawRoundRect( graphics, rect, float(m_nCornerRadius), m_colCustomBk1, m_colCustomBk2,
			m_colBorderColor, m_nBorderWidth, m_bGradientBackground, m_nBorderWidth != 0);
	}
	else
	{
		if ( m_colCustomBk1.GetValue() == Color::Transparent )
			return;
		DrawNormalRect( graphics, rect, m_colCustomBk1, m_colCustomBk2,
			m_colBorderColor, m_nBorderWidth, m_bGradientBackground, m_nBorderWidth != 0 );
	}
}

void CLedTextCtrl::DrawNormalRect( Graphics& graphics, Rect& rect, Color col1, Color col2,
								   Color colBorder, int nBorderWidth, BOOL bGradient, BOOL bDrawBorder )
{
	if ( bGradient )
	{
		LinearGradientBrush br( rect, col1, col2, 90 );
		graphics.FillRectangle( &br, rect );
	}
	else
	{
		SolidBrush br(col1);
		graphics.FillRectangle( &br, rect );
	}
	if (bDrawBorder)
	{
		Pen pen( m_colBorderColor );
		graphics.DrawRectangle( &pen, rect );
	}
}

void CLedTextCtrl::DrawRoundRect( Graphics& graphics, Rect& rect, float radius, Color col1,
						   Color col2, Color colBorder, int nBorderWidth, BOOL bGradient, BOOL bDrawBorder)
{
	GraphicsPath roundRect;
	float diameter = radius + radius;
	RectF arcRect(0.f, 0.f, diameter, diameter);

	// create round rect
	arcRect.X = (float)rect.GetLeft();
	arcRect.Y = (float)rect.GetTop();
	roundRect.AddArc(arcRect, 180, 90);
	arcRect.X = (float)rect.GetRight() - 1 - diameter;
	roundRect.AddArc(arcRect, 270, 90);
	arcRect.Y = (float)rect.GetBottom() - 1 - diameter;
	roundRect.AddArc(arcRect, 0, 90);
	arcRect.X = (float)rect.GetLeft();
	roundRect.AddArc(arcRect, 90, 90);
	roundRect.CloseFigure();

	graphics.SetSmoothingMode( SmoothingModeAntiAlias );

	// paint
	if ( bGradient )
	{
		LinearGradientBrush br( rect, col1, col2, 90.f, FALSE );
		graphics.FillPath( &br, &roundRect );

	}
	else
	{
		SolidBrush br( col1 );
		graphics.FillPath( &br, &roundRect );
	}

	if ( bDrawBorder )
	{
		Pen pen(colBorder);
		pen.SetWidth( float(nBorderWidth) );
		graphics.DrawPath( &pen, &roundRect );
	}

	graphics.SetSmoothingMode( SmoothingModeNone );
}

void CLedTextCtrl::DestoryCache()
{
	for ( int i = 0; i < MAX_CACHE_PATH; ++i )
	{
		m_CachedPaths[i].Reset();
	}
}



void CLedTextCtrl::CreateCache( Rect& rectBound, float fBevelRate, float fSegmentWidth, float fSegmentInterval)
{
	PointF pathPointCollection[6];
	PointF pathDotCollection[4];

	DestoryCache();

	// top '-'
	//m_CachedPaths[0].StartFigure();
	pathPointCollection[0].X = fSegmentWidth * fBevelRate + fSegmentInterval;
	pathPointCollection[0].Y = fSegmentWidth * fBevelRate;
	pathPointCollection[1].X = fSegmentInterval + fSegmentWidth * fBevelRate * 2;
	pathPointCollection[1].Y = 0.f;
	pathPointCollection[2].X = rectBound.Width - fSegmentInterval - fSegmentWidth * fBevelRate * 2;
	pathPointCollection[2].Y = 0.f;
	pathPointCollection[3].X = rectBound.Width - fSegmentInterval - fSegmentWidth * fBevelRate;
	pathPointCollection[3].Y = fSegmentWidth * fBevelRate;
	pathPointCollection[4].X = rectBound.Width - fSegmentInterval - fSegmentWidth;
	pathPointCollection[4].Y = fSegmentWidth;
	pathPointCollection[5].X = fSegmentWidth + fSegmentInterval;
	pathPointCollection[5].Y = fSegmentWidth;
	m_CachedPaths[0].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[0].CloseFigure();
	// upper right '|'
	//m_CachedPaths[1].StartFigure();
	pathPointCollection[0].X = rectBound.Width - fSegmentWidth;
	pathPointCollection[0].Y = fSegmentWidth + fSegmentInterval;
	pathPointCollection[1].X = rectBound.Width - fSegmentWidth * fBevelRate;
	pathPointCollection[1].Y = fSegmentWidth * fBevelRate + fSegmentInterval;
	pathPointCollection[2].X = rectBound.Width + 1.f;
	pathPointCollection[2].Y = fSegmentWidth * fBevelRate * 2 + fSegmentInterval + 1;
	pathPointCollection[3].X = rectBound.Width + 1.f;
	pathPointCollection[3].Y = (rectBound.Height >> 1) - fSegmentWidth * 0.5f - fSegmentInterval - 1;
	pathPointCollection[4].X = rectBound.Width - fSegmentWidth * 0.5f;
	pathPointCollection[4].Y = (rectBound.Height >> 1) - fSegmentInterval;
	pathPointCollection[5].X = rectBound.Width - fSegmentWidth;
	pathPointCollection[5].Y = (rectBound.Height >> 1) - fSegmentWidth * 0.5f - fSegmentInterval;
	m_CachedPaths[1].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[1].CloseFigure();
	// bottom right '|'
	//m_CachedPaths[2].StartFigure();
	pathPointCollection[0].X = rectBound.Width - fSegmentWidth;
	pathPointCollection[0].Y = (rectBound.Height >> 1) + fSegmentInterval + fSegmentWidth * 0.5f;
	pathPointCollection[1].X = rectBound.Width - fSegmentWidth * 0.5f;
	pathPointCollection[1].Y = (rectBound.Height >> 1) + fSegmentInterval;
	pathPointCollection[2].X = rectBound.Width + 1.f;
	pathPointCollection[2].Y = (rectBound.Height >> 1) + fSegmentInterval + fSegmentWidth * 0.5f + 1;
	pathPointCollection[3].X = rectBound.Width + 1.f;
	pathPointCollection[3].Y = rectBound.Height - fSegmentInterval - fSegmentWidth * fBevelRate * 2 - 1;
	pathPointCollection[4].X = rectBound.Width - fSegmentWidth * fBevelRate;
	pathPointCollection[4].Y = rectBound.Height - fSegmentWidth * fBevelRate - fSegmentInterval;
	pathPointCollection[5].X = rectBound.Width - fSegmentWidth;
	pathPointCollection[5].Y = rectBound.Height - fSegmentWidth - fSegmentInterval;
	m_CachedPaths[2].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[2].CloseFigure();
	// bottom '-'
	//m_CachedPaths[3].StartFigure();
	pathPointCollection[0].X = fSegmentWidth * fBevelRate + fSegmentInterval;
	pathPointCollection[0].Y = rectBound.Height - fSegmentWidth * fBevelRate;
	pathPointCollection[1].X = fSegmentWidth + fSegmentInterval;
	pathPointCollection[1].Y = rectBound.Height - fSegmentWidth;
	pathPointCollection[2].X = rectBound.Width - fSegmentInterval - fSegmentWidth;
	pathPointCollection[2].Y = rectBound.Height - fSegmentWidth;
	pathPointCollection[3].X = rectBound.Width - fSegmentInterval - fSegmentWidth * fBevelRate;
	pathPointCollection[3].Y = rectBound.Height - fSegmentWidth * fBevelRate;
	pathPointCollection[4].X = rectBound.Width - fSegmentInterval - fSegmentWidth * fBevelRate * 2;
	pathPointCollection[4].Y = float(rectBound.Height);
	pathPointCollection[5].X = fSegmentWidth * fBevelRate * 2 + fSegmentInterval;
	pathPointCollection[5].Y = float(rectBound.Height);
	m_CachedPaths[3].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[3].CloseFigure();
	// bottom left '|'
	//m_CachedPaths[4].StartFigure();
	pathPointCollection[0].X = 0.f;
	pathPointCollection[0].Y = (rectBound.Height >> 1) + fSegmentInterval + fSegmentWidth * 0.5f;
	pathPointCollection[1].X = fSegmentWidth * 0.5f;
	pathPointCollection[1].Y = (rectBound.Height >> 1) + fSegmentInterval;
	pathPointCollection[2].X = fSegmentWidth;
	pathPointCollection[2].Y = (rectBound.Height >> 1) + fSegmentInterval + fSegmentWidth * 0.5f;
	pathPointCollection[3].X = fSegmentWidth;
	pathPointCollection[3].Y = rectBound.Height - fSegmentWidth - fSegmentInterval;
	pathPointCollection[4].X = fSegmentWidth * fBevelRate;
	pathPointCollection[4].Y = rectBound.Height - fSegmentWidth * fBevelRate - fSegmentInterval;
	pathPointCollection[5].X = 0.f;
	pathPointCollection[5].Y = rectBound.Height - fSegmentInterval - fSegmentWidth * fBevelRate * 2;
	m_CachedPaths[4].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[4].CloseFigure();
	// upper left '|'
	//m_CachedPaths[5].StartFigure();
	pathPointCollection[0].X = 0.f;
	pathPointCollection[0].Y = fSegmentWidth * fBevelRate * 2 + fSegmentInterval;
	pathPointCollection[1].X = fSegmentWidth * fBevelRate;
	pathPointCollection[1].Y = fSegmentWidth * fBevelRate + fSegmentInterval;
	pathPointCollection[2].X = fSegmentWidth;
	pathPointCollection[2].Y = fSegmentWidth + fSegmentInterval;
	pathPointCollection[3].X = fSegmentWidth;
	pathPointCollection[3].Y = (rectBound.Height >> 1) - fSegmentWidth * 0.5f - fSegmentInterval;
	pathPointCollection[4].X = fSegmentWidth * 0.5f;
	pathPointCollection[4].Y = (rectBound.Height >> 1) - fSegmentInterval;
	pathPointCollection[5].X = 0.f;
	pathPointCollection[5].Y = (rectBound.Height >> 1) - fSegmentWidth * 0.5f - fSegmentInterval;
	m_CachedPaths[5].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[5].CloseFigure();
	// draw med '-'
	//m_CachedPaths[6].StartFigure();
	pathPointCollection[0].X = (fSegmentWidth * 0.5f) + fSegmentInterval;
	pathPointCollection[0].Y = float(rectBound.Height >> 1);
	pathPointCollection[1].X = fSegmentWidth + fSegmentInterval;
	pathPointCollection[1].Y = (rectBound.Height >> 1) - fSegmentWidth * 0.5f;
	pathPointCollection[2].X = rectBound.Width - fSegmentInterval - fSegmentWidth;
	pathPointCollection[2].Y = (rectBound.Height >> 1) - fSegmentWidth * 0.5f;
	pathPointCollection[3].X = rectBound.Width - fSegmentInterval - fSegmentWidth * 0.5f;
	pathPointCollection[3].Y = float(rectBound.Height >> 1);
	pathPointCollection[4].X = rectBound.Width - fSegmentInterval - fSegmentWidth;
	pathPointCollection[4].Y = (rectBound.Height >> 1) + fSegmentWidth * 0.5f;
	pathPointCollection[5].X = fSegmentWidth + fSegmentInterval;
	pathPointCollection[5].Y = (rectBound.Height >> 1) + fSegmentWidth * 0.5f;
	m_CachedPaths[6].AddPolygon(pathPointCollection, 6);
	//m_CachedPaths[6].CloseFigure();
	// draw dot
	//m_CachedPaths[7].StartFigure();
	pathDotCollection[0].X = 0.f;
	pathDotCollection[0].Y = (float)rectBound.Height;
	pathDotCollection[1].X = fSegmentWidth;
	pathDotCollection[1].Y = (float)rectBound.Height;
	pathDotCollection[2].X = fSegmentWidth;
	pathDotCollection[2].Y = rectBound.Height - fSegmentWidth;
	pathDotCollection[3].X = 0.f;
	pathDotCollection[3].Y = rectBound.Height - fSegmentWidth;
	m_CachedPaths[7].AddPolygon(pathDotCollection, 4);
	//m_CachedPaths[7].CloseFigure();


	// draw colon top dot
	pathDotCollection[0].X = (rectBound.Width>>1) - fSegmentWidth * 0.5f;
	pathDotCollection[0].Y = (rectBound.Height>>1) - (rectBound.Height>>2) + fSegmentWidth * 0.5f;
	pathDotCollection[1].X = (rectBound.Width>>1) + fSegmentWidth * 0.5f;
	pathDotCollection[1].Y = (rectBound.Height>>1) - (rectBound.Height>>2) + fSegmentWidth * 0.5f;
	pathDotCollection[2].X = (rectBound.Width>>1) + fSegmentWidth * 0.5f;
	pathDotCollection[2].Y = (rectBound.Height>>1) - (rectBound.Height>>2) - fSegmentWidth * 0.5f;
	pathDotCollection[3].X = (rectBound.Width>>1) - fSegmentWidth * 0.5f;
	pathDotCollection[3].Y = (rectBound.Height>>1) - (rectBound.Height>>2) - fSegmentWidth * 0.5f;
	m_CachedPaths[8].AddPolygon(pathDotCollection, 4);

	// draw colon btm dot
	pathDotCollection[0].X = (rectBound.Width>>1) - fSegmentWidth * 0.5f;
	pathDotCollection[0].Y = (rectBound.Height>>1) + (rectBound.Height>>2) + fSegmentWidth * 0.5f;
	pathDotCollection[1].X = (rectBound.Width>>1) + fSegmentWidth * 0.5f;
	pathDotCollection[1].Y = (rectBound.Height>>1) + (rectBound.Height>>2) + fSegmentWidth * 0.5f;
	pathDotCollection[2].X = (rectBound.Width>>1) + fSegmentWidth * 0.5f;
	pathDotCollection[2].Y = (rectBound.Height>>1) + (rectBound.Height>>2) - fSegmentWidth * 0.5f;
	pathDotCollection[3].X = (rectBound.Width>>1) - fSegmentWidth * 0.5f;
	pathDotCollection[3].Y = (rectBound.Height>>1) + (rectBound.Height>>2) - fSegmentWidth * 0.5f;
	m_CachedPaths[9].AddPolygon(pathDotCollection, 4);

	// ok change state
	m_bIsCacheBuild = TRUE;
}


void CLedTextCtrl::DrawSegment( const CRect& rcClientRect, Graphics& graphics, Rect& rectBound, Color colSegment, int nIndex,
								float fBevelRate, float fSegmentWidth, float fSegmentInterval)
{
	Rect rcClient( rcClientRect.left, rcClientRect.top, rcClientRect.Width(), rcClientRect.Height() );
	Matrix offsetMatrix;
	if ( !m_bIsCacheBuild )
	{
		TRACE( _T("DrawSegment::Rebuilding cache...\r\n") );

		DestoryCache();
		CreateCache( rectBound, fBevelRate, fSegmentWidth, fSegmentInterval );
	}

	GraphicsPath* pSegmentPath = m_CachedPaths[nIndex - 1].Clone();
	// offset path to desired position
	offsetMatrix.Translate( (float)rectBound.X, (float)rectBound.Y );
	pSegmentPath->Transform( &offsetMatrix );
	// clip drawing(optimize drawing)
	SolidBrush segmentBrush(colSegment);
	Region region(rcClient);
	graphics.SetClip( &region );
	graphics.FillPath( &segmentBrush, pSegmentPath );
	//graphics.FillRectangle( &segmentBrush, rectBound );
	graphics.ResetClip();

	delete pSegmentPath;
}

void CLedTextCtrl::DrawSingleChar( const CRect& rcClientRect, Graphics& graphics, Rect& rectBound, Color colCharacter, char cCharacter,
								   float fBevelRate, float fSegmentWidth, float fSegmentInterval)
{
	switch (cCharacter)
	{
	case '0':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '1':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '2':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '3':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '4':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '5':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '6':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '7':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '8':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '9':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case ':':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 9, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 10, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '-':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '.':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 8, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '_':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case '(':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case ')':
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'E':
		// Relative frequency of 'E' in English spellings is 0.1268
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'T':
		// Relative frequency of 'T' in English spellings is 0.0978
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'A':
		// Relative frequency of 'A' in English spellings is 0.0788
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'O':
		// Relative frequency of 'O' in English spellings is 0.0766
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'I':
		// Relative frequency of 'I' in English spellings is 0.0707
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'N':
		// Relative frequency of 'I' in English spellings is 0.0706
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'S':
		// Relative frequency of 'S' in English spellings is 0.0634
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'R':
		// Relative frequency of 'R' in English spellings is 0.0594
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'H':
		// Relative frequency of 'H' in English spellings is 0.0573
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'L':
		// Relative frequency of 'L' in English spellings is 0.0394
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'D':
		// Relative frequency of 'L' in English spellings is 0.0389
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'U':
		// Relative frequency of 'U' in English spellings is 0.0280
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'C':
		// Relative frequency of 'C' in English spellings is 0.0268
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'F':
		// Relative frequency of 'F' in English spellings is 0.0256
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'Y':
		// Relative frequency of 'Y' in English spellings is 0.0202
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'G':
		// Relative frequency of 'G' in English spellings is 0.0187
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'P':
		// Relative frequency of 'P' in English spellings is 0.0186
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'B':
		// Relative frequency of 'B' in English spellings is 0.0156
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'V':
		// Relative frequency of 'B' in English spellings is 0.0102
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'J':
		// Relative frequency of 'J' in English spellings is 0.0010
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case 'Z':
		// Relative frequency of 'Z' in English spellings is 0.0006
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	default:
		break;
	}
}


void CLedTextCtrl::DrawSingleCharWithFadedBk( const CRect& rcClientRect, Graphics& graphics, Rect& rectBound, Color colCharacter,
											  Color colFaded, TCHAR cCharacter, float fBevelRate, float fSegmentWidth, float fSegmentInterval )
{
	switch (cCharacter)
	{
	case _T('0'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('1'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('2'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('3'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('4'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('5'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('6'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('7'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('8'):
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('9'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T(':'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 9, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 10, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;		
	case _T('-'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('.'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 8, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('_'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('('):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T(')'):
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('E'):
		// Relative frequency of 'E' in English spellings is 0.1268
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('T'):
		// Relative frequency of 'T' in English spellings is 0.0978
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('A'):
		// Relative frequency of 'A' in English spellings is 0.0788
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('O'):
		// Relative frequency of 'O' in English spellings is 0.0766
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('I'):
		// Relative frequency of 'I' in English spellings is 0.0707
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('N'):
		// Relative frequency of 'N' in English spellings is 0.0706
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('S'):
		// Relative frequency of 'S' in English spellings is 0.0634
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('R'):
		// Relative frequency of 'R' in English spellings is 0.0594
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('H'):
		// Relative frequency of 'H' in English spellings is 0.0573
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('L'):
		// Relative frequency of 'L' in English spellings is 0.0394
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('D'):
		// Relative frequency of 'D' in English spellings is 0.0389
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('U'):
		// Relative frequency of 'U' in English spellings is 0.0280
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('C'):
		// Relative frequency of 'C' in English spellings is 0.0268
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('F'):
		// Relative frequency of 'F' in English spellings is 0.0256
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('Y'):
		// Relative frequency of 'Y' in English spellings is 0.0202
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('G'):
		// Relative frequency of 'G' in English spellings is 0.0187
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('P'):
		// Relative frequency of 'P' in English spellings is 0.0186
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('B'):
		// Relative frequency of 'B' in English spellings is 0.0156
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('V'):
		// Relative frequency of 'B' in English spellings is 0.0102
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('J'):
		// Relative frequency of 'J' in English spellings is 0.0010
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	case _T('Z'):
		// Relative frequency of 'Z' in English spellings is 0.0006
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colCharacter, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	default:
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 1, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 2, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 3, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 4, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 5, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 6, fBevelRate, fSegmentWidth, fSegmentInterval);
		DrawSegment(rcClientRect, graphics, rectBound, colFaded, 7, fBevelRate, fSegmentWidth, fSegmentInterval);
		break;
	}
}

void CLedTextCtrl::AdjustWnd( BOOL bAlignRight/*=FALSE*/ )
{
	CRect rc;
	GetWindowRect(rc);
	GetParent()->ScreenToClient(rc);
	float fSegmentWidth = 0;
	float fSegmentInterval = 0;
	CalculateDrawingParams( rc, fSegmentWidth, fSegmentInterval );

	int nSubRectWidth = (int)(rc.Height() * WIDTHHEIGHTRATIO);
	int nCharIntervals = fSegmentInterval > 0.5f ? (int)(fSegmentInterval * 2) : 1;

	int nNewWndWidth = m_nCharacterNumber * nSubRectWidth + nCharIntervals;

	if (nNewWndWidth != rc.Width())
	{
		if (bAlignRight)
		{
			int nXPos = rc.left + rc.Width() - nNewWndWidth;
			//SetWindowPos(NULL, nXPos, rc.top, nNewWndWidth, rc.Height(), 0);
			MoveWindow( nXPos, rc.top, nNewWndWidth, rc.Height() );
		}
		else
		{
			SetWindowPos(NULL, 0, 0, nNewWndWidth, rc.Height(), SWP_NOMOVE);
		}
	}
}

void CLedTextCtrl::DrawChars( const CRect& rcClientRect, Graphics& graphics, float fSegmentWidth, float fSegmentInterval )
{
	Rect clientRect( rcClientRect.left, rcClientRect.top, rcClientRect.Width(), rcClientRect.Height() );
	Rect subRect;

	int nSubRectWidth = (int)(clientRect.Height * WIDTHHEIGHTRATIO);
	int nSubRectHeight = clientRect.Height;
	int nTotalElements = m_nCharacterNumber;
	int nCharIntervals = fSegmentInterval > 0.5f ? (int)(fSegmentInterval * 2) : 1;

	// for right alignemnt only
	int nBeginIndexRight = 0;
	if ( m_enumAlign == TA_Right )
	{
		if ( m_strText.GetLength() >= nTotalElements )
			nBeginIndexRight = 0;
		else
			nBeginIndexRight = nTotalElements - m_strText.GetLength();
	}
	for (int i = 0; i < nTotalElements; ++i)
	{
		// draw faded led(s)
		subRect.Width = nSubRectWidth;
		subRect.Height = nSubRectHeight;
		subRect.X = (int)(i * subRect.Width);
		subRect.Y = 0;
		subRect.Inflate( -nCharIntervals, -nCharIntervals );
		// draw foreground led(s)
		if ( m_enumAlign == TA_Left )
		{
			if (i < m_strText.GetLength())
			{
				TCHAR cTestChar = m_strText.GetAt(i);
				if ( cTestChar == _T(' ') )
				{
					DrawSingleChar( rcClientRect,graphics, subRect, m_colFadedColor, _T('8'), m_fBevelRate, fSegmentWidth,
						fSegmentInterval );
				}
				else
				{
					DrawSingleCharWithFadedBk( rcClientRect, graphics, subRect, m_colForeColor, m_colFadedColor, m_strText.GetAt(i),
						m_fBevelRate, fSegmentWidth, fSegmentInterval);
				}
			}
			else
			{
				DrawSingleChar( rcClientRect,graphics, subRect, m_colFadedColor, _T('8'), m_fBevelRate, fSegmentWidth,
					fSegmentInterval );
			}
		}
		else
		{
			if (i >= nBeginIndexRight)
			{
				DrawSingleCharWithFadedBk(rcClientRect, graphics, subRect, m_colForeColor, m_colFadedColor, m_strText.GetAt(i - nBeginIndexRight),
					m_fBevelRate, fSegmentWidth, fSegmentInterval);
			}
			else
			{
				DrawSingleChar( rcClientRect,graphics, subRect, m_colFadedColor, _T('8'), m_fBevelRate, fSegmentWidth,
					fSegmentInterval );
			}
		}
	}
}


void CLedTextCtrl::OnDraw( Graphics& graphics, Rect rect )
{
	CRect rcClientRect = CRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	//GetClientRect( &rcClientRect );

	//HDC hDC = pDC->GetSafeHdc();
	//Graphics graphics(hDC);

	float fSegmentWidth = 0;
	float fSegmentInterval = 0;
	if ( rcClientRect.Height() >= 20 && rcClientRect.Width() >= 20 )
	{
		DrawBackground( rcClientRect, graphics );
		CalculateDrawingParams( rcClientRect, fSegmentWidth, fSegmentInterval );
		DrawChars( rcClientRect, graphics, fSegmentWidth, fSegmentInterval );
		DrawHighlight( rcClientRect, graphics );
	}
}


void CLedTextCtrl::PreSubclassWindow()
{
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	CGdipStatic::PreSubclassWindow();
}
