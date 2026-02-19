// TeachTab3GDesiccantCutting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab3GDesiccantCutting.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "InspectionVision.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "StringMacro.h"

#include "UIExt/ResourceManager.h"

#include <XUtil/xUtils.h>
#include <XGraphic\xGraphicObject.h>

// CTeachTab3GDesiccantCutting 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab3GDesiccantCutting, CDialog)

BOOL CTeachTab3GDesiccantCutting::m_bPreviewLeft = FALSE;
BOOL CTeachTab3GDesiccantCutting::m_bPreviewRight = FALSE;

CTeachTab3GDesiccantCutting::CTeachTab3GDesiccantCutting(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab3GDesiccantCutting::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsCheckedLeftROI(FALSE)
	, m_bIsCheckedRightROI(FALSE)
	, m_bIsPreviewLeft(FALSE)
	, m_bIsPositionLeftROI(FALSE)
	, m_bIsPositionRightROI(FALSE)
	, m_nScrollPos(0)
{
	m_pMainView = NULL;
	m_DesiccantCuttingInfo.Clear();
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab3GDesiccantCutting::~CTeachTab3GDesiccantCutting()
{
}

void CTeachTab3GDesiccantCutting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_3G,								m_wndLabelTitle3G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,							m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_DESICCANT_CUTTING,				m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_DESICCANT_CUTTING,				m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_DESICCANT_POSITION_BYPASS,			m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft);
	DDX_Radio(pDX,		IDC_RADIO_DESICCANT_POSITION2_BYPASS,			m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight);
	DDX_Radio(pDX,		IDC_RADIO_DESICCANT_CUTTING_BYPASS,				m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft );
	DDX_Radio(pDX,		IDC_RADIO_DESICCANT_CUTTING2_BYPASS,			m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight );
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_CUTTING_DESCRIPTION,			m_DesiccantCuttingInfo.strDescription);

	DDX_Text(pDX, 		IDC_EDIT_DESICCANT_CUTTING_PIXEL_VALUE,			m_DesiccantCuttingInfo.nDesiccantCuttingPixelValue);
	DDX_Text(pDX, 		IDC_EDIT_CUTTING_SEALING_LENGTH,				m_DesiccantCuttingInfo.dSealingLength);
	DDX_Text(pDX, 		IDC_EDIT_COLOR_LINE_LENGTH_DIFFERENCE,			m_DesiccantCuttingInfo.dColorLineLengthDifference);

	DDX_Text(pDX,		IDC_EDIT_DESICCANT_CUTTING_LED_LIGHT_VALUE,		m_DesiccantCuttingInfo.nValueCh1);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_CUTTING_LED_LIGHT_VALUE2,	m_DesiccantCuttingInfo.nValueCh2);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_CUTTING_LED_LIGHT_VALUE3,	m_DesiccantCuttingInfo.nValueCh3);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_CUTTING_LED_LIGHT_VALUE4,	m_DesiccantCuttingInfo.nValueCh4);

	// Desiccant Position
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH,								m_DesiccantCuttingInfo.nROIWidth);
	DDX_Text(pDX,		IDC_EDIT_ROI_HEIGHT,							m_DesiccantCuttingInfo.nROIHeight);
	DDX_Text(pDX,		IDC_EDIT_COLOR_LINE_THRESHOLD,					m_DesiccantCuttingInfo.nColorLineThreshold);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_PIXEL_VALUE,					m_DesiccantCuttingInfo.nDesiccantPixelValue);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_WIDTH,						m_DesiccantCuttingInfo.dDesiccantWidth);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_HEIGHT,						m_DesiccantCuttingInfo.dDesiccantHeight);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_LEFT, 							m_DesiccantCuttingInfo.nRoiOffsetLeft);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_RIGHT, 							m_DesiccantCuttingInfo.nRoiOffsetRight);
	DDX_Text(pDX, 		IDC_EDIT_ERODE_WIDTH, 							m_DesiccantCuttingInfo.nOpenBoxWidth);
	DDX_Text(pDX, 		IDC_EDIT_ERODE_HEIGHT, 							m_DesiccantCuttingInfo.nOpenBoxHeight);
	DDX_Text(pDX,		IDC_EDIT_POSITION_COLOR_LINE_LENGTH_DIFFERENCE, m_DesiccantCuttingInfo.dPosColorLineLengthDiff);

	DDX_Control(pDX,	IDC_BTN_TEST_DESICCANT_POSITION,				m_btnTest_DesiccantPosition);
	DDX_Control(pDX,	IDC_BTN_TEST_DESICCANT_POSITION2,				m_btnTest_DesiccantPosition2);
	DDX_Control( pDX,	IDC_BTN_TEST_DESICCANT_CUTTING,					m_btnTest_DesiccantCutting);
	DDX_Control( pDX,	IDC_BTN_TEST_DESICCANT_CUTTING2,				m_btnTest_DesiccantCutting2);

	DDX_Control(pDX,	IDC_BTN_DESICCANT_CUTTING_RECIPE_NO,			m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_DESICCANT_CUTTING,					m_btnSave);
	
	DDX_Check(pDX, 		IDC_BTN_ROI_DESICCANT_CUTTING_LEFT, 			m_bIsCheckedLeftROI);
	DDX_Check(pDX, 		IDC_BTN_ROI_DESICCANT_CUTTING_RIGHT, 			m_bIsCheckedRightROI);
	DDX_Control(pDX, 	IDC_BTN_ROI_DESICCANT_CUTTING_LEFT, 			m_btnLeftROI);
	DDX_Control(pDX, 	IDC_BTN_ROI_DESICCANT_CUTTING_RIGHT, 			m_btnRightROI);

	DDX_Control(pDX,	IDC_CHECK_VIEW_CONVERTED_IMAGE,					m_btnPreviewLeft);
	DDX_Check(pDX,		IDC_CHECK_VIEW_CONVERTED_IMAGE,					m_bIsPreviewLeft);
	DDX_Control(pDX,	IDC_CHECK_VIEW_CONVERTED_IMAGE2,				m_btnPreviewRight);
	DDX_Check(pDX,		IDC_CHECK_VIEW_CONVERTED_IMAGE2,				m_bIsPreviewRight);

	DDV_MinMaxInt(pDX,	m_DesiccantCuttingInfo.nDesiccantPixelValue,	GV8_MIN, GV8_MAX);
	DDX_Control(pDX,	IDC_BTN_ROI_DESICCANT_POSITION_LEFT,			m_btnPositionLeftROI);
	DDX_Check(pDX,		IDC_BTN_ROI_DESICCANT_POSITION_LEFT,			m_bIsPositionLeftROI);
	DDX_Control(pDX,	IDC_BTN_ROI_DESICCANT_POSITION_RIGHT,			m_btnPositionRightROI);
	DDX_Check(pDX,		IDC_BTN_ROI_DESICCANT_POSITION_RIGHT,			m_bIsPositionRightROI);
}

