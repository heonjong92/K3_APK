// TeachTab1GChip.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab1GChip.h"
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
#include <XGraphic\xGraphicObject.h>

// CTeachTab1GChip 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab1GChip, CDialog)

CTeachTab1GChip::CTeachTab1GChip(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab1GChip::IDD, pParent)
	, m_bIsTeachChipArea(FALSE)
	, m_bIsTeachChipAreaCheck(FALSE)
{
	m_pMainView = NULL;
	m_Chip.Clear();
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);

	m_bIsTeachMatch = FALSE;
}

CTeachTab1GChip::~CTeachTab1GChip()
{
}

void CTeachTab1GChip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX,	IDC_LABEL_TITLE_PANEL, 				m_wndLabelTitlePanel );
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,				m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_CHIP,				m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_CHIP,				m_nSelectRecipeIndex);
	
	DDX_Radio( pDX,		IDC_RADIO_CHIP_BYPASS, 				m_Chip.nUseBypass_Chip );
	DDX_Text(pDX,		IDC_EDIT_CHIP_DESCRIPTION,			m_Chip.strDescription);

	DDX_Text(pDX,		IDC_EDIT_CHIP_LED_LIGHT_VALUE,		m_Chip.nValueCh1);
	DDX_Text(pDX,		IDC_EDIT_CHIP_LED_LIGHT_VALUE2,		m_Chip.nValueCh2);
	DDX_Text(pDX,		IDC_EDIT_CHIP_LED_LIGHT_VALUE3,		m_Chip.nValueCh3);

	DDX_Text( pDX,		IDC_EDIT_CHIP_MATRIX_X,				m_Chip.nMatrix_X );
	DDX_Text( pDX,		IDC_EDIT_CHIP_MATRIX_Y,				m_Chip.nMatrix_Y );
	DDX_Text( pDX,		IDC_EDIT_CHIP_CHIPMATRIX_X,			m_Chip.nChipFovCnt_X );
	DDX_Text( pDX,		IDC_EDIT_CHIP_CHIPMATRIX_Y,			m_Chip.nChipFovCnt_Y );
	DDX_Text( pDX,		IDC_EDIT_CHIP_CHIPUNITPITCH_X_MM,	m_Chip.fChipUnitPitch_X_MM );
	DDX_Text( pDX,		IDC_EDIT_CHIP_CHIPUNITPITCH_Y_MM,	m_Chip.fChipUnitPitch_Y_MM );
	DDX_Text( pDX,		IDC_EDIT_CHIP_CHIPUNITPITCH_X_PX,	m_Chip.nChipUnitPitch_X_PX );
	DDX_Text( pDX,		IDC_EDIT_CHIP_CHIPUNITPITCH_Y_PX,	m_Chip.nChipUnitPitch_Y_PX );
	DDX_Text( pDX,		IDC_EDIT_CHIP_AVG_GV,				m_Chip.nAvgGv );

	DDX_Text(pDX,		IDC_EDIT_CHIP_MATCH_RATIO,			m_Chip.fRatio);
	DDX_Text(pDX,		IDC_EDIT_CHIP_MATCH_SHIFT_X_PX,		m_Chip.nShiftX_PX);
	DDX_Text(pDX,		IDC_EDIT_CHIP_MATCH_SHIFT_Y_PX,		m_Chip.nShiftY_PX);
	DDX_Text(pDX,		IDC_EDIT_CHIP_MATCH_SHIFT_X_MM,		m_Chip.fShiftX_MM);
	DDX_Text(pDX,		IDC_EDIT_CHIP_MATCH_SHIFT_Y_MM,		m_Chip.fShiftY_MM);

	DDX_Control(pDX,	IDC_BTN_TEST_CHIP,					m_btnTest_Chip );
	DDX_Control(pDX,	IDC_BTN_CHIP_RECIPE_NO,				m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_CHIP,					m_btnSave);

	DDX_Control(pDX,	IDC_BTN_ROI_CHIPAREA,				m_btnTeachChipArea);
	DDX_Check(pDX,		IDC_BTN_ROI_CHIPAREA,				m_bIsTeachChipArea);
	DDX_Control(pDX,	IDC_BTN_ROI_CHECKAREA,				m_btnTeachChipAreaCheck);
	DDX_Check(pDX,		IDC_BTN_ROI_CHECKAREA,				m_bIsTeachChipAreaCheck);

	DDV_MinMaxInt(pDX,	m_Chip.nAvgGv,						GV8_MIN, GV8_MAX);

	DDX_Radio(pDX, IDC_RADIO_SELECT_UNIT, m_Chip.nSelectUnit);
}


