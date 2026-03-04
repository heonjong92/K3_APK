// TeachTab1G3DChipCnt.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab1G3DChipCnt.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "InspectionVision.h"
#include "StringMacro.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>

constexpr double TRAY_COMMON_WIDTH = 322.6;

UINT nManualChipNum; // 수동 칩 수량 검사용 파라미터

// CTeachTab1G3DChipCnt 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab1G3DChipCnt, CDialog)

CTeachTab1G3DChipCnt::CTeachTab1G3DChipCnt(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab1G3DChipCnt::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsTeachFirst(FALSE)
	, m_bCheckEqualize(FALSE)
{
	m_pMainView = NULL;
	m_3DChipCnt.Clear();
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab1G3DChipCnt::~CTeachTab1G3DChipCnt()
{
}

void CTeachTab1G3DChipCnt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,		IDC_LABEL_TITLE_1G_3DCHIPCNT,		m_wndLabelTitle1G3DChipCnt);
	DDX_Control(pDX,		IDC_LABEL_RECIPE_TITLE,				m_wndLabelRecipeTitle);

	DDX_Control(pDX,		IDC_COMBO_RECIPE_3DCHIPCNT,			m_wndSelectRecipe);
	DDX_CBIndex(pDX,		IDC_COMBO_RECIPE_3DCHIPCNT,			m_nSelectRecipeIndex);
		
	DDX_Radio(pDX,			IDC_RADIO_3DCHIPCNT_BYPASS,			m_3DChipCnt.nUseBypass_3DChipCnt);
	DDX_Text(pDX,			IDC_EDIT_3DCHIPCNT_DESCRIPTION,		m_3DChipCnt.strDescription);

	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_IMAGE_CUT,			m_3DChipCnt.nImageCut);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_IMAGE_CUT2,		m_3DChipCnt.nImageCut2);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_IMAGE_SHIFTX,		m_3DChipCnt.nShiftX);
	DDX_Check(pDX,			IDC_CHECK_CHIPCNT_EQUALIZE,			m_3DChipCnt.bEqualize);
	
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_MIN_THRESHOLD,		m_3DChipCnt.nMinThreshold);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_MAX_THRESHOLD,		m_3DChipCnt.nMaxThreshold);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_COUNT_X,			m_3DChipCnt.nXCount);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_COUNT_Y,			m_3DChipCnt.nYCount);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_MANUAL_CHIPCNT,	nManualChipNum);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_ROW_PITCH,			m_3DChipCnt.nRowPitchPx);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_COLUMN_PITCH,		m_3DChipCnt.nColumnPitchPx);
	
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_ROW_PITCH_MM,		m_3DChipCnt.dRowPitch);
	DDX_Text(pDX,			IDC_EDIT_CHIPCNT_COLUMN_PITCH_MM,	m_3DChipCnt.dColumnPitch);

	DDX_Control(pDX,		IDC_BTN_TEST_3DCHIPCNT,				m_btnTest_3DChipCnt);
	DDX_Control(pDX,		IDC_BTN_3DCHIPCNT_RECIPE_NO,		m_btnSaveRecipeNo);
	DDX_Control(pDX,		IDC_BTN_SAVE_3DCHIPCNT,				m_btnSave);

	DDV_MinMaxInt(pDX,		m_3DChipCnt.nMinThreshold,			GV8_MIN, GV8_MAX);
	DDV_MinMaxInt(pDX,		m_3DChipCnt.nMaxThreshold,			GV8_MIN, GV8_MAX);
	DDX_Control(pDX,		IDC_BTN_ROI_3DCHIPCNT_FIRST,		m_btnFirstROI);
	DDX_Check(	pDX,		IDC_BTN_ROI_3DCHIPCNT_FIRST,		m_bIsTeachFirst);

	DDX_Radio(pDX,			IDC_RADIO_CHIPCNT_SELECT_UNIT,		m_3DChipCnt.nSelectUnit);
}


