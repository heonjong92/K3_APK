// TeachTab4GMBB.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab4GMBB.h"
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

// CTeachTab4GMBB 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab4GMBB, CDialog)

CTeachTab4GMBB::CTeachTab4GMBB(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab4GMBB::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsTeachMBBSize(FALSE)
	, m_bIsTeachMBBRotate(FALSE)
	, m_nModelNum(0)
{
	m_pMainView = NULL;
	m_MBB.Clear();

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab4GMBB::~CTeachTab4GMBB()
{
}

void CTeachTab4GMBB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_4G,				m_wndLabelTitle4G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,			m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_MBB,			m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_MBB,			m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_MBB_BYPASS,			m_MBB.nUseBypass_MBB);
	DDX_Text(pDX,		IDC_EDIT_MBB_DESCRIPTION,		m_MBB.strDescription);
	DDX_Text(pDX,		IDC_EDIT_MBB_LED_LIGHT_VALUE,	m_MBB.nValueCh1);

	DDX_Text(pDX,		IDC_EDIT_MBB_TOLERANCE,			m_MBB.fTolerance);
	DDX_Text(pDX,		IDC_EDIT_MBB_LENGTH,			m_MBB.fLength);

	DDX_Text(pDX,		IDC_EDIT_MBB_WIDTH,				m_MBB.nWidth);
	DDX_Text(pDX,		IDC_EDIT_MBB_WIDTH_PX,			m_MBB.nWidth_PX);
	DDX_Text(pDX,		IDC_EDIT_MBB_HEIGHT,			m_MBB.nHeight);
	DDX_Text(pDX,		IDC_EDIT_MBB_HEIGHT_PX,			m_MBB.nHeight_PX);
	DDX_Text(pDX,		IDC_EDIT_MBB_RANGE,				m_MBB.nRange);
	DDX_Text(pDX,		IDC_EDIT_MBB_RANGE_PX,			m_MBB.nRange_PX);

	DDX_Text(pDX,		IDC_EDIT_MBB_MATCHING_SCORE,	m_MBB.fRatio);

	DDX_Control( pDX,	IDC_BTN_TEST_MBB,				m_btnTest_MBB );
	DDX_Control(pDX,	IDC_BTN_MBB_RECIPE_NO,			m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_MBB,				m_btnSave);

	DDX_Control(pDX,	IDC_BTN_ROI_MBB,				m_btnTeachMBB);
	DDX_Check(pDX,		IDC_BTN_ROI_MBB,				m_bIsTeachMBBSize);

	DDX_Control(pDX,	IDC_BTN_MBB_TECH_MODEL,			m_btnMBBTechModel);
	DDX_Check(pDX,		IDC_BTN_MBB_TECH_MODEL,			m_bIsTeachMBBRotate);

	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_TEACH_MBB,	m_ctrlPicPreviewTeachMBB);

	DDX_Radio(pDX,		IDC_RADIO_SELECT_UNIT,			m_MBB.nSelectUnit);
	
	DDX_Control(pDX,	IDC_COMBO_MBB_MODEL_NUM, 		m_Combo_SelectModelNum);
	DDX_CBIndex(pDX,	IDC_COMBO_MBB_MODEL_NUM, 		m_nModelNum);
}

BEGIN_MESSAGE_MAP(CTeachTab4GMBB, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_MBB_BYPASS,		&CTeachTab4GMBB::OnBnClickedRadioMbbBypass)
	ON_BN_CLICKED(IDC_RADIO_MBB_BYPASS2,	&CTeachTab4GMBB::OnBnClickedRadioMbbBypass)
	ON_BN_CLICKED(IDC_RADIO_MBB_BYPASS3,	&CTeachTab4GMBB::OnBnClickedRadioMbbBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_MBB,			&CTeachTab4GMBB::OnBnClickedBtnTestMbb)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_MBB,	&CTeachTab4GMBB::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_MBB,			&CTeachTab4GMBB::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_MBB_RECIPE_NO,	&CTeachTab4GMBB::OnBnClickedBtnMbbRecipeNo)
	ON_BN_CLICKED(IDC_BTN_ROI_MBB,			&CTeachTab4GMBB::OnBnClickedBtnRoiMbb)
	ON_BN_CLICKED(IDC_BTN_MBB_TECH_MODEL,	&CTeachTab4GMBB::OnBnClickedBtnMbbTechModel)
	ON_BN_CLICKED(IDC_RADIO_SELECT_UNIT,	&CTeachTab4GMBB::OnBnClickedRadioSelectUnit)
	ON_CBN_SELCHANGE(IDC_COMBO_MBB_MODEL_NUM,	&CTeachTab4GMBB::OnCbnSelchangeComboMBBModelNum)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab4GMBB 메시지 처리기입니다.
