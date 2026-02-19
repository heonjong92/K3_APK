#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "UIExt/FlatButton.h"
#include "JobNumberListCtrl.h"

// CEditJobNumberDlg 대화 상자입니다.

class CEditJobNumberDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditJobNumberDlg)

public:
	CEditJobNumberDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEditJobNumberDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDIT_JOBNUMBER };

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	CBrush	m_brushBody;
	COLORREF m_clrBody;
	int m_nBorderWidth;

public:
	int  m_nRecipeJobNumber;
	CString m_strKind;
	eTeachTabIndex m_eTabIdx;

	void SetTitleName(CString strName) { m_strTitle = strName; }
	void SetRecipeKind(CString strKind, eTeachTabIndex eTabIdx) { m_strKind = strKind; m_eTabIdx = eTabIdx;  }
	LPCTSTR GetRecipeKind() { return m_strKind; }
	eTeachTabIndex GetRecipeKindTabIdx() { return m_eTabIdx; }

protected:
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnClose;
	CJobNumberListCtrl m_wndJobNumberList;

	void UpdateJobNumberList();
	void AddComboList( CStringList* pComboList );

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT PopulateComboList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCancel();
};
