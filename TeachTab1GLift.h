#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

#define	TRAY_COMMON_WIDTH	322.6

// CTeachTab1GLift 대화 상자입니다.
class CAPKView;
class CTeachTab1GLift : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab1GLift)

public:
	CTeachTab1GLift(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab1GLift();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_1G_LIFT };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stLiftInfo m_Lift;

	UIExt::CFlatLabel m_wndLabelTitle1GLift;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_Lift;
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

	void EnableLift();

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
	afx_msg void OnBnClickedRadioLiftBypass();
	afx_msg void OnBnClickedBtnTestLift();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnLiftRecipeNo();
	afx_msg void OnBnClickedRadioSelectUnit();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
