#pragma once
#include "resource.h"

// CWaitMessageDlg 대화 상자입니다.

class CWaitMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitMessageDlg)

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;
	CString m_strMessage;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush	m_brushBody;
	int m_nBorderWidth;

public:
	void SetTitle( LPCTSTR lpszTitle );
	void SetMessage( LPCTSTR lpszMessage );

	virtual void OnOK();
	virtual void OnCancel();

public:
	CWaitMessageDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWaitMessageDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHOW_WAIT_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
