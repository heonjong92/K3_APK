// TeachTab1GLift.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab1GLift.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "StringMacro.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>

// CTeachTab1GLift 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab1GLift, CDialog)

CTeachTab1GLift::CTeachTab1GLift(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab1GLift::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
{
	m_pMainView = NULL;
	m_Lift.Clear();
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab1GLift::~CTeachTab1GLift()
{
}

void CTeachTab1GLift::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_1G_LIFT,				m_wndLabelTitle1GLift);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE_LIFT,			m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_LIFT,					m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_LIFT,					m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_LIFT_BYPASS,					m_Lift.nUseBypass_Lift);
	DDX_Text(pDX,		IDC_EDIT_LIFT_DESCRIPTION,				m_Lift.strDescription);
	DDX_Text(pDX,		IDC_EDIT_LIFTINFO_PAGECOUNT,			m_Lift.lPageCount);
	DDX_Text(pDX,		IDC_EDIT_LIFT_TRAY_PIXEL_THRESHOLD,		m_Lift.nTrayPixelThreshold);
	DDX_Text(pDX,		IDC_EDIT_LIFT_HEIGHT_MM,				m_Lift.dbLiftHeight_MM);
	DDX_Text(pDX,		IDC_EDIT_BREAK_WIDTH_MM,				m_Lift.dbBreakWidth_MM);
	DDX_Text(pDX,		IDC_EDIT_BREAK_HEIGHT_MM,				m_Lift.dbBreakHeight_MM);
	DDX_Text(pDX,		IDC_EDIT_LIFT_HEIGHT_PX,				m_Lift.nLiftHeight_PX);
	DDX_Text(pDX,		IDC_EDIT_BREAK_WIDTH_PX,				m_Lift.nBreakWidth_PX);
	DDX_Text(pDX,		IDC_EDIT_BREAK_HEIGHT_PX,				m_Lift.nBreakHeight_PX);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_L2,						m_Lift.nLeftOffset_Front);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_L,						m_Lift.nLeftOffset);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_R,						m_Lift.nRightOffset);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_T,						m_Lift.nTopOffset);
	DDX_Text(pDX, 		IDC_EDIT_OFFSET_B,						m_Lift.nBottomOffset);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH1,					m_Lift.nWidthFirst);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH2,					m_Lift.nWidthSecond);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH3,					m_Lift.nWidthThird);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH4,					m_Lift.nWidthFourth);

	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH_REAR1,				m_Lift.nRearWidthFirst);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH_REAR2,				m_Lift.nRearWidthSecond);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH_REAR3,				m_Lift.nRearWidthThird);
	DDX_Text(pDX,		IDC_EDIT_ROI_WIDTH_REAR4,				m_Lift.nRearWidthFourth);

	DDX_Control(pDX,	IDC_BTN_TEST_LIFT,						m_btnTest_Lift);
	DDX_Control(pDX,	IDC_BTN_LIFT_RECIPE_NO,					m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_LIFT,						m_btnSave);

	DDV_MinMaxInt(pDX,	m_Lift.nTrayPixelThreshold,				GV8_MIN, GV8_MAX);

	DDX_Radio(pDX,		IDC_RADIO_SELECT_UNIT,					m_Lift.nSelectUnit);

	DDX_Text(pDX,		IDC_EDIT_LIFT_FRONT_NONINSP_START,		m_Lift.nFrontNonInspStart);
	DDX_Text(pDX,		IDC_EDIT_LIFT_FRONT_NONINSP_WIDTH,		m_Lift.nFrontNonInspWidth);
}


