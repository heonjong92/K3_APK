#pragma once

#include "UIExt/FlatButton.h"
#include "afxwin.h"

// CChangePasswordDlg 대화 상자입니다.

class CChangePasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CChangePasswordDlg)
protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush	m_brushBody;
	int		m_nBorderWidth;

	UIExt::CFlatButton m_btnOK;
	UIExt::CFlatButton m_btnCancel;

public:
	CString m_strNewPassword;
	CString m_strConfirmPassword;
public:
	CChangePasswordDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChangePasswordDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHANGE_PASSWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();

	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual BOOL OnInitDialog();
};
