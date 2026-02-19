// IOStatusCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "IOStatusCtrlEx.h"
#include "../Dev/xIOCard.h"

#include "../Dev/xIOMapper.h"

#include "GdiplusExt.h"

#include "ResourceManager.h"

using namespace UIExt;
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIOStatusCtrlEx

CIOStatusCtrlEx::CIOStatusCtrlEx() : m_pBitmap(NULL)
{
	m_dwIOStatus = 0x0000;
	m_bEnable = TRUE;
	m_bEnableMouseEvent = TRUE;
	m_pIOCard = NULL;
	m_uTimer = 0;
	m_nIOCount = 16;

	m_IOType = IOTypeInput;

	m_pIORects = new Rect[m_nIOCount];
	m_pImageIcon = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_IO_STATUS );
	m_sizeIcon.cx = m_pImageIcon->GetHeight();
	m_sizeIcon.cy = m_pImageIcon->GetHeight();
	ASSERT( (m_pImageIcon->GetWidth() / m_pImageIcon->GetHeight()) == 2 );
}

CIOStatusCtrlEx::~CIOStatusCtrlEx()
{
	if ( m_pBitmap )
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
	if (m_pImageIcon)
	{
		delete m_pImageIcon;
		m_pImageIcon = NULL;
	}
	if (m_pIORects)
	{
		delete[] m_pIORects;
		m_pIORects = NULL;
	}
}


BEGIN_MESSAGE_MAP(CIOStatusCtrlEx, CGdipStatic)
	//{{AFX_MSG_MAP(CIOStatusCtrlEx)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIOStatusCtrlEx message handlers

void CIOStatusCtrlEx::SetIOPortModule( VisionDevice::CxIOCard* pIOCard, IOType type, int nIOCount ) 
{ 
	m_pIOCard = pIOCard;
	m_IOType = type;
	if ( m_nIOCount != nIOCount )
	{
		delete[] m_pIORects;
		m_pIORects = new Rect[nIOCount];
		m_nIOCount = nIOCount;
	}
}

int CIOStatusCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CGdipStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( m_MemDC.GetSafeHdc() == NULL )
	{
		CClientDC dc(this);
		m_MemDC.CreateCompatibleDC(&dc);
	}
	
	if ( m_uTimer == 0 )
	{
		m_uTimer = SetTimer( 0x501, 100, NULL );
		Invalidate();
	}

	ModifyStyle( 0, SS_NOTIFY );
	
	return 0;
}

void CIOStatusCtrlEx::OnDestroy() 
{
	CGdipStatic::OnDestroy();
	
	if ( m_uTimer != 0 )
	{
		KillTimer( m_uTimer );
	}	
}

void CIOStatusCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( !m_bEnableMouseEvent || (m_IOType == IOTypeInput) ) return;

	if ( m_pIOCard )
	{
		int nIndex = 0;
		if ( HitTest( point, nIndex ) )
		{
			ToggleIO( nIndex );
			m_dwIOStatus = m_pIOCard->GetOutPortStatus();
			Invalidate();
		}
	}
	
	CGdipStatic::OnLButtonDown(nFlags, point);
}

void CIOStatusCtrlEx::PreSubclassWindow() 
{
	ModifyStyle( 0, SS_NOTIFY );
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );
	
	CGdipStatic::PreSubclassWindow();
}

void CIOStatusCtrlEx::OnDraw( Gdiplus::Graphics& g, Rect rect )
{
	if ( m_uTimer == 0 )
	{
		m_uTimer = SetTimer( 0x501, 100, NULL );
	}

	//g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	Rect rectBody = rect;

	g.FillRectangle( &SolidBrush(Color(220,220,220)), rectBody );

	const int nTitleHeight = 28;

	Rect rectTitle = rectBody;
	rectTitle.Height = nTitleHeight;

	DrawTitle( g, rectTitle );

	Rect rectIOBody = rectBody;
	rectIOBody.Y = rectTitle.GetBottom();
	rectIOBody.Height = rectBody.GetBottom() - rectIOBody.Y;

	DrawIOItems( g, rectIOBody );
}

void CIOStatusCtrlEx::DrawTitle( Gdiplus::Graphics& g, Gdiplus::Rect rectTitle )
{
	g.FillRectangle( &SolidBrush(Color(67, 72, 78)), rectTitle );

	int nIconOffset = (rectTitle.Height-m_sizeIcon.cy) / 2;

	int nIX = rectTitle.X + nIconOffset;
	int nIY = rectTitle.Y + nIconOffset;
	int nIW = m_sizeIcon.cx;
	int nIH = m_sizeIcon.cy;

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.7f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	g.DrawImage( m_pImageIcon, Rect(nIX, nIY, nIW, nIH), nIW*m_IOType, 0, nIW, nIH, UnitPixel, &ImgAttr );

	RectF rectTextF( 
		(float)rectTitle.X+nIconOffset*2+nIW, 
		(float)rectTitle.Y, 
		(float)rectTitle.Width-(nIconOffset*2+nIW), 
		(float)rectTitle.Height 
		);

	Color colorText(200, 255,255,255);
	SolidBrush brushText( colorText );
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentNear );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	Gdiplus::Font fontText(CResourceManager::SystemFontFamily(), 12, FontStyleBold, UnitPixel);

	CString strTitleText;
	GetWindowText(strTitleText);
	BSTR bstrTitle = strTitleText.AllocSysString();
	g.DrawString( bstrTitle, -1, &fontText, rectTextF, &stringFormat, &brushText );
	SysFreeString( bstrTitle );
}

