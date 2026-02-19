#pragma once

#include "GdipWndExt.h"

// CYieldChartCtrl

namespace UIExt {

class CYieldChartCtrl : public CGdipStatic
{
	DECLARE_DYNAMIC(CYieldChartCtrl)

public:
	CYieldChartCtrl();
	virtual ~CYieldChartCtrl();

	void SetYield( float fYield );

	void SetBackgroundColor( DWORD dwColor );
protected:
	float	m_fYield;

	COLORREF	m_dwBackgroundColor;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd(Graphics& g, Rect rect) override;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

} // namespace UIExt
