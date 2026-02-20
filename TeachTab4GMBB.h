#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab4GMBB 대화 상자입니다.
class CAPKView;
class CTeachTab4GMBB : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab4GMBB)

public:
	CTeachTab4GMBB(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab4GMBB();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_4G_MBB };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stMBBInfo m_MBB;

	UIExt::CFlatLabel m_wndLabelTitle4G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_MBB;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnTeachMBB;
	UIExt::CFlatButton m_btnMBBTechModel;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	BOOL m_bIsTeachMBBSize;
	BOOL m_bIsTeachMBBRotate;
	BOOL m_bPendingSaveAfterTeaching;

	CStatic m_ctrlPicPreviewTeachMBB;		// pic
	CRect m_rcPicCtrlSizeTeachMBB;			// pic size

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
	void EnableMBB();

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

	CxImageViewCtrl		m_ImageViewForMBBTeaching;
	CxImageViewManager	m_ImageViewManagerForMBBTeaching;
	CxImageObject		m_ImageObjectForMBBTeaching;

public:
	afx_msg void OnBnClickedRadioMbbBypass();
	afx_msg void OnBnClickedBtnTestMbb();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnMbbRecipeNo();
	afx_msg void OnBnClickedBtnRoiMbb();
	afx_msg void OnBnClickedBtnMbbTechModel();
	afx_msg void OnBnClickedRadioSelectUnit();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
