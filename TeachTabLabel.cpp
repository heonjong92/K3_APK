// TeachTabLabel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachTabLabel.h"
#include "afxdialogex.h"
#include "AddNewRecipeDlg.h"

#include "ModelInfo.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "TeachOCRFont.h"
#include "EditJobNumberDlg.h"
#include "LanguageInfo.h"
#include "StringMacro.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>

#include <XGraphic\xGraphicObject.h>

// CTeachTabLabel 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachTabLabel, CDialog)

CTeachTabLabel::CTeachTabLabel(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachTabLabel::IDD, pParent)
	, m_nInspCount(0)
	, m_nInspIndex(0)
{
	m_pMainView = NULL;
	m_Label.Clear();

	m_bIsTeachLabel = FALSE;
	m_bIsTeachLabel_Align = FALSE;
	m_nCombo_Masking_Select = 0;

	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	CString strFontFace = ncm.lfMessageFont.lfFaceName;
	if (strFontFace.IsEmpty())
		strFontFace = _T("Arial");

	m_fontRecipe.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, strFontFace);
}

CTeachTabLabel::~CTeachTabLabel()
{
}

void CTeachTabLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LABEL_TITLE_LABEL,					m_wndLabelTitleLabel);
	DDX_Control(pDX, IDC_LABEL_RECIPE_TITLE,				m_wndLabelRecipeTitle);

	DDX_Control(pDX, IDC_COMBO_RECIPE_LABEL,				m_wndSelectRecipe);
	DDX_CBIndex(pDX, IDC_COMBO_RECIPE_LABEL,				m_nSelectRecipeIndex);

	DDX_Text(pDX, IDC_EDIT_LABEL_DESCRIPTION,				m_Label.strDescription);
	DDX_Text(pDX, IDC_EDIT_LABEL_LED_LIGHT_VALUE,			m_Label.nValueCh1);
	
	DDX_Radio( pDX, IDC_RADIO_LABEL_BYPASS,					m_Label.nUseBypass_Label );

	DDX_Text	(pDX, IDC_EDIT_LABEL_ANGLE_SPEC,			m_Label.fLabelAngleSpec);
	DDV_MinMaxFloat	(pDX, m_Label.fLabelAngleSpec, 0.5f, 5.0f);
	DDX_Text	(pDX, IDC_EDIT_LABEL_EDGE_OFFSET,			m_Label.fLabelEdgeOffset);
	DDV_MinMaxFloat	(pDX, m_Label.fLabelEdgeOffset, 0.4f, 10.0f);
	DDX_Text	(pDX, IDC_EDIT_LABEL_EDGE_MARGIN,			m_Label.nLabelEdgeMargin);

	DDX_Control	(pDX, IDC_COMBO_LABEL_TOTAL_MASKING_COUNT,	m_Combo_Masking_Total_Cnt);
	DDX_CBIndex	(pDX, IDC_COMBO_LABEL_TOTAL_MASKING_COUNT,	m_Label.nLabelMaskingCount);
	DDX_Control	(pDX, IDC_COMBO_LABEL_MASKING_NUMBER,		m_Combo_Select_Masking_Num);
	DDX_CBIndex	(pDX, IDC_COMBO_LABEL_MASKING_NUMBER,		m_nCombo_Masking_Select);
	DDX_Control	(pDX, IDC_COMBO_LABEL_MASKING_SCORE,		m_Combo_Masking_Score);
	DDX_CBIndex	(pDX, IDC_COMBO_LABEL_MASKING_SCORE,		m_Label.nLabelMaskingScoreType[m_nCombo_Masking_Select]);

	DDX_Control (pDX, IDC_PICTURE_PREVIEW_TEACH_LABEL,				m_ctrlPicPreviewTeachLabel);
	DDX_Control	(pDX, IDC_PICTURE_PREVIEW_TEACH_LABEL_ALIGN,		m_ctrlPicPreviewTeachLabel_Align);
	
	DDX_Check(pDX, IDC_BTN_LABEL_TECH_MODEL,				m_bIsTeachLabel);
	DDX_Control	(pDX, IDC_BTN_LABEL_TECH_MODEL,				m_btnLabelTechModel);
	DDX_Check(pDX, IDC_BTN_LABEL_TECH_ALIGN_MODEL, m_bIsTeachLabel_Align);
	DDX_Control(pDX, IDC_BTN_LABEL_TECH_ALIGN_MODEL, m_btnLabelTechModel_Align);

	DDX_Control	(pDX, IDC_BTN_LABEL_TECH_OCRFONTTEACHING,	m_btnOCRFontTeaching);

	DDX_Control	(pDX, IDC_BTN_TEST_LABEL,					m_btnTest_Label);
	DDX_Control(pDX, IDC_BTN_LABEL_RECIPE_NO,				m_btnSaveRecipeNo);
	DDX_Control(pDX, IDC_BTN_SAVE_LABEL, 					m_btnSave);

	DDX_Text(pDX, IDC_EDIT_INSP_CNT, m_nInspCount);
	DDX_Text(pDX, IDC_EDIT_INSP_INDEX, m_nInspIndex);
	DDX_Control(pDX, IDC_SPIN_INSP_SEL, m_wndSpinInsp);
}


