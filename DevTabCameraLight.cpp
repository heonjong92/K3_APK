// DevTabCameraLight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "DevTabCameraLight.h"
#include "afxdialogex.h"

#include "APKView.h"
#include "VisionSystem.h"
#include "VisionInterfaces.h"
#include "SystemConfig.h"
#include "LightInfo.h"

#include "UIExt/ResourceManager.h"
#include <XGraphic\xGraphicObject.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define DEF_LFINE_CTRL_MAX	1024

// CDevTabCameraLight 대화 상자입니다.
IMPLEMENT_DYNAMIC(CDevTabCameraLight, CDialog)

CDevTabCameraLight::CDevTabCameraLight(CWnd* pParent /*=NULL*/)
	: CDialog(CDevTabCameraLight::IDD, pParent)
	, m_pMainView(NULL)
	, m_nScrollPos(0)
{
	for (int i = 0; i < LedCtrlTypeAreaMax; ++i)
	{
		for (int j = 0; j < USE_LFINE_LCP100_CTRL_CH_MAX; ++j)
			m_nAreaLight[j][i] = 0;

		m_nAreaLightCtrlPort[i] = -1;
	}

	for (int n = 0; n < 4; ++n)
	{
		m_CamScaleValue.fScaleLowValues[n] = 0.f;
		m_CamScaleValue.fScaleHighValues[n] = 255.f;
	}
	m_CamScaleValue.fScaleAreaLowValues = 0.f;
	m_CamScaleValue.fScaleAreaHighValues = 1024.f;
}

CDevTabCameraLight::~CDevTabCameraLight()
{
}

void CDevTabCameraLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LABEL_TITLE_AREA_CAMERA, m_wndLabelTitleAreaCamera);
	DDX_Control(pDX, IDC_LABEL_ITEM_LIGHT1, m_wndLabelTitleLight1);
	DDX_Control(pDX, IDC_LABEL_ITEM_LIGHT2, m_wndLabelTitleLight2);
	DDX_Control(pDX, IDC_LABEL_ITEM_LIGHT3, m_wndLabelTitleLight3);

	DDX_Control(pDX, IDC_BTN_AREA_CAM_LIVE,  m_btnAreaLive);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_LIVE2, m_btnAreaLive2);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_LIVE3, m_btnAreaLive3);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_LIVE4, m_btnAreaLive4);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_LIVE5, m_btnAreaLive5);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_LIVE6, m_btnAreaLive6);

	DDX_Control(pDX, IDC_BTN_AREA_CAM_GRAB,  m_btnAreaGrab);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_GRAB2, m_btnAreaGrab2);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_GRAB3, m_btnAreaGrab3);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_GRAB4, m_btnAreaGrab4);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_GRAB5, m_btnAreaGrab5);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_GRAB6, m_btnAreaGrab6);

	DDX_Control(pDX, IDC_BTN_AREA_CAM_IDLE,	 m_btnAreaIdle);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_IDLE2, m_btnAreaIdle2);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_IDLE3, m_btnAreaIdle3);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_IDLE4, m_btnAreaIdle4);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_IDLE5, m_btnAreaIdle5);
	DDX_Control(pDX, IDC_BTN_AREA_CAM_IDLE6, m_btnAreaIdle6);

	DDX_Text(pDX, IDC_EDIT_CH1_LIGHT1, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea1]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea1], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH2_LIGHT1, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea1]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea1], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH3_LIGHT1, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea1]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea1], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH4_LIGHT1, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea1]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea1], 0, DEF_LFINE_CTRL_MAX);

	DDX_Text(pDX, IDC_EDIT_CH1_LIGHT2, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea2]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea2], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH2_LIGHT2, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea2]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea2], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH3_LIGHT2, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea2]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea2], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH4_LIGHT2, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea2]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea2], 0, DEF_LFINE_CTRL_MAX);

	DDX_Text(pDX, IDC_EDIT_CH1_LIGHT3, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea3]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea3], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH2_LIGHT3, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea3]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea3], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH3_LIGHT3, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea3]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea3], 0, DEF_LFINE_CTRL_MAX);
	DDX_Text(pDX, IDC_EDIT_CH4_LIGHT3, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea3]);
	DDV_MinMaxInt(pDX, m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea3], 0, DEF_LFINE_CTRL_MAX);

	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW, m_CamScaleValue.fScaleLowValues[0]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleLowValues[0], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW, m_CamScaleValue.fScaleLowValues[1]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleLowValues[1], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW, m_CamScaleValue.fScaleLowValues[2]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleLowValues[2], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW, m_CamScaleValue.fScaleLowValues[3]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleLowValues[3], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_AREA_SCALE_VAL_LOW, m_CamScaleValue.fScaleAreaLowValues);
	
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH, m_CamScaleValue.fScaleHighValues[0]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleHighValues[0], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH, m_CamScaleValue.fScaleHighValues[1]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleHighValues[1], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH, m_CamScaleValue.fScaleHighValues[2]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleHighValues[2], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH, m_CamScaleValue.fScaleHighValues[3]);
	DDV_MinMaxFloat(pDX, m_CamScaleValue.fScaleHighValues[3], 0.f, 255.0f);
	DDX_Text(pDX, IDC_EDIT_3D_AREA_SCALE_VAL_HIGH, m_CamScaleValue.fScaleAreaHighValues);

	DDX_Control(pDX, IDC_BTN_ONOFF_CH1_LIGHT1, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH2_LIGHT1, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH3_LIGHT1, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH4_LIGHT1, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea1]);

	DDX_Control(pDX, IDC_BTN_ONOFF_CH1_LIGHT2, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH2_LIGHT2, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH3_LIGHT2, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH4_LIGHT2, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea2]);

	DDX_Control(pDX, IDC_BTN_ONOFF_CH1_LIGHT3, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea3]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH2_LIGHT3, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea3]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH3_LIGHT3, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea3]);
	DDX_Control(pDX, IDC_BTN_ONOFF_CH4_LIGHT3, m_btnOnoffLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea3]);

	DDX_Control(pDX, IDC_BTN_APPLY_LCTRL_PORT_LIGHT1, m_btnApplyLctrlPortLight[LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_BTN_APPLY_LCTRL_PORT_LIGHT2, m_btnApplyLctrlPortLight[LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_BTN_APPLY_LCTRL_PORT_LIGHT3, m_btnApplyLctrlPortLight[LedCtrlTypeArea3]);

	DDX_Control(pDX, IDC_CB_LCTRL_PORT_LIGHT1, m_wndCbLCtrlPortArea[LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_CB_LCTRL_PORT_LIGHT2, m_wndCbLCtrlPortArea[LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_CB_LCTRL_PORT_LIGHT3, m_wndCbLCtrlPortArea[LedCtrlTypeArea3]);

	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH1_LIGHT1, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH2_LIGHT1, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH3_LIGHT1, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea1]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH4_LIGHT1, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea1]);

	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH1_LIGHT2, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH2_LIGHT2, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH3_LIGHT2, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea2]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH4_LIGHT2, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea2]);

	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH1_LIGHT3, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH1][LedCtrlTypeArea3]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH2_LIGHT3, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH2][LedCtrlTypeArea3]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH3_LIGHT3, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH3][LedCtrlTypeArea3]);
	DDX_Control(pDX, IDC_SLIDER_CNTRL_CH4_LIGHT3, m_wndSliderLight[USE_LFINE_LCP100_CTRL_CH4][LedCtrlTypeArea3]);
}