BEGIN_MESSAGE_MAP(CTeachTab1G3DChipCnt, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_3DCHIPCNT_BYPASS,		&CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipCntBypass)
	ON_BN_CLICKED(IDC_RADIO_3DCHIPCNT_BYPASS2,		&CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipCntBypass)
	ON_BN_CLICKED(IDC_RADIO_3DCHIPCNT_BYPASS3,		&CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipCntBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_3DCHIPCNT,			&CTeachTab1G3DChipCnt::OnBnClickedBtnTest3DChipCnt)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_3DCHIPCNT,	&CTeachTab1G3DChipCnt::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_3DCHIPCNT,			&CTeachTab1G3DChipCnt::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_3DCHIPCNT_RECIPE_NO,		&CTeachTab1G3DChipCnt::OnBnClickedBtn3DChipCntRecipeNo)
	ON_BN_CLICKED(IDC_BTN_ROI_3DCHIPCNT_FIRST,		&CTeachTab1G3DChipCnt::OnBnClickedBtnRoi3dchipcntFirst)
	ON_BN_CLICKED(IDC_RADIO_CHIPCNT_SELECT_UNIT,	&CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipSelectUnit)
	ON_BN_CLICKED(IDC_RADIO_CHIPCNT_SELECT_UNIT2,	&CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipSelectUnit)
ON_WM_LBUTTONDBLCLK()
ON_BN_CLICKED(IDC_CHECK_CHIPCNT_EQUALIZE, &CTeachTab1G3DChipCnt::OnBnClickedCheckChipcntEqualize)
END_MESSAGE_MAP()


// CTeachTab1G3DChipCnt 메시지 처리기입니다.
BOOL CTeachTab1G3DChipCnt::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1G3DChipCnt.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle1G3DChipCnt.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_3DChipCnt.SetColor( DEF_BTN_COLOR_MEASURE );

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab1G3DChipCnt::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_1G_3DCHIPCNT			)->SetWindowText(CLanguageInfo::Instance()->ReadString(50));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(51));
	GetDlgItem(IDC_BTN_3DCHIPCNT_RECIPE_NO			)->SetWindowText(CLanguageInfo::Instance()->ReadString(52));
	GetDlgItem(IDC_STATIC_CHIPCNT_DESCRIPTION		)->SetWindowText(CLanguageInfo::Instance()->ReadString(53));
	GetDlgItem(IDC_STATIC_CHIPCNT_EMPTY_CHIP_AREA	)->SetWindowText(CLanguageInfo::Instance()->ReadString(54));
	GetDlgItem(IDC_STATIC_CHIPCNT_ROW_COLUMN_COUNT	)->SetWindowText(CLanguageInfo::Instance()->ReadString(57));
	GetDlgItem(IDC_BTN_TEST_3DCHIPCNT				)->SetWindowText(CLanguageInfo::Instance()->ReadString(58));
	GetDlgItem(IDC_STATIC_CHIPCNT_MANUAL_CHIPCNT	)->SetWindowText(CLanguageInfo::Instance()->ReadString(59));
	GetDlgItem(IDC_RADIO_3DCHIPCNT_BYPASS			)->SetWindowText(CLanguageInfo::Instance()->ReadString(60));
	GetDlgItem(IDC_RADIO_3DCHIPCNT_BYPASS2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(61));
	GetDlgItem(IDC_RADIO_3DCHIPCNT_BYPASS3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(62));
	GetDlgItem(IDC_BTN_ROI_3DCHIPCNT_FIRST			)->SetWindowText(CLanguageInfo::Instance()->ReadString(64));
	GetDlgItem(IDC_STATIC_CHIPCNT_SELECT_UNIT		)->SetWindowText(CLanguageInfo::Instance()->ReadString(10));
	
	CModelInfo::st3DChipCnt& st3DChipCnt = CModelInfo::Instance()->Get3DChipCnt();
	if (st3DChipCnt.nSelectUnit == 0)		GetDlgItem(IDC_STATIC_CHIPCNT_ROW_COLUMN_PITCH)->SetWindowText(CLanguageInfo::Instance()->ReadString(56));
	else									GetDlgItem(IDC_STATIC_CHIPCNT_ROW_COLUMN_PITCH)->SetWindowText(CLanguageInfo::Instance()->ReadString(46));

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1G3DChipCnt.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab1G3DChipCnt::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_CUT), _T("2회 Grab 이후 Image Merge하기 위해 사용되는 Parameter이며, 초기 셋업 이후 축 위치가 변경되지 않는 이상 값이 바꾸지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_CUT2), _T("2회 Grab 이후 Image Merge하기 위해 사용되는 Parameter이며, 초기 셋업 이후 축 위치가 변경되지 않는 이상 값이 바꾸지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_SHIFTX), _T("1회 Grab Image와 2회 Grab Image를 수직을 맞추기 위해 X축 이동으로 사용되는 Parameter이며, 초기 셋업 이후 값이 바뀌지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_CHIPCNT_EQUALIZE), _T("영상의 명암 대비를 자동으로 보정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_MIN_THRESHOLD), _T("Chip이 없는 포켓의 Gray Value 값 범위를 설정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_MAX_THRESHOLD), _T("Chip이 없는 포켓의 Gray Value 값 범위를 설정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_COLUMN_PITCH), _T("Chip과 Chip의 거리 값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_ROW_PITCH), _T("Chip과 Chip의 거리 값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_COUNT_X), _T("Tray의 X열 전체 포켓 수를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_COUNT_Y), _T("Tray의 X열 전체 포켓 수를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPCNT_MANUAL_CHIPCNT), _T("수동 검사를 할 때만 사용하는 파라미터 입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_3DCHIPCNT_FIRST), _T("좌 상단의 Chip 검사 영역을 설정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT2), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab1G3DChipCnt::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab1G3DChipCnt::Save()
{
	UpdateData(TRUE);

	CModelInfo::st3DChipCnt& st3DChipCnt = CModelInfo::Instance()->Get3DChipCnt();

	// Validation Check
	if (m_3DChipCnt.nMinThreshold > m_3DChipCnt.nMaxThreshold)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("Empty Chip Area 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum value of the Empty Chip Area must be less than or equal to the maximum value."), MB_ICONERROR);
		
		m_3DChipCnt = st3DChipCnt;
		UpdateData(FALSE);

		return FALSE;
	}

	st3DChipCnt = m_3DChipCnt;