BEGIN_MESSAGE_MAP(CTeachTab1GLift, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_LIFT_BYPASS,		&CTeachTab1GLift::OnBnClickedRadioLiftBypass)
	ON_BN_CLICKED(IDC_RADIO_LIFT_BYPASS2,		&CTeachTab1GLift::OnBnClickedRadioLiftBypass)
	ON_BN_CLICKED(IDC_RADIO_LIFT_BYPASS3,		&CTeachTab1GLift::OnBnClickedRadioLiftBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_LIFT,			&CTeachTab1GLift::OnBnClickedBtnTestLift)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_LIFT,		&CTeachTab1GLift::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_LIFT,			&CTeachTab1GLift::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_LIFT_RECIPE_NO,		&CTeachTab1GLift::OnBnClickedBtnLiftRecipeNo)
	ON_BN_CLICKED(IDC_RADIO_SELECT_UNIT,		&CTeachTab1GLift::OnBnClickedRadioSelectUnit)
	ON_BN_CLICKED(IDC_RADIO_SELECT_UNIT2,		&CTeachTab1GLift::OnBnClickedRadioSelectUnit)
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTab1GLift 메시지 처리기입니다.
BOOL CTeachTab1GLift::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1GLift.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle1GLift.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_Lift.SetColor( DEF_BTN_COLOR_MEASURE );

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab1GLift::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_1G_LIFT					)->SetWindowText(CLanguageInfo::Instance()->ReadString(30));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE_LIFT				)->SetWindowText(CLanguageInfo::Instance()->ReadString(31));
	GetDlgItem(IDC_BTN_LIFT_RECIPE_NO					)->SetWindowText(CLanguageInfo::Instance()->ReadString(32));
	GetDlgItem(IDC_RADIO_LIFT_BYPASS					)->SetWindowText(CLanguageInfo::Instance()->ReadString(33));
	GetDlgItem(IDC_RADIO_LIFT_BYPASS2					)->SetWindowText(CLanguageInfo::Instance()->ReadString(34));
	GetDlgItem(IDC_RADIO_LIFT_BYPASS3					)->SetWindowText(CLanguageInfo::Instance()->ReadString(35));
	GetDlgItem(IDC_STATIC_LIFT_DESCRIPTION				)->SetWindowText(CLanguageInfo::Instance()->ReadString(36));
	GetDlgItem(IDC_STATIC_LIFT_PAGE_COUNT				)->SetWindowText(CLanguageInfo::Instance()->ReadString(37));
	GetDlgItem(IDC_STATIC_LIFT_HEIGHT					)->SetWindowText(CLanguageInfo::Instance()->ReadString(38));
	GetDlgItem(IDC_BTN_TEST_LIFT						)->SetWindowText(CLanguageInfo::Instance()->ReadString(39));
	GetDlgItem(IDC_STATIC_BREAK_WIDTH_HEIGHT			)->SetWindowText(CLanguageInfo::Instance()->ReadString(40));
	GetDlgItem(IDC_STATIC_LIFT_TRAY_PIXEL_THRESHOLD		)->SetWindowText(CLanguageInfo::Instance()->ReadString(41));
	GetDlgItem(IDC_STATIC_LIFT_INSPECTION_ROI_WIDTH		)->SetWindowText(CLanguageInfo::Instance()->ReadString(42));
	GetDlgItem(IDC_STATIC_OFFSET						)->SetWindowText(CLanguageInfo::Instance()->ReadString(43));
	GetDlgItem(IDC_STATIC_OFFSET2						)->SetWindowText(CLanguageInfo::Instance()->ReadString(66));
	GetDlgItem(IDC_STATIC_LIFT_INSPECTION_ROI_WIDTH_REAR)->SetWindowText(CLanguageInfo::Instance()->ReadString(44));
	GetDlgItem(IDC_STATIC_LIFT_FRONT_NONINSP			)->SetWindowText(CLanguageInfo::Instance()->ReadString(29));

	CModelInfo::stLiftInfo& stLiftInfo = CModelInfo::Instance()->GetLiftInfo();

	//언어
	if (m_Lift.nSelectUnit== 0) // px
	{
		GetDlgItem(IDC_STATIC_LIFT_HEIGHT)->SetWindowText(CLanguageInfo::Instance()->ReadString(38));
		GetDlgItem(IDC_STATIC_BREAK_WIDTH_HEIGHT)->SetWindowText(CLanguageInfo::Instance()->ReadString(40));
	}
	else
	{
		GetDlgItem(IDC_STATIC_LIFT_HEIGHT)->SetWindowText(CLanguageInfo::Instance()->ReadString(48));
		GetDlgItem(IDC_STATIC_BREAK_WIDTH_HEIGHT)->SetWindowText(CLanguageInfo::Instance()->ReadString(49));
	}

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1GLift.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab1GLift::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LIFTINFO_PAGECOUNT), _T("Line Scan Camera로 읽는 범위이며, 이미지의 폭(Width)이 됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LIFT_TRAY_PIXEL_THRESHOLD), _T("Tray의 픽셀 값을 입력합니다.입력된 Gray Value이상인 Blob 을 Tray로 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LIFT_HEIGHT_MM), _T("[MM] 입력한 값 이상의 들뜸이 감지 된 경우에 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LIFT_HEIGHT_PX), _T("[PX] 입력한 값 이상의 들뜸이 감지 된 경우에 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BREAK_WIDTH), _T("입력한 값 이상의 깨짐이 감지 된 경우에 NG가 발생합니다. &(and)연산으로, 폭 또는 높이 중 하나의 값이 해당 값 미만일 경우 NG가 발생하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BREAK_HEIGHT), _T("입력한 값 이상의 깨짐이 감지 된 경우에 NG가 발생합니다. &(and)연산으로, 폭 또는 높이 중 하나의 값이 해당 값 미만일 경우 NG가 발생하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_L2), _T("Front Image, Left 영역에서만 적용 됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_L), _T("Tray 전체 영역에서 Edge Left 부분을 보정합니다. ex)최 상단 영역은 Tray 옆 면이 아닌 Tray Pocket의 요철 부분이므로 검사영역에서 제외합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_R), _T("Tray 전체 영역에서 Edge Right 부분을 보정합니다. ex)최 상단 영역은 Tray 옆 면이 아닌 Tray Pocket의 요철 부분이므로 검사영역에서 제외합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_T), _T("Tray 전체 영역에서 Edge Top 부분을 보정합니다. ex)최 상단 영역은 Tray 옆 면이 아닌 Tray Pocket의 요철 부분이므로 검사영역에서 제외합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OFFSET_B), _T("Tray 전체 영역에서 Edge Bottom 부분을 보정합니다. ex)최 상단 영역은 Tray 옆 면이 아닌 Tray Pocket의 요철 부분이므로 검사영역에서 제외합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH1), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Front Image에서 Tray 좌측 끝에서 좌측 홈까지 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH2), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Front Image에서 Tray 좌측 홈의 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH3), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Front Image에서 Tray 좌측 홈에서 우측 홈까지 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH4), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Front Image에서 Tray 우측 홈에서 우측 끝까지 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH_REAR1), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Rear Image에서 Tray 좌측 끝에서 좌측 홈까지 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH_REAR2), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Rear Image에서 Tray 좌측 홈의 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH_REAR3), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Rear Image에서 Tray 좌측 홈에서 우측 홈까지 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_ROI_WIDTH_REAR4), _T("Tray내에서 각 검사영역의 폭을 지정합니다. Rear Image에서 Tray 우측 홈에서 우측 끝까지 거리"));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_SELECT_UNIT), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_SELECT_UNIT2), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab1GLift::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab1GLift::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stLiftInfo& LiftInfo = CModelInfo::Instance()->GetLiftInfo();
	LiftInfo = m_Lift;
	
#ifdef RELEASE_1G
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_LIFTINFO );
#endif

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab1GLift::CheckData()
{
#ifdef RELEASE_1G
	CString strLog;
	CModelInfo::stLiftInfo& LiftInfo = CModelInfo::Instance()->GetLiftInfo();

	// ----- Lift Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[LiftInfo.nUseBypass_Lift], strBypassName[m_Lift.nUseBypass_Lift] );
	if( LiftInfo.nUseBypass_Lift != m_Lift.nUseBypass_Lift) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeLiftFront, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), LiftInfo.strDescription, m_Lift.strDescription);
	if (LiftInfo.strDescription != m_Lift.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);
	
	strLog.Format( _T("[PageCount][%ld→%ld]"), LiftInfo.lPageCount, m_Lift.lPageCount );
	if (LiftInfo.lPageCount != m_Lift.lPageCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog );

	strLog.Format(_T("[TrayPixelThreshold][%u→%u]"), LiftInfo.nTrayPixelThreshold, m_Lift.nTrayPixelThreshold);
	if (LiftInfo.nTrayPixelThreshold != m_Lift.nTrayPixelThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[LOffset_Front][%d→%d]"), LiftInfo.nLeftOffset_Front, m_Lift.nLeftOffset_Front);
	if (LiftInfo.nLeftOffset_Front != m_Lift.nLeftOffset_Front)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);
	strLog.Format(_T("[LOffset][%d→%d]"), LiftInfo.nLeftOffset, m_Lift.nLeftOffset);
	if (LiftInfo.nLeftOffset != m_Lift.nLeftOffset)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);
	strLog.Format(_T("[ROffset][%d→%d]"), LiftInfo.nRightOffset, m_Lift.nRightOffset);
	if (LiftInfo.nRightOffset != m_Lift.nRightOffset)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);	
	strLog.Format(_T("[TOffset][%d→%d]"), LiftInfo.nTopOffset, m_Lift.nTopOffset);
	if (LiftInfo.nTopOffset != m_Lift.nTopOffset)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);
	strLog.Format(_T("[BOffset][%d→%d]"), LiftInfo.nBottomOffset, m_Lift.nBottomOffset);
	if (LiftInfo.nBottomOffset != m_Lift.nBottomOffset)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[LiftHeight_MM][%.4lf→%.4lf]"), LiftInfo.dbLiftHeight_MM, m_Lift.dbLiftHeight_MM);
	if (LiftInfo.dbLiftHeight_MM != m_Lift.dbLiftHeight_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[BreakWidth_MM][%.4lf→%.4lf]"), LiftInfo.dbBreakWidth_MM, m_Lift.dbBreakWidth_MM);
	if (LiftInfo.dbBreakWidth_MM != m_Lift.dbBreakWidth_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[BreakHeight_MM][%.4lf→%.4lf]"), LiftInfo.dbBreakHeight_MM, m_Lift.dbBreakHeight_MM);
	if (LiftInfo.dbBreakHeight_MM != m_Lift.dbBreakHeight_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[LiftHeight_PX][%d→%d]"), LiftInfo.nLiftHeight_PX, m_Lift.nLiftHeight_PX);
	if (LiftInfo.nLiftHeight_PX != m_Lift.nLiftHeight_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[BreakWidth_PX][%d→%d]"), LiftInfo.nBreakWidth_PX, m_Lift.nBreakWidth_PX);
	if (LiftInfo.nBreakWidth_PX != m_Lift.nBreakWidth_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[BreakHeight_PX][%d→%d]"), LiftInfo.nBreakHeight_PX, m_Lift.nBreakHeight_PX);
	if (LiftInfo.nBreakHeight_PX != m_Lift.nBreakHeight_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[FrontRoiWidthFirst][%d→%d]"), LiftInfo.nWidthFirst, m_Lift.nWidthFirst);
	if (LiftInfo.nWidthFirst != m_Lift.nWidthFirst) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[FrontRoiWidthSecond][%d→%d]"), LiftInfo.nWidthSecond, m_Lift.nWidthSecond);
	if (LiftInfo.nWidthSecond != m_Lift.nWidthSecond) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[FrontRoiWidthThird][%d→%d]"), LiftInfo.nWidthThird, m_Lift.nWidthThird);
	if (LiftInfo.nWidthThird != m_Lift.nWidthThird) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[FrontRoiWidthFourth][%d→%d]"), LiftInfo.nWidthFourth, m_Lift.nWidthFourth);
	if (LiftInfo.nWidthFourth != m_Lift.nWidthFourth) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[RearRoiWidthFirst][%d→%d]"), LiftInfo.nRearWidthFirst, m_Lift.nRearWidthFirst);
	if (LiftInfo.nRearWidthFirst != m_Lift.nRearWidthFirst) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[RearRoiWidthSecond][%d→%d]"), LiftInfo.nRearWidthSecond, m_Lift.nRearWidthSecond);
	if (LiftInfo.nRearWidthSecond != m_Lift.nRearWidthSecond) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[RearRoiWidthThird][%d→%d]"), LiftInfo.nRearWidthThird, m_Lift.nRearWidthThird);
	if (LiftInfo.nRearWidthThird != m_Lift.nRearWidthThird) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[RearRoiWidthFourth][%d→%d]"), LiftInfo.nRearWidthFourth, m_Lift.nRearWidthFourth);
	if (LiftInfo.nRearWidthFourth != m_Lift.nRearWidthFourth) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);

	strLog.Format(_T("[nFrontNonInspStart][%d→%d]"), LiftInfo.nFrontNonInspStart, m_Lift.nFrontNonInspStart);
	if (LiftInfo.nFrontNonInspStart != m_Lift.nFrontNonInspStart) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);
	strLog.Format(_T("[nFrontNonInspWidth][%d→%d]"), LiftInfo.nFrontNonInspWidth, m_Lift.nFrontNonInspWidth);
	if (LiftInfo.nFrontNonInspWidth != m_Lift.nFrontNonInspWidth) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLiftFront, strLog);
#endif
}

void CTeachTab1GLift::Refresh()
{
	UpdateRecipeList();
	CModelInfo::stLiftInfo& LiftInfo = CModelInfo::Instance()->GetLiftInfo();
	m_Lift = LiftInfo;

	BOOL bPx = (LiftInfo.nSelectUnit == 0) ? TRUE : FALSE;
	GetDlgItem(IDC_EDIT_LIFT_HEIGHT_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_BREAK_WIDTH_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_BREAK_HEIGHT_PX)->EnableWindow(bPx);

	GetDlgItem(IDC_EDIT_LIFT_HEIGHT_MM)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_BREAK_WIDTH_MM)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_BREAK_HEIGHT_MM)->EnableWindow(!bPx);

	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_EDIT_LIFTINFO_PAGECOUNT)->EnableWindow(TRUE);

		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(TRUE);

		GetDlgItem(IDC_EDIT_LIFT_HEIGHT_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_BREAK_WIDTH_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_BREAK_HEIGHT_PX)->ShowWindow(bPx);

		GetDlgItem(IDC_EDIT_LIFT_HEIGHT_MM)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_BREAK_WIDTH_MM)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_BREAK_HEIGHT_MM)->ShowWindow(!bPx);
	}
	else
	{
		GetDlgItem(IDC_EDIT_LIFTINFO_PAGECOUNT)->EnableWindow(FALSE);

		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(FALSE);
	}
	UpdateLanguage();
	UpdateData(FALSE);
}