BEGIN_MESSAGE_MAP(CDevTabCameraLight, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_LIVE, &CDevTabCameraLight::OnBnClickedBtnAreaCamLive)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_LIVE2, &CDevTabCameraLight::OnBnClickedBtnAreaCamLive2)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_LIVE3, &CDevTabCameraLight::OnBnClickedBtnAreaCamLive3)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_LIVE4, &CDevTabCameraLight::OnBnClickedBtnAreaCamLive4)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_LIVE5, &CDevTabCameraLight::OnBnClickedBtnAreaCamLive5)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_LIVE6, &CDevTabCameraLight::OnBnClickedBtnAreaCamLive6)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_IDLE, &CDevTabCameraLight::OnBnClickedBtnAreaCamIdle)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_IDLE2, &CDevTabCameraLight::OnBnClickedBtnAreaCamIdle2)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_IDLE3, &CDevTabCameraLight::OnBnClickedBtnAreaCamIdle3)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_IDLE4, &CDevTabCameraLight::OnBnClickedBtnAreaCamIdle4)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_IDLE5, &CDevTabCameraLight::OnBnClickedBtnAreaCamIdle5)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_IDLE6, &CDevTabCameraLight::OnBnClickedBtnAreaCamIdle6)
	ON_BN_CLICKED(IDC_BTN_RESET, &CDevTabCameraLight::OnBnClickedBtnResetGraphic)

	ON_CBN_SELCHANGE(IDC_CB_LCTRL_PORT_LIGHT1, &CDevTabCameraLight::OnCbnSelchangeCbLctrlPort)
	ON_CBN_SELCHANGE(IDC_CB_LCTRL_PORT_LIGHT2, &CDevTabCameraLight::OnCbnSelchangeCbLctrlPort2)
	ON_CBN_SELCHANGE(IDC_CB_LCTRL_PORT_LIGHT3, &CDevTabCameraLight::OnCbnSelchangeCbLctrlPort3)

	ON_EN_CHANGE(IDC_EDIT_CH1_LIGHT1, &CDevTabCameraLight::OnEnChangeEditCh1Light1Val)
	ON_EN_CHANGE(IDC_EDIT_CH2_LIGHT1, &CDevTabCameraLight::OnEnChangeEditCh2Light1Val)
	ON_EN_CHANGE(IDC_EDIT_CH3_LIGHT1, &CDevTabCameraLight::OnEnChangeEditCh3Light1Val)
	ON_EN_CHANGE(IDC_EDIT_CH4_LIGHT1, &CDevTabCameraLight::OnEnChangeEditCh4Light1Val)

	ON_EN_CHANGE(IDC_EDIT_CH1_LIGHT2, &CDevTabCameraLight::OnEnChangeEditCh1Light2Val)
	ON_EN_CHANGE(IDC_EDIT_CH2_LIGHT2, &CDevTabCameraLight::OnEnChangeEditCh2Light2Val)
	ON_EN_CHANGE(IDC_EDIT_CH3_LIGHT2, &CDevTabCameraLight::OnEnChangeEditCh3Light2Val)
	ON_EN_CHANGE(IDC_EDIT_CH4_LIGHT2, &CDevTabCameraLight::OnEnChangeEditCh4Light2Val)

	ON_EN_CHANGE(IDC_EDIT_CH1_LIGHT3, &CDevTabCameraLight::OnEnChangeEditCh1Light3Val)
	ON_EN_CHANGE(IDC_EDIT_CH2_LIGHT3, &CDevTabCameraLight::OnEnChangeEditCh2Light3Val)
	ON_EN_CHANGE(IDC_EDIT_CH3_LIGHT3, &CDevTabCameraLight::OnEnChangeEditCh3Light3Val)
	ON_EN_CHANGE(IDC_EDIT_CH4_LIGHT3, &CDevTabCameraLight::OnEnChangeEditCh4Light3Val)

	ON_BN_CLICKED(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1, &CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight1)
	ON_BN_CLICKED(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2, &CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight2)
	ON_BN_CLICKED(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3, &CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight3)

	ON_BN_CLICKED(IDC_BTN_ONOFF_CH1_LIGHT1, &CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light1)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH2_LIGHT1, &CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light1)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH3_LIGHT1, &CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light1)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH4_LIGHT1, &CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light1)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH1_LIGHT2, &CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light2)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH2_LIGHT2, &CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light2)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH3_LIGHT2, &CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light2)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH4_LIGHT2, &CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light2)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH1_LIGHT3, &CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light3)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH2_LIGHT3, &CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light3)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH3_LIGHT3, &CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light3)
	ON_BN_CLICKED(IDC_BTN_ONOFF_CH4_LIGHT3, &CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light3)

	ON_BN_CLICKED(IDC_BTN_AREA_CAM_GRAB,	&CDevTabCameraLight::OnBnClickedBtnAreaCamGrab)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_GRAB2,	&CDevTabCameraLight::OnBnClickedBtnAreaCamGrab2)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_GRAB3,	&CDevTabCameraLight::OnBnClickedBtnAreaCamGrab3)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_GRAB4,	&CDevTabCameraLight::OnBnClickedBtnAreaCamGrab4)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_GRAB5,	&CDevTabCameraLight::OnBnClickedBtnAreaCamGrab5)
	ON_BN_CLICKED(IDC_BTN_AREA_CAM_GRAB6,	&CDevTabCameraLight::OnBnClickedBtnAreaCamGrab6)

	ON_WM_VSCROLL()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CDevTabCameraLight 메시지 처리기입니다.
