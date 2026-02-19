#if !defined(AFX_IOSTATUSCTRLEX_H__2ADF78D2_7E77_4356_99D4_4706EA30F2D0__INCLUDED_)
#define AFX_IOSTATUSCTRLEX_H__2ADF78D2_7E77_4356_99D4_4706EA30F2D0__INCLUDED_

#include <map>
#include "GdipWndExt.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IOStatusCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIOStatusCtrlEx window

namespace VisionDevice {
class CxIOCard;
}

namespace UIExt
{

class CIOStatusCtrlEx: public CGdipStatic
{
// Construction
public:
	CIOStatusCtrlEx();

	void SetIOStatus( DWORD dwStatus ) 
	{ 
		m_dwIOStatus = dwStatus; 
		Invalidate();
	}

	void EnableMouseHandleEvent( BOOL bEnable );

	enum IOType { IOTypeInput=0, IOTypeOutput=1 };

	void SetIOPortModule( VisionDevice::CxIOCard* pIOCard, IOType type, int nIOCount );

	void SetEnable( BOOL bEnable ) { m_bEnable = bEnable; }
	void SetDescription( int nIONumber, LPCTSTR lpszDescription );

// Attributes
protected:
	virtual void OnDraw(Gdiplus::Graphics& g, Rect rect) override;

	void DrawTitle( Gdiplus::Graphics& g, Gdiplus::Rect rectTitle );
	void DrawIOItems( Gdiplus::Graphics& g, Gdiplus::Rect rectIOBody );

	void ToggleIO( int nIndex );
	BOOL HitTest( CPoint point, int& nIndex );

	void ResetDescription();

	Gdiplus::Rect* m_pIORects;
	Gdiplus::Image*	m_pImageIcon;
	CSize	m_sizeIcon;

	IOType		m_IOType;
	BOOL		m_bEnableMouseEvent;
	
	DWORD		m_dwIOStatus;
	BOOL		m_bEnable;

	VisionDevice::CxIOCard*	m_pIOCard;

	UINT_PTR	m_uTimer;

	int			m_nIOCount;

	CDC			m_MemDC;
	CBitmap*	m_pBitmap;

	typedef std::map<int, CString> MapIODesc;

	MapIODesc	m_MapIODesc;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIOStatusCtrlEx)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIOStatusCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIOStatusCtrlEx)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

} // namespace UIExt

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IOSTATUSCTRLEX_H__2ADF78D2_7E77_4356_99D4_4706EA30F2D0__INCLUDED_)
