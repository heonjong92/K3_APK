#pragma once
#include "TeachTabLabel.h"
#include "TeachAddFont.h"
#include "InspectionVision.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>
#include <vector>

#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"
#include "afxwin.h"

// TeachOCRFontTray 대화 상자
class TeachAddFont;
class TeachOCRReRegistration;
class TeachOCRFontChip : public CDialogEx
{
	DECLARE_DYNAMIC(TeachOCRFontChip)

public:
	TeachOCRFontChip(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~TeachOCRFontChip();

	enum { IDD = IDD_TEACH_OCR_FONT_CHIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

protected:
	Gdiplus::Image*	m_pIconImage;

	CString	 m_strTitle;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush m_brushBody;
	int m_nBorderWidth;
	int m_nMenuIconSize;

protected:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef struct _stSegmentFileData
	{
		CString strSegmentLineNum;
		CString strFilePath;

		void clear()
		{
			strSegmentLineNum.Empty();
			strFilePath.Empty();
		}
	}stSegmentFileData;
	////////////////////////////////////////////////////////////////////////////////////////////////////
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
	////////////////////////////////////////////////////////////////////////////////////////////////////

	CString strFontPatch;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<stSegmentFileData> m_SegmentFileData;
	stOCRFontData m_OCRFontLoadData;
	//VisionProcess::TrayOCRData m_TrayData;
	VisionProcess::ChipOCRData m_ChipData;
	VisionProcess::stOCRResult m_OCRFontReadingData;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CxImageViewCtrl m_ImageViewForOcr;
	CxImageViewManager m_ImageViewManagerForOcr;
	CxImageObject m_ImageObjectForOcr;
	CPoint m_FontSizeViewPosition;
	////////////////////////////////////////////////////////////////////////////////////////////////////


	static BOOL APIENTRY ImageViewEvent_OCR(ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData);
	static BOOL APIENTRY ImageViewMouseEvent_OCR(DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData);
	static void APIENTRY ImageViewDrawExt_OCR(IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData);

	void OCRSegmentReading(BOOL bSave = FALSE);
	void OCRSegmentBtnCheck();
	//void OCRSegmentImgTypeCheck(int nImageType);
	//void SegmentImageChange(CString strImagePath);

	void ViewGraphicObjectUpdate();
	void AddFontData(int nAreaNum, CPoint ptImagePoint);

public:
	CxImageObject* GetObject_TeachOCR() { return &m_ImageObjectForOcr; }

	UIExt::CFlatButton m_btnFontSave;
	UIExt::CFlatButton m_btnViewRefresh;
	UIExt::CFlatButton m_btnDataClear;
	UIExt::CFlatButton m_btnReRegistration;
	UIExt::CFlatButton m_btnClose;

	//CString strImgSubCode[LABLE_INSPECTION_MAX_IMAGE_TYPE];				// 0:Center Cut Image, 1:Algorithm Cut Image, 2:OneThird Cut Image, 3:TwoThirds Cut Image, 4:Original Image
	//int m_nImageType;					// Image Type 0:Center Cut Image, 1:Algorithm Cut Image, 2:OneThird Cut Image, 3:TwoThirds Cut Image, 4:Original Image

	//CComboBox m_Combo_Option1;				// Image Type
	//CComboBox m_Combo_Option2;				// Threshold Check
	//CComboBox m_Combo_Option3;				// Image Filtering Check
	//CComboBox m_Combo_Option4;				// Font Size Filtering Check
	//CComboBox m_Combo_Option5;				// eVision Large Chars Cut Check

	//CButton m_Check_ManualTeachingMode;		// Reading Option Manual Teaching Mode
	//CComboBox m_Combo_ManualOption;			// Manual Option List


	//BOOL SetReadingOption(VisionProcess::SegmentReadingOption &stReadingOption, BOOL bTeachingMode);
	//void SetManualOptionList();

	//////////////////////////////////////////////////////////////////////////

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFontteachBtnFontsave();
	afx_msg void OnBnClickedFontteachBtnViewrefresh();
	//afx_msg void OnBnClickedFontteachBtnTeachingDataClear();
	afx_msg void OnBnClickedFontteachBtnReregisterFont();
};