BEGIN_MESSAGE_MAP(CTeachTabLabel, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_LABEL_BYPASS,					&CTeachTabLabel::OnBnClickedRadioLabelBypass)
	ON_BN_CLICKED(IDC_RADIO_LABEL_BYPASS2,					&CTeachTabLabel::OnBnClickedRadioLabelBypass)
	ON_BN_CLICKED(IDC_RADIO_LABEL_BYPASS3,					&CTeachTabLabel::OnBnClickedRadioLabelBypass)

	ON_CBN_SELCHANGE(IDC_COMBO_LABEL_TOTAL_MASKING_COUNT,	&CTeachTabLabel::OnCbnSelchangeComboLabelTotalMaskingCount )
	ON_CBN_SELCHANGE(IDC_COMBO_LABEL_MASKING_NUMBER,		&CTeachTabLabel::OnCbnSelchangeComboLabelMaskingNumber )
	ON_CBN_SELCHANGE(IDC_COMBO_RECIPE_LABEL,				&CTeachTabLabel::OnCbnSelchangeComboRecipe)

	ON_BN_CLICKED(IDC_BTN_LABEL_TECH_MODEL,					&CTeachTabLabel::OnBnClickedBtnLabelTechModel)
	ON_BN_CLICKED(IDC_BTN_LABEL_TECH_ALIGN_MODEL,			&CTeachTabLabel::OnBnClickedBtnLabelTechAlignModel)
	ON_BN_CLICKED(IDC_BTN_LABEL_TECH_OCRFONTTEACHING,		&CTeachTabLabel::OnBnClickedBtnLabelTechOcrfontteaching)
	ON_BN_CLICKED(IDC_CHECK_LABEL_USEMANUALDATA,			&CTeachTabLabel::OnBnClickedCheckLabelUsemanualdata)
	ON_BN_CLICKED(IDC_BTN_TEST_LABEL,						&CTeachTabLabel::OnBnClickedBtnTestLabel)
	ON_BN_CLICKED(IDC_BTN_LABEL_RECIPE_NO,					&CTeachTabLabel::OnBnClickedBtnLabelRecipeNo)
	ON_BN_CLICKED(IDC_BTN_SAVE_LABEL, &CTeachTabLabel::OnBnClickedBtnSaveLabel)
	ON_BN_CLICKED(IDC_BTN_INSP_ADD, &CTeachTabLabel::OnBnClickedBtnInspAdd)
	ON_BN_CLICKED(IDC_BTN_SHOWAREA, &CTeachTabLabel::OnBnClickedBtnShowarea)
	ON_BN_CLICKED(IDC_BTN_INSP_SELDEL, &CTeachTabLabel::OnBnClickedBtnInspSeldel)
	ON_BN_CLICKED(IDC_BTN_INSP_ALLDEL, &CTeachTabLabel::OnBnClickedBtnInspAlldel)
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTeachTabLabel 메시지 처리기입니다.
BOOL CTeachTabLabel::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitleLabel.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitleLabel.SetFontHeight( 12 );

	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetBold(TRUE);
	m_wndLabelRecipeTitle.SetFontHeight(12);

	m_wndSelectRecipe.SetFont(&m_fontRecipe);

	m_btnTest_Label.SetColor( DEF_BTN_COLOR_MEASURE );
	//////////////////////////////////////////////////////////////////////////

	m_wndSpinInsp.Invalidate();

	//////////////////////////////////////////////////////////////////////////
	m_ctrlPicPreviewTeachLabel.GetClientRect(m_rcPicCtrlSizeTeachLabel);
	m_ctrlPicPreviewTeachLabel_Align.GetClientRect(m_rcPicCtrlSizeTeachLabel_Align);

	m_Combo_Masking_Total_Cnt.ResetContent();
	m_Combo_Masking_Total_Cnt.AddString( _T("No Marking") );
	for(int i = 0; i < EMATCH_TYPE_LABEL_MAX-1; i ++)
	{
		CString strBuff;
		strBuff.Format(_T("Marking %d EA"), i + 1);
		m_Combo_Masking_Total_Cnt.AddString( strBuff );
	}
	m_Combo_Masking_Total_Cnt.SetCurSel(0);

	m_Combo_Masking_Score.ResetContent();
	m_Combo_Masking_Score.AddString( _T("High Score(0.9)"	) );
	m_Combo_Masking_Score.AddString( _T("Middle Score(0.7)"	) );
	m_Combo_Masking_Score.AddString( _T("Low Score(0.5)"	) );
	m_Combo_Masking_Score.SetCurSel(0);

	//////////////////////////////////////////////////////////////////////////
	// Match Viewer Create
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor     = UIExt::CResourceManager::Instance()->GetViewerBodyColor();

	m_ImageViewForLabelTeaching.Create(&m_ImageViewManagerForLabelTeaching, this);
	m_ImageViewForLabelTeaching.ShowWindow(SW_SHOW);
	m_ImageViewForLabelTeaching.ShowTitleWindow(FALSE);
	m_ImageViewForLabelTeaching.SetAnimateWindow(FALSE);

	m_ImageViewForLabelTeaching.SetImageObject(&m_ImageObjectForLabelTeaching, FALSE);
	m_ImageViewForLabelTeaching.SetRegisterCallBack(0, &regCB);

	m_ImageViewForLabelTeaching.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForLabelTeaching.SetBodyColor(dwBodyColor);

	m_ImageViewForLabelTeaching.ShowStatusWindow(FALSE);
	m_ImageViewForLabelTeaching.ShowScrollBar(FALSE);
	m_ImageViewForLabelTeaching.ShowScaleBar(FALSE);
	m_ImageViewForLabelTeaching.EnableMouseControl(FALSE);

	m_ImageViewForLabelTeaching_Align.Create(&m_ImageViewManagerForLabelTeaching_Align, this);
	m_ImageViewForLabelTeaching_Align.ShowWindow(SW_SHOW);
	m_ImageViewForLabelTeaching_Align.ShowTitleWindow(FALSE);
	m_ImageViewForLabelTeaching_Align.SetAnimateWindow(FALSE);
							   
	m_ImageViewForLabelTeaching_Align.SetImageObject(&m_ImageObjectForLabelTeaching_Align, FALSE);
	m_ImageViewForLabelTeaching_Align.SetRegisterCallBack(0, &regCB);
							   
	m_ImageViewForLabelTeaching_Align.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForLabelTeaching_Align.SetBodyColor(dwBodyColor);
							   
	m_ImageViewForLabelTeaching_Align.ShowStatusWindow(FALSE);
	m_ImageViewForLabelTeaching_Align.ShowScrollBar(FALSE);
	m_ImageViewForLabelTeaching_Align.ShowScaleBar(FALSE);
	m_ImageViewForLabelTeaching_Align.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize, reViewSize_Align;
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_LABEL)->GetWindowRect(reViewSize);
	GetDlgItem(IDC_PICTURE_PREVIEW_TEACH_LABEL_ALIGN)->GetWindowRect(reViewSize_Align);
	
	ScreenToClient(reViewSize);
	ScreenToClient(reViewSize_Align);

	HDWP hDWP = BeginDeferWindowPos(2);
	hDWP = DeferWindowPos( hDWP, m_ImageViewForLabelTeaching.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0 );	
	hDWP = DeferWindowPos(hDWP, m_ImageViewForLabelTeaching_Align.GetSafeHwnd(), NULL, reViewSize_Align.left, reViewSize_Align.top, reViewSize_Align.Width(), reViewSize_Align.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight(14);
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
	UpdateToolTip();

	return TRUE;
}