BEGIN_MESSAGE_MAP(CTeachTab1GChip, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_CHIP_BYPASS,		&CTeachTab1GChip::OnBnClickedRadioChipBypass )
	ON_BN_CLICKED(IDC_RADIO_CHIP_BYPASS2,		&CTeachTab1GChip::OnBnClickedRadioChipBypass )
	ON_BN_CLICKED(IDC_RADIO_CHIP_BYPASS3,		&CTeachTab1GChip::OnBnClickedRadioChipBypass )
	ON_BN_CLICKED(IDC_BTN_TEST_CHIP,			&CTeachTab1GChip::OnBnClickedBtnTestChip)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_CHIP,		&CTeachTab1GChip::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_CHIP,			&CTeachTab1GChip::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CHIP_RECIPE_NO,		&CTeachTab1GChip::OnBnClickedBtnChipRecipeNo)
	ON_BN_CLICKED(IDC_BTN_ROI_CHIPAREA,			&CTeachTab1GChip::OnBnClickedBtnRoiChipArea)
	ON_BN_CLICKED(IDC_BTN_ROI_CHECKAREA,		&CTeachTab1GChip::OnBnClickedBtnRoiCheckarea)
	ON_BN_CLICKED(IDC_RADIO_SELECT_UNIT,		&CTeachTab1GChip::OnBnClickedRadioSelectUnit)
	ON_BN_CLICKED(IDC_RADIO_SELECT_UNIT2,		&CTeachTab1GChip::OnBnClickedRadioSelectUnit)
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTab1GChip 메시지 처리기입니다.
BOOL CTeachTab1GChip::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitlePanel.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitlePanel.SetFontHeight( 12 );
	
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);
	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_Chip.SetColor( DEF_BTN_COLOR_MEASURE );

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab1GChip::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_PANEL			)->SetWindowText(CLanguageInfo::Instance()->ReadString(131));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(132));
	GetDlgItem(IDC_BTN_CHIP_RECIPE_NO			)->SetWindowText(CLanguageInfo::Instance()->ReadString(133));
	GetDlgItem(IDC_RADIO_CHIP_BYPASS			)->SetWindowText(CLanguageInfo::Instance()->ReadString(134));
	GetDlgItem(IDC_RADIO_CHIP_BYPASS2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(135));
	GetDlgItem(IDC_RADIO_CHIP_BYPASS3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(136));
	GetDlgItem(IDC_STATIC_DESCRIPTION			)->SetWindowText(CLanguageInfo::Instance()->ReadString(137));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(138));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(139));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(140));
	GetDlgItem(IDC_STATIC_CHIP_MATRIX_XY		)->SetWindowText(CLanguageInfo::Instance()->ReadString(141));
	GetDlgItem(IDC_STATIC_CHIP_INSPMATRIX_XY	)->SetWindowText(CLanguageInfo::Instance()->ReadString(143));
	GetDlgItem(IDC_BTN_TEST_CHIP				)->SetWindowText(CLanguageInfo::Instance()->ReadString(144));
	GetDlgItem(IDC_BTN_ROI_CHIPAREA				)->SetWindowText(CLanguageInfo::Instance()->ReadString(145));
	GetDlgItem(IDC_BTN_ROI_CHECKAREA			)->SetWindowText(CLanguageInfo::Instance()->ReadString(146));
	GetDlgItem(IDC_STATIC_CHIP_AVG_GV			)->SetWindowText(CLanguageInfo::Instance()->ReadString(147));
	GetDlgItem(IDC_STATIC_CHIP_MATCH_RATIO		)->SetWindowText(CLanguageInfo::Instance()->ReadString(148));
	
	CModelInfo::stChip& Chip = CModelInfo::Instance()->GetChip();
	
	if( m_Chip.nSelectUnit == 0) // px
	{
		GetDlgItem(IDC_STATIC_CHIP_UNITPITCH_XY)->SetWindowText(CLanguageInfo::Instance()->ReadString(142));
		GetDlgItem(IDC_STATIC_CHIP_MATCH_SHIFT)->SetWindowText(CLanguageInfo::Instance()->ReadString(149));
	}
	else
	{
		GetDlgItem(IDC_STATIC_CHIP_UNITPITCH_XY)->SetWindowText(CLanguageInfo::Instance()->ReadString(129));
		GetDlgItem(IDC_STATIC_CHIP_MATCH_SHIFT)->SetWindowText(CLanguageInfo::Instance()->ReadString(130));
	}

		
	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitlePanel.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab1GChip::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATRIX_X), _T("Tray의 X열 전체 포켓 수를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATRIX_Y), _T("Tray의 Y열 전체 포켓 수를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_CHIPMATRIX_X), _T("Grab Image에서 검사되는 포켓의 수(X)를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_CHIPMATRIX_Y), _T("Grab Image에서 검사되는 포켓의 수(Y)를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_X_MM), _T("[MM] Chip과 Chip사이의 거리(pitch) X값을 입력합니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_Y_MM), _T("[MM] Chip과 Chip사이의 거리(pitch) Y값을 입력합니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_X_PX), _T("[PX] Chip과 Chip사이의 거리(pitch) X값을 입력합니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_Y_PX), _T("[PX] Chip과 Chip사이의 거리(pitch) Y값을 입력합니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_AVG_GV), _T("검사 영역의 평균 GV(Gray Value)값을 입력합니다. 검사 영역 내의 GV값이 적혀있는 GV이상이면 Chip이 존재하는 것으로 판단합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATCH_RATIO), _T("Chip Shift 검사에 사용되는 파라미터입니다. 0. ~ 100. 사이의 값을 입력합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_X_PX), _T("[MM] Shift 검사의 Spec입니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_Y_PX), _T("[MM] Shift 검사의 Spec입니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_X_MM), _T("[PX] Shift 검사의 Spec입니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_Y_MM), _T("[PX] Shift 검사의 Spec입니다. 해당 값 보다 크면 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_CHIPAREA), _T("첫 Grab Image 내에서 좌 상단 포켓 영역을 선택합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_CHECKAREA), _T("Chip 검사 영역을 Viewer에 표시해줍니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_SELECT_UNIT), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_SELECT_UNIT2), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab1GChip::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab1GChip::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stChip& Chip = CModelInfo::Instance()->GetChip();
	Chip = m_Chip;

