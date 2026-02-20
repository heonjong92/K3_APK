// TeachTab3GDesiccantMaterial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab3GDesiccantMaterial.h"
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

// CTeachTab3GDesiccantMaterial 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab3GDesiccantMaterial, CDialog)

int CTeachTab3GDesiccantMaterial::m_nManualTray = 6;

CTeachTab3GDesiccantMaterial::CTeachTab3GDesiccantMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab3GDesiccantMaterial::IDD, pParent)
	, m_RadioTeachingSubMaterial(0)
	, m_bIsTeachSubMaterial(FALSE)
	, m_nSelectRecipeIndex(-1)
	, m_bIsTeachTrayROI(FALSE)
	, m_nSelectTrayRecipeIndex(-1)
	, m_nRadioTeachingSubMaterialTray(0)
	, m_nScrollPos(0)
{
	m_pMainView = NULL;
	m_DesiccantMaterialInfo.Clear();

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab3GDesiccantMaterial::~CTeachTab3GDesiccantMaterial()
{
	m_ImageObjectForHICDesiccantTeaching.Destroy();
	//m_ImageObjectForTrayModelTeaching.Destroy();
}

void CTeachTab3GDesiccantMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_3G,								m_wndLabelTitle3G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,							m_wndLabelRecipeTitle);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE_TRAY,					m_wndLabelRecipeTitleTray);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_MATERIAL,						m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_MATERIAL,						m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_MATERIAL_BYPASS,						m_DesiccantMaterialInfo.nUseBypass_Material);
	DDX_Text(pDX,		IDC_EDIT_MATERIAL_DESCRIPTION,					m_DesiccantMaterialInfo.strDescription);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_MATERIAL_LED_LIGHT_VALUE,	m_DesiccantMaterialInfo.nValueCh1);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_MATERIAL_LED_LIGHT_VALUE2,	m_DesiccantMaterialInfo.nValueCh2);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT_MATERIAL_QUANTITY,			m_DesiccantMaterialInfo.nDesiccantQuantity);

	DDX_Text(pDX,		IDC_EDIT_INSPECTION_ZONE_COL,					m_DesiccantMaterialInfo.nInspectionZoneCol);
	DDX_Text(pDX,		IDC_EDIT_INSPECTION_ZONE_RATIO,					m_DesiccantMaterialInfo.nRatio1);
	DDX_Text(pDX,		IDC_EDIT_INSPECTION_ZONE_RATIO2,				m_DesiccantMaterialInfo.nRatio2);
	DDX_Text(pDX,		IDC_EDIT_INSPECTION_ZONE_RATIO3,				m_DesiccantMaterialInfo.nRatio3);
	DDX_Text(pDX,		IDC_EDIT_SUBMATERIAL_PIXEL_VAL,					m_DesiccantMaterialInfo.nSubMaterialPixelVal);
	DDX_Text(pDX,		IDC_EDIT_TOP_BOT_MARGIN,						m_DesiccantMaterialInfo.nInspectionZoneMargin);
	DDX_Text(pDX,		IDC_EDIT_SUBMATERIAL_OUT_TOLERANCE,				m_DesiccantMaterialInfo.dbOutTolerance);

	DDX_Text(pDX,		IDC_EDIT_HIC_PATTERN_POSI_X,					m_DesiccantMaterialInfo.ptHicShift.x);
	DDX_Text(pDX,		IDC_EDIT_HIC_PATTERN_POSI_Y,					m_DesiccantMaterialInfo.ptHicShift.y);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT1_PATTERN_POSI_X,				m_DesiccantMaterialInfo.ptDesiccant1Shift.x);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT1_PATTERN_POSI_Y,				m_DesiccantMaterialInfo.ptDesiccant1Shift.y);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT2_PATTERN_POSI_X,				m_DesiccantMaterialInfo.ptDesiccant2Shift.x);
	DDX_Text(pDX,		IDC_EDIT_DESICCANT2_PATTERN_POSI_Y,				m_DesiccantMaterialInfo.ptDesiccant2Shift.y);
	DDX_Text(pDX,		IDC_EDIT_SUBMATERIAL_PATTERN_SHIFT_X,			m_DesiccantMaterialInfo.ptPatternShiftTolerance.x);
	DDX_Text(pDX,		IDC_EDIT_SUBMATERIAL_PATTERN_SHIFT_Y,			m_DesiccantMaterialInfo.ptPatternShiftTolerance.y);
	DDX_Text(pDX,		IDC_EDIT_SUBMATERIAL_MATCH_SCORE_HIC,			m_DesiccantMaterialInfo.fMatchScoreHIC);
	DDX_Text(pDX,		IDC_EDIT_SUBMATERIAL_MATCH_SCORE_DESICCANT,		m_DesiccantMaterialInfo.fMatchScoreDesiccant);

	DDX_Radio(pDX,		IDC_RADIO_TEACHING_SUBMATERIAL,					m_RadioTeachingSubMaterial);
	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_TEACH_SUBMATERIAL,			m_ctrlPicPreviewTeachSubMaterial);
	DDX_Control(pDX,	IDC_BTN_SUBMATERIAL_TECH_MODEL,					m_btnSubMaterialTechModel);
	DDX_Check(pDX,		IDC_BTN_SUBMATERIAL_TECH_MODEL,					m_bIsTeachSubMaterial);

	DDX_Control(pDX,	IDC_BTN_TEST_SUBMATERIAL,						m_btnTest_SubMaterial);

	DDX_Control(pDX,	IDC_BTN_MATERIAL_RECIPE_NO,						m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_MATERIAL,							m_btnSave);

	DDX_Check(pDX,		IDC_BTN_SUBMATERIAL_TEACH_TRAYMODEL,			m_bIsTeachTrayROI);
	DDX_Control(pDX,	IDC_BTN_SUBMATERIAL_TEACH_TRAYMODEL,			m_btnTeachModelTray);

	DDX_Text(pDX,		IDC_EDIT_MATERIAL_TRAY_DESCRIPTION,				m_DesiccantMaterialTrayInfo.strDescription);
	DDX_Text(pDX,		IDC_EDIT_TRAY_AMOUNT_MIN,						m_DesiccantMaterialTrayInfo.nTrayQuantityMin);
	DDX_Text(pDX,		IDC_EDIT_TRAY_AMOUNT_MAX,						m_DesiccantMaterialTrayInfo.nTrayQuantityMax);
	DDX_Text(pDX,		IDC_EDIT_MANUAL_TRAY_AMOUNT,					m_nManualTray);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_MATERIAL_TRAY,					m_wndSelectTrayRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_MATERIAL_TRAY,					m_nSelectTrayRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_TEACHING_SUBMATERIAL_TRAY,			m_nRadioTeachingSubMaterialTray);
	DDX_Control(pDX,	IDC_BTN_TEST_SUBMATERIAL_TRAY,					m_btnTest_SubMaterialTray);
	DDX_Control(pDX,	IDC_BTN_MATERIAL_RECIPE_NO_TRAY,				m_btnSaveTrayRecipeNo);

	DDV_MinMaxInt(pDX,	m_DesiccantMaterialInfo.nInspectionZoneCol,		1, 3);
	DDV_MinMaxInt(pDX,	m_DesiccantMaterialInfo.nDesiccantQuantity,		0, 2);
}