void CTeachTabLabel::UpdateLanguage()
{
#ifdef RELEASE_4G
	GetDlgItem(IDC_LABEL_TITLE_LABEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(31));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE)->SetWindowText(CLanguageInfo::Instance()->ReadString(32));
	GetDlgItem(IDC_BTN_LABEL_RECIPE_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(33));
	GetDlgItem(IDC_RADIO_LABEL_BYPASS)->SetWindowText(CLanguageInfo::Instance()->ReadString(34));
	GetDlgItem(IDC_RADIO_LABEL_BYPASS2)->SetWindowText(CLanguageInfo::Instance()->ReadString(35));
	GetDlgItem(IDC_RADIO_LABEL_BYPASS3)->SetWindowText(CLanguageInfo::Instance()->ReadString(36));
	GetDlgItem(IDC_STATIC_DESCRIPTION)->SetWindowText(CLanguageInfo::Instance()->ReadString(37));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE)->SetWindowText(CLanguageInfo::Instance()->ReadString(38));
	GetDlgItem(IDC_STATIC_LABEL_ANGLE_SPEC)->SetWindowText(CLanguageInfo::Instance()->ReadString(39));
	GetDlgItem(IDC_STATIC_LABEL_EDGE_OFFSET)->SetWindowText(CLanguageInfo::Instance()->ReadString(40));
	GetDlgItem(IDC_STATIC_LABEL_MASKING_COUNT)->SetWindowText(CLanguageInfo::Instance()->ReadString(41));
	GetDlgItem(IDC_STATIC_LABEL_MASKING_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(42));
	GetDlgItem(IDC_STATIC_LABEL_MASKING_SCORE)->SetWindowText(CLanguageInfo::Instance()->ReadString(43));
	GetDlgItem(IDC_BTN_LABEL_TECH_MODEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(44));
	GetDlgItem(IDC_BTN_LABEL_TECH_OCRFONTTEACHING)->SetWindowText(CLanguageInfo::Instance()->ReadString(45));
	GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA)->SetWindowText(CLanguageInfo::Instance()->ReadString(46));
	GetDlgItem(IDC_BTN_TEST_LABEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(47));

	GetDlgItem(IDC_BTN_LABEL_TECH_ALIGN_MODEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(48));
	GetDlgItem(IDC_STATIC_INSP_AREA_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(49));
	GetDlgItem(IDC_STATIC_INSP_AREA_SELECT_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(50));
	GetDlgItem(IDC_BTN_INSP_ADD)->SetWindowText(CLanguageInfo::Instance()->ReadString(51));
	GetDlgItem(IDC_BTN_SHOWAREA)->SetWindowText(CLanguageInfo::Instance()->ReadString(52));
	GetDlgItem(IDC_BTN_INSP_SELDEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(53));
	GetDlgItem(IDC_BTN_INSP_ALLDEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(54));
	GetDlgItem(IDC_STATIC_LABEL_EDGE_OFFSET2)->SetWindowText(CLanguageInfo::Instance()->ReadString(55));

#elif RELEASE_6G
	GetDlgItem(IDC_LABEL_TITLE_LABEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(11));
	GetDlgItem(IDC_LABEL_RECIPE_TITLE)->SetWindowText(CLanguageInfo::Instance()->ReadString(12));
	GetDlgItem(IDC_BTN_LABEL_RECIPE_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(13));
	GetDlgItem(IDC_RADIO_LABEL_BYPASS)->SetWindowText(CLanguageInfo::Instance()->ReadString(14));
	GetDlgItem(IDC_RADIO_LABEL_BYPASS2)->SetWindowText(CLanguageInfo::Instance()->ReadString(15));
	GetDlgItem(IDC_RADIO_LABEL_BYPASS3)->SetWindowText(CLanguageInfo::Instance()->ReadString(16));
	GetDlgItem(IDC_STATIC_DESCRIPTION)->SetWindowText(CLanguageInfo::Instance()->ReadString(17));
	GetDlgItem(IDC_STATIC_LIGHT_VALUE)->SetWindowText(CLanguageInfo::Instance()->ReadString(18));
	GetDlgItem(IDC_STATIC_LABEL_ANGLE_SPEC)->SetWindowText(CLanguageInfo::Instance()->ReadString(19));
	GetDlgItem(IDC_STATIC_LABEL_EDGE_OFFSET)->SetWindowText(CLanguageInfo::Instance()->ReadString(20));
	GetDlgItem(IDC_STATIC_LABEL_MASKING_COUNT)->SetWindowText(CLanguageInfo::Instance()->ReadString(21));
	GetDlgItem(IDC_STATIC_LABEL_MASKING_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(22));
	GetDlgItem(IDC_STATIC_LABEL_MASKING_SCORE)->SetWindowText(CLanguageInfo::Instance()->ReadString(23));
	GetDlgItem(IDC_BTN_LABEL_TECH_MODEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(24));
	GetDlgItem(IDC_BTN_LABEL_TECH_OCRFONTTEACHING)->SetWindowText(CLanguageInfo::Instance()->ReadString(25));
	GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA)->SetWindowText(CLanguageInfo::Instance()->ReadString(26));
	GetDlgItem(IDC_BTN_TEST_LABEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(27));
	GetDlgItem(IDC_STATIC_LABEL_EDGE_OFFSET2)->SetWindowText(CLanguageInfo::Instance()->ReadString(28));

	GetDlgItem(IDC_BTN_LABEL_TECH_ALIGN_MODEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(4));
	GetDlgItem(IDC_STATIC_INSP_AREA_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(5));
	GetDlgItem(IDC_STATIC_INSP_AREA_SELECT_NO)->SetWindowText(CLanguageInfo::Instance()->ReadString(6));
	GetDlgItem(IDC_BTN_INSP_ADD)->SetWindowText(CLanguageInfo::Instance()->ReadString(7));
	GetDlgItem(IDC_BTN_SHOWAREA)->SetWindowText(CLanguageInfo::Instance()->ReadString(8));
	GetDlgItem(IDC_BTN_INSP_SELDEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(9));
	GetDlgItem(IDC_BTN_INSP_ALLDEL)->SetWindowText(CLanguageInfo::Instance()->ReadString(10));
