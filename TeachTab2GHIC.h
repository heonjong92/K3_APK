#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab2GHIC 대화 상자입니다.
class CAPKView;
class CTeachTab2GHIC : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab2GHIC)

public:
	CTeachTab2GHIC(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab2GHIC();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_2G_HIC };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stHICInfo		m_HIC;

	UIExt::CFlatLabel m_wndLabelTitle2G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_HIC;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnHICTechModel;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int	m_nSelectRecipeIndex;

	BOOL m_bIsTeachHIC;
	BOOL m_bPendingSaveAfterTeaching;

	CStatic m_ctrlPicPreviewTeachHIC;		// pic
	CRect m_rcPicCtrlSizeTeachHIC;			// pic size

public:
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
	void EnableHICLight();

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

	CxImageViewCtrl		m_ImageViewForHICTeaching;
	CxImageViewManager	m_ImageViewManagerForHICTeaching;
	CxImageObject		m_ImageObjectForHICTeaching;

public:
	afx_msg void OnBnClickedRadioHicBypass();
	afx_msg void OnBnClickedBtnTestHic();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnHICRecipeNo();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnHicTechModel();
};
