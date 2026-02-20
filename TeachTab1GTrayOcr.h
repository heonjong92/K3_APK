#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "InspectionVision.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FilteredEdit.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

// CTeachTab1GTrayOcr 대화 상자입니다.
class CAPKView;
class CTeachTab1GTrayOcr : public CDialog
{
	DECLARE_DYNAMIC(CTeachTab1GTrayOcr)

public:
	CTeachTab1GTrayOcr(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTab1GTrayOcr();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_1G_TRAYOCR };

protected:
	typedef struct _stOCRFontData
	{
		BOOL bSegmentFileLoad;

		CString strFilePath;
		int nImgType;

		CString strSegmentLineNum;
		CString strMinWidth;
		CString strMaxWidth;
		CString strMinHeight;
		CString strMaxHeight;

		void clear()
		{
			bSegmentFileLoad = FALSE;

			strFilePath.Empty();
			nImgType = 0;

			strSegmentLineNum.Empty();
			strMinWidth.Empty();
			strMaxWidth.Empty();
			strMinHeight.Empty();
			strMaxHeight.Empty();
		}
	}stOCRFontData;

	CAPKView* m_pMainView;

	CModelInfo::stTrayOcr m_TrayOcr;

	UIExt::CFlatLabel m_wndLabelTitle1GTrayOcr;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_TrayOcr;
	UIExt::CFlatButton m_btnTabAlignTeachModel;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSaveRecipeInfo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnOcrFontRegister;
	UIExt::CFlatButton m_btnROI;
	UIExt::CFlatButton m_btnOcrPreview;
	UIExt::CFlatButton m_btnTrayNonInspTeachModel;

	CComboBox m_Combo_Tray_Select_Masking_Num;
	int m_nCombo_Tray_Masking_Select;

	BOOL m_bIsTeachTabBegin;
	BOOL m_bIsTeachOcrROI;
	BOOL m_bIsTrayNonInsp;
	BOOL m_bPendingSaveAfterTeaching;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	CComboBox m_wndMethod;

	BOOL m_bIsOcrFontRegister;
	BOOL m_bOCRPreview;

	CStatic m_ctrlPicPreviewTeachTab;		// pic
	CRect m_rcPicCtrlSizeTeachTab;			// pic size

	stOCRFontData m_OCRFontLoadData;
	VisionProcess::TrayOCRData m_stTrayData;
	VisionProcess::stOCRResult m_OCRFontReadingData;

	CxImageObject m_ImageObjectForOcr;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Default();
	void Refresh();
	void UpdateUI();
	void UpdateMethod();
	void UpdateMasking();
	void CheckData();
	void DisableWnd(BOOL bEnable);

	void UpdateLanguage();
	void UpdateToolTip();

	void EnableAdaptiveWindows();
	void LockButtonsUntilSave();

	//void UpdateCount_Tab(BOOL bComboReset = FALSE);
	void UpdateTeachingImage_Tab();
	void NoninspTeachingDelete_All();

	CxImageObject* GetObject_TeachOCR() { return &m_ImageObjectForOcr; }
	void SelectRecipeIndex(int nIndex);

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
	static void APIENTRY _OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData);
	void OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex);

	CxImageViewCtrl		m_ImageViewForTabTeaching;
	CxImageViewManager	m_ImageViewManagerForTabTeaching;
	CxImageObject		m_ImageObjectForTabTeaching;

	//void OCRSegmentReading();
public:
	afx_msg void OnBnClickedRadioTrayOcrBypass();
	afx_msg void OnBnClickedBtnTestTrayOcr();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnTrayOcrRecipeNo();
	afx_msg void OnBnClickedBtnTestTrayocrFontRegister();
	afx_msg void OnBnClickedBtnRoiTrayocr();
	afx_msg void OnBnClickedBtnTabAlignTeachModel();
	afx_msg void OnBnClickedCheckTrayocrUseAdaptiveThreshold();
	afx_msg void OnBnClickedBtnOcrPreview();
	afx_msg void OnCbnSelchangeComboTrayocrMethod();
	afx_msg void OnBnClickedBtnTabNoninspTeaching();
	afx_msg void OnCbnSelchangeComboTrayNonInspModelNum();
	afx_msg void OnBnClickedBtnTabNoninspTeachingDelete();
	afx_msg void OnBnClickedBtnTrayocrRecipeInfo();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