BEGIN_MESSAGE_MAP(CTeachTab3GDesiccantMaterial, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	
	// Bypass - Material
	ON_BN_CLICKED(IDC_RADIO_MATERIAL_BYPASS,				&CTeachTab3GDesiccantMaterial::OnBnClickedRadioMaterialBypass)
	ON_BN_CLICKED(IDC_RADIO_MATERIAL_BYPASS2,				&CTeachTab3GDesiccantMaterial::OnBnClickedRadioMaterialBypass)
	ON_BN_CLICKED(IDC_RADIO_MATERIAL_BYPASS3,				&CTeachTab3GDesiccantMaterial::OnBnClickedRadioMaterialBypass)

	// Teaching
	ON_BN_CLICKED(IDC_BTN_SUBMATERIAL_TECH_MODEL,			&CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTechModel)
	ON_BN_CLICKED(IDC_RADIO_TEACHING_SUBMATERIAL,			&CTeachTab3GDesiccantMaterial::OnBnClickedRadioTeachingSubmaterial)
	ON_BN_CLICKED(IDC_RADIO_TEACHING_SUBMATERIAL2,			&CTeachTab3GDesiccantMaterial::OnBnClickedRadioTeachingSubmaterial)
	ON_BN_CLICKED(IDC_RADIO_TEACHING_SUBMATERIAL_TRAY,		&CTeachTab3GDesiccantMaterial::OnBnClickedRadioTeachingSubmaterialTray)
	ON_BN_CLICKED(IDC_RADIO_TEACHING_SUBMATERIAL_TRAY2,		&CTeachTab3GDesiccantMaterial::OnBnClickedRadioTeachingSubmaterialTray)
	ON_BN_CLICKED(IDC_BTN_SUBMATERIAL_TEACH_TRAYMODEL,		&CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTeachTraymodel)

	// Inspection
	ON_BN_CLICKED(IDC_BTN_TEST_SUBMATERIAL,					&CTeachTab3GDesiccantMaterial::OnBnClickedBtnTestSubmaterial)
	
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_MATERIAL,				&CTeachTab3GDesiccantMaterial::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_MATERIAL_RECIPE_NO,				&CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNo)

	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_MATERIAL_TRAY, 		&CTeachTab3GDesiccantMaterial::OnCbnSelchangeComboRecipeMaterialTray)
	ON_BN_CLICKED(IDC_BTN_MATERIAL_RECIPE_NO_TRAY,			&CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNoTray)

	ON_BN_CLICKED(IDC_BTN_SAVE_MATERIAL,					&CTeachTab3GDesiccantMaterial::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_TEST_SUBMATERIAL_TRAY,			&CTeachTab3GDesiccantMaterial::OnBnClickedBtnTestSubmaterialTray)
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab3GDesiccantMaterial 메시지 처리기입니다.
BOOL CTeachTab3GDesiccantMaterial::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle3G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle3G.SetFontHeight( 12 );

	m_ctrlPicPreviewTeachSubMaterial.GetClientRect(m_rcPicCtrlSizeTeachSubMaterial);

	m_btnTest_SubMaterial.SetColor( DEF_BTN_COLOR_MEASURE );
	//m_btnTest_SubMaterialTray.SetColor(DEF_BTN_COLOR_MEASURE); 

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndLabelRecipeTitleTray.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitleTray.SetBold(TRUE);
	m_wndLabelRecipeTitleTray.SetFontHeight(12);
	
	m_wndSelectRecipe.SetFont(&m_fontRecipe);
	
	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);
	
	// Matching Viewer
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor     = UIExt::CResourceManager::Instance()->GetViewerBodyColor();

	m_ImageViewForHICDesiccantTeaching.Create(&m_ImageViewManagerForHICDesiccantTeaching, this);
	m_ImageViewForHICDesiccantTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForHICDesiccantTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForHICDesiccantTeaching.SetAnimateWindow(FALSE);

	m_ImageViewForHICDesiccantTeaching.SetImageObject(&m_ImageObjectForHICDesiccantTeaching, FALSE);
	m_ImageViewForHICDesiccantTeaching.SetRegisterCallBack(0, &regCB);

	m_ImageViewForHICDesiccantTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForHICDesiccantTeaching.SetBodyColor(dwBodyColor);

	m_ImageViewForHICDesiccantTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForHICDesiccantTeaching.ShowScrollBar(FALSE);
	m_ImageViewForHICDesiccantTeaching.ShowScaleBar(FALSE);
	m_ImageViewForHICDesiccantTeaching.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_SUBMATERIAL)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(1);
	hDWP = DeferWindowPos( hDWP, m_ImageViewForHICDesiccantTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0 );

	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_STATIC_SUBMATERIAL_PIXEL_VALUE			), CLanguageInfo::Instance()->ReadString(44));
	m_toolTip.AddTool(GetDlgItem(IDC_STATIC_DESICCANT1_PATTERN_POSITION		), CLanguageInfo::Instance()->ReadString(47));
	m_toolTip.AddTool(GetDlgItem(IDC_STATIC_DESICCANT2_PATTERN_POSITION		), CLanguageInfo::Instance()->ReadString(48));
	m_toolTip.AddTool(GetDlgItem(IDC_STATIC_SUBMATERIAL_SHIFT_TOLERANCE		), CLanguageInfo::Instance()->ReadString(49));
	m_toolTip.AddTool(GetDlgItem(IDC_STATIC_SUBMATERIAL_MATCH_SCORE			), CLanguageInfo::Instance()->ReadString(50));
	
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INSPECTION_ZONE_COL), _T("검사 구획의 열 개수을 지정합니다. 1~3 값을 입력해주세요."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TOP_BOT_MARGIN), _T("검사 구획의 위아래 여백(Tray 크기에서 + px)을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO), _T("검사 구획의 비율을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO2), _T("검사 구획의 비율을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO3), _T("검사 구획의 비율을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SUBMATERIAL_PIXEL_VAL), _T("부자재의 픽셀 값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT_MATERIAL_QUANTITY), _T("방습제의 수량을 각각 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SUBMATERIAL_OUT_TOLERANCE), _T("부자재가 Tray에서 위아래로 이탈해도 허용되는 값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_PATTERN_POSI_X), _T("HIC의 X 위치를 입력합니다. 기준점은 패턴의 중앙입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_PATTERN_POSI_Y), _T("HIC의 Y 위치를 입력합니다. 기준점은 패턴의 중앙입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_X), _T("첫 번째 방습제의 X 위치를 입력합니다. 기준점은 패턴의 중앙입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_Y), _T("첫 번째 방습제의 Y 위치를 입력합니다. 기준점은 패턴의 중앙입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_X), _T("두 번째 방습제의 X 위치를 입력합니다. 기준점은 패턴의 중앙입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_Y), _T("두 번째 방습제의 Y 위치를 입력합니다. 기준점은 패턴의 중앙입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SUBMATERIAL_PATTERN_SHIFT_X), _T("부자재 패턴의 X Shift 허용치를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SUBMATERIAL_PATTERN_SHIFT_Y), _T("부자재 패턴의 Y Shift 허용치를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SUBMATERIAL_MATCH_SCORE_HIC), _T("HIC 매칭 Score를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SUBMATERIAL_MATCH_SCORE_DESICCANT), _T("Desiccant 매칭 Score를 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_SUBMATERIAL_TECH_MODEL), _T("HIC, 또는 방습제 라디오 버튼을 선택한 뒤 부자재 패턴 검사 모델을 등록합니다"));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAY_AMOUNT_MIN), _T("해당 Recipe의 소분된 Tray가 올 수 있는 최소 수량을 입력합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAY_AMOUNT_MAX), _T("해당 Recipe의 소분된 Tray가 올 수 있는 최대 수량을 입력합니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MANUAL_TRAY_AMOUNT), _T("수동 검사 시 Tray의 개수를 입력합니다. 최소 영역과 최대 영역에 따라 자동으로 Tray 수량에 따른 영역이 계산됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_SUBMATERIAL_TEACH_TRAYMODEL), _T("최소, 최대 영역 라디오 버튼을 클릭 후 영역을 설정합니다. Tray 기준으로 영역 설정하면 됩니다."));
	m_toolTip.Activate(TRUE);
	
	UpdateRecipeList();
	UpdateUI();
	UpdateLanguage();

	InitScroll();

	//GetDlgItem(IDC_BTN_SUBMATERIAL_ROI)->EnableWindow(FALSE); // 검사 영역 설정 안쓸 듯

	return TRUE;
}

