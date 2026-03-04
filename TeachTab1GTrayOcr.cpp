// TeachTab1GTrayOcr.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab1GTrayOcr.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"
#include "TeachOCRFontTray.h"
#include "TeachAddFont.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "InspectionVision.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "TrayRecipeInfoDlg.h"

#include "UIExt/ResourceManager.h"
#include "StringMacro.h"

#include <XUtil/xUtils.h>
#include <XGraphic\xGraphicObject.h>

// CTeachTab1GTrayOcr 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab1GTrayOcr, CDialog)

CTeachTab1GTrayOcr::CTeachTab1GTrayOcr(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab1GTrayOcr::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsTeachTabBegin(FALSE)
	, m_bIsOcrFontRegister(FALSE)
	, m_bIsTeachOcrROI(FALSE)
	, m_bOCRPreview(FALSE)
	, m_bIsTrayNonInsp(FALSE)
{
	m_pMainView = NULL;
	m_TrayOcr.Clear();

	m_nCombo_Tray_Masking_Select = 0;
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab1GTrayOcr::~CTeachTab1GTrayOcr()
{
}

void CTeachTab1GTrayOcr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX,	IDC_LABEL_TITLE_1G_TRAYOCR,					m_wndLabelTitle1GTrayOcr);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,						m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_TRAYOCR,					m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_TRAYOCR,					m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_TRAYOCR_BYPASS,					m_TrayOcr.nUseBypass_TrayOcr);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_DESCRIPTION,				m_TrayOcr.strDescription);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_INFO,						m_TrayOcr.strTrayOcrInfo);

	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_WIDTH_MIN,					m_TrayOcr.nCharWidthMin);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_WIDTH_MAX,					m_TrayOcr.nCharWidthMax);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_HEIGHT_MIN,				m_TrayOcr.nCharHeightMin);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_HEIGHT_MAX,				m_TrayOcr.nCharHeightMax);

	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_PAGECOUNT,					m_TrayOcr.lPageCount);

	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_TEACH_MODEL,			m_ctrlPicPreviewTeachTab);

	DDX_Control(pDX,	IDC_BTN_TAB_ALIGN_TEACH_MODEL,				m_btnTabAlignTeachModel);
	DDX_Check(pDX,		IDC_BTN_TAB_ALIGN_TEACH_MODEL,				m_bIsTeachTabBegin);

	DDX_Control(pDX,	IDC_BTN_TEST_TRAYOCR,						m_btnTest_TrayOcr);
	DDX_Control(pDX,	IDC_BTN_TRAYOCR_RECIPE_NO,					m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_TRAYOCR_RECIPE_INFO,				m_btnSaveRecipeInfo);
	DDX_Control(pDX,	IDC_BTN_SAVE_TRAYOCR,						m_btnSave);
	DDX_Control(pDX,	IDC_BTN_TEST_TRAYOCR_FONT_REGISTER,			m_btnOcrFontRegister);
	DDX_Check(pDX,		IDC_BTN_ROI_TRAYOCR,						m_bIsTeachOcrROI);
	DDX_Control(pDX,	IDC_BTN_ROI_TRAYOCR,						m_btnROI);

	DDX_Check(pDX,		IDC_CHECK_TRAYOCR_REMOVE_BORDER,			m_TrayOcr.bRemoveBorder);
	DDX_Check(pDX,		IDC_CHECK_TRAYOCR_REMOVE_NARROW_OR_FLAT,	m_TrayOcr.bRemoveNarrowFlat);
	DDX_Check(pDX,		IDC_CHECK_TRAYOCR_CUT_LARGE_CHARACTERS,		m_TrayOcr.bCutLargeChar);
	DDX_Check(pDX,		IDC_CHECK_TRAYOCR_ROTATE_IMAGE,				m_TrayOcr.bRotateImage);

	DDX_Check(pDX,		IDC_CHECK_TRAYOCR_USE_ADAPTIVE_THRESHOLD,	m_TrayOcr.bUseAdaptiveThreshold);
	DDX_Control(pDX,	IDC_COMBO_TRAYOCR_METHOD,					m_wndMethod);
	DDX_CBIndex(pDX,	IDC_COMBO_TRAYOCR_METHOD,					m_TrayOcr.nMethod);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_HALF_KERNEL_SIZE,			m_TrayOcr.nHalfKernelSize);
	DDX_Text(pDX,		IDC_EDIT_TRAYOCR_THRESHOLD_OFFSET,			m_TrayOcr.nThresholdOffset);
	DDX_Control(pDX,	IDC_BTN_OCR_PREVIEW,						m_btnOcrPreview);
	DDX_Check(pDX,		IDC_BTN_OCR_PREVIEW,						m_bOCRPreview);

	DDX_Check(pDX,		IDC_BTN_TAB_NONINSP_TEACHING,				m_bIsTrayNonInsp);
	DDX_Control(pDX,	IDC_BTN_TAB_NONINSP_TEACHING,				m_btnTrayNonInspTeachModel);

	DDX_Control(pDX,	IDC_COMBO_TRAY_NON_INSP_MODEL_NUM,			m_Combo_Tray_Select_Masking_Num);
	DDX_CBIndex(pDX,	IDC_COMBO_TRAY_NON_INSP_MODEL_NUM,			m_nCombo_Tray_Masking_Select);
}

