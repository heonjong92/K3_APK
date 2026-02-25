// AddNewRecipeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "AddNewRecipeDlg.h"
#include "afxdialogex.h"

#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"
#include "ModelInfo.h"

#include "VisionSystem.h"
#include "StringMacro.h"
#include "LanguageInfo.h"

#include <XUtil/xUtils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CAddNewRecipeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAddNewRecipeDlg, CDialog)

CAddNewRecipeDlg::CAddNewRecipeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNewRecipeDlg::IDD, pParent)
	, m_strNewRecipeName(_T(""))
	, m_nSelectRecipeIndex(0)
{
	m_clrBorder = RGB(126, 27, 29);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("Add New Recipe");
	m_strKind = _T("");
	m_eTabIdx = TEACH_TAB_IDX_ALL;

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_ACCESS_RIGHT );

	m_fontEdit.CreateFont( 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, _T("Arial") );
}

CAddNewRecipeDlg::~CAddNewRecipeDlg()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CAddNewRecipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABEL_RECIPE, m_wndLabelRecipeName);
	DDX_Text(pDX, IDC_EDIT_RECIPE_NAME, m_strNewRecipeName);
	DDX_Control(pDX, IDC_COMBO_RECIPE, m_wndRecipeList);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_EDIT_RECIPE_NAME, m_edtRecipeName);
	DDX_CBIndex(pDX, IDC_COMBO_RECIPE, m_nSelectRecipeIndex);
}


BEGIN_MESSAGE_MAP(CAddNewRecipeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CAddNewRecipeDlg::OnBnClickedOk)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CAddNewRecipeDlg::OnPaint()
{
	CPaintDC dc(this);

	using namespace Gdiplus;

	CRect rcClient;
	GetClientRect( rcClient );

	CRect rcSplit2;
	GetDlgItem(IDC_SPLIT2)->GetWindowRect(rcSplit2);
	ScreenToClient(rcSplit2);

	int nTopOffset = 36;
	int nBorderWidth = m_nBorderWidth;

	CRect rcTitle = rcClient;
	rcTitle.bottom = rcTitle.top + nTopOffset;

	dc.FillSolidRect( rcClient.left, rcClient.top, rcClient.Width(), rcSplit2.top-rcClient.top, m_clrBody );

	dc.FillSolidRect( rcTitle, m_clrBorder );

	dc.FillSolidRect( rcClient.left, rcClient.bottom-nBorderWidth, rcClient.Width(), nBorderWidth, m_clrBorder );
	dc.FillSolidRect( rcClient.left, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );
	dc.FillSolidRect( rcClient.right-nBorderWidth, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );

	//dc.FillSolidRect( rcClient.left, rcSplit2.top, rcClient.Width(), nBorderWidth, m_clrBorder );

	const int nLeftOffsetX = 8;

	Graphics g(dc.GetSafeHdc());

	int nIW = m_pIconImage->GetWidth();
	int nIH = m_pIconImage->GetHeight();
	int nIX = rcTitle.left + nLeftOffsetX;
	int nIY = rcTitle.top + (rcTitle.Height() - nIH) / 2;

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.7f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	g.DrawImage( m_pIconImage, Rect(nIX, nIY, nIW, nIH), 0, 0, nIW, nIH, UnitPixel, &ImgAttr );

	Color colorText(180, 0,0,0);
	SolidBrush brushText( colorText );
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	stringFormat.SetAlignment( StringAlignmentNear );
	SolidBrush brushTitle(Color(200,255,255,255));
	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();
	Gdiplus::Font fontTitle(pFontFamily, 15, FontStyleBold, UnitPixel);
	BSTR bstrTitle = m_strTitle.AllocSysString();
	g.DrawString( bstrTitle, -1, &fontTitle, RectF((float)rcTitle.left+nIX+nIW+3, (float)rcTitle.top, (float)rcTitle.Width()-8, (float)rcTitle.Height()), &stringFormat, &brushTitle );
	SysFreeString( bstrTitle );
}

BOOL CAddNewRecipeDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CAddNewRecipeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		return m_brushBody;
		break;
	}

	return hbr;
}

