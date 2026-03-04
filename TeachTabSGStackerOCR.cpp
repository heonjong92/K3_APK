// TeachTabSGStackerOCR.cpp: 구현 파일
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTabSGStackerOCR.h"
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
#include "StringMacro.h"
#include "TrayRecipeInfoDlg.h"

#include "UIExt/ResourceManager.h"

#include <XUtil/xUtils.h>
#include <XGraphic\xGraphicObject.h>

// CTeachTabSGStackerOCR 대화 상자

/* m_StackerTrayOcr이외 다른 멤버 변수 값 TrayOcr과 똑같이 사용 */

IMPLEMENT_DYNAMIC(CTeachTabSGStackerOcr, CDialog)

CTeachTabSGStackerOcr::CTeachTabSGStackerOcr(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTabSGStackerOcr::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsOcrFontRegister(FALSE)
	, m_bOCRPreview(FALSE)
	, m_bIsStackerNonInsp(FALSE)
{
	m_pMainView = NULL;
	m_StackerOcr.Clear();

	m_nCombo_Stacker_Masking_Select = 0;

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTabSGStackerOcr::~CTeachTabSGStackerOcr()
{
}

void CTeachTabSGStackerOcr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LABEL_TITLE_SG_STACKEROCR, m_wndLabelTitleSGStackerOcr);
	DDX_Control(pDX, IDC_LABEL_RECIPE_TITLE, m_wndLabelRecipeTitle);

	DDX_Control(pDX, IDC_COMBO_RECIPE_STACKEROCR, m_wndSelectRecipe);
	DDX_CBIndex(pDX, IDC_COMBO_RECIPE_STACKEROCR, m_nSelectRecipeIndex);

	DDX_Text(pDX, IDC_EDIT_STACKEROCR_DESCRIPTION, m_StackerOcr.strDescription);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_WIDTH_MIN, m_StackerOcr.nCharWidthMin);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_WIDTH_MAX, m_StackerOcr.nCharWidthMax);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_HEIGHT_MIN, m_StackerOcr.nCharHeightMin);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_HEIGHT_MAX, m_StackerOcr.nCharHeightMax);
	DDX_Text(pDX, IDC_EDIT_STACKER_OCR_INFO, m_StackerOcr.strOcrInfo);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_PAGECOUNT, m_StackerOcr.lPageCount);

	DDX_Control(pDX, IDC_BTN_TEST_STACKEROCR, m_btnTest_StackerOcr);
	DDX_Control(pDX, IDC_BTN_STACKEROCR_RECIPE_NO, m_btnSaveRecipeNo);
	DDX_Control(pDX, IDC_BTN_STACKEROCR_RECIPE_INFO, m_btnSaveRecipeInfo);
	DDX_Control(pDX, IDC_BTN_SAVE_STACKEROCR, m_btnSave);

	DDX_Check(pDX, IDC_CHECK_STACKER_ROTATE_IMAGE, m_StackerOcr.bRotateImage);
	DDX_Check(pDX, IDC_CHECK_STACKEROCR_REMOVE_BORDER, m_StackerOcr.bRemoveBorder);
	DDX_Check(pDX, IDC_CHECK_STACKEROCR_REMOVE_NARROW_OR_FLAT, m_StackerOcr.bRemoveNarrowFlat);
	DDX_Check(pDX, IDC_CHECK_STACKEROCR_CUT_LARGE_CHARACTERS, m_StackerOcr.bCutLargeChar);

	DDX_Check(pDX, IDC_CHECK_STACKEROCR_USE_ADAPTIVE_THRESHOLD, m_StackerOcr.bUseAdaptiveThreshold);
	DDX_Control(pDX, IDC_COMBO_STACKEROCR_METHOD, m_wndMethod);
	DDX_CBIndex(pDX, IDC_COMBO_STACKEROCR_METHOD, m_StackerOcr.nMethod);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_HALF_KERNEL_SIZE, m_StackerOcr.nHalfKernelSize);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_THRESHOLD_OFFSET, m_StackerOcr.nThresholdOffset);
	DDX_Text(pDX, IDC_EDIT_STACKEROCR_DEPTH_OFFSET, m_StackerOcr.nDepthOffset);
	DDX_Control(pDX, IDC_BTN_OCR_PREVIEW, m_btnOcrPreview);
	DDX_Check(pDX, IDC_BTN_OCR_PREVIEW, m_bOCRPreview);
	DDX_Control(pDX, IDC_BTN_STACKER_FONT_REGISTER, m_btnOcrFontRegister);

	DDX_Control(pDX, IDC_BUTTON_SET_DEPTH_OFFSET, m_btnDepthOffset);

	DDX_Check(pDX, IDC_BTN_STACKER_TAB_NONINSP_TEACHING, m_bIsStackerNonInsp);
	DDX_Control(pDX, IDC_BTN_STACKER_TAB_NONINSP_TEACHING, m_btnStackerNonInspTeachModel);

	DDX_Control(pDX, IDC_COMBO_STACKER_NON_INSP_MODEL_NUM, m_Combo_Stacker_Select_Masking_Num);
	DDX_CBIndex(pDX, IDC_COMBO_STACKER_NON_INSP_MODEL_NUM, m_nCombo_Stacker_Masking_Select);
}