BEGIN_MESSAGE_MAP(CTeachTab1GTrayOcr, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_TRAYOCR_BYPASS,						&CTeachTab1GTrayOcr::OnBnClickedRadioTrayOcrBypass)
	ON_BN_CLICKED(IDC_RADIO_TRAYOCR_BYPASS2,					&CTeachTab1GTrayOcr::OnBnClickedRadioTrayOcrBypass)
	ON_BN_CLICKED(IDC_RADIO_TRAYOCR_BYPASS3,					&CTeachTab1GTrayOcr::OnBnClickedRadioTrayOcrBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_TRAYOCR,							&CTeachTab1GTrayOcr::OnBnClickedBtnTestTrayOcr)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_TRAYOCR,					&CTeachTab1GTrayOcr::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_TRAYOCR, 						&CTeachTab1GTrayOcr::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_TRAYOCR_RECIPE_NO,					&CTeachTab1GTrayOcr::OnBnClickedBtnTrayOcrRecipeNo)
	ON_BN_CLICKED(IDC_BTN_TRAYOCR_RECIPE_INFO,					&CTeachTab1GTrayOcr::OnBnClickedBtnTrayocrRecipeInfo)
	ON_BN_CLICKED(IDC_BTN_TEST_TRAYOCR_FONT_REGISTER,			&CTeachTab1GTrayOcr::OnBnClickedBtnTestTrayocrFontRegister)
	ON_BN_CLICKED(IDC_BTN_ROI_TRAYOCR,							&CTeachTab1GTrayOcr::OnBnClickedBtnRoiTrayocr)
	ON_BN_CLICKED(IDC_BTN_TAB_ALIGN_TEACH_MODEL,				&CTeachTab1GTrayOcr::OnBnClickedBtnTabAlignTeachModel)
	ON_BN_CLICKED(IDC_CHECK_TRAYOCR_USE_ADAPTIVE_THRESHOLD,		&CTeachTab1GTrayOcr::OnBnClickedCheckTrayocrUseAdaptiveThreshold)
	ON_BN_CLICKED(IDC_BTN_OCR_PREVIEW,							&CTeachTab1GTrayOcr::OnBnClickedBtnOcrPreview)
	ON_CBN_SELCHANGE(IDC_COMBO_TRAYOCR_METHOD,					&CTeachTab1GTrayOcr::OnCbnSelchangeComboTrayocrMethod)
	ON_BN_CLICKED(IDC_BTN_TAB_NONINSP_TEACHING,					&CTeachTab1GTrayOcr::OnBnClickedBtnTabNoninspTeaching)
	ON_CBN_SELCHANGE(IDC_COMBO_TRAY_NON_INSP_MODEL_NUM,			&CTeachTab1GTrayOcr::OnCbnSelchangeComboTrayNonInspModelNum)
	ON_BN_CLICKED(IDC_BTN_TAB_NONINSP_TEACHING_DELETE,			&CTeachTab1GTrayOcr::OnBnClickedBtnTabNoninspTeachingDelete)
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab1GTrayOcr 메시지 처리기입니다.
BOOL CTeachTab1GTrayOcr::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1GTrayOcr.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle1GTrayOcr.SetFontHeight( 12 );

	m_ctrlPicPreviewTeachTab.GetClientRect(m_rcPicCtrlSizeTeachTab);

	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_TrayOcr.SetColor(DEF_BTN_COLOR_MEASURE);
	m_btnTrayNonInspTeachModel.SetColor(DEF_BTN_COLOR_TEACING);

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	m_btnOcrFontRegister.SetCheck(BST_UNCHECKED);

	// Matching Viewer
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetViewerBodyColor();

	m_ImageViewForTabTeaching.Create(&m_ImageViewManagerForTabTeaching, this);
	m_ImageViewForTabTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForTabTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForTabTeaching.SetAnimateWindow(FALSE);

	m_ImageViewForTabTeaching.SetImageObject(&m_ImageObjectForTabTeaching, FALSE);
	m_ImageViewForTabTeaching.SetRegisterCallBack(0, &regCB);

	m_ImageViewForTabTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForTabTeaching.SetBodyColor(dwBodyColor);
	m_ImageViewForTabTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForTabTeaching.ShowScrollBar(FALSE);
	m_ImageViewForTabTeaching.ShowScaleBar(FALSE);
	m_ImageViewForTabTeaching.EnableMouseControl(FALSE);

	m_ImageViewForTabTeaching.EnableWindow(TRUE);
	m_btnTabAlignTeachModel.EnableWindow(TRUE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_TAB)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(0);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForTabTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab1GTrayOcr::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_1G_TRAYOCR			)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_TRAYOCR_RECIPE_NO			)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_RADIO_TRAYOCR_BYPASS				)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_TRAYOCR_BYPASS2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_TRAYOCR_BYPASS3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_STATIC_DESCRIPTION				)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_PAGECOUNT					)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_BTN_TAB_ALIGH_TEACH_MODEL		)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_BTN_TEST_TRAYOCR					)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_TRAYOCR_WIDTH				)->SetWindowText(CLanguageInfo::Instance()->ReadString(23));
	GetDlgItem(IDC_STATIC_TRAYOCR_HEIGHT			)->SetWindowText(CLanguageInfo::Instance()->ReadString(24));
	GetDlgItem(IDC_BTN_ROI_TRAYOCR					)->SetWindowText(CLanguageInfo::Instance()->ReadString(25));
	GetDlgItem(IDC_BTN_TEST_TRAYOCR_FONT_REGISTER	)->SetWindowText(CLanguageInfo::Instance()->ReadString(26));
	GetDlgItem(IDC_STATIC_TRAYOCR_INFO				)->SetWindowText(CLanguageInfo::Instance()->ReadString(27));


	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1GTrayOcr.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab1GTrayOcr::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.SetMaxTipWidth(300);  //멀티라인 활성화
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_INFO), _T("OCR로 인식된 텍스트와 비교할 문자열 입니다.두 개가 같으면 OK, 다르면 NG로 판정합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_PAGECOUNT), _T("Line Scan Camera로 읽는 범위이며, 이미지의 폭(Width)이 됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_WIDTH_MIN), _T("OCR로 읽을 글자 너비의 최소값을 입력합니다.해당 값 범위 이내의 글자만 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_WIDTH_MAX), _T("OCR로 읽을 글자 너비의 최대값을 입력합니다.해당 값 범위 이내의 글자만 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_HEIGHT_MIN), _T("OCR로 읽을 글자 높이의 최소값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_HEIGHT_MAX), _T("OCR로 읽을 글자 높이의 최대값을 입력합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_TRAYOCR_REMOVE_BORDER), _T("OCR 영역의 가장자리에 맞닿는 글자를 인식하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_TRAYOCR_REMOVE_NARROW_OR_FLAT), _T("|이나 -(하이픈)과 같은 글자를 인식하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_TRAYOCR_CUT_LARGE_CHARACTERS), _T("사이즈가 큰 글자를 적절한 크기로 잘라서 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_TRAYOCR_USE_ADAPTIVE_THRESHOLD), _T("글자 사이의 간격이 좁아 글자를 제대로 인식하지 못하는 경우 사용합니다."));

	CString strMethod;
	strMethod = _T("Mean : 주변 픽셀들의 “평균값(Mean)”을 임계값으로 사용\r\n");
	strMethod += _T(" - 가장 일반적인 방식, 밝기 변화가 완만한 이미지는 잘됨, 노이즈가 많으면 평균이 흔들릴 수 있음\r\n");
	strMethod += _T("Median : 주변 픽셀들의 “중앙값(Median)”을 임계값으로 사용\r\n");
	strMethod += _T(" - 노이즈에 강함 (특히 Salt & Pepper noise), 평균보다 안정적, 값이 극단적으로 튀는 픽셀이 있어도 영향 적음\r\n");
	strMethod += _T("Middle : 주변 픽셀 값의 “최소값과 최대값의 중간값”을 사용\r\n");
	strMethod += _T(" - 밝기 대비가 큰 경우 유리, 평균 / 중앙값과 다르게 범위(contrast)만 보고 판단, 명암이 강한 이미지에서 객체 / 배경이 잘 나뉠 수 있음, 하지만 노이즈가 min 또는 max 를 잡아버리면 불안정함");

	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_TRAYOCR_METHOD), strMethod);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_HALF_KERNEL_SIZE), _T("Convolution연산을 수행할 Kernel의 크기를 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAYOCR_THRESHOLD_OFFSET), _T("설정한 ‘Method’에 Offset으로 주는 값입니다.‘이미지 미리보기’ 기능을 통해 변환된 이미지를 확인해가며 최적의 값을 입력해야합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_BTN_OCR_PREVIEW), _T("설정된 adaptive threshold 파라미터로 OCR을 할 때의 이미지를 확인할 수 있습니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_TRAYOCR), _T("누르면 OCR 검사를 수행할 영역을 Viewer에서 설정할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_TEST_TRAYOCR_FONT_REGISTER), _T("OCR에 사용 되는 폰트를 등록하는 창이 열립니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_TRAYOCR_ROTATE_IMAGE), _T("Tab부분 글자가 회전 되어 있는 Tray의 경우, 박스를 체크한 뒤 저장하면 Line Scan Camera로 이미지를 Grab할 때 Viewer에 이미지가 회전되어 나타납니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_TAB_ALIGN_TEACH_MODEL), _T("이미지에서 Tab을 찾을 때 비교할 모델을 등록합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_TAB_NONINSP_TEACHING), _T("비검사 영역을 설정합니다. ex) @ 마크"));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab1GTrayOcr::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab1GTrayOcr::Save()
{
	UpdateData(TRUE);
	
	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();

	// Validation Check
	if (m_TrayOcr.nCharWidthMin > m_TrayOcr.nCharWidthMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("글자 너비 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum character width must be less than or equal to the maximum value."), MB_ICONERROR);
		
		m_TrayOcr = stTrayOcr;
		UpdateData(FALSE);

		return FALSE;
	}
	if (m_TrayOcr.nCharHeightMin > m_TrayOcr.nCharHeightMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("글자 높이 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum character height must be less than or equal to the maximum value."), MB_ICONERROR);

		m_TrayOcr = stTrayOcr;
		UpdateData(FALSE);

		return FALSE;
	}

	CheckData();

	stTrayOcr = m_TrayOcr;

