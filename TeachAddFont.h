#pragma once
#include "resource.h"

#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

class TeachAddFont : public CDialogEx
{
	DECLARE_DYNAMIC(TeachAddFont)
public:
	TeachAddFont(CWnd* pParent = NULL);
	virtual ~TeachAddFont();

	enum { IDD = IDD_TEACH_OCR_FONTADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	DECLARE_MESSAGE_MAP()

protected:
	Gdiplus::Image*	m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	COLORREF m_clrBody;
	CBrush m_brushBody;
	int m_nBorderWidth;

	BOOL bCheckOne;
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	int nSegmentIndex;
	CString strReadingText;
	CString m_TeachingText;

	UIExt::CFlatLabel m_SegmentNum;
	UIExt::CFlatLabel m_ReadingText;
	UIExt::CFlatLabel m_Teaching_Text;

	UIExt::CFlatButton m_Btn_Apply;
	UIExt::CFlatButton m_Btn_Cancell;
};