BEGIN_MESSAGE_MAP(CTeachTabSGStackerOcr, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_TEST_STACKEROCR, &CTeachTabSGStackerOcr::OnBnClickedBtnTestStackerOcr)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_STACKEROCR, &CTeachTabSGStackerOcr::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_STACKEROCR, &CTeachTabSGStackerOcr::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_STACKEROCR_RECIPE_NO, &CTeachTabSGStackerOcr::OnBnClickedBtnStackerOcrRecipeNo)
	ON_BN_CLICKED(IDC_CHECK_STACKEROCR_USE_ADAPTIVE_THRESHOLD, &CTeachTabSGStackerOcr::OnBnClickedCheckStackerocrUseAdaptiveThreshold)
	ON_BN_CLICKED(IDC_BTN_OCR_PREVIEW, &CTeachTabSGStackerOcr::OnBnClickedBtnOcrPreview)
	ON_CBN_SELCHANGE(IDC_COMBO_STACKEROCR_METHOD, &CTeachTabSGStackerOcr::OnCbnSelchangeComboStackerocrMethod)
	ON_BN_CLICKED(IDC_BTN_STACKER_FONT_REGISTER, &CTeachTabSGStackerOcr::OnBnClickedBtnStackerFontRegister)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEPTH_OFFSET, &CTeachTabSGStackerOcr::OnBnClickedButtonSetDepthOffset)
	ON_BN_CLICKED(IDC_BTN_STACKER_TAB_NONINSP_TEACHING, &CTeachTabSGStackerOcr::OnBnClickedBtnStackerTabNoninspTeaching)
	ON_BN_CLICKED(IDC_BTN_STACKER_TAB_NONINSP_TEACHING_DELETE, &CTeachTabSGStackerOcr::OnBnClickedBtnStackerTabNoninspTeachingDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_STACKER_NON_INSP_MODEL_NUM, &CTeachTabSGStackerOcr::OnCbnSelchangeComboStackerNonInspModelNum)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_BIN_DATA, &CTeachTabSGStackerOcr::OnBnClickedButtonLoadBinData)
	ON_BN_CLICKED(IDC_BTN_STACKEROCR_RECIPE_INFO, &CTeachTabSGStackerOcr::OnBnClickedBtnStackerocrRecipeInfo)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTabSGStackerOcr 메시지 처리기입니다.