void CTeachTab3GDesiccantMaterial::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_3G						)->SetWindowText(CLanguageInfo::Instance()->ReadString(31));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(32));
	GetDlgItem(IDC_BTN_MATERIAL_RECIPE_NO				)->SetWindowText(CLanguageInfo::Instance()->ReadString(33));
	GetDlgItem(IDC_BTN_MATERIAL_RECIPE_NO_TRAY			)->SetWindowText(CLanguageInfo::Instance()->ReadString(33));
	GetDlgItem(IDC_RADIO_MATERIAL_BYPASS				)->SetWindowText(CLanguageInfo::Instance()->ReadString(34));
	GetDlgItem(IDC_RADIO_MATERIAL_BYPASS2				)->SetWindowText(CLanguageInfo::Instance()->ReadString(35));
	GetDlgItem(IDC_RADIO_MATERIAL_BYPASS3				)->SetWindowText(CLanguageInfo::Instance()->ReadString(36));
	GetDlgItem(IDC_STATIC_DESCRIPTION					)->SetWindowText(CLanguageInfo::Instance()->ReadString(37));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(38));
	GetDlgItem(IDC_RADIO_TEACHING_SUBMATERIAL			)->SetWindowText(CLanguageInfo::Instance()->ReadString(39));
	GetDlgItem(IDC_RADIO_TEACHING_SUBMATERIAL2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(40));
	GetDlgItem(IDC_BTN_SUBMATERIAL_TECH_MODEL			)->SetWindowText(CLanguageInfo::Instance()->ReadString(41));
	GetDlgItem(IDC_BTN_TEST_SUBMATERIAL					)->SetWindowText(CLanguageInfo::Instance()->ReadString(42));
	GetDlgItem(IDC_STATIC_INSPECTION_ZONE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(43));
	GetDlgItem(IDC_STATIC_SUBMATERIAL_PIXEL_VALUE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(44));
	GetDlgItem(IDC_STATIC_SUBMATERIAL_OUT_TOLERANCE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(45));
	GetDlgItem(IDC_STATIC_HIC_PATTERN_POSITION			)->SetWindowText(CLanguageInfo::Instance()->ReadString(46));
	GetDlgItem(IDC_STATIC_DESICCANT1_PATTERN_POSITION	)->SetWindowText(CLanguageInfo::Instance()->ReadString(47));
	GetDlgItem(IDC_STATIC_DESICCANT2_PATTERN_POSITION	)->SetWindowText(CLanguageInfo::Instance()->ReadString(48));
	GetDlgItem(IDC_STATIC_SUBMATERIAL_SHIFT_TOLERANCE	)->SetWindowText(CLanguageInfo::Instance()->ReadString(49));
	GetDlgItem(IDC_STATIC_SUBMATERIAL_MATCH_SCORE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(50));
	GetDlgItem(IDC_STATIC_INSPECTION_ZONE_RATIO			)->SetWindowText(CLanguageInfo::Instance()->ReadString(51));

	GetDlgItem(IDC_LABEL_RECIPE_TITLE_TRAY				)->SetWindowText(CLanguageInfo::Instance()->ReadString(61));
	GetDlgItem(IDC_STATIC_MATERIAL_TRAY_DESCRIPTION		)->SetWindowText(CLanguageInfo::Instance()->ReadString(62));
	GetDlgItem(IDC_STATIC_TRAY_AMOUNT					)->SetWindowText(CLanguageInfo::Instance()->ReadString(63));
	GetDlgItem(IDC_STATIC_MANUAL_TRAY_AMOUNT			)->SetWindowText(CLanguageInfo::Instance()->ReadString(64));
	GetDlgItem(IDC_RADIO_TEACHING_SUBMATERIAL_TRAY		)->SetWindowText(CLanguageInfo::Instance()->ReadString(65));
	GetDlgItem(IDC_RADIO_TEACHING_SUBMATERIAL_TRAY2		)->SetWindowText(CLanguageInfo::Instance()->ReadString(66));
	GetDlgItem(IDC_BTN_SUBMATERIAL_TEACH_TRAYMODEL		)->SetWindowText(CLanguageInfo::Instance()->ReadString(67));
	GetDlgItem(IDC_BTN_TEST_SUBMATERIAL_TRAY			)->SetWindowText(CLanguageInfo::Instance()->ReadString(68));

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle3G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitleTray.SetColor(dwLabelBodyColor, dwLabelTextColor);

	m_toolTip.UpdateTipText(CLanguageInfo::Instance()->ReadString(44), GetDlgItem(IDC_STATIC_SUBMATERIAL_PIXEL_VALUE));
	m_toolTip.UpdateTipText(CLanguageInfo::Instance()->ReadString(47), GetDlgItem(IDC_STATIC_DESICCANT1_PATTERN_POSITION));
	m_toolTip.UpdateTipText(CLanguageInfo::Instance()->ReadString(48), GetDlgItem(IDC_STATIC_DESICCANT2_PATTERN_POSITION));
	m_toolTip.UpdateTipText(CLanguageInfo::Instance()->ReadString(49), GetDlgItem(IDC_STATIC_SUBMATERIAL_SHIFT_TOLERANCE));
	m_toolTip.UpdateTipText(CLanguageInfo::Instance()->ReadString(50), GetDlgItem(IDC_STATIC_SUBMATERIAL_MATCH_SCORE));
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab3GDesiccantMaterial::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab3GDesiccantMaterial::Save()
{
	UpdateData(TRUE);

	switch (m_DesiccantMaterialInfo.nInspectionZoneCol)
	{
	case 1:
		m_DesiccantMaterialInfo.nRatio2 = 0;
		m_DesiccantMaterialInfo.nRatio3 = 0;
		break;
	case 2:
		m_DesiccantMaterialInfo.nRatio3 = 0;
	default:
		break;
	}

	CheckData();

	CModelInfo::stDesiccantMaterialInfo& DesiccantMaterialInfo = CModelInfo::Instance()->GetDesiccantMaterialInfo();
	if ((m_DesiccantMaterialInfo.nRatio1 + m_DesiccantMaterialInfo.nRatio2 + m_DesiccantMaterialInfo.nRatio3) > 100)
	{
		AfxMessageBox(_T("Ratio의 합은 100이하여야 합니다."), MB_ICONERROR);
		m_DesiccantMaterialInfo = DesiccantMaterialInfo;
		UpdateData(FALSE);
		return FALSE;
	}
	DesiccantMaterialInfo = m_DesiccantMaterialInfo;

	CModelInfo::stDesiccantMaterialTrayInfo& DesiccantTrayInfo = CModelInfo::Instance()->GetDesiccantMaterialTrayInfo();
	DesiccantTrayInfo = m_DesiccantMaterialTrayInfo;

#ifdef RELEASE_3G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save( CModelInfo::Instance()->GetModelNameDesiccantMaterial(), TEACH_TAB_IDX_DESICCANT_MATERIAL );
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_DESICCANT_MATERIAL );

	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameDesiccantMaterialTray(), TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY);
	CModelInfo::Instance()->Save(TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY);
