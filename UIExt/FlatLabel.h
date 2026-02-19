#pragma once

#include "GdipWndExt.h"

// CFlatLabel

namespace UIExt
{

class CFlatLabel : public CGdipStatic
{
	DECLARE_DYNAMIC(CFlatLabel)

protected:
	COLORREF		m_dwTextColor;
	COLORREF		m_dwBodyColor;
	COLORREF		m_dwBorderColor;
	Gdiplus::Image* m_pImageIcon;
	int				m_nBorderWidth;
	int				m_nFontHeight;
	BOOL			m_bIsBold;

	virtual void OnDraw(Gdiplus::Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

public:
	void SetColor( COLORREF dwBodyColor, COLORREF dwTextColor, COLORREF dwBorderColor=-1 );
	void SetBorderWidth( int nWidth );
	void SetBold( BOOL bBold );
	void SetFontHeight( int nHeight=0 );	// 0: variable, other: fixed
	CFlatLabel();
	virtual ~CFlatLabel();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
};

} // namespace UIExt