#endif
	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	m_wndLabelTitleLabel.SetColor(dwLabelBodyColor, dwLabelTextColor);
	m_wndLabelRecipeTitle.SetColor(dwLabelBodyColor, dwLabelTextColor);
}

void CTeachTabLabel::UpdateToolTip()
{
	m_toolTip.Create(this, TTS_BALLOON);
	m_toolTip.SetMaxTipWidth(300);  //멀티라인 활성화

	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LABEL_EDGE_MARGIN), _T("Label 영역을 잡고 Warpage로 인해 Margin 값 넣는 파라미터 입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LABEL_EDGE_OFFSET), _T("Label 인쇄 상태가 끝으로 쏠렸는지 확인하는 파라미터 입니다.검사에서 Label이라고 찾은 영역 Edge에서 보정 값 만큼 영역을 획득 후 검은색이 있는지 확인합니다.검은색이 발견되면 NG로 판정합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LABEL_ANGLE_SPEC), _T("Label이 발행 됐을 때, 검사에 사용하기 위해 Label 이미지를 회전 시켜서 검사합니다.만약, 각도의 범위가 커서 OK가 됐다면 MBB에 부착 상태는 회전되어 있을 수도 있습니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_LABEL_TECH_ALIGN_MODEL), _T("Label의 OCR을 검사하기 위해 Fiducial Mark처럼 사용하는 모델을 등록합니다.해당 검사가 NG시, OCR 검사를 하지 않고 NG 판정합니다."));

	CString strMethod, strMethod2;
	strMethod = _T("OCR 검사를 할 영역을 설정합니다. \r\n");
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INSP_CNT), strMethod + _T(" - 영역 수량 : 설정한 영역이 몇 개인지 표기됩니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_INSP_ADD), strMethod + _T(" - 영역 추가 : 검사할 영역을 추가합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_SHOWAREA), strMethod + _T(" - 영역 보기 : 설정한 영역을 전부 보여줍니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INSP_INDEX), strMethod + _T(" - 선택 번호 : 영역 보기 클릭 시 해당 번호의 그래픽 색상은 변경합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_INSP_SELDEL), strMethod + _T(" - 선택 번호 삭제 : 잘못된 Teaching을 진행할 수 있으므로 해당 번호만 삭제하는 기능입니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_INSP_ALLDEL), strMethod + _T(" - 전체 삭제 : 등록된 영역을 전부 삭제 합니다."));
	
	strMethod = _T("OCR, Barcode 검사에 불필요한 그림을 지우는 기능입니다. \r\n");
	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_LABEL_TOTAL_MASKING_COUNT), strMethod + _T(" - 마킹 수량 : 최대 10개까지 설정 가능합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_LABEL_MASKING_NUMBER), strMethod + _T(" - 마킹 번호 : 수량에서 설정한 값 만큼 번호가 부여되며, 각 번호마다 모델 티칭을 해야 합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_COMBO_LABEL_MASKING_SCORE), strMethod + _T(" - 마킹 점수 : High Score(0.9), Middle Score(0.7), Low Score(0.5) 세 가지 중에서 선택합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_BTN_LABEL_TECH_MODEL), strMethod + _T(" - 라벨 마킹 모델 등록 : 마킹 검사 모델을 등록 합니다."));

	m_toolTip.AddTool(GetDlgItem(IDC_BTN_LABEL_TECH_OCRFONTTEACHING), _T("OCR NG 발생하면 해당 버튼을 눌러서 티칭 화면을 Open하여 Font 티칭을 진행합니다."));
	m_toolTip.AddTool(GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA), _T("상위에서 전달 받은 Data와 검사에서 획득한 OCR Data가 일치하는지 확인합니다. 수동운전에서만 사용하는 기능이며, 체크를 하지 않으면 OCR이 NG여도 OK 판정이 됩니다."));
	m_toolTip.Activate(TRUE);

//	CString strPath;
//	strPath = GetExecuteDirectory() + _T("\\Manual\\HA-1154 Manual (Label).JPG");
//	m_toolTipImg.Create(this);
//	m_toolTipImg.SetImage(strPath);
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CTeachTabLabel::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

void CTeachTabLabel::CheckData()
{
#if defined(RELEASE_4G) || defined(RELEASE_6G)
	CString strLog;
	CModelInfo::stLabelInfo& Label = CModelInfo::Instance()->GetLabelInfo();

	// ----- Label Teaching -----
	strLog.Format( _T("[Bypass][%s→%s]"), strBypassName[Label.nUseBypass_Label], strBypassName[m_Label.nUseBypass_Label] );
	if(Label.nUseBypass_Label != m_Label.nUseBypass_Label) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );

	strLog.Format(_T("[Description][%s→%s]"), Label.strDescription, Label.strDescription);
	if(Label.strDescription != Label.strDescription) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog);

	strLog.Format(_T("[LightValueCh1][%d→%d]"), Label.nValueCh1, Label.nValueCh1);
	if(Label.nValueCh1 != Label.nValueCh1)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog);

	strLog.Format(_T("[Label Edge Margin][%d→%d]"), Label.nLabelEdgeMargin, Label.nLabelEdgeMargin);
	if (Label.nLabelEdgeMargin != Label.nLabelEdgeMargin)	CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog);

	strLog.Format( _T("[Label Edge Offset][%.1f→%.1f]"), Label.fLabelEdgeOffset, m_Label.fLabelEdgeOffset );
	if(Label.fLabelEdgeOffset != m_Label.fLabelEdgeOffset) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );

	strLog.Format( _T("[Label Angle Spec][%.1f→%.1f]"), Label.fLabelAngleSpec, m_Label.fLabelAngleSpec );
	if(Label.fLabelAngleSpec != m_Label.fLabelAngleSpec) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );

	for(int nMatchCnt = 0; nMatchCnt < EMATCH_TYPE_LABEL_MAX; nMatchCnt++)
	{
		strLog.Format( _T("[Label Marking(%d) Score Type][%d→%d]"), nMatchCnt, Label.nLabelMaskingScoreType[nMatchCnt], m_Label.nLabelMaskingScoreType[nMatchCnt] );
		if(Label.nLabelMaskingScoreType[nMatchCnt] != m_Label.nLabelMaskingScoreType[nMatchCnt]) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );

		strLog.Format( _T("[Label Marking(%d) Offset X][%d→%d]"), nMatchCnt, Label.ptOffset_Lable[nMatchCnt].x, m_Label.ptOffset_Lable[nMatchCnt].x );
		if(Label.ptOffset_Lable[nMatchCnt].x != m_Label.ptOffset_Lable[nMatchCnt].x) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );

		strLog.Format( _T("[Label Marking(%d) Offset Y][%d→%d]"), nMatchCnt, Label.ptOffset_Lable[nMatchCnt].y, m_Label.ptOffset_Lable[nMatchCnt].y );
		if(Label.ptOffset_Lable[nMatchCnt].y != m_Label.ptOffset_Lable[nMatchCnt].y) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );

		strLog.Format( _T("[Label Marking Count][%d→%d]"), Label.nLabelMaskingCount, m_Label.nLabelMaskingCount );
		if(Label.nLabelMaskingCount != m_Label.nLabelMaskingCount) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeLabel, strLog );
	}
#endif
}

void CTeachTabLabel::DisableWnd(BOOL bEnable)
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

BOOL CTeachTabLabel::Save()
{
	UpdateData(TRUE);
	
	CheckData();

	CModelInfo::stLabelInfo& LabelInfo = CModelInfo::Instance()->GetLabelInfo();
	LabelInfo = m_Label;

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->m_bLabelAlignTeaching = TRUE;
	pInspectionVision->Save( CModelInfo::Instance()->GetModelNameLabel(), TEACH_TAB_IDX_LABEL );
#endif

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_LABEL );
#endif

	Refresh();

	UpdateData(FALSE);

	return TRUE;
}

void CTeachTabLabel::Refresh()
{
	UpdateRecipeList();
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	pInspectionVision->Load_Label( CModelInfo::Instance()->GetModelNameLabel() );

	CModelInfo::stLabelInfo& LabelInfo = CModelInfo::Instance()->GetLabelInfo();
	m_Label = LabelInfo;

	m_nInspCount = (int)m_Label.stInspectArea.Areas.size();

	GetDlgItem(IDC_SPIN_INSP_SEL)->EnableWindow(FALSE);

	UpdateUI();

	UpdateData(FALSE);
}

void CTeachTabLabel::Default()
{
	m_Label.Clear();

	UpdateData(FALSE);
}

void CTeachTabLabel::Cleanup()
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
	
	m_bIsTeachLabel = FALSE;
	m_bIsTeachLabel_Align = FALSE;
//	m_toolTipImg.HideTip();
}

HBRUSH CTeachTabLabel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_CHECK_LABEL_USEMANUALDATA:
	case IDC_LABEL_LABEL_USEMANUALDATA:
	case IDC_STATIC_DESCRIPTION:
	case IDC_STATIC_LIGHT_VALUE:
	case IDC_STATIC_LABEL_ANGLE_SPEC:
	case IDC_STATIC_LABEL_EDGE_OFFSET:
	case IDC_STATIC_LABEL_EDGE_OFFSET2:
	case IDC_STATIC_LABEL_MASKING_COUNT:
	case IDC_STATIC_LABEL_MASKING_NO:
	case IDC_STATIC_LABEL_MASKING_SCORE:
	case IDC_STATIC_INSP_AREA_NO:
	case IDC_STATIC_INSP_AREA_SELECT_NO:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
		break;
	}

	return hbr;
}

BOOL CTeachTabLabel::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );

	return TRUE;
}

void CTeachTabLabel::OnShowWindow(BOOL bShow, UINT nStatus)
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
		GetDlgItem(IDC_SPIN_INSP_SEL)->EnableWindow(FALSE);
	}
}

BOOL CTeachTabLabel::PreTranslateMessage(MSG* pMsg)
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

void APIENTRY CTeachTabLabel::_OnConfirmTracker( CRect& rcTrackRegion, UINT nIndexData, LPVOID lpUsrData )
{
	CTeachTabLabel* pThis = (CTeachTabLabel*)lpUsrData;
	pThis->OnConfirmTracker( rcTrackRegion, nIndexData );
}

void CTeachTabLabel::OnConfirmTracker( CRect& rcTrackRegion, UINT nViewIndex )
{
	COLORBOX clrBox;

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	CxImageObject* pImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex);
	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, nViewIndex);
	pGO->Reset();

	rcTrackRegion.NormalizeRect();
	CRect rcTrarack = rcTrackRegion;

	if(rcTrarack.left > rcTrarack.right)
	{
		if(rcTrarack.top > rcTrarack.bottom)
			rcTrackRegion.SetRect(rcTrarack.right, rcTrarack.bottom, rcTrarack.left, rcTrarack.top);
		else
			rcTrackRegion.SetRect(rcTrarack.right, rcTrarack.top, rcTrarack.left, rcTrarack.bottom);
	}
	else
		if(rcTrarack.top > rcTrarack.bottom)
			rcTrackRegion.SetRect(rcTrarack.left, rcTrarack.bottom, rcTrarack.right, rcTrarack.top);

	BOOL bRet = FALSE;

	if( m_bIsTeachLabel )
	{
		bRet = pInspectionVision->SetLearnModel_ForLabel( pGO, pImgObj, rcTrackRegion, nViewIndex, m_nCombo_Masking_Select );
		m_Label.ptOffset_Lable[m_nCombo_Masking_Select] = CPoint(rcTrackRegion.left, rcTrackRegion.top);

 		Save();
 		bRet = pInspectionVision->MatchModel_ForLabel( pGO, pImgObj, nViewIndex, m_nCombo_Masking_Select );
	}
	
	if ( m_bIsTeachLabel_Align )
	{
#if defined(RELEASE_4G) || defined(RELEASE_6G)
		bRet = pInspectionVision->SetLearnModel(pGO, pImgObj, rcTrackRegion, InspectTypeLabel, nViewIndex, 0, FALSE, FALSE);
#endif
		m_Label.ptOffset_Align = CPoint(rcTrackRegion.left, rcTrackRegion.top);

		Save();
		bRet = pInspectionVision->MatchModel_ForLabelAlign(pGO, pImgObj, nViewIndex);
	}

	if( !m_bIsTeachLabel && !m_bIsTeachLabel_Align )
	{
		pInspectionVision->SetInspPosition(nViewIndex, rcTrackRegion, rcTrackRegion, pGO);

		m_Label.stInspectArea.Areas.push_back(rcTrackRegion);
		m_Label.stInspectArea.AlignedAreas.push_back(rcTrackRegion);
		m_nInspCount++;

		bRet = TRUE;
	}

	if( bRet )
	{
		Cleanup();
		Save();

		UpdateData(FALSE);
	}
}

void CTeachTabLabel::OnBnClickedRadioLabelBypass()
{
	UpdateData(TRUE);
}

void CTeachTabLabel::OnCbnSelchangeComboLabelTotalMaskingCount()
{
	m_Label.nLabelMaskingCount = m_Combo_Masking_Total_Cnt.GetCurSel();

	if( m_Label.nLabelMaskingCount <= 0 /*m_nCombo_Masking_Select*/ )	UpdateMaskingCount(TRUE);
	else																UpdateMaskingCount();
	UpdateMaskingScore();
	UpdateTeachingImage();

	UpdateData( FALSE );
}

void CTeachTabLabel::OnCbnSelchangeComboLabelMaskingNumber()
{
	m_nCombo_Masking_Select = m_Combo_Select_Masking_Num.GetCurSel();	

	UpdateMaskingScore();
	UpdateTeachingImage();

	UpdateData( FALSE );
}

void CTeachTabLabel::UpdateMaskingCount(BOOL bComboReset/*= FALSE*/)
{
	BOOL bEnableUI = TRUE;
	if(m_Label.nLabelMaskingCount == 0) 
		bEnableUI = FALSE;

	GetDlgItem(IDC_COMBO_LABEL_MASKING_NUMBER)->EnableWindow(bEnableUI);
	GetDlgItem(IDC_COMBO_LABEL_MASKING_SCORE)->EnableWindow(bEnableUI);
	GetDlgItem(IDC_BTN_LABEL_TECH_MODEL)->EnableWindow(bEnableUI);

	m_Combo_Select_Masking_Num.ResetContent();
	if (m_Label.nLabelMaskingCount == 1)
		m_nCombo_Masking_Select = 0;
	if(!bEnableUI) return;

	for(int i = 0; i < m_Label.nLabelMaskingCount; i ++)
	{
		CString strBuff;
		strBuff.Format(_T("%d번 마킹"), i + 1);
		m_Combo_Select_Masking_Num.AddString( strBuff );
	}

	if(bComboReset)
	{
		m_nCombo_Masking_Select = 0;
		m_Combo_Select_Masking_Num.SetCurSel(0);
	}
	else if(0 <= m_nCombo_Masking_Select) 	
		m_Combo_Select_Masking_Num.SetCurSel(m_nCombo_Masking_Select);
}

void CTeachTabLabel::UpdateMaskingScore()
{
	int nSelectNum = m_Label.nLabelMaskingScoreType[m_nCombo_Masking_Select];
	m_Combo_Masking_Score.SetCurSel( nSelectNum );
}

void CTeachTabLabel::UpdateTeachingImage()
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return;

	int nMaskingCount = m_Label.nLabelMaskingCount;
	if (nMaskingCount == 0)
		m_nCombo_Masking_Select = 0;

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	EMatcher* MatchBuff = pInspectionVision->GetLabelMatchData(m_nCombo_Masking_Select);

	CString strImgPath;
	strImgPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TeachingCross.bmp");


	if ( nMaskingCount == 0 )
	{
		m_ImageObjectForLabelTeaching.LoadFromFile(strImgPath);
	}
	else if( MatchBuff->GetPatternLearnt() && nMaskingCount != 0 )
	{
		int nWidth =  MatchBuff->GetPatternWidth();
		int nHeight = MatchBuff->GetPatternHeight();

		m_ImageObjectForLabelTeaching.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strImgPath;
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("Label\\");
		strLoadPatch += CModelInfo::Instance()->GetModelNameLabel();
		strLoadPatch += _T("\\LabelInfo\\");

		strMatchBmpFileName.Format(_T("%sLabel_Mask%d.bmp"), strLoadPatch, m_nCombo_Masking_Select);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForLabelTeaching.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForLabelTeaching.LoadFromFile(strImgPath);
	}
	else 
	{
		m_ImageObjectForLabelTeaching.LoadFromFile(strImgPath);
	}
	m_ImageViewForLabelTeaching.ImageUpdate();

	////////////////////////////////////////////////////
	// Align
	int nIndex = IDX_AREA1;
#ifdef RELEASE_4G
	nIndex = IDX_AREA2;
#elif RELEASE_6G
	nIndex = IDX_AREA1;
#endif

	EMatcher* MatchBuff_Align = pInspectionVision->GetMatchData( nIndex, 0 );
	if (MatchBuff_Align->GetPatternLearnt())
	{
		int nWidth = MatchBuff_Align->GetPatternWidth();
		int nHeight = MatchBuff_Align->GetPatternHeight();

		m_ImageObjectForLabelTeaching_Align.Create(nWidth, nHeight, 8, 1);

		CString strLoadPatch, strInspName, strMatchBmpFileName, strImgPath;
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("Label\\");
		strLoadPatch += CModelInfo::Instance()->GetModelNameLabel();
		strLoadPatch += _T("\\LabelInfo\\");

		strMatchBmpFileName.Format(_T("%sAlign_%d.bmp"), strLoadPatch, 0);

		if (IsExistFile((LPCTSTR)strMatchBmpFileName))
			m_ImageObjectForLabelTeaching_Align.LoadFromFile(strMatchBmpFileName);
		else
			m_ImageObjectForLabelTeaching_Align.LoadFromFile(strImgPath);
	}
	else
	{
		m_ImageObjectForLabelTeaching_Align.LoadFromFile(strImgPath);
	}
	m_ImageViewForLabelTeaching_Align.ImageUpdate();

	m_pMainView->UpdateImageView();
#endif
}

void CTeachTabLabel::UpdateUI()
{
	UpdateMaskingCount();
	UpdateMaskingScore();

	UpdateTeachingImage();

	if( CVisionSystem::Instance()->GetManualData_Path().IsEmpty() ) 
	{
		GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA))->SetCheck(FALSE);

		GetDlgItem(IDC_LABEL_LABEL_USEMANUALDATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_LABEL_LABEL_USEMANUALDATA)->SetWindowText(_T("No ZPL Data"));
	}
	else
	{
		GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_LABEL_LABEL_USEMANUALDATA)->EnableWindow( ((CButton*)GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA))->GetCheck() );
		GetDlgItem(IDC_LABEL_LABEL_USEMANUALDATA)->SetWindowText(CVisionSystem::Instance()->GetManualData_Path());
	}
}

void CTeachTabLabel::OnBnClickedBtnLabelTechModel()
{
	WRITE_LOG( WL_BTN, _T("CTeachTabLabel::OnBnClickedBtnLabelTechModel") );

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	int nIndex = IDX_AREA1;
#ifdef RELEASE_4G
	nIndex = IDX_AREA2;
#elif RELEASE_6G
	nIndex = IDX_AREA1;
#endif
	m_pMainView->ResetGraphic( CamTypeAreaScan, nIndex );

	if( m_bIsTeachLabel )
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

		m_btnLabelTechModel.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, nIndex, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTabLabel::OnBnClickedBtnLabelTechOcrfontteaching()
{
	TeachOCRFont FontTeaching;
	if( FontTeaching.DoModal() == IDOK ){}
}


void CTeachTabLabel::OnBnClickedCheckLabelUsemanualdata()
{
	GetDlgItem(IDC_LABEL_LABEL_USEMANUALDATA)->EnableWindow( ((CButton*)GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA))->GetCheck() );
}


void CTeachTabLabel::OnBnClickedBtnTestLabel()
{
#if defined(RELEASE_4G) || defined(RELEASE_6G)
	WRITE_LOG( WL_BTN, _T("CTeachTabLabel::OnBtnTestLabel") );

	if( !CVisionSystem::Instance()->GetManualData_Path().IsEmpty() && ((CButton*)GetDlgItem(IDC_CHECK_LABEL_USEMANUALDATA))->GetCheck() ) 
		CModelInfo::Instance()->GetLabelInfo().bLabelManualInsp = TRUE;
	else	CModelInfo::Instance()->GetLabelInfo().bLabelManualInsp = FALSE;

#ifdef RELEASE_4G
	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA2 );
//	CVisionSystem::Instance()->StartManualLabelInspect(IDX_AREA2, InspectTypeLabel);
	CVisionSystem::Instance()->StartInspection( InspectTypeLabel, IDX_AREA2, 0, TRUE);
#elif RELEASE_6G
	CVisionSystem::Instance()->BeginStopWatch( IDX_AREA1 );
	CVisionSystem::Instance()->StartInspection( InspectTypeLabel, IDX_AREA1, 0, TRUE);
#endif

#endif
}


void CTeachTabLabel::OnBnClickedBtnLabelRecipeNo()
{
#if defined(RELEASE_4G) || defined(RELEASE_6G)
	WRITE_LOG(WL_MSG, _T("CTeachTabLabel::OnBnClickedBtnLabelRecipeNo :: Start"));

	if (CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)
	{
		WRITE_LOG(WL_MSG, _T("CTeachTabLabel::OnBnClickedBtnLabelRecipeNo :: Error - RunStatusAutoRun!!"));
		return;
	}

	CEditJobNumberDlg EditJobNumberDlg(this);
	EditJobNumberDlg.SetRecipeKind(_T("LABEL"), TEACH_TAB_IDX_LABEL);
	EditJobNumberDlg.SetTitleName(_T("Recipe No : MBB Label"));

	if (EditJobNumberDlg.DoModal() == IDOK)
	{

	}

	WRITE_LOG(WL_MSG, _T("CTeachTabLabel::OnBnClickedBtnLabelRecipeNo :: End"));
#endif

}

