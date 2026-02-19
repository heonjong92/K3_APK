#pragma once
#include "afxwin.h"
#include "resource.h"
#include "UIExt/FlatButton.h"

// CShowErrorDlg 대화 상자입니다.

class CShowErrorDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowErrorDlg)

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;
	CString m_strMessage;
	CString m_strDetail;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush	m_brushBody;
	int m_nBorderWidth;

public:
	void SetTitle( LPCTSTR lpszTitle );
	void SetMessage( LPCTSTR lpszMessage );
	void SetDetailMessage( LPCTSTR lpszDetail );

public:
	CShowErrorDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CShowErrorDlg();

	virtual void OnOK();
	virtual void OnCancel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHOW_ERROR };

protected:
	UIExt::CFlatButton m_btnOK;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnNcHitTest(CPoint point);
public:
	afx_msg void OnBnClickedOk();
};
