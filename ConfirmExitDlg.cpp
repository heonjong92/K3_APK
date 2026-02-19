// ConfirmExitDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "ConfirmExitDlg.h"
#include "afxdialogex.h"

#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CConfirmExitDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConfirmExitDlg, CDialog)

CConfirmExitDlg::CConfirmExitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmExitDlg::IDD, pParent)
	, m_nExitCode(0)
	, m_strEtc(_T(""))
{
	//m_clrBorder = RGB(35, 47, 56);
	m_clrBorder = RGB(126, 27, 29);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

#ifdef RELEASE_1G
	m_strTitle = _T("AUTO PACKING SYSTEM PTC201");
#elif RELEASE_SG
	m_strTitle = _T("AUTO PACKING SYSTEM PTC201S");
#elif RELEASE_2G
	m_strTitle = _T("AUTO PACKING SYSTEM PBH202");
#elif RELEASE_3G
	m_strTitle = _T("AUTO PACKING SYSTEM PDK203");
#elif RELEASE_4G
	m_strTitle = _T("AUTO PACKING SYSTEM PMB204");
#elif RELEASE_5G
	m_strTitle = _T("AUTO PACKING SYSTEM PMF205");
#elif RELEASE_6G
	m_strTitle = _T("AUTO PACKING SYSTEM PWF207");
#endif

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_EXIT );
}

CConfirmExitDlg::~CConfirmExitDlg()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CConfirmExitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_EXIT_CODE1, m_nExitCode);
	DDX_Text(pDX, IDC_EDIT_ETC, m_strEtc);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CConfirmExitDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_EXIT_CODE1, &CConfirmExitDlg::OnBnClickedExitCode)
	ON_BN_CLICKED(IDC_EXIT_CODE2, &CConfirmExitDlg::OnBnClickedExitCode)
	ON_BN_CLICKED(IDC_EXIT_CODE3, &CConfirmExitDlg::OnBnClickedExitCode)
	ON_BN_CLICKED(IDC_EXIT_CODE4, &CConfirmExitDlg::OnBnClickedExitCode)
	ON_BN_CLICKED(IDOK, &CConfirmExitDlg::OnBnClickedOk)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CConfirmExitDlg 메시지 처리기입니다.


void CConfirmExitDlg::OnPaint()
{
	CPaintDC dc(this);

	using namespace Gdiplus;

	CRect rcClient;
	GetClientRect( rcClient );

	CRect rcSplit1, rcSplit2;
	GetDlgItem(IDC_SPLIT1)->GetWindowRect(rcSplit1);
	ScreenToClient(rcSplit1);
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

	//dc.FillSolidRect( rcClient.left, rcSplit1.top, rcClient.Width(), nBorderWidth, m_clrBorder );
	//dc.FillSolidRect( rcClient.left, rcSplit2.top, rcClient.Width(), nBorderWidth, m_clrBorder );

	CRect rcMessageBody;
	rcMessageBody.left = rcClient.left;
	rcMessageBody.top = rcTitle.bottom;
	rcMessageBody.right = rcClient.right;
	rcMessageBody.bottom = rcSplit1.top;

	Graphics g(dc.GetSafeHdc());

	//g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintAntiAlias);
	//g.SetCompositingQuality(CompositingQualityHighQuality);

	RectF rectMessageF(
		(float)rcMessageBody.left,
		(float)rcMessageBody.top,
		(float)rcMessageBody.Width(),
		(float)rcMessageBody.Height()
		);

	Color colorText(200, 126, 27, 29);
	SolidBrush brushText( colorText );
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();

	Gdiplus::Font fontText(pFontFamily, 20, FontStyleBold, UnitPixel);
	WCHAR* wszMessage = L"Do you want Exit System?";

	RectF boundingBoxF;
	g.MeasureString( wszMessage, -1, &fontText, rectMessageF, &stringFormat, &boundingBoxF );

	int nIW = m_pIconImage->GetWidth();
	int nIH = m_pIconImage->GetHeight();
	int nIX = (int)(boundingBoxF.X - (boundingBoxF.Height - nIH) / 2 + .5f) - nIW/2;
	int nIY = (int)(boundingBoxF.Y + (boundingBoxF.Height - nIH) / 2 + .5f);

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.9f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	g.DrawImage( m_pIconImage, Rect(nIX, nIY, nIW, nIH), 0, 0, nIW, nIH, UnitPixel, &ImgAttr );

	boundingBoxF.X += nIW/2.f;
	g.DrawString( wszMessage, -1, &fontText, boundingBoxF, &stringFormat, &brushText );

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	stringFormat.SetAlignment( StringAlignmentNear );
	SolidBrush brushTitle(Color(200,255,255,255));
	Gdiplus::Font fontTitle(pFontFamily, 15, FontStyleBold, UnitPixel);
	BSTR bstrTitle = m_strTitle.AllocSysString();
	g.DrawString( bstrTitle, -1, &fontTitle, RectF((float)rcTitle.left+8, (float)rcTitle.top, (float)rcTitle.Width()-8, (float)rcTitle.Height()), &stringFormat, &brushTitle );
	SysFreeString( bstrTitle );
}


BOOL CConfirmExitDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}


HBRUSH CConfirmExitDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_EXIT_CODE1:
	case IDC_EXIT_CODE2:
	case IDC_EXIT_CODE3:
	case IDC_EXIT_CODE4:
		pDC->SetBkMode(TRANSPARENT);
		return m_brushBody;
		break;
	}

	return hbr;
}


void CConfirmExitDlg::OnBnClickedExitCode()
{
	UpdateData(TRUE);

	if (m_nExitCode == 3)
	{
		GetDlgItem(IDC_EDIT_ETC)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_ETC)->EnableWindow(FALSE);
	}

}


BOOL CConfirmExitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_ETC)->EnableWindow(FALSE);

	DWORD dwBtnColor = RGB(175, 37, 40);
	m_btnOK.SetFontHeight(12);
	m_btnOK.SetButtonColor( dwBtnColor );
	m_btnOK.SetBold(FALSE);
	m_btnCancel.SetFontHeight(12);
	m_btnCancel.SetButtonColor( dwBtnColor );
	m_btnCancel.SetBold(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CConfirmExitDlg::OnBnClickedOk()
{
	WRITE_LOG( WL_BTN, _T("CConfirmExitDlg::OnBnClickedOk") );
	UpdateData(TRUE);

	switch (m_nExitCode)
	{
	case 0:
		WRITE_LOG( WL_SYSTEM, _T("EXIT CODE: 0 - Maintenance") );
		break;
	case 1:
		WRITE_LOG( WL_SYSTEM, _T("EXIT CODE: 1 - Machine Error") );
		break;
	case 2:
		WRITE_LOG( WL_SYSTEM, _T("EXIT CODE: 2 - Vision Error") );
		break;
	case 3:
		WRITE_LOG( WL_SYSTEM, _T("EXIT CODE: 3 - Etc[%s]"), m_strEtc );
		break;
	}

	CDialog::OnOK();
}


LRESULT CConfirmExitDlg::OnNcHitTest(CPoint point)
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
