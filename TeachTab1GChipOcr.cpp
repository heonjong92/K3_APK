// TeachTab1GChipOcr.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab1GChipOcr.h"
#include "TeachOCRFontChip.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "TeachThresholdPreview.h"

#include "UIExt/ResourceManager.h"
#include "StringMacro.h"

#include <XUtil/xUtils.h>
#include <XGraphic\xGraphicObject.h>

// CTeachTab1GChipOcr 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab1GChipOcr, CDialog)

CTeachTab1GChipOcr::CTeachTab1GChipOcr(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab1GChipOcr::IDD, pParent)
	, m_bIsTeachChipROI(FALSE)
	, m_bIsTeachChipOcr2(FALSE)
	, m_bIsOcrROI(FALSE)
{
	m_pMainView = NULL;
	m_ChipOcr.Clear();
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab1GChipOcr::~CTeachTab1GChipOcr()
{
}

void CTeachTab1GChipOcr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_1G,							m_wndLabelTitle1G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,						m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_CHIPOCR,					m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_CHIPOCR,					m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_CHIPOCR_BYPASS,					m_ChipOcr.nUseBypass_ChipOcr);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_DESCRIPTION,				m_ChipOcr.strDescription);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_INFO,						m_ChipOcr.strChipOcrInfo);

	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_LED_LIGHT_VALUE,			m_ChipOcr.nValueCh1);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_LED_LIGHT_VALUE2,			m_ChipOcr.nValueCh2);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_LED_LIGHT_VALUE3,			m_ChipOcr.nValueCh3);

	DDX_Radio(pDX,		IDC_RADIO_CHIPOCR_BLACK_ON_WHITE,			m_ChipOcr.nFontColor);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_CHAR_WIDTH_MIN,			m_ChipOcr.nCharWidthMin);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_CHAR_WIDTH_MAX,			m_ChipOcr.nCharWidthMax);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_CHAR_HEIGHT_MIN,			m_ChipOcr.nCharHeightMin);
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_CHAR_HEIGHT_MAX,			m_ChipOcr.nCharHeightMax);
	
	DDX_Text(pDX,		IDC_EDIT_CHIPOCR_THRESHOLD,					m_ChipOcr.nThreshold);

	DDX_Control(pDX,	IDC_BTN_TEST_CHIPOCR,						m_btnTest_ChipOcr);
	DDX_Control(pDX,	IDC_BTN_CHIPOCR_RECIPE_NO,					m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_CHIPOCR,						m_btnSave);

	DDX_Control(pDX,	IDC_BTN_ROI_CHIPOCR,						m_btnTeachChipROI);
	DDX_Check(pDX,		IDC_BTN_ROI_CHIPOCR,						m_bIsTeachChipROI);

	DDX_Control(pDX,	IDC_BTN_ROI_CHIPOCR2,						m_btnTeachChipOcr2);
	DDX_Check(pDX,		IDC_BTN_ROI_CHIPOCR2,						m_bIsTeachChipOcr2);

	DDX_Control(pDX,	IDC_BTN_CHIPROI_OCR_FONT_TEACH,				m_btnOcrFontRegister);

	DDX_Check(pDX,		IDC_CHECK_CHIPOCR_REMOVE_BORDER,			m_ChipOcr.bRemoveBorder);
	DDX_Check(pDX,		IDC_CHECK_CHIPOCR_REMOVE_NARROW_OR_FLAT,	m_ChipOcr.bRemoveNarrowFlat);
	DDX_Check(pDX,		IDC_CHECK_CHIPOCR_CUT_LARGE_CHAR,			m_ChipOcr.bCutLargeChar);
	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_CHIPOCR_TEACH_MODEL2,	m_ctrlPicPreviewTeach2);
	DDX_Control(pDX,	IDC_BTN_ROI_CHIPOCR_OCRAREA,				m_btnOcrROI);
	DDX_Check(pDX,		IDC_BTN_ROI_CHIPOCR_OCRAREA,				m_bIsOcrROI);
}