BOOL CTeachTabSGStackerOcr::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitleSGStackerOcr.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelTitleSGStackerOcr.SetFontHeight(12);

	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_StackerOcr.SetColor(DEF_BTN_COLOR_MEASURE);
	m_btnStackerNonInspTeachModel.SetColor(DEF_BTN_COLOR_TEACING);
	m_btnStackerNonInspTeachModel.SetFontHeight(10);

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTabSGStackerOcr::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_SG_STACKEROCR			)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_STACKEROCR_RECIPE_NO				)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_STATIC_DESCRIPTION					)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_PAGECOUNT						)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_BTN_TEST_STACKEROCR					)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_STACKEROCR_WIDTH				)->SetWindowText(CLanguageInfo::Instance()->ReadString(23));
	GetDlgItem(IDC_STATIC_STACKEROCR_HEIGHT				)->SetWindowText(CLanguageInfo::Instance()->ReadString(24));
	GetDlgItem(IDC_BTN_STACKER_FONT_REGISTER			)->SetWindowText(CLanguageInfo::Instance()->ReadString(26));
	GetDlgItem(IDC_STATIC_STACKER_OCR_INFO				)->SetWindowText(CLanguageInfo::Instance()->ReadString(29));
	GetDlgItem(IDC_STATIC_STACKEROCR_DEPTH_DATA_OFFSET	)->SetWindowText(CLanguageInfo::Instance()->ReadString(30));
	
	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitleSGStackerOcr.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTabSGStackerOcr::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.SetMaxTipWidth(300);  //멀티라인 활성화
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKER_OCR_INFO), _T("OCR로 인식된 텍스트와 비교할 문자열 입니다.두 개가 같으면 OK, 다르면 NG로 판정합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_PAGECOUNT), _T("Line Scan Camera로 읽는 범위이며, 이미지의 폭(Width)이 됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_WIDTH_MIN), _T("OCR로 읽을 글자 너비의 최소값을 입력합니다.해당 값 범위 이내의 글자만 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_WIDTH_MAX), _T("OCR로 읽을 글자 너비의 최대값을 입력합니다.해당 값 범위 이내의 글자만 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_HEIGHT_MIN), _T("OCR로 읽을 글자 높이의 최소값을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_HEIGHT_MAX), _T("OCR로 읽을 글자 높이의 최대값을 입력합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_STACKEROCR_REMOVE_BORDER), _T("OCR 영역의 가장자리에 맞닿는 글자를 인식하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_STACKEROCR_REMOVE_NARROW_OR_FLAT), _T("|이나 -(하이픈)과 같은 글자를 인식하지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_STACKEROCR_CUT_LARGE_CHARACTERS), _T("사이즈가 큰 글자를 적절한 크기로 잘라서 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_STACKEROCR_USE_ADAPTIVE_THRESHOLD), _T("글자 사이의 간격이 좁아 글자를 제대로 인식하지 못하는 경우 사용합니다."));

	CString strMethod;
	strMethod = _T("Mean : 주변 픽셀들의 “평균값(Mean)”을 임계값으로 사용\n");
	strMethod += _T(" - 가장 일반적인 방식\n - 밝기 변화가 완만한 이미지는 잘됨\n - 노이즈가 많으면 평균이 흔들릴 수 있음\n\n");
	strMethod += _T("Median : 주변 픽셀들의 “중앙값(Median)”을 임계값으로 사용\n");
	strMethod += _T(" - 노이즈에 강함 (특히 Salt & Pepper noise)\n - 평균보다 안정적\n - 값이 극단적으로 튀는 픽셀이 있어도 영향 적음\n\n");
	strMethod += _T("Middle : 주변 픽셀 값의 “최소값과 최대값의 중간값”을 사용\n");
	strMethod += _T(" - 밝기 대비가 큰 경우 유리\n - 평균 / 중앙값과 다르게 범위(contrast)만 보고 판단\n - 명암이 강한 이미지에서 객체 / 배경이 잘 나뉠 수 있음, 하지만 노이즈가 min 또는 max 를 잡아버리면 불안정함");

	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_STACKEROCR_METHOD), strMethod);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_HALF_KERNEL_SIZE), _T("Convolution연산을 수행할 Kernel의 크기를 지정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_STACKEROCR_THRESHOLD_OFFSET), _T("설정한 ‘Method’에 Offset으로 주는 값입니다.‘이미지 미리보기’ 기능을 통해 변환된 이미지를 확인해가며 최적의 값을 입력해야합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_BTN_OCR_PREVIEW), _T("설정된 adaptive threshold 파라미터로 OCR을 할 때의 이미지를 확인할 수 있습니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_BTN_STACKER_FONT_REGISTER), _T("OCR에 사용 되는 폰트를 등록하는 창이 열립니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_STACKER_ROTATE_IMAGE), _T("Tab부분 글자가 회전 되어 있는 Tray의 경우, 박스를 체크한 뒤 저장하면 Line Scan Camera로 이미지를 Grab할 때 Viewer에 이미지가 회전되어 나타납니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTabSGStackerOcr::IsModified()
{
	UpdateData(TRUE);

	return TRUE;
}

BOOL CTeachTabSGStackerOcr::Save()
{
	UpdateData(TRUE);

	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();

	// Validation Check
	if (m_StackerOcr.nCharWidthMin > m_StackerOcr.nCharWidthMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("글자 너비 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum character width must be less than or equal to the maximum value."), MB_ICONERROR);
		
		m_StackerOcr = stStackerOcr;
		UpdateData(FALSE);

		return FALSE;
	}
	if (m_StackerOcr.nCharHeightMin > m_StackerOcr.nCharHeightMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("글자 높이 최소값은 최대값 이하여야 합니다."), MB_ICONERROR);
		else												AfxMessageBox(_T("The minimum character height must be less than or equal to the maximum value."), MB_ICONERROR);
		m_StackerOcr = stStackerOcr;
		UpdateData(FALSE);

		return FALSE;
	}

	CheckData();

	stStackerOcr = m_StackerOcr;