#ifdef RELEASE_1G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameTrayOcr(), TEACH_TAB_IDX_TRAYOCR);
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_TRAYOCR );
#endif

	UpdateData(FALSE);

	UpdateUI();

	return TRUE;
}

void CTeachTab1GTrayOcr::CheckData()
{
#ifdef RELEASE_1G
	CString strLog;
	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();

	// ----- TrayOcr Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[stTrayOcr.nUseBypass_TrayOcr], strBypassName[m_TrayOcr.nUseBypass_TrayOcr] );
	if( stTrayOcr.nUseBypass_TrayOcr != m_TrayOcr.nUseBypass_TrayOcr) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeTrayOcr, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), stTrayOcr.strDescription, m_TrayOcr.strDescription);
	if (stTrayOcr.strDescription != m_TrayOcr.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[TrayOcrInfo][%s→%s]"), stTrayOcr.strTrayOcrInfo, m_TrayOcr.strTrayOcrInfo);
	if (stTrayOcr.strTrayOcrInfo != m_TrayOcr.strTrayOcrInfo) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[CharacterWidthMin][%d→%d]"), stTrayOcr.nCharWidthMin, m_TrayOcr.nCharWidthMin);
	if (stTrayOcr.nCharWidthMin != m_TrayOcr.nCharWidthMin) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[CharacterWidthMax][%d→%d]"), stTrayOcr.nCharWidthMax, m_TrayOcr.nCharWidthMax);
	if (stTrayOcr.nCharWidthMax != m_TrayOcr.nCharWidthMax) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[CharacterHeightMin][%d→%d]"), stTrayOcr.nCharHeightMin, m_TrayOcr.nCharHeightMin);
	if (stTrayOcr.nCharHeightMin != m_TrayOcr.nCharHeightMin) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[CharacterHeightMax][%d→%d]"), stTrayOcr.nCharHeightMax, m_TrayOcr.nCharHeightMax);
	if (stTrayOcr.nCharHeightMax != m_TrayOcr.nCharHeightMax) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[OcrAreaDistanceX][%d→%d]"), stTrayOcr.nOcrAreaDistX, m_TrayOcr.nOcrAreaDistX);
	if (stTrayOcr.nOcrAreaDistX != m_TrayOcr.nOcrAreaDistX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[OcrAreaDistanceY][%d→%d]"), stTrayOcr.nOcrAreaDistY, m_TrayOcr.nOcrAreaDistY);
	if (stTrayOcr.nOcrAreaDistY != m_TrayOcr.nOcrAreaDistY) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[PageCount][%ld→%ld]"), stTrayOcr.lPageCount, m_TrayOcr.lPageCount);
	if (stTrayOcr.lPageCount != m_TrayOcr.lPageCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Remove Border][%d→%d]"), stTrayOcr.bRemoveBorder, m_TrayOcr.bRemoveBorder);
	if (stTrayOcr.bRemoveBorder != m_TrayOcr.bRemoveBorder) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Remove Narrow or Flat][%d→%d]"), stTrayOcr.bRemoveNarrowFlat, m_TrayOcr.bRemoveNarrowFlat);
	if (stTrayOcr.bRemoveNarrowFlat != m_TrayOcr.bRemoveNarrowFlat) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Cut Large Char][%d→%d]"), stTrayOcr.bCutLargeChar, m_TrayOcr.bCutLargeChar);
	if (stTrayOcr.bCutLargeChar != m_TrayOcr.bCutLargeChar) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Use Adaptive Threshold][%d→%d]"), stTrayOcr.bUseAdaptiveThreshold, m_TrayOcr.bUseAdaptiveThreshold);
	if (stTrayOcr.bUseAdaptiveThreshold != m_TrayOcr.bUseAdaptiveThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);
	
	strLog.Format(_T("[Rotate Image][%d→%d]"), stTrayOcr.bRotateImage, m_TrayOcr.bRotateImage);
	if (stTrayOcr.bRotateImage != m_TrayOcr.bRotateImage) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Method][%d→%d]"), stTrayOcr.nMethod, m_TrayOcr.nMethod);
	if (stTrayOcr.nMethod != m_TrayOcr.nMethod) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Half Kernel Size][%d→%d]"), stTrayOcr.nHalfKernelSize, m_TrayOcr.nHalfKernelSize);
	if (stTrayOcr.nHalfKernelSize != m_TrayOcr.nHalfKernelSize) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);

	strLog.Format(_T("[Threshold Offset][%d→%d]"), stTrayOcr.nThresholdOffset, m_TrayOcr.nThresholdOffset);
	if (stTrayOcr.nThresholdOffset != m_TrayOcr.nThresholdOffset) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTrayOcr, strLog);
