// MeasuredGraphCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MeasuredGraphCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
// CMeasuredGraphCtrl

IMPLEMENT_DYNAMIC(CMeasuredGraphCtrl, CGdipStatic)

CMeasuredGraphCtrl::CMeasuredGraphCtrl()
{
	m_dwColorBody = RGB(255, 255, 255);
	m_dwColorAxis1 = RGB(204, 204, 204);
	m_dwColorAxis2 = RGB(51, 51, 51);
	m_dwColorOutline = RGB(150, 150, 170);

	m_nDivisionX = 20;
	m_nDivisionY = 10;
	m_strLabelY = _T("Axis Y");
	m_strTitle = _T("Graph");

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

	m_bFixedRangeAxisY = FALSE;
}

CMeasuredGraphCtrl::~CMeasuredGraphCtrl()
{
	if (m_pFontFamily)
	{
		::delete m_pFontFamily;
		m_pFontFamily = NULL;
	}
}


BEGIN_MESSAGE_MAP(CMeasuredGraphCtrl, CGdipStatic)
END_MESSAGE_MAP()

int CMeasuredGraphCtrl::AddGraph( LPCTSTR lpszLabel, COLORREF dwColor )
{
	GraphItem dummyItem;
	m_GraphItems.push_back(dummyItem);
	m_fGraphMax = FLT_MIN;
	m_fGraphMin = FLT_MAX;
	m_GraphColor.push_back(dwColor);
	m_GraphLabel.push_back(CString(lpszLabel));

	return (int)m_GraphItems.size();
}

const int CMeasuredGraphCtrl::GetGraphCount() const
{
	return (int)m_GraphItems.size();
}

void CMeasuredGraphCtrl::Redraw()
{
	if (GetSafeHwnd())
		Invalidate();
}

void CMeasuredGraphCtrl::AddData( int nAxisIndexX, float fData )
{
	if (nAxisIndexX < 0 || nAxisIndexX >= GetGraphCount())
		return;

	GraphItem& item = m_GraphItems.at(nAxisIndexX);
	
	if (fData < m_fGraphMin)
		m_fGraphMin = fData;
	if (fData > m_fGraphMax)
		m_fGraphMax = fData;
	item.push_back( fData );
}

void CMeasuredGraphCtrl::ResetData( int nAxisIndexX )
{
	if (nAxisIndexX < 0 || nAxisIndexX >= GetGraphCount())
		return;

	m_fGraphMax = FLT_MIN;
	m_fGraphMin = FLT_MAX;
	m_bFixedRangeAxisY = FALSE;

	GraphItem& item = m_GraphItems.at(nAxisIndexX);
	item.clear();
}

void CMeasuredGraphCtrl::ResetGraph()
{
	m_GraphItems.clear();
	m_fGraphMax = FLT_MIN;
	m_fGraphMin = FLT_MAX;
	m_GraphColor.clear();
	m_GraphLabel.clear();
	m_bFixedRangeAxisY = FALSE;
}

void CMeasuredGraphCtrl::SetFixedRangeAxisY( float fMin, float fMax )
{
	m_bFixedRangeAxisY = TRUE;
	m_fGraphMax = fMax;
	m_fGraphMin = fMin;
}

void CMeasuredGraphCtrl::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	Color colorBody;
	colorBody.SetFromCOLORREF(m_dwColorBody);
	rect.Inflate( 2, 2 );
	g.FillRectangle( &SolidBrush(colorBody), rect );
}

