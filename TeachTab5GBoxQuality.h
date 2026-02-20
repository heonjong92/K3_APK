#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab5GBoxQuality 대화 상자입니다.
class CAPKView;
class CTeachTab5GBoxQuality : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab5GBoxQuality)

public:
	CTeachTab5GBoxQuality(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab5GBoxQuality();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_5G_BOX_QUALITY };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stBoxInfo		m_BoxInfo;

	UIExt::CFlatLabel m_wndLabelTitle5G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_BoxQuality;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnBoxTeachModel;

	CToolTipCtrl m_toolTip;

	CFont				m_fontRecipe;

	CComboBox			m_wndSelectRecipe;
	int					m_nSelectRecipeIndex;

	CComboBox			m_Combo_SelectModelNum;
	int					m_nModelNum;

	BOOL m_bIsTeachBoxQuality;

	CxImageViewCtrl		m_ImageViewForBoxQualityTeaching;
	CxImageViewManager	m_ImageViewManagerForBoxQualityTeaching;
	CxImageObject		m_ImageObjectForBoxQualityTeaching;

	CStatic m_ctrlPicPreviewTeachBoxQuality;
	CRect m_rcPicCtrSizeTeachBoxQuality;

	BOOL m_bPendingSaveAfterTeaching;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void Refresh();
	void UpdateUI();
	void UpdateTeachingImage();
	void UpdateCount_Box();
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
	afx_msg void OnBnClickedRadioBoxBypass();
	afx_msg void OnBnClickedBtnTestBoxQuality();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnBoxQualityRecipeNo();
	afx_msg void OnBnClickedTeachingBoxQuality();
	afx_msg void OnBnClickedBtnBoxTeachModel();
	afx_msg void OnCbnSelchangeComboBoxQualityModelNum();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