BEGIN_MESSAGE_MAP(CTeachTab3GDesiccantCutting, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	// Bypass - Desiccant Position1
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_POSITION_BYPASS,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPositionBypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_POSITION_BYPASS2,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPositionBypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_POSITION_BYPASS3,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPositionBypass)

	// Bypass - Desiccant Position2
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_POSITION_BYPASS,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPosition2Bypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_POSITION_BYPASS2,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPosition2Bypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_POSITION_BYPASS3,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPosition2Bypass)

	// Bypass - Desiccant1
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_CUTTING_BYPASS,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCuttingBypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_CUTTING_BYPASS2,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCuttingBypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_CUTTING_BYPASS3,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCuttingBypass)

	// Bypass - Desiccant2
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_CUTTING2_BYPASS,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCutting2Bypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_CUTTING2_BYPASS2,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCutting2Bypass)
	ON_BN_CLICKED(IDC_RADIO_DESICCANT_CUTTING2_BYPASS3,		&CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCutting2Bypass)

	// Inspection
	ON_BN_CLICKED(IDC_BTN_TEST_DESICCANT_POSITION,			&CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantPosition)
	ON_BN_CLICKED(IDC_BTN_TEST_DESICCANT_POSITION2,			&CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantPosition2)
	ON_BN_CLICKED(IDC_BTN_TEST_DESICCANT_CUTTING,			&CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantCutting)
	ON_BN_CLICKED(IDC_BTN_TEST_DESICCANT_CUTTING2,			&CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantCutting2)

	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_DESICCANT_CUTTING,	&CTeachTab3GDesiccantCutting::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_DESICCANT_CUTTING,			&CTeachTab3GDesiccantCutting::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DESICCANT_CUTTING_RECIPE_NO,		&CTeachTab3GDesiccantCutting::OnBnClickedBtnDesiccantCuttingRecipeNo)
	ON_BN_CLICKED(IDC_BTN_ROI_DESICCANT_CUTTING_LEFT,		&CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingLeft)
	ON_BN_CLICKED(IDC_BTN_ROI_DESICCANT_CUTTING_RIGHT,		&CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingRight)
	ON_BN_CLICKED(IDC_CHECK_VIEW_CONVERTED_IMAGE,			&CTeachTab3GDesiccantCutting::OnBnClickedCheckViewConvertedImage)
	ON_BN_CLICKED(IDC_CHECK_VIEW_CONVERTED_IMAGE2,			&CTeachTab3GDesiccantCutting::OnBnClickedCheckViewConvertedImage2)
	ON_BN_CLICKED(IDC_BTN_ROI_DESICCANT_POSITION_LEFT,		&CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantPositionLeft)
	ON_BN_CLICKED(IDC_BTN_ROI_DESICCANT_POSITION_RIGHT,		&CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantPositionRight)

	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab3GDesiccantCutting 메시지 처리기입니다.
