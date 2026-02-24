#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab5GSealingQuality 대화 상자입니다.
class CAPKView;
class CTeachTab5GSealingQuality : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab5GSealingQuality)

public:
	CTeachTab5GSealingQuality(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab5GSealingQuality();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_5G_SEALING_QUALITY };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stSealingInfo		m_Sealing;

	UIExt::CFlatLabel m_wndLabelTitle5G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_SealingQuality;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnSealingTeachModel;
	UIExt::CFlatButton m_btnPreview;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int	m_nSelectRecipeIndex;

	BOOL m_bIsTeachSealingQuality;

	BOOL m_bIsPreview;

public:
	static BOOL m_bPreview;

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
	afx_msg void OnBnClickedRadioSealingBypass();
	afx_msg void OnBnClickedBtnTestSealingQuality();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnSealingQualityRecipeNo();
	afx_msg void OnBnClickedTeachingSealingQuality();
	afx_msg void OnBnClickedBtnSealingTeachModel();
	afx_msg void OnBnClickedCheckViewConvertedImage();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedRadioSelectUnit();
};
