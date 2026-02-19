// IOStatusCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../resource.h"
#include "IOStatusCtrl.h"
#include "GdiplusExt.h"

#include "../Dev/xIOCard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
using namespace Gdiplus;
// CIOStatusCtrl

IMPLEMENT_DYNAMIC(CIOStatusCtrl, CGdipBaseWnd)

CIOStatusCtrl::CIOStatusCtrl()
{
	m_pIOCard = NULL;
	m_pImageIcon = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_IO_STATUS );
	m_sizeIcon.cx = m_pImageIcon->GetHeight();
	m_sizeIcon.cy = m_pImageIcon->GetHeight();
	ASSERT( (m_pImageIcon->GetWidth() / m_pImageIcon->GetHeight()) == 2 );

	m_IOType = IOTypeInput;
	m_nIOCount = 16;
	m_pIORects = new Rect[m_nIOCount];
	m_dwIOStatus = 0x0000;

	m_bEnableMouseEvent = TRUE;
}

CIOStatusCtrl::~CIOStatusCtrl()
{
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


BEGIN_MESSAGE_MAP(CIOStatusCtrl, CGdipBaseWnd)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CIOStatusCtrl::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("IOStatusCtrl"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1 );
}

void CIOStatusCtrl::SetIOModule( VisionDevice::CxIOCard* pIOCard, IOType type, int nIOCount )
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

void CIOStatusCtrl::OnDraw(Graphics& g, Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	CRect rcIOBody;
	CRect rcTextBody;

	int nIX = rect.X;
	int nIY = rect.Y + (rect.Height-m_sizeIcon.cy) / 2;
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

	rcIOBody = CRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	rcIOBody.DeflateRect(nIW, 6, 1, 5);

	SolidBrush brushON( Color(136, 0, 21) );
	SolidBrush brushOFF( Color(120, 0, 0, 0) );

	//rcTextBody = rc;s
	//rcTextBody.DeflateRect(nIW, 1, 1, rc.bottom-rcIOBody.top);

	Pen penLine(Color(180,180,180) );

	int nIOCnt = m_nIOCount;
	int nSplit = nIOCnt / 4;

	int nW = rcIOBody.Width();
	float fR = nW / (nIOCnt+(nSplit+1)/4.f);
	float fC = fR;

	float fOffset = fR/4.f;

	Rect rcCell;
	rcCell.Y = rcIOBody.top;
	rcCell.Height = rcIOBody.Height();
	rcCell.X = int(rcIOBody.left+fOffset+.5f);
	rcCell.Width = int(fR+.5f);

	FontFamily fontFamily(L"Arial");
	Gdiplus::Font font(&fontFamily, (float)rcCell.Width*0.63f, FontStyleRegular, UnitPixel);
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap|StringFormatFlagsNoClip);
	SolidBrush brushText(Color(180,255,255,255));

	CString strNum;
	float fX = (float)rcCell.X;
	BSTR bstrNum;
	for ( int i=0 ; i<nIOCnt ; i++ )
	{
		if ( (i != 0) && ((i % 4) == 0) )
		{
			fX += fOffset;
			rcCell.X = int(fX+.5f);
			rcCell.Width = int(fX+fR+.5f) - rcCell.X;
		}
		strNum.Format( _T("%d"), i );

		if ( (m_dwIOStatus & (0x01 << i)) == 0 )
		{
			g.FillRectangle( &brushOFF, rcCell );
		}
		else
		{
			g.FillRectangle( &brushON, rcCell );
		}

		m_pIORects[i] = rcCell;

		g.DrawRectangle( &penLine, rcCell );
		bstrNum = strNum.AllocSysString();
		g.DrawString(bstrNum, -1, &font, RectF((float)rcCell.X, (float)rcCell.Y, (float)rcCell.Width, (float)rcCell.Height), &stringFormat, &brushText);
		SysFreeString(bstrNum);

		fX += fR;
		rcCell.X = int(fX+.5f);
		rcCell.Width = int(fX+fR+.5f) - rcCell.X;
	}
}

void CIOStatusCtrl::OnGdipEraseBkgnd(Graphics& g, Rect rect)
{
	g.FillRectangle( &SolidBrush(Color(67,72,78)), rect );
}

void CIOStatusCtrl::ToggleIO( int nIndex )
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

BOOL CIOStatusCtrl::HitTest( CPoint point, int& nIndex )
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

void CIOStatusCtrl::OnTimer(UINT_PTR nIDEvent)
{
	switch ( nIDEvent )
	{
	case 0x501:
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
		break;
	}

	CGdipBaseWnd::OnTimer(nIDEvent);
}


void CIOStatusCtrl::OnLButtonDown(UINT nFlags, CPoint point)
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

	CGdipBaseWnd::OnLButtonDown(nFlags, point);
}


int CIOStatusCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGdipBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer( 0x501, 100, NULL );

	return 0;
}


void CIOStatusCtrl::OnDestroy()
{
	CGdipBaseWnd::OnDestroy();

	KillTimer( 0x501 );
}