BOOL CDevTabCameraLight::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	int nFontHeight = 12;
	m_wndLabelTitleAreaCamera.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelTitleAreaCamera.SetFontHeight(nFontHeight);

	m_wndLabelTitleLight1.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelTitleLight1.SetFontHeight(nFontHeight);
	m_wndLabelTitleLight2.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelTitleLight2.SetFontHeight(nFontHeight);
	m_wndLabelTitleLight3.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelTitleLight3.SetFontHeight(nFontHeight);

	DWORD dwLabelItemBodyColor = pRes->GetSideBarItemBackgroundColor();
	DWORD dwLabelItemTextColor = pRes->GetSideBarItemForegroundColor();

	m_btnAreaIdle.SetCheck(BST_CHECKED);
	m_btnAreaIdle2.SetCheck(BST_CHECKED);
	m_btnAreaIdle3.SetCheck(BST_CHECKED);
	m_btnAreaIdle4.SetCheck(BST_CHECKED);
	m_btnAreaIdle5.SetCheck(BST_CHECKED);
	m_btnAreaIdle6.SetCheck(BST_CHECKED);

//	SetScrollRange(SB_VERT, 0, 300); // 범위 설정
	CRect clientRect;
	GetClientRect(&clientRect);

	int dialogHeight = clientRect.Height();
	int dialogWidth = clientRect.Width();

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

	// 설정 변경
	si.nMin = 0;
	si.nMax = dialogHeight;
	//si.nPage = GetRoundShort(dialogHeight/1.5);
	si.nPage = GetRoundShort(dialogHeight/1.2);
	si.nPos = 0;

	SetScrollInfo(SB_VERT, &si, TRUE);  // 수직 스크롤 설정
	SetScrollPos(SB_VERT, 0);        // 초기 스크롤 위치 설정

	UpdateUI();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CDevTabCameraLight::IsModified()
{
	UpdateData(TRUE);

	return TRUE;
}

BOOL CDevTabCameraLight::Save()
{
	UpdateData(TRUE);

	CSystemConfig::LightControllerLFine& LightCtrlAreaScanConfig = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();
	CLightInfo::LightControllerLFine& LightCtrlAreaScanConfig2 = CLightInfo::Instance()->GetLightCtrlAreaScanConfig();
	CSystemConfig::Cam3DScaleValue& Cam3DScaleValue = CSystemConfig::Instance()->GetCam3DScaleValue();
	Cam3DScaleValue = m_CamScaleValue;

	for (int i = 0; i < LedCtrlTypeAreaMax; ++i)
	{
		LightCtrlAreaScanConfig.nPort[i] = m_nAreaLightCtrlPort[i];

		LightCtrlAreaScanConfig2.cLightValues[USE_LFINE_LCP100_CTRL_CH1][i] = (BYTE)m_nAreaLight[USE_LFINE_LCP100_CTRL_CH1][i];
		LightCtrlAreaScanConfig2.cLightValues[USE_LFINE_LCP100_CTRL_CH2][i] = (BYTE)m_nAreaLight[USE_LFINE_LCP100_CTRL_CH2][i];
		LightCtrlAreaScanConfig2.cLightValues[USE_LFINE_LCP100_CTRL_CH3][i] = (BYTE)m_nAreaLight[USE_LFINE_LCP100_CTRL_CH3][i];
		LightCtrlAreaScanConfig2.cLightValues[USE_LFINE_LCP100_CTRL_CH4][i] = (BYTE)m_nAreaLight[USE_LFINE_LCP100_CTRL_CH4][i];
	}

	CSystemConfig::Instance()->Save();

	return TRUE;
}

void CDevTabCameraLight::UpdateUI()
{
	BOOL bEnableCtrl = TRUE;
#ifdef RELEASE_1G
	// Line Scan Live & Idle 비활성화
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE6)->EnableWindow(!bEnableCtrl);
	
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB4)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB5)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB6)->EnableWindow(bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_IDLE)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE6)->EnableWindow(!bEnableCtrl);


	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_LOW)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_HIGH)->EnableWindow(bEnableCtrl);

#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_GRAB)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_IDLE)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_HIGH)->EnableWindow(!bEnableCtrl);

#elif RELEASE_3G
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_GRAB)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_IDLE)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_HIGH)->EnableWindow(!bEnableCtrl);

#elif RELEASE_5G
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_GRAB)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_IDLE)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_HIGH)->EnableWindow(!bEnableCtrl);
	
#elif RELEASE_SG
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_LIVE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_GRAB)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_GRAB6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_BTN_AREA_CAM_IDLE)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE4)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE5)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_AREA_CAM_IDLE6)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_LOW)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_HIGH)->EnableWindow(!bEnableCtrl);
	
#endif

#ifdef RELEASE_SG
	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT1)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT1)->EnableWindow(!bEnableCtrl);

#else
	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT1)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT1)->EnableWindow(bEnableCtrl);
#endif

	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT2)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT2)->EnableWindow(!bEnableCtrl);

	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT3)->EnableWindow(!bEnableCtrl);

#if defined(RELEASE_2G) || defined(RELEASE_3G)
	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT2)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT2)->EnableWindow(bEnableCtrl);

	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT3)->EnableWindow(!bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT3)->EnableWindow(!bEnableCtrl);

#if defined(RELEASE_3G)
	GetDlgItem(IDC_CB_LCTRL_PORT_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH1_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH2_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH3_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_CH4_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH1_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH2_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH3_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_BTN_ONOFF_CH4_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH1_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH2_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH3_LIGHT3)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_SLIDER_CNTRL_CH4_LIGHT3)->EnableWindow(bEnableCtrl);
#endif
#endif
}