BEGIN_MESSAGE_MAP(CTeachTab1GChipOcr, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_CHIPOCR_BYPASS,			&CTeachTab1GChipOcr::OnBnClickedRadioChipOcrBypass)
	ON_BN_CLICKED(IDC_RADIO_CHIPOCR_BYPASS2,		&CTeachTab1GChipOcr::OnBnClickedRadioChipOcrBypass)
	ON_BN_CLICKED(IDC_RADIO_CHIPOCR_BYPASS3,		&CTeachTab1GChipOcr::OnBnClickedRadioChipOcrBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_CHIPOCR,				&CTeachTab1GChipOcr::OnBnClickedBtnTestChipOcr)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_CHIPOCR,		&CTeachTab1GChipOcr::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_CHIPOCR,				&CTeachTab1GChipOcr::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CHIPOCR_RECIPE_NO,		&CTeachTab1GChipOcr::OnBnClickedBtnChipOcrRecipeNo)
	ON_BN_CLICKED(IDC_BTN_ROI_CHIPOCR,				&CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr)
	ON_BN_CLICKED(IDC_BTN_ROI_CHIPOCR2,				&CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr2)
	ON_BN_CLICKED(IDC_RADIO_CHIPOCR_BLACK_ON_WHITE, &CTeachTab1GChipOcr::OnBnClickedRadioChipocrBlackOnWhite)
	ON_BN_CLICKED(IDC_RADIO_CHIPOCR_WHITE_ON_BLACK, &CTeachTab1GChipOcr::OnBnClickedRadioChipocrBlackOnWhite)
	ON_BN_CLICKED(IDC_BTN_CHIPROI_OCR_FONT_TEACH,	&CTeachTab1GChipOcr::OnBnClickedBtnChiproiOcrFontTeach)
	ON_BN_CLICKED(IDC_BTN_ROI_CHIPOCR_OCRAREA,		&CTeachTab1GChipOcr::OnBnClickedBtnRoiChipocrOcrarea)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD,				&CTeachTab1GChipOcr::OnBnClickedBtnThreshold)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTab1GChipOcr 메시지 처리기입니다.
