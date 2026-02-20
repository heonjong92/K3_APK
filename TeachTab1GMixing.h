#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

extern int nManualMixingNum; // 수동 Mixing 검사용 파라미터

// CTeachTab1GMixing 대화 상자입니다.
class CAPKView;
class CTeachTab1GMixing : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab1GMixing)

public:
	CTeachTab1GMixing(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab1GMixing();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_1G_MIXING };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stMixing m_Mixing;

	UIExt::CFlatLabel m_wndLabelTitle1GMixing;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_Mixing;
	UIExt::CFlatButton m_btnMixingTechModel;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int	m_nSelectRecipeIndex;

	CComboBox m_Combo_Select_Mixing_Num;
	int m_nCombo_Mixing_Select;

	BOOL m_bIsTeachMixing;
	BOOL m_bPendingSaveAfterTeaching;

	CStatic m_ctrlPicPreviewTeachMixing;
	CRect m_rcPicCtrlSizeTeachMixing;

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

	void UpdateCount_Mixing(BOOL bComboReset = FALSE);
	void UpdateTeachingImage_Mixing();

	void LockButtonsUntilSave();

protected:
	void UpdateRecipeList();
	void EnableModelTeaching();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//////////////////////////////////////////////////////////////////////////
	static void APIENTRY _OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData);
	void OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex);

	CxImageViewCtrl		m_ImageViewForMixingTeaching;
	CxImageViewManager	m_ImageViewManagerForMixingTeaching;
	CxImageObject		m_ImageObjectForMixingTeaching;

public:
	afx_msg void OnBnClickedRadioMixingBypass();
	afx_msg void OnBnClickedBtnTestMixing();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnMixingRecipeNo();
	afx_msg void OnCbnSelchangeComboMixingNumber();
	afx_msg void OnBnClickedBtnMixingTechModel();
	afx_msg void OnBnClickedCheckPlainPattern();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
