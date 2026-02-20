#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include <XGraphic/xGraphicObject.h>
#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

extern UINT nManualChipNum; // Tray Chip Cnt 수동 검사용

// CTeachTab1G3DChipCnt 대화 상자입니다.
class CAPKView;
class CTeachTab1G3DChipCnt : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab1G3DChipCnt)

public:
	CTeachTab1G3DChipCnt(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab1G3DChipCnt();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_1G_3D_CHIP_COUNT };

protected:
	CAPKView*	m_pMainView;

	CModelInfo::st3DChipCnt m_3DChipCnt;

	UIExt::CFlatLabel m_wndLabelTitle1G3DChipCnt;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_3DChipCnt;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSave;

	UIExt::CFlatButton m_btnFirstROI;
	
	BOOL m_bIsTeachFirst;
	BOOL m_bCheckEqualize;
	BOOL m_bPendingSaveAfterTeaching;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	// px, mm 단위 변환 에디트
	int m_nRowPitch;
	int m_nColumnPitch;
	int m_nChipWidth;
	int m_nChipHeight;

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
	void EnableCutShift();

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
	afx_msg void OnBnClickedRadio3DChipCntBypass();
	afx_msg void OnBnClickedRadio3DChipSelectUnit();
	afx_msg void OnBnClickedBtnTest3DChipCnt();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtn3DChipCntRecipeNo();
	afx_msg void OnBnClickedBtnRoi3dchipcntFirst();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCheckChipcntEqualize();
};
