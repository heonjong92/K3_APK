// TeachTab2GHIC.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab2GHIC.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "StringMacro.h"

#include "AccessRightsDlg.h"
#include "UIExt/ResourceManager.h"

#include <XUtil/xUtils.h>
#include <XGraphic\xGraphicObject.h>

// CTeachTab2GHIC 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab2GHIC, CDialog)

CTeachTab2GHIC::CTeachTab2GHIC(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab2GHIC::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsTeachHIC(FALSE)
{
	m_pMainView = NULL;
	m_HIC.Clear();
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab2GHIC::~CTeachTab2GHIC()
{
}

void CTeachTab2GHIC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_2G, 					m_wndLabelTitle2G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,					m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_HIC,					m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_HIC,					m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_HIC_BYPASS, 					m_HIC.nUseBypass_HIC);
	DDX_Radio(pDX,		IDC_RADIO_HIC_RED,						m_HIC.nDisColor);
	DDX_Text(pDX,		IDC_EDIT_HIC_DESCRIPTION,				m_HIC.strDescription);

	DDX_Text(pDX,		IDC_EDIT_HIC_LED_LIGHT_VALUE,			m_HIC.nValueCh1);
	
	DDX_Text(pDX,		IDC_EDIT_HIC_MATCHING_SCORE,			m_HIC.fRatio);
	DDX_Text(pDX,		IDC_EDIT_HIC_COLOR_TIP_RED,				m_HIC.nColorRedLuminance);
	DDX_Text(pDX,		IDC_EDIT_HIC_COLOR_TIP_GREEN,			m_HIC.nColorGreenLuminance);
	DDX_Text(pDX,		IDC_EDIT_HIC_COLOR_TIP_BLUE,			m_HIC.nColorBlueLuminance);
	
	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_TEACH_HIC,			m_ctrlPicPreviewTeachHIC);

	DDX_Text(pDX,		IDC_EDIT_HIC_CIRCLE_OUTER_DIAMETER_60,	m_HIC.nCircleOuterDiameter60);
	DDX_Text(pDX,		IDC_EDIT_HIC_CIRCLE_OUTER_DIAMETER_10,	m_HIC.nCircleOuterDiameter10);
	DDX_Text(pDX,		IDC_EDIT_HIC_CIRCLE_OUTER_DIAMETER_5,	m_HIC.nCircleOuterDiameter5);

	DDX_Text(pDX,		IDC_EDIT_HIC_CIRCLE_INNER_DIAMETER_60,	m_HIC.nCircleInnerDiameter60);
	DDX_Text(pDX,		IDC_EDIT_HIC_CIRCLE_INNER_DIAMETER_10,	m_HIC.nCircleInnerDiameter10);
	DDX_Text(pDX,		IDC_EDIT_HIC_CIRCLE_INNER_DIAMETER_5,	m_HIC.nCircleInnerDiameter5);

	DDX_Text(pDX,		IDC_EDIT_HIC_RED_VALUE,					m_HIC.nPixelAvgValue);
	DDV_MinMaxInt(pDX,	m_HIC.nPixelAvgValue,					GV8_MIN, GV8_MAX);

	DDX_Control( pDX,	IDC_BTN_TEST_HIC, 						m_btnTest_HIC );
	DDX_Control(pDX,	IDC_BTN_HIC_RECIPE_NO,					m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_HIC,						m_btnSave);
	
	DDX_Control(pDX,	IDC_BTN_HIC_TECH_MODEL,					m_btnHICTechModel);
	DDX_Check(pDX,		IDC_BTN_HIC_TECH_MODEL,					m_bIsTeachHIC);
}