BOOL CTeachTab3GDesiccantCutting::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle3G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle3G.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_btnTest_DesiccantPosition.SetColor(DEF_BTN_COLOR_MEASURE);
	m_btnTest_DesiccantPosition2.SetColor(DEF_BTN_COLOR_MEASURE);
	m_btnTest_DesiccantCutting.SetColor( DEF_BTN_COLOR_MEASURE );
	m_btnTest_DesiccantCutting2.SetColor( DEF_BTN_COLOR_MEASURE );
	
	m_wndSelectRecipe.SetFont(&m_fontRecipe);
	
	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);
	
	UpdateLanguage();

	InitScroll();

	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT_PIXEL_VALUE), _T("방습제 픽셀 값을 입력합니다. 입력된 Gray Value 이상인 Blob을 방습제로 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT_WIDTH), _T("방습제와 가로 크기를 입력합니다. 인식된 방습제의 길이가 값 이하이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT_HEIGHT), _T("방습제와 세로 크기를 입력합니다. 인식된 방습제의 길이가 값 이하이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_LEFT), _T("방습제 Blob내에서 좌측 검사 영역 박스의 Offset을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_RIGHT), _T("방습제 Blob내에서 우측 검사 영역 박스의 Offset을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH), _T("검사 영역 박스의 폭을 지정합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_HEIGHT), _T("검사 영역 박스의 높이를 지정합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_COLOR_LINE_THRESHOLD), _T("방습제의 컬러 라인의 Gray Value 값을 입력합니다. 입력된 Gray Value 이하인 Blob을 컬러라인으로 인식합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ERODE_WIDTH), _T("Image Processing Opening Convolution 박스의 폭을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ERODE_HEIGHT), _T("Image Processing Opening Convolution 박스의 높이를 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_POSITION_COLOR_LINE_LENGTH_DIFFERENCE), _T("양쪽 컬러라인의 길이 차이 허용치를 입력합니다. 양쪽 컬러 라인의 길이 차이가 해당 값 이상이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_VIEW_CONVERTED_IMAGE), _T("Opening Convolution 연산으로 변환 된 이미지를 Viewer에서 확인할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_VIEW_CONVERTED_IMAGE2), _T("Opening Convolution 연산으로 변환 된 이미지를 Viewer에서 확인할 수 있습니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_DESICCANT_POSITION_LEFT), _T("좌측 Position 검사 영역을 지정할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_DESICCANT_POSITION_RIGHT), _T("우측 Position 검사 영역을 지정할 수 있습니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT_CUTTING_PIXEL_VALUE), _T("방습제 픽셀 값을 입력합니다. 입력된 Gray Value 이하인 Blob을 방습제로 인식합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CUTTING_SEALING_LENGTH), _T("방습제의 실링부 길이의 허용치를 입력합니다. 길이가 입력한 값 미만이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_COLOR_LINE_LENGTH_DIFFERENCE), _T("양쪽 컬러라인의 길이 차이 허용치를 입력합니다. 양쪽 컬러 라인이 길이 차이가 해당 값 이상이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_DESICCANT_CUTTING_LEFT), _T("좌측 Desiccant 검사 영역을 지정할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_DESICCANT_CUTTING_RIGHT), _T("우측 Desiccant 검사 영역을 지정할 수 있습니다."));
	m_toolTip.Activate(TRUE);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab3GDesiccantCutting::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab3GDesiccantCutting::Save()
{
	UpdateData(TRUE);

	CModelInfo::stDesiccantCuttingInfo& DesiccantCuttingInfo = CModelInfo::Instance()->GetDesiccantCuttingInfo();

	CheckData();

	DesiccantCuttingInfo = m_DesiccantCuttingInfo;

#ifdef RELEASE_3G
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_DESICCANT_CUTTING );
#endif

	Refresh();
	
	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab3GDesiccantCutting::CheckData()
{
#ifdef RELEASE_3G
	CString strLog;
	CModelInfo::stDesiccantCuttingInfo& DesiccantCuttingInfo = CModelInfo::Instance()->GetDesiccantCuttingInfo();

	// ----- Desiccant Teaching -----
	strLog.Format( _T("[Bypass_POS_Left][%s→%s]"), strBypassName[DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft], strBypassName[m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft] );
	if (DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft != m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypePositionLeft, strLog);

	strLog.Format(_T("[Bypass_POS_Right][%s→%s]"), strBypassName[DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight], strBypassName[m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight]);
	if (DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight != m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypePositionRight, strLog);

	strLog.Format( _T("[Bypass_CUT_Left][%s→%s]"), strBypassName[DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft], strBypassName[m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft] );
	if( DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft != m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeDesiccantLeft, strLog );

	strLog.Format( _T("[Bypass_CUT_Right][%s→%s]"), strBypassName[DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight], strBypassName[m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight] );
	if( DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight != m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeDesiccantRight, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), DesiccantCuttingInfo.strDescription, m_DesiccantCuttingInfo.strDescription);
	if (DesiccantCuttingInfo.strDescription != m_DesiccantCuttingInfo.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);

	strLog.Format(_T("[LightValueCh1][%d→%d]"), DesiccantCuttingInfo.nValueCh1, m_DesiccantCuttingInfo.nValueCh1);
	if (DesiccantCuttingInfo.nValueCh1 != m_DesiccantCuttingInfo.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[LightValueCh2][%d→%d]"), DesiccantCuttingInfo.nValueCh2, m_DesiccantCuttingInfo.nValueCh2);
	if (DesiccantCuttingInfo.nValueCh2 != m_DesiccantCuttingInfo.nValueCh2) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[LightValueCh3][%d→%d]"), DesiccantCuttingInfo.nValueCh3, m_DesiccantCuttingInfo.nValueCh3);
	if (DesiccantCuttingInfo.nValueCh3 != m_DesiccantCuttingInfo.nValueCh3) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[LightValueCh4][%d→%d]"), DesiccantCuttingInfo.nValueCh4, m_DesiccantCuttingInfo.nValueCh4);
	if (DesiccantCuttingInfo.nValueCh4 != m_DesiccantCuttingInfo.nValueCh4) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);

	// Desiccant Cutting
	strLog.Format(_T("[DesiccantCuttingPixelThreshold][%d→%d]"), DesiccantCuttingInfo.nDesiccantCuttingPixelValue, m_DesiccantCuttingInfo.nDesiccantCuttingPixelValue);
	if (DesiccantCuttingInfo.nDesiccantCuttingPixelValue != m_DesiccantCuttingInfo.nDesiccantCuttingPixelValue) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[SealingLength][%.1lf→%.1lf]"), DesiccantCuttingInfo.dSealingLength, m_DesiccantCuttingInfo.dSealingLength);
	if (DesiccantCuttingInfo.dSealingLength != m_DesiccantCuttingInfo.dSealingLength) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[ColorLineLengthDifference][%.1lf→%.1lf]"), DesiccantCuttingInfo.dColorLineLengthDifference, m_DesiccantCuttingInfo.dColorLineLengthDifference);
	if (DesiccantCuttingInfo.dColorLineLengthDifference != m_DesiccantCuttingInfo.dColorLineLengthDifference) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	
	// Desiccant Position
	strLog.Format(_T("[ROIWidth][%d→%d]"), DesiccantCuttingInfo.nROIWidth, m_DesiccantCuttingInfo.nROIWidth);
	if (DesiccantCuttingInfo.nROIWidth != m_DesiccantCuttingInfo.nROIWidth) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[ROIHeight][%d→%d]"), DesiccantCuttingInfo.nROIHeight, m_DesiccantCuttingInfo.nROIHeight);
	if (DesiccantCuttingInfo.nROIHeight != m_DesiccantCuttingInfo.nROIHeight) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[DesiccantPositionPixelThreshold][%d→%d]"), DesiccantCuttingInfo.nDesiccantPixelValue, m_DesiccantCuttingInfo.nDesiccantPixelValue);
	if (DesiccantCuttingInfo.nDesiccantPixelValue != m_DesiccantCuttingInfo.nDesiccantPixelValue) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[DesiccantWidth][%.3lf→%.3lf]"), DesiccantCuttingInfo.dDesiccantWidth, m_DesiccantCuttingInfo.dDesiccantWidth);
	if (DesiccantCuttingInfo.dDesiccantWidth != m_DesiccantCuttingInfo.dDesiccantWidth) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[DesiccantHeight][%.3lf→%.3lf]"), DesiccantCuttingInfo.dDesiccantHeight, m_DesiccantCuttingInfo.dDesiccantHeight);
	if (DesiccantCuttingInfo.dDesiccantHeight != m_DesiccantCuttingInfo.dDesiccantHeight) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[PosColorLineLengthDiff][%.3lf→%.3lf]"), DesiccantCuttingInfo.dPosColorLineLengthDiff, m_DesiccantCuttingInfo.dPosColorLineLengthDiff);
	if (DesiccantCuttingInfo.dPosColorLineLengthDiff != m_DesiccantCuttingInfo.dPosColorLineLengthDiff) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[ColorLineValue][%d→%d]"), DesiccantCuttingInfo.nColorLineThreshold, m_DesiccantCuttingInfo.nColorLineThreshold);
	if (DesiccantCuttingInfo.nColorLineThreshold != m_DesiccantCuttingInfo.nColorLineThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);

	strLog.Format(_T("[InspectionBoxOffsetL][%d→%d]"), DesiccantCuttingInfo.nRoiOffsetLeft, m_DesiccantCuttingInfo.nRoiOffsetLeft);
	if (DesiccantCuttingInfo.nRoiOffsetLeft != m_DesiccantCuttingInfo.nRoiOffsetLeft) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[InspectionBoxOffsetR][%d→%d]"), DesiccantCuttingInfo.nRoiOffsetRight, m_DesiccantCuttingInfo.nRoiOffsetRight);
	if (DesiccantCuttingInfo.nRoiOffsetRight != m_DesiccantCuttingInfo.nRoiOffsetRight) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[ErodeConvBoxWidth][%d→%d]"), DesiccantCuttingInfo.nOpenBoxWidth, m_DesiccantCuttingInfo.nOpenBoxWidth);
	if (DesiccantCuttingInfo.nOpenBoxWidth != m_DesiccantCuttingInfo.nOpenBoxWidth) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
	strLog.Format(_T("[ErodeConvBoxHeight][%d→%d]"), DesiccantCuttingInfo.nOpenBoxHeight, m_DesiccantCuttingInfo.nOpenBoxHeight);
	if (DesiccantCuttingInfo.nOpenBoxHeight != m_DesiccantCuttingInfo.nOpenBoxHeight) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeDesiccantLeft, strLog);