void CMeasuredGraphCtrl::OnDraw( Graphics& g, Rect rect )
{
	using namespace Gdiplus;
	g.SetSmoothingMode( SmoothingModeHighQuality );
	g.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

	Gdiplus::Font font(m_pFontFamily, 11, FontStyleRegular, UnitPixel);
	SolidBrush brushText(Color::Black);

	Rect rectBody = rect;
	rectBody.X = 0;
	rectBody.Width--;
	rectBody.Y = 0;
	rectBody.Height--;

	Rect rectInnerBody = rectBody;
	rectInnerBody.Inflate( -2, -2 );

	const int nLeftDivisionWidth = 50;
	const int nBottomDivisionHeight = 20;
	const int nRightLegendWidth = 80;
	const int nTopMargin = 10;
	const int nInnerMarginX = 10;
	const float fDataRangeMargin = 0.05f;

	Rect rectGraph = rectInnerBody;
	rectGraph.X += nLeftDivisionWidth;
	rectGraph.Y += nTopMargin;
	rectGraph.Height -= nBottomDivisionHeight+nTopMargin;
	rectGraph.Width -= nLeftDivisionWidth+nRightLegendWidth;

	float fMax = m_fGraphMax;
	float fMin = m_fGraphMin;
	if (fMax == FLT_MIN ||
		fMin == FLT_MAX)
	{
		fMax = 100.f;
		fMin = 0.f;
	}

	if (!m_bFixedRangeAxisY)
	{
		float fRange = fMax - fMin;
		fMax = fMax + fRange * fDataRangeMargin;
		fMin = fMin - fRange * fDataRangeMargin;
		if (fMin < 0.f) fMin = 0.f;
	}

	DrawInterLineAndValue(g, font, brushText, fMin, fMax, rectGraph, nLeftDivisionWidth );

	DrawAxisXText(g, font, brushText, rectGraph, nInnerMarginX);

	for (int i=0 ; i<(int)m_GraphItems.size() ; i++)
	{
		GraphItem& item = m_GraphItems.at(i);
		COLORREF dwColor = m_GraphColor.at(i);
		Color colorData( 250, GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor) );
		Pen penData( colorData, 1.5f );
		DrawGraph( g, penData, item, fMin, fMax, rectGraph, nInnerMarginX );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 외곽선
	Color colorOutline;
	colorOutline.SetFromCOLORREF(m_dwColorOutline);
	Pen penOutline( colorOutline, 1 );
	g.DrawRectangle( &penOutline, rectBody );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 범례
	Rect rectLegend = rectInnerBody;
	rectLegend.X = rectInnerBody.GetRight() - nRightLegendWidth;
	rectLegend.Width = nRightLegendWidth;

	DrawLegend(g, font, brushText, rectLegend);
}

void CMeasuredGraphCtrl::DrawLegend( Gdiplus::Graphics& g, Gdiplus::Font& font, Gdiplus::SolidBrush& brushText, Gdiplus::Rect rectLegend )
{
	const int nLegendOffsetY = rectLegend.Height / 8;
	const int nLegendMarginX = 5;
	RectF rectLegendItem;
	Rect rectLegendColor;
	int nColorWH = 14;
	Color colorLegend;

	StringFormat stringFormatLegend;
	stringFormatLegend.SetAlignment(StringAlignmentNear);
	stringFormatLegend.SetLineAlignment(StringAlignmentCenter);
	stringFormatLegend.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap|StringFormatFlagsNoClip);

	g.SetSmoothingMode( SmoothingModeNone );
	for (int i=0 ; i<(int)m_GraphItems.size() ; i++)
	{
		COLORREF dwColor = m_GraphColor.at(i);
		colorLegend.SetFromCOLORREF(dwColor);
		CString& strLabel = m_GraphLabel.at(i);

		rectLegendItem.X = (float)rectLegend.X + nLegendMarginX;
		rectLegendItem.Width = (float)rectLegend.Width - nLegendMarginX;
		rectLegendItem.Y = (float)rectLegend.Y + nLegendOffsetY + i * 22;
		rectLegendItem.Height = 20.f;

		rectLegendColor.X = (int)(rectLegendItem.X + 0.5f);
		rectLegendColor.Y = (int)(rectLegendItem.Y + (rectLegendItem.Height-nColorWH) / 2.f + 0.5f);
		rectLegendColor.Width = nColorWH;
		rectLegendColor.Height = nColorWH;

		rectLegendItem.X += (float)nColorWH + 2.f;
		rectLegendItem.Width -= (float)nColorWH + 2.f;

		g.FillRectangle( &SolidBrush(colorLegend), rectLegendColor );

		BSTR bstrLabel = strLabel.AllocSysString();
		g.DrawString( bstrLabel, -1, &font, rectLegendItem, &stringFormatLegend, &brushText );
		::SysFreeString(bstrLabel);
	}
}