void CDevTabCameraLight::Refresh()
{
	CSystemConfig::LightControllerLFine& LCtrlAreaCfg = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();
	CLightInfo::LightControllerLFine& LightCtrlAreaScanConfig = CLightInfo::Instance()->GetLightCtrlAreaScanConfig();

	CSystemConfig::Cam3DScaleValue& Cam3DScaleValue = CSystemConfig::Instance()->GetCam3DScaleValue();
	m_CamScaleValue = Cam3DScaleValue;

	{
		int nCurSel = m_wndCbLCtrlPortArea[LedCtrlTypeArea1].GetCurSel();
		if (nCurSel < 0)
			return;

		UINT nPort = (UINT)m_wndCbLCtrlPortArea[LedCtrlTypeArea1].GetItemData(nCurSel);
		if (nPort != LCtrlAreaCfg.nPort[LedCtrlTypeArea1])			GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(TRUE);
		else														GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(FALSE);
	}

	{
		int nCurSel = m_wndCbLCtrlPortArea[LedCtrlTypeArea2].GetCurSel();
		if (nCurSel < 0)
			return;

		UINT nPort = (UINT)m_wndCbLCtrlPortArea[LedCtrlTypeArea2].GetItemData(nCurSel);
		if (nPort != LCtrlAreaCfg.nPort[LedCtrlTypeArea2])			GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(TRUE);
		else														GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(FALSE);
	}

	{
		int nCurSel = m_wndCbLCtrlPortArea[LedCtrlTypeArea3].GetCurSel();
		if (nCurSel < 0)
			return;

		UINT nPort = (UINT)m_wndCbLCtrlPortArea[LedCtrlTypeArea3].GetItemData(nCurSel);
		if (nPort != LCtrlAreaCfg.nPort[LedCtrlTypeArea3])			GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(TRUE);
		else														GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(FALSE);
	}

	for (int i = 0; i < LedCtrlTypeAreaMax; ++i)
	{
		m_nAreaLightCtrlPort[i] = LCtrlAreaCfg.nPort[i];

		for (int j = 0; j < USE_LFINE_LCP100_CTRL_CH_MAX; ++j)
		{
			m_nAreaLight[j][i] = (int)LightCtrlAreaScanConfig.cLightValues[j][i];
			m_wndSliderLight[j][i].SetPos(LightCtrlAreaScanConfig.cLightValues[j][i]);
		}
	}

	// Programmer 권한일 때만 3D Scale 설정 가능
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
		EnableCameraScale(TRUE);
	else
		EnableCameraScale(FALSE);
#endif
	UpdateData(FALSE);
}

void CDevTabCameraLight::Cleanup()
{
#ifdef RELEASE_1G
	OnBnClickedBtnAreaCamIdle();
	OnBnClickedBtnAreaCamIdle2();
	OnBnClickedBtnAreaCamIdle3();
	OnBnClickedBtnAreaCamIdle4();
	OnBnClickedBtnAreaCamIdle5();
	OnBnClickedBtnAreaCamIdle6();

#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	OnBnClickedBtnAreaCamIdle();
	OnBnClickedBtnAreaCamIdle2();

#elif RELEASE_3G
	OnBnClickedBtnAreaCamIdle();
	OnBnClickedBtnAreaCamIdle2();
	OnBnClickedBtnAreaCamIdle3();
#elif RELEASE_SG
	OnBnClickedBtnAreaCamIdle();
#endif

	// Light
#ifdef RELEASE_1G
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);

#elif RELEASE_2G
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);

	SetLightAllOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2);

#elif RELEASE_3G
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);

	SetLightAllOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH4);

	SetLightAllOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH4);

#elif RELEASE_4G
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);

#elif RELEASE_5G
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);

#elif RELEASE_6G
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
	SetLightAllOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);

#endif

	UpdateUI();
}

HBRUSH CDevTabCameraLight::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
	}

	return hbr;
}

BOOL CDevTabCameraLight::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	return TRUE;
}

void CDevTabCameraLight::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		CUIntArray ports;
		EnumerateSerialPorts(ports);

		CString strPort;
		UINT nPort;
		CString strV;

		// AreaScan Light Ctrl Port Reset
		CSystemConfig::LightControllerLFine& LedCtrlAreaCfg = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();
		CLightInfo::LightControllerLFine& LedCtrlAreaCfg2 = CLightInfo::Instance()->GetLightCtrlAreaScanConfig();

		// Set ComboBox
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(FALSE);

		for (int i = 0; i < LedCtrlTypeAreaMax; ++i)
		{
			m_wndCbLCtrlPortArea[i].ResetContent();

			int nSelectedIndex = 0;
			m_wndCbLCtrlPortArea[i].AddString(_T("None"));
			m_wndCbLCtrlPortArea[i].SetItemData(0, (DWORD_PTR)DEF_PORT_NONE);

			//////////////////////////////////////////////////////////////////////////
			// Test
			// 		m_wndCbLCtrlPortArea.AddString( _T("Com 3") );
			// 		m_wndCbLCtrlPortArea.SetItemData( 1, (DWORD_PTR)3 );
			//////////////////////////////////////////////////////////////////////////

			for (INT_PTR nI = 0; nI < ports.GetCount(); nI++)
			{
				nPort = ports.GetAt(nI);
				strPort.Format(_T("COM %d"), nPort);
				int nIdx = m_wndCbLCtrlPortArea[i].AddString(strPort);
				if (nPort == LedCtrlAreaCfg.nPort[i])
				{
					nSelectedIndex = nIdx;
				}
				m_wndCbLCtrlPortArea[i].SetItemData(nIdx, (DWORD_PTR)nPort);
			}
			m_wndCbLCtrlPortArea[i].SetCurSel(nSelectedIndex);
		}

		for (int i = 0; i < LedCtrlTypeAreaMax; ++i)
		{
			for (int j = 0; j < USE_LFINE_LCP100_CTRL_CH_MAX; ++j)
			{
				// Set Slider
				m_wndSliderLight[j][i].SetRange(0, DEF_LFINE_CTRL_MAX);
				m_wndSliderLight[j][i].SetPos(LedCtrlAreaCfg2.cLightValues[j][i]);

				// Set Edit
				strV.Format(_T("%d"), LedCtrlAreaCfg2.cLightValues[j][i]);
				GetDlgItem(IDC_EDIT_CH1_LIGHT1)->SetWindowText(strV);
				m_nAreaLight[j][i] = (int)LedCtrlAreaCfg2.cLightValues[j][i];
			}
		}

		UpdateUI();
	}
	else
	{
		Cleanup();
	}
}