#endif
}

void CTeachTab1GTrayOcr::Refresh()
{
	UpdateRecipeList();
	CString strModelName = CModelInfo::Instance()->GetModelNameTrayOcr();

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(strModelName, TRAYOCR_KIND);

	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();
	m_TrayOcr = stTrayOcr;

	UpdateUI();

	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
	if (strSelectModelName == strModelName)
		EnableAdaptiveWindows();

	UpdateData(FALSE);
}

void CTeachTab1GTrayOcr::Default()
{
	m_TrayOcr.Clear();

	UpdateData(FALSE);
}

void CTeachTab1GTrayOcr::Cleanup()
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

	m_bIsTeachTabBegin = FALSE;
	m_bIsTeachOcrROI = FALSE;
	m_bIsTrayNonInsp = FALSE;
}

HBRUSH CTeachTab1GTrayOcr::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_TRAYOCR_INFO:
	case IDC_STATIC_TRAYOCR_WIDTH:
	case IDC_STATIC_TRAYOCR_HEIGHT:
	case IDC_STATIC_PAGECOUNT:
	case IDC_STATIC_MATCHING_SCORE:
	case IDC_CHECK_TRAYOCR_REMOVE_BORDER:
	case IDC_CHECK_TRAYOCR_REMOVE_NARROW_OR_FLAT:
	case IDC_CHECK_TRAYOCR_CUT_LARGE_CHARACTERS:
	case IDC_CHECK_TRAYOCR_USE_ADAPTIVE_THRESHOLD:
	case IDC_CHECK_TRAYOCR_ROTATE_IMAGE:
	case IDC_STATIC_TRAYOCR_METHOD:
	case IDC_STATIC_TRAYOCR_HALF_KERNEL_SIZE:
	case IDC_STATIC_TRAYOCR_THRESHOLD_OFFSET:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab1GTrayOcr::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab1GTrayOcr::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab1GTrayOcr::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab1GTrayOcr::OnBnClickedRadioTrayOcrBypass()
{
	UpdateData(TRUE);
}

