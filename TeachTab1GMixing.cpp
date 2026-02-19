// TeachTab1GMixing.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab1GMixing.h"
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

int nManualMixingNum; // 수동 Mixing 검사용 파라미터

// CTeachTab1GMixing 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab1GMixing, CDialog)

CTeachTab1GMixing::CTeachTab1GMixing(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab1GMixing::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
	, m_bIsTeachMixing(FALSE)
{
	m_pMainView = NULL;
	m_Mixing.Clear();

	m_nCombo_Mixing_Select = 0;
	
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab1GMixing::~CTeachTab1GMixing()
{
}

void CTeachTab1GMixing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_1G_MIXING,			m_wndLabelTitle1GMixing);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,				m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_MIXING,			m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_MIXING,			m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_MIXING_BYPASS,			m_Mixing.nUseBypass_Mixing);
	DDX_Text(pDX,		IDC_EDIT_MIXING_DESCRIPTION,		m_Mixing.strDescription);
	DDX_Text(pDX,		IDC_EDIT_MIXING_PAGECOUNT,			m_Mixing.lPageCount);
	DDX_Text(pDX,		IDC_EDIT_MIXING_TRAY_PIXEL_VALUE,	m_Mixing.nTrayPixelValue);
	DDX_Text(pDX,		IDC_EDIT_MIXING_MATCHING_SCORE,		m_Mixing.fMatchingScore);
	DDX_Text(pDX,		IDC_EDIT_MIXING_ZONE_WIDTH,			m_Mixing.nMixingZoneWidth);
	DDX_Text(pDX,		IDC_EDIT_TRAY_COUNT_ZONE_WIDTH,		m_Mixing.nTrayCountZoneWidth);

	DDX_Control(pDX,	IDC_COMBO_MIXING_NUMBER,			m_Combo_Select_Mixing_Num);
	DDX_CBIndex(pDX,	IDC_COMBO_MIXING_NUMBER,			m_nCombo_Mixing_Select);

	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_TEACH_MIXING,	m_ctrlPicPreviewTeachMixing);

	DDX_Control(pDX,	IDC_BTN_MIXING_TECH_MODEL,			m_btnMixingTechModel);
	DDX_Check(pDX,		IDC_BTN_MIXING_TECH_MODEL,			m_bIsTeachMixing);

	DDX_Control(pDX,	IDC_BTN_TEST_MIXING,				m_btnTest_Mixing);
	DDX_Control(pDX,	IDC_BTN_MIXING_RECIPE_NO,			m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_MIXING,				m_btnSave);

	DDX_Text(pDX,		IDC_EDIT_MIXING_MANUAL_CNT,			nManualMixingNum);
	DDV_MinMaxInt(pDX,	m_Mixing.nTrayPixelValue,			GV8_MIN, GV8_MAX);
	DDX_Check(pDX,		IDC_CHECK_PLAIN_PATTERN,			m_Mixing.bIsPlainPattern);
}