void CTeachTab1GLift::Default()
{
	m_Lift.Clear();

	UpdateData(FALSE);
}

void CTeachTab1GLift::Cleanup()
{
	GetDlgItem(IDC_BTN_TEST_LIFT)->EnableWindow(TRUE);
}

HBRUSH CTeachTab1GLift::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_LIFT_DESCRIPTION:
	case IDC_STATIC_LIFT_PAGE_COUNT:
	case IDC_STATIC_LIFT_TRAY_PIXEL_THRESHOLD:
	case IDC_STATIC_LIFT_HEIGHT:
	case IDC_STATIC_BREAK_WIDTH_HEIGHT:
	case IDC_STATIC_LIFT_INSPECTION_ROI_WIDTH:
	case IDC_STATIC_FIRST:
	case IDC_STATIC_SECOND:
	case IDC_STATIC_THIRD:
	case IDC_STATIC_FOURTH:
	case IDC_STATIC_OFFSET:
	case IDC_STATIC_OFFSET2:
	case IDC_STATIC_LIFT_INSPECTION_ROI_WIDTH_REAR:
	case IDC_STATIC_SELECT_UNIT:
	case IDC_STATIC_LIFT_FRONT_NONINSP:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab1GLift::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab1GLift::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab1GLift::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab1GLift::OnBnClickedRadioSelectUnit()
{
	UpdateData(TRUE);
}