BOOL CTeachTab4GMBB::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle4G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle4G.SetFontHeight( 12 );

	m_ctrlPicPreviewTeachMBB.GetClientRect(m_rcPicCtrlSizeTeachMBB);

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_MBB.SetColor( DEF_BTN_COLOR_MEASURE );

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

	m_ImageViewForMBBTeaching.Create(&m_ImageViewManagerForMBBTeaching, this);
	m_ImageViewForMBBTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForMBBTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForMBBTeaching.SetAnimateWindow(FALSE);

	m_ImageViewForMBBTeaching.SetImageObject(&m_ImageObjectForMBBTeaching, FALSE);
	m_ImageViewForMBBTeaching.SetRegisterCallBack(0, &regCB);

	m_ImageViewForMBBTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForMBBTeaching.SetBodyColor(dwBodyColor);

	m_ImageViewForMBBTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForMBBTeaching.ShowScrollBar(FALSE);
	m_ImageViewForMBBTeaching.ShowScaleBar(FALSE);
	m_ImageViewForMBBTeaching.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_MBB)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(0);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForMBBTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab4GMBB::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_4G			)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_MBB_RECIPE_NO		)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_RADIO_MBB_BYPASS			)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_MBB_BYPASS2		)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_MBB_BYPASS3		)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_STATIC_DESCRIPTION		)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_BTN_TEST_MBB				)->SetWindowText(CLanguageInfo::Instance()->ReadString(19));
	GetDlgItem(IDC_BTN_ROI_MBB				)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_STATIC_MBB_TOLERANCE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_MBB_LENGTH		)->SetWindowText(CLanguageInfo::Instance()->ReadString(22));
	GetDlgItem(IDC_BTN_MBB_TECH_MODEL		)->SetWindowText(CLanguageInfo::Instance()->ReadString(25));
	GetDlgItem(IDC_STATIC_MATCHING_SCORE	)->SetWindowText(CLanguageInfo::Instance()->ReadString(26));
	
	CModelInfo::stMBBInfo& stMBBInfo = CModelInfo::Instance()->GetMBBInfo();
	if (stMBBInfo.nSelectUnit == 0)
	{
		GetDlgItem(IDC_STATIC_MBB_SPEC)->SetWindowText(CLanguageInfo::Instance()->ReadString(23));
		GetDlgItem(IDC_STATIC_MBB_RANGE)->SetWindowText(CLanguageInfo::Instance()->ReadString(24));
	}
	else
	{
		GetDlgItem(IDC_STATIC_MBB_SPEC)->SetWindowText(CLanguageInfo::Instance()->ReadString(28));
		GetDlgItem(IDC_STATIC_MBB_RANGE)->SetWindowText(CLanguageInfo::Instance()->ReadString(29));
	}

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle4G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab4GMBB::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_TOLERANCE), _T("MBB Edge를 찾기 위한 ROI 영역 입니다. 중심과 수직 방향으로 검사 영역에서 ~ 파라미터 값까지의 영역입니다"));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_LENGTH), _T("MBB Edge를 찾기 위한 ROI 영역 입니다.중심과 수평 방향으로 검사 영역에서 ~파라미터 값까지의 영역입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_WIDTH), _T("[MM] MBB 가로 길이 Spec을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_WIDTH_PX), _T("[PX] MBB 가로 길이 Spec을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_HEIGHT), _T("[MM] MBB 세로 길이 Spec을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_HEIGHT_PX), _T("[PX] MBB 세로 길이 Spec을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_RANGE), _T("[MM] Spec 대비 길이 오차 허용 범위를 입력합니다. 측정 길이가 해당 범위 안에 있어야 OK 판정입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_RANGE_PX), _T("[PX] Spec 대비 길이 오차 허용 범위를 입력합니다. 측정 길이가 해당 범위 안에 있어야 OK 판정입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MBB_MATCHING_SCORE), _T("MBB 방향 검사 입니다. 매칭 모델을 등록하며, 모델 검사 일치율 파라미터도 입력해야 합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_MBB_TECH_MODEL), _T("MBB 방향 검사 입니다. 매칭 모델을 등록하며, 모델 검사 일치율 파라미터도 입력해야 합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_MBB), _T("MBB 길이 검사 영역 설정입니다. MBB 보다 크게 영역을 설정하면 됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_SELECT_UNIT), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_SELECT_UNIT2), _T("길이 측정 단위를 설정 할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_MBB_MODEL_NUM), _T("등록할 모델을 선택할 수 있는 콤보 박스입니다. 모델 등록 순서는 검사와 무관하며 최대 10개의 모델을 등록할 수 있습니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab4GMBB::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab4GMBB::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stMBBInfo& MBBInfo = CModelInfo::Instance()->GetMBBInfo();
	MBBInfo = m_MBB;

#ifdef RELEASE_4G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameMBB(), TEACH_TAB_IDX_MBB);
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_MBB );
#endif

	Refresh();

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab4GMBB::CheckData()
{
#ifdef RELEASE_4G
	CString strLog;
	CModelInfo::stMBBInfo& MBBInfo = CModelInfo::Instance()->GetMBBInfo();

	// ----- MBB Teaching -----
	strLog.Format(_T("[Bypass][%s→%s]"), strBypassName[MBBInfo.nUseBypass_MBB], strBypassName[m_MBB.nUseBypass_MBB] );
	if( MBBInfo.nUseBypass_MBB != m_MBB.nUseBypass_MBB) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeMBB, strLog );
	strLog.Format(_T("[Description][%s→%s]"), MBBInfo.strDescription, m_MBB.strDescription);
	if (MBBInfo.strDescription != m_MBB.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	strLog.Format(_T("[LightValueCh1][%d→%d]"), MBBInfo.nValueCh1, m_MBB.nValueCh1);
	if (MBBInfo.nValueCh1 != m_MBB.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);

	strLog.Format(_T("[Area_Left][%d→%d]"), MBBInfo.rcInspArea.left, m_MBB.rcInspArea.left);
	if (MBBInfo.rcInspArea.left != m_MBB.rcInspArea.left) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	strLog.Format(_T("[Area_Right][%d→%d]"), MBBInfo.rcInspArea.right, m_MBB.rcInspArea.right);
	if (MBBInfo.rcInspArea.right != m_MBB.rcInspArea.right) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	strLog.Format(_T("[Area_Top][%d→%d]"), MBBInfo.rcInspArea.top, m_MBB.rcInspArea.top);
	if (MBBInfo.rcInspArea.top != m_MBB.rcInspArea.top)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	strLog.Format(_T("[Area_Bottom][%d→%d]"), MBBInfo.rcInspArea.bottom, m_MBB.rcInspArea.bottom);
	if (MBBInfo.rcInspArea.bottom != m_MBB.rcInspArea.bottom) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);

	strLog.Format(_T("[Tolerance][%d→%d]"), MBBInfo.fTolerance, m_MBB.fTolerance);
	if (MBBInfo.fTolerance != m_MBB.fTolerance)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	strLog.Format(_T("[Length][%d→%d]"), MBBInfo.fLength, m_MBB.fLength);
	if (MBBInfo.fLength != m_MBB.fLength) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	
	for (int i = 0; i < nMATCH_MAX; i++)
	{
		strLog.Format(_T("[MBB_Offset_%d X][%d→%d]"), i, m_MBB.ptOffset_MBB[i].x, m_MBB.ptOffset_MBB[i].x);
		if (m_MBB.ptOffset_MBB[i].x != m_MBB.ptOffset_MBB[i].x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
		strLog.Format(_T("[MBB_Offset_%d Y][%d→%d]"), i, m_MBB.ptOffset_MBB[i].y, m_MBB.ptOffset_MBB[i].y);
		if (m_MBB.ptOffset_MBB[i].y != m_MBB.ptOffset_MBB[i].y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
	}

	strLog.Format(_T("[RATIO][%d→%d]"), MBBInfo.fRatio, m_MBB.fRatio);
	if (MBBInfo.fRatio != m_MBB.fRatio) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMBB, strLog);
#endif
}

