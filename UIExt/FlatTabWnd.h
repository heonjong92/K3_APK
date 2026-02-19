#pragma once

#include <afxtempl.h>
#include "GdipWndExt.h"

namespace UIExt
{

extern const UINT WM_FLAT_TABWND_SELCHANGE;
extern const UINT WM_FLAT_TABWND_SELCHANGED;

// CFlatTabWnd

class CFlatTabWnd : public CGdipStatic
{
	DECLARE_DYNAMIC(CFlatTabWnd)

public:
	CFlatTabWnd();
	virtual ~CFlatTabWnd();

	BOOL IsHorzTab();

	enum TabDirection { TabDirectionHorzLeftTop, TabDirectionHorzRightTop, TabDirectionVertLeftTop, TabDirectionVertLeftBottom };
	void SetTabDirection( TabDirection eDirection, int nBarHeight, int nBarOffset=0 );
	void SetTabWidth( int nBarWidth=0 );	// 0: variable, 
	void SetTabTextHeight( int nHeight ) { m_nTextHeight = nHeight; }

	INT_PTR AddTab( LPCTSTR lpszCaption, CWnd* pChildWnd, Color colorTab=Color(200, 255, 60, 60), Image* pIconImage=NULL );

	void SelectTab( INT_PTR nI );

	INT_PTR GetActiveTab() { return m_nActiveTabIdx; }

	void SetTabText( INT_PTR nI, LPCTSTR lpszNewText );

	void NotifyTab( INT_PTR nI );
	void ResetNotify();

	void EnableTabChange( BOOL bEnable );

	void SetBackgroundColor( DWORD dwColor );

	BOOL EnableWindow(BOOL bEnable);
protected:

	BOOL		m_bIsHovered;
	BOOL		m_bIsMouseDown;

	COLORREF	m_dwBackgroundColor;

	BOOL		m_bEnableTabChange;

	BOOL				m_bNotifyTab;
	CArray<INT_PTR>		m_aryNotifyTabIdx;
	int			m_nBlinkCount;

	int			m_nBarOffset;

	TabDirection m_eTabDirection;
	CSize		m_szTabHeader;
	int			m_nBarWidth;
	int			m_nTextHeight;

protected:
	virtual void OnDraw( Graphics& g, Rect rect ) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

	typedef struct tagTabInfo
	{
		CString strCaption;
		CRect	rcArea;
		CWnd*	pChildWnd;
		CSize	szTab;
		Image*	pIconImage;
		Color	colorTab;
	} TabInfo;

	CArray<TabInfo, TabInfo>	m_aryTabInfo;
	INT_PTR		m_nActiveTabIdx;
	INT_PTR		m_nHoverTabIdx;
	CRect		m_rcChildArea;

	void DrawTabs( Graphics& g, Rect rc );
	void DrawTab( Graphics& g, Rect& rectTab, const TabInfo& ti, BOOL bHover, BOOL bActive, BOOL bNotify );

	CSize CalcTabSize( Graphics& g, const TabInfo& ti );

	INT_PTR IsNotifyTab( INT_PTR nI );

protected:
	void OnMouseEnter();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PreSubclassWindow();
public:
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
};

inline BOOL CFlatTabWnd::IsHorzTab()
{
	return (m_eTabDirection == CFlatTabWnd::TabDirectionHorzLeftTop || m_eTabDirection == CFlatTabWnd::TabDirectionHorzRightTop) ? TRUE : FALSE;
}

} // namespace UIExt