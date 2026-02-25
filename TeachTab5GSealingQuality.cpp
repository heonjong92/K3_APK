// TeachTab5GSealingQuality.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab5GSealingQuality.h"
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
#include <XGraphic/xGraphicObject.h>

// CTeachTab5GSealingQuality 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab5GSealingQuality, CDialog)

BOOL CTeachTab5GSealingQuality::m_bPreview = FALSE;

CTeachTab5GSealingQuality::CTeachTab5GSealingQuality(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab5GSealingQuality::IDD, pParent)
	, m_bIsTeachSealingQuality(FALSE)
	, m_bIsPreview(FALSE)
{
	m_pMainView = NULL;
	m_Sealing.Clear();

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab5GSealingQuality::~CTeachTab5GSealingQuality()
{
}

void CTeachTab5GSealingQuality::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_5G,							m_wndLabelTitle5G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,						m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_SEALING_QUALITY,			m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_SEALING_QUALITY,			m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_SEALING_BYPASS,					m_Sealing.nUseBypass_Sealing);

	DDX_Text(pDX,		IDC_EDIT_SEALING_QUALITY_DESCRIPTION,		m_Sealing.strDescription);
	DDX_Text(pDX,		IDC_EDIT_SEALINGQUALITY_LED_LIGHT_VALUE,	m_Sealing.nValueCh1);
	
	DDX_Radio(pDX,		IDC_RADIO_SELECT_UNIT,						m_Sealing.nSelectUnit);
	DDX_Text(pDX,		IDC_EDIT_SEALINGQUALITY_COUNT,				m_Sealing.nCount);
	DDX_Text(pDX,		IDC_EDIT_SEALINGQUALITY_INSP_SPEC,			m_Sealing.nInspSpec);

	DDX_Control(pDX,	IDC_BTN_TEST_SEALING_QUALITY,				m_btnTest_SealingQuality);
	DDX_Control(pDX,	IDC_BTN_SEALING_QUALITY_RECIPE_NO,			m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_SEALING_QUALITY,				m_btnSave);
	
	DDX_Check(pDX,		IDC_BTN_SEALING_TEACH_MODEL, 				m_bIsTeachSealingQuality);
	DDX_Control(pDX,	IDC_BTN_SEALING_TEACH_MODEL, 				m_btnSealingTeachModel);

	DDX_Control(pDX,	IDC_CHECK_VIEW_CONVERTED_IMAGE,				m_btnPreview);
	DDX_Check(pDX,		IDC_CHECK_VIEW_CONVERTED_IMAGE,				m_bIsPreview);
}