#endif
}

void CTeachTab3GDesiccantCutting::UpdateLanguage()
{
	GetDlgItem(IDC_STATIC_COLOR_LINE_LENGTH_DIFFERENCE	)->SetWindowText(CLanguageInfo::Instance()->ReadString(10));
	GetDlgItem(IDC_LABEL_TITLE_3G						)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_DESICCANT_CUTTING_RECIPE_NO		)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_RADIO_DESICCANT_POSITION_BYPASS		)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_DESICCANT_POSITION_BYPASS2		)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_DESICCANT_POSITION_BYPASS3		)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_RADIO_DESICCANT_POSITION2_BYPASS		)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_DESICCANT_POSITION2_BYPASS2	)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_DESICCANT_POSITION2_BYPASS3	)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_RADIO_DESICCANT_CUTTING_BYPASS		)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_DESICCANT_CUTTING_BYPASS2		)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_DESICCANT_CUTTING_BYPASS3		)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_RADIO_DESICCANT_CUTTING2_BYPASS		)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_DESICCANT_CUTTING2_BYPASS2		)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_DESICCANT_CUTTING2_BYPASS3		)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_STATIC_DESCRIPTION					)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_TOP_LIGHT_VALUE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_STATIC_BOTTOM_LIGHT_VALUE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(19));
	GetDlgItem(IDC_BTN_TEST_DESICCANT_POSITION			)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_BTN_TEST_DESICCANT_POSITION2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_BTN_TEST_DESICCANT_CUTTING			)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_BTN_TEST_DESICCANT_CUTTING2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_DESICCANT_CUTTING_PIXEL_VALUE	)->SetWindowText(CLanguageInfo::Instance()->ReadString(22));
	GetDlgItem(IDC_STATIC_CUTTING_SEALING_LENGTH		)->SetWindowText(CLanguageInfo::Instance()->ReadString(23));

	// Desiccant Position
	GetDlgItem(IDC_STATIC_DESICCANT_PIXEL_VALUE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(25));
	GetDlgItem(IDC_STATIC_DESICCANT_WIDTH_HEIGHT		)->SetWindowText(CLanguageInfo::Instance()->ReadString(26));
	GetDlgItem(IDC_STATIC_ROI_GAIN						)->SetWindowText(CLanguageInfo::Instance()->ReadString(27));
	GetDlgItem(IDC_STATIC_ROI_WIDTH						)->SetWindowText(CLanguageInfo::Instance()->ReadString(28));
	GetDlgItem(IDC_CHECK_VIEW_CONVERTED_IMAGE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(29));
	GetDlgItem(IDC_CHECK_VIEW_CONVERTED_IMAGE2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(29));
	GetDlgItem(IDC_STATIC_OFFSET						)->SetWindowText(CLanguageInfo::Instance()->ReadString(24));
	GetDlgItem(IDC_STATIC_ERODE							)->SetWindowText(CLanguageInfo::Instance()->ReadString(30));
	GetDlgItem(IDC_STATIC_POSITION_COLOR_LINE_LENGTH_DIFFERENCE)->SetWindowText(CLanguageInfo::Instance()->ReadString(10));


	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle3G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab3GDesiccantCutting::UpdateUI()
{
#ifdef RELEASE_3G
	CxGraphicObject* pGO;
	pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA1);
	pGO->Reset();
	pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA2);
	pGO->Reset();
	pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA3);
	pGO->Reset();
	pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA4);
	pGO->Reset();