void CTeachTab4GMBB::Refresh()
{
	UpdateRecipeList();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(CModelInfo::Instance()->GetModelNameMBB(), MBB_KIND);

	CModelInfo::stMBBInfo& MBBInfo = CModelInfo::Instance()->GetMBBInfo();
	m_MBB = MBBInfo;

	BOOL bPx = (MBBInfo.nSelectUnit == 0) ? TRUE : FALSE;
	GetDlgItem(IDC_EDIT_MBB_WIDTH_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_MBB_HEIGHT_PX)->EnableWindow(bPx);
	GetDlgItem(IDC_EDIT_MBB_RANGE_PX)->EnableWindow(bPx);

	GetDlgItem(IDC_EDIT_MBB_WIDTH)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_MBB_HEIGHT)->EnableWindow(!bPx);
	GetDlgItem(IDC_EDIT_MBB_RANGE)->EnableWindow(!bPx);
	
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(TRUE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(TRUE);

		GetDlgItem(IDC_EDIT_MBB_WIDTH_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_MBB_HEIGHT_PX)->ShowWindow(bPx);
		GetDlgItem(IDC_EDIT_MBB_RANGE_PX)->ShowWindow(bPx);

		GetDlgItem(IDC_EDIT_MBB_WIDTH)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_MBB_HEIGHT)->ShowWindow(!bPx);
		GetDlgItem(IDC_EDIT_MBB_RANGE)->ShowWindow(!bPx);
	}
	else
	{
		GetDlgItem(IDC_STATIC_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SELECT_UNIT2)->ShowWindow(FALSE);
	}
	UpdateLanguage();

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab4GMBB::UpdateUI()
{
	UpdateCount_MBB();
	UpdateTeachingImage();

	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA1);
	pGO->Reset();

	COLORBOX clrBox;
	clrBox.CreateObject(PDC_ORANGE, m_MBB.rcInspArea);
	pGO->AddDrawBox(clrBox);
}