BOOL CTeachTab1GChipOcr::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle1G.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_ctrlPicPreviewTeach2.GetClientRect(m_rcPicCtrlSizeTeach2);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_ChipOcr.SetColor( DEF_BTN_COLOR_MEASURE );

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
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetViewerBodyColor();

	m_btnTeachChipROI.EnableWindow(TRUE);

	//
	m_ImgViewForTrayOutlineTeaching2.Create(&m_ImgViewManager2, this);
	m_ImgViewForTrayOutlineTeaching2.ShowWindow(SW_SHOW);
	m_ImgViewForTrayOutlineTeaching2.ShowTitleWindow(FALSE);
	m_ImgViewForTrayOutlineTeaching2.SetAnimateWindow(FALSE);

	m_ImgViewForTrayOutlineTeaching2.SetImageObject(&m_ImgObj2, FALSE);
	m_ImgViewForTrayOutlineTeaching2.SetRegisterCallBack(0, &regCB);

	m_ImgViewForTrayOutlineTeaching2.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImgViewForTrayOutlineTeaching2.SetBodyColor(dwBodyColor);
	m_ImgViewForTrayOutlineTeaching2.ShowStatusWindow(FALSE);
	m_ImgViewForTrayOutlineTeaching2.ShowScrollBar(FALSE);
	m_ImgViewForTrayOutlineTeaching2.ShowScaleBar(FALSE);
	m_ImgViewForTrayOutlineTeaching2.EnableMouseControl(FALSE);

	m_ImgViewForTrayOutlineTeaching2.EnableWindow(TRUE);
	m_btnTeachChipOcr2.EnableWindow(TRUE);

	LockWindowUpdate();

	HDWP hDWP = BeginDeferWindowPos(1);

	CRect reViewSize2;
	GetDlgItem(IDC_PICTURE_PREVIEW_CHIPOCR_TEACH_MODEL2)->GetWindowRect(reViewSize2);
	ScreenToClient(reViewSize2);

	hDWP = DeferWindowPos(hDWP, m_ImgViewForTrayOutlineTeaching2.GetSafeHwnd(), NULL, reViewSize2.left, reViewSize2.top, reViewSize2.Width(), reViewSize2.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	UpdateUI();
	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab1GChipOcr::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_1G						)->SetWindowText(CLanguageInfo::Instance()->ReadString(91));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(92));
	GetDlgItem(IDC_BTN_CHIPOCR_RECIPE_NO				)->SetWindowText(CLanguageInfo::Instance()->ReadString(93));
	GetDlgItem(IDC_RADIO_CHIPOCR_BYPASS					)->SetWindowText(CLanguageInfo::Instance()->ReadString(94));
	GetDlgItem(IDC_RADIO_CHIPOCR_BYPASS2				)->SetWindowText(CLanguageInfo::Instance()->ReadString(95));
	GetDlgItem(IDC_RADIO_CHIPOCR_BYPASS3				)->SetWindowText(CLanguageInfo::Instance()->ReadString(96));
	GetDlgItem(IDC_STATIC_DESCRIPTION					)->SetWindowText(CLanguageInfo::Instance()->ReadString(97));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(98));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE2					)->SetWindowText(CLanguageInfo::Instance()->ReadString(99));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE3					)->SetWindowText(CLanguageInfo::Instance()->ReadString(100));
	GetDlgItem(IDC_BTN_ROI_CHIPOCR						)->SetWindowText(CLanguageInfo::Instance()->ReadString(101));
	GetDlgItem(IDC_BTN_TEST_CHIPOCR						)->SetWindowText(CLanguageInfo::Instance()->ReadString(102));
	GetDlgItem(IDC_STATIC_CHIPOCR_FONT_COLOR			)->SetWindowText(CLanguageInfo::Instance()->ReadString(108));
	GetDlgItem(IDC_STATIC_CHIPOCR_CHAR_WIDTH			)->SetWindowText(CLanguageInfo::Instance()->ReadString(109));
	GetDlgItem(IDC_STATIC_CHIPOCR_CHAR_HEIGHT			)->SetWindowText(CLanguageInfo::Instance()->ReadString(110));
	GetDlgItem(IDC_BTN_CHIPROI_OCR_FONT_TEACH			)->SetWindowText(CLanguageInfo::Instance()->ReadString(111));
	GetDlgItem(IDC_STATIC_CHIPOCR_INFO					)->SetWindowText(CLanguageInfo::Instance()->ReadString(112));
	GetDlgItem(IDC_BTN_ROI_CHIPOCR2						)->SetWindowText(CLanguageInfo::Instance()->ReadString(113));

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab1GChipOcr::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPOCR_INFO), _T("OCR로 인식된 텍스트와 비교할 문자열입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPOCR_CHAR_WIDTH_MIN), _T("OCR로 읽을 글자 너비의 최소값을 입력합니다. 해당 값 범위 이내의 글자만 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPOCR_CHAR_WIDTH_MAX), _T("OCR로 읽을 글자 너비의 최대값을 입력합니다. 해당 값 범위 이내의 글자만 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPOCR_CHAR_HEIGHT_MIN), _T("OCR로 읽을 글자 높이의 최소값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPOCR_CHAR_HEIGHT_MAX), _T("OCR로 읽을 글자 높이의 최대값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CHIPOCR_THRESHOLD), _T("OCR 검사하기 위해 영상 이진화할 때 사용하는 Threshold 값 입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_CHIPOCR_BLACK_ON_WHITE), _T("글자의 gray value값이 배경보다 낮으면 ‘Black On White’, 높으면 ‘White On Black’을 선택합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_CHIPOCR_WHITE_ON_BLACK), _T("글자의 gray value값이 배경보다 낮으면 ‘Black On White’, 높으면 ‘White On Black’을 선택합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_CHIPOCR_REMOVE_BORDER), _T("OCR 영역의 가장자리에 맞닿는 글자를 인식하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_CHIPOCR_REMOVE_NARROW_OR_FLAT), _T("|이나 -(하이픈)과 같은 글자를 인식하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_CHIPOCR_CUT_LARGE_CHAR), _T("사이즈가 큰 글자를 적절한 크기로 잘라서 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_CHIPOCR_OCRAREA), _T("OCR을 수행할 영역을 설정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_CHIPOCR2), _T("OCR을 수행할 영역의 기준을 잡는데 필요한 Chip 내부의 Align 모델을 등록하는 버튼입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_CHIPOCR), _T("OCR을 수행할 때 Chip이 위치하는 영역을 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_CHIPROI_OCR_FONT_TEACH), _T("OCR에 사용되는 폰트를 등록하는 창이 열립니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab1GChipOcr::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab1GChipOcr::Save()
{
	UpdateData(TRUE);

	CModelInfo::stChipOcr& stChipOcr = CModelInfo::Instance()->GetChipOcr();

	// Validation Check
	if (m_ChipOcr.nCharWidthMin > m_ChipOcr.nCharWidthMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("글자 너비 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum character width must be less than or equal to the maximum value."), MB_ICONERROR);
		
		m_ChipOcr = stChipOcr;
		UpdateData(FALSE);

		return FALSE;
	}
	if (m_ChipOcr.nCharHeightMin > m_ChipOcr.nCharHeightMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("글자 높이 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum character height must be less than or equal to the maximum value."), MB_ICONERROR);

		m_ChipOcr = stChipOcr;
		UpdateData(FALSE);

		return FALSE;
	}

	CheckData();

	stChipOcr = m_ChipOcr;

