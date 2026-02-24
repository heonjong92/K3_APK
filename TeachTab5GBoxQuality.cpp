// TeachTab5GBoxQuality.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab5GBoxQuality.h"
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

// CTeachTab5GBoxQuality 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab5GBoxQuality, CDialog)

CTeachTab5GBoxQuality::CTeachTab5GBoxQuality(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab5GBoxQuality::IDD, pParent)
	, m_bIsTeachBoxQuality(FALSE)
	, m_nModelNum(0)
{
	m_pMainView = NULL;
	m_BoxInfo.Clear();

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab5GBoxQuality::~CTeachTab5GBoxQuality()
{
}

void CTeachTab5GBoxQuality::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,		IDC_LABEL_TITLE_5G,						m_wndLabelTitle5G);
	DDX_Control(pDX,		IDC_LABEL_RECIPE_TITLE,					m_wndLabelRecipeTitle);

	DDX_Control(pDX,		IDC_COMBO_RECIPE_BOX_QUALITY,			m_wndSelectRecipe);
	DDX_CBIndex(pDX,		IDC_COMBO_RECIPE_BOX_QUALITY,			m_nSelectRecipeIndex);

	DDX_Radio(pDX,			IDC_RADIO_BOX_BYPASS,					m_BoxInfo.nUseBypass_Box);

	DDX_Text(pDX,			IDC_EDIT_BOX_QUALITY_DESCRIPTION,		m_BoxInfo.strDescription);
	DDX_Text(pDX,			IDC_EDIT_BOXQUALITY_LED_LIGHT_VALUE,	m_BoxInfo.nValueCh1);
	DDX_Text(pDX,			IDC_EDIT_BOXQUALITY_OK_SCORE,			m_BoxInfo.fOkScore);

	DDX_Text(pDX,			IDC_EDIT_BOXQUALITY_SHIFT_X,			m_BoxInfo.nShiftX);
	DDX_Text(pDX,			IDC_EDIT_BOXQUALITY_SHIFT_Y,			m_BoxInfo.nShiftY);

	DDX_Control(pDX,		IDC_BTN_TEST_BOX_QUALITY,				m_btnTest_BoxQuality);
	DDX_Control(pDX,		IDC_BTN_BOX_QUALITY_RECIPE_NO,			m_btnSaveRecipeNo);
	DDX_Control(pDX,		IDC_BTN_SAVE_BOX_QUALITY,				m_btnSave);
	
	DDX_Check(pDX,			IDC_BTN_BOX_TEACH_MODEL, 				m_bIsTeachBoxQuality);
	DDX_Control(pDX,		IDC_PICTURE_PREVIEW_TEACH_BOX_QUALITY,	m_ctrlPicPreviewTeachBoxQuality);
	DDX_Control(pDX,		IDC_BTN_BOX_TEACH_MODEL, 				m_btnBoxTeachModel);

	DDV_MinMaxFloat(pDX,	m_BoxInfo.fOkScore, 					0.0f, 100.0f);
	DDX_Control(pDX,		IDC_COMBO_BOX_QUALITY_MODEL_NUM, 		m_Combo_SelectModelNum);
	DDX_CBIndex(pDX,		IDC_COMBO_BOX_QUALITY_MODEL_NUM, 		m_nModelNum);
}

