// TeachTab2GBanding.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab2GBanding.h"
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

// CTeachTab2GBanding 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab2GBanding, CDialog)

CTeachTab2GBanding::CTeachTab2GBanding(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab2GBanding::IDD, pParent)
	, m_bIsTeachBanding(FALSE)
{
	m_pMainView = NULL;
	m_Banding.Clear();
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab2GBanding::~CTeachTab2GBanding()
{
}

void CTeachTab2GBanding::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_2G, 					m_wndLabelTitle2G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,					m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_BANDING,				m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_BANDING,				m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_BANDING_BYPASS, 				m_Banding.nUseBypass_Banding);
	DDX_Text(pDX,		IDC_EDIT_BANDING_DESCRIPTION,			m_Banding.strDescription);
	DDX_Text(pDX,		IDC_EDIT_BANDING_THRESHOLD,				m_Banding.nThreshold);
	DDX_Text(pDX,		IDC_EDIT_BANDING_WIDTH_SPEC,			m_Banding.nBandWidthSpec);
	DDX_Text(pDX,		IDC_EDIT_BANDING_WIDTH_RANGE,			m_Banding.nBandWidthRange);

	DDX_Text(pDX,		IDC_EDIT_BANDING_LED_LIGHT_VALUE,		m_Banding.nValueCh1);

	DDX_Control(pDX,	IDC_BTN_TEST_BANDING,					m_btnTest_Banding);
	DDX_Control(pDX,	IDC_BTN_BANDING_RECIPE_NO,				m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_BANDING,					m_btnSave);

	DDX_Control(pDX,	IDC_BTN_ROI_BANDING,					m_btnTeachBanding);
	DDX_Check(pDX,		IDC_BTN_ROI_BANDING,					m_bIsTeachBanding);

	DDV_MinMaxInt(pDX, m_Banding.nThreshold,					GV8_MIN, GV8_MAX);
}

BEGIN_MESSAGE_MAP(CTeachTab2GBanding, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_RADIO_BANDING_BYPASS,		&CTeachTab2GBanding::OnBnClickedRadioBandingBypass)
	ON_BN_CLICKED(IDC_RADIO_BANDING_BYPASS2,	&CTeachTab2GBanding::OnBnClickedRadioBandingBypass)
	ON_BN_CLICKED(IDC_RADIO_BANDING_BYPASS3,	&CTeachTab2GBanding::OnBnClickedRadioBandingBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_BANDING,			&CTeachTab2GBanding::OnBnClickedBtnTestBanding)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_BANDING,	&CTeachTab2GBanding::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_BANDING,			&CTeachTab2GBanding::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_BANDING_RECIPE_NO,	&CTeachTab2GBanding::OnBnClickedBtnBandingRecipeNo)
	ON_BN_CLICKED(IDC_BTN_ROI_BANDING, 			&CTeachTab2GBanding::OnBnClickedBtnRoiBanding)
END_MESSAGE_MAP()

// CTeachTab2GBanding 메시지 처리기입니다.
BOOL CTeachTab2GBanding::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle2G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle2G.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_Banding.SetColor(DEF_BTN_COLOR_MEASURE);

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab2GBanding::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_2G				)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_BANDING_RECIPE_NO		)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_RADIO_BANDING_BYPASS			)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_BANDING_BYPASS2		)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_BANDING_BYPASS3		)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_STATIC_DESCRIPTION			)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_STATIC_BAND_THRESHOLD		)->SetWindowText(CLanguageInfo::Instance()->ReadString(19));
	GetDlgItem(IDC_BTN_ROI_BANDING				)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_BTN_TEST_BANDING				)->SetWindowText(CLanguageInfo::Instance()->ReadString(23));
	GetDlgItem(IDC_STATIC_BAND_WIDTH_SPEC		)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_BAND_WIDTH_RANGE		)->SetWindowText(CLanguageInfo::Instance()->ReadString(22));

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle2G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab2GBanding::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BANDING_THRESHOLD), _T("Laser선의 밝기 값(Gv)을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BANDING_WIDTH_SPEC), _T("Band의 가로 폭(px)을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BANDING_WIDTH_RANGE), _T("Spec 대비 길이 오차 허용 범위를 입력합니다. 측정 길이가 해당 범위 안에 있어야 OK 판정입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_ROI_BANDING), _T("Banding의 검사 영역을 설정합니다. Band 기준으로 좌우 여유 있게 설정합니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab2GBanding::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab2GBanding::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stBandingInfo& BandingInfo = CModelInfo::Instance()->GetBandingInfo();
	BandingInfo = m_Banding;

#ifdef RELEASE_2G
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_BANDING );
#endif
	
	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab2GBanding::CheckData()
{
#ifdef RELEASE_2G
	CString strLog;
	CModelInfo::stBandingInfo& BandingInfo = CModelInfo::Instance()->GetBandingInfo();

	// ----- Banding Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[BandingInfo.nUseBypass_Banding], strBypassName[m_Banding.nUseBypass_Banding] );
	if( BandingInfo.nUseBypass_Banding != m_Banding.nUseBypass_Banding) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeBanding,	strLog );
	strLog.Format(_T("[Description][%s→%s]"), BandingInfo.strDescription, m_Banding.strDescription);
	if (BandingInfo.strDescription != m_Banding.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);

	strLog.Format(_T("[LightValueCh1][%d→%d]"), BandingInfo.nValueCh1, m_Banding.nValueCh1);
	if (BandingInfo.nValueCh1 != m_Banding.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);

	strLog.Format(_T("[Center_Area_Left][%d→%d]"), BandingInfo.rcInspArea.left, m_Banding.rcInspArea.left);
	if (BandingInfo.rcInspArea.left != m_Banding.rcInspArea.left) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);
	strLog.Format(_T("[Center_Area_Right][%d→%d]"), BandingInfo.rcInspArea.right, m_Banding.rcInspArea.right);
	if (BandingInfo.rcInspArea.right != m_Banding.rcInspArea.right) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);
	strLog.Format(_T("[Center_Area_Top][%d→%d]"), BandingInfo.rcInspArea.top, m_Banding.rcInspArea.top);
	if (BandingInfo.rcInspArea.top != m_Banding.rcInspArea.top) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);
	strLog.Format(_T("[Center_Area_Bottom][%d→%d]"), BandingInfo.rcInspArea.bottom, m_Banding.rcInspArea.bottom);
	if (BandingInfo.rcInspArea.bottom != m_Banding.rcInspArea.bottom) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);

	strLog.Format(_T("[Threshold][%d→%d]"), BandingInfo.nThreshold, m_Banding.nThreshold);
	if (BandingInfo.nThreshold != m_Banding.nThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);
	strLog.Format(_T("[Spec][%d→%d]"), BandingInfo.nBandWidthSpec, m_Banding.nBandWidthSpec);
	if (BandingInfo.nBandWidthSpec != m_Banding.nBandWidthSpec) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);
	strLog.Format(_T("[Range][%d→%d]"), BandingInfo.nBandWidthRange, m_Banding.nBandWidthRange);
	if (BandingInfo.nBandWidthRange != m_Banding.nBandWidthRange) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBanding, strLog);
