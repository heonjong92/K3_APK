#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab1GChip 대화 상자입니다.
class CAPKView;
class CTeachTab1GChip : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab1GChip)

public:
	CTeachTab1GChip(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab1GChip();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_1G_CHIP	};

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stChip		m_Chip;

	UIExt::CFlatLabel m_wndLabelTitlePanel;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_Chip;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnTeachChipArea;
	UIExt::CFlatButton m_btnTeachChipAreaCheck;

	CToolTipCtrl m_toolTip;
	
	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int	m_nSelectRecipeIndex;

	BOOL m_bIsTeachChipArea;
	BOOL m_bIsTeachChipAreaCheck;
	BOOL m_bIsTeachMatch;
	BOOL m_bPendingSaveAfterTeaching;

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

	void EnableChip();
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
	afx_msg void OnBnClickedRadioChipBypass();
	afx_msg void OnBnClickedRadioSelectUnit();
	afx_msg void OnBnClickedBtnTestChip();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnChipRecipeNo();
	afx_msg void OnBnClickedBtnRoiChipArea();
	afx_msg void OnBnClickedBtnRoiCheckarea();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