BEGIN_MESSAGE_MAP(CTeachTab5GBoxQuality, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_RADIO_BOX_BYPASS,					&CTeachTab5GBoxQuality::OnBnClickedRadioBoxBypass)
	ON_BN_CLICKED(IDC_RADIO_BOX_BYPASS2,				&CTeachTab5GBoxQuality::OnBnClickedRadioBoxBypass)
	ON_BN_CLICKED(IDC_RADIO_BOX_BYPASS3,				&CTeachTab5GBoxQuality::OnBnClickedRadioBoxBypass)

	ON_BN_CLICKED(IDC_BTN_TEST_BOX_QUALITY,				&CTeachTab5GBoxQuality::OnBnClickedBtnTestBoxQuality)
	ON_BN_CLICKED(IDC_BTN_SAVE_BOX_QUALITY,				&CTeachTab5GBoxQuality::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_BOX_QUALITY_RECIPE_NO,		&CTeachTab5GBoxQuality::OnBnClickedBtnBoxQualityRecipeNo)
	ON_BN_CLICKED(IDC_BTN_BOX_TEACH_MODEL,				&CTeachTab5GBoxQuality::OnBnClickedBtnBoxTeachModel)

	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_BOX_QUALITY,		&CTeachTab5GBoxQuality::OnCbnSelchangeComboRecipe)

	ON_BN_CLICKED(IDC_RADIO_TEACH_CAUTION_BOX,			&CTeachTab5GBoxQuality::OnBnClickedTeachingBoxQuality)
	ON_BN_CLICKED(IDC_RADIO_TEACH_MOISTURE_BOX,			&CTeachTab5GBoxQuality::OnBnClickedTeachingBoxQuality)
	ON_BN_CLICKED(IDC_RADIO_TEACH_FRAGILE_BOX,			&CTeachTab5GBoxQuality::OnBnClickedTeachingBoxQuality)
	ON_BN_CLICKED(IDC_RADIO_TEACH_UMBRELLA_BOX,			&CTeachTab5GBoxQuality::OnBnClickedTeachingBoxQuality)
	ON_BN_CLICKED(IDC_RADIO_TEACH_RECYCLABLE_BOX,		&CTeachTab5GBoxQuality::OnBnClickedTeachingBoxQuality)
	ON_CBN_SELCHANGE(IDC_COMBO_BOX_QUALITY_MODEL_NUM,	&CTeachTab5GBoxQuality::OnCbnSelchangeComboBoxQualityModelNum)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CTeachTab5GBoxQuality 메시지 처리기입니다.
BOOL CTeachTab5GBoxQuality::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle5G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle5G.SetFontHeight( 12 );

	m_ctrlPicPreviewTeachBoxQuality.GetClientRect(m_rcPicCtrSizeTeachBoxQuality);

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_BoxQuality.SetColor( DEF_BTN_COLOR_MEASURE );

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

	m_ImageViewForBoxQualityTeaching.Create(&m_ImageViewManagerForBoxQualityTeaching, this);
	m_ImageViewForBoxQualityTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForBoxQualityTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForBoxQualityTeaching.SetAnimateWindow(FALSE);

	m_ImageViewForBoxQualityTeaching.SetImageObject(&m_ImageObjectForBoxQualityTeaching, FALSE);
	m_ImageViewForBoxQualityTeaching.SetRegisterCallBack(0, &regCB);

	m_ImageViewForBoxQualityTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForBoxQualityTeaching.SetBodyColor(dwBodyColor);

	m_ImageViewForBoxQualityTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForBoxQualityTeaching.ShowScrollBar(FALSE);
	m_ImageViewForBoxQualityTeaching.ShowScaleBar(FALSE);
	m_ImageViewForBoxQualityTeaching.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_BOX_QUALITY)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(0);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForBoxQualityTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab5GBoxQuality::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_5G				)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_BOX_QUALITY_RECIPE_NO 	)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_RADIO_BOX_BYPASS				)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_BOX_BYPASS2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_BOX_BYPASS3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_STATIC_DESCRIPTION			)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_BTN_TEST_BOX_QUALITY			)->SetWindowText(CLanguageInfo::Instance()->ReadString(19));
	GetDlgItem(IDC_STATIC_BOX_OK_SCORE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_BTN_BOX_TEACH_MODEL			)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_BOX_SHIFT				)->SetWindowText(CLanguageInfo::Instance()->ReadString(22));
	
	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle5G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab5GBoxQuality::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BOXQUALITY_OK_SCORE), _T("등록 되어있는 모델 패턴과 실제 촬영 된 이미지의 패턴을 비교하여, 해당 값 이하이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BOXQUALITY_SHIFT_X), _T("등록 되어있는 모델의 중심과 검사 된 모델의 중심을 비교하여, 해당 값 이상이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_BOXQUALITY_SHIFT_Y), _T("등록 되어있는 모델의 중심과 검사 된 모델의 중심을 비교하여, 해당 값 이상이면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_BOX_QUALITY_MODEL_NUM), _T("등록할 모델을 선택할 수 있는 콤보 박스입니다. 모델 등록 순서는 검사와 무관하며 최대 10개의 모델을 등록할 수 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_BOX_TEACH_MODEL), _T("선택된 Model 번호에 이미지 패턴을 등록합니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab5GBoxQuality::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab5GBoxQuality::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stBoxInfo& BoxInfo = CModelInfo::Instance()->GetBoxInfo();
	BoxInfo = m_BoxInfo;

#ifdef RELEASE_5G
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Save(CModelInfo::Instance()->GetModelNameBoxQuality(), TEACH_TAB_IDX_BOX);
	
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_BOX );
#endif
	
	Refresh();

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab5GBoxQuality::CheckData()
{
#ifdef RELEASE_5G
	CString strLog;
	CModelInfo::stBoxInfo& BoxInfo = CModelInfo::Instance()->GetBoxInfo();

	// ----- BoxQuality Teaching -----
	strLog.Format(_T("[Bypass][%s→%s]"), strBypassName[BoxInfo.nUseBypass_Box], strBypassName[m_BoxInfo.nUseBypass_Box] );
	if( BoxInfo.nUseBypass_Box != m_BoxInfo.nUseBypass_Box ) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeBox, strLog );
	strLog.Format(_T("[Description][%s→%s]"), BoxInfo.strDescription, m_BoxInfo.strDescription);
	if (BoxInfo.strDescription != m_BoxInfo.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBox, strLog);
	strLog.Format(_T("[LightValueCh1][%d→%d]"), BoxInfo.nValueCh1, m_BoxInfo.nValueCh1);
	if (BoxInfo.nValueCh1 != m_BoxInfo.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBox, strLog);
	strLog.Format(_T("[OkScore][%.1f→%.1f]"), BoxInfo.fOkScore, m_BoxInfo.fOkScore);
	if (BoxInfo.fOkScore != m_BoxInfo.fOkScore) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBox, strLog);

	for (int i = 0; i < nMATCH_MAX; i++)
	{
		strLog.Format(_T("[Match[%d] Center X][%d→%d]"), i, BoxInfo.ptMatchCenter[i].x, m_BoxInfo.ptMatchCenter[i].x);
		if (BoxInfo.ptMatchCenter[i].x != m_BoxInfo.ptMatchCenter[i].x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBox, strLog);
		strLog.Format(_T("[Match[%d] Center Y][%d→%d]"), i, BoxInfo.ptMatchCenter[i].y, m_BoxInfo.ptMatchCenter[i].y);
		if (BoxInfo.ptMatchCenter[i].y != m_BoxInfo.ptMatchCenter[i].y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeBox, strLog);
	}