#ifdef RELEASE_SG
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameStackerOcr(), TEACH_TAB_IDX_STACKER_OCR);
	CModelInfo::Instance()->Save(TEACH_TAB_IDX_STACKER_OCR);
#endif

	UpdateData(FALSE);

	UpdateUI();

	return TRUE;
}

void CTeachTabSGStackerOcr::CheckData()
{
#ifdef RELEASE_SG
	CString strLog;
	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();

	strLog.Format(_T("[Description][%s→%s]"), stStackerOcr.strDescription, m_StackerOcr.strDescription);
	if (stStackerOcr.strDescription != m_StackerOcr.strDescription)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);	
	
	strLog.Format(_T("[OcrInfo][%s→%s]"), stStackerOcr.strOcrInfo, m_StackerOcr.strOcrInfo);
	if (stStackerOcr.strOcrInfo != m_StackerOcr.strOcrInfo)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[CharacterWidthMin][%d→%d]"), stStackerOcr.nCharWidthMin, m_StackerOcr.nCharWidthMin);
	if (stStackerOcr.nCharWidthMin != m_StackerOcr.nCharWidthMin) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[CharacterWidthMax][%d→%d]"), stStackerOcr.nCharWidthMax, m_StackerOcr.nCharWidthMax);
	if (stStackerOcr.nCharWidthMax != m_StackerOcr.nCharWidthMax) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[CharacterHeightMin][%d→%d]"), stStackerOcr.nCharHeightMin, m_StackerOcr.nCharHeightMin);
	if (stStackerOcr.nCharHeightMin != m_StackerOcr.nCharHeightMin)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[CharacterHeightMax][%d→%d]"), stStackerOcr.nCharHeightMax, m_StackerOcr.nCharHeightMax);
	if (stStackerOcr.nCharHeightMax != m_StackerOcr.nCharHeightMax)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[OcrAreaDistanceX][%d→%d]"), stStackerOcr.nOcrAreaDistX, m_StackerOcr.nOcrAreaDistX);
	if (stStackerOcr.nOcrAreaDistX != m_StackerOcr.nOcrAreaDistX) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[OcrAreaDistanceY][%d→%d]"), stStackerOcr.nOcrAreaDistY, m_StackerOcr.nOcrAreaDistY);
	if (stStackerOcr.nOcrAreaDistY != m_StackerOcr.nOcrAreaDistY) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[PageCount][%ld→%ld]"), stStackerOcr.lPageCount, m_StackerOcr.lPageCount);
	if (stStackerOcr.lPageCount != m_StackerOcr.lPageCount)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Remove Border][%d→%d]"), stStackerOcr.bRemoveBorder, m_StackerOcr.bRemoveBorder);
	if (stStackerOcr.bRemoveBorder != m_StackerOcr.bRemoveBorder) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Remove Narrow or Flat][%d→%d]"), stStackerOcr.bRemoveNarrowFlat, m_StackerOcr.bRemoveNarrowFlat);
	if (stStackerOcr.bRemoveNarrowFlat != m_StackerOcr.bRemoveNarrowFlat) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Cut Large Char][%d→%d]"), stStackerOcr.bCutLargeChar, m_StackerOcr.bCutLargeChar);
	if (stStackerOcr.bCutLargeChar != m_StackerOcr.bCutLargeChar) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Use Adaptive Threshold][%d→%d]"), stStackerOcr.bUseAdaptiveThreshold, m_StackerOcr.bUseAdaptiveThreshold);
	if (stStackerOcr.bUseAdaptiveThreshold != m_StackerOcr.bUseAdaptiveThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Method][%d→%d]"), stStackerOcr.nMethod, m_StackerOcr.nMethod);
	if (stStackerOcr.nMethod != m_StackerOcr.nMethod) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Half Kernel Size][%d→%d]"), stStackerOcr.nHalfKernelSize, m_StackerOcr.nHalfKernelSize);
	if (stStackerOcr.nHalfKernelSize != m_StackerOcr.nHalfKernelSize) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);

	strLog.Format(_T("[Threshold Offset][%d→%d]"), stStackerOcr.nThresholdOffset, m_StackerOcr.nThresholdOffset);
	if (stStackerOcr.nThresholdOffset != m_StackerOcr.nThresholdOffset) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeStackerOcr, strLog);
