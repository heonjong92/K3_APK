#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab6GBoxTape 대화 상자입니다.
class CAPKView;
class CTeachTab6GBoxTape : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab6GBoxTape)

public:
	CTeachTab6GBoxTape(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab6GBoxTape();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_6G_BOX_TAPE };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stTapeInfo		m_Tape;

	UIExt::CFlatLabel m_wndLabelTitle6G;
	UIExt::CFlatLabel m_wndLabel;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_Tape;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	
	CToolTipCtrl m_toolTip;

	CFont				m_fontRecipe;

	CComboBox			m_wndSelectRecipe;
	int					m_nSelectRecipeIndex;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void Refresh();
	void CheckData();
	void DisableWnd(BOOL bEnable);

	void UpdateLanguage();
	void UpdateToolTip();

protected:
	void UpdateRecipeList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnClickedRadioTapeBypass();
	afx_msg void OnBnClickedBtnTestBoxTape();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnTapeRecipeNo();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
