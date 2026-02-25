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
class TeachOCRFontTray : public CDialogEx
{
	DECLARE_DYNAMIC(TeachOCRFontTray)

public:
	TeachOCRFontTray(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~TeachOCRFontTray();

	enum { IDD = IDD_TEACH_OCR_FONT_TRAY };

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

	CString m_strFontPatch;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<stSegmentFileData> m_SegmentFileData;
	stOCRFontData m_OCRFontLoadData;
	VisionProcess::TrayOCRData m_TrayData;
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

	void ViewGraphicObjectUpdate();
	void AddFontData(int nAreaNum, CPoint ptImagePoint);

public:
	CxImageObject* GetObject_TeachOCR() { return &m_ImageObjectForOcr; }
	void UpdateLanguage();

	UIExt::CFlatButton m_btnFontSave;
	UIExt::CFlatButton m_btnViewRefresh;
	UIExt::CFlatButton m_btnDataClear;
	UIExt::CFlatButton m_btnReRegistration;
	UIExt::CFlatButton m_btnClose;

	//////////////////////////////////////////////////////////////////////////

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFontteachBtnFontsave();
	afx_msg void OnBnClickedFontteachBtnViewrefresh();
	afx_msg void OnBnClickedFontteachBtnReregisterFont();
};
