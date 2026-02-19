#pragma once

#include "afxwin.h"

#include <XImageView/xImageViewCtrl.h>
#include <XImage/xImageObject.h>

#include "UIExt/FlatButton.h"

#include "ModelInfo.h"

enum eIndex_2D_Mapping { IDX_2D_MAPPING1=0, IDX_2D_MAPPING2, IDX_2D_MAPPING_MAX };

// CTeachThresholdPreview 대화 상자입니다.

class CTeachTab1GChipOcr;
class CAPKView;
class CTeachThresholdPreview : public CDialog
{
	DECLARE_DYNAMIC(CTeachThresholdPreview)

public:
	CTeachThresholdPreview(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachThresholdPreview();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_THRESHOLD_PREVIEW };

protected:
	CAPKView*	m_pMainView;
	Gdiplus::Image*			m_pIconImage;

	UIExt::CFlatButton m_btnSave;
	UIExt::CFlatButton m_btnExit;	

	CModelInfo::stChipOcr m_stChipOcr;


	CString					m_strTitle;

	COLORREF				m_clrBorder;
	COLORREF				m_clrBody;
	CBrush					m_brushBody;

	int						m_nBorderWidth;

	void InitButton(UIExt::CFlatButton *pBtn, DWORD dNormal, DWORD dActive);

	CTeachTab1GChipOcr*			m_pTeachTabView;

	CSliderCtrl		m_wndSliderPadThreshold;

	CxImageViewCtrl		m_wndMappingImageView[IDX_2D_MAPPING_MAX];
	CxImageViewManager	m_ImageViewManagerMappingTeach;
	CxImageObject		m_ImageObjectMappingTeach[IDX_2D_MAPPING_MAX];

	CxImageViewSyncManager m_SyncManager;

public:
	void Refresh();
	void Cleanup();
	void DisableWnd();
	void UpdateUI();
	void UpdatePreview();
	void UpdatePreviewGainOffset();

	void Save();

	void SetMainView( CTeachTab1GChipOcr* pView ) { m_pTeachTabView = pView; }

	void UpdateLanguage();

	BOOL CreateViewModeTeachMapping();

protected:
	void SetTrackerMode( BOOL bTrackerMode, int nIndex=0, FnOnConfirmTracker fnOnConfirmTracker=NULL, LPVOID lpContext=NULL );

	static void APIENTRY _OnConfirmTracker( CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData );
	void OnConfirmTracker( CRect& rcTrackRegion, UINT nViewIndex );

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetMainView(CAPKView* pView) { m_pMainView = pView; }
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditPadGain();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