void CTeachTab4GMBB::Default()
{
	m_MBB.Clear();

	UpdateData(FALSE);
}

void CTeachTab4GMBB::Cleanup()
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

	m_bIsTeachMBBSize = FALSE;
	m_bIsTeachMBBRotate = FALSE;
}

HBRUSH CTeachTab4GMBB::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_MBB_TOLERANCE:
	case IDC_STATIC_MBB_LENGTH:
	case IDC_STATIC_MBB_SPEC:
	case IDC_STATIC_MBB_RANGE:
	case IDC_STATIC_MATCHING_SCORE:
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

BOOL CTeachTab4GMBB::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab4GMBB::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab4GMBB::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab4GMBB::OnBnClickedRadioMbbBypass()
{
	UpdateData(TRUE);
}

void CTeachTab4GMBB::OnBnClickedBtnTestMbb()
{
#ifdef RELEASE_4G
	WRITE_LOG( WL_BTN, _T("CTeachTab4GMBB::OnBnClickedBtnTestMbb") );

	CString strModelName = CModelInfo::Instance()->GetModelNameMBB();
	if (strModelName == _T("CALIBRATION"))
		CVisionSystem::Instance()->m_bAcm[ACM_MBB] = TRUE;

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA1 );
	CVisionSystem::Instance()->StartInspection( InspectTypeMBB, IDX_AREA1, 0, TRUE);
#endif
}

void CTeachTab4GMBB::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_4G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(MBB_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_MBB);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameMBB();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameMBB();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameMBB();
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

void CTeachTab4GMBB::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("4G_MBB\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameMBB();
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

void CTeachTab4GMBB::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab4GMBB::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameMBB();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		CModelInfo::Instance()->Load(strSelectModelName, MBB_KIND);
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, MBB_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab4GMBB::OnBnClickedBtnSave :: End"));
}

void CTeachTab4GMBB::DisableWnd(BOOL bEnable)
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