#ifdef RELEASE_1G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameChipOcr(), TEACH_TAB_IDX_CHIPOCR);
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_CHIPOCR );
#endif

	Refresh();
	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab1GChipOcr::CheckData()
{
#ifdef RELEASE_1G
	CString strLog;
	CModelInfo::stChipOcr& ChipOcr = CModelInfo::Instance()->GetChipOcr();

	// ----- ChipOcr Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[ChipOcr.nUseBypass_ChipOcr], strBypassName[m_ChipOcr.nUseBypass_ChipOcr] );
	if( ChipOcr.nUseBypass_ChipOcr != m_ChipOcr.nUseBypass_ChipOcr) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeChipOcr,	strLog );
	strLog.Format(_T("[Description][%s→%s]"), ChipOcr.strDescription, m_ChipOcr.strDescription);
	if (ChipOcr.strDescription != m_ChipOcr.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[ChipOCRInfo][%s→%s]"), ChipOcr.strChipOcrInfo, m_ChipOcr.strChipOcrInfo);
	if (ChipOcr.strChipOcrInfo != m_ChipOcr.strChipOcrInfo) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);

	strLog.Format(_T("[LightValueCh1][%d→%d]"), ChipOcr.nValueCh1, m_ChipOcr.nValueCh1);
	if (ChipOcr.nValueCh1 != m_ChipOcr.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[LightValueCh2][%d→%d]"), ChipOcr.nValueCh2, m_ChipOcr.nValueCh2);
	if (ChipOcr.nValueCh2 != m_ChipOcr.nValueCh2) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[LightValueCh3][%d→%d]"), ChipOcr.nValueCh3, m_ChipOcr.nValueCh3);
	if (ChipOcr.nValueCh3 != m_ChipOcr.nValueCh3) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);

	strLog.Format(_T("[OcrAreaDistanceX][%d→%d]"), ChipOcr.nOcrAreaDistanceX, m_ChipOcr.nOcrAreaDistanceX);
	if (ChipOcr.nOcrAreaDistanceX != m_ChipOcr.nOcrAreaDistanceX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[OcrAreaDistanceY][%d→%d]"), ChipOcr.nOcrAreaDistanceY, m_ChipOcr.nOcrAreaDistanceY);
	if (ChipOcr.nOcrAreaDistanceY != m_ChipOcr.nOcrAreaDistanceY) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);

	strLog.Format(_T("[Font Color][%d→%d]"), ChipOcr.nFontColor, m_ChipOcr.nFontColor);
	if (ChipOcr.nFontColor != m_ChipOcr.nFontColor) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);

	strLog.Format(_T("[CharWidthMin][%d→%d]"), ChipOcr.nCharWidthMin, m_ChipOcr.nCharWidthMin);
	if (ChipOcr.nCharWidthMin != m_ChipOcr.nCharWidthMin) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[CharWidthMax][%d→%d]"), ChipOcr.nCharWidthMax, m_ChipOcr.nCharWidthMax);
	if (ChipOcr.nCharWidthMax != m_ChipOcr.nCharWidthMax) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[CharHeightMin][%d→%d]"), ChipOcr.nCharHeightMin, m_ChipOcr.nCharHeightMin);
	if (ChipOcr.nCharHeightMin != m_ChipOcr.nCharHeightMin) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
	strLog.Format(_T("[CharHeightMax][%d→%d]"), ChipOcr.nCharHeightMax, m_ChipOcr.nCharHeightMax);
	if (ChipOcr.nCharHeightMax != m_ChipOcr.nCharHeightMax) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);

	strLog.Format(_T("[Remove Border][%d→%d]"), ChipOcr.bRemoveBorder, m_ChipOcr.bRemoveBorder);
	if (ChipOcr.bRemoveBorder != m_ChipOcr.bRemoveBorder) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);
	strLog.Format(_T("[Remove Narrow or Flat][%d→%d]"), ChipOcr.bRemoveNarrowFlat, m_ChipOcr.bRemoveNarrowFlat);
	if (ChipOcr.bRemoveNarrowFlat != m_ChipOcr.bRemoveNarrowFlat) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);
	strLog.Format(_T("[Cut Large Char][%d→%d]"), ChipOcr.bCutLargeChar, m_ChipOcr.bCutLargeChar);
	if (ChipOcr.bCutLargeChar != m_ChipOcr.bCutLargeChar) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Threshold][%d→%d]"), ChipOcr.nThreshold, m_ChipOcr.nThreshold);
	if (ChipOcr.nThreshold != m_ChipOcr.nThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);