#endif
}

void CTeachTabSGStackerOcr::Refresh()
{
	UpdateRecipeList();
	CString strModelName = CModelInfo::Instance()->GetModelNameStackerOcr();

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(strModelName, STACKEROCR_KIND);

	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();
	m_StackerOcr = stStackerOcr;

	UpdateUI();

	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
	if (strSelectModelName == strModelName)
		EnableAdaptiveWindows();

	UpdateData(FALSE);
}

void CTeachTabSGStackerOcr::Default()
{
	m_StackerOcr.Clear();

	UpdateData(FALSE);
}

void CTeachTabSGStackerOcr::Cleanup()
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

	m_bIsStackerNonInsp = FALSE;
}

HBRUSH CTeachTabSGStackerOcr::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_STACKEROCR_WIDTH:
	case IDC_STATIC_STACKEROCR_HEIGHT:
	case IDC_STATIC_PAGECOUNT:
	case IDC_STATIC_MATCHING_SCORE:
	case IDC_CHECK_STACKEROCR_REMOVE_BORDER:
	case IDC_CHECK_STACKEROCR_REMOVE_NARROW_OR_FLAT:
	case IDC_CHECK_STACKEROCR_CUT_LARGE_CHARACTERS:
	case IDC_CHECK_STACKER_ROTATE_IMAGE:
	case IDC_CHECK_STACKEROCR_USE_ADAPTIVE_THRESHOLD:
	case IDC_STATIC_STACKEROCR_METHOD:
	case IDC_STATIC_STACKEROCR_HALF_KERNEL_SIZE:
	case IDC_STATIC_STACKEROCR_THRESHOLD_OFFSET:
	case IDC_STATIC_STACKER_OCR_INFO:
	case IDC_STATIC_STACKEROCR_DEPTH_DATA_OFFSET:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTabSGStackerOcr::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));

	return TRUE;
}

