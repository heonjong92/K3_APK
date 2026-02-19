// SimpleGraphCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SimpleGraphCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
// CSimpleGraphCtrl

IMPLEMENT_DYNAMIC(CSimpleGraphCtrl, CGdipStatic)

CSimpleGraphCtrl::CSimpleGraphCtrl()
{
	m_nMaxDataCount = 100;

	m_fDataMax = 1.f;
	m_fDataMin = -1.f;
	m_fDataLimitMax = 0.5f;
	m_fDataLimitMin = -0.5f;

	m_bEnableRangeLimit = TRUE;
	m_bEnableCenterAxis = TRUE;

	m_fDataCenter = 0.f;

	m_dwColorData = RGB(13, 97, 189);
	m_dwColorBody = RGB(255, 255, 255);
	m_dwColorAxis = RGB(150, 150, 170);
	m_dwColorCenterAxis = RGB(204, 204, 204);
	m_dwColorLimitAxis = RGB(224, 224, 224);
	m_dwColorLimitOverData = RGB(220, 57, 18);

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	USES_CONVERSION;

	WCHAR* szFaceName;
	CString strX = (WCHAR*)T2W(ncm.lfMessageFont.lfFaceName);
	if ( strX.IsEmpty() )
		szFaceName = L"Arial";
	else
		szFaceName = (WCHAR*)T2W(ncm.lfMessageFont.lfFaceName);

	m_pFontFamily = ::new Gdiplus::FontFamily( szFaceName );
}

CSimpleGraphCtrl::~CSimpleGraphCtrl()
{
	if (m_pFontFamily)
	{
		::delete m_pFontFamily;
		m_pFontFamily = NULL;
	}
}


BEGIN_MESSAGE_MAP(CSimpleGraphCtrl, CGdipStatic)
END_MESSAGE_MAP()

void CSimpleGraphCtrl::AddData( float fData )
{
	m_aryGraphData.Add(fData);

	if (m_aryGraphData.GetSize() > m_nMaxDataCount)
	{
		m_aryGraphData.RemoveAt(0);
	}

	if (GetSafeHwnd())
		Invalidate();
}

void CSimpleGraphCtrl::ResetData()
{
	m_aryGraphData.RemoveAll();
	if (GetSafeHwnd())
		Invalidate();
}

void CSimpleGraphCtrl::SetMaxDataCount( int nMaxDataCount )
{
	m_nMaxDataCount = nMaxDataCount;
	while (m_aryGraphData.GetSize() > m_nMaxDataCount)
	{
		m_aryGraphData.RemoveAt(0);
	}

	if (GetSafeHwnd())
		Invalidate();
}

void CSimpleGraphCtrl::SetRangeAxisY( float fMin, float fMax )
{
	m_fDataMax = fMax;
	m_fDataMin = fMin;

	if (GetSafeHwnd())
		Invalidate();
}

void CSimpleGraphCtrl::EnableCenterAxis( BOOL bEnable )
{
	m_bEnableCenterAxis = bEnable;
}

void CSimpleGraphCtrl::SetCenterAxisY( float fCenter )
{
	m_fDataCenter = fCenter;
}

void CSimpleGraphCtrl::EnableLimitY( BOOL bEnable )
{
	m_bEnableRangeLimit = bEnable;
}

void CSimpleGraphCtrl::SetRangeLimitAxisY(float fLimitMin, float fLimitMax )
{
	m_fDataLimitMin = fLimitMin;
	m_fDataLimitMax = fLimitMax;

	if (GetSafeHwnd())
		Invalidate();
}

void CSimpleGraphCtrl::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	Color colorBody;
	colorBody.SetFromCOLORREF(m_dwColorBody);
	rect.Inflate( 2, 2 );
	g.FillRectangle( &SolidBrush(colorBody), rect );
}