#endif
}

void CTeachTab1GChipOcr::Refresh()
{
	UpdateRecipeList();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(CModelInfo::Instance()->GetModelNameChipOcr(), CHIPOCR_KIND);

	CModelInfo::stChipOcr& ChipOcr = CModelInfo::Instance()->GetChipOcr();
	m_ChipOcr = ChipOcr;

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab1GChipOcr::UpdateUI()
{
	UpdateTeachingImage();
}

void CTeachTab1GChipOcr::Default()
{
	m_ChipOcr.Clear();

	UpdateData(FALSE);
}

void CTeachTab1GChipOcr::Cleanup()
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

	m_bIsOcrROI = FALSE;
	m_bIsTeachChipROI = FALSE;
	m_bIsTeachChipOcr2 = FALSE;
}

HBRUSH CTeachTab1GChipOcr::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
	case IDC_STATIC_CHIPOCR_FONT_COLOR:
	case IDC_STATIC_CHIPOCR_CHAR_WIDTH:
	case IDC_STATIC_CHIPOCR_CHAR_HEIGHT:
	case IDC_STATIC_CHIPOCR_INFO:
	case IDC_CHECK_CHIPOCR_REMOVE_BORDER:
	case IDC_CHECK_CHIPOCR_REMOVE_NARROW_OR_FLAT:
	case IDC_CHECK_CHIPOCR_CUT_LARGE_CHAR:
	case IDC_STATIC_CHIPOCR_THRESHOLD:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab1GChipOcr::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab1GChipOcr::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab1GChipOcr::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab1GChipOcr::OnBnClickedRadioChipOcrBypass()
{
	UpdateData( TRUE );
}

void CTeachTab1GChipOcr::OnBnClickedBtnTestChipOcr()
{
#ifdef RELEASE_1G
	WRITE_LOG( WL_BTN, _T("CTeachTab1GChipOcr::OnBnClickedBtnTestChipOcr") );

	CString strModelName = CModelInfo::Instance()->GetModelNameChipOcr();
	if (strModelName == _T("CALIBRATION"))
		CVisionSystem::Instance()->m_bAcm[ACM_CHIP] = TRUE;

	CVisionSystem::Instance()->BeginStopWatch(IDX_AREA4);
	CVisionSystem::Instance()->StartInspection(InspectTypeChipOcr, IDX_AREA4, 0, TRUE);
#endif
}

void CTeachTab1GChipOcr::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(CHIPOCR_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_CHIPOCR);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameChipOcr();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameChipOcr();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameChipOcr();
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

void CTeachTab1GChipOcr::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_ChipOcr\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameChipOcr();
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


void CTeachTab1GChipOcr::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChipOcr::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameChipOcr();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, CHIPOCR_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, CHIPOCR_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnSave :: End"));
}

void CTeachTab1GChipOcr::DisableWnd(BOOL bEnable)
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


void CTeachTab1GChipOcr::OnBnClickedBtnChipOcrRecipeNo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnChipOcrRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnChipOcrRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( CHIPOCR_KIND, TEACH_TAB_IDX_CHIPOCR );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 1G ChipOcr") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnChipOcrRecipeNo :: End"));
#endif
}

void CTeachTab1GChipOcr::UpdateTeachingImage()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return;