#ifdef RELEASE_1G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameChip(), TEACH_TAB_IDX_CHIP);

	CModelInfo::Instance()->Save(TEACH_TAB_IDX_CHIP);
#endif
	
	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab1GChip::CheckData()
{
#ifdef RELEASE_1G
	CString strLog;
	CModelInfo::stChip& Chip = CModelInfo::Instance()->GetChip();

	// ----- Chip Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[Chip.nUseBypass_Chip], strBypassName[m_Chip.nUseBypass_Chip] );
	if( Chip.nUseBypass_Chip != m_Chip.nUseBypass_Chip) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeChip, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), Chip.strDescription, m_Chip.strDescription);
	if (Chip.strDescription != m_Chip.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[nLightValue1][%d→%d]"), Chip.nValueCh1, m_Chip.nValueCh1);
	if (Chip.nValueCh1 != m_Chip.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nLightValue2][%d→%d]"), Chip.nValueCh2, m_Chip.nValueCh2);
	if (Chip.nValueCh2 != m_Chip.nValueCh2) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nLightValue3][%d→%d]"), Chip.nValueCh3, m_Chip.nValueCh3);
	if (Chip.nValueCh3 != m_Chip.nValueCh3) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[nMatrix_X][%d→%d]"), Chip.nMatrix_X, m_Chip.nMatrix_X);
	if (Chip.nMatrix_X != m_Chip.nMatrix_X) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nMatrix_Y][%d→%d]"), Chip.nMatrix_Y, m_Chip.nMatrix_Y);
	if (Chip.nMatrix_Y != m_Chip.nMatrix_Y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nChipFovCnt_X][%d→%d]"), Chip.nChipFovCnt_X, m_Chip.nChipFovCnt_X);
	if (Chip.nChipFovCnt_X != m_Chip.nChipFovCnt_X) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nChipFovCnt_Y][%d→%d]"), Chip.nChipFovCnt_Y, m_Chip.nChipFovCnt_Y);
	if (Chip.nChipFovCnt_Y != m_Chip.nChipFovCnt_Y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[fChipUnitPitch_X_MM][%.4f→%.4f]"), Chip.fChipUnitPitch_X_MM, m_Chip.fChipUnitPitch_X_MM);
	if (Chip.fChipUnitPitch_X_MM != m_Chip.fChipUnitPitch_X_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[fChipUnitPitch_Y_MM][%.4f→%.4f]"), Chip.fChipUnitPitch_Y_MM, m_Chip.fChipUnitPitch_Y_MM);
	if (Chip.fChipUnitPitch_Y_MM != m_Chip.fChipUnitPitch_Y_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nChipUnitPitch_X_PX][%d→%d]"), Chip.nChipUnitPitch_X_PX, m_Chip.nChipUnitPitch_X_PX);
	if (Chip.nChipUnitPitch_X_PX != m_Chip.nChipUnitPitch_X_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[nChipUnitPitch_Y_PX][%d→%d]"), Chip.nChipUnitPitch_Y_PX, m_Chip.nChipUnitPitch_Y_PX);
	if (Chip.nChipUnitPitch_Y_PX != m_Chip.nChipUnitPitch_Y_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[Area_Left][%d→%d]"), Chip.rcInspArea.left, m_Chip.rcInspArea.left);
	if (Chip.rcInspArea.left != m_Chip.rcInspArea.left) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Area_Right][%d→%d]"), Chip.rcInspArea.right, m_Chip.rcInspArea.right);
	if (Chip.rcInspArea.right != m_Chip.rcInspArea.right) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Area_Top][%d→%d]"), Chip.rcInspArea.top, m_Chip.rcInspArea.top);
	if (Chip.rcInspArea.top != m_Chip.rcInspArea.top) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Area_Bottom][%d→%d]"), Chip.rcInspArea.bottom, m_Chip.rcInspArea.bottom);
	if (Chip.rcInspArea.bottom != m_Chip.rcInspArea.bottom) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[Match_Area_Left][%d→%d]"), Chip.rcMatchArea.left, m_Chip.rcMatchArea.left);
	if (Chip.rcMatchArea.left != m_Chip.rcMatchArea.left) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Match_Area_Left][%d→%d]"), Chip.rcMatchArea.right, m_Chip.rcMatchArea.right);
	if (Chip.rcMatchArea.right != m_Chip.rcMatchArea.right) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Match_Area_Left][%d→%d]"), Chip.rcMatchArea.top, m_Chip.rcMatchArea.top);
	if (Chip.rcMatchArea.top != m_Chip.rcMatchArea.top) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Match_Area_Left][%d→%d]"), Chip.rcMatchArea.bottom, m_Chip.rcMatchArea.bottom);
	if (Chip.rcMatchArea.bottom != m_Chip.rcMatchArea.bottom) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[Match_CenterPoint_X][%d→%d]"), Chip.ptMatch.x, Chip.ptMatch.x);
	if (Chip.ptMatch.x != Chip.ptMatch.x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[Match_CenterPoint_Y][%d→%d]"), Chip.ptMatch.y, Chip.ptMatch.y);
	if (Chip.ptMatch.y != Chip.ptMatch.y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[ChipAvgGv][%d→%d]"), Chip.nAvgGv, m_Chip.nAvgGv);
	if (Chip.nAvgGv != m_Chip.nAvgGv) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);

	strLog.Format(_T("[fRatio][%.2f→%.2f]"), Chip.fRatio, m_Chip.fRatio);
	if (Chip.fRatio != m_Chip.fRatio) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[ShiftX_PX][%d→%d]"), Chip.nShiftX_PX, m_Chip.nShiftX_PX);
	if (Chip.nShiftX_PX != m_Chip.nShiftX_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[ShiftY_PX][%d→%d]"), Chip.nShiftY_PX, m_Chip.nShiftY_PX);
	if (Chip.nShiftY_PX != m_Chip.nShiftY_PX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[ShiftX_MM][%.4f→%.4f]"), Chip.fShiftX_MM, m_Chip.fShiftX_MM);
	if (Chip.fShiftX_MM != m_Chip.fShiftX_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
	strLog.Format(_T("[ShiftX_MM][%.4f→%.4f]"), Chip.fShiftY_MM, m_Chip.fShiftY_MM);
	if (Chip.fShiftY_MM != m_Chip.fShiftY_MM) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChip, strLog);
#endif
}

void CTeachTab1GChip::Refresh()
{
	UpdateRecipeList();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load( CModelInfo::Instance()->GetModelNameChip(), CHIP_KIND);

	CModelInfo::stChip& Chip = CModelInfo::Instance()->GetChip();

	m_Chip = Chip;

	BOOL bPx = (m_Chip.nSelectUnit == 0) ? TRUE : FALSE;
	GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_X_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_Y_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_X_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_Y_PX)->EnableWindow(bPx);

	GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_X_MM)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_Y_MM)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_X_MM)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_Y_MM)->EnableWindow(!bPx);

	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(TRUE);

		GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_X_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_Y_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_X_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_Y_PX)->ShowWindow(bPx);

		GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_X_MM)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_CHIP_CHIPUNITPITCH_Y_MM)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_X_MM)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_CHIP_MATCH_SHIFT_Y_MM)->ShowWindow(!bPx);
	}
	else
	{
		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(FALSE);
	}

	UpdateLanguage();
	UpdateData(FALSE);
}