void CDevTabCameraLight::OnBnClickedBtnResetGraphic()
{
	m_pMainView->ResetGraphic();
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamLive()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamLive"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	if (!FrameGrabber.IsOpen(CameraTypeLine))
	{
		m_btnAreaLive.SetCheck(BST_UNCHECKED);
		return;
	}

#else
	if (!FrameGrabber.IsOpen(CameraTypeArea))
	{
		m_btnAreaLive.SetCheck(BST_UNCHECKED);
		return;
	}

#endif

	m_btnAreaLive.SetCheck(BST_CHECKED);

	m_btnAreaIdle.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle.EnableWindow(TRUE);

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA1], nAREA_CAM_SIZE_Y[IDX_AREA1], IDX_AREA1, TRUE);
	FrameGrabber.Live(CameraTypeLine);

#else
	FrameGrabber.Live(CameraTypeArea);

#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamLive2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamLive2"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraType3DArea))
	{
		m_btnAreaLive2.SetCheck(BST_UNCHECKED);
		return;
	}
#elif RELEASE_2G || RELEASE_3G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	if (!FrameGrabber.IsOpen(CameraTypeArea2))
	{
		m_btnAreaLive2.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive2.SetCheck(BST_CHECKED);

	m_btnAreaIdle2.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle2.EnableWindow(TRUE);

#ifdef RELEASE_1G
	FrameGrabber.Live(CameraType3DArea);
#elif RELEASE_2G || RELEASE_3G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	FrameGrabber.Live(CameraTypeArea2);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamLive3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamLive3"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraTypeArea))
	{
		m_btnAreaLive3.SetCheck(BST_UNCHECKED);
		return;
	}
#elif RELEASE_3G
	if (!FrameGrabber.IsOpen(CameraTypeArea3))
	{
		m_btnAreaLive3.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive3.SetCheck(BST_CHECKED);

	m_btnAreaIdle3.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle3.EnableWindow(TRUE);

#ifdef RELEASE_1G
	FrameGrabber.Live(CameraTypeArea);
#elif RELEASE_3G
	FrameGrabber.Live(CameraTypeArea3);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamLive4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamLive4"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#if defined(RELEASE_1G)
	if (!FrameGrabber.IsOpen(CameraTypeLine2))
	{
		m_btnAreaLive4.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive4.SetCheck(BST_CHECKED);

	m_btnAreaIdle4.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle4.EnableWindow(TRUE);

#if defined(RELEASE_1G)
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA5], nAREA_CAM_SIZE_Y[IDX_AREA5], IDX_AREA5, TRUE);
	FrameGrabber.Live(CameraTypeLine2);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamLive5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamLive5"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#if defined(RELEASE_1G)
	if (!FrameGrabber.IsOpen(CameraTypeLine3))
	{
		m_btnAreaLive5.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive5.SetCheck(BST_CHECKED);

	m_btnAreaIdle5.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle5.EnableWindow(TRUE);

#if defined(RELEASE_1G)
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA6], nAREA_CAM_SIZE_Y[IDX_AREA6], IDX_AREA6, TRUE);
	FrameGrabber.Live(CameraTypeLine3);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamLive6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamLive6"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#if defined(RELEASE_1G)
	if (!FrameGrabber.IsOpen(CameraTypeLine4))
	{
		m_btnAreaLive6.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive6.SetCheck(BST_CHECKED);

	m_btnAreaIdle6.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle6.EnableWindow(TRUE);

#if defined(RELEASE_1G)
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA7], nAREA_CAM_SIZE_Y[IDX_AREA7], IDX_AREA7, TRUE);
	FrameGrabber.Live(CameraTypeLine4);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamGrab()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamGrab"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	if (!FrameGrabber.IsOpen(CameraTypeLine) || FrameGrabber.IsLive(CameraTypeLine)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab.SetCheck(BST_UNCHECKED);
		return;
	}

#else
	if (!FrameGrabber.IsOpen(CameraTypeArea) ) // || FrameGrabber.IsLive(CameraTypeArea))
	{
		m_btnAreaLive.SetCheck(BST_UNCHECKED);
		return;
	}

#endif

	m_btnAreaLive.SetCheck(BST_UNCHECKED);
	m_btnAreaGrab.SetCheck(BST_CHECKED);
	m_btnAreaIdle.SetCheck(BST_UNCHECKED);

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA1], nAREA_CAM_SIZE_Y[IDX_AREA1], IDX_AREA1, TRUE);
	FrameGrabber.Grab(CameraTypeLine);

#else
	FrameGrabber.Grab(CameraTypeArea);

#endif

	m_btnAreaGrab.SetCheck(BST_UNCHECKED);
#if !defined(RELEASE_1G) && !defined(RELEASE_SG)
	m_btnAreaIdle.SetCheck(BST_CHECKED);
	m_btnAreaIdle.EnableWindow(TRUE);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamGrab2()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamGrab2"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraType3DArea) || FrameGrabber.IsLive(CameraType3DArea))  // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab2.SetCheck(BST_UNCHECKED);
		return;
	}
#elif RELEASE_2G || RELEASE_3G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	if (!FrameGrabber.IsOpen(CameraTypeArea2) ) // || FrameGrabber.IsLive(CameraTypeArea2))
	{
		m_btnAreaGrab2.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive2.SetCheck(BST_UNCHECKED);
	m_btnAreaGrab2.SetCheck(BST_CHECKED);
	m_btnAreaIdle2.SetCheck(BST_UNCHECKED);

#if defined(RELEASE_1G)
	FrameGrabber.Grab(CameraType3DArea);
#elif RELEASE_2G || RELEASE_3G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	FrameGrabber.Grab(CameraTypeArea2);
#endif

	m_btnAreaGrab2.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle2.SetCheck(BST_CHECKED);
	m_btnAreaIdle2.EnableWindow(TRUE);
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamGrab3()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamGrab3"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraTypeArea) || FrameGrabber.IsLive(CameraTypeArea)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab3.SetCheck(BST_UNCHECKED);
		return;
	}
#elif RELEASE_3G
	if (!FrameGrabber.IsOpen(CameraTypeArea3) ) // || FrameGrabber.IsLive(CameraTypeArea3))
	{
		m_btnAreaGrab3.SetCheck(BST_UNCHECKED);
		return;
	}
#endif

	m_btnAreaLive3.SetCheck(BST_UNCHECKED);
	m_btnAreaGrab3.SetCheck(BST_CHECKED);
	m_btnAreaIdle3.SetCheck(BST_UNCHECKED);

