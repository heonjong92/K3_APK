#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab3GDesiccantCutting 대화 상자입니다.
class CAPKView;
class CTeachTab3GDesiccantCutting : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab3GDesiccantCutting)

public:
	CTeachTab3GDesiccantCutting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab3GDesiccantCutting();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_3G_DESICCANT_CUTTING };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::stDesiccantCuttingInfo m_DesiccantCuttingInfo;

	UIExt::CFlatLabel m_wndLabelTitle3G;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_DesiccantPosition;
	UIExt::CFlatButton m_btnTest_DesiccantPosition2;
	UIExt::CFlatButton m_btnTest_DesiccantCutting;
	UIExt::CFlatButton m_btnTest_DesiccantCutting2;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnLeftROI;
	UIExt::CFlatButton m_btnRightROI;

	UIExt::CFlatButton m_btnPreviewLeft;
	UIExt::CFlatButton m_btnPreviewRight;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int	m_nSelectRecipeIndex;

	BOOL m_bIsCheckedLeftROI;
	BOOL m_bIsCheckedRightROI;

	BOOL m_bIsPreviewLeft;
	BOOL m_bIsPreviewRight;

public:
	static BOOL m_bPreviewLeft;
	static BOOL m_bPreviewRight;

	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void UpdateUI();
	void Refresh();
	void CheckData();
	void DisableWnd(BOOL bEnable);

	void UpdateLanguage();

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
	void InitScroll();

	int m_nScrollPos;
public:
	afx_msg void OnBnClickedRadioDesiccantPositionBypass();
	afx_msg void OnBnClickedRadioDesiccantPosition2Bypass();
	afx_msg void OnBnClickedRadioDesiccantCuttingBypass();
	afx_msg void OnBnClickedRadioDesiccantCutting2Bypass();
	afx_msg void OnBnClickedBtnTestDesiccantPosition();
	afx_msg void OnBnClickedBtnTestDesiccantPosition2();
	afx_msg void OnBnClickedBtnTestDesiccantCutting();
	afx_msg void OnBnClickedBtnTestDesiccantCutting2();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDesiccantCuttingRecipeNo();
	afx_msg void OnBnClickedBtnRoiDesiccantCuttingLeft();
	afx_msg void OnBnClickedBtnRoiDesiccantCuttingRight();
	afx_msg void OnBnClickedCheckViewConvertedImage();
	afx_msg void OnBnClickedCheckViewConvertedImage2();
	afx_msg void OnBnClickedBtnRoiDesiccantPositionLeft();
protected:
	UIExt::CFlatButton m_btnPositionLeftROI;
	BOOL m_bIsPositionLeftROI;
	UIExt::CFlatButton m_btnPositionRightROI;
	BOOL m_bIsPositionRightROI;
public:
	afx_msg void OnBnClickedBtnRoiDesiccantPositionRight();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
