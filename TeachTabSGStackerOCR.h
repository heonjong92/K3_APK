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
// CTeachTabSGStackerOcr 대화 상자

class CAPKView;
class CTeachTabSGStackerOcr : public CDialog
{
	DECLARE_DYNAMIC(CTeachTabSGStackerOcr)

public:
	CTeachTabSGStackerOcr(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachTabSGStackerOcr();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_TAB_SG_STACKER_OCR };


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

	CModelInfo::stStackerOcr m_StackerOcr;

	UIExt::CFlatLabel m_wndLabelTitleSGStackerOcr;
	UIExt::CFlatLabel m_wndLabelRecipeTitle;

	UIExt::CFlatButton m_btnTest_StackerOcr;
	UIExt::CFlatButton m_btnSaveRecipeNo;
	UIExt::CFlatButton m_btnSaveRecipeInfo;
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnOcrPreview;
	UIExt::CFlatButton m_btnDepthOffset;
	UIExt::CFlatButton m_btnStackerNonInspTeachModel;
	UIExt::CFlatButton m_btnOcrFontRegister;
	
	CComboBox m_Combo_Stacker_Select_Masking_Num;
	int m_nCombo_Stacker_Masking_Select;

	// Teach
	BOOL m_bIsStackerNonInsp;
	BOOL m_bIsOcrFontRegister;
	BOOL m_bOCRPreview;

	CToolTipCtrl m_toolTip;

	CFont m_fontRecipe;

	CComboBox m_wndMethod;
	CComboBox m_wndSelectRecipe;
	int m_nSelectRecipeIndex;

	stOCRFontData m_OCRFontLoadData;
	VisionProcess::TrayOCRData m_stTrayData;
	VisionProcess::stOCRResult m_OCRFontReadingData;

	CxImageObject m_ImageObjectForOcr;

public:
	void SetMainView(CAPKView* pView) { m_pMainView = pView; }
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

	//void UpdateCount_Tab(BOOL bComboReset = FALSE);
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

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnTestStackerOcr();
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnStackerOcrRecipeNo();
	afx_msg void OnBnClickedCheckStackerocrUseAdaptiveThreshold();
	afx_msg void OnBnClickedBtnOcrPreview();
	afx_msg void OnCbnSelchangeComboStackerocrMethod();
	afx_msg void OnBnClickedBtnStackerFontRegister();
	afx_msg void OnBnClickedButtonSetDepthOffset();
	afx_msg void OnBnClickedBtnStackerTabNoninspTeaching();
	afx_msg void OnBnClickedBtnStackerTabNoninspTeachingDelete();
	afx_msg void OnCbnSelchangeComboStackerNonInspModelNum();
	afx_msg void OnBnClickedButtonLoadBinData();
	afx_msg void OnBnClickedBtnStackerocrRecipeInfo();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