#endif

	Refresh();
	
	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab3GDesiccantMaterial::CheckData()
{
#ifdef RELEASE_3G
	CString strLog;
	CModelInfo::stDesiccantMaterialInfo& DesiccantMaterialInfo = CModelInfo::Instance()->GetDesiccantMaterialInfo();

	// ----- Material Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[DesiccantMaterialInfo.nUseBypass_Material], strBypassName[m_DesiccantMaterialInfo.nUseBypass_Material] );
	if( DesiccantMaterialInfo.nUseBypass_Material != m_DesiccantMaterialInfo.nUseBypass_Material ) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeMaterial, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), DesiccantMaterialInfo.strDescription, m_DesiccantMaterialInfo.strDescription);
	if (DesiccantMaterialInfo.strDescription != m_DesiccantMaterialInfo.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[LightValueCh1][%d→%d]"), DesiccantMaterialInfo.nValueCh1, m_DesiccantMaterialInfo.nValueCh1);
	if (DesiccantMaterialInfo.nValueCh1 != m_DesiccantMaterialInfo.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);
	
	strLog.Format(_T("[LightValueCh2][%d→%d]"), DesiccantMaterialInfo.nValueCh2, m_DesiccantMaterialInfo.nValueCh2);
	if (DesiccantMaterialInfo.nValueCh2 != m_DesiccantMaterialInfo.nValueCh2) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[InspectionZoneCol][%d→%d]"), DesiccantMaterialInfo.nInspectionZoneCol, m_DesiccantMaterialInfo.nInspectionZoneCol);
	if (DesiccantMaterialInfo.nInspectionZoneCol != m_DesiccantMaterialInfo.nInspectionZoneCol) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[1stZoneRatio][%d→%d]"), DesiccantMaterialInfo.nRatio1, m_DesiccantMaterialInfo.nRatio1);
	if (DesiccantMaterialInfo.nRatio1 != m_DesiccantMaterialInfo.nRatio1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);	
	
	strLog.Format(_T("[2ndZoneRatio][%d→%d]"), DesiccantMaterialInfo.nRatio2, m_DesiccantMaterialInfo.nRatio2);
	if (DesiccantMaterialInfo.nRatio2 != m_DesiccantMaterialInfo.nRatio2) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);	
	
	strLog.Format(_T("[3rdZoneRatio][%d→%d]"), DesiccantMaterialInfo.nRatio3, m_DesiccantMaterialInfo.nRatio3);
	if (DesiccantMaterialInfo.nRatio3 != m_DesiccantMaterialInfo.nRatio3) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);
	
	strLog.Format(_T("[InspectionZoneMargin][%d→%d]"), DesiccantMaterialInfo.nInspectionZoneMargin, m_DesiccantMaterialInfo.nInspectionZoneMargin);
	if (DesiccantMaterialInfo.nInspectionZoneMargin != m_DesiccantMaterialInfo.nInspectionZoneMargin) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[SubMaterialPixelValue][%d→%d]"), DesiccantMaterialInfo.nSubMaterialPixelVal, m_DesiccantMaterialInfo.nSubMaterialPixelVal);
	if (DesiccantMaterialInfo.nSubMaterialPixelVal != m_DesiccantMaterialInfo.nSubMaterialPixelVal) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[DesiccantQuantity][%d→%d]"), DesiccantMaterialInfo.nDesiccantQuantity, m_DesiccantMaterialInfo.nDesiccantQuantity);
	if (DesiccantMaterialInfo.nDesiccantQuantity != m_DesiccantMaterialInfo.nDesiccantQuantity) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[HicShiftX][%d→%d]"), DesiccantMaterialInfo.ptHicShift.x, m_DesiccantMaterialInfo.ptHicShift.x);
	if (DesiccantMaterialInfo.ptHicShift.x != m_DesiccantMaterialInfo.ptHicShift.x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[HicShiftY][%d→%d]"), DesiccantMaterialInfo.ptHicShift.y, m_DesiccantMaterialInfo.nDesiccantQuantity);
	if (DesiccantMaterialInfo.nDesiccantQuantity != m_DesiccantMaterialInfo.nDesiccantQuantity) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[Desiccant1ShiftX][%d→%d]"), DesiccantMaterialInfo.ptDesiccant1Shift.x, m_DesiccantMaterialInfo.ptDesiccant1Shift.x);
	if (DesiccantMaterialInfo.ptDesiccant1Shift.x != m_DesiccantMaterialInfo.ptDesiccant1Shift.x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[Desiccant1ShiftY][%d→%d]"), DesiccantMaterialInfo.ptDesiccant1Shift.y, m_DesiccantMaterialInfo.ptDesiccant1Shift.y);
	if (DesiccantMaterialInfo.ptDesiccant1Shift.y != m_DesiccantMaterialInfo.ptDesiccant1Shift.y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[Desiccant2ShiftX][%d→%d]"), DesiccantMaterialInfo.ptDesiccant2Shift.x, m_DesiccantMaterialInfo.ptDesiccant2Shift.x);
	if (DesiccantMaterialInfo.ptDesiccant2Shift.x != m_DesiccantMaterialInfo.ptDesiccant2Shift.x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[Desiccant2ShiftY][%d→%d]"), DesiccantMaterialInfo.ptDesiccant2Shift.y, m_DesiccantMaterialInfo.ptDesiccant2Shift.y);
	if (DesiccantMaterialInfo.ptDesiccant2Shift.y != m_DesiccantMaterialInfo.ptDesiccant2Shift.y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[SubMaterialTrayOutTolerance][%.1lf→%.1lf]"), DesiccantMaterialInfo.dbOutTolerance, m_DesiccantMaterialInfo.dbOutTolerance);
	if (DesiccantMaterialInfo.dbOutTolerance != m_DesiccantMaterialInfo.dbOutTolerance) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[PatternShiftToleranceX][%d→%d]"), DesiccantMaterialInfo.ptPatternShiftTolerance.x, m_DesiccantMaterialInfo.ptPatternShiftTolerance.x);
	if (DesiccantMaterialInfo.ptPatternShiftTolerance.x != m_DesiccantMaterialInfo.ptPatternShiftTolerance.x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[PatternShiftToleranceY][%d→%d]"), DesiccantMaterialInfo.ptPatternShiftTolerance.y, m_DesiccantMaterialInfo.ptPatternShiftTolerance.y);
	if (DesiccantMaterialInfo.ptPatternShiftTolerance.y != m_DesiccantMaterialInfo.ptPatternShiftTolerance.y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	strLog.Format(_T("[PatternMatchScoreHIC][%.1lf→%.1lf]"), DesiccantMaterialInfo.fMatchScoreHIC, m_DesiccantMaterialInfo.fMatchScoreHIC);
	if (DesiccantMaterialInfo.fMatchScoreHIC != m_DesiccantMaterialInfo.fMatchScoreHIC) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);	
	
	strLog.Format(_T("[PatternMatchScoreDesiccant][%.1lf→%.1lf]"), DesiccantMaterialInfo.fMatchScoreDesiccant, m_DesiccantMaterialInfo.fMatchScoreDesiccant);
	if (DesiccantMaterialInfo.fMatchScoreDesiccant != m_DesiccantMaterialInfo.fMatchScoreDesiccant)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterial, strLog);

	// Tray Edge Inspection
	CModelInfo::stDesiccantMaterialTrayInfo& DesiccantMaterialTrayInfo = CModelInfo::Instance()->GetDesiccantMaterialTrayInfo();
	strLog.Format(_T("[Tray Description][%s→%s]"), DesiccantMaterialTrayInfo.strDescription, m_DesiccantMaterialTrayInfo.strDescription);
	if (DesiccantMaterialTrayInfo.strDescription != m_DesiccantMaterialTrayInfo.strDescription)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterialTray, strLog);

	strLog.Format(_T("[Tray Quantity min][%d→%d]"), DesiccantMaterialTrayInfo.nTrayQuantityMin, m_DesiccantMaterialTrayInfo.nTrayQuantityMin);
	if (DesiccantMaterialTrayInfo.nTrayQuantityMin != m_DesiccantMaterialTrayInfo.nTrayQuantityMin)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterialTray, strLog);

	strLog.Format(_T("[Tray Quantity MAX][%d→%d]"), DesiccantMaterialTrayInfo.nTrayQuantityMax, m_DesiccantMaterialTrayInfo.nTrayQuantityMax);
	if (DesiccantMaterialTrayInfo.nTrayQuantityMax != m_DesiccantMaterialTrayInfo.nTrayQuantityMax)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMaterialTray, strLog);
#endif
}

void CTeachTab3GDesiccantMaterial::Refresh()
{
	UpdateRecipeList();
	CString strMaterialModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(strMaterialModelName, SUBMATERIAL_KIND);
	pInspectionVision->Load(CModelInfo::Instance()->GetModelNameDesiccantMaterialTray(), SUBMATERIALTRAY_KIND);

	CModelInfo::stDesiccantMaterialInfo& DesiccantMaterialInfo = CModelInfo::Instance()->GetDesiccantMaterialInfo();
	m_DesiccantMaterialInfo = DesiccantMaterialInfo;

	CModelInfo::stDesiccantMaterialTrayInfo& DesiccantMaterialTrayInfo = CModelInfo::Instance()->GetDesiccantMaterialTrayInfo();
	m_DesiccantMaterialTrayInfo = DesiccantMaterialTrayInfo;

	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
	if (strSelectModelName == strMaterialModelName)
		EnableContionalCtrl(m_DesiccantMaterialInfo, TRUE);

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab3GDesiccantMaterial::Default()
{
	m_DesiccantMaterialInfo.Clear();

	UpdateData(FALSE);
}

void CTeachTab3GDesiccantMaterial::Cleanup()
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

	m_bIsTeachSubMaterial = FALSE;
	m_bIsTeachTrayROI = FALSE;
}

HBRUSH CTeachTab3GDesiccantMaterial::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_DESICCANT_QUANTITY:
	case IDC_STATIC_INSPECTION_ZONE:
	case IDC_STATIC_SUBMATERIAL_PIXEL_VALUE:
	case IDC_STATIC_HIC_PATTERN_POSITION:
	case IDC_STATIC_DESICCANT1_PATTERN_POSITION:
	case IDC_STATIC_DESICCANT2_PATTERN_POSITION:
	case IDC_STATIC_SUBMATERIAL_SHIFT_TOLERANCE:
	case IDC_STATIC_MATERIAL_TRAY_DESCRIPTION:
	case IDC_STATIC_INSPECTION_ZONE_RATIO:
	case IDC_STATIC_SUBMATERIAL_MATCH_SCORE:
	case IDC_STATIC_TRAY_AMOUNT:
	case IDC_STATIC_MANUAL_TRAY_AMOUNT:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab3GDesiccantMaterial::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab3GDesiccantMaterial::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		UpdateRecipeList();
		UpdateTrayRecipeList();
		DisableWnd(TRUE);
//		Refresh();
	}
	else
	{
		Cleanup();
	}
}