#endif
}

void CTeachTab5GBoxQuality::Refresh()
{
	UpdateRecipeList();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load(CModelInfo::Instance()->GetModelNameBoxQuality(), BOXQUALITY_KIND);

	CModelInfo::stBoxInfo& BoxInfo = CModelInfo::Instance()->GetBoxInfo();
	m_BoxInfo = BoxInfo;

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTab5GBoxQuality::UpdateUI()
{
	UpdateCount_Box();
	UpdateTeachingImage();
}

void CTeachTab5GBoxQuality::UpdateTeachingImage()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return;

#ifdef RELEASE_5G
	USES_CONVERSION;
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetMatchData(IDX_AREA1, m_nModelNum);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");

	if (MatchBuff->GetPatternLearnt())
	{
		int nWidth = MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForBoxQualityTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strLastRecipe;
		strLastRecipe = CModelInfo::Instance()->GetLastModelNameBoxQuality();
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("5G_Box\\") + strLastRecipe + _T("\\");
		strInspName = _T("Box");

		strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, m_nModelNum);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForBoxQualityTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForBoxQualityTeaching.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImageObjectForBoxQualityTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForBoxQualityTeaching.ImageUpdate();

	m_pMainView->UpdateImageView();
#endif
}

void CTeachTab5GBoxQuality::UpdateCount_Box()
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

void CTeachTab5GBoxQuality::Default()
{
	m_BoxInfo.Clear();

	UpdateData(FALSE);
}

void CTeachTab5GBoxQuality::Cleanup()
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

	m_bIsTeachBoxQuality = FALSE;
}

HBRUSH CTeachTab5GBoxQuality::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_BOX_OK_SCORE:
	case IDC_STATIC_BOX_SHIFT:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab5GBoxQuality::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab5GBoxQuality::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab5GBoxQuality::PreTranslateMessage(MSG* pMsg)
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

