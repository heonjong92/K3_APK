#pragma once

#include "UIExt/GdipWndExt.h"
#include "UIExt/DisplayTimeCtrl.h"

// CBottomStatusBar

class CBottomStatusBar : public UIExt::CGdipBaseWnd
{
	DECLARE_DYNAMIC(CBottomStatusBar)

public:
	CBottomStatusBar();
	virtual ~CBottomStatusBar();

	BOOL Create(CWnd* pParentWnd = NULL);

	void NotifyAlive();

protected:
	Gdiplus::Image*	m_pImageHITSLogo;

	UIExt::CDisplayTimeCtrl		m_wndDisplayTimeCtrl;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


