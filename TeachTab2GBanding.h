#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab2GBanding 대화 상자입니다.
class CAPKView;
class CTeachTab2GBanding : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab2GBanding)

public:
	CTeachTab2GBanding(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab2GBanding();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_2G_BANDING };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stBandingInfo		m_Banding;

	UIExt::CFlatLabel m_wndLabelTitle2G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_Banding;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnTeachBanding;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	BOOL m_bIsTeachBanding;
	BOOL m_bPendingSaveAfterTeaching;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void Refresh();
	void UpdateUI();
	void CheckData();
	void DisableWnd(BOOL bEnable);

	void UpdateLanguage();
	void UpdateToolTip();
	void LockButtonsUntilSave();

protected:
	void UpdateRecipeList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	static void APIENTRY _OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData);
	void OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex);

public:
	afx_msg void OnBnClickedRadioBandingBypass();
	afx_msg void OnBnClickedBtnTestBanding();
	afx_msg void OnBnClickedBtnTestBanding2();
	afx_msg void OnBnClickedBtnTestBanding3();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnBandingRecipeNo();
	afx_msg void OnBnClickedBtnRoiBanding();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
