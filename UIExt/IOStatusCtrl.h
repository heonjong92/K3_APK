#pragma once

#include "GdipWndExt.h"

// CIOStatusCtrl
namespace VisionDevice {
class CxIOCard;
}

namespace UIExt
{

class CIOStatusCtrl : public CGdipBaseWnd
{
	DECLARE_DYNAMIC(CIOStatusCtrl)

public:
	CIOStatusCtrl();
	virtual ~CIOStatusCtrl();

	enum IOType { IOTypeInput=0, IOTypeOutput=1 };

	BOOL Create(CWnd* pParentWnd = NULL);

	void SetIOModule( VisionDevice::CxIOCard* pIOCard, IOType type, int nIOCount );
	void EnableMouseEvent( BOOL bEnable ) { m_bEnableMouseEvent = bEnable; }

protected:
	VisionDevice::CxIOCard*	m_pIOCard;
	int			m_nIOCount;
	DWORD		m_dwIOStatus;
	BOOL		m_bEnableMouseEvent;
	Gdiplus::Rect* m_pIORects;

	Gdiplus::Image*	m_pImageIcon;
	CSize	m_sizeIcon;
	IOType	m_IOType;

	void ToggleIO( int nIndex );
	BOOL HitTest( CPoint point, int& nIndex );

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};


} // namespace UIExt