void CTeachTabLabel::OnCbnSelchangeComboRecipe()
{
#if defined(RELEASE_4G) || defined(RELEASE_6G)
	UpdateData(TRUE);

	if (m_nSelectRecipeIndex < 0)
		return;

	if (m_nSelectRecipeIndex == 0)
	{
		CAddNewRecipeDlg NewRecipeDlg;
		NewRecipeDlg.SetRecipeKind(LABEL_KIND);
		NewRecipeDlg.SetTeachTab(TEACH_TAB_IDX_LABEL);

		if (NewRecipeDlg.DoModal() == IDOK)
		{
			UpdateRecipeList();

			CString strModelName = CModelInfo::Instance()->GetModelNameLabel();

			Refresh();
		}
		else
		{
			CString strModelName = CModelInfo::Instance()->GetModelNameLabel();
			int nIndex = m_wndSelectRecipe.FindStringExact(0, strModelName);
			m_wndSelectRecipe.SetCurSel(nIndex);
			m_nSelectRecipeIndex = nIndex;
		}
	}
	else
	{
		CString strModelName = CModelInfo::Instance()->GetModelNameLabel();
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

void CTeachTabLabel::UpdateRecipeList()
{
	m_wndSelectRecipe.ResetContent();

	m_wndSelectRecipe.AddString(_T("New Recipe..."));

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("Label\\");
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

	CString strModelName = CModelInfo::Instance()->GetModelNameLabel();
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

void CTeachTabLabel::OnBnClickedBtnSaveLabel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTabLabel::OnBnClickedBtnSaveLabel :: Start"));

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CString strModelName = CModelInfo::Instance()->GetModelNameLabel();
	CString strSelectModelName;
	m_wndSelectRecipe.GetLBText(m_nSelectRecipeIndex, strSelectModelName);

	if (strSelectModelName != strModelName)
	{
		m_btnSave.SetWindowText(_T("Save"));
		if (CModelInfo::Instance()->Load(strSelectModelName, LABEL_KIND))
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
			pInspectionVision->Load_Label(strSelectModelName);
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

void CTeachTabLabel::OnBnClickedBtnInspAdd()
{
	ASSERT(m_pMainView);

	UpdateData(TRUE);

	int nIndex = IDX_AREA1;
#ifdef RELEASE_4G
	nIndex = IDX_AREA2;
#elif RELEASE_6G
	nIndex = IDX_AREA1;
#endif

	m_pMainView->ResetGraphic(CamTypeAreaScan, nIndex);

	GetDlgItem(IDC_BTN_LABEL_TECH_ALIGN_MODEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_LABEL_TECH_MODEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_TEST_LABEL)->EnableWindow(FALSE);

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	CVisionSystem::Instance()->m_bLabelROITeaching = TRUE;
	if( CVisionSystem::Instance()->StartInspection(InspectTypeLabel, nIndex, 0, TRUE) )
		m_pMainView->SetTrackerMode(TRUE, nIndex, _OnConfirmTracker, this);
	else
		AfxMessageBox(_T("Teaching Error : Label 찾기 실패!!"), MB_ICONERROR);
#endif
}

void CTeachTabLabel::OnBnClickedBtnShowarea()
{
	UpdateData(TRUE);
	AlignInspectionAreas();

	ViewInspAreas();

	GetDlgItem(IDC_SPIN_INSP_SEL)->EnableWindow(TRUE);
	m_wndSpinInsp.SetRange(0, m_nInspCount - 1);
}

void CTeachTabLabel::ViewInspAreas()
{
	int nIdx = IDX_AREA1;
#ifdef RELEASE_4G
	nIdx = IDX_AREA2;
#elif RELEASE_6G
	nIdx = IDX_AREA1;
#endif

	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, nIdx);
	pGO->Reset();

	CModelInfo::stLabelInfo::InspVecIter iterBegin = m_Label.stInspectArea.AlignedAreas.begin();
	CModelInfo::stLabelInfo::InspVecIter iterEnd = m_Label.stInspectArea.AlignedAreas.end();

	COLORBOX clrBox;
	COLORTEXT clrText;
	int nIndex = 0;
	for (CModelInfo::stLabelInfo::InspVecIter iter = iterBegin; iter != iterEnd; ++iter, ++nIndex)
	{
		CRect& rcArea = (*iter);
		clrBox.CreateObject((nIndex == m_nInspIndex ? PDC_GREEN : PDC_YELLOW), rcArea.left, rcArea.top, rcArea.right, rcArea.bottom);
		clrText.CreateObject((nIndex == m_nInspIndex ? PDC_GREEN : PDC_DEEPRED), rcArea.left, rcArea.top, 10);
		clrText.SetText(_T("%d"), nIndex);
		pGO->AddDrawBox(clrBox);
		pGO->AddDrawText(clrText);
	}

	m_pMainView->UpdateImageView();
}

void CTeachTabLabel::AlignInspectionAreas()
{
	int nIndex = IDX_AREA1;
#ifdef RELEASE_4G
	nIndex = IDX_AREA2;
#elif RELEASE_6G
	nIndex = IDX_AREA1;
#endif

	CxGraphicObject* pGO = m_pMainView->GetGraphicObject(CamTypeAreaScan, nIndex);
	pGO->Reset();

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	CVisionSystem::Instance()->m_bLabelROITeaching = TRUE;
	CVisionSystem::Instance()->StartInspection(InspectTypeLabel, nIndex, 0, TRUE);

	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
	CPoint ptLeftTop = pInspectionVision->m_ptAlign;

	pInspectionVision->MakeMarkAlignedInspectArea(m_Label.stInspectArea, ptLeftTop, 0.f); // Label 회전 이후 -> 각도 0?
#endif
}

void CTeachTabLabel::OnBnClickedBtnInspSeldel()
{
	UpdateData(TRUE);
	if (m_nInspIndex >= m_nInspCount || m_nInspIndex < 0)
	{
		return;
	}

	if (m_Label.stInspectArea.Areas.size() <= 0)
		return;

	m_Label.stInspectArea.Areas.erase(m_Label.stInspectArea.Areas.begin() + m_nInspIndex);
	AlignInspectionAreas();

	m_nInspCount = (int)m_Label.stInspectArea.Areas.size();
	if (m_nInspCount != 0 && m_nInspIndex >= m_nInspCount)
	{
		m_nInspIndex = m_nInspCount - 1;
	}

	if (m_nInspCount <= 0)
		m_wndSpinInsp.SetRange(0, 0);
	else
		m_wndSpinInsp.SetRange(0, m_nInspCount - 1);

	m_wndSpinInsp.Invalidate();

	ViewInspAreas();

	UpdateData(FALSE);
}


void CTeachTabLabel::OnBnClickedBtnInspAlldel()
{
	CString strMessage = _T("모든 검사 영역을 삭제 하시겠습니까?");
	if (CLanguageInfo::Instance()->m_nLangType == 1)
		strMessage = _T("Do you want to delete all inspection areas?");

	if (AfxMessageBox(strMessage, MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	UpdateData(TRUE);

	m_Label.stInspectArea.AlignedAreas.clear();
	m_Label.stInspectArea.Areas.clear();

	ViewInspAreas();

	m_nInspCount = 0;
	m_nInspIndex = 0;
	m_wndSpinInsp.SetRange(0, 0);
	m_wndSpinInsp.Invalidate();

	UpdateData(FALSE);
}

void CTeachTabLabel::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_INSP_SEL)
	{
		m_nInspIndex = nPos;
		if ((m_Label.stInspectArea.Areas.size() != 0) && (m_nInspIndex >= m_Label.stInspectArea.Areas.size()))
			m_nInspIndex = (int)m_Label.stInspectArea.Areas.size() - 1;
		if ((m_Label.stInspectArea.Areas.size() == 0))	m_nInspIndex = 0;
		m_wndSpinInsp.SetPos(m_nInspIndex);
		UpdateData(FALSE);
		ViewInspAreas();
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CTeachTabLabel::OnBnClickedBtnLabelTechAlignModel()
{
	WRITE_LOG(WL_BTN, _T("CTeachTabLabel::OnBnClickedBtnLabelTechAlignModel"));

	ASSERT(m_pMainView);

	UpdateData(TRUE);

	int nIndex = IDX_AREA1;
#ifdef RELEASE_4G
	nIndex = IDX_AREA2;
#elif RELEASE_6G
	nIndex = IDX_AREA1;
#endif
	m_pMainView->ResetGraphic(CamTypeAreaScan, nIndex);

	if (m_bIsTeachLabel_Align)
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

		m_btnLabelTechModel_Align.EnableWindow(TRUE);
		m_pMainView->SetTrackerMode(TRUE, nIndex, _OnConfirmTracker, this);
	}
	else
	{
		Cleanup();
	}
}

void CTeachTabLabel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pCtrl = GetDlgItem(IDC_LABEL_TITLE_LABEL);

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
				strParams.Format(_T("\"file:///%s#page=54\""), (LPCTSTR)strPdfPath);

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