void CMeasuredGraphCtrl::DrawAxisXText( Gdiplus::Graphics& g, Gdiplus::Font& font, Gdiplus::SolidBrush& brushText, Gdiplus::Rect rectGraph, const int nInnerMarginX )
{
	const int nXMarginY = 1;
	int nIndexMin = 1;
	int nIndexMax = 100;
	if (m_GraphItems.size() != 0)
	{
		if (m_GraphItems[0].size() != 0)
		{
			nIndexMax = (int)m_GraphItems[0].size();
		}
	}
	int nIndexRange = nIndexMax - nIndexMin;
	float fStepIndexRange = 0.f;
	int nDivisionX = m_nDivisionX;
	if (nDivisionX != 0)
	{
		fStepIndexRange = (float)nIndexRange / nDivisionX;
		if (fStepIndexRange < 1.f)
		{
			fStepIndexRange = 1.f;
			nDivisionX = nIndexRange;
		}
	}
	float fIndex = 0.f;

	StringFormat stringFormatX;
	stringFormatX.SetAlignment(StringAlignmentCenter);
	stringFormatX.SetLineAlignment(StringAlignmentCenter);

	RectF rectXF;
	rectXF.Y = (float)rectGraph.GetBottom() + nXMarginY;
	rectXF.Height = 20.f;
	float fIndexPosX = (float)rectGraph.GetLeft() + nInnerMarginX;
	float fStepX = 0.f;
	if (nDivisionX != 0)
	{
		fStepX = (float)(rectGraph.Width-nInnerMarginX*2) / nDivisionX;
	}
	int nIndexData;
	CString strIndex;
	for (int i=0 ; i<nDivisionX ; i++)
	{
		nIndexData = (int)(nIndexMin + (fStepIndexRange*i));
		strIndex.Format( _T("%d"), nIndexData );
		BSTR bstrIndex = strIndex.AllocSysString();
		rectXF.X = fIndexPosX-fStepX/2.f;
		rectXF.Width = fStepX;
		g.DrawString(bstrIndex, -1, &font, rectXF, &stringFormatX, &brushText);
		::SysFreeString(bstrIndex);
		fIndexPosX += fStepX;
	}
	fIndexPosX = (float)(rectGraph.GetRight() - nInnerMarginX);
	nIndexData = nIndexMax;
	strIndex.Format( _T("%d"), nIndexData );
	BSTR bstrIndex = strIndex.AllocSysString();
	rectXF.X = fIndexPosX-fStepX/2.f;
	rectXF.Width = fStepX;
	g.DrawString(bstrIndex, -1, &font, rectXF, &stringFormatX, &brushText);
	::SysFreeString(bstrIndex);
}