void CTeachTabSGStackerOcr::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTabSGStackerOcr::PreTranslateMessage(MSG* pMsg)
{
	UIExt::CFilteredEdit* pEdit = NULL;

	CRect rcClient;
	int nDlgCtrlID = ::GetDlgCtrlID(pMsg->hwnd);

	if (m_toolTip.m_hWnd != nullptr)
		m_toolTip.RelayEvent(pMsg);

	pEdit = (UIExt::CFilteredEdit*)GetDlgItem(nDlgCtrlID);

	if (!pEdit || !pEdit->IsKindOf(RUNTIME_CLASS(UIExt::CFilteredEdit)))
	{
		return CDialog::PreTranslateMessage(pMsg);
	}

	if (pMsg->message == WM_KEYUP)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (nVirtKey == VK_RETURN)
		{
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTeachTabSGStackerOcr::OnBnClickedBtnTestStackerOcr()
{
#ifdef RELEASE_SG
	WRITE_LOG(WL_BTN, _T("CTeachTabSGStackerOcr::OnBnClickedBtnTestStackerOcr"));

	CVisionSystem::Instance()->BeginStopWatch(IDX_AREA1);
	CVisionSystem::Instance()->StartInspection(InspectTypeStackerOcr, IDX_AREA1, 0, TRUE);
#endif
}

void CTeachTabSGStackerOcr::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_SG
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(STACKEROCR_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_STACKER_OCR);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameStackerOcr();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameStackerOcr();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameStackerOcr();
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

void CTeachTabSGStackerOcr::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("SG_StackerOcr\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameStackerOcr();
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

void CTeachTabSGStackerOcr::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTabSGStackerOcr::OnBnClickedBtnSave::Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameStackerOcr();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, STACKEROCR_KIND))
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, STACKEROCR_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

#ifdef RELEASE_SG
	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
	FrameGrabber.SetPageCount(CameraTypeLine);
#endif

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnSave :: End"));
}

void CTeachTabSGStackerOcr::DisableWnd(BOOL bEnable)
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

void CTeachTabSGStackerOcr::OnBnClickedBtnStackerOcrRecipeNo()
{
#ifdef RELEASE_SG
	WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerOcrRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerOcrRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind(STACKEROCR_KIND, TEACH_TAB_IDX_STACKER_OCR);
	EditJobNumberDlg.SetTitleName(_T("Recipe No : SG STACKER TRAYOCR"));

	if (EditJobNumberDlg.DoModal() == IDOK)
	{
	}

	WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerOcrRecipeNo :: End"));
#endif
}

void CTeachTabSGStackerOcr::UpdateUI()
{
	UpdateMethod();
	UpdateMasking();

	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
		GetDlgItem(IDC_EDIT_STACKEROCR_PAGECOUNT)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_STACKEROCR_PAGECOUNT)->EnableWindow(FALSE);
}

void CTeachTabSGStackerOcr::UpdateMasking()
{
	m_Combo_Stacker_Select_Masking_Num.ResetContent();

	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		CString strModelNum;
		strModelNum.Format(_T("Masking_%d"), i);
		m_Combo_Stacker_Select_Masking_Num.AddString(strModelNum);
	}

	m_Combo_Stacker_Select_Masking_Num.SetCurSel(m_nCombo_Stacker_Masking_Select);
}

void CTeachTabSGStackerOcr::UpdateMethod()
{
	m_wndMethod.ResetContent();

	m_wndMethod.AddString(_T("Mean"));
	m_wndMethod.AddString(_T("Median"));
	m_wndMethod.AddString(_T("Middle"));

	m_wndMethod.SetCurSel(m_StackerOcr.nMethod);
}

void APIENTRY CTeachTabSGStackerOcr::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTabSGStackerOcr* pThis = (CTeachTabSGStackerOcr*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTabSGStackerOcr::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
{
#ifdef RELEASE_SG
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

	if (m_bIsStackerNonInsp)
	{
		double dResultAngle;
		DPOINT dptGaugeCenter, dptResult;
		dptGaugeCenter.x = pImgObj->GetWidth() / 2;
		dptGaugeCenter.y = pImgObj->GetHeight() / 2;

		// ocr
		BOOL bLine = FALSE;
		CRect rcOcrArea(0, 0, 0, 0);
		int nOffset = 1;
		
		if (m_StackerOcr.bRotateImage)
		{
			bLine = pInspectionVision->LineGauge(pImgObj, ETransitionType::ETransitionType_Bw, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, 500.f, 2000.f,
				0, 20, 1, 10, 0, 1, 3, 20.f, FALSE, FALSE, TRUE, dptResult, dResultAngle);
			nOffset = -1;
		}
		else
		{
			bLine = pInspectionVision->LineGauge(pImgObj, ETransitionType::ETransitionType_Wb, ETransitionChoice::ETransitionChoice_NthFromEnd, dptGaugeCenter, 500.f, 2000.f,
				0, 20, 1, 10, 0, 1, 3, 20.f, FALSE, FALSE, TRUE, dptResult, dResultAngle);
		}

		rcOcrArea.left = 0;
		rcOcrArea.top = GetRoundShort(dptResult.y - (190 * nOffset));
		rcOcrArea.right = pImgObj->GetWidth();
		rcOcrArea.bottom = rcOcrArea.top + (180 * nOffset);
		rcOcrArea.NormalizeRect();

		m_StackerOcr.rcMaskingArea[m_nCombo_Stacker_Masking_Select] = rcTrackRegion;

		clrBox.CreateObject(PDC_ORANGE, m_StackerOcr.rcMaskingArea[m_nCombo_Stacker_Masking_Select]);
		pGO->AddDrawBox(clrBox);

		m_StackerOcr.nMaskingAreaDistX[m_nCombo_Stacker_Masking_Select] = m_StackerOcr.rcMaskingArea[m_nCombo_Stacker_Masking_Select].left - rcOcrArea.left;
		m_StackerOcr.nMaskingAreaDistY[m_nCombo_Stacker_Masking_Select] = m_StackerOcr.rcMaskingArea[m_nCombo_Stacker_Masking_Select].top - rcOcrArea.top;

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

void CTeachTabSGStackerOcr::OnBnClickedCheckStackerocrUseAdaptiveThreshold()
{
	UpdateData(TRUE);

	EnableAdaptiveWindows();
}

void CTeachTabSGStackerOcr::EnableAdaptiveWindows()
{
	if (m_StackerOcr.bUseAdaptiveThreshold)
	{
		GetDlgItem(IDC_STATIC								)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_STACKEROCR_METHOD				)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_STACKEROCR_HALF_KERNEL_SIZE	)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_STACKEROCR_THRESHOLD_OFFSET	)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_STACKEROCR_METHOD				)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STACKEROCR_HALF_KERNEL_SIZE		)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STACKEROCR_THRESHOLD_OFFSET		)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STACKEROCR_DEPTH_OFFSET			)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_OCR_PREVIEW						)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_STATIC								)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_STACKEROCR_METHOD				)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_STACKEROCR_HALF_KERNEL_SIZE	)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_STACKEROCR_THRESHOLD_OFFSET	)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_STACKEROCR_METHOD				)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STACKEROCR_HALF_KERNEL_SIZE		)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STACKEROCR_THRESHOLD_OFFSET		)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STACKEROCR_DEPTH_OFFSET			)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_OCR_PREVIEW						)->EnableWindow(FALSE);
	}
}

void CTeachTabSGStackerOcr::OnBnClickedBtnOcrPreview()
{
#ifdef RELEASE_SG
	CVisionSystem* pVisionSystem = CVisionSystem::Instance();
	CxImageObject* pImgObj = pVisionSystem->GetImageObject(CamTypeAreaScan, IDX_AREA1);
	CVisionSystem::Instance()->OCRImagePreview_stacker(pImgObj);
#endif
}

void CTeachTabSGStackerOcr::OnCbnSelchangeComboStackerocrMethod()
{
	m_StackerOcr.nMethod = m_wndMethod.GetCurSel();

	UpdateData(FALSE);
}

void CTeachTabSGStackerOcr::OnBnClickedBtnStackerFontRegister()
{
	TeachOCRFontTray FontTrayTeaching;
	if (FontTrayTeaching.DoModal() == IDOK) {}
}

void CTeachTabSGStackerOcr::OnBnClickedButtonSetDepthOffset()
{
	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();
	CVisionSystem::Instance()->SetDepthOffset(stStackerOcr.nDepthOffset);
}

void CTeachTabSGStackerOcr::OnBnClickedBtnStackerTabNoninspTeaching()
{
	WRITE_LOG(WL_BTN, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerTabNoninspTeaching"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_SG
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsStackerNonInsp)
	{
		CWnd* pChild = GetWindow(GW_CHILD);
		while (pChild)
		{
			if (pChild->IsKindOf(RUNTIME_CLASS(CButton)))
			{
				pChild->EnableWindow(FALSE);
			}
			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}

		m_btnStackerNonInspTeachModel.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
}
#endif
}

void CTeachTabSGStackerOcr::OnCbnSelchangeComboStackerNonInspModelNum()
{
	m_nCombo_Stacker_Masking_Select = m_Combo_Stacker_Select_Masking_Num.GetCurSel();

	UpdateData(FALSE);
}

void CTeachTabSGStackerOcr::OnBnClickedBtnStackerTabNoninspTeachingDelete()
{
	UpdateData(TRUE);

	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();
	stStackerOcr.rcMaskingArea[m_nCombo_Stacker_Masking_Select].SetRectEmpty();
	stStackerOcr.nMaskingAreaDistX[m_nCombo_Stacker_Masking_Select] = 0;
	stStackerOcr.nMaskingAreaDistY[m_nCombo_Stacker_Masking_Select] = 0;

#ifdef RELEASE_SG
	CModelInfo::Instance()->Save(TEACH_TAB_IDX_STACKER_OCR);
#endif

	UpdateData(FALSE);

	Refresh();
}

void CTeachTabSGStackerOcr::NoninspTeachingDelete_All()
{
	UpdateData(TRUE);

	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();

	for (int i = 0; i < nTRAY_MASKING_MAX; i++)
	{
		stStackerOcr.rcMaskingArea[i].SetRectEmpty();
		stStackerOcr.nMaskingAreaDistX[i] = 0;
		stStackerOcr.nMaskingAreaDistY[i] = 0;
	}

#ifdef RELEASE_SG
	CModelInfo::Instance()->Save(TEACH_TAB_IDX_STACKER_OCR);
#endif

	UpdateData(FALSE);

	Refresh();
}

void CTeachTabSGStackerOcr::OnBnClickedButtonLoadBinData()
{
	CString strFilter = _T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||");
	CFileDialog FileDialog(TRUE, _T("*.bin"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, AfxGetMainWnd());

	if (FileDialog.DoModal() == IDOK)
	{
		CVisionSystem::Instance()->LoadDepthFromBin(FileDialog.GetPathName());
		CVisionSystem::Instance()->SetDepthOffset();
	}
}

void CTeachTabSGStackerOcr::SelectRecipeIndex(int nIndex)
{
	m_nSelectRecipeIndex = nIndex;
}


void CTeachTabSGStackerOcr::OnBnClickedBtnStackerocrRecipeInfo()
{
#ifdef RELEASE_SG
	WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerocrRecipeInfo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerocrRecipeInfo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CTrayRecipeInfoDlg TrayRecipeInfoDlg(this);
	TrayRecipeInfoDlg.SetRecipeKind(STACKEROCR_KIND, TEACH_TAB_IDX_STACKER_OCR);
	TrayRecipeInfoDlg.SetTitleName(_T("Recipe Info : TRAY TAB OCR DATA"));

	if (TrayRecipeInfoDlg.DoModal() == IDOK)
	{
	}

	WRITE_LOG(WL_MSG, _T("CTeachTabSGStackerOcr::OnBnClickedBtnStackerocrRecipeInfo :: End"));
#endif
}

void CTeachTabSGStackerOcr::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_SG_STACKEROCR);
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
				strParams.Format(_T("\"file:///%s#page=65\""), (LPCTSTR)strPdfPath);

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