#if defined(RELEASE_1G)
	FrameGrabber.Grab(CameraTypeArea);
#elif RELEASE_3G
	FrameGrabber.Grab(CameraTypeArea3);
#endif
	m_btnAreaGrab3.SetCheck(BST_UNCHECKED);
	m_btnAreaIdle3.SetCheck(BST_CHECKED);
	m_btnAreaIdle3.EnableWindow(TRUE);
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamGrab4()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamGrab4"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraTypeLine2) || FrameGrabber.IsLive(CameraTypeLine2)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab4.SetCheck(BST_UNCHECKED);
		return;
	}
	m_btnAreaGrab4.SetCheck(BST_CHECKED);
	m_btnAreaIdle4.SetCheck(BST_UNCHECKED);

	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA5], nAREA_CAM_SIZE_Y[IDX_AREA5], IDX_AREA5, TRUE);
	FrameGrabber.Grab(CameraTypeLine2);
#endif
	m_btnAreaGrab4.SetCheck(BST_UNCHECKED);
#ifndef RELEASE_1G
	m_btnAreaIdle4.SetCheck(BST_CHECKED);
	m_btnAreaIdle4.EnableWindow(TRUE);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamGrab5()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamGrab5"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraTypeLine3) || FrameGrabber.IsLive(CameraTypeLine3)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab5.SetCheck(BST_UNCHECKED);
		return;
	}
	if (!FrameGrabber.IsOpen(CameraTypeLine4) || FrameGrabber.IsLive(CameraTypeLine4)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab6.SetCheck(BST_UNCHECKED);
		return;
	}
	m_btnAreaGrab5.SetCheck(BST_CHECKED);
	m_btnAreaIdle5.SetCheck(BST_UNCHECKED);

	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA6], nAREA_CAM_SIZE_Y[IDX_AREA6], IDX_AREA6, TRUE);
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA7], nAREA_CAM_SIZE_Y[IDX_AREA7], IDX_AREA7, TRUE);
	FrameGrabber.Grab(CameraTypeLine3);
	FrameGrabber.Grab(CameraTypeLine4);
#endif
	m_btnAreaGrab5.SetCheck(BST_UNCHECKED);
#ifndef RELEASE_1G
	m_btnAreaIdle5.SetCheck(BST_CHECKED);
	m_btnAreaIdle5.EnableWindow(TRUE);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamGrab6()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamGrab6"));

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#ifdef RELEASE_1G
	if (!FrameGrabber.IsOpen(CameraTypeLine3) || FrameGrabber.IsLive(CameraTypeLine3)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab5.SetCheck(BST_UNCHECKED);
		return;
	}
	if (!FrameGrabber.IsOpen(CameraTypeLine4) || FrameGrabber.IsLive(CameraTypeLine4)) // Live 중에 Grab 버튼 누르면 동작 안되도록 추가
	{
		m_btnAreaGrab6.SetCheck(BST_UNCHECKED);
		return;
	}
	m_btnAreaGrab6.SetCheck(BST_CHECKED);
	m_btnAreaIdle6.SetCheck(BST_UNCHECKED);

	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA6], nAREA_CAM_SIZE_Y[IDX_AREA6], IDX_AREA6, TRUE);
	CVisionSystem::Instance()->CreateImageGrabBuffers_Line(nAREA_CAM_SIZE_X[IDX_AREA7], nAREA_CAM_SIZE_Y[IDX_AREA7], IDX_AREA7, TRUE);
	FrameGrabber.Grab(CameraTypeLine3);
	FrameGrabber.Grab(CameraTypeLine4);
#endif
	m_btnAreaGrab6.SetCheck(BST_UNCHECKED);
#ifndef RELEASE_1G
	m_btnAreaIdle6.SetCheck(BST_CHECKED);
	m_btnAreaIdle6.EnableWindow(TRUE);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamIdle()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamIdle"));

	m_btnAreaLive.SetCheck(BST_UNCHECKED);

	m_btnAreaIdle.SetCheck(BST_CHECKED);
	m_btnAreaIdle.EnableWindow(TRUE);

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	FrameGrabber.Idle(CameraTypeLine);

#else
	FrameGrabber.Idle(CameraTypeArea);

#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamIdle2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamIdle2"));

	m_btnAreaLive2.SetCheck(BST_UNCHECKED);

	m_btnAreaIdle2.SetCheck(BST_CHECKED);
	m_btnAreaIdle2.EnableWindow(TRUE);

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#ifdef RELEASE_1G
	FrameGrabber.Idle(CameraType3DArea);
#elif RELEASE_2G || RELEASE_3G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	FrameGrabber.Idle(CameraTypeArea2);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamIdle3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamIdle3"));

	m_btnAreaLive3.SetCheck(BST_UNCHECKED);

	m_btnAreaIdle3.SetCheck(BST_CHECKED);
	m_btnAreaIdle3.EnableWindow(TRUE);

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#ifdef RELEASE_1G
	FrameGrabber.Idle(CameraTypeArea);
#elif RELEASE_3G
	FrameGrabber.Idle(CameraTypeArea3);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamIdle4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamIdle4"));

	m_btnAreaLive4.SetCheck(BST_UNCHECKED);

	m_btnAreaIdle4.SetCheck(BST_CHECKED);
	m_btnAreaIdle4.EnableWindow(TRUE);

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#if defined(RELEASE_1G)
	FrameGrabber.Idle(CameraTypeLine2);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamIdle5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamIdle5"));

	m_btnAreaLive5.SetCheck(BST_UNCHECKED);

	m_btnAreaIdle5.SetCheck(BST_CHECKED);
	m_btnAreaIdle5.EnableWindow(TRUE);

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#if defined(RELEASE_1G)
	FrameGrabber.Idle(CameraTypeLine3);
#endif
}

void CDevTabCameraLight::OnBnClickedBtnAreaCamIdle6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnAreaCamIdle6"));

	m_btnAreaLive6.SetCheck(BST_UNCHECKED);

	m_btnAreaIdle6.SetCheck(BST_CHECKED);
	m_btnAreaIdle6.EnableWindow(TRUE);

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
#if defined(RELEASE_1G)
	FrameGrabber.Idle(CameraTypeLine4);
#endif
}