void CTeachTab1GChip::Default()
{
	m_Chip.Clear();

	UpdateData(FALSE);
}

void CTeachTab1GChip::Cleanup()
{
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

	m_bIsTeachChipArea = FALSE;
}

HBRUSH CTeachTab1GChip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_LIGHT_VALUE2:
	case IDC_STATIC_LIGHT_VALUE3:
	case IDC_STATIC_CHIP_MATRIX_XY:
	case IDC_STATIC_CHIP_UNITPITCH_XY:
	case IDC_STATIC_CHIP_INSPMATRIX_XY:
	case IDC_STATIC_CHIP_AVG_GV:
	case IDC_STATIC_CHIP_MATCH_RATIO:
	case IDC_STATIC_CHIP_MATCH_SHIFT:
	case IDC_STATIC_SELECT_UNIT:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab1GChip::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab1GChip::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab1GChip::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab1GChip::OnBnClickedRadioChipBypass()
{
	UpdateData(TRUE);
}

void CTeachTab1GChip::OnBnClickedRadioSelectUnit()
{
	UpdateData(TRUE);
}

void CTeachTab1GChip::OnBnClickedBtnTestChip()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChip::OnBnClickedBtnTestChip"));

	CVisionSystem::Instance()->BeginStopWatch(IDX_AREA4);
	CVisionSystem::Instance()->StartInspection(InspectTypeChip, IDX_AREA4, 0, TRUE);