BEGIN_MESSAGE_MAP(CTeachTab1GMixing, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_MIXING_BYPASS,		&CTeachTab1GMixing::OnBnClickedRadioMixingBypass)
	ON_BN_CLICKED(IDC_RADIO_MIXING_BYPASS2,		&CTeachTab1GMixing::OnBnClickedRadioMixingBypass)
	ON_BN_CLICKED(IDC_RADIO_MIXING_BYPASS3,		&CTeachTab1GMixing::OnBnClickedRadioMixingBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_MIXING,			&CTeachTab1GMixing::OnBnClickedBtnTestMixing)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_MIXING,	&CTeachTab1GMixing::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_MIXING,			&CTeachTab1GMixing::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_MIXING_RECIPE_NO,		&CTeachTab1GMixing::OnBnClickedBtnMixingRecipeNo)
	ON_CBN_SELCHANGE(IDC_COMBO_MIXING_NUMBER,	&CTeachTab1GMixing::OnCbnSelchangeComboMixingNumber)
	ON_BN_CLICKED(IDC_BTN_MIXING_TECH_MODEL,	&CTeachTab1GMixing::OnBnClickedBtnMixingTechModel)
	ON_BN_CLICKED(IDC_CHECK_PLAIN_PATTERN,		&CTeachTab1GMixing::OnBnClickedCheckPlainPattern)
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTab1GMixing 메시지 처리기입니다.
BOOL CTeachTab1GMixing::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1GMixing.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle1GMixing.SetFontHeight( 12 );

	m_ctrlPicPreviewTeachMixing.GetClientRect(m_rcPicCtrlSizeTeachMixing);

	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_Mixing.SetColor( DEF_BTN_COLOR_MEASURE );

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

	m_ImageViewForMixingTeaching.Create(&m_ImageViewManagerForMixingTeaching, this);
	m_ImageViewForMixingTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForMixingTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForMixingTeaching.SetAnimateWindow(FALSE);

	m_ImageViewForMixingTeaching.SetImageObject(&m_ImageObjectForMixingTeaching, FALSE);
	m_ImageViewForMixingTeaching.SetRegisterCallBack(0, &regCB);

	m_ImageViewForMixingTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForMixingTeaching.SetBodyColor(dwBodyColor);

	m_ImageViewForMixingTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForMixingTeaching.ShowScrollBar(FALSE);
	m_ImageViewForMixingTeaching.ShowScaleBar(FALSE);
	m_ImageViewForMixingTeaching.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_MIXING)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(0);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForMixingTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab1GMixing::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_1G_MIXING			)->SetWindowText(CLanguageInfo::Instance()->ReadString(70));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE				)->SetWindowText(CLanguageInfo::Instance()->ReadString(71));
	GetDlgItem(IDC_BTN_MIXING_RECIPE_NO				)->SetWindowText(CLanguageInfo::Instance()->ReadString(72));
	GetDlgItem(IDC_RADIO_MIXING_BYPASS				)->SetWindowText(CLanguageInfo::Instance()->ReadString(73));
	GetDlgItem(IDC_RADIO_MIXING_BYPASS2				)->SetWindowText(CLanguageInfo::Instance()->ReadString(74));
	GetDlgItem(IDC_RADIO_MIXING_BYPASS3				)->SetWindowText(CLanguageInfo::Instance()->ReadString(75));
	GetDlgItem(IDC_STATIC_DESCRIPTION				)->SetWindowText(CLanguageInfo::Instance()->ReadString(76));
	GetDlgItem(IDC_STATIC_PAGECOUNT					)->SetWindowText(CLanguageInfo::Instance()->ReadString(77));
	GetDlgItem(IDC_STATIC_MATCHING_SCORE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(78));
	GetDlgItem(IDC_BTN_MIXING_TECH_MODEL			)->SetWindowText(CLanguageInfo::Instance()->ReadString(79));
	GetDlgItem(IDC_BTN_TEST_MIXING					)->SetWindowText(CLanguageInfo::Instance()->ReadString(80));
	GetDlgItem(IDC_STATIC_MANUAL_CNT				)->SetWindowText(CLanguageInfo::Instance()->ReadString(81));
	GetDlgItem(IDC_STATIC_MIXING_TRAY_PIXEL_VALUE	)->SetWindowText(CLanguageInfo::Instance()->ReadString(82));
	GetDlgItem(IDC_STATIC_MIXING_ZONE_WIDTH			)->SetWindowText(CLanguageInfo::Instance()->ReadString(83));
	GetDlgItem(IDC_STATIC_TRAY_COUNT_ZONE_WIDTH		)->SetWindowText(CLanguageInfo::Instance()->ReadString(84));


	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle1GMixing.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab1GMixing::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MIXING_PAGECOUNT), _T("Line Scan Camera로 읽는 범위이며, 이미지의 폭(Width)이 됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MIXING_TRAY_PIXEL_VALUE), _T("Tray의 픽셀 값을 입력합니다.입력된 Gray Value이상인 Blob을 Tray로 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MIXING_ZONE_WIDTH), _T("Tray Blob내에서 Mixing검사를 수행할 영역의 폭을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TRAY_COUNT_ZONE_WIDTH), _T("Tray Blob내에서 Tray 개수 검사를 수행할 영역의 폭을 입력합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MIXING_MANUAL_CNT), _T("수동 검사 시에 이미지에 보이는 Tray의 개수를 입력합니다.AutoRun에서는 사용되지 않습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_MIXING_MATCHING_SCORE), _T("기 등록된 패턴과 매칭 된 패턴의 점수 유사도를 입력합니다.입력한 매칭 점수보다 점수가 낮은 패턴이 발견되면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_MIXING_NUMBER), _T("등록할 모델을 선택할 수 있는 콤보 박스 입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_PLAIN_PATTERN), _T("옆면에 패턴이 없는 Tray의 경우 선택합니다.체크 되어 있다면 Tray 개수 검사만 수행합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_MIXING_TECH_MODEL), _T("콤보 박스에서 'OCR Zone Matching Model'을 선택한 뒤, 매칭할 패턴이 있는 영역을 지정 후 Save를 눌러 모델을 등록합니다. 콤보 박스에서 'Match Type 1'부터 선택한 뒤 OCR Zone에 있는 패턴을 지정 후 모델로 등록합니다. 모델 하나를 등록할 때마다 반드시 'Save'버튼을 누른 뒤 다음 모델을 등록해야 합니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab1GMixing::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab1GMixing::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stMixing& Mixing = CModelInfo::Instance()->GetMixing();
	Mixing = m_Mixing;

#ifdef RELEASE_1G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameMixing(), TEACH_TAB_IDX_MIXING);
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_MIXING );
#endif

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab1GMixing::CheckData()
{
#ifdef RELEASE_1G
	CString strLog;
	CModelInfo::stMixing& Mixing = CModelInfo::Instance()->GetMixing();

	// ----- Mixing Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[Mixing.nUseBypass_Mixing], strBypassName[m_Mixing.nUseBypass_Mixing] );
	if(Mixing.nUseBypass_Mixing != m_Mixing.nUseBypass_Mixing) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog );
	
	strLog.Format(_T("[Description][%s→%s]"), Mixing.strDescription, m_Mixing.strDescription);
	if (Mixing.strDescription != m_Mixing.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog);

	strLog.Format( _T("[PageCount][%lld→%lld]"), Mixing.lPageCount, m_Mixing.lPageCount );
	if (Mixing.lPageCount != m_Mixing.lPageCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog );

	strLog.Format(_T("[Matching Score][%.2f→%.2f]"), Mixing.fMatchingScore, m_Mixing.fMatchingScore);
	if (Mixing.fMatchingScore != m_Mixing.fMatchingScore) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog);

	strLog.Format(_T("[Tray Pixel Value][%d→%d]"), Mixing.nTrayPixelValue, m_Mixing.nTrayPixelValue);
	if (Mixing.nTrayPixelValue != m_Mixing.nTrayPixelValue) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog);	
	
	strLog.Format(_T("[Plain Pattern][%d→%d]"), Mixing.bIsPlainPattern, m_Mixing.bIsPlainPattern);
	if (Mixing.bIsPlainPattern != m_Mixing.bIsPlainPattern) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog);	
	
	for (int nMatchCnt = 0; nMatchCnt < nMATCH_MAX; nMatchCnt++)
	{
		strLog.Format(_T("[Mixing(%d) Offset X][%d→%d]"), nMatchCnt, Mixing.ptOffset[nMatchCnt].x, m_Mixing.ptOffset[nMatchCnt].x);
		if (Mixing.ptOffset[nMatchCnt].x != m_Mixing.ptOffset[nMatchCnt].x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog);

		strLog.Format(_T("[Mixing(%d) Offset Y][%d→%d]"), nMatchCnt, Mixing.ptOffset[nMatchCnt].y, m_Mixing.ptOffset[nMatchCnt].y);
		if (Mixing.ptOffset[nMatchCnt].y != m_Mixing.ptOffset[nMatchCnt].y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeMixing, strLog);
	}
