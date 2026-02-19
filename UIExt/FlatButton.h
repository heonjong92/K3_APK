#pragma once

#include "GdiplusExt.h"

namespace UIExt
{
// CFlatButton

class CFlatButton : public CButton
{
	DECLARE_DYNAMIC(CFlatButton)

protected:
	CDC			m_MemDC;
	CBitmap*	m_pBitmap;
	COLORREF	m_dwBackgroundColor;
	COLORREF	m_dwNormalColor;
	COLORREF	m_dwHoverColor;
	COLORREF	m_dwActiveColor;
	COLORREF	m_dwDisabledColor;
	COLORREF	m_dwBorderColor;
	COLORREF	m_dwTextColor;
	int			m_nBorderWidth;
	int			m_nFontHeight;

	BOOL		m_bIsHovered;
	BOOL		m_bIsChecked;

	BOOL		m_bShowFocusRect;

	BOOL		m_bIsBold;

	UINT		m_nButtonType;
	
	BOOL		m_bIgnoreDblClick;

	BOOL		m_bInvertIconColor;

	Gdiplus::Image* m_pImageIcon;
	int		m_nIconIndex;

public:
	CFlatButton();
	virtual ~CFlatButton();

	void SetColor( DWORD dwNormalColor, DWORD dwHoverColor, DWORD dwActiveColor, DWORD dwDisabledColor, DWORD dwTextColor, DWORD dwBorderColor=(DWORD)-1 );
	void SetButtonColor( DWORD dwNormalColor, DWORD dwActiveColor=(DWORD)-1 );
	void SetBorderWidth( int nWidth );
	void SetIcon( HMODULE hRes, LPCTSTR lpszResType, UINT nId, int nIconIndex=-1 );
	void SetInvertActiveIcon( BOOL bInvertIconColor ) { m_bInvertIconColor = bInvertIconColor; }

	void ShowFocusRect( BOOL bShow );
	void SetBackgroundColor( DWORD dwColor );
	void SetBold( BOOL bBold );
	void SetFontHeight( int nHeight=0 );	// 0: variable, other: fixed

protected:
	void DrawFocusRect( Gdiplus::Graphics& g, Gdiplus::Rect& rectBody );
	void DrawButton( CDC* pDC, CRect& rcBody, BOOL bEnabled, BOOL bFocused, BOOL bChecked, BOOL bSelected );

protected:

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	void OnMouseEnter();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg LRESULT OnSetCheck( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetCheck( WPARAM wParam, LPARAM lParam );
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnBnClicked();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

} // UIExt
