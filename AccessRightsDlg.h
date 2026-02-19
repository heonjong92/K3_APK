#pragma once
#include "afxwin.h"

#include "UIExt/FlatButton.h"

// CAccessRightsDlg 대화 상자입니다.

class CAccessRightsDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccessRightsDlg)

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush	m_brushBody;
	int m_nBorderWidth;

	UIExt::CFlatButton m_btnOK;
	UIExt::CFlatButton m_btnCancel;

public:
	CAccessRightsDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAccessRightsDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ACCESS_RIGHTS };

protected:
	int m_nAccessRightType;
	CString m_strPassword;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnChangePasswd();
	afx_msg void OnBnClickedRadioAccessRights();
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
