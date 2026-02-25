#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <atlimage.h>

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#define DLG_VIEW_MAX_NUMBER 10

class TeachOCRReRegistration : public CDialogEx
{
	DECLARE_DYNAMIC(TeachOCRReRegistration)

public:
	TeachOCRReRegistration(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~TeachOCRReRegistration();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_OCR_FONTREREGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

protected:
	Gdiplus::Image*	m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush m_brushBody;
	int m_nBorderWidth;
	int m_nOcrMode; // Segment Image Data Load 할 때 사용 250120 황대은J
protected:
	typedef struct _stOCRSegmentData
	{
		CString strCharacterName;
		
		std::vector<CString> vstrPath;
		std::vector<CString> vstrCharacterSize;

		std::vector<BOOL> vbCheck;

		void clear()
		{
			strCharacterName.Empty();

			vstrPath.clear();
			vstrCharacterSize.clear();

			vbCheck.clear();
		}

	}stOCRSegmentData;

	CxImageViewCtrl m_ImageViewRefont[DLG_VIEW_MAX_NUMBER];
	CxImageViewManager m_ImageViewManagerRefont[DLG_VIEW_MAX_NUMBER];
	CxImageObject m_ImageObjectRefont[DLG_VIEW_MAX_NUMBER];

	static BOOL APIENTRY ImageViewEventRefont( ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData );
	static BOOL APIENTRY ImageViewMouseEventRefont( DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData );
	static void APIENTRY ImageViewDrawExtRefont( IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData );

protected:	
	int nCharacterIndex;
	int nSegmentNumber;

	std::vector<stOCRSegmentData> m_SegmentfileData;

	void SegmentImageDataLoad();
	void ViewListUpdate();

	BOOL DeleteFolderContents(const CString& folderPath);

public:
	CTabCtrl	m_FontCharacterTab;
	CComboBox	m_FontCharacterCombobox;
	
	UIExt::CFlatButton m_btnListPrev;
	UIExt::CFlatButton m_btnListNext;
	UIExt::CFlatButton m_btnSelectAllList;
	UIExt::CFlatButton m_btnDataAllClear;
	UIExt::CFlatButton m_btnFontReRegistration;
	UIExt::CFlatButton m_btnClose;

	void SetOcrMode(int nOcrMode) { m_nOcrMode = nOcrMode; }
	void UpdateLanguage();

	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTcnSelchangeTabOcrtext(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonListprev();
	afx_msg void OnBnClickedButtonListnext();
	afx_msg void OnCbnSelchangeComboOcrtext();
	afx_msg void OnBnClickedButtonSelectalllits();
	afx_msg void OnBnClickedButtonSelectdataallclear();
	afx_msg void OnBnClickedButtonRecreatefontfile();
	afx_msg void OnBnClickedOk();
};