BOOL CTeachTab3GDesiccantMaterial::PreTranslateMessage(MSG* pMsg)
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

void APIENTRY CTeachTab3GDesiccantMaterial::_OnConfirmTracker( CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData )
{
	CTeachTab3GDesiccantMaterial* pThis = (CTeachTab3GDesiccantMaterial*)lpUsrData;
	pThis->OnConfirmTracker( rcTrackRegion, nIndexData );
}

void CTeachTab3GDesiccantMaterial::OnConfirmTracker( CRect& rcTrackRegion, UINT nViewIndex )
{
#ifdef RELEASE_3G
	COLORBOX clrBox;

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	CxImageObject* pImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex);
	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, nViewIndex);

	CRect rcTrack = rcTrackRegion;
	if(rcTrack.left > rcTrack.right)
	{
		if(rcTrack.top > rcTrack.bottom)
			rcTrackRegion.SetRect(rcTrack.right, rcTrack.bottom, rcTrack.left, rcTrack.top);
		else
			rcTrackRegion.SetRect(rcTrack.right, rcTrack.top, rcTrack.left, rcTrack.bottom);
	}
	else
		if(rcTrack.top > rcTrack.bottom)
			rcTrackRegion.SetRect(rcTrack.left, rcTrack.bottom, rcTrack.right, rcTrack.top);

	pGO->Reset();

	BOOL bRet = FALSE;

	if( m_bIsTeachSubMaterial)
	{
		switch (m_RadioTeachingSubMaterial)
		{
		case EMATCH_TYPE_HIC:
			bRet = pInspectionVision->SetLearnModel( pGO, pImgObj, rcTrackRegion, InspectTypeMaterial, nViewIndex, m_RadioTeachingSubMaterial, TRUE, FALSE );
			//m_DesiccantMaterialInfo.ptOffset_HICExistShift = CPoint(rcTrackRegion.left, rcTrackRegion.top);
			break;

		case EMATCH_TYPE_HIC_DESICCANT:
			bRet = pInspectionVision->SetLearnModel( pGO, pImgObj, rcTrackRegion, InspectTypeMaterial, nViewIndex, m_RadioTeachingSubMaterial, FALSE, FALSE );
			//m_DesiccantMaterialInfo.ptOffset_DesiccantExistShift = CPoint(rcTrackRegion.left, rcTrackRegion.top);
			break;
		}
	}

	if (m_bIsTeachTrayROI)
	{
		switch (m_nRadioTeachingSubMaterialTray)
		{
		case 0:
			m_DesiccantMaterialTrayInfo.rcTrayRoiMin = rcTrackRegion;
			UpdateUI();

			clrBox.CreateObject(PDC_ORANGE, m_DesiccantMaterialTrayInfo.rcTrayRoiMin);
			pGO->AddDrawBox(clrBox);			
			break;


		case 1:
			m_DesiccantMaterialTrayInfo.rcTrayRoiMax = rcTrackRegion;
			UpdateUI();

			clrBox.CreateObject(PDC_ORANGE, m_DesiccantMaterialTrayInfo.rcTrayRoiMax);
			pGO->AddDrawBox(clrBox);
			break;
		}
	}

	//if( bRet )
	//{
	Cleanup();
	UpdateUI();
	UpdateData(FALSE);
	//}