BEGIN_MESSAGE_MAP(CTeachTab2GHIC, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_HIC_BYPASS,			&CTeachTab2GHIC::OnBnClickedRadioHicBypass)
	ON_BN_CLICKED(IDC_RADIO_HIC_BYPASS2,		&CTeachTab2GHIC::OnBnClickedRadioHicBypass)
	ON_BN_CLICKED(IDC_RADIO_HIC_BYPASS3,		&CTeachTab2GHIC::OnBnClickedRadioHicBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_HIC,				&CTeachTab2GHIC::OnBnClickedBtnTestHic)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_HIC,		&CTeachTab2GHIC::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_HIC,				&CTeachTab2GHIC::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_HIC_RECIPE_NO,		&CTeachTab2GHIC::OnBnClickedBtnHICRecipeNo)
	ON_BN_CLICKED(IDC_BTN_HIC_TECH_MODEL,		&CTeachTab2GHIC::OnBnClickedBtnHicTechModel)
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab2GHIC 메시지 처리기입니다.
BOOL CTeachTab2GHIC::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle2G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle2G.SetFontHeight( 12 );

	m_ctrlPicPreviewTeachHIC.GetClientRect(m_rcPicCtrlSizeTeachHIC);

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_HIC.SetColor( DEF_BTN_COLOR_MEASURE );

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

	m_ImageViewForHICTeaching.Create(&m_ImageViewManagerForHICTeaching, this);
	m_ImageViewForHICTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForHICTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForHICTeaching.SetAnimateWindow(FALSE);
	m_ImageViewForHICTeaching.SetImageObject(&m_ImageObjectForHICTeaching, FALSE);
	m_ImageViewForHICTeaching.SetRegisterCallBack(0, &regCB);
	m_ImageViewForHICTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForHICTeaching.SetBodyColor(dwBodyColor);
	m_ImageViewForHICTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForHICTeaching.ShowScrollBar(FALSE);
	m_ImageViewForHICTeaching.ShowScaleBar(FALSE);
	m_ImageViewForHICTeaching.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_HIC)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(0);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForHICTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	CModelInfo::stHICInfo& stHIC = CModelInfo::Instance()->GetHICInfo();

	UpdateRecipeList();

	Refresh();
	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab2GHIC::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_2G						)->SetWindowText(CLanguageInfo::Instance()->ReadString(31));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(32));
	GetDlgItem(IDC_BTN_HIC_RECIPE_NO					)->SetWindowText(CLanguageInfo::Instance()->ReadString(33));
	GetDlgItem(IDC_RADIO_HIC_BYPASS						)->SetWindowText(CLanguageInfo::Instance()->ReadString(34));
	GetDlgItem(IDC_RADIO_HIC_BYPASS2					)->SetWindowText(CLanguageInfo::Instance()->ReadString(35));
	GetDlgItem(IDC_RADIO_HIC_BYPASS3					)->SetWindowText(CLanguageInfo::Instance()->ReadString(36));
	GetDlgItem(IDC_STATIC_DESCRIPTION					)->SetWindowText(CLanguageInfo::Instance()->ReadString(37));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE					)->SetWindowText(CLanguageInfo::Instance()->ReadString(38));
	GetDlgItem(IDC_STATIC_OUTER_CIRCLE_DIAMETER			)->SetWindowText(CLanguageInfo::Instance()->ReadString(39));
	GetDlgItem(IDC_STATIC_INNER_CIRCLE_DIAMETER			)->SetWindowText(CLanguageInfo::Instance()->ReadString(40));
	GetDlgItem(IDC_STATIC_PIXEL_RED_VALUE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(41));
	GetDlgItem(IDC_STATIC_HIC_DISCOLOR					)->SetWindowText(CLanguageInfo::Instance()->ReadString(42));
	GetDlgItem(IDC_BTN_TEST_HIC							)->SetWindowText(CLanguageInfo::Instance()->ReadString(43));
	GetDlgItem(IDC_STATIC_HIC_COLOR_TIP_AVG_LUMINANCE	)->SetWindowText(CLanguageInfo::Instance()->ReadString(44));
	GetDlgItem(IDC_STATIC_MATCHING_SCORE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(45));
	GetDlgItem(IDC_BTN_HIC_TECH_MODEL					)->SetWindowText(CLanguageInfo::Instance()->ReadString(46));

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle2G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab2GHIC::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_RED), _T("Color Red Tip 값 보다 +4~+5 높게 설정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_GREEN), _T("Color Green Tip 값 보다 +4~+5 높게 설정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_BLUE), _T("Color Blue Tip 값 보다 +4~+5 높게 설정합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_CIRCLE_OUTER_DIAMETER_60), _T("HIC Circle의 외부 원 (60%) 지름을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_CIRCLE_OUTER_DIAMETER_10), _T("HIC Circle의 외부 원 (10%) 지름을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_CIRCLE_OUTER_DIAMETER_5), _T("HIC Circle의 외부 원 (5%) 지름을 입력합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_CIRCLE_INNER_DIAMETER_60), _T("HIC Circle의 내부 원 (60%) 지름을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_CIRCLE_INNER_DIAMETER_10), _T("HIC Circle의 내부 원 (10%) 지름을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_CIRCLE_INNER_DIAMETER_5), _T("HIC Circle의 내부 원 (5%) 지름을 입력합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_HIC_RED), _T("변색 검사에 사용할 색상을 선택합니다. (Red)"));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_HIC_GREEN), _T("변색 검사에 사용할 색상을 선택합니다. (Green)"));
	m_toolTip.AddTool(GetDlgItem(IDC_RADIO_HIC_BLUE), _T("변색 검사에 사용할 색상을 선택합니다. (Blue)"));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_RED_VALUE), _T("내부 원 지름에 속하는 Pixels의 색상 Value 값의 평균을 입력합니다.평균 값이 입력한 값 이상이면 NG가 발생합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_HIC_MATCHING_SCORE), _T("HIC 종류 검사 입니다. 모델 검사 일치율(50~100)을 입력하세요."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_HIC_TECH_MODEL), _T("HIC 종류를 구분하기 위한 모델을 등록해주세요."));

	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab2GHIC::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab2GHIC::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stHICInfo& HICInfo = CModelInfo::Instance()->GetHICInfo();
	HICInfo = m_HIC;

#ifdef RELEASE_2G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameHIC(), TEACH_TAB_IDX_HIC);
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_HIC );
#endif
	
	UpdateData(FALSE);
	Refresh();

	return TRUE;
}