void APIENTRY CTeachTab5GBoxQuality::_OnConfirmTracker(CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData)
{
	CTeachTab5GBoxQuality* pThis = (CTeachTab5GBoxQuality*)lpUsrData;
	pThis->OnConfirmTracker(rcTrackRegion, nIndexData);
}

void CTeachTab5GBoxQuality::OnConfirmTracker(CRect& rcTrackRegion, UINT nViewIndex)
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
			//rcTrackRegion.SetRect(rcTrack.right, rcTrack.bottom, rcTrack.left, rcTrack.bottom);
			rcTrackRegion.SetRect(rcTrack.right, rcTrack.top, rcTrack.left, rcTrack.bottom);
	}
	else
		if (rcTrack.top > rcTrack.bottom)
			rcTrackRegion.SetRect(rcTrack.left, rcTrack.bottom, rcTrack.right, rcTrack.top);

	pGO->Reset();

	BOOL bRet = FALSE;

	if (m_bIsTeachBoxQuality)
	{
		bRet = pInspectionVision->SetLearnModel_ForBox(pGO, pImgObj, rcTrackRegion, nViewIndex, m_nModelNum);
	}

	if (bRet)
	{
		m_BoxInfo.ptMatchCenter[m_nModelNum] = rcTrackRegion.CenterPoint();
		
		Cleanup();
		UpdateUI();
		UpdateData(FALSE);
	}
#endif
}

void CTeachTab5GBoxQuality::OnBnClickedRadioBoxBypass()
{
	UpdateData(TRUE);
}


void CTeachTab5GBoxQuality::OnBnClickedBtnTestBoxQuality()
{
#ifdef RELEASE_5G
	WRITE_LOG( WL_BTN, _T("CTeachTab5GBoxQuality::OnBnClickedBtnTestBoxQuality") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA1 );
	CVisionSystem::Instance()->StartInspection( InspectTypeBox, IDX_AREA1, 0, TRUE);
#endif
}

void CTeachTab5GBoxQuality::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_5G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(BOXQUALITY_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_BOX);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameBoxQuality();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameBoxQuality();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameBoxQuality();
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

void CTeachTab5GBoxQuality::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("5G_Box\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameBoxQuality();
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

void CTeachTab5GBoxQuality::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab5GBoxQuality::OnBnClickedBtnSave :: Start"));

	if (IDYES != AfxMessageBox(_T("Do you want Save?"), MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameBoxQuality();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, BOXQUALITY_KIND) )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load(strSelectModelName, BOXQUALITY_KIND);
		}

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab5GBoxQuality::OnBnClickedBtnSave :: End"));
}

void CTeachTab5GBoxQuality::DisableWnd(BOOL bEnable)
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

void CTeachTab5GBoxQuality::OnBnClickedBtnBoxQualityRecipeNo()
{
#ifdef RELEASE_5G
	WRITE_LOG(WL_MSG, _T("CTeachTab5GBoxQuality::OnBnClickedBtnBoxQualityRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab5GBoxQuality::OnBnClickedBtnBoxQualityRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( BOXQUALITY_KIND, TEACH_TAB_IDX_BOX );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 5G BOX QUALITY") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab5GBoxQuality::OnBnClickedBtnBoxQualityRecipeNo :: End"));
#endif
}

void CTeachTab5GBoxQuality::OnBnClickedTeachingBoxQuality()
{
	UpdateData(TRUE);

	UpdateUI();
}

void CTeachTab5GBoxQuality::OnBnClickedBtnBoxTeachModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab3GDesiccantMaterial::OnBnClickedBtnSubmaterialTechModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

	if (m_bIsTeachBoxQuality)
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
		
		m_btnBoxTeachModel.EnableWindow(TRUE);
		AfxMessageBox(_T("등록할 모델의 영역을 지정하세요."));
		m_pMainView->SetTrackerMode(TRUE, IDX_AREA1, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTab5GBoxQuality::OnCbnSelchangeComboBoxQualityModelNum()
{
	m_nModelNum = m_Combo_SelectModelNum.GetCurSel();

	UpdateTeachingImage();

	UpdateData(FALSE);
}

void CTeachTab5GBoxQuality::OnLButtonDblClk(UINT nFlags, CPoint point)
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
				strParams.Format(_T("\"file:///%s#page=58\""), (LPCTSTR)strPdfPath);

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