#endif
}

void CTeachTab1GMixing::Refresh()
{
	UpdateRecipeList();
	CString strModelName = CModelInfo::Instance()->GetModelNameMixing();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(strModelName, MIXING_KIND);

	CModelInfo::stMixing& Mixing = CModelInfo::Instance()->GetMixing();
	m_Mixing = Mixing;

	UpdateUI();

	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
	if (strSelectModelName == strModelName)
		EnableModelTeaching();

	UpdateData(FALSE);
}

void CTeachTab1GMixing::Default()
{
	m_Mixing.Clear();

	UpdateData(FALSE);
}

void CTeachTab1GMixing::Cleanup()
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

	m_bIsTeachMixing = FALSE;
}

HBRUSH CTeachTab1GMixing::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_PAGECOUNT:
	case IDC_STATIC_MATCHING_SCORE:
	case IDC_STATIC_MANUAL_CNT:
	case IDC_STATIC_MIXING_TRAY_PIXEL_VALUE:
	case IDC_CHECK_PLAIN_PATTERN:
	case IDC_STATIC_MIXING_ZONE_WIDTH:
	case IDC_STATIC_TRAY_COUNT_ZONE_WIDTH:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab1GMixing::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab1GMixing::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab1GMixing::PreTranslateMessage(MSG* pMsg)
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