#ifdef RELEASE_1G
	USES_CONVERSION;

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA4, 0);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if (MatchBuff->GetPatternLearnt())
	{
		int nWidth = MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImgObj2.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		//strLastRecipe = CModelInfo::Instance()->GetLastModelNameChipOcr();		
		strLastRecipe = CModelInfo::Instance()->GetModelNameChipOcr();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_ChipOcr\\") + strLastRecipe + _T("\\");
		strInspName = _T("ChipOcr");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, 0);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImgObj2.LoadFromFile(strMatchBmpFileName);
		else
			m_ImgObj2.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImgObj2.LoadFromFile(strImgPath);
	}

	m_ImgViewForTrayOutlineTeaching2.ImageUpdate();

	m_pMainView->UpdateImageView();

#endif
}

void CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_1G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

	if (m_bIsTeachChipROI)
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

		m_btnTeachChipROI.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("검사할 Chip의 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the area of the chip to be inspected"));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA4, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr2()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GChipOcr::OnBnClickedBtnRoiChipOcr2"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_1G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

	if (m_bIsTeachChipOcr2)
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

		m_btnTeachChipOcr2.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("OCR 검사 위치를 보정하기 위한 Fiducial Mark의 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the area of the fiducial mark to correct the OCR inspection position"));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA4, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab1GChipOcr::OnBnClickedBtnRoiChipocrOcrarea()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GTrayOcr::OnBnClickedBtnRoiTrayOcr"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	if (m_ChipOcr.rcInspInside.IsRectEmpty())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("Chip Inside Align 모델 티칭을 먼저 진행해주세요."), MB_ICONERROR);
		else												AfxMessageBox(_T("Please perform the teaching of the Chip Inside Align model first."), MB_ICONERROR);
		
		m_bIsOcrROI = FALSE;
		m_btnOcrROI.SetCheck(BST_UNCHECKED);
		return;
	}

#ifdef RELEASE_1G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

	if (m_bIsOcrROI)
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

		m_btnOcrROI.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("검사할 OCR 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the OCR area to be inspected."));
		
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA4, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void APIENTRY CTeachTab1GChipOcr::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab1GChipOcr* pThis = (CTeachTab1GChipOcr*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab1GChipOcr::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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

#ifdef RELEASE_1G

	BOOL bRet = FALSE;

	if (m_bIsTeachChipROI)
	{
		m_ChipOcr.rcInspArea = rcTrackRegion;

		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_ChipOcr.rcInspArea);
		pGO->AddDrawBox(clrBox);

		bRet = TRUE;
	}

	if (m_bIsTeachChipOcr2)
	{
		m_ChipOcr.rcInspInside = rcTrackRegion;

		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_ChipOcr.rcInspInside);
		pGO->AddDrawBox(clrBox);
		bRet = pInspectionVision->SetLearnModel(pGO, pImgObj, m_ChipOcr.rcInspInside, InspectTypeChipOcr, nViewIndex, 0, FALSE, FALSE);
		//bRet = TRUE;
	}

	if (m_bIsOcrROI)
	{
		m_ChipOcr.rcOcrROI = rcTrackRegion;

		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_ChipOcr.rcOcrROI);
		pGO->AddDrawBox(clrBox);

		m_ChipOcr.nOcrAreaDistanceX = m_ChipOcr.rcInspInside.left - m_ChipOcr.rcOcrROI.left;
		m_ChipOcr.nOcrAreaDistanceY = m_ChipOcr.rcInspInside.top - m_ChipOcr.rcOcrROI.top ;

		bRet = TRUE;
	}

	if (bRet)
	{
		Cleanup();
		UpdateData(FALSE);

		Save();
	}
#endif
}

void CTeachTab1GChipOcr::OnBnClickedRadioChipocrBlackOnWhite()
{
	UpdateData(TRUE);
}

void CTeachTab1GChipOcr::OnBnClickedBtnChiproiOcrFontTeach()
{
	TeachOCRFontChip FontChipTeach;
	if (FontChipTeach.DoModal() == IDOK) {}	
}

void CTeachTab1GChipOcr::OnBnClickedBtnThreshold()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnThreshold :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnThreshold :: Error - RunStatusAutoRun!!"));
		return;
	}

	CTeachThresholdPreview ThresholdPreview(this);
	if (ThresholdPreview.DoModal() == IDOK)
	{
		Refresh();
	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GChipOcr::OnBnClickedBtnThreshold :: End"));
#endif
}

void CTeachTab1GChipOcr::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_1G);

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
				strParams.Format(_T("\"file:///%s#page=32\""), (LPCTSTR)strPdfPath);

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