void CSimpleGraphCtrl::OnDraw( Graphics& g, Rect rect )
{
	using namespace Gdiplus;
	g.SetSmoothingMode( SmoothingModeHighQuality );
	g.SetTextRenderingHint( TextRenderingHintAntiAlias );

	Rect rectBody = rect;
	rectBody.X = 0;
	rectBody.Width--;
	rectBody.Y = 0;
	rectBody.Height--;

	Rect rectGraph = rectBody;
	rectGraph.Inflate( -2, -2 );

	if ( m_bEnableCenterAxis )
	{
		Color colorCenterAxis;
		colorCenterAxis.SetFromCOLORREF( m_dwColorCenterAxis );
		Pen penCenterAxis(colorCenterAxis);
		penCenterAxis.SetDashStyle( DashStyleDashDot );
		int nCenterY = rectGraph.Height - (int)(rectGraph.Height * (m_fDataCenter-m_fDataMin) / (m_fDataMax-m_fDataMin) + 0.5f) + rectGraph.Y;
		g.DrawLine( &penCenterAxis,
			rectBody.GetLeft(), nCenterY, 
			rectBody.GetRight(), nCenterY
			);
	}

	if ( m_bEnableRangeLimit )
	{
		Color colorLimitAxis;
		colorLimitAxis.SetFromCOLORREF( m_dwColorLimitAxis );
		Pen penLimitAxis(colorLimitAxis);
		penLimitAxis.SetDashStyle( DashStyleDash );

		int nLimitMaxY = rectGraph.Height - (int)(rectGraph.Height * (m_fDataLimitMax-m_fDataMin) / (m_fDataMax-m_fDataMin) + 0.5f) + rectGraph.Y;
		g.DrawLine( &penLimitAxis,
			rectBody.GetLeft(), nLimitMaxY, 
			rectBody.GetRight(), nLimitMaxY
			);

		int nLimitMinY = rectGraph.Height - (int)(rectGraph.Height * (m_fDataLimitMin-m_fDataMin) / (m_fDataMax-m_fDataMin) + 0.5f) + rectGraph.Y;
		g.DrawLine( &penLimitAxis,
			rectBody.GetLeft(), nLimitMinY, 
			rectBody.GetRight(), nLimitMinY
			);
	}

	Color colorData( 250, GetRValue(m_dwColorData), GetGValue(m_dwColorData), GetBValue(m_dwColorData) );
	Pen penData( colorData );
	SolidBrush brushData( colorData );
	DrawData( g, penData, brushData, m_aryGraphData, rectGraph );

	Color colorAxis;
	colorAxis.SetFromCOLORREF(m_dwColorAxis);
	Pen penAxis( colorAxis, 1 );
	g.DrawRectangle( &penAxis, rectBody );

	if (m_aryGraphData.GetCount() > 0)
	{
		float fLastData = m_aryGraphData.GetAt(m_aryGraphData.GetCount()-1);
		RectF rectDataTextF((float)rectBody.X, (float)rectBody.Y, 0.f, 0.f);
		rectDataTextF.Width = (float)rectBody.Width;
		rectDataTextF.Height = 14.f;

		StringFormat stringFormat;
		stringFormat.SetAlignment(StringAlignmentNear);
		stringFormat.SetLineAlignment(StringAlignmentCenter);

		CString strData;
		strData.Format( _T("%.4f"), fLastData );
		BSTR bstrData = strData.AllocSysString();
		Gdiplus::Font font(m_pFontFamily, 12, FontStyleRegular, UnitPixel);
		g.DrawString( bstrData, -1, &font, rectDataTextF, &stringFormat, &SolidBrush(Color(80,80,80)) );
		::SysFreeString(bstrData);
	}
}

void CSimpleGraphCtrl::DrawData( Gdiplus::Graphics& g, Gdiplus::Pen& penData, Gdiplus::Brush& brushData, CArray<float, float>& aryData, Gdiplus::Rect rectGraph )
{
	using namespace Gdiplus;

	float fStepX = (float)rectGraph.Width / (m_nMaxDataCount-1);
	float fX = (float)rectGraph.X;
	int nX = 0;
	int nY = 0;
	int nCenterY = rectGraph.Height / 2 + rectGraph.Y;

	Point* pGraphPoints = new Point[aryData.GetSize()];

	Color colorLimitOver;
	colorLimitOver.SetFromCOLORREF( m_dwColorLimitOverData );
	SolidBrush brushLimitOver( colorLimitOver );

	CArray<INT_PTR> LimitOverPoints;
	CArray<INT_PTR> NormalPoints;
	for (INT_PTR nI=0 ; nI<aryData.GetSize() ; nI++)
	{
		nX = (int)(fX + 0.5f);

		float fData1 = aryData.GetAt(nI);

		if (fData1 > m_fDataLimitMax || fData1 < m_fDataLimitMin)
		{
			LimitOverPoints.Add(nI);
		}
		else
		{
			NormalPoints.Add(nI);
		}

		nY = rectGraph.Height - (int)(rectGraph.Height * (fData1-m_fDataMin) / (m_fDataMax-m_fDataMin) + 0.5f) + rectGraph.Y;
		if (nY < rectGraph.GetTop()) nY = rectGraph.GetTop();
		if (nY > rectGraph.GetBottom()) nY = rectGraph.GetBottom();

		pGraphPoints[nI].X = nX;
		pGraphPoints[nI].Y = nY;

		fX += fStepX;
	}

	g.DrawLines( &penData, pGraphPoints, (INT)aryData.GetSize() );

	INT_PTR nIdx;
	for (INT_PTR nI=0 ; nI<NormalPoints.GetSize() ; nI++)
	{
		nIdx = NormalPoints.GetAt(nI);
		g.FillEllipse( &brushData, pGraphPoints[nIdx].X-1.5f, pGraphPoints[nIdx].Y-1.5f, 3.f, 3.f );
	}
	for (INT_PTR nI=0 ; nI<LimitOverPoints.GetSize() ; nI++)
	{
		nIdx = LimitOverPoints.GetAt(nI);
		g.FillEllipse( &brushLimitOver, pGraphPoints[nIdx].X-2.f, pGraphPoints[nIdx].Y-2.f, 4.f, 4.f );
	}

	delete[] pGraphPoints;
}



void CSimpleGraphCtrl::PreSubclassWindow()
{
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	CGdipStatic::PreSubclassWindow();
}
