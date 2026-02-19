#pragma once

#include "UIExt/GdipWndExt.h"

// CTitleBar

class CTitleBar : public UIExt::CGdipBaseWnd
{
	DECLARE_DYNAMIC(CTitleBar)

public:
	CTitleBar();
	virtual ~CTitleBar();

	BOOL Create(CWnd* pParentWnd = NULL);

	void SetModelName( LPCTSTR lpszModelName );
	void SetJobNumber( UINT nJobNumber );
	void SetAccessName( LPCTSTR lpszAccessName );
	CString GetCompileTime();

protected:
	Gdiplus::Image*	m_pLogoImage;
	CString			m_strTitle;
	UINT			m_nJobNumber;
	CString			m_strModelName;
	CString			m_strAccessName;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
};


