// TrayRecipeInfoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TrayRecipeInfoDlg.h"
#include "afxdialogex.h"

#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"

#include "JobNumberData.h"
#include "ModelInfo.h"
#include <XUtil/xUtils.h>
#include "APKView.h"
#include "VisionSystem.h"
#include "StringMacro.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CTrayRecipeInfoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTrayRecipeInfoDlg, CDialog)

#ifdef RELEASE_1G
CTrayRecipeInfoDlg::CTrayRecipeInfoDlg(CTeachTab1GTrayOcr* pParentTab, CWnd* pParent /*=NULL*/)
#else
CTrayRecipeInfoDlg::CTrayRecipeInfoDlg(CTeachTabSGStackerOcr* pParentTab, CWnd* pParent /*=NULL*/)
#endif
	: CDialog(CTrayRecipeInfoDlg::IDD, pParent), m_pParentTab(pParentTab)
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

CTrayRecipeInfoDlg::~CTrayRecipeInfoDlg()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CTrayRecipeInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,	IDOK,								m_btnSave);
	DDX_Control(pDX,	IDCANCEL,							m_btnClose);
	DDX_Control(pDX,	IDC_LIST_TRAY_RECIPE,				m_wndJobNumberList);
	DDX_Control(pDX,	IDC_PICTURE_PREVIEW_RECIPE_INFO,	m_ctrlPicPreviewRecipeInfo);
}

BEGIN_MESSAGE_MAP(CTrayRecipeInfoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDOK,	&CTrayRecipeInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,	&CTrayRecipeInfoDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TRAY_RECIPE, &CTrayRecipeInfoDlg::OnNMClickJobNumberList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TRAY_RECIPE, &CTrayRecipeInfoDlg::OnLvnItemchangedJobNumberList)
END_MESSAGE_MAP()

void CTrayRecipeInfoDlg::OnPaint()
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

BOOL CTrayRecipeInfoDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CTrayRecipeInfoDlg::UpdateJobNumberList()
{
	m_wndJobNumberList.DeleteAllItems();

	CJobNumberData* pJobNumberData = CJobNumberData::Instance();
	
	CString strJobNumber;
	UINT nMaxCnt = pJobNumberData->GetMaxCount();
	
	for (UINT i=0 ; i<nMaxCnt ; i++)
	{
		strJobNumber.Format(_T("%d"), i+1);
		int nJobIdx = m_wndJobNumberList.InsertItem(LVIF_TEXT, m_wndJobNumberList.GetItemCount(), strJobNumber, 0, 0, 0, 0);
		CString strModelName = pJobNumberData->GetModelName(i + 1, GetRecipeKindTabIdx());
		m_wndJobNumberList.SetItemText( nJobIdx, 1, strModelName );
		m_wndJobNumberList.SetItemText( nJobIdx, 2, pJobNumberData->GetTrayOcrInfo(strModelName, GetRecipeKindTabIdx()) );
	}

	m_wndJobNumberList.SetColumnWidth( 2, LVSCW_AUTOSIZE_USEHEADER );
}

HBRUSH CTrayRecipeInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CTrayRecipeInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwBtnColor = RGB(175, 37, 40);
	m_btnSave.SetFontHeight(12);
	m_btnSave.SetButtonColor( dwBtnColor );
	m_btnSave.SetBold(FALSE);
	m_btnClose.SetFontHeight(12);
	m_btnClose.SetButtonColor( dwBtnColor );
	m_btnClose.SetBold(FALSE);

	m_ctrlPicPreviewRecipeInfo.GetClientRect(m_rcPicCtrlSizeRecipeInfo);

	m_wndJobNumberList.InsertColumn( 0, _T("No"), LVCFMT_RIGHT, 50 );
	m_wndJobNumberList.InsertColumn( 1, _T("Tray SID"), LVCFMT_CENTER, 150 );
	m_wndJobNumberList.InsertColumn( 2, _T("Tab Ocr Info"), LVCFMT_LEFT, 200 );

	m_wndJobNumberList.SetReadOnlyColumns(0);
	m_wndJobNumberList.SetReadOnlyColumns(1);
	m_wndJobNumberList.SetReadOnlyColumns(2);

	m_wndJobNumberList.EnableVScroll(); 

	m_wndJobNumberList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	CJobNumberData* pJobNumberData = CJobNumberData::Instance();
	pJobNumberData->SetRecipeKind(GetRecipeKind(), GetRecipeKindTabIdx());

	UpdateJobNumberList();

	// Matching Viewer
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetViewerBodyColor();

	m_ImageViewForRecipeInfo.Create(&m_ImageViewManagerForRecipeInfo, this);
	m_ImageViewForRecipeInfo.ShowWindow(SW_SHOW);
	m_ImageViewForRecipeInfo.ShowTitleWindow(FALSE);
	m_ImageViewForRecipeInfo.SetAnimateWindow(FALSE);
	m_ImageViewForRecipeInfo.SetImageObject(&m_ImageObjectForRecipeInfo, FALSE);
	m_ImageViewForRecipeInfo.SetRegisterCallBack(0, &regCB);
	m_ImageViewForRecipeInfo.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForRecipeInfo.SetBodyColor(dwBodyColor);
	m_ImageViewForRecipeInfo.ShowStatusWindow(FALSE);
	m_ImageViewForRecipeInfo.ShowScrollBar(FALSE);
	m_ImageViewForRecipeInfo.ShowScaleBar(FALSE);
	m_ImageViewForRecipeInfo.EnableMouseControl(FALSE);

	LockWindowUpdate();

	CRect reViewSize;
	GetDlgItem(IDC_PICTURE_PREVIEW_RECIPE_INFO)->GetWindowRect(reViewSize);
	ScreenToClient(reViewSize);

	HDWP hDWP = BeginDeferWindowPos(0);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForRecipeInfo.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);
	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CTrayRecipeInfoDlg::OnNcHitTest(CPoint point)
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

