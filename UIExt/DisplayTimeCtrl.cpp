// DisplayTimeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../resource.h"
#include "DisplayTimeCtrl.h"
#include "GdiplusExt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
using namespace Gdiplus;
// CDisplayTimeCtrl

IMPLEMENT_DYNAMIC(CDisplayTimeCtrl, CGdipBaseWnd)

CDisplayTimeCtrl::CDisplayTimeCtrl()
{
	m_pIconTime = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_TIME );
}

CDisplayTimeCtrl::~CDisplayTimeCtrl()
{
	if (m_pIconTime)
	{
		delete m_pIconTime;
		m_pIconTime = NULL;
	}
}


BEGIN_MESSAGE_MAP(CDisplayTimeCtrl, CGdipBaseWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDisplayTimeCtrl::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("DisplayTimeCtrl"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1);
}

void CDisplayTimeCtrl::OnDraw(Graphics& g, Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	CTime time = CTime::GetCurrentTime();

	CString strTime = time.Format( _T("%H:%M:%S") );

	BSTR bstrTime = strTime.AllocSysString();
	FontFamily fontFamily(L"Verdana");
	Gdiplus::Font font(&fontFamily, 20, FontStyleRegular, UnitPixel);
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );
	RectF rectText;
	rectText.X = (float)rect.X;
	rectText.Y = (float)rect.Y;
	rectText.Width = (float)rect.Width;
	rectText.Height = (float)rect.Height;
	RectF rectBB;
	g.MeasureString( bstrTime, -1, &font, rectText, &stringFormat, &rectBB );

	int nIW = m_pIconTime->GetWidth();
	int nIH = m_pIconTime->GetHeight();

	rectText.X += nIW*0.85f/2.f;

	RectF rectIcon( rectBB.X-nIW*0.85f/2, rectBB.Y+(rectBB.Height-nIH)/2, (float)nIW, (float)nIH );

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.6f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	g.DrawImage( m_pIconTime, Rect((int)rectIcon.X, (int)rectIcon.Y, nIW, nIH), 0, 0, nIW, nIH, UnitPixel, &ImgAttr );

	g.DrawString( bstrTime, -1, &font, rectText, &stringFormat, &SolidBrush(Color(180,255,255,255)) );
	SysFreeString( bstrTime );
}

void CDisplayTimeCtrl::OnGdipEraseBkgnd(Graphics& g, Rect rect)
{
	g.FillRectangle( &SolidBrush(Color(67,72,78)), rect );
}


void CDisplayTimeCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 )
		Invalidate();

	CGdipBaseWnd::OnTimer(nIDEvent);
}


int CDisplayTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGdipBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1, 500, NULL);

	return 0;
}


void CDisplayTimeCtrl::OnDestroy()
{
	CGdipBaseWnd::OnDestroy();

	KillTimer(1);
}