//	st3DChipCnt.bEqualize = m_bCheckEqualize;

	CheckData();

#ifdef RELEASE_1G
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_3DCHIPCNT );
#endif

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab1G3DChipCnt::CheckData()
{
#ifdef RELEASE_1G
	CString strLog;
	CModelInfo::st3DChipCnt& st3DChipCnt = CModelInfo::Instance()->Get3DChipCnt();

	// ----- 3DChipCnt Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[st3DChipCnt.nUseBypass_3DChipCnt], strBypassName[m_3DChipCnt.nUseBypass_3DChipCnt] );
	if( st3DChipCnt.nUseBypass_3DChipCnt != m_3DChipCnt.nUseBypass_3DChipCnt) CVisionSystem::Instance()->WriteLogforTeaching( InspectType3DChipCnt, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), st3DChipCnt.strDescription, m_3DChipCnt.strDescription);
	if (st3DChipCnt.strDescription != m_3DChipCnt.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Image Cut 1][%d→%d]"), st3DChipCnt.nImageCut, m_3DChipCnt.nImageCut);
	if (st3DChipCnt.nImageCut != m_3DChipCnt.nImageCut) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Image Cut 2][%d→%d]"), st3DChipCnt.nImageCut2, m_3DChipCnt.nImageCut2);
	if (st3DChipCnt.nImageCut2 != m_3DChipCnt.nImageCut2) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Image Shift X][%d→%d]"), st3DChipCnt.nShiftX, m_3DChipCnt.nShiftX);
	if (st3DChipCnt.nShiftX != m_3DChipCnt.nShiftX) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Image Equalize][%d→%d]"), st3DChipCnt.bEqualize, m_3DChipCnt.bEqualize);
	if (st3DChipCnt.bEqualize != m_3DChipCnt.bEqualize) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Min Threshold][%d→%d]"), st3DChipCnt.nMinThreshold, m_3DChipCnt.nMinThreshold);
	if (st3DChipCnt.nMinThreshold != m_3DChipCnt.nMinThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Max Threshold][%d→%d]"), st3DChipCnt.nMaxThreshold, m_3DChipCnt.nMaxThreshold);
	if (st3DChipCnt.nMaxThreshold != m_3DChipCnt.nMaxThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Row Pitch][%.3lf→%.3lf]"), st3DChipCnt.dRowPitch, m_3DChipCnt.dRowPitch);
	if (st3DChipCnt.dRowPitch != m_3DChipCnt.dRowPitch) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Row Pitch Px][%d→%d]"), st3DChipCnt.nRowPitchPx, m_3DChipCnt.nRowPitchPx);
	if (st3DChipCnt.nRowPitchPx != m_3DChipCnt.nRowPitchPx) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Column Pitch][%.3lf→%.3lf]"), st3DChipCnt.dColumnPitch, m_3DChipCnt.dColumnPitch);
	if (st3DChipCnt.dColumnPitch != m_3DChipCnt.dColumnPitch) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Column Pitch Px][%d→%d]"), st3DChipCnt.nColumnPitchPx, m_3DChipCnt.nColumnPitchPx);
	if (st3DChipCnt.nColumnPitchPx != m_3DChipCnt.nColumnPitchPx) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[X Count][%d→%d]"), st3DChipCnt.nXCount, m_3DChipCnt.nXCount);
	if (st3DChipCnt.nXCount != m_3DChipCnt.nXCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

	strLog.Format(_T("[Y Count][%d→%d]"), st3DChipCnt.nYCount, m_3DChipCnt.nYCount);
	if (st3DChipCnt.nYCount != m_3DChipCnt.nYCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectType3DChipCnt, strLog);

#endif
}