void CTeachTab1GMixing::OnBnClickedRadioMixingBypass()
{
	UpdateData(TRUE);
}

void CTeachTab1GMixing::OnBnClickedBtnTestMixing()
{
#ifdef RELEASE_1G
	WRITE_LOG( WL_BTN, _T("CTeachTab1GMixing::OnBnClickedBtnTestMixing") );
	UpdateData(TRUE);

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA5 );
	CVisionSystem::Instance()->StartInspection(InspectTypeMixing, IDX_AREA5, 0, TRUE);
#endif
}

void CTeachTab1GMixing::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_1G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(MIXING_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_MIXING);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameMixing();
			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameMixing();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameMixing();
		CString strSelectModelName;
		m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);
		if (strSelectModelName == strModelName)
		{
			m_btnSave.SetWindowText(_T("Save"));
			DisableWnd(TRUE);
			m_btnSave.EnableWindow(TRUE);
			Refresh();
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

void CTeachTab1GMixing::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_Mixing\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameMixing();
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


void CTeachTab1GMixing::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GMixing::OnBnClickedBtnSave :: Start"));

	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameMixing();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, MIXING_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, MIXING_KIND);
		}

		DisableWnd(TRUE);
		Refresh();
		
		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();
#ifdef RELEASE_1G
	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();
	FrameGrabber.SetPageCount(CameraTypeLine2);
#endif

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab1GMixing::OnBnClickedBtnSave :: End"));
	Refresh();
}

void CTeachTab1GMixing::DisableWnd(BOOL bEnable)
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

void CTeachTab1GMixing::OnBnClickedBtnMixingRecipeNo()
{
#ifdef RELEASE_1G
	WRITE_LOG(WL_MSG, _T("CTeachTab1GMixing::OnBnClickedBtnMixingRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab1GMixing::OnBnClickedBtnMixingRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( MIXING_KIND, TEACH_TAB_IDX_MIXING);
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 1G Mixing") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab1GMixing::OnBnClickedBtnMixingRecipeNo :: End"));
#endif
}

void CTeachTab1GMixing::OnCbnSelchangeComboMixingNumber()
{
	m_nCombo_Mixing_Select = m_Combo_Select_Mixing_Num.GetCurSel();

	UpdateTeachingImage_Mixing();

	UpdateData(FALSE);
}

