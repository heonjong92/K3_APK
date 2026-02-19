#pragma once

#include "GdipWndExt.h"

// CSystemStatusCtrl

namespace UIExt {

class CSystemStatusCtrl : public CGdipStatic
{
	DECLARE_DYNAMIC(CSystemStatusCtrl)

public:
	enum Status { StatusReady=0, StatusStop };
	void SetStatus( Status status );
	int GetStatus() { return int(m_Status); }

protected:
	Gdiplus::Image* m_pImageIcon;
	Gdiplus::Color	m_colorBody[2];
	CSize			m_sizeIcon;
	Status			m_Status;

	virtual void OnDraw(Gdiplus::Graphics& g, Gdiplus::Rect rect) override;
	virtual void OnGdipEraseBkgnd(Gdiplus::Graphics& g, Gdiplus::Rect rect) override;

public:
	CSystemStatusCtrl();
	virtual ~CSystemStatusCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

}