void CTeachTab2GHIC::CheckData()
{
#ifdef RELEASE_2G
	CString strLog;
	CModelInfo::stHICInfo& HICInfo = CModelInfo::Instance()->GetHICInfo();

	// ----- HIC Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[HICInfo.nUseBypass_HIC], strBypassName[m_HIC.nUseBypass_HIC] );
	if( HICInfo.nUseBypass_HIC != m_HIC.nUseBypass_HIC) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeHIC,	strLog );
	strLog.Format(_T("[Description][%s→%s]"), HICInfo.strDescription, m_HIC.strDescription);
	if (HICInfo.strDescription != m_HIC.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);

	strLog.Format(_T("[LightValueCh1][%d→%d]"), HICInfo.nValueCh1, m_HIC.nValueCh1);
	if (HICInfo.nValueCh1 != m_HIC.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);

	strLog.Format(_T("[CircleOuterDiameter60][%d→%d]"), HICInfo.nCircleOuterDiameter60, m_HIC.nCircleOuterDiameter60);
	if (HICInfo.nCircleOuterDiameter60 != m_HIC.nCircleOuterDiameter60) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);
	strLog.Format(_T("[CircleOuterDiameter10][%d→%d]"), HICInfo.nCircleOuterDiameter10, m_HIC.nCircleOuterDiameter10);
	if (HICInfo.nCircleOuterDiameter10 != m_HIC.nCircleOuterDiameter10) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);
	strLog.Format(_T("[CircleOuterDiameter5][%d→%d]"), HICInfo.nCircleOuterDiameter5, m_HIC.nCircleOuterDiameter5);
	if (HICInfo.nCircleOuterDiameter5 != m_HIC.nCircleOuterDiameter5) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);

	strLog.Format(_T("[CircleInnerDiameter60][%d→%d]"), HICInfo.nCircleInnerDiameter60, m_HIC.nCircleInnerDiameter60);
	if (HICInfo.nCircleInnerDiameter60 != m_HIC.nCircleInnerDiameter60) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);
	strLog.Format(_T("[CircleInnerDiameter10][%d→%d]"), HICInfo.nCircleInnerDiameter10, m_HIC.nCircleInnerDiameter10);
	if (HICInfo.nCircleInnerDiameter10 != m_HIC.nCircleInnerDiameter10) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);
	strLog.Format(_T("[CircleInnerDiameter5][%d→%d]"), HICInfo.nCircleInnerDiameter5, m_HIC.nCircleInnerDiameter5);
	if (HICInfo.nCircleInnerDiameter5 != m_HIC.nCircleInnerDiameter5) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);

	strLog.Format(_T("[DisColor][%d→%d]"), HICInfo.nDisColor, m_HIC.nDisColor);
	if (HICInfo.nDisColor != m_HIC.nDisColor) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);

	strLog.Format(_T("[PixelAvgValue][%d→%d]"), HICInfo.nPixelAvgValue, m_HIC.nPixelAvgValue);
	if (HICInfo.nPixelAvgValue != m_HIC.nPixelAvgValue) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeHIC, strLog);
#endif
}

void CTeachTab2GHIC::Refresh()
{
	UpdateRecipeList();
	CString strModelName = CModelInfo::Instance()->GetModelNameHIC();
	CModelInfo::stHICInfo& HICInfo = CModelInfo::Instance()->GetHICInfo();
	m_HIC = HICInfo;

	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
	if (strSelectModelName == strModelName)
		EnableHICLight();

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab2GHIC::Default()
{
	m_HIC.Clear();

	UpdateData(FALSE);
}

void CTeachTab2GHIC::Cleanup()
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

	m_bIsTeachHIC = FALSE;
}