void CTeachTab1G3DChipCnt::Refresh()
{
	UpdateRecipeList();
	CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();
	CModelInfo::st3DChipCnt& st3DChipCnt = CModelInfo::Instance()->Get3DChipCnt();
	m_3DChipCnt = st3DChipCnt;
	
	m_bCheckEqualize = st3DChipCnt.bEqualize;

	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName == strModelName)
		EnableCutShift();
	
	BOOL bPx = (st3DChipCnt.nSelectUnit == 0) ? TRUE : FALSE;
	GetDlgItem(IDC_EDIT_CHIPCNT_COLUMN_PITCH)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_CHIPCNT_ROW_PITCH)->EnableWindow(bPx);

	GetDlgItem(IDC_EDIT_CHIPCNT_COLUMN_PITCH_MM)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_CHIPCNT_ROW_PITCH_MM)->EnableWindow(!bPx);

	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_STATIC_CHIPCNT_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT2)->ShowWindow(TRUE);

		GetDlgItem(IDC_EDIT_CHIPCNT_COLUMN_PITCH)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_CHIPCNT_ROW_PITCH)->ShowWindow(bPx);

		GetDlgItem(IDC_EDIT_CHIPCNT_COLUMN_PITCH_MM)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_CHIPCNT_ROW_PITCH_MM)->ShowWindow(!bPx);
	}
	else
	{
		GetDlgItem(IDC_STATIC_CHIPCNT_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT2)->ShowWindow(FALSE);
	}
	UpdateLanguage();
	UpdateData(FALSE);
}

void CTeachTab1G3DChipCnt::UpdateUI()
{
#ifdef RELEASE_1G
	CxGraphicObject* pGO;
	pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA2);
	pGO->Reset();
	pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA3);
	pGO->Reset();		
#endif
}

void CTeachTab1G3DChipCnt::Default()
{
	m_3DChipCnt.Clear();

	UpdateData(FALSE);
}

void CTeachTab1G3DChipCnt::Cleanup()
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

	m_bIsTeachFirst = FALSE;
}

HBRUSH CTeachTab1G3DChipCnt::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_CHIPCNT_BYPASS:
	case IDC_STATIC_CHIPCNT_DESCRIPTION:
	case IDC_STATIC_CHIPCNT_EMPTY_CHIP_AREA:
	case IDC_STATIC_CHIPCNT_ROW_COLUMN_FIRST:
	case IDC_STATIC_CHIPCNT_ROW_COLUMN_PITCH:
	case IDC_STATIC_CHIPCNT_ROW_COLUMN_COUNT:
	case IDC_STATIC_CHIPCNT_THICKNESS:
	case IDC_STATIC_CHIPCNT_MANUAL_CHIPCNT:
	case IDC_STATIC_CHIPCNT_IMAGE_CUT:
	case IDC_STATIC_CHIPCNT_IMAGE_SHIFTX:
	case IDC_STATIC_CHIPCNT_SELECT_UNIT:
	case IDC_STATIC_EQUALIZE:
	case IDC_CHECK_CHIPCNT_EQUALIZE:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab1G3DChipCnt::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab1G3DChipCnt::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		UpdateRecipeList();
		DisableWnd(TRUE);
		//Refresh();
	}
	else
	{
		Cleanup();
	}
}

BOOL CTeachTab1G3DChipCnt::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipCntBypass()
{
	UpdateData(TRUE);
}