void CDevTabCameraLight::SetBtnApplyLctrlPort(short nCtrlId, LedCtrlType ledctrltype)
{
	UpdateData(TRUE);

	int nCurSel = m_wndCbLCtrlPortArea[ledctrltype].GetCurSel();
	if (nCurSel < 0)
		return;
	UINT nPort = (UINT)m_wndCbLCtrlPortArea[ledctrltype].GetItemData(nCurSel);

	VisionInterface::LightController & LCtrl = CVisionSystem::Instance()->GetLightControllerInterface();
	CSystemConfig::LightControllerLFine& LCtrlConfig = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();
	CVisionSystem* pSystem = CVisionSystem::Instance();

	//포트변경
	UINT nOldPort = LCtrlConfig.nPort[ledctrltype];
	LCtrlConfig.nPort[ledctrltype] = nPort;

	if (nPort != DEF_PORT_NONE)
	{
		if (!LCtrl.ChangePort(ledctrltype, nPort))
		{
			LCtrlConfig.nPort[ledctrltype] = nOldPort;
			LCtrl.ChangePort(ledctrltype, nOldPort);

			int nIndex = 0;
			for (int nI = 0; nI < m_wndCbLCtrlPortArea[ledctrltype].GetCount(); nI++)
			{
				if (nOldPort == m_wndCbLCtrlPortArea[ledctrltype].GetItemData(nI))
				{
					nIndex = nI;
					break;
				}
			}
			m_wndCbLCtrlPortArea[ledctrltype].SetCurSel(nIndex);
		}
	}
	else
	{
		LCtrl.ClosePort(ledctrltype);
	}

	m_nAreaLightCtrlPort[ledctrltype] = LCtrlConfig.nPort[ledctrltype];

	GetDlgItem(nCtrlId)->EnableWindow(FALSE);
}

void CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight1()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight1"));
	SetBtnApplyLctrlPort(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1, LedCtrlTypeArea1);
}

void CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight2()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight2"));
	SetBtnApplyLctrlPort(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2, LedCtrlTypeArea2);
}

void CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight3()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnApplyLctrlPortLight3"));
	SetBtnApplyLctrlPort(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3, LedCtrlTypeArea3);
}

void CDevTabCameraLight::OnCbnSelchangeCbLctrlPort()
{
	int nCurSel = m_wndCbLCtrlPortArea[LedCtrlTypeArea1].GetCurSel();
	if (nCurSel < 0)
		return;
	UINT nPort = (UINT)m_wndCbLCtrlPortArea[LedCtrlTypeArea1].GetItemData(nCurSel);
	CSystemConfig::LightControllerLFine& LCtrlAreaCfg = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();

	if (nPort != LCtrlAreaCfg.nPort[LedCtrlTypeArea1])
	{
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT1)->EnableWindow(FALSE);
	}
}

void CDevTabCameraLight::OnCbnSelchangeCbLctrlPort2()
{
	int nCurSel = m_wndCbLCtrlPortArea[LedCtrlTypeArea2].GetCurSel();
	if (nCurSel < 0)
		return;
	UINT nPort = (UINT)m_wndCbLCtrlPortArea[LedCtrlTypeArea2].GetItemData(nCurSel);
	CSystemConfig::LightControllerLFine& LCtrlAreaCfg = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();

	if (nPort != LCtrlAreaCfg.nPort[LedCtrlTypeArea2])
	{
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT2)->EnableWindow(FALSE);
	}
}

void CDevTabCameraLight::OnCbnSelchangeCbLctrlPort3()
{
	int nCurSel = m_wndCbLCtrlPortArea[LedCtrlTypeArea3].GetCurSel();
	if (nCurSel < 0)
		return;
	UINT nPort = (UINT)m_wndCbLCtrlPortArea[LedCtrlTypeArea3].GetItemData(nCurSel);
	CSystemConfig::LightControllerLFine& LCtrlAreaCfg = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();

	if (nPort != LCtrlAreaCfg.nPort[LedCtrlTypeArea3])
	{
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_APPLY_LCTRL_PORT_LIGHT3)->EnableWindow(FALSE);
	}
}

void CDevTabCameraLight::OnEnChangeEditCh1Light1Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH1_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
}

void CDevTabCameraLight::OnEnChangeEditCh2Light1Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH2_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
}

void CDevTabCameraLight::OnEnChangeEditCh3Light1Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH3_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
}

void CDevTabCameraLight::OnEnChangeEditCh4Light1Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH4_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);
}

void CDevTabCameraLight::OnEnChangeEditCh1Light2Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH1_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1);
}

void CDevTabCameraLight::OnEnChangeEditCh2Light2Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH2_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2);
}

void CDevTabCameraLight::OnEnChangeEditCh3Light2Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH3_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH3);
}

void CDevTabCameraLight::OnEnChangeEditCh4Light2Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH4_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH4);
}

void CDevTabCameraLight::OnEnChangeEditCh1Light3Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH1_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH1);
}

void CDevTabCameraLight::OnEnChangeEditCh2Light3Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH2_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH2);
}

void CDevTabCameraLight::OnEnChangeEditCh3Light3Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH3_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH3);
}

void CDevTabCameraLight::OnEnChangeEditCh4Light3Val()
{
	EditLightVal(IDC_BTN_ONOFF_CH4_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH4);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light1()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light1"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH1_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light1()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light1"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH2_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light1()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light1"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH3_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light1()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light1"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH4_LIGHT1, LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light2()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light2"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH1_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light2()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light2"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH2_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light2()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light2"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH3_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH3);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light2()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light2"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH4_LIGHT2, LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH4);
}

void CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light3()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh1Light3"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH1_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH1);
}


void CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light3()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh2Light3"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH2_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH2);
}


void CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light3()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh3Light3"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH3_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH3);
}


void CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light3()
{
	WRITE_LOG(WL_BTN, _T("CDevTabCameraLight::OnBnClickedBtnOnoffCh4Light3"));
	SetBtnLightOnOff(IDC_BTN_ONOFF_CH4_LIGHT3, LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH4);
}