void CTeachTab1GTrayOcr::OnBnClickedBtnTestTrayOcr()
{
#ifdef RELEASE_1G
	WRITE_LOG( WL_BTN, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTestTrayOcr") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA1 );
	CVisionSystem::Instance()->StartInspection( InspectTypeTrayOcr, IDX_AREA1, TRUE);
#endif
}

void CTeachTab1GTrayOcr::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(TRAYOCR_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_TRAYOCR);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameTrayOcr();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameTrayOcr();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameTrayOcr();
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

void CTeachTab1GTrayOcr::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_TrayOcr\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameTrayOcr();
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

void CTeachTab1GTrayOcr::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GTrayOcr::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameTrayOcr();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, TRAYOCR_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, TRAYOCR_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

#ifdef RELEASE_1G
	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
	FrameGrabber.SetPageCount(CameraTypeLine);
#endif

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnSave :: End"));
}

void CTeachTab1GTrayOcr::DisableWnd(BOOL bEnable)
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

void CTeachTab1GTrayOcr::OnBnClickedBtnTrayOcrRecipeNo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTrayOcrRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTrayOcrRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( TRAYOCR_KIND, TEACH_TAB_IDX_TRAYOCR);
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 1G TRAYOCR") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTrayOcrRecipeNo :: End"));
#endif
}