HBRUSH CTeachTab2GHIC::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_OUTER_CIRCLE_DIAMETER:
	case IDC_STATIC_INNER_CIRCLE_DIAMETER:
	case IDC_STATIC_PIXEL_RED_VALUE:
	case IDC_STATIC_HIC_DISCOLOR:
	case IDC_STATIC_HIC_COLOR_TIP_AVG_LUMINANCE:
	case IDC_STATIC_MATCHING_SCORE:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab2GHIC::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab2GHIC::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab2GHIC::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab2GHIC::OnBnClickedRadioHicBypass()
{
	UpdateData( TRUE );
}

void CTeachTab2GHIC::OnBnClickedBtnTestHic()
{
#ifdef RELEASE_2G
	WRITE_LOG( WL_BTN, _T("CTeachTab2GHIC::OnBnClickedBtnTestHic") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA2 );
	CVisionSystem::Instance()->StartInspection( InspectTypeHIC, IDX_AREA2, 0, TRUE);
#endif
}

void CTeachTab2GHIC::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_2G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(HIC_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_HIC);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameHIC();
			
			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameHIC();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameHIC();
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

void CTeachTab2GHIC::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("2G_HIC\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameHIC();
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

void CTeachTab2GHIC::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab2GHIC::OnBnClickedBtnSave :: Start"));

	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameHIC();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, HIC_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, HIC_KIND);
		}

		DisableWnd(TRUE);
		Refresh();

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab2GHIC::OnBnClickedBtnSave :: End"));
}

void CTeachTab2GHIC::DisableWnd(BOOL bEnable)
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

void CTeachTab2GHIC::OnBnClickedBtnHICRecipeNo()
{
#ifdef RELEASE_2G
	WRITE_LOG(WL_MSG, _T("CTeachTab2GHIC::OnBnClickedBtnHICRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab2GHIC::OnBnClickedBtnHICRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( HIC_KIND, TEACH_TAB_IDX_HIC);
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 2G HIC") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab2GHIC::OnBnClickedBtnHICRecipeNo :: End"));
#endif
}

void CTeachTab2GHIC::EnableHICLight()
{
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_EDIT_HIC_LED_LIGHT_VALUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_RED)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_GREEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_BLUE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_HIC_LED_LIGHT_VALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_RED)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_GREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HIC_COLOR_TIP_BLUE)->EnableWindow(FALSE);
	}
}

void CTeachTab2GHIC::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_2G);

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
				strParams.Format(_T("\"file:///%s#page=42\""), (LPCTSTR)strPdfPath);

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

void APIENTRY CTeachTab2GHIC::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab2GHIC* pThis = (CTeachTab2GHIC*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab2GHIC::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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

#ifdef RELEASE_2G
	if (m_bIsTeachHIC)
	{
		bRet = pInspectionVision->SetLearnModel(pGO, pImgObj, rcTrackRegion, InspectTypeHIC, nViewIndex, 0, FALSE, TRUE);
		m_HIC.ptOffset_HIC = rcTrackRegion.CenterPoint();
	}
#endif

	if (bRet)
	{
		Cleanup();
		UpdateData(FALSE);
	}
}

void CTeachTab2GHIC::OnBnClickedBtnHicTechModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab2GHIC::OnBnClickedBtnHicTechModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_2G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

	if (m_bIsTeachHIC)
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

		m_btnHICTechModel.EnableWindow(TRUE);
		AfxMessageBox(_T("등록할 모델의 영역을 지정하세요."));
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA2, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab2GHIC::UpdateUI()
{
	UpdateTeachingImage();
}

void CTeachTab2GHIC::UpdateTeachingImage()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return;

#ifdef RELEASE_2G
	USES_CONVERSION;
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA2, 0);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if (MatchBuff->GetPatternLearnt())
	{
		int nWidth = MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForHICTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		strLastRecipe = CModelInfo::Instance()->GetLastModelNameHIC();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("2G_HIC\\") + strLastRecipe + _T("\\");
		strInspName = _T("HIC");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, 0);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForHICTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForHICTeaching.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImageObjectForHICTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForHICTeaching.ImageUpdate();

	m_pMainView->UpdateImageView();

#endif
}