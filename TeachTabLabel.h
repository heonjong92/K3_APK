#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

#include "CustomTooltip.h"

// CTeachTabLabel 대화 상자입니다.
class CAPKView;
class CTeachTabLabel : public CDialog
{
	DECLARE_DYNAMIC(CTeachTabLabel)

public:
	CTeachTabLabel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTabLabel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_LABEL };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stLabelInfo		m_Label;

	UIExt::CFlatLabel m_wndLabelTitleLabel;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnLabelTechModel;
	UIExt::CFlatButton m_btnLabelTechModel_Align;
	UIExt::CFlatButton m_btnOCRFontTeaching;
	UIExt::CFlatButton m_btnTest_Label;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	
	CStatic m_ctrlPicPreviewTeachLabel;
	CStatic m_ctrlPicPreviewTeachLabel_Align;
	CRect m_rcPicCtrlSizeTeachLabel;
	CRect m_rcPicCtrlSizeTeachLabel_Align;

	BOOL m_bIsTeachLabel;
	BOOL m_bIsTeachLabel_Align;
	BOOL m_bPendingSaveAfterTeaching;

	CToolTipCtrl m_toolTip;
//	CImageToolTip m_toolTipImg;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	CComboBox m_Combo_Masking_Total_Cnt;
	CComboBox m_Combo_Select_Masking_Num;
	CComboBox m_Combo_Masking_Score;
	int m_nCombo_Masking_Select;

	int m_nInspCount;
	int m_nInspIndex;
	CSpinButtonCtrl m_wndSpinInsp;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void Refresh();
	void CheckData();
	void DisableWnd(BOOL bEnable);

	void UpdateUI();
	void UpdateTeachingImage();

	void UpdateMaskingCount(BOOL bComboReset = FALSE);
	void UpdateMaskingScore();

	void UpdateLanguage();
	void UpdateToolTip();
	void LockButtonsUntilSave();

	void ViewInspAreas();
	void AlignInspectionAreas();

protected:
	void UpdateRecipeList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//////////////////////////////////////////////////////////////////////////
	static void APIENTRY _OnConfirmTracker( CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData );
	void OnConfirmTracker( CRect& rcTrackRegion, UINT nViewIndex );

	CxImageViewCtrl		m_ImageViewForLabelTeaching;
	CxImageViewCtrl		m_ImageViewForLabelTeaching_Align;
	CxImageViewManager	m_ImageViewManagerForLabelTeaching;
	CxImageViewManager	m_ImageViewManagerForLabelTeaching_Align;
	CxImageObject		m_ImageObjectForLabelTeaching;
	CxImageObject		m_ImageObjectForLabelTeaching_Align;

public:
	afx_msg void OnBnClickedRadioLabelBypass();

	afx_msg void OnCbnSelchangeComboLabelTotalMaskingCount();
	afx_msg void OnCbnSelchangeComboLabelMaskingNumber();
	afx_msg void OnCbnSelchangeComboRecipe();

	afx_msg void OnBnClickedBtnLabelTechModel();
	afx_msg void OnBnClickedBtnLabelTechOcrfontteaching();
	afx_msg void OnBnClickedCheckLabelUsemanualdata();
	afx_msg void OnBnClickedBtnTestLabel();
	afx_msg void OnBnClickedBtnLabelRecipeNo();
	afx_msg void OnBnClickedBtnSaveLabel();
	afx_msg void OnBnClickedBtnInspAdd();
	afx_msg void OnBnClickedBtnShowarea();
	afx_msg void OnBnClickedBtnInspSeldel();
	afx_msg void OnBnClickedBtnInspAlldel();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnLabelTechAlignModel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