void CTeachTab1GMixing::UpdateTeachingImage_Mixing()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return;

#ifdef RELEASE_1G
	USES_CONVERSION;
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA5, m_nCombo_Mixing_Select);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if (MatchBuff->GetPatternLearnt())
	{
		int nWidth = MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForMixingTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		strLastRecipe = CModelInfo::Instance()->GetLastModelNameMixing();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_Mixing\\") + strLastRecipe + _T("\\");
		strInspName = _T("Mixing");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, m_nCombo_Mixing_Select);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForMixingTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForMixingTeaching.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImageObjectForMixingTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForMixingTeaching.ImageUpdate();

	m_pMainView->UpdateImageView();

#endif
}

void CTeachTab1GMixing::UpdateUI()
{
	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
	{
		GetDlgItem(IDC_EDIT_MIXING_PAGECOUNT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_MIXING_PAGECOUNT)->EnableWindow(FALSE);
	}

	UpdateCount_Mixing();
	UpdateTeachingImage_Mixing();
}

void CTeachTab1GMixing::UpdateCount_Mixing(BOOL bComboReset/*= FALSE*/)
{
	m_Combo_Select_Mixing_Num.ResetContent();

	for (int i = 0; i < nMATCH_MAX; i++)
	{
		CString strBuff;
		if (i == 0)	strBuff = _T("OCR Zone Matching Model");
		else		strBuff.Format(_T("Match Type %d"), i);

		m_Combo_Select_Mixing_Num.AddString(strBuff);
	}

	if (bComboReset)
	{
		m_nCombo_Mixing_Select = 0;
		m_Combo_Select_Mixing_Num.SetCurSel(0);
	}
	else if (0 <= m_nCombo_Mixing_Select)
		m_Combo_Select_Mixing_Num.SetCurSel(m_nCombo_Mixing_Select);
}

void CTeachTab1GMixing::OnBnClickedBtnMixingTechModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab1GMixing::OnBnClickedBtnMixingTechModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);
#ifdef RELEASE_1G
	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA5);

	if (m_bIsTeachMixing)
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

		m_btnMixingTechModel.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA5, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
#endif
}

void APIENTRY CTeachTab1GMixing::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab1GMixing* pThis = (CTeachTab1GMixing*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab1GMixing::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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
		if (rcTrarack.top > rcTrarack.bottom)
			rcTrackRegion.SetRect(rcTrarack.left, rcTrarack.bottom, rcTrarack.right, rcTrarack.top);

	pGO->Reset();

	BOOL bRet = FALSE;

	if (m_bIsTeachMixing)
	{
		bRet = pInspectionVision->SetLearnModel(pGO, pImgObj, rcTrackRegion, InspectTypeMixing, nViewIndex, m_nCombo_Mixing_Select, FALSE, FALSE);
		m_Mixing.ptOffset[m_nCombo_Mixing_Select] = CPoint(rcTrackRegion.left, rcTrackRegion.top);
	}

	if (bRet)
	{
		Cleanup();
		//UpdateUI();
		EnableModelTeaching();
		UpdateData(FALSE);
	}
#endif
}

void CTeachTab1GMixing::OnBnClickedCheckPlainPattern()
{
	UpdateData(TRUE);
	EnableModelTeaching();
}

void CTeachTab1GMixing::EnableModelTeaching()
{
	if (m_Mixing.bIsPlainPattern)
	{
		GetDlgItem(IDC_COMBO_MIXING_NUMBER)->EnableWindow(FALSE);
		m_btnMixingTechModel.EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_MIXING_NUMBER)->EnableWindow(TRUE);
		m_btnMixingTechModel.EnableWindow(TRUE);
	}
}

void CTeachTab1GMixing::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_1G_MIXING);

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
				strParams.Format(_T("\"file:///%s#page=36\""), (LPCTSTR)strPdfPath);

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