#endif
}

void CTeachTab3GDesiccantMaterial::OnBnClickedRadioMaterialBypass()
{
	UpdateData( TRUE );
}

void CTeachTab3GDesiccantMaterial::OnBnClickedRadioTeachingSubmaterial()
{
	UpdateData( TRUE );

	UpdateUI();
}

void CTeachTab3GDesiccantMaterial::OnBnClickedRadioTeachingSubmaterialTray()
{
	UpdateData( TRUE );

	UpdateUI();
}

void CTeachTab3GDesiccantMaterial::UpdateUI()
{
	UpdateTeachingImage();
}

void CTeachTab3GDesiccantMaterial::UpdateTeachingImage()
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return;

#ifdef RELEASE_3G
	USES_CONVERSION;
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA5, m_RadioTeachingSubMaterial);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if( MatchBuff->GetPatternLearnt() )
	{
		int nWidth =  MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForHICDesiccantTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		strLastRecipe = CModelInfo::Instance()->GetLastModelNameDesiccantMaterial();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("3G_Desiccant_Material\\") + strLastRecipe + _T("\\");
		strInspName = _T("Material");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, m_RadioTeachingSubMaterial);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForHICDesiccantTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForHICDesiccantTeaching.LoadFromFile(strImgPath);
	}
	else 
	{
		m_ImageObjectForHICDesiccantTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForHICDesiccantTeaching.ImageUpdate();

	m_pMainView->UpdateImageView();

#endif
}

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTechModel()
{
#ifdef RELEASE_3G
	WRITE_LOG( WL_BTN, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTechModel") );

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic( CamTypeAreaScan, IDX_AREA5 );

	if( m_bIsTeachSubMaterial )
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

		m_btnSubMaterialTechModel.EnableWindow(TRUE);
		AfxMessageBox(_T("등록할 모델의 영역을 지정하세요."));
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA5, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTeachTraymodel()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTeachTraymodel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA5);

	if (m_bIsTeachTrayROI)
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

		m_btnTeachModelTray.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA5, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnTestSubmaterial()
{
#ifdef RELEASE_3G
	WRITE_LOG( WL_BTN, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnTestSubmaterial") );
	UpdateData(TRUE);

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA5 );
	CVisionSystem::Instance()->StartInspection( InspectTypeMaterial, IDX_AREA5, 0, TRUE);
#endif
}

void CTeachTab3GDesiccantMaterial::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_3G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(SUBMATERIAL_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_DESICCANT_MATERIAL);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();
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

void CTeachTab3GDesiccantMaterial::OnCbnSelchangeComboRecipeMaterialTray()
{
#ifdef RELEASE_3G
	UpdateData(TRUE);

	if (m_nSelectTrayRecipeIndex < 0)
		return;

	if (m_nSelectTrayRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(SUBMATERIALTRAY_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_DESICCANT_MATERIAL);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateTrayRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterialTray();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterialTray();
			int nIndex = m_wndSelectTrayRecipe.FindStringExact(0, strModelName);
			m_wndSelectTrayRecipe.SetCurSel(nIndex);
			m_nSelectTrayRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterialTray();
		CString strSelectModelName;
		m_wndSelectTrayRecipe.GetLBText(m_nSelectTrayRecipeIndex, strSelectModelName);
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

void CTeachTab3GDesiccantMaterial::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("3G_Desiccant_Material\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();
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

void CTeachTab3GDesiccantMaterial::UpdateTrayRecipeList()
{
	m_wndSelectTrayRecipe.ResetContent();

	m_wndSelectTrayRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("3G_Desiccant_Material_Tray\\");
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
			m_wndSelectTrayRecipe.AddString(ff.GetFileTitle());
		}
	}

	ff.Close();

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterialTray();
	int nIndex = m_wndSelectTrayRecipe.FindStringExact(0, strModelName);
	m_wndSelectTrayRecipe.SetCurSel(nIndex);
	m_nSelectTrayRecipeIndex = nIndex;

	if (m_nSelectTrayRecipeIndex < 0)
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

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnSave :: Start"));


	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, SUBMATERIAL_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, SUBMATERIAL_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		//return;
	}

	// Tray Info
	strModelName = CModelInfo::Instance()->GetModelNameDesiccantMaterialTray();
	m_wndSelectTrayRecipe.GetLBText(m_nSelectTrayRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, SUBMATERIALTRAY_KIND))
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, SUBMATERIALTRAY_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnSave :: End"));
}