void CMeasuredGraphCtrl::DrawInterLineAndValue( Gdiplus::Graphics& g, Gdiplus::Font& font, Gdiplus::SolidBrush& brushText, float fMin, float fMax, Gdiplus::Rect rectGraph, const int nLeftDivisionWidth )
{
	StringFormat stringFormatY;
	stringFormatY.SetAlignment(StringAlignmentFar);
	stringFormatY.SetLineAlignment(StringAlignmentCenter);
	//stringFormatY.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap|StringFormatFlagsNoClip);

	Color colorAxis1;
	colorAxis1.SetFromCOLORREF(m_dwColorAxis1);
	Pen penAxis1( colorAxis1, 1 );
	Color colorAxis2;
	colorAxis2.SetFromCOLORREF(m_dwColorAxis2);
	Pen penAxis2( colorAxis2, 1 );

	float fDivisionStepY = (float)rectGraph.Height / m_nDivisionY;
	float fY = (float)rectGraph.Y;
	int nY;

	float fRange = fMax - fMin;
	float fStepRange = 0.f;
	if (m_nDivisionY != 0)
	{
		fStepRange = fRange / m_nDivisionY;
	}
	float fValueY = 0.f;

	const int nYMarginX = 5;
	CString strY;
	RectF rectYF;
	rectYF.X = (float)rectGraph.GetLeft() - nLeftDivisionWidth;
	rectYF.Width = (float)nLeftDivisionWidth - nYMarginX;
	for (int i=0 ; i<m_nDivisionY ; i++)
	{
		nY = (int)(fY + 0.5f);

		fValueY = (fMax - fStepRange*i);
		if ( (int)(fValueY*10.f - (int)fValueY*10.f) == 0 )
			strY.Format( _T("%d"), (int)fValueY );
		else
			strY.Format( _T("%.1f"), fValueY );

		g.DrawLine( &penAxis1, rectGraph.GetLeft(), nY, rectGraph.GetRight(), nY );

		rectYF.Y = (float)nY - 10.f;
		rectYF.Height = 20.f;

		BSTR bstrY = strY.AllocSysString();
		g.DrawString( bstrY, -1, &font, rectYF, &stringFormatY, &brushText );
		::SysFreeString(bstrY);

		fY += fDivisionStepY;
	}

	//strY.Format( _T("%.1f"), fMin );
	if ( (int)(fValueY*10.f - (int)fValueY*10.f) == 0 )
		strY.Format( _T("%d"), (int)fMin );
	else
		strY.Format( _T("%.1f"), fMin );

	rectYF.Y = (float)rectGraph.GetBottom() - 10.f;
	rectYF.Height = 20.f;

	g.DrawLine( &penAxis2, rectGraph.GetLeft(), rectGraph.GetBottom(), rectGraph.GetRight(), rectGraph.GetBottom() );

	BSTR bstrY = strY.AllocSysString();
	g.DrawString( bstrY, -1, &font, rectYF, &stringFormatY, &brushText );
	::SysFreeString(bstrY);
}

void CMeasuredGraphCtrl::DrawGraph( Gdiplus::Graphics& g, Gdiplus::Pen& penData, GraphItem& graphData, float fMin, float fMax, Gdiplus::Rect rectGraph, const int nInnerMarginX )
{
	using namespace Gdiplus;

	rectGraph.X += nInnerMarginX;
	rectGraph.Width -= nInnerMarginX*2;

	int nDataCount = (int)graphData.size();

	float fStepX = (float)rectGraph.Width / (nDataCount-1);
	float fX = (float)rectGraph.X;
	int nX = 0;
	int nY = 0;

	Point* pGraphPoints = new Point[nDataCount];

	for (int nI=0 ; nI<nDataCount ; nI++)
	{
		nX = (int)(fX + 0.5f);

		float fData = graphData.at(nI);

		nY = rectGraph.Height - (int)(rectGraph.Height * (fData-fMin) / (fMax-fMin) + 0.5f) + rectGraph.Y;
		if (nY < rectGraph.GetTop()) nY = rectGraph.GetTop();
		if (nY > rectGraph.GetBottom()) nY = rectGraph.GetBottom();

		pGraphPoints[nI].X = nX;
		pGraphPoints[nI].Y = nY;

		fX += fStepX;
	}

	g.DrawLines( &penData, pGraphPoints, nDataCount );

	delete[] pGraphPoints;
}



void CMeasuredGraphCtrl::PreSubclassWindow()
{
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	CGdipStatic::PreSubclassWindow();
}