BEGIN_MESSAGE_MAP(CTeachTab5GSealingQuality, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_RADIO_SEALING_BYPASS,				&CTeachTab5GSealingQuality::OnBnClickedRadioSealingBypass)
	ON_BN_CLICKED(IDC_RADIO_SEALING_BYPASS2,			&CTeachTab5GSealingQuality::OnBnClickedRadioSealingBypass)
	ON_BN_CLICKED(IDC_RADIO_SEALING_BYPASS3,			&CTeachTab5GSealingQuality::OnBnClickedRadioSealingBypass)

	ON_BN_CLICKED(IDC_BTN_TEST_SEALING_QUALITY,			&CTeachTab5GSealingQuality::OnBnClickedBtnTestSealingQuality)
	ON_BN_CLICKED(IDC_BTN_SAVE_SEALING_QUALITY,			&CTeachTab5GSealingQuality::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_SEALING_QUALITY_RECIPE_NO,	&CTeachTab5GSealingQuality::OnBnClickedBtnSealingQualityRecipeNo)
	ON_BN_CLICKED(IDC_BTN_SEALING_TEACH_MODEL,			&CTeachTab5GSealingQuality::OnBnClickedBtnSealingTeachModel)

	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_SEALING_QUALITY,	&CTeachTab5GSealingQuality::OnCbnSelchangeComboRecipe)

	ON_BN_CLICKED(IDC_CHECK_VIEW_CONVERTED_IMAGE,		&CTeachTab5GSealingQuality::OnBnClickedCheckViewConvertedImage)
	ON_BN_CLICKED(IDC_RADIO_SELECT_UNIT,				&CTeachTab5GSealingQuality::OnBnClickedRadioSelectUnit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab5GSealingQuality 메시지 처리기입니다.
BOOL CTeachTab5GSealingQuality::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle5G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle5G.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_SealingQuality.SetColor( DEF_BTN_COLOR_MEASURE );

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab5GSealingQuality::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_5G					)->SetWindowText(CLanguageInfo::Instance()->ReadString(41));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(42));
	GetDlgItem(IDC_BTN_SEALING_QUALITY_RECIPE_NO 	)->SetWindowText(CLanguageInfo::Instance()->ReadString(43));
	GetDlgItem(IDC_RADIO_SEALING_BYPASS				)->SetWindowText(CLanguageInfo::Instance()->ReadString(44));
	GetDlgItem(IDC_RADIO_SEALING_BYPASS2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(45));
	GetDlgItem(IDC_RADIO_SEALING_BYPASS3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(46));
	GetDlgItem(IDC_STATIC_DESCRIPTION				)->SetWindowText(CLanguageInfo::Instance()->ReadString(47));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(48));
	GetDlgItem(IDC_BTN_TEST_SEALING_QUALITY			)->SetWindowText(CLanguageInfo::Instance()->ReadString(49));
	GetDlgItem(IDC_BTN_SEALING_TEACH_MODEL			)->SetWindowText(CLanguageInfo::Instance()->ReadString(50));
	GetDlgItem(IDC_STATIC_SEALING_SPEC				)->SetWindowText(CLanguageInfo::Instance()->ReadString(51));
	GetDlgItem(IDC_CHECK_VIEW_CONVERTED_IMAGE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(52));
	GetDlgItem(IDC_STATIC_SEALING_ISNP_UNIT			)->SetWindowText(CLanguageInfo::Instance()->ReadString(53));
	
	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle5G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab5GSealingQuality::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SEALINGQUALITY_COUNT), _T("표면의 굴곡 검출 퍼센트 Spec입니다. Spec 기준 퍼센트 미달이면 Loose Sealing으로 판단하여 NG 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SEALINGQUALITY_INSP_SPEC), _T("검사 기준 Spec입니다. User는 해당 값을 수정할 수 없으며 해당 값이 아래 퍼센트 계산의 기준인 100%의 값 입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_SEALING_TEACH_MODEL), _T("Sealing 구겨짐 검사를 진행할 영역을 Teaching 합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_VIEW_CONVERTED_IMAGE), _T("Sobel 연산으로 변환 된 검사 이미지를 Viewer에서 확인할 수 있습니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab5GSealingQuality::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab5GSealingQuality::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stSealingInfo& SealingInfo = CModelInfo::Instance()->GetSealingInfo();
	SealingInfo = m_Sealing;

#ifdef RELEASE_5G	
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_SEALING );
#endif
	
	Refresh();

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab5GSealingQuality::CheckData()
{
#ifdef RELEASE_5G
	CString strLog;
	CModelInfo::stSealingInfo& SealingInfo = CModelInfo::Instance()->GetSealingInfo();

	// ----- SealingQuality Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[SealingInfo.nUseBypass_Sealing], strBypassName[m_Sealing.nUseBypass_Sealing] );
	if( SealingInfo.nUseBypass_Sealing != m_Sealing.nUseBypass_Sealing) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeSealing,	strLog );
	strLog.Format(_T("[Description][%s→%s]"), SealingInfo.strDescription, m_Sealing.strDescription);
	if (SealingInfo.strDescription != m_Sealing.strDescription)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
	strLog.Format(_T("[LightValueCh1][%d→%d]"), SealingInfo.nValueCh1, m_Sealing.nValueCh1);
	if (SealingInfo.nValueCh1 != m_Sealing.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
	strLog.Format(_T("[nCount][%d→%d]"), SealingInfo.nCount, m_Sealing.nCount);
	if (SealingInfo.nCount != m_Sealing.nCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
	strLog.Format(_T("[nInspRoiTop][%d→%d]"), SealingInfo.nInspSpec, m_Sealing.nInspSpec);
	if (SealingInfo.nInspSpec != m_Sealing.nInspSpec) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);

	strLog.Format(_T("[Area_Left][%d→%d]"), SealingInfo.rcInspArea.left, m_Sealing.rcInspArea.left);
	if (SealingInfo.rcInspArea.left != m_Sealing.rcInspArea.left) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
	strLog.Format(_T("[Area_Right][%d→%d]"), SealingInfo.rcInspArea.right, m_Sealing.rcInspArea.right);
	if (SealingInfo.rcInspArea.right != m_Sealing.rcInspArea.right) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
	strLog.Format(_T("[Area_Top][%d→%d]"), SealingInfo.rcInspArea.top, m_Sealing.rcInspArea.top);
	if (SealingInfo.rcInspArea.top != m_Sealing.rcInspArea.top) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
	strLog.Format(_T("[Area_Bottom][%d→%d]"), SealingInfo.rcInspArea.bottom, m_Sealing.rcInspArea.bottom);
	if (SealingInfo.rcInspArea.bottom != m_Sealing.rcInspArea.bottom) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeSealing, strLog);
#endif
}

void CTeachTab5GSealingQuality::Refresh()
{
	UpdateRecipeList();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(CModelInfo::Instance()->GetModelNameSealingQuality(), SEALINGQUALITY_KIND);

	CModelInfo::stSealingInfo& SealingInfo = CModelInfo::Instance()->GetSealingInfo();
	m_Sealing = SealingInfo;

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab5GSealingQuality::UpdateUI()
{
#ifdef RELEASE_5G
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
		GetDlgItem(IDC_EDIT_SEALINGQUALITY_INSP_SPEC)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_SEALINGQUALITY_INSP_SPEC)->EnableWindow(FALSE);

	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA2);
	pGO->Reset();

	COLORBOX clrBox;
	clrBox.CreateObject(PDC_ORANGE, m_Sealing.rcInspArea);
	pGO->AddDrawBox(clrBox);