void CTeachTab1G3DChipCnt::OnBnClickedRadio3DChipSelectUnit()
{
	UpdateData(TRUE);
}

void CTeachTab1G3DChipCnt::OnBnClickedBtnTest3DChipCnt()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	WRITE_LOG( WL_BTN, _T("CTeachTab1G3DChipCnt::OnBnClickedBtnTest3DChipCnt") );

	CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();
	if (strModelName == _T("CALIBRATION"))
		CVisionSystem::Instance()->m_bAcm[ACM_3DCHIPCNT] = TRUE;

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA3 );
	CVisionSystem::Instance()->StartInspection(InspectType3DChipCnt, IDX_AREA3, 1, TRUE );

#endif
}

void CTeachTab1G3DChipCnt::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(TOPCHIPCNT_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_3DCHIPCNT);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();
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

void CTeachTab1G3DChipCnt::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_3DChipCnt\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();
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


void CTeachTab1G3DChipCnt::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1G3DChipCnt::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelName3DChipCnt();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, TOPCHIPCNT_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, TOPCHIPCNT_KIND);
		}

		DisableWnd(TRUE);
		Refresh();

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();
	Refresh();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab1G3DChipCnt::OnBnClickedBtnSave :: End"));
}

void CTeachTab1G3DChipCnt::DisableWnd(BOOL bEnable)
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

void CTeachTab1G3DChipCnt::OnBnClickedBtn3DChipCntRecipeNo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1G3DChipCnt::OnBnClickedBtn3DChipCntRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1G3DChipCnt::OnBnClickedBtn3DChipCntRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( TOPCHIPCNT_KIND, TEACH_TAB_IDX_3DCHIPCNT);
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 1G 3DCHIPCNT") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{
	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1G3DChipCnt::OnBnClickedBtn3DChipCntRecipeNo :: End"));
#endif
}

void APIENTRY CTeachTab1G3DChipCnt::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab1G3DChipCnt* pThis = (CTeachTab1G3DChipCnt*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab1G3DChipCnt::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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

	BOOL bRet = FALSE;

	if (m_bIsTeachFirst)
	{
		m_3DChipCnt.rcChipPosFirst = rcTrackRegion;

		UpdateUI();
		bRet = TRUE;

		clrBox.CreateObject(PDC_ORANGE, m_3DChipCnt.rcChipPosFirst);
		pGO->AddDrawBox(clrBox);
	}

	if (bRet)
	{
		Cleanup();
		UpdateData(FALSE);

		Save();
	}
}

void CTeachTab1G3DChipCnt::OnBnClickedBtnRoi3dchipcntFirst()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_1G

	CString strDescription = _T("Tray 좌상단 Chip의 영역을 선택해주세요.");
	
	if (CLanguageInfo::Instance()->m_nLangType == 1)
		strDescription = _T("Please select the area of the chip in the upper left of the tray.");

	if(m_bIsTeachFirst)	
		AfxMessageBox(strDescription);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA3);

	if (m_bIsTeachFirst)
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
		
		m_btnFirstROI.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA3, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab1G3DChipCnt::EnableCutShift()
{
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_CUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_CUT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_SHIFTX)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CHIPCNT_EQUALIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_CHIPCNT_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT2)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_CUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_CUT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHIPCNT_IMAGE_SHIFTX)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CHIPCNT_EQUALIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_CHIPCNT_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_CHIPCNT_SELECT_UNIT2)->ShowWindow(FALSE);
	}
}

void CTeachTab1G3DChipCnt::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_1G_3DCHIPCNT);

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
				strParams.Format(_T("\"file:///%s#page=30\""), (LPCTSTR)strPdfPath);

				// 3. ShellExecute 실행
				HINSTANCE hInst = ShellExecute(NULL, _T("open"), _T("msedge.exe"), strParams, NULL, SW_SHOWNORMAL);

				if ((INT_PTR)hInst <= 32)
					AfxMessageBox(_T("Manual Pdf 파일을 열 수 없습니다."));
			}
			else if (point.x > nRightAreaEndX)	// Right
			{
				CString strPath;
				strPath.Format(_T("%s\\Manual"), GetExecuteDirectory());
				ShellExecute( NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL );
			}
		}
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CTeachTab1G3DChipCnt::OnBnClickedCheckChipcntEqualize()
{
	UpdateData(TRUE);
}