BOOL CAddNewRecipeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwBtnColor = RGB(175, 37, 40);
	m_btnOK.SetFontHeight(12);
	m_btnOK.SetButtonColor( dwBtnColor );
	m_btnOK.SetBold(FALSE);
	m_btnCancel.SetFontHeight(12);
	m_btnCancel.SetButtonColor( dwBtnColor );
	m_btnCancel.SetBold(FALSE);

	m_wndLabelRecipeName.SetFontHeight(12);
	m_wndLabelRecipeName.SetColor( m_clrBorder, RGB(255, 255, 255) );
	m_wndLabelRecipeName.SetBold(FALSE);

	m_edtRecipeName.SetInvertFilter( _T("/\\:*<>?\"|") );
	m_edtRecipeName.SetFont(&m_fontEdit);

	UpdateRecipeList();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAddNewRecipeDlg::OnBnClickedOk()
{
	WRITE_LOG( WL_BTN, _T("CAddNewRecipeDlg::OnBnClickedOk") );

	UpdateData(TRUE);

	BOOL bSave = FALSE;

	if (m_strNewRecipeName.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_RECIPE_NAME)->SetFocus();
		return;
	}

	if (CModelInfo::IsExistModelName(m_strKind + _T("\\") + m_strNewRecipeName)) // 검사 종류 추가
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	AfxMessageBox(_T("레시피 이름이 이미 존재합니다."), MB_OK | MB_ICONERROR);
		else												AfxMessageBox(_T("The recipe name already exists."), MB_OK | MB_ICONERROR);
		return;
	}

	if (m_nSelectRecipeIndex < 0)
	{
		// 신규 레시피
		CModelInfo::Instance()->New(m_strNewRecipeName, GetRecipeKind());
		bSave = CModelInfo::Instance()->Save(GetTeachTab());
	}
	else
	{
		CString strSrcRecipe;
		m_wndRecipeList.GetLBText(m_nSelectRecipeIndex, strSrcRecipe);
		if( !strSrcRecipe.IsEmpty() )
		{
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();

			CModelInfo::Instance()->Load(strSrcRecipe, GetRecipeKind());
			pInspectionVision->Load_Label(strSrcRecipe);
			pInspectionVision->Load(strSrcRecipe, GetRecipeKind());

			CString strKind = GetRecipeKind();
			
			if		(strKind == TRAYOCR_KIND)					CModelInfo::Instance()->RenameTrayOcr(m_strNewRecipeName);
			else if (strKind == TOPCHIPCNT_KIND)				CModelInfo::Instance()->Rename3DChipCnt(m_strNewRecipeName);
			else if (strKind == CHIPOCR_KIND)					CModelInfo::Instance()->RenameChipOcr(m_strNewRecipeName);
			else if (strKind == CHIP_KIND)						CModelInfo::Instance()->RenameChip(m_strNewRecipeName);
			else if (strKind == MIXING_KIND)					CModelInfo::Instance()->RenameMixing(m_strNewRecipeName);
			else if (strKind == LIFT_KIND)						CModelInfo::Instance()->RenameLift(m_strNewRecipeName);
			else if (strKind == STACKEROCR_KIND)				CModelInfo::Instance()->RenameStackerOcr(m_strNewRecipeName);
			else if (strKind == BANDING_KIND)					CModelInfo::Instance()->RenameBanding(m_strNewRecipeName);
			else if (strKind == HIC_KIND)						CModelInfo::Instance()->RenameHIC(m_strNewRecipeName);
			else if (strKind == DESICCANT_KIND)					CModelInfo::Instance()->RenameDesiccantCutting(m_strNewRecipeName);
			else if (strKind == SUBMATERIAL_KIND)				CModelInfo::Instance()->RenameDesiccantMaterial(m_strNewRecipeName);
			else if (strKind == SUBMATERIALTRAY_KIND)			CModelInfo::Instance()->RenameDesiccantMaterialTray(m_strNewRecipeName); // 추가
			else if (strKind == MBB_KIND)						CModelInfo::Instance()->RenameMBB(m_strNewRecipeName);
			else if (strKind == BOXQUALITY_KIND)				CModelInfo::Instance()->RenameBoxQuality(m_strNewRecipeName);
			else if (strKind == SEALINGQUALITY_KIND)			CModelInfo::Instance()->RenameSealingQuality(m_strNewRecipeName);
			else if (strKind == BOXTAPE_KIND)					CModelInfo::Instance()->RenameTape(m_strNewRecipeName);
			else if (strKind == LABEL_KIND)						CModelInfo::Instance()->RenameLabel(m_strNewRecipeName);
			else												CModelInfo::Instance()->Rename(m_strNewRecipeName);

			//////////////////////////////////////////////////////////////////////////
			bSave = CModelInfo::Instance()->Save();

#ifdef RELEASE_1G
			if (strKind == TRAYOCR_KIND)				pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_TRAYOCR);
			if (strKind == CHIPOCR_KIND)				pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_CHIPOCR);
			if (strKind == MIXING_KIND)					pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_MIXING);

#elif RELEASE_SG
			if (strKind == STACKEROCR_KIND)				pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_STACKER_OCR);

#elif RELEASE_3G
			if (strKind == DESICCANT_KIND)				pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_DESICCANT_MATERIAL);
			if (strKind == SUBMATERIAL_KIND)			pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY); // 추가

#elif RELEASE_4G
			if (strKind == LABEL_KIND)					pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_LABEL);

#elif RELEASE_5G
			if (strKind == BOXQUALITY_KIND)				pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_BOX);

#elif RELEASE_6G
			if (strKind == LABEL_KIND)					pInspectionVision->Save(m_strNewRecipeName, TEACH_TAB_IDX_LABEL);

#endif
		}
	}

	if (bSave)
	{
		CModelInfo::Instance()->Save_JobNumber(GetTeachTab());
		CVisionSystem::Instance()->JobNumberLoad();
	}

	CDialog::OnOK();
}

LRESULT CAddNewRecipeDlg::OnNcHitTest(CPoint point)
{
	int nTopOffset = 36;
	CRect rc;
	GetClientRect(rc);
	ClientToScreen(rc);
	rc.bottom = rc.top + nTopOffset;
	if (rc.PtInRect(point))
		return HTCAPTION;

	return CDialog::OnNcHitTest(point);
}

void CAddNewRecipeDlg::UpdateRecipeList()
{
	m_wndRecipeList.ResetContent();

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath();
	CString strKind = GetRecipeKind();
	strRecipeRootPath += strKind + _T("\\");

	CString strFilter = strRecipeRootPath+_T("*.*");

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

		if (IsExistFile((LPCTSTR)(strRecipeRootPath+strRecipeName+_T("\\ModelInfo.ini"))))
		{
			m_wndRecipeList.AddString( ff.GetFileTitle() );
		}
	}

	ff.Close();

	if (m_wndRecipeList.GetCount() == 0)
	{
		m_wndRecipeList.EnableWindow(FALSE);
	}
	else
	{
		m_wndRecipeList.EnableWindow(TRUE);
	}
}
