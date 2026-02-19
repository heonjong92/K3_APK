#pragma once

#include "GdipWndExt.h"
#include <vector>

// CMeasuredGraphCtrl

namespace UIExt {

class CMeasuredGraphCtrl : public CGdipStatic
{
	DECLARE_DYNAMIC(CMeasuredGraphCtrl)

public:
	int AddGraph( LPCTSTR lpszLabel, COLORREF dwColor );
	void AddData( int nGraphIndex, float fData );
	void ResetData( int nGraphIndex );
	void ResetGraph();
	void SetFixedRangeAxisY( float fMin, float fMax );
	void SetDivisionY( int nDivisionY )
	{
		m_nDivisionY = nDivisionY;
	}
	void SetDivisionX( int nDivisionX )
	{
		m_nDivisionX = nDivisionX;
	}
	const int GetGraphCount() const;

	void SetLabelAxisY( LPCTSTR lpszLabel )
	{
		m_strLabelY = lpszLabel;
	}
	void SetTitle( LPCTSTR lpszTitle )
	{
		m_strTitle = lpszTitle;
	}

	void Redraw();

protected:
	Gdiplus::FontFamily* m_pFontFamily;

	typedef std::vector<float>	GraphItem;
	std::vector<GraphItem>		m_GraphItems;
	std::vector<COLORREF>		m_GraphColor;
	std::vector<CString>		m_GraphLabel;

	float		m_fGraphMin;
	float		m_fGraphMax;

	COLORREF	m_dwColorBody;
	COLORREF	m_dwColorAxis1;
	COLORREF	m_dwColorAxis2;
	COLORREF	m_dwColorOutline;

	int			m_nDivisionX;
	int			m_nDivisionY;

	CString		m_strLabelY;
	CString		m_strTitle;

	BOOL		m_bFixedRangeAxisY;

protected:
	virtual void OnDraw( Graphics& g, Rect rect ) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;
	void DrawGraph( Gdiplus::Graphics& g, Gdiplus::Pen& penData, GraphItem& graphData, float fMin, float fMax, Gdiplus::Rect rectGraph, const int nInnerMarginX );
	void DrawInterLineAndValue( Gdiplus::Graphics& g, Gdiplus::Font& font, Gdiplus::SolidBrush& brushText, float fMin, float fMax, Gdiplus::Rect rectGraph, const int nLeftDivisionWidth );
	void DrawAxisXText( Gdiplus::Graphics& g, Gdiplus::Font& font, Gdiplus::SolidBrush& brushText, Gdiplus::Rect rectGraph, const int nInnerMarginX );
	void DrawLegend( Gdiplus::Graphics& g, Gdiplus::Font& font, Gdiplus::SolidBrush& brushText, Gdiplus::Rect rectLegend );

public:
	CMeasuredGraphCtrl();
	virtual ~CMeasuredGraphCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

}