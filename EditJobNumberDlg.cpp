// EditJobNumberDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "EditJobNumberDlg.h"
#include "afxdialogex.h"

#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"

#include "JobNumberData.h"
#include "ModelInfo.h"
#include <XUtil/xUtils.h>
#include "APKView.h"
#include "VisionSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CEditJobNumberDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEditJobNumberDlg, CDialog)

CEditJobNumberDlg::CEditJobNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditJobNumberDlg::IDD, pParent)
{
	m_clrBorder = RGB(126, 27, 29);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("Recipe No : None");
	m_strKind = _T("");
	m_eTabIdx = TEACH_TAB_IDX_ALL;

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_ACCESS_RIGHT );

	m_nRecipeJobNumber = 0;
}

CEditJobNumberDlg::~CEditJobNumberDlg()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CEditJobNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,	IDOK,					m_btnSave);
	DDX_Control(pDX,	IDCANCEL,				m_btnClose);
	DDX_Control(pDX,	IDC_LIST_JOB_NUMBER,	m_wndJobNumberList);
}


BEGIN_MESSAGE_MAP(CEditJobNumberDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDOK,			&CEditJobNumberDlg::OnBnClickedOk)
	ON_MESSAGE(WM_SET_ITEMS,	PopulateComboList)
	ON_BN_CLICKED(IDCANCEL,		&CEditJobNumberDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


void CEditJobNumberDlg::OnPaint()
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

BOOL CEditJobNumberDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CEditJobNumberDlg::UpdateJobNumberList()
{
	m_wndJobNumberList.DeleteAllItems();

	CJobNumberData* pJobNumberData = CJobNumberData::Instance();
	
	CString strJobNumber;
	UINT nMaxCnt = pJobNumberData->GetMaxCount();
	
	for (UINT i=0 ; i<nMaxCnt ; i++)
	{
		strJobNumber.Format(_T("%d"), i+1);
		int nJobIdx = m_wndJobNumberList.InsertItem(LVIF_TEXT, m_wndJobNumberList.GetItemCount(), strJobNumber, 0, 0, 0, 0);
		m_wndJobNumberList.SetItemText( nJobIdx, 1, pJobNumberData->GetModelName(i+1, GetRecipeKindTabIdx()) );
	}

	m_wndJobNumberList.SetColumnWidth( 1, LVSCW_AUTOSIZE_USEHEADER );
}

void CEditJobNumberDlg::AddComboList( CStringList* pComboList )
{
	CString strRecipeKind = GetRecipeKind();
	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + strRecipeKind + _T("\\");	
	CString strFilter = strRecipeRootPath + _T("*.*");

	pComboList->AddTail( _T("") );

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
			pComboList->AddTail( ff.GetFileTitle() );
		}
	}

	ff.Close();
}

LRESULT CEditJobNumberDlg::PopulateComboList(WPARAM wParam, LPARAM lParam)
{
	CComboBox* pInPlaceCombo = static_cast<CComboBox*>( GetFocus() );

	CRect obWindowRect;

	pInPlaceCombo->GetWindowRect( &obWindowRect );
	
	CPoint obInPlaceComboTopLeft( obWindowRect.TopLeft() ); 
	
	m_wndJobNumberList.GetWindowRect( &obWindowRect );
	
	int iColIndex = (int)wParam;
	
	CStringList* pComboList = reinterpret_cast<CStringList*>(lParam);
	pComboList->RemoveAll(); 

	if ( obWindowRect.PtInRect(obInPlaceComboTopLeft) )
	{
		if ( 1 == iColIndex )
		{
			AddComboList( pComboList );
		}
	}
	
	return 1;
}

HBRUSH CEditJobNumberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CEditJobNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwBtnColor = RGB(175, 37, 40);
	m_btnSave.SetFontHeight(12);
	m_btnSave.SetButtonColor( dwBtnColor );
	m_btnSave.SetBold(FALSE);
	m_btnClose.SetFontHeight(12);
	m_btnClose.SetButtonColor( dwBtnColor );
	m_btnClose.SetBold(FALSE);

	m_wndJobNumberList.InsertColumn( 0, _T("No"), LVCFMT_CENTER, 80 );
	m_wndJobNumberList.InsertColumn( 1, _T("Name"), LVCFMT_LEFT, 100 );
	m_wndJobNumberList.SetColumnWidth( 1, LVSCW_AUTOSIZE_USEHEADER );

	m_wndJobNumberList.SetReadOnlyColumns(0);
	m_wndJobNumberList.SetComboColumns(1);

	m_wndJobNumberList.EnableVScroll(); 

	m_wndJobNumberList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	CJobNumberData* pJobNumberData = CJobNumberData::Instance();
	pJobNumberData->SetRecipeKind(GetRecipeKind(), GetRecipeKindTabIdx());

	UpdateJobNumberList();

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CEditJobNumberDlg::OnNcHitTest(CPoint point)
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


void CEditJobNumberDlg::OnBnClickedOk()
{
	WRITE_LOG( WL_BTN, _T("CEditJobNumberDlg::OnBnClickedOk") );

	CString strModelName = CModelInfo::Instance()->GetModelName();
	m_nRecipeJobNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, GetRecipeKindTabIdx());

	CString strRecipe;
	CJobNumberData* pJobNumberData = CJobNumberData::Instance();
	UINT nMaxCnt = pJobNumberData->GetMaxCount();

	for ( UINT i=0 ; i<nMaxCnt ; i++ )
	{
		strRecipe = m_wndJobNumberList.GetItemText( i, 1 );
		pJobNumberData->SetJobNumber(i+1, strRecipe, GetRecipeKindTabIdx());
	}

	pJobNumberData->Save();

	int nJobNumber_Check = CJobNumberData::Instance()->GetJobNumber(strModelName, GetRecipeKindTabIdx());

	if(m_nRecipeJobNumber != nJobNumber_Check) m_nRecipeJobNumber = nJobNumber_Check;

	CDialog::OnOK();
}


void CEditJobNumberDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