void CTeachTab4GMBB::OnBnClickedBtnMbbRecipeNo()
{
#ifdef RELEASE_4G
	WRITE_LOG(WL_MSG, _T("CTeachTab4GMBB::OnBnClickedBtnMbbRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab4GMBB::OnBnClickedBtnMbbRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( MBB_KIND, TEACH_TAB_IDX_MBB );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 4G MBB") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab4GMBB::OnBnClickedBtnMbbRecipeNo :: End"));
#endif
}

void CTeachTab4GMBB::OnBnClickedBtnRoiMbb()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab4GMBB::OnBnClickedBtnRoiMbb"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTeachMBBSize)
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

		m_btnTeachMBB.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("MBB 보다 조금 크게 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the area slightly larger than the MBB."));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
}

void APIENTRY CTeachTab4GMBB::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab4GMBB* pThis = (CTeachTab4GMBB*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab4GMBB::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
{
	COLORBOX clrBox;

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	CxImageObject* pImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex);
	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, nViewIndex);

	CRect rcTrack = rcTrackRegion;
	if (rcTrack.left > rcTrack.right)
	{
		if (rcTrack.top > rcTrack.bottom)
			rcTrackRegion.SetRect(rcTrack.right, rcTrack.bottom, rcTrack.left, rcTrack.top);
		else
			rcTrackRegion.SetRect(rcTrack.right, rcTrack.top, rcTrack.left, rcTrack.bottom);
	}
	else
	{
		if (rcTrack.top > rcTrack.bottom)
			rcTrackRegion.SetRect(rcTrack.left, rcTrack.bottom, rcTrack.right, rcTrack.top);
	}

	pGO->Reset();

	BOOL bRet = FALSE;

	if (m_bIsTeachMBBSize)
	{
		m_MBB.rcInspArea = rcTrackRegion;

		UpdateUI();
		bRet = TRUE;
	}
	else if (m_bIsTeachMBBRotate)
	{
#ifdef RELEASE_4G
		bRet = pInspectionVision->SetLearnModel_ForMBB(pGO, pImgObj, rcTrackRegion, InspectTypeMBB, nViewIndex, m_nModelNum, FALSE);
#endif
	}

	if (bRet)
	{
		m_MBB.ptOffset_MBB[m_nModelNum] = rcTrackRegion.CenterPoint();

		Cleanup();
		UpdateData(FALSE);

		Save();
	}
}

void CTeachTab4GMBB::OnBnClickedBtnMbbTechModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab4GMBB::OnBnClickedBtnMbbTechModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTeachMBBRotate)
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

		m_btnMBBTechModel.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("등록할 모델의 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the area of the model to be registered."));
		
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTab4GMBB::UpdateTeachingImage()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return;

#ifdef RELEASE_4G
	USES_CONVERSION;
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA1, m_nModelNum);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if (MatchBuff->GetPatternLearnt())
	{
		int nWidth = MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForMBBTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		strLastRecipe = CModelInfo::Instance()->GetLastModelNameMBB();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("4G_MBB\\") + strLastRecipe + _T("\\");
		strInspName = _T("MBB");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, m_nModelNum);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForMBBTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForMBBTeaching.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImageObjectForMBBTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForMBBTeaching.ImageUpdate();

	m_pMainView->UpdateImageView();

#endif
}

void CTeachTab4GMBB::UpdateCount_MBB()
{
	m_Combo_SelectModelNum.ResetContent();

	for (int i = 0; i < nMATCH_MAX; ++i)
	{
		CString strModelNum;
		strModelNum.Format(_T("Model_%d"), i);
		m_Combo_SelectModelNum.AddString(strModelNum);
	}

	m_Combo_SelectModelNum.SetCurSel(m_nModelNum);
}

void CTeachTab4GMBB::OnBnClickedRadioSelectUnit()
{
	UpdateData(TRUE);
}

void CTeachTab4GMBB::EnableMBB()
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

void CTeachTab4GMBB::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_4G);

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
				strParams.Format(_T("\"file:///%s#page=52\""), (LPCTSTR)strPdfPath);

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

void CTeachTab4GMBB::OnCbnSelchangeComboMBBModelNum()
{
	m_nModelNum = m_Combo_SelectModelNum.GetCurSel();

	UpdateTeachingImage();

	UpdateData(FALSE);
}
