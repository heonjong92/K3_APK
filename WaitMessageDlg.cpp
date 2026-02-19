// WaitMessageDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "WaitMessageDlg.h"
#include "afxdialogex.h"
#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CWaitMessageDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWaitMessageDlg, CDialog)

CWaitMessageDlg::CWaitMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitMessageDlg::IDD, pParent)
{
	m_clrBorder = RGB(29, 27, 126);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("정보");
	m_strMessage = _T("");

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_ERROR );
}

CWaitMessageDlg::~CWaitMessageDlg()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CWaitMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWaitMessageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()
 //// 
void CWaitMessageDlg::SetTitle( LPCTSTR lpszTitle )
{
	m_strTitle = lpszTitle;
	if (GetSafeHwnd())
		Invalidate();
}
 //// 
void CWaitMessageDlg::SetMessage( LPCTSTR lpszMessage )
{
	m_strMessage = lpszMessage;
	if (GetSafeHwnd())
		Invalidate();
}
 //// 
void CWaitMessageDlg::OnPaint()
{
	CPaintDC dc(this);

	using namespace Gdiplus;

	CRect rcClient;
	GetClientRect( rcClient );

	int nTopOffset = 36;
	int nBorderWidth = m_nBorderWidth;

	CRect rcTitle = rcClient;
	rcTitle.bottom = rcTitle.top + nTopOffset;

	dc.FillSolidRect( rcClient, m_clrBody );
	//dc.FillSolidRect( rcClient.left, rcClient.top, rcClient.Width(), rcSplit2.top-rcClient.top, m_clrBody );

	dc.FillSolidRect( rcTitle, m_clrBorder );

	dc.FillSolidRect( rcClient.left, rcClient.bottom-nBorderWidth, rcClient.Width(), nBorderWidth, m_clrBorder );
	dc.FillSolidRect( rcClient.left, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );
	dc.FillSolidRect( rcClient.right-nBorderWidth, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );

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
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetAlignment( StringAlignmentNear );
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();

	SolidBrush brushTitle(Color(200,255,255,255));
	Gdiplus::Font fontTitle(pFontFamily, 15, FontStyleBold, UnitPixel);
	BSTR bstrTitle = m_strTitle.AllocSysString();
	g.DrawString( bstrTitle, -1, &fontTitle, RectF((float)rcTitle.left+nIX+nIW+3, (float)rcTitle.top, (float)rcTitle.Width()-8, (float)rcTitle.Height()), &stringFormat, &brushTitle );
	SysFreeString( bstrTitle );

	stringFormat.SetAlignment( StringAlignmentCenter );
	SolidBrush brushMessage( Color(220,236,0,21) );
	Gdiplus::Font fontMessage(pFontFamily, 30, FontStyleBold, UnitPixel);
	BSTR bstrMessage = m_strMessage.AllocSysString();
	RectF rectMessageF;
	rectMessageF.X = (float)rcClient.left + 30;
	rectMessageF.Y = (float)rcClient.top+nTopOffset;
	rectMessageF.Width = (float)rcClient.Width() - 60;
	rectMessageF.Height = (float)rcClient.Height() - nTopOffset;

	g.DrawString( bstrMessage, -1, &fontMessage, 
		rectMessageF, 
		&stringFormat, &brushMessage );

	SysFreeString( bstrMessage );
}
 //// 
BOOL CWaitMessageDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}
 //// 
BOOL CWaitMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}
 //// 
void CWaitMessageDlg::OnOK()
{
	ShowWindow(SW_HIDE);
}
 //// 
void CWaitMessageDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}
 //// 
void CWaitMessageDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		CenterWindow(GetParent());
		AfxGetMainWnd()->BeginModalState();
		EnableWindow(TRUE);
	}
	else
	{
		AfxGetMainWnd()->EndModalState();
	}

}
 //// 
LRESULT CWaitMessageDlg::OnNcHitTest(CPoint point)
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