void CTeachTab1GTrayOcr::UpdateTeachingImage_Tab()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return;

#ifdef RELEASE_1G
	USES_CONVERSION;
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA1, 0);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if (MatchBuff->GetPatternLearnt())
	{
		int nWidth = MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForTabTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		strLastRecipe = CModelInfo::Instance()->GetLastModelNameTrayOcr();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_TrayOcr\\") + strLastRecipe + _T("\\");
		strInspName = _T("Tray");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, 0);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForTabTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForTabTeaching.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImageObjectForTabTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForTabTeaching.ImageUpdate();

	m_pMainView->UpdateImageView();

#endif
}

void CTeachTab1GTrayOcr::UpdateUI()
{
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_EDIT_TRAYOCR_PAGECOUNT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_TRAYOCR_PAGECOUNT)->EnableWindow(FALSE);
	}

	UpdateTeachingImage_Tab();
	UpdateMethod();
	UpdateMasking();
}

void CTeachTab1GTrayOcr::UpdateMasking()
{
	m_Combo_Tray_Select_Masking_Num.ResetContent();

	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		CString strModelNum;
		strModelNum.Format(_T("Masking_%d"), i);
		m_Combo_Tray_Select_Masking_Num.AddString(strModelNum);
	}

	m_Combo_Tray_Select_Masking_Num.SetCurSel(m_nCombo_Tray_Masking_Select);
}

void CTeachTab1GTrayOcr::UpdateMethod()
{
	m_wndMethod.ResetContent();

	m_wndMethod.AddString(_T("Mean"));
	m_wndMethod.AddString(_T("Median"));
	m_wndMethod.AddString(_T("Middle"));
	
	m_wndMethod.SetCurSel(m_TrayOcr.nMethod);
}

void CTeachTab1GTrayOcr::OnBnClickedBtnTestTrayocrFontRegister()
{
	TeachOCRFontTray FontTrayTeaching;
	if (FontTrayTeaching.DoModal() == IDOK) {}
}

