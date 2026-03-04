#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab3GDesiccantMaterial 대화 상자입니다.
class CAPKView;
class CTeachTab3GDesiccantMaterial : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab3GDesiccantMaterial)

public:
	CTeachTab3GDesiccantMaterial(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab3GDesiccantMaterial();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_3G_DESICCANT_MATERIAL };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stDesiccantMaterialInfo		m_DesiccantMaterialInfo;
	CModelInfo::stDesiccantMaterialTrayInfo m_DesiccantMaterialTrayInfo;

	UIExt::CFlatLabel m_wndLabelTitle3G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;
	UIExt::CFlatLabel m_wndLabelRecipeTitleTray;

	UIExt::CFlatButton m_btnTest_SubMaterial;
	UIExt::CFlatButton m_btnTest_SubMaterialTray;
	UIExt::CFlatButton m_btnSubMaterialTechModel;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;

	UIExt::CFlatButton m_btnTeachModelTray;
	UIExt::CFlatButton m_btnSaveTrayRecipeNo;

	//BOOL m_bIsTeachROI;
	BOOL m_bIsTeachTrayROI;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int	m_nSelectRecipeIndex;
	
	CStatic m_ctrlPicPreviewTeachSubMaterial;		// pic
	CRect m_rcPicCtrlSizeTeachSubMaterial;			// pic size
	int m_RadioTeachingSubMaterial;					// select radio button
	BOOL m_bIsTeachSubMaterial;						// teaching

	CStatic m_ctrlPicPreviewTeachTray;

	CComboBox m_wndSelectTrayRecipe;
	int	m_nSelectTrayRecipeIndex;

	int m_nRadioTeachingSubMaterialTray;

	int m_nScrollPos;

public:
	static int m_nManualTray; // 수동검사 전용

	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void Refresh();
	void UpdateUI();
	void UpdateTeachingImage();
	void CheckData();
	void DisableWnd(BOOL bEnable);
	void EnableContionalCtrl(const CModelInfo::stDesiccantMaterialInfo& stDesiccant, BOOL bEnable);

	void UpdateLanguage();

protected:
	void UpdateRecipeList();
	void UpdateTrayRecipeList();

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

	void InitScroll();

	CxImageViewCtrl		m_ImageViewForHICDesiccantTeaching;
	CxImageViewManager	m_ImageViewManagerForHICDesiccantTeaching;
	CxImageObject		m_ImageObjectForHICDesiccantTeaching;

public:
	afx_msg void OnBnClickedRadioMaterialBypass();
	afx_msg void OnBnClickedRadioTeachingSubmaterial();
	afx_msg void OnBnClickedBtnSubmaterialTechModel();
	afx_msg void OnBnClickedBtnTestSubmaterial();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnCbnSelchangeComboRecipeMaterialTray();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnMaterialRecipeNo();
	afx_msg void OnBnClickedBtnSubmaterialTeachTraymodel();
	afx_msg void OnBnClickedBtnMaterialRecipeNoTray();
	afx_msg void OnBnClickedRadioTeachingSubmaterialTray();
	afx_msg void OnBnClickedBtnTestSubmaterialTray();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