#endif
}

void CTeachTab3GDesiccantCutting::Refresh()
{
	UpdateRecipeList();
	CModelInfo::stDesiccantCuttingInfo& DesiccantCuttingInfo = CModelInfo::Instance()->GetDesiccantCuttingInfo();
	m_DesiccantCuttingInfo = DesiccantCuttingInfo;

	UpdateData(FALSE);
}

void CTeachTab3GDesiccantCutting::Default()
{
	m_DesiccantCuttingInfo.Clear();

	UpdateData(FALSE);
}

void CTeachTab3GDesiccantCutting::Cleanup()
{
#ifdef RELEASE_3G
	m_pMainView->SetTrackerMode(FALSE);

	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
		{
			pChild->EnableWindow(TRUE);
		}
		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	if (m_bPreviewLeft)
	{
		m_bPreviewLeft = FALSE;
		CVisionSystem::Instance()->SetPreviewGainOffset(IDX_AREA1, m_bPreviewLeft);
	}
	else if (m_bPreviewRight)
	{
		m_bPreviewRight = FALSE;
		CVisionSystem::Instance()->SetPreviewGainOffset(IDX_AREA2, m_bPreviewRight);
	}

	m_bIsCheckedLeftROI = FALSE;
	m_bIsCheckedRightROI = FALSE;
	m_bIsPositionLeftROI = FALSE;
	m_bIsPositionRightROI = FALSE;
	m_bIsPreviewLeft = FALSE;
	m_bIsPreviewRight = FALSE;
#endif // RELEASE_3G
}

