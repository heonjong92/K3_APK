#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FlatButton.h"
#include "JobNumberListCtrl.h"
#include "TeachTabSGStackerOCR.h"
#include "TeachTab1GTrayOcr.h"

// CTrayRecipeInfoDlg 대화 상자입니다.

class CTrayRecipeInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CTrayRecipeInfoDlg)

public:
#ifdef RELEASE_1G
	CTrayRecipeInfoDlg(CTeachTab1GTrayOcr* pParentTab, CWnd* pParent = NULL);   // 표준 생성자입니다.
#else
	CTrayRecipeInfoDlg(CTeachTabSGStackerOcr* pParentTab, CWnd* pParent = NULL);   // 표준 생성자입니다.
#endif
	virtual ~CTrayRecipeInfoDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TRAY_RECIPE_INFO	};

protected:
	Gdiplus::Image*			m_pIconImage;

	CString m_strTitle;

	COLORREF m_clrBorder;
	CBrush	m_brushBody;
	COLORREF m_clrBody;
	int m_nBorderWidth;

	CStatic m_ctrlPicPreviewRecipeInfo;		// pic
	CRect m_rcPicCtrlSizeRecipeInfo;		// pic size

	CString m_strSelectRecipe;
	int m_nSelectIndex;

public:
	int  m_nRecipeJobNumber;
	CString m_strKind;
	eTeachTabIndex m_eTabIdx;

	void SetTitleName(CString strName) { m_strTitle = strName; }
	void SetRecipeKind(CString strKind, eTeachTabIndex eTabIdx) { m_strKind = strKind; m_eTabIdx = eTabIdx;  }
	LPCTSTR GetRecipeKind() { return m_strKind; }
	eTeachTabIndex GetRecipeKindTabIdx() { return m_eTabIdx; }

private:
#ifdef RELEASE_1G
	CTeachTab1GTrayOcr* m_pParentTab;
#else
	CTeachTabSGStackerOcr* m_pParentTab;
#endif // RELEASE_1G


protected:
	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnClose;
	CJobNumberListCtrl m_wndJobNumberList;

	void UpdateJobNumberList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	CxImageViewCtrl		m_ImageViewForRecipeInfo;
	CxImageViewManager	m_ImageViewManagerForRecipeInfo;
	CxImageObject		m_ImageObjectForRecipeInfo;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMClickJobNumberList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedJobNumberList(NMHDR* pNMHDR, LRESULT* pResult);
};
