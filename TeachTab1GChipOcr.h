#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab1GChipOcr 대화 상자입니다.
class CAPKView;
class CTeachTab1GChipOcr : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab1GChipOcr)

public:
	CTeachTab1GChipOcr(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab1GChipOcr();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_1G_CHIPOCR };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stChipOcr		m_ChipOcr;

	UIExt::CFlatLabel m_wndLabelTitle1G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_ChipOcr;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnTeachChipROI;	// Tray Outline 모델 등록 버튼
	UIExt::CFlatButton m_btnTeachChipOcr2;  // Chip Align 모델 등록 버튼
	UIExt::CFlatButton m_btnOcrFontRegister;
	UIExt::CFlatButton m_btnOcrROI;

	CToolTipCtrl m_toolTip;
	
	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	BOOL m_bIsTeachChipROI;
	BOOL m_bIsTeachChipOcr2;
	BOOL m_bIsOcrROI;
	BOOL m_bPendingSaveAfterTeaching;

	CStatic m_ctrlPicPreviewTeach2;
	CRect m_rcPicCtrlSizeTeach2;
	
	CxImageViewCtrl		m_ImgViewForTrayOutlineTeaching2;
	CxImageViewManager	m_ImgViewManager2;
	CxImageObject		m_ImgObj2;

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
	void UpdateTeachingImage();

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
	afx_msg void OnBnClickedRadioChipOcrBypass();
	afx_msg void OnBnClickedBtnTestChipOcr();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnChipOcrRecipeNo();
	afx_msg void OnBnClickedBtnRoiChipOcr();
	afx_msg void OnBnClickedBtnRoiChipOcr2();
	afx_msg void OnBnClickedRadioChipocrBlackOnWhite();
	afx_msg void OnBnClickedBtnChiproiOcrFontTeach();
	//afx_msg void OnBnClickedRadioChipOcrBlack();
	afx_msg void OnBnClickedBtnRoiChipocrOcrarea();
	afx_msg void OnBnClickedBtnThreshold();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};