void APIENTRY CTeachTab1GTrayOcr::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab1GTrayOcr* pThis = (CTeachTab1GTrayOcr*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab1GTrayOcr::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
{
#ifdef RELEASE_1G
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

	// 기준 모델
	if (m_bIsTeachTabBegin)
	{
		m_TrayOcr.rcTabBegin = rcTrackRegion;

		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_TrayOcr.rcTabBegin);
		pGO->AddDrawBox(clrBox);
		pInspectionVision->SetLearnModel(pGO, pImgObj, m_TrayOcr.rcTabBegin, InspectTypeTrayOcr, nViewIndex, 0, FALSE, FALSE);

		bRet = TRUE;
	}

	// OCR
	if (m_bIsTeachOcrROI)
	{
		NoninspTeachingDelete_All();

		m_TrayOcr.rcInspArea = rcTrackRegion;

		UpdateUI();

		clrBox.CreateObject(PDC_ORANGE, m_TrayOcr.rcInspArea);
		pGO->AddDrawBox(clrBox);

		m_TrayOcr.nOcrAreaDistX = m_TrayOcr.rcInspArea.left - m_TrayOcr.rcTabBegin.left;
		m_TrayOcr.nOcrAreaDistY = m_TrayOcr.rcInspArea.top - m_TrayOcr.rcTabBegin.top;

		bRet = TRUE;
	}

	// 비검사
	if (m_bIsTrayNonInsp)
	{
		m_TrayOcr.rcMaskingArea[m_nCombo_Tray_Masking_Select] = rcTrackRegion;

		clrBox.CreateObject(PDC_ORANGE, m_TrayOcr.rcMaskingArea[m_nCombo_Tray_Masking_Select]);
		pGO->AddDrawBox(clrBox);

		m_TrayOcr.nMaskingAreaDistX[m_nCombo_Tray_Masking_Select] = m_TrayOcr.rcMaskingArea[m_nCombo_Tray_Masking_Select].left - m_TrayOcr.rcInspArea.left;
		m_TrayOcr.nMaskingAreaDistY[m_nCombo_Tray_Masking_Select] = m_TrayOcr.rcMaskingArea[m_nCombo_Tray_Masking_Select].top - m_TrayOcr.rcInspArea.top;

		bRet = TRUE;
	}

	if (bRet)
	{
		Cleanup();
		UpdateUI();
		UpdateData(FALSE);

		Save();
	}
#endif
}

void CTeachTab1GTrayOcr::OnBnClickedBtnRoiTrayocr()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GTrayOcr::OnBnClickedBtnRoiTrayOcr"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	if (m_TrayOcr.rcTabBegin.IsRectEmpty())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("Tab Align 모델 티칭을 먼저 진행해주세요."), MB_ICONERROR);
		else												AfxMessageBox(_T("Please perform the teaching of the Tab Align model first"), MB_ICONERROR);
		
		m_bIsTeachOcrROI = FALSE;
		m_btnROI.SetCheck(BST_UNCHECKED);
		return;
	}

#ifdef RELEASE_1G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTeachOcrROI)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while(pChild) 
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(UIExt::CFlatButton))) 
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnROI.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("OCR 검사 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the OCR inspection area."));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab1GTrayOcr::OnBnClickedBtnTabAlignTeachModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTabAlignTeachModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTeachTabBegin)
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

		m_btnTabAlignTeachModel.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("Tray 기준점을 찾기 위한 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the area to locate the tray reference point"));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTab1GTrayOcr::OnBnClickedCheckTrayocrUseAdaptiveThreshold()
{
	UpdateData(TRUE);
	
	EnableAdaptiveWindows();
}

void CTeachTab1GTrayOcr::EnableAdaptiveWindows() 
{
	if (m_TrayOcr.bUseAdaptiveThreshold)
	{
		GetDlgItem(IDC_STATIC							)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_TRAYOCR_METHOD			)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_TRAYOCR_HALF_KERNEL_SIZE	)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_TRAYOCR_THRESHOLD_OFFSET	)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_TRAYOCR_METHOD				)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TRAYOCR_HALF_KERNEL_SIZE	)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TRAYOCR_THRESHOLD_OFFSET	)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_OCR_PREVIEW					)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_STATIC							)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_TRAYOCR_METHOD			)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_TRAYOCR_HALF_KERNEL_SIZE	)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_TRAYOCR_THRESHOLD_OFFSET	)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_TRAYOCR_METHOD				)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TRAYOCR_HALF_KERNEL_SIZE	)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TRAYOCR_THRESHOLD_OFFSET	)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_OCR_PREVIEW					)->EnableWindow(FALSE);
	}
}