void CDevTabCameraLight::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar)
	{
		int nMinPos, nMaxPos;
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
		pSlider->GetRange(nMinPos, nMaxPos);
		int nPos = pSlider->GetPos();

		VisionInterface::LightController& LightController = CVisionSystem::Instance()->GetLightControllerInterface();

		CString strV;
		switch ((short)pSlider->GetDlgCtrlID())
		{
		case IDC_SLIDER_CNTRL_CH1_LIGHT1:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH1_LIGHT1)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH2_LIGHT1:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH2_LIGHT1)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH3_LIGHT1:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH3_LIGHT1)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH4_LIGHT1:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH4_LIGHT1)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH1_LIGHT2:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH1_LIGHT2)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH2_LIGHT2:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH2_LIGHT2)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH3_LIGHT2:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH3_LIGHT2)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH4_LIGHT2:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH4_LIGHT2)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH1_LIGHT3:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH1_LIGHT3)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH2_LIGHT3:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH2_LIGHT3)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH3_LIGHT3:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH3_LIGHT3)->SetWindowText(strV);
			break;

		case IDC_SLIDER_CNTRL_CH4_LIGHT3:
			strV.Format(_T("%d"), nPos);
			GetDlgItem(IDC_EDIT_CH4_LIGHT3)->SetWindowText(strV);
			break;

			UpdateData(FALSE);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDevTabCameraLight::EditLightVal(short nCtrlId, LedCtrlType ledctrltype, eUSE_LFINE_LCP100_CTRL_CH_MAX ch)
{
	UpdateData(TRUE);

	m_wndSliderLight[ch][ledctrltype].SetPos(m_nAreaLight[ch][ledctrltype]);
	VisionInterface::LightController& LightController = CVisionSystem::Instance()->GetLightControllerInterface();

	CString strText(_T(""));
	GetDlgItemText(nCtrlId, strText);

	if (strText == _T("Off"))
	{
		LightController.TurnOn(ledctrltype, ch, m_nAreaLight[ch][ledctrltype]);
	}

	UpdateData(FALSE);
}

void CDevTabCameraLight::SetBtnLightOnOff(short nCtrlId, LedCtrlType ledctrltype, eUSE_LFINE_LCP100_CTRL_CH_MAX ch)
{
	VisionInterface::LightController& LightController = CVisionSystem::Instance()->GetLightControllerInterface();
	if (!LightController.IsConnected(ledctrltype))
	{
		return;
	}

	UpdateData(TRUE);

	CString strText;
	GetDlgItemText(nCtrlId, strText);

	BOOL bOn = FALSE;
	if (strText == _T("On"))
		bOn = TRUE;

	if (bOn)
	{
		SetDlgItemTextW(nCtrlId, _T("Off"));
		m_btnOnoffLight[ch][ledctrltype].SetCheck(BST_CHECKED);
		LightController.TurnOn(ledctrltype, ch, m_nAreaLight[ch][ledctrltype]);
	}
	else
	{
		SetDlgItemTextW(nCtrlId, _T("On"));
		m_btnOnoffLight[ch][ledctrltype].SetCheck(BST_UNCHECKED);
		LightController.TurnOff(ledctrltype, ch);
	}
}

void CDevTabCameraLight::SetLightAllOff(LedCtrlType ledctrltype, eUSE_LFINE_LCP100_CTRL_CH_MAX ch)
{
	VisionInterface::LightController& LightController = CVisionSystem::Instance()->GetLightControllerInterface();
	if (!LightController.IsConnected(ledctrltype))
	{
		return;
	}

	LightController.TurnOff(ledctrltype, ch);
}

void CDevTabCameraLight::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int scrollMin, scrollMax;
	GetScrollRange(SB_VERT, &scrollMin, &scrollMax);
	
	// Dialog Box 높이보다 더 아래로 내려가는 버그 수정
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	GetScrollInfo(SB_VERT, &si);

	int maxScrollPos = scrollMax - (int)si.nPage + 1;
	maxScrollPos = max(scrollMin, maxScrollPos); // 음수 방지

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nScrollPos = max(scrollMin, m_nScrollPos - 100);
		break;

	case SB_LINEDOWN:
		m_nScrollPos = min(maxScrollPos, m_nScrollPos + 100);
		break;

	case SB_PAGEUP:
		m_nScrollPos = max(scrollMin, m_nScrollPos - 300);
		break;

	case SB_PAGEDOWN:
		m_nScrollPos = min(maxScrollPos, m_nScrollPos + 300);
		break;

	case SB_THUMBTRACK:
		m_nScrollPos = min(maxScrollPos, (int)nPos);
		break;

	default:
		break;
	}

	int delta = m_nScrollPos - GetScrollPos(SB_VERT); // 스크롤 변화량 계산
	SetScrollPos(SB_VERT, m_nScrollPos);             // 스크롤 위치 갱신
	ScrollWindow(0, -delta);                         // 변화량만큼 화면 이동
	//Invalidate();                                    // 화면 갱신

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CDevTabCameraLight::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	 // zDelta: 휠 회전값, scrollAmount: 휠로 이동할 픽셀 수
    int scrollAmount = -zDelta / WHEEL_DELTA * 20; // 한 번의 휠 이동량 (조정 가능)

    // 현재 스크롤 위치 가져오기
    int scrollPos = GetScrollPos(SB_VERT);

    // 수직 스크롤 범위 정보 가져오기
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE;
    GetScrollInfo(SB_VERT, &si);

    // 새로운 스크롤 위치 계산
    int newScrollPos = scrollPos + scrollAmount;

    // 범위 제한 적용
    newScrollPos = max(si.nMin, min(newScrollPos, si.nMax - (int)si.nPage + 1));

    // 스크롤 위치가 변경된 경우에만 업데이트
    if (newScrollPos != scrollPos)
    {
        SetScrollPos(SB_VERT, newScrollPos);
        ScrollWindow(0, scrollPos - newScrollPos); // 컨텐츠 스크롤
    }

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CDevTabCameraLight::EnableCameraScale(BOOL bEnableCtrl)
{
#if defined(RELEASE_1G)
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_LOW	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_LOW	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_LOW	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_MIXING_HIGH	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTLEFT_HIGH	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_LIFTRIGHT_HIGH)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_LOW			)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_AREA_SCALE_VAL_HIGH			)->EnableWindow(bEnableCtrl);
#elif defined(RELEASE_SG)
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_LOW	)->EnableWindow(bEnableCtrl);
	GetDlgItem(IDC_EDIT_3D_LINE_SCALE_VAL_TRAYOCR_HIGH	)->EnableWindow(bEnableCtrl);
#endif
}

BOOL CDevTabCameraLight::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(msg);
}