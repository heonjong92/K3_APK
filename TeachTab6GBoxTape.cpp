// TeachTab6GBoxTape.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTab6GBoxTape.h"
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

// CTeachTab6GBoxTape 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTab6GBoxTape, CDialog)

CTeachTab6GBoxTape::CTeachTab6GBoxTape(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTab6GBoxTape::IDD, pParent)
	, m_nSelectRecipeIndex(-1)
{
	m_pMainView = NULL;
	m_Tape.Clear();

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTab6GBoxTape::~CTeachTab6GBoxTape()
{
}

void CTeachTab6GBoxTape::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_6G, 			m_wndLabelTitle6G);
	DDX_Control(pDX,	IDC_LABEL_RECIPE_TITLE,			m_wndLabelRecipeTitle);

	DDX_Control(pDX,	IDC_COMBO_RECIPE_TAPE,			m_wndSelectRecipe);
	DDX_CBIndex(pDX,	IDC_COMBO_RECIPE_TAPE,			m_nSelectRecipeIndex);

	DDX_Radio(pDX,		IDC_RADIO_TAPE_BYPASS, 			m_Tape.nUseBypass_Tape );
	DDX_Text(pDX,		IDC_EDIT_TAPE_DESCRIPTION,		m_Tape.strDescription);
	DDX_Text(pDX,		IDC_EDIT_TAPE_LED_LIGHT_VALUE,	m_Tape.nValueCh1);
	DDX_Text(pDX,		IDC_EDIT_TAPE_RED_THRESHOLD,	m_Tape.nRedThreshold);
	DDX_Text(pDX,		IDC_EDIT_TAPE_GREEN_THRESHOLD,	m_Tape.nGreenThreshold);
	DDX_Text(pDX,		IDC_EDIT_TAPE_BLUE_THRESHOLD,	m_Tape.nBlueThreshold);
	DDX_Text(pDX,		IDC_EDIT_TAPE_WIDTH,			m_Tape.nTapeWidth);
	DDX_Text(pDX,		IDC_EDIT_TAPE_HEIGHT,			m_Tape.nTapeHeight);

	DDX_Control(pDX,	IDC_BTN_TEST_BOX_TAPE, 			m_btnTest_Tape );
	DDX_Control(pDX,	IDC_BTN_TAPE_RECIPE_NO,			m_btnSaveRecipeNo);
	DDX_Control(pDX,	IDC_BTN_SAVE_TAPE,				m_btnSave);

	DDV_MinMaxInt(pDX,	m_Tape.nRedThreshold,			GV8_MIN,	GV8_MAX);
	DDV_MinMaxInt(pDX,	m_Tape.nGreenThreshold,			GV8_MIN,	GV8_MAX);
	DDV_MinMaxInt(pDX,	m_Tape.nBlueThreshold,			GV8_MIN,	GV8_MAX);
}


BEGIN_MESSAGE_MAP(CTeachTab6GBoxTape, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_TAPE_BYPASS,	&CTeachTab6GBoxTape::OnBnClickedRadioTapeBypass)
	ON_BN_CLICKED(IDC_RADIO_TAPE_BYPASS2,	&CTeachTab6GBoxTape::OnBnClickedRadioTapeBypass)
	ON_BN_CLICKED(IDC_RADIO_TAPE_BYPASS3,	&CTeachTab6GBoxTape::OnBnClickedRadioTapeBypass)
	ON_BN_CLICKED(IDC_BTN_TEST_BOX_TAPE,	&CTeachTab6GBoxTape::OnBnClickedBtnTestBoxTape)
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_TAPE, &CTeachTab6GBoxTape::OnCbnSelchangeComboRecipe)
	ON_BN_CLICKED(IDC_BTN_SAVE_TAPE,		&CTeachTab6GBoxTape::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_TAPE_RECIPE_NO,	&CTeachTab6GBoxTape::OnBnClickedBtnTapeRecipeNo)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTab6GBoxTape 메시지 처리기입니다.
BOOL CTeachTab6GBoxTape::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle6G.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitle6G.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_Tape.SetColor( DEF_BTN_COLOR_MEASURE );

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTab6GBoxTape::UpdateLanguage()
{
	GetDlgItem(IDC_LABEL_TITLE_6G				)->SetWindowText(CLanguageInfo::Instance()->ReadString(31));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(32));
	GetDlgItem(IDC_BTN_TAPE_RECIPE_NO			)->SetWindowText(CLanguageInfo::Instance()->ReadString(33));
	GetDlgItem(IDC_RADIO_TAPE_BYPASS			)->SetWindowText(CLanguageInfo::Instance()->ReadString(34));
	GetDlgItem(IDC_RADIO_TAPE_BYPASS2			)->SetWindowText(CLanguageInfo::Instance()->ReadString(35));
	GetDlgItem(IDC_RADIO_TAPE_BYPASS3			)->SetWindowText(CLanguageInfo::Instance()->ReadString(36));
	GetDlgItem(IDC_STATIC_DESCRIPTION			)->SetWindowText(CLanguageInfo::Instance()->ReadString(37));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(38));
	GetDlgItem(IDC_BTN_TEST_BOX_TAPE			)->SetWindowText(CLanguageInfo::Instance()->ReadString(39));
	GetDlgItem(IDC_STATIC_TAPE_RGB_THRESHOLD	)->SetWindowText(CLanguageInfo::Instance()->ReadString(40));
	GetDlgItem(IDC_STATIC_TAPE_WIDTH_HEIGHT		)->SetWindowText(CLanguageInfo::Instance()->ReadString(41));

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitle6G.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTab6GBoxTape::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TAPE_RED_THRESHOLD), _T("Tape 부분의 Red 픽셀 임계값을 입력합니다. 픽셀의 R/G/B값 모두가 임계값 이상인 Blob을 Tape로 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TAPE_GREEN_THRESHOLD), _T("Tape 부분의 Green 픽셀 임계값을 입력합니다. 픽셀의 R/G/B값 모두가 임계값 이상인 Blob을 Tape로 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TAPE_BLUE_THRESHOLD), _T("Tape 부분의 Bule 픽셀 임계값을 입력합니다. 픽셀의 R/G/B값 모두가 임계값 이상인 Blob을 Tape로 인식합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TAPE_WIDTH), _T("Tape의 폭의 값을 입력합니다. 값이 해당 값보다 작으면 NG가 발생합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TAPE_HEIGHT), _T("Tape의 높이 값을 입력합니다. 값이 해당 값보다 작으면 NG가 발생합니다."));
	m_toolTip.Activate(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTab6GBoxTape::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CTeachTab6GBoxTape::Save()
{
	UpdateData(TRUE);

	CheckData();

	CModelInfo::stTapeInfo& TapeInfo = CModelInfo::Instance()->GetTapeInfo();
	TapeInfo = m_Tape;

#ifdef RELEASE_6G
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_TAPE );
#endif
	
	UpdateData(FALSE);

	return TRUE;
}

void CTeachTab6GBoxTape::CheckData()
{
#ifdef RELEASE_6G
	CString strLog;
	CModelInfo::stTapeInfo& TapeInfo = CModelInfo::Instance()->GetTapeInfo();

	// ----- Box Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]" ), strBypassName[TapeInfo.nUseBypass_Tape], strBypassName[m_Tape.nUseBypass_Tape] );
	if( TapeInfo.nUseBypass_Tape != m_Tape.nUseBypass_Tape) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeTape, strLog );
	strLog.Format( _T("[Description][%s→%s]" ), TapeInfo.strDescription, m_Tape.strDescription);
	if ( TapeInfo.strDescription != m_Tape.strDescription) CVisionSystem::Instance()->WriteLogforTeaching( InspectTypeTape, strLog );

	strLog.Format(_T("[LightValueCh1][%d→%d]"), TapeInfo.nValueCh1, m_Tape.nValueCh1);
	if (TapeInfo.nValueCh1 != m_Tape.nValueCh1) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTape, strLog);

	strLog.Format(_T("[RedThreshold][%d→%d]"), TapeInfo.nRedThreshold, m_Tape.nRedThreshold);
	if (TapeInfo.nRedThreshold != m_Tape.nRedThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTape, strLog);

	strLog.Format(_T("[GreenThreshold][%d→%d]"), TapeInfo.nGreenThreshold, m_Tape.nGreenThreshold);
	if (TapeInfo.nGreenThreshold != m_Tape.nGreenThreshold)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTape, strLog);

	strLog.Format(_T("[BlueThreshold][%d→%d]"), TapeInfo.nBlueThreshold, m_Tape.nBlueThreshold);
	if (TapeInfo.nBlueThreshold != m_Tape.nBlueThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTape, strLog);

	strLog.Format(_T("[TapeWidth][%d→%d]"), TapeInfo.nTapeWidth, m_Tape.nTapeWidth);
	if (TapeInfo.nTapeWidth != m_Tape.nTapeWidth)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTape, strLog);

	strLog.Format(_T("[TapeHeight][%d→%d]"), TapeInfo.nTapeHeight, m_Tape.nTapeHeight);
	if (TapeInfo.nTapeHeight != m_Tape.nTapeHeight) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeTape, strLog);
#endif
}


void CTeachTab6GBoxTape::Refresh()
{
	UpdateRecipeList();
	CModelInfo::stTapeInfo& TapeInfo = CModelInfo::Instance()->GetTapeInfo();

	m_Tape = TapeInfo;

	UpdateData(FALSE);
}

void CTeachTab6GBoxTape::Default()
{
	m_Tape.Clear();

	UpdateData(FALSE);
}

void CTeachTab6GBoxTape::Cleanup()
{
	GetDlgItem(IDC_BTN_TEST_BOX_TAPE)->EnableWindow(TRUE);
}

HBRUSH CTeachTab6GBoxTape::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_TAPE_RGB_THRESHOLD:
	case IDC_STATIC_TAPE_AREA_CRITERIA:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTab6GBoxTape::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTab6GBoxTape::OnShowWindow(BOOL bShow, UINT nStatus)
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

BOOL CTeachTab6GBoxTape::PreTranslateMessage(MSG* pMsg)
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


void CTeachTab6GBoxTape::OnBnClickedRadioTapeBypass()
{
	UpdateData(TRUE);
}


void CTeachTab6GBoxTape::OnBnClickedBtnTestBoxTape()
{
#ifdef RELEASE_6G
	WRITE_LOG( WL_BTN, _T("CTeachTab6GBoxTape::OnBnClickedBtnTestBoxTape") );

	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA2 );
	CVisionSystem::Instance()->StartInspection( InspectTypeTape, IDX_AREA2, 0, TRUE);
#endif
}

void CTeachTab6GBoxTape::OnCbnSelchangeComboRecipe()
{
#ifdef RELEASE_6G
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(BOXTAPE_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_TAPE);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameTape();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameTape();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameTape();
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

void CTeachTab6GBoxTape::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("6G_Tape\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameTape();
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


void CTeachTab6GBoxTape::OnBnClickedBtnSave()
{
	WRITE_LOG(WL_BTN, _T("CTeachTab6GBoxTape::OnBnClickedBtnSave :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameTape();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		CModelInfo::Instance()->Load(strSelectModelName, BOXTAPE_KIND);

		Refresh();
		DisableWnd(TRUE);

		return;
	}

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));

	Save();

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG(WL_MSG, _T("CTeachTab6GBoxTape::OnBnClickedBtnSave :: End"));
}

void CTeachTab6GBoxTape::DisableWnd(BOOL bEnable)
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


void CTeachTab6GBoxTape::OnBnClickedBtnTapeRecipeNo()
{
#ifdef RELEASE_6G
	WRITE_LOG(WL_MSG, _T("CTeachTab6GBoxTape::OnBnClickedBtnTapeRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTab6GBoxTape::OnBnClickedBtnTapeRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind( BOXTAPE_KIND, TEACH_TAB_IDX_TAPE );
	EditJobNumberDlg.SetTitleName( _T("Recipe No : 6G TAPE") );

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTab6GBoxTape::OnBnClickedBtnTapeRecipeNo :: End"));
#endif
}

void CTeachTab6GBoxTape::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_6G);

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
				strParams.Format(_T("\"file:///%s#page=62\""), (LPCTSTR)strPdfPath);

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
