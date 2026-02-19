#pragma once

#include "GdipWndExt.h"

// CDisplayTimeCtrl

namespace UIExt
{

class CDisplayTimeCtrl : public CGdipBaseWnd
{
	DECLARE_DYNAMIC(CDisplayTimeCtrl)

public:
	CDisplayTimeCtrl();
	virtual ~CDisplayTimeCtrl();

	BOOL Create(CWnd* pParentWnd = NULL);

protected:
	Gdiplus::Image*	m_pIconTime;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};


} // namespace UIExt