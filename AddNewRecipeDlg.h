#pragma once
#include "afxwin.h"

#include "UIExt/FlatButton.h"
#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"

// CAddNewRecipeDlg 대화 상자입니다.

class CAddNewRecipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddNewRecipeDlg)

public:
	CAddNewRecipeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddNewRecipeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADD_NEW_RECIPE };

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;
	CString m_strKind;
	eTeachTabIndex m_eTabIdx;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush	m_brushBody;
	int m_nBorderWidth;
	CFont	m_fontEdit;

protected:
	CString m_strNewRecipeName;
	CComboBox m_wndRecipeList;

	UIExt::CFlatLabel		m_wndLabelRecipeName;
	UIExt::CFilteredEdit	m_edtRecipeName;

	UIExt::CFlatButton		m_btnOK;
	UIExt::CFlatButton		m_btnCancel;

	void UpdateRecipeList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnNcHitTest(CPoint point);
public:
	void SetRecipeKind(CString strKind) { m_strKind = strKind; }
	LPCTSTR GetRecipeKind() { return m_strKind; }
	void SetTeachTab(eTeachTabIndex eTabIdx) { m_eTabIdx = eTabIdx; }
	eTeachTabIndex GetTeachTab() { return m_eTabIdx; }

public:
	int m_nSelectRecipeIndex;
};
