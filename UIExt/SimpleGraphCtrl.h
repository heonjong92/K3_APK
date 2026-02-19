#pragma once

#include "GdipWndExt.h"

// CSimpleGraphCtrl

namespace UIExt {

class CSimpleGraphCtrl : public CGdipStatic
{
	DECLARE_DYNAMIC(CSimpleGraphCtrl)

public:
	void AddData( float fData );
	void ResetData();
	void SetMaxDataCount( int nMaxDataCount );
	void SetRangeAxisY( float fMin, float fMax );
	void EnableCenterAxis( BOOL bEnable );
	void SetCenterAxisY( float fCenter );
	void EnableLimitY( BOOL bEnable );
	void SetRangeLimitAxisY(float fLimitMin, float fLimitMax );

protected:
	Gdiplus::FontFamily* m_pFontFamily;

	float		m_fDataMax;
	float		m_fDataMin;
	float		m_fDataLimitMax;
	float		m_fDataLimitMin;

	float		m_fDataCenter;
	BOOL		m_bEnableCenterAxis;
	BOOL		m_bEnableRangeLimit;

	COLORREF	m_dwColorData;
	COLORREF	m_dwColorBody;
	COLORREF	m_dwColorAxis;
	COLORREF	m_dwColorCenterAxis;
	COLORREF	m_dwColorLimitAxis;
	COLORREF	m_dwColorLimitOverData;

	int			m_nMaxDataCount;

	CArray<float, float>	m_aryGraphData;

protected:
	virtual void OnDraw( Graphics& g, Rect rect ) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;
	void DrawData( Gdiplus::Graphics& g, Gdiplus::Pen& penData, Gdiplus::Brush& brushData, CArray<float, float>& aryData, Gdiplus::Rect rectGraph );

public:
	CSimpleGraphCtrl();
	virtual ~CSimpleGraphCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

}