#pragma once
#include "afxwin.h"

#include "UIExt/FlatButton.h"

// CConfirmExitDlg 대화 상자입니다.

class CConfirmExitDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfirmExitDlg)

public:
	CConfirmExitDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConfirmExitDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONFIRM_EXIT };

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush	m_brushBody;
	int		m_nBorderWidth;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	int m_nExitCode;
	CString m_strEtc;

	UIExt::CFlatButton m_btnOK;
	UIExt::CFlatButton m_btnCancel;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedExitCode();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnNcHitTest(CPoint point);

};