void CIOStatusCtrlEx::DrawIOItems( Gdiplus::Graphics& g, Gdiplus::Rect rectIOBody )
{

	int nIOCnt = m_nIOCount;
	int nSplit = nIOCnt / 4;

	int nH = rectIOBody.Height;
	float fR = nH / (nIOCnt+(nSplit+1)/4.f);
	float fC = fR;

	float fOffset = fR/4.f;

	Rect rectItem;
	rectItem.X = int(rectIOBody.X + fOffset + .5f);
	rectItem.Width = int(rectIOBody.Width - fOffset*2 + .5f);
	rectItem.Y = int(rectIOBody.Y+fOffset+.5f);
	rectItem.Height = int(fR+.5f);

	Pen penLine(Color(240,240,240) );
	SolidBrush brushON( Color(136, 0, 21) );
	SolidBrush brushOFF( Color(210, 0, 0, 0) );

	FontFamily fontFamily(L"Arial");
	Gdiplus::Font font(&fontFamily, (float)rectItem.Height*0.65f, FontStyleRegular, UnitPixel);
	Gdiplus::Font fontDesc(CResourceManager::SystemFontFamily(), (float)rectItem.Height*0.61f, FontStyleRegular, UnitPixel);
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap|StringFormatFlagsNoClip);
	StringFormat stringFormatDesc;
	stringFormatDesc.SetAlignment( StringAlignmentNear );
	stringFormatDesc.SetLineAlignment( StringAlignmentCenter );
	stringFormatDesc.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap|StringFormatFlagsNoClip);
	SolidBrush brushText(Color(180,255,255,255));
	SolidBrush brushDesc(Color(200,0,0,0));

	CString strNum;
	BSTR bstrNum;
	float fY = (float)rectItem.Y;
	Rect rectIO;
	RectF rectDescF;
	BSTR bstrDesc;
	for ( int i=0 ; i<nIOCnt ; i++ )
	{
		if ( (i != 0) && ((i % 4) == 0) )
		{
			fY += fOffset;
			rectItem.Y = int(fY+.5f);
			rectItem.Height = int(fY+fR+.5f) - rectItem.Y;
		}
		strNum.Format( _T("%d"), i );

		rectIO = rectItem;
		rectIO.Width = rectItem.Height;
		m_pIORects[i] = rectIO;
		rectDescF.X = (float)rectItem.X + rectIO.Width + fOffset;
		rectDescF.Width = (float)rectItem.Width - rectIO.Width - fOffset*2;
		rectDescF.Y = (float)rectItem.Y;
		rectDescF.Height = (float)rectItem.Height;

		if ( (m_dwIOStatus & (0x01 << i)) == 0 )
		{
			g.FillRectangle( &brushOFF, rectIO );
		}
		else
		{
			g.FillRectangle( &brushON, rectIO );
		}

		g.DrawRectangle( &penLine, rectIO );

		bstrNum = strNum.AllocSysString();
		g.DrawString(bstrNum, -1, &font, RectF((float)rectIO.X, (float)rectIO.Y, (float)rectIO.Width, (float)rectIO.Height), &stringFormat, &brushText);
		SysFreeString(bstrNum);

		CString& strDescription = m_MapIODesc.at(i);
		if ( !strDescription.IsEmpty() )
		{
			bstrDesc = strDescription.AllocSysString();
			g.DrawString(bstrDesc, -1, &fontDesc, rectDescF, &stringFormatDesc, &brushDesc);
			SysFreeString(bstrDesc);
		}

		fY += fR;
		rectItem.Y = int(fY+.5f);
		rectItem.Height = int(fY+fR+.5f) - rectItem.Y;
	}

	//g.FillRectangle( &SolidBrush(Color(200,10,10)), rectIOBody );
}

void CIOStatusCtrlEx::OnTimer(UINT_PTR nIDEvent) 
{
	if ( m_pIOCard )
	{
		DWORD dwNow = 0;
		if ( m_IOType == IOTypeInput )
			dwNow = m_pIOCard->GetInPortStatus();
		else
			dwNow = m_pIOCard->GetOutPortStatus();

		if ( dwNow != m_dwIOStatus )
		{
			m_dwIOStatus = dwNow;
			Invalidate();
		}
	}
	
	CGdipStatic::OnTimer(nIDEvent);
}

void CIOStatusCtrlEx::EnableMouseHandleEvent( BOOL bEnable )
{
	m_bEnableMouseEvent = bEnable;
}

void CIOStatusCtrlEx::ResetDescription()
{
	for ( int i = 0 ; i < 16 ; i++ )
	{
		SetDescription( i, VisionDevice::CxIOMapper::Instance()->GetIODescAt( i, m_IOType == IOTypeInput ? TRUE : FALSE ).second.c_str() );
	}
}

BOOL CIOStatusCtrlEx::HitTest( CPoint point, int& nIndex )
{
	for ( int i=0 ; i<m_nIOCount ; i++ )
	{
		if ( m_pIORects[i].Contains(point.x, point.y) )
		{
			nIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

void CIOStatusCtrlEx::SetDescription( int nIONumber, LPCTSTR lpszDescription )
{
	m_MapIODesc[nIONumber] = lpszDescription;
}

void CIOStatusCtrlEx::ToggleIO( int nIndex )
{
	if ( m_pIOCard )
	{
		DWORD dwPortStatus = m_pIOCard->GetOutPortStatus();
		DWORD dwNewMask = 0x01 << nIndex;
		DWORD dwNewPortStatus = dwPortStatus ^ dwNewMask;
		dwNewPortStatus = (dwNewPortStatus & dwNewMask) | (dwPortStatus & ~dwNewMask);

		m_pIOCard->Write( dwNewPortStatus );
	}
}