void CTeachTab1GTrayOcr::OnBnClickedBtnOcrPreview()
{
	CVisionSystem* pVisionSystem = CVisionSystem::Instance();
	CxImageObject* pImgObj = pVisionSystem->GetImageObject(CamTypeAreaScan, IDX_AREA1);
	CVisionSystem::Instance()->OCRImagePreview(pImgObj);
}

void CTeachTab1GTrayOcr::OnCbnSelchangeComboTrayocrMethod()
{
	m_TrayOcr.nMethod = m_wndMethod.GetCurSel();

	UpdateData(FALSE);
}

void CTeachTab1GTrayOcr::OnBnClickedBtnTabNoninspTeaching()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTabNoninspTeaching"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	if (m_TrayOcr.rcTabBegin.IsRectEmpty())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("Ocr 영역 설정을 먼저 진행해주세요."), MB_ICONERROR);
		else												AfxMessageBox(_T("Please perform the OCR area setting first."), MB_ICONERROR);
		
		m_bIsTrayNonInsp = FALSE;
		m_btnTrayNonInspTeachModel.SetCheck(BST_UNCHECKED);
		return;
	}

#ifdef RELEASE_1G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTrayNonInsp)
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

		m_btnTrayNonInspTeachModel.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("OCR 검사에서 제외할 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the area to be excluded from the OCR inspection."));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab1GTrayOcr::OnCbnSelchangeComboTrayNonInspModelNum()
{
	m_nCombo_Tray_Masking_Select = m_Combo_Tray_Select_Masking_Num.GetCurSel();

	UpdateData(FALSE);
}

void CTeachTab1GTrayOcr::OnBnClickedBtnTabNoninspTeachingDelete()
{
	UpdateData(TRUE);

	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();
	stTrayOcr.rcMaskingArea[m_nCombo_Tray_Masking_Select].SetRectEmpty();
	stTrayOcr.nMaskingAreaDistX[m_nCombo_Tray_Masking_Select] = 0;
	stTrayOcr.nMaskingAreaDistY[m_nCombo_Tray_Masking_Select] = 0;

#ifdef RELEASE_1G
	CModelInfo::Instance()->Save(TEACH_TAB_IDX_TRAYOCR);
#endif

	UpdateData(FALSE);

	Refresh();
}

void CTeachTab1GTrayOcr::NoninspTeachingDelete_All()
{
	UpdateData(TRUE);

	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();
	
	for (int i = 0; i < nTRAY_MASKING_MAX; i++)
	{
		stTrayOcr.rcMaskingArea[i].SetRectEmpty();
		stTrayOcr.nMaskingAreaDistX[i] = 0;
		stTrayOcr.nMaskingAreaDistY[i] = 0;
	}

#ifdef RELEASE_1G
	CModelInfo::Instance()->Save(TEACH_TAB_IDX_TRAYOCR);
#endif

	UpdateData(FALSE);

	Refresh();
}

void CTeachTab1GTrayOcr::SelectRecipeIndex(int nIndex)
{
	m_nSelectRecipeIndex = nIndex;
}

void CTeachTab1GTrayOcr::OnBnClickedBtnTrayocrRecipeInfo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTrayocrRecipeInfo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTrayocrRecipeInfo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CTrayRecipeInfoDlg TrayRecipeInfoDlg(this);
	TrayRecipeInfoDlg.SetRecipeKind(TRAYOCR_KIND, TEACH_TAB_IDX_TRAYOCR);
	TrayRecipeInfoDlg.SetTitleName(_T("Recipe Info : TRAY TAB OCR DATA"));

	if (TrayRecipeInfoDlg.DoModal() == IDOK)
	{
	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GTrayOcr::OnBnClickedBtnTrayocrRecipeInfo :: End"));
#endif
}

void CTeachTab1GTrayOcr::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_1G_TRAYOCR);

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
				strParams.Format(_T("\"file:///%s#page=26\""), (LPCTSTR)strPdfPath);

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