#endif
}

void CTeachTab2GBanding::Refresh()
{
	UpdateRecipeList();
	CModelInfo::stBandingInfo& BandingInfo = CModelInfo::Instance()->GetBandingInfo();
	m_Banding = BandingInfo;

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab2GBanding::UpdateUI()
{
	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, IDX_AREA1);
	pGO->Reset();

	COLORBOX clrBox;
	clrBox.CreateObject(PDC_ORANGE, m_Banding.rcInspArea);
	pGO->AddDrawBox(clrBox);
}

void CTeachTab2GBanding::Default()
{
	m_Banding.Clear();

	UpdateData(FALSE);
}

void CTeachTab2GBanding::Cleanup()
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

	m_bIsTeachBanding = FALSE;
}

HBRUSH CTeachTab2GBanding::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_BAND_THRESHOLD:
	case IDC_STATIC_BAND_WIDTH_SPEC:
	case IDC_STATIC_BAND_WIDTH_RANGE:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab2GBanding::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab2GBanding::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab2GBanding::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab2GBanding::OnBnClickedRadioBandingBypass()
{
	UpdateData( TRUE );
}

void CTeachTab2GBanding::OnBnClickedBtnTestBanding()
{
#ifdef RELEASE_2G
	WRITE_LOG( WL_BTN, _T("CTeachTab2GBanding::OnBnClickedBtnTestBanding") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA1 );
	CVisionSystem::Instance()->StartInspection( InspectTypeBanding, IDX_AREA1, 0, TRUE);
#endif
}

void CTeachTab2GBanding::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_2G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(BANDING_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_BANDING);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameBanding();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameBanding();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameBanding();
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

void CTeachTab2GBanding::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("2G_Banding\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameBanding();
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

void CTeachTab2GBanding::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab2GBanding::OnBnClickedBtnSave :: Start"));

	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameBanding();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		CModelInfo::Instance()->Load(strSelectModelName, BANDING_KIND);

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab2GBanding::OnBnClickedBtnSave :: End"));
}

void CTeachTab2GBanding::DisableWnd(BOOL bEnable)
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

void CTeachTab2GBanding::OnBnClickedBtnBandingRecipeNo()
{
#ifdef RELEASE_2G
	WRITE_LOG(WL_MSG, _T("CTeachTab2GBanding::OnBnClickedBtnBandingRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab2GBanding::OnBnClickedBtnBandingRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( BANDING_KIND, TEACH_TAB_IDX_BANDING );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 2G BANDING") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab2GBanding::OnBnClickedBtnBandingRecipeNo :: End"));
#endif
}

void CTeachTab2GBanding::OnBnClickedBtnRoiBanding()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab2GBanding::OnBnClickedBtnRoiBanding"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTeachBanding)
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

		m_btnTeachBanding.EnableWindow(TRUE);
		AfxMessageBox(_T("검사할 Banding 영역을 지정하세요."));
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this, m_Banding.rcInspArea);
	}
	else
	{
		Cleanup();
	}
}

void APIENTRY CTeachTab2GBanding::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab2GBanding* pThis = (CTeachTab2GBanding*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab2GBanding::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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

	if (m_bIsTeachBanding)
	{
		m_Banding.rcInspArea = rcTrackRegion;

		//UpdateUI();
		clrBox.CreateObject(PDC_ORANGE, m_Banding.rcInspArea);
		pGO->AddDrawBox(clrBox);
		bRet = TRUE;
	}

	if (bRet)
	{
		Cleanup();
		UpdateData(FALSE);
	}
}

void CTeachTab2GBanding::OnLButtonDblClk(UINT nFlags, CPoint point)
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
				strParams.Format(_T("\"file:///%s#page=40\""), (LPCTSTR)strPdfPath);

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
