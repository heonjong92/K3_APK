#pragma once

#include "GdipWndExt.h"

// CHeartBeatCtrl

namespace UIExt
{

class CHeartBeatCtrl : public CGdipBaseWnd
{
	DECLARE_DYNAMIC(CHeartBeatCtrl)

public:
	CHeartBeatCtrl();
	virtual ~CHeartBeatCtrl();

	BOOL Create(CWnd* pParentWnd = NULL);

	void Beat();

protected:
	int m_nPosX;
	int m_nPosStep;
	BOOL m_bBeat;
	BOOL m_bDelayBeat;
	int m_nBeatStep;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

} // namespace UIExt