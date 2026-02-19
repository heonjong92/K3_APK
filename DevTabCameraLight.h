#pragma once

#include "UIExt/FlatLabel.h"
#include "UIExt/FlatButton.h"

#include "SystemConfig.h"

// CDevTabCameraLight 대화 상자입니다.
class CAPKView;
class CDevTabCameraLight : public CDialog
{
	DECLARE_DYNAMIC(CDevTabCameraLight)

public:
	CDevTabCameraLight(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDevTabCameraLight();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DEV_TAB_CAMERA_LIGHT };

protected:
	CAPKView*	m_pMainView;
	CSystemConfig::LightControllerLFine m_LightPort;

	UIExt::CFlatLabel m_wndLabelTitleAreaCamera;
	UIExt::CFlatLabel m_wndLabelTitleLight1;
	UIExt::CFlatLabel m_wndLabelTitleLight2;
	UIExt::CFlatLabel m_wndLabelTitleLight3;

	UIExt::CFlatButton m_btnAreaLive;
	UIExt::CFlatButton m_btnAreaLive2;
	UIExt::CFlatButton m_btnAreaLive3;
	UIExt::CFlatButton m_btnAreaLive4;
	UIExt::CFlatButton m_btnAreaLive5;
	UIExt::CFlatButton m_btnAreaLive6;

	UIExt::CFlatButton m_btnAreaGrab;
	UIExt::CFlatButton m_btnAreaGrab2;
	UIExt::CFlatButton m_btnAreaGrab3;
	UIExt::CFlatButton m_btnAreaGrab4;
	UIExt::CFlatButton m_btnAreaGrab5;
	UIExt::CFlatButton m_btnAreaGrab6;

	UIExt::CFlatButton m_btnAreaIdle;
	UIExt::CFlatButton m_btnAreaIdle2;
	UIExt::CFlatButton m_btnAreaIdle3;
	UIExt::CFlatButton m_btnAreaIdle4;
	UIExt::CFlatButton m_btnAreaIdle5;
	UIExt::CFlatButton m_btnAreaIdle6;

	UIExt::CFlatButton m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH_MAX][LedCtrlTypeAreaMax];
	UIExt::CFlatButton m_btnApplyLctrlPortLight[LedCtrlTypeAreaMax];

	CSliderCtrl		m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH_MAX][LedCtrlTypeAreaMax];

	CComboBox	m_wndCbLCtrlPortArea[LedCtrlTypeAreaMax];
	int m_nAreaLightCtrlPort[LedCtrlTypeAreaMax];
	int m_nAreaLight[USE_LFINE_LCP100_CTRL_CH_MAX][LedCtrlTypeAreaMax];
	int m_nScrollPos;

	CSystemConfig::Cam3DScaleValue m_CamScaleValue;

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }
	void Cleanup();

	BOOL IsModified();

	BOOL Save();
	void Refresh();
	void UpdateUI();

	void SetLightAllOff(LedCtrlType ledctrltype, eUSE_LFINE_LCP100_CTRL_CH_MAX ch);
	void SetBtnLightOnOff( short nCtrlId, LedCtrlType ledctrltype, eUSE_LFINE_LCP100_CTRL_CH_MAX ch );
	void EditLightVal( short nCtrlId, LedCtrlType ledctrltype, eUSE_LFINE_LCP100_CTRL_CH_MAX ch );

	void SetBtnApplyLctrlPort( short nCtrlId, LedCtrlType ledctrltype );

	void EnableCameraScale(BOOL bEnable);

protected:
	void Load();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	virtual BOOL OnInitDialog();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedBtnApplyLctrlPortLight1();
	afx_msg void OnBnClickedBtnApplyLctrlPortLight2();
	afx_msg void OnBnClickedBtnApplyLctrlPortLight3();

	afx_msg void OnCbnSelchangeCbLctrlPort();
	afx_msg void OnCbnSelchangeCbLctrlPort2();
	afx_msg void OnCbnSelchangeCbLctrlPort3();

	afx_msg void OnEnChangeEditCh1Light1Val();
	afx_msg void OnEnChangeEditCh2Light1Val();
	afx_msg void OnEnChangeEditCh3Light1Val();
	afx_msg void OnEnChangeEditCh4Light1Val();

	afx_msg void OnEnChangeEditCh1Light2Val();
	afx_msg void OnEnChangeEditCh2Light2Val();
	afx_msg void OnEnChangeEditCh3Light2Val();
	afx_msg void OnEnChangeEditCh4Light2Val();

	afx_msg void OnEnChangeEditCh1Light3Val();
	afx_msg void OnEnChangeEditCh2Light3Val();
	afx_msg void OnEnChangeEditCh3Light3Val();
	afx_msg void OnEnChangeEditCh4Light3Val();

public:
	afx_msg void OnBnClickedBtnAreaCamLive();
	afx_msg void OnBnClickedBtnAreaCamLive2();
	afx_msg void OnBnClickedBtnAreaCamLive3();
	afx_msg void OnBnClickedBtnAreaCamLive4();
	afx_msg void OnBnClickedBtnAreaCamLive5();
	afx_msg void OnBnClickedBtnAreaCamLive6();

	afx_msg void OnBnClickedBtnAreaCamIdle();
	afx_msg void OnBnClickedBtnAreaCamIdle2();
	afx_msg void OnBnClickedBtnAreaCamIdle3();
	afx_msg void OnBnClickedBtnAreaCamIdle4();
	afx_msg void OnBnClickedBtnAreaCamIdle5();
	afx_msg void OnBnClickedBtnAreaCamIdle6();
	afx_msg void OnBnClickedBtnResetGraphic();

	afx_msg void OnBnClickedBtnOnoffCh1Light1();
	afx_msg void OnBnClickedBtnOnoffCh2Light1();
	afx_msg void OnBnClickedBtnOnoffCh3Light1();
	afx_msg void OnBnClickedBtnOnoffCh4Light1();

	afx_msg void OnBnClickedBtnOnoffCh1Light2();
	afx_msg void OnBnClickedBtnOnoffCh2Light2();
	afx_msg void OnBnClickedBtnOnoffCh3Light2();
	afx_msg void OnBnClickedBtnOnoffCh4Light2();

	afx_msg void OnBnClickedBtnOnoffCh1Light3();
	afx_msg void OnBnClickedBtnOnoffCh2Light3();
	afx_msg void OnBnClickedBtnOnoffCh3Light3();
	afx_msg void OnBnClickedBtnOnoffCh4Light3();

	afx_msg void OnBnClickedBtnAreaCamGrab();
	afx_msg void OnBnClickedBtnAreaCamGrab2();
	afx_msg void OnBnClickedBtnAreaCamGrab3();
	afx_msg void OnBnClickedBtnAreaCamGrab4();
	afx_msg void OnBnClickedBtnAreaCamGrab5();
	afx_msg void OnBnClickedBtnAreaCamGrab6();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	BOOL PreTranslateMessage(MSG* msg) override;
};