HBRUSH CTeachTab3GDesiccantCutting::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_TOP_LIGHT_VALUE:
	case IDC_STATIC_BOTTOM_LIGHT_VALUE:
	case IDC_STATIC_DESICCANT_CUTTING_PIXEL_VALUE:
	case IDC_STATIC_CUTTING_SEALING_LENGTH:
	case IDC_STATIC_DESICCANT_PIXEL_VALUE:
	case IDC_STATIC_DESICCANT_WIDTH_HEIGHT:
	case IDC_STATIC_ROI_GAIN:
	case IDC_STATIC_ROI_WIDTH:
	case IDC_CHECK_VIEW_CONVERTED_IMAGE:
	case IDC_CHECK_VIEW_CONVERTED_IMAGE2:
	case IDC_STATIC_COLOR_LINE_LENGTH_DIFFERENCE:
	case IDC_STATIC_ERODE:
	case IDC_STATIC_OFFSET:
	case IDC_STATIC_POSITION_COLOR_LINE_LENGTH_DIFFERENCE:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab3GDesiccantCutting::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab3GDesiccantCutting::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		UpdateRecipeList();
		DisableWnd(TRUE);
//		Refresh();
	}
	else
	{
		Cleanup();
	}
}

BOOL CTeachTab3GDesiccantCutting::PreTranslateMessage(MSG* pMsg)
{
	UIExt::CFilteredEdit* pEdit = NULL;

	CRect rcClient;
	int nDlgCtrlID = ::GetDlgCtrlID( pMsg->hwnd );

	if (m_toolTip.m_hWnd != nullptr)
		m_toolTip.RelayEvent(pMsg);

	pEdit = (UIExt::CFilteredEdit*)GetDlgItem( nDlgCtrlID );

	if ( !pEdit || !pEdit->IsKindOf( RUNTIME_CLASS(UIExt::CFilteredEdit) ) )
	{
		return CDialog::PreTranslateMessage(pMsg);
	}

	if ( pMsg->message == WM_KEYUP )
	{
		int nVirtKey = (int)pMsg->wParam;
		if ( nVirtKey == VK_RETURN )
		{
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPositionBypass()
{
	UpdateData(TRUE);
}

void CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantPosition2Bypass()
{
	UpdateData(TRUE);
}

void CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCuttingBypass()
{
	UpdateData( TRUE );
}

void CTeachTab3GDesiccantCutting::OnBnClickedRadioDesiccantCutting2Bypass()
{
	UpdateData( TRUE );
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantPosition()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantPosition"));

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();
	if (strModelName == _T("CALIBRATION"))
		CVisionSystem::Instance()->m_bAcm[ACM_DESI_32_L] = TRUE;

	CVisionSystem::Instance()->BeginStopWatch(IDX_AREA1);
	CVisionSystem::Instance()->StartInspection(InspectTypePositionLeft, IDX_AREA1, 0, TRUE);
#endif
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantPosition2()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantPosition2"));

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();
	if (strModelName == _T("CALIBRATION"))
		CVisionSystem::Instance()->m_bAcm[ACM_DESI_31_R] = TRUE;

	CVisionSystem::Instance()->BeginStopWatch(IDX_AREA2);
	CVisionSystem::Instance()->StartInspection(InspectTypePositionRight, IDX_AREA2, 0, TRUE);
#endif
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantCutting()
{
#ifdef RELEASE_3G
	WRITE_LOG( WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantCutting") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA3 );
	CVisionSystem::Instance()->StartInspection( InspectTypeDesiccantLeft, IDX_AREA3, 0, TRUE);
#endif
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantCutting2()
{
#ifdef RELEASE_3G
	WRITE_LOG( WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnTestDesiccantCutting2") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA4 );
	CVisionSystem::Instance()->StartInspection( InspectTypeDesiccantRight, IDX_AREA4, 0, TRUE);
#endif
}

void CTeachTab3GDesiccantCutting::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_3G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(DESICCANT_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_DESICCANT_CUTTING);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();
		CString strSelectModelName;
		m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
		if (strSelectModelName == strModelName)
		{
			m_btnSave.SetWindowText(_T("Save"));
			DisableWnd(TRUE);
			m_btnSave.EnableWindow(TRUE);
		}
		else
		{
			m_btnSave.SetWindowText(_T("Apply"));
			DisableWnd(FALSE);
			m_btnSave.EnableWindow(TRUE);
		}
	}
#endif
}

void CTeachTab3GDesiccantCutting::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("3G_Desiccant_Cutting\\");
	CString strFilter = strRecipeRootPath + _T("*.*");

	CFileFind ff;
	BOOL bFind = ff.FindFile(strFilter);
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (ff.IsDots())
			continue;
		if (!ff.IsDirectory())
			continue;
		CString strRecipeName = ff.GetFileName();

		if (IsExistFile((LPCTSTR)(strRecipeRootPath + strRecipeName + _T("\\ModelInfo.ini"))))
		{
			m_wndSelectRecipe.AddString(ff.GetFileTitle());
		}
	}

	ff.Close();

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();
	int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
	m_wndSelectRecipe.SetCurSel(nIndex);
	m_nSelectRecipeIndex = nIndex;

	if (m_nSelectRecipeIndex < 0)
	{
		// Disable ALL
		DisableWnd(FALSE);
		m_btnSave.EnableWindow(FALSE);
	}
	else
	{
		m_btnSave.SetWindowText(_T("Save"));

		DisableWnd(TRUE);
		m_btnSave.EnableWindow(TRUE);
	}
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnSave :: Start"));

	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantCutting();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		CModelInfo::Instance()->Load(strSelectModelName, DESICCANT_KIND);

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnSave :: End"));
}

void CTeachTab3GDesiccantCutting::DisableWnd(BOOL bEnable)
{
	CWnd* pChild = GetWindow(GW_CHILD);
	while (pChild)
	{
		if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatLabel)))
		{
			pChild = pChild->GetWindow(GW_HWNDNEXT);
			continue;
		}

		pChild->EnableWindow(bEnable);

		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}

	m_wndSelectRecipe.EnableWindow(TRUE);
	//m_btnSave.EnableWindow(TRUE);
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnDesiccantCuttingRecipeNo()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnDesiccantCuttingRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnDesiccantCuttingRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( DESICCANT_KIND, TEACH_TAB_IDX_DESICCANT_CUTTING );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 3G DESICCANT CUTTING") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnDesiccantCuttingRecipeNo :: End"));
#endif
}

