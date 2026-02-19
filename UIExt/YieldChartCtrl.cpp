// YieldChartCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "YieldChartCtrl.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
using namespace Gdiplus;
// CYieldChartCtrl

IMPLEMENT_DYNAMIC(CYieldChartCtrl, CGdipStatic)

CYieldChartCtrl::CYieldChartCtrl()
{
	m_fYield = 0.0f;

	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetSideBarTitleForegroundColor();

	m_dwBackgroundColor = dwBodyColor;
}

CYieldChartCtrl::~CYieldChartCtrl()
{
}

void CYieldChartCtrl::SetBackgroundColor( DWORD dwColor )
{
	m_dwBackgroundColor = dwColor;
	if (GetSafeHwnd())
		Invalidate();
}


BEGIN_MESSAGE_MAP(CYieldChartCtrl, CGdipStatic)
END_MESSAGE_MAP()

void CYieldChartCtrl::SetYield( float fYield )
{
	m_fYield = fYield;
	Invalidate();
}

void CYieldChartCtrl::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	Rect rectBody = rect;
	rectBody.Inflate( 1, 1 );

	Color colorBody;
	colorBody.SetFromCOLORREF( m_dwBackgroundColor );

	rectBody.Inflate(-2,-2);

	int nChartWH = rectBody.Width > rectBody.Height ? rectBody.Height : rectBody.Width;
	Rect rectChart;
	rectChart.X = rectBody.X + (rectBody.Width - nChartWH)/2;
	rectChart.Y = rectBody.Y + (rectBody.Height - nChartWH)/2;
	rectChart.Width = rectChart.Height = nChartWH;

	SolidBrush brushNG(Color(220,57,18));
	SolidBrush brushOK(Color(51,102,204));

	g.FillEllipse( &brushOK, rectChart );
	g.DrawEllipse( &Pen(colorBody), rectChart );

	g.FillPie( &brushNG, rectChart, -90, -360*(1.f-m_fYield) );

	g.DrawPie( &Pen(colorBody), rectChart, -90, -360*(1.f-m_fYield) );

	RectF rectTextF;
	rectTextF.X = (float)rectChart.X;
	rectTextF.Y = (float)rectChart.Y + rectChart.Height/4;
	rectTextF.Width = (float)rectChart.Width;
	rectTextF.Height = (float)rectChart.Height - + rectChart.Height/4;

	CString strYield;
	strYield.Format( _T("%.1f%%"), m_fYield * 100.f );

	Gdiplus::Font font(CResourceManager::SystemFontFamily(), rectTextF.Height*0.22f, FontStyleBold, UnitPixel);
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	BSTR bstrYield = strYield.AllocSysString();
	g.DrawString( bstrYield, -1, &font, rectTextF, &stringFormat, &SolidBrush(Color(200,255,255,255)) );
	SysFreeString( bstrYield );
}

void CYieldChartCtrl::OnGdipEraseBkgnd(Gdiplus::Graphics& g, Rect rect)
{
	Color colorBody;
	colorBody.SetFromCOLORREF( m_dwBackgroundColor );

	rect.Inflate( 1, 1 );

	g.FillRectangle( &SolidBrush(colorBody), rect );
}

void CYieldChartCtrl::PreSubclassWindow()
{
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	CGdipStatic::PreSubclassWindow();
}
