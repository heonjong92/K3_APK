#pragma once

#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"
#include "UIExt/YieldChartCtrl.h"

#include "SystemConfig.h"

// CDevTabSystem 대화 상자입니다.
class CAPKView;
class CDevTabSystem : public CDialog
{
	DECLARE_DYNAMIC(CDevTabSystem)

public:
	CDevTabSystem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDevTabSystem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DEV_TAB_SYSTEM };

protected:
	CAPKView*	m_pMainView;

	CSystemConfig::CalibrationConfig m_Calibration;
	CSystemConfig::CameraSerialNo m_CameraSerialNo;

	UIExt::CFlatLabel m_wndLabelTitleSystem;

	UIExt::CFlatLabel m_wndLabelLeft;

	UIExt::CFlatLabel m_wndLabelAreaCam;
	UIExt::CFlatLabel m_wndLabelTitleX2;
	UIExt::CFlatLabel m_wndLabelTitleY2;
	UIExt::CFlatLabel m_wndLabelTitleCam1;
	UIExt::CFlatLabel m_wndLabelTitleCam2;
	UIExt::CFlatLabel m_wndLabelTitleCam3;
	UIExt::CFlatLabel m_wndLabelTitleCam4;
	UIExt::CFlatLabel m_wndLabelTitleCam5;
	UIExt::CFlatLabel m_wndLabelTitleCam6;
	UIExt::CFlatLabel m_wndLabelTitleSerial;

	CComboBox		m_wndSaveLogPath;
	UIExt::CYieldChartCtrl m_wndYieldChart;
	int m_nExpiredDate;
	int m_nDiskCapacity;
	CString m_strSendPath;

	CButton m_chkOriginImgeSave;
	CButton m_chkAutoDelete;
	CButton m_chkOriginImgeSend;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Refresh();
	void UpdateUI(BOOL);

	void UpdateLanguage();

protected:
	void Load();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL OnInitDialog();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void UpdateDiskSpace( CString strDrive );

	afx_msg void OnCbnSelchangeCbSaveLogPath();
	afx_msg void OnBnClickedChkOriginimgsave();
	afx_msg void OnBnClickedChkAutodelete();
	afx_msg void OnBnClickedChkOriginimgsaveSend();
	BOOL PreTranslateMessage(MSG* msg) override;
};

 