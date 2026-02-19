#pragma once

//#include "UIExt/GdipWndExt.h"

// CSideBar
class CSideBar : public CWnd/*: public UIExt::CGdipBaseWnd*/
{
	DECLARE_DYNAMIC(CSideBar)

public:
	CSideBar();
	virtual ~CSideBar();

	BOOL Create(CWnd* pParentWnd = NULL);

	CFormView* CreateFormView( CRuntimeClass* pClass );

protected:
	CArray<CFormView*> m_FormViews;
	
protected:
	//virtual void OnDraw(Graphics& g, Rect rect) override;
	//virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