void APIENTRY CTeachTab3GDesiccantCutting::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab3GDesiccantCutting* pThis = (CTeachTab3GDesiccantCutting*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab3GDesiccantCutting::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
{
	COLORBOX clrBox;

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	CxImageObject* pImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex);
	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, nViewIndex);

	CRect rcTrarack = rcTrackRegion;
	if (rcTrarack.left > rcTrarack.right)
	{
		if (rcTrarack.top > rcTrarack.bottom)
			rcTrackRegion.SetRect(rcTrarack.right, rcTrarack.bottom, rcTrarack.left, rcTrarack.top);
		else
			rcTrackRegion.SetRect(rcTrarack.right, rcTrarack.top, rcTrarack.left, rcTrarack.bottom);
	}
	else
	{
		if (rcTrarack.top > rcTrarack.bottom)
			rcTrackRegion.SetRect(rcTrarack.left, rcTrarack.bottom, rcTrarack.right, rcTrarack.top);
	}

	pGO->Reset();

	if (m_bIsCheckedLeftROI)
	{
		m_DesiccantCuttingInfo.rcLeftInspArea = rcTrackRegion;
		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_DesiccantCuttingInfo.rcLeftInspArea);
		pGO->AddDrawBox(clrBox);
	}
	
	if(m_bIsCheckedRightROI)
	{
		m_DesiccantCuttingInfo.rcRightInspArea = rcTrackRegion;
		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_DesiccantCuttingInfo.rcRightInspArea);
		pGO->AddDrawBox(clrBox);
	}

	if (m_bIsPositionLeftROI)
	{
		m_DesiccantCuttingInfo.rcPositionLeftInspArea = rcTrackRegion;
		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_DesiccantCuttingInfo.rcPositionLeftInspArea);
		pGO->AddDrawBox(clrBox);
	}
	
	if (m_bIsPositionRightROI)
	{
		m_DesiccantCuttingInfo.rcPositionRightInspArea = rcTrackRegion;
		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_DesiccantCuttingInfo.rcPositionRightInspArea);
		pGO->AddDrawBox(clrBox);
	}

	Cleanup();
	UpdateData(FALSE);
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingLeft()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingLeft"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA3);

	if (m_bIsCheckedLeftROI)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnLeftROI.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA3, _OnConfirmTracker, this, m_DesiccantCuttingInfo.rcLeftInspArea);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingRight()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingRight"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_3G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

	if (m_bIsCheckedRightROI)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnRightROI.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA4, _OnConfirmTracker, this, m_DesiccantCuttingInfo.rcRightInspArea);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab3GDesiccantCutting::OnBnClickedCheckViewConvertedImage()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedCheckViewConvertedImage"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	if (m_bIsPreviewLeft)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnPreviewLeft.EnableWindow(TRUE);
		m_bPreviewLeft = TRUE;
		CVisionSystem::Instance()->SetPreviewGainOffset(IDX_AREA1, m_bPreviewLeft);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTab3GDesiccantCutting::OnBnClickedCheckViewConvertedImage2()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedCheckViewConvertedImage2"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	if (m_bIsPreviewRight)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnPreviewRight.EnableWindow(TRUE);
		m_bPreviewRight = TRUE;
		CVisionSystem::Instance()->SetPreviewGainOffset(IDX_AREA2, m_bPreviewRight);
	}
	else
	{
		Cleanup();
	}