#endif
}

void CTeachTab5GSealingQuality::Default()
{
	m_Sealing.Clear();

	UpdateData(FALSE);
}

void CTeachTab5GSealingQuality::Cleanup()
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

#ifdef RELEASE_5G
	if (m_bPreview)
	{
		m_bPreview = FALSE;
		CVisionSystem::Instance()->SetPreviewGainOffset(IDX_AREA2, m_bPreview);
	}
#endif

	m_bIsTeachSealingQuality = FALSE;
	m_bIsPreview = FALSE;
}

HBRUSH CTeachTab5GSealingQuality::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_SEALING_SPEC:
	case IDC_CHECK_VIEW_CONVERTED_IMAGE:
	case IDC_STATIC_SEALING_ISNP_SPEC:
	case IDC_STATIC_SEALING_ISNP_UNIT:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab5GSealingQuality::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab5GSealingQuality::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab5GSealingQuality::PreTranslateMessage(MSG* pMsg)
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

void APIENTRY CTeachTab5GSealingQuality::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab5GSealingQuality* pThis = (CTeachTab5GSealingQuality*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab5GSealingQuality::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
{
#ifdef RELEASE_5G
	COLORBOX clrBox;

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	CxImageObject *pImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex);
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
		if (rcTrack.top > rcTrack.bottom)
			rcTrackRegion.SetRect(rcTrack.left, rcTrack.bottom, rcTrack.right, rcTrack.top);

	pGO->Reset();

	BOOL bRet = FALSE;

	if (m_bIsTeachSealingQuality)
	{
		m_Sealing.rcInspArea = rcTrackRegion;

		clrBox.CreateObject(PDC_ORANGE, m_Sealing.rcInspArea);
		pGO->AddDrawBox(clrBox);

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

void CTeachTab5GSealingQuality::OnBnClickedRadioSealingBypass()
{
	UpdateData(TRUE);
}

void CTeachTab5GSealingQuality::OnBnClickedBtnTestSealingQuality()
{
#ifdef RELEASE_5G
	WRITE_LOG( WL_BTN, _T("CTeachTab5GSealingQuality::OnBnClickedBtnTestSealingQuality") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA2 );
	CVisionSystem::Instance()->StartInspection( InspectTypeSealing, IDX_AREA2, 0, TRUE );
#endif
}

void CTeachTab5GSealingQuality::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_5G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(SEALINGQUALITY_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_SEALING);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameSealingQuality();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameSealingQuality();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameSealingQuality();
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

void CTeachTab5GSealingQuality::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("5G_Sealing\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameSealingQuality();
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

void CTeachTab5GSealingQuality::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab5GSealingQuality::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameSealingQuality();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, SEALINGQUALITY_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, SEALINGQUALITY_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab5GSealingQuality::OnBnClickedBtnSave :: End"));
}

void CTeachTab5GSealingQuality::DisableWnd(BOOL bEnable)
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


void CTeachTab5GSealingQuality::OnBnClickedBtnSealingQualityRecipeNo()
{
#ifdef RELEASE_5G
	WRITE_LOG(WL_MSG, _T("CTeachTab5GSealingQuality::OnBnClickedBtnSealingQualityRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab5GSealingQuality::OnBnClickedBtnSealingQualityRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( SEALINGQUALITY_KIND, TEACH_TAB_IDX_SEALING );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 5G SEALING QUALITY") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab5GSealingQuality::OnBnClickedBtnSealingQualityRecipeNo :: End"));
#endif
}

void CTeachTab5GSealingQuality::OnBnClickedTeachingSealingQuality()
{
	UpdateData(TRUE);

	UpdateUI();
}

void CTeachTab5GSealingQuality::OnBnClickedBtnSealingTeachModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab5GDesiccantMaterial::OnBnClickedBtnSealingTeachModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

#ifdef RELEASE_5G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

	if (m_bIsTeachSealingQuality)
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
		
		m_btnSealingTeachModel.EnableWindow(TRUE);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("박스 내부 MBB 영역을 지정하세요."));
		else												AfxMessageBox(_T("Please specify the MBB area inside the box."));

		m_pMainView->SetTrackerMode(TRUE, IDX_AREA2, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void CTeachTab5GSealingQuality::OnBnClickedCheckViewConvertedImage()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab5GSealingQuality::OnBnClickedCheckViewConvertedImage"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	if (m_bIsPreview)
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

		m_btnPreview.EnableWindow(TRUE);
		m_bPreview = TRUE;
#ifdef RELEASE_5G
		CVisionSystem::Instance()->SetPreviewGainOffset(IDX_AREA2, m_bPreview);
#endif
	}
	else
	{
		Cleanup();
	}
}

void CTeachTab5GSealingQuality::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_5G);

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
				strParams.Format(_T("\"file:///%s#page=60\""), (LPCTSTR)strPdfPath);

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

void CTeachTab5GSealingQuality::OnBnClickedRadioSelectUnit()
{
	UpdateData(TRUE);
}