void CTeachTab1GLift::OnBnClickedRadioLiftBypass()
{
	UpdateData(TRUE);
}

void CTeachTab1GLift::OnBnClickedBtnTestLift()
{
#ifdef RELEASE_1G
	WRITE_LOG( WL_BTN, _T("CTeachTab1GLift::OnBnClickedBtnTestLift") );

	for (int nIndex = IDX_AREA6; nIndex < IDX_AREA_MAX; nIndex++)
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameLift();
		if (strModelName == _T("CALIBRATION"))
			CVisionSystem::Instance()->m_bAcm[ACM_LIFT] = TRUE;

		CVisionSystem::Instance()->BeginStopWatch(nIndex);
		CVisionSystem::Instance()->StartInspection( (nIndex ==IDX_AREA6) ? InspectTypeLiftFront : InspectTypeLiftRear, nIndex, 0, TRUE);
	}
#endif
}

void CTeachTab1GLift::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(LIFT_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_LIFTINFO);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameLift();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameLift();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameLift();
		CString strSelectModelName;
		m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
		if (strSelectModelName == strModelName)
		{
			m_btnSave.SetWindowText(_T("Save"));
			DisableWnd(TRUE);
			Refresh();
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

void CTeachTab1GLift::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_Lift\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameLift();
	int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
	m_wndSelectRecipe.SetCurSel(nIndex);
	m_nSelectRecipeIndex = nIndex;

	if (m_nSelectRecipeIndex < 0)
	{
		// Disable ALL
		m_btnSave.EnableWindow(FALSE);
	}
	else
	{
		m_btnSave.SetWindowText(_T("Save"));

		m_btnSave.EnableWindow(TRUE);
	}
}