#endif // RELEASE_3G
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantPositionLeft()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingLeft"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsPositionLeftROI)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnPositionLeftROI.EnableWindow(TRUE);
		AfxMessageBox(_T("검사 영역을 지정하세요."));
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this, m_DesiccantCuttingInfo.rcPositionLeftInspArea);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantPositionRight()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantCutting::OnBnClickedBtnRoiDesiccantCuttingRight"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

	if (m_bIsPositionRightROI)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnPositionRightROI.EnableWindow(TRUE);
		AfxMessageBox(_T("검사 영역을 지정하세요."));
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA2, _OnConfirmTracker, this, m_DesiccantCuttingInfo.rcPositionRightInspArea);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab3GDesiccantCutting::InitScroll()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	int nDialogHeight = clientRect.Height();
	int nDialogWidth = clientRect.Width();

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

	// 설정 변경
	si.nMin = 0;
	si.nMax = nDialogHeight;
	//si.nPage = GetRoundShort(nDialogHeight / 1.5);
	si.nPage = GetRoundShort(nDialogHeight / 1.2);
	si.nPos = 0;

	SetScrollInfo(SB_VERT, &si, TRUE);  // 수직 스크롤 설정
	SetScrollPos(SB_VERT, 0);        // 초기 스크롤 위치 설정
}

BOOL CTeachTab3GDesiccantCutting::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void CTeachTab3GDesiccantCutting::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int scrollMin, scrollMax;
	GetScrollRange(SB_VERT, &scrollMin, &scrollMax);

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_VERT, &si);

	// 스크롤 가능한 최대 위치 (전체 - 페이지 + 1)
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

void CTeachTab3GDesiccantCutting::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_3G);

	if (pCtrl)
	{
		CRect rc;
		pCtrl->GetWindowRect(&rc);
		ScreenToClient(&rc);

		int nLeftAreaEndX = rc.left + rc.Height(); // 좌측 정사각형
		int nRightAreaEndX = rc.right - (rc.Height() * 3); // 우측 정사각형 * 3

		if (rc.PtInRect(point))
		{
			if (point.x < nLeftAreaEndX) // Left
			{
				CString strPdfPath;
				strPdfPath.Format(_T("%s\\Manual\\HA-1154 Manual_260205.pdf"), GetExecuteDirectory());

				// 1. 역슬래시(\)를 슬래시(/)로 변환 (브라우저 인식용)
				strPdfPath.Replace(_T('\\'), _T('/'));

				// 2. file:/// 프로토콜을 사용하고 전체를 큰따옴표로 감싸기
				// # 문자가 인코딩되지 않도록 직접 구성
				CString strParams;
				strParams.Format(_T("\"file:///%s#page=44\""), (LPCTSTR)strPdfPath);

				// 3. ShellExecute 실행
				HINSTANCE hInst = ShellExecute(NULL, _T("open"), _T("msedge.exe"), strParams, NULL, SW_SHOWNORMAL);

				if ((INT_PTR)hInst <= 32)
					AfxMessageBox(_T("Manual Pdf 파일을 열 수 없습니다."));
			}
			else if (point.x > nRightAreaEndX)	// Right
			{
				CString strPath;
				strPath.Format(_T("%s\\Manual"), GetExecuteDirectory());
				ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}