#endif
}

void CTeachTab1GChip::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(CHIP_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_CHIP);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameChip();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameChip();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameChip();
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

void CTeachTab1GChip::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_Chip\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameChip();
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


void CTeachTab1GChip::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChip::OnBnClickedBtnSave :: Start"));

	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameChip();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, CHIP_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, CHIP_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();
	Refresh();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab1GChip::OnBnClickedBtnSave :: End"));
}

void CTeachTab1GChip::DisableWnd(BOOL bEnable)
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

void CTeachTab1GChip::OnBnClickedBtnChipRecipeNo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GChip::OnBnClickedBtnChipRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GChip::OnBnClickedBtnChipRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( CHIP_KIND, TEACH_TAB_IDX_CHIP);
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 1G CHIP") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GChip::OnBnClickedBtnChipRecipeNo :: End"));
#endif
}

void CTeachTab1GChip::OnBnClickedBtnRoiChipArea()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChip::OnBnClickedBtnRoiChipArea"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

	if (m_bIsTeachChipArea)
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

		m_btnTeachChipArea.EnableWindow(TRUE);
		AfxMessageBox(_T("Tray의 우하단 첫 번째 Chip 영역을 지정하세요."));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA4, _OnConfirmTracker, this, m_Chip.rcInspArea);
	}
	else
	{
		Cleanup();
	}
#endif
}