void CTeachTab1GLift::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GLift::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameLift();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		CModelInfo::Instance()->Load(strSelectModelName, LIFT_KIND);
//		if (CModelInfo::Instance()->Load(strSelectModelName, LIFT_KIND) )
//		{
//			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
//			pInspectionVision->Load(strSelectModelName);
//		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();
	Refresh();

#ifdef RELEASE_1G
	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
	FrameGrabber.SetPageCount(CameraTypeLine3);
	FrameGrabber.SetPageCount(CameraTypeLine4);
#endif

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab1GLift::OnBnClickedBtnSave :: End"));
}

void CTeachTab1GLift::DisableWnd(BOOL bEnable)
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

void CTeachTab1GLift::OnBnClickedBtnLiftRecipeNo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GLift::OnBnClickedBtnLiftRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GLift::OnBnClickedBtnLiftRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( LIFT_KIND, TEACH_TAB_IDX_LIFTINFO);
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 1G LIFT") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GLift::OnBnClickedBtnLiftRecipeNo :: End"));
#endif
}

void CTeachTab1GLift::EnableLift()
{
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(FALSE);
	}
}

void CTeachTab1GLift::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_1G_LIFT);

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
				strPdfPath.Format(_T("%s\\Manual\\HA-1154 Manual_260304.pdf"), GetExecuteDirectory());

				// 1. 역슬래시(\)를 슬래시(/)로 변환 (브라우저 인식용)
				strPdfPath.Replace(_T('\\'), _T('/'));

				// 2. file:/// 프로토콜을 사용하고 전체를 큰따옴표로 감싸기
				// # 문자가 인코딩되지 않도록 직접 구성
				CString strParams;
				strParams.Format(_T("\"file:///%s#page=38\""), (LPCTSTR)strPdfPath);

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