void CTeachTab3GDesiccantMaterial::DisableWnd(BOOL bEnable)
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
	m_wndSelectTrayRecipe.EnableWindow(TRUE);
	//m_btnSave.EnableWindow(TRUE);
}

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNo()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( SUBMATERIAL_KIND, TEACH_TAB_IDX_DESICCANT_MATERIAL );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 3G DESICCANT MATERIAL") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNo :: End"));
#endif
}

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNoTray()
{
#ifdef RELEASE_3G
	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNoTray :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNoTray :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind(SUBMATERIALTRAY_KIND, TEACH_TAB_IDX_DESICCANT_MATERIAL);
	EditJobNumberDlg.SetTitleName(_T("Recipe No : 3G DESICCANT MATERIAL TRAY"));

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnMaterialRecipeNo :: End"));
#endif
}

void CTeachTab3GDesiccantMaterial::OnBnClickedBtnTestSubmaterialTray()
{
#ifdef RELEASE_3G
	COLORBOX clrBox;
	COLORTEXT clrText;

	UpdateData(TRUE);

	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA5);
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA5);

	CModelInfo::stDesiccantMaterialTrayInfo& stDesiccantTray = CModelInfo::Instance()->GetDesiccantMaterialTrayInfo();

	const int nTrayQuantityMin = stDesiccantTray.nTrayQuantityMin;
	const int nTrayQuantityMax = stDesiccantTray.nTrayQuantityMax;
	const CRect rcTrayRoiMin = stDesiccantTray.rcTrayRoiMin;
	const CRect rcTrayRoiMax = stDesiccantTray.rcTrayRoiMax;

	const int nTrayQuantity = CTeachTab3GDesiccantMaterial::m_nManualTray;

	CRect rcTray;
	rcTray.left		= rcTrayRoiMin.left - (abs(rcTrayRoiMin.left - rcTrayRoiMax.left) / (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);
	rcTray.top		= rcTrayRoiMin.top - (abs(rcTrayRoiMin.top - rcTrayRoiMax.top) / (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);
	rcTray.right	= rcTrayRoiMin.right + (abs(rcTrayRoiMin.right - rcTrayRoiMax.right) / (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);
	rcTray.bottom	= rcTrayRoiMin.bottom + (abs(rcTrayRoiMin.bottom - rcTrayRoiMax.bottom) / (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);

	clrBox.CreateObject(PDC_LIGHTGREEN, rcTray, PS_DASH, 1);
	pGO->AddDrawBox(clrBox);

	//WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnTestSubmaterialTray"));
	//UpdateData(TRUE);

	//CVisionSystem::Instance()->BeginStopWatch(IDX_AREA5);
	//CVisionSystem::Instance()->StartInspection(InspectTypeMaterialTray, IDX_AREA5, 0, TRUE);
#endif
}

void CTeachTab3GDesiccantMaterial::InitScroll()
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
	si.nPage = GetRoundShort(nDialogHeight / 1.3);
	si.nPos = 0;

	SetScrollInfo(SB_VERT, &si, TRUE);  // 수직 스크롤 설정
	SetScrollPos(SB_VERT, 0);        // 초기 스크롤 위치 설정
}

BOOL CTeachTab3GDesiccantMaterial::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void CTeachTab3GDesiccantMaterial::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

inline void CTeachTab3GDesiccantMaterial::EnableContionalCtrl(const CModelInfo::stDesiccantMaterialInfo& DesiccantMaterialInfo, BOOL bEnable)
{
	switch (DesiccantMaterialInfo.nDesiccantQuantity)
	{
	case 0:
		GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_X)->EnableWindow(!bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_Y)->EnableWindow(!bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_X)->EnableWindow(!bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_Y)->EnableWindow(!bEnable);
		break;
	case 1:
		GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_X)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_Y)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_X)->EnableWindow(!bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_Y)->EnableWindow(!bEnable);
		break;
	default:
		GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_X)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT1_PATTERN_POSI_Y)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_X)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_DESICCANT2_PATTERN_POSI_Y)->EnableWindow(bEnable);
		break;
	}

	switch (DesiccantMaterialInfo.nInspectionZoneCol)
	{
	case 1:
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO2)->EnableWindow(!bEnable);
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO3)->EnableWindow(!bEnable);
		break;
	case 2:
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO2)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO3)->EnableWindow(!bEnable);
		break;
	case 3:
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO2)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_INSPECTION_ZONE_RATIO3)->EnableWindow(bEnable);
		break;
	}
}

void CTeachTab3GDesiccantMaterial::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_3G);

	if (pCtrl)
	{
		CRect rc;
		pCtrl->GetWindowRect(&rc);
		ScreenToClient(&rc);

		int nLeftAreaEndX = rc.left + rc.Height(); // 좌측 정사각형
		int nRightAreaEndX = rc.right - (rc.Height()*3); // 우측 정사각형 * 3

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
				strParams.Format(_T("\"file:///%s#page=48\""), (LPCTSTR)strPdfPath);

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