void APIENTRY CTeachTab1GChip::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab1GChip* pThis = (CTeachTab1GChip*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab1GChip::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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

	BOOL bRet = FALSE;

	if (m_bIsTeachChipArea)
	{
		pGO->Reset();
		m_Chip.rcInspArea = rcTrackRegion;

		clrBox.CreateObject(PDC_ORANGE, m_Chip.rcInspArea);
		pGO->AddDrawBox(clrBox);

		m_bIsTeachChipArea = FALSE;
		m_bIsTeachMatch = TRUE;

		AfxMessageBox(_T("Shift 검사에 사용할 Fiducial Mark의 영역을 지정하세요."));

#ifdef RELEASE_1G
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA4, _OnConfirmTracker, this, m_Chip.rcMatchArea);
#endif
	}
	else if (m_bIsTeachMatch)
	{
		m_Chip.rcMatchArea = rcTrackRegion;
		m_Chip.ptMatch.x = rcTrackRegion.CenterPoint().x - m_Chip.rcInspArea.left;
		m_Chip.ptMatch.y = rcTrackRegion.CenterPoint().y - m_Chip.rcInspArea.top;

		bRet = pInspectionVision->SetLearnModel_ForChip(pGO, pImgObj, m_Chip.rcMatchArea);
		if (!bRet)
		{
			AfxMessageBox(_T("Fiducial Mark Teaching Fail !!"));

			m_Chip.rcMatchArea.SetRectEmpty();
			m_Chip.ptMatch = (0, 0);
		}
	}

	if (bRet)
	{
		Cleanup();
		UpdateData(FALSE);
	}
}

void CTeachTab1GChip::OnBnClickedBtnRoiCheckarea()
{
#ifdef RELEASE_1G
	COLORBOX clrBox;
	COLORTEXT clrText;

	UpdateData(TRUE);

	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA4);
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

	if (m_bIsTeachChipAreaCheck)
	{
		CString strNum;
		CRect rcRect;
		rcRect.SetRectEmpty();

		int nNum = 0;
		float fCalibrationX = CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[2][CalibrationDirX];
		float fCalibrationY = CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[2][CalibrationDirY];
		//nNum = m_Chip.nChipFovCnt_X * m_Chip.nChipFovCnt_Y;

		for (int nY = 0; nY < m_Chip.nChipFovCnt_Y; ++nY)
		{
			for (int nX = 0; nX < m_Chip.nChipFovCnt_X; ++nX)
			{
				nNum++;

				if (m_Chip.nSelectUnit == 0) // px
				{
					rcRect.left = m_Chip.rcInspArea.left - m_Chip.nChipUnitPitch_X_PX * nX;
					rcRect.right = m_Chip.rcInspArea.right - m_Chip.nChipUnitPitch_X_PX * nX;
					rcRect.top = m_Chip.rcInspArea.top - m_Chip.nChipUnitPitch_Y_PX * nY;
					rcRect.bottom = m_Chip.rcInspArea.bottom - m_Chip.nChipUnitPitch_Y_PX * nY;
				}
				else
				{
					rcRect.left = m_Chip.rcInspArea.left - GetRoundLong( (m_Chip.fChipUnitPitch_X_MM * 1000.f / fCalibrationX) * nX);
					rcRect.right = m_Chip.rcInspArea.right - GetRoundLong( (m_Chip.fChipUnitPitch_X_MM * 1000.f / fCalibrationX) * nX);
					rcRect.top = m_Chip.rcInspArea.top - GetRoundLong( (m_Chip.fChipUnitPitch_Y_MM * 1000.f / fCalibrationY) * nY);
					rcRect.bottom = m_Chip.rcInspArea.bottom - GetRoundLong( (m_Chip.fChipUnitPitch_Y_MM * 1000.f / fCalibrationY) * nY);
				}
				clrBox.CreateObject(PDC_ORANGE, rcRect, PS_DOT, 3);
				pGO->AddDrawBox(clrBox);

				strNum.Format(_T("No. %d"), nNum);
				clrText.SetText(strNum);
				clrText.CreateObject(PDC_ORANGE, rcRect.left + 20, rcRect.top + 30, 15);
				pGO->AddDrawText(clrText);
			}
		}

		m_bIsTeachChipAreaCheck = FALSE;
		UpdateData(FALSE);
	}
#endif
}

void CTeachTab1GChip::EnableChip()
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

void CTeachTab1GChip::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_PANEL);

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
				strParams.Format(_T("\"file:///%s#page=34\""), (LPCTSTR)strPdfPath);

				// 3. ShellExecute 실행
				HINSTANCE hInst = ShellExecute(NULL, _T("open"), _T("msedge.exe"), strParams, NULL, SW_SHOWNORMAL);

				if ((INT_PTR)hInst <= 32)
					AfxMessageBox(_T("Manual Pdf 파일을 열 수 없습니다."));
			}
			else if (point.x < nRightAreaEndX)	// Right
			{
				CString strPath;
				strPath.Format(_T("%s\\Manual"), GetExecuteDirectory());
				ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}