void CTrayRecipeInfoDlg::OnBnClickedOk()
{
	WRITE_LOG( WL_BTN, _T("CTrayRecipeInfoDlg::OnBnClickedOk") );

	UpdateData(TRUE);

#ifdef RELEASE_1G
	if (CModelInfo::Instance()->GetModelNameTrayOcr() != m_strSelectRecipe)
	{
		CModelInfo::Instance()->Load(m_strSelectRecipe, TRAYOCR_KIND);

		if (m_pParentTab)
		{
			m_pParentTab->SelectRecipeIndex(m_nSelectIndex);
			m_pParentTab->Refresh();   // 모델 로드 후 UI 갱신
		}
	}

#else
	if (CModelInfo::Instance()->GetModelNameStackerOcr() != m_strSelectRecipe)
	{
		CModelInfo::Instance()->Load(m_strSelectRecipe, STACKEROCR_KIND);

		if (m_pParentTab)
		{
			m_pParentTab->SelectRecipeIndex(m_nSelectIndex);
			m_pParentTab->Refresh();   // 모델 로드 후 UI 갱신
		}
	}

#endif

	UpdateData(FALSE);
	CDialog::OnOK();
}

void CTrayRecipeInfoDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void CTrayRecipeInfoDlg::OnNMClickJobNumberList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 클릭된 항목 인덱스 얻기
	int nItem = m_wndJobNumberList.GetNextItem(-1, LVNI_SELECTED);
	if (nItem < 0)
		return;

	// Tray SID (1번 컬럼) 읽기
	CString traySID = m_wndJobNumberList.GetItemText(nItem, 1);

#ifdef RELEASE_1G
	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_TrayOcr");

#else
	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("SG_StackerOcr");

#endif

	CString strFile = _T("OriginImage.bmp");

	// 폴더 경로 구성 (원하는 경로로 바꿔도 됨)
	CString folderPath;
	folderPath.Format(_T("%s\\%s\\%s"), strRecipeRootPath, traySID, strFile);

	// 폴더 열기
	m_ImageObjectForRecipeInfo.LoadFromFile(folderPath);

	m_ImageViewForRecipeInfo.ImageUpdate();

	m_strSelectRecipe = traySID;
	m_nSelectIndex = nItem+1;
	*pResult = 0;
}

void CTrayRecipeInfoDlg::OnLvnItemchangedJobNumberList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 선택된 항목일 때만 처리
	if (pNMLV->uChanged & LVIF_STATE)
	{
		// 선택상태가 ON일 때만
		if ((pNMLV->uNewState & LVIS_SELECTED) != 0)
		{
			int nItem = pNMLV->iItem;
			if (nItem >= 0)
			{
				CString traySID = m_wndJobNumberList.GetItemText(nItem, 1);
#ifdef RELEASE_1G
				CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("1G_TrayOcr");

#else
				CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("SG_StackerOcr");

#endif
				CString strFile = _T("OriginImage.bmp");

				CString folderPath;
				folderPath.Format(_T("%s\\%s\\%s"), strRecipeRootPath, traySID, strFile);

				m_ImageObjectForRecipeInfo.LoadFromFile(folderPath);
				m_ImageViewForRecipeInfo.ImageUpdate();

				m_strSelectRecipe = traySID;
				m_nSelectIndex = nItem+1;
			}
		}
	}

	*pResult = 0;
}
