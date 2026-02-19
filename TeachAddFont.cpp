

#include "stdafx.h"
#include "APK.h"
#include "TeachAddFont.h"
#include "afxdialogex.h"

#include "VisionSystem.h"
#include "TeachTabLabel.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>

IMPLEMENT_DYNAMIC(TeachAddFont, CDialogEx)

TeachAddFont::TeachAddFont(CWnd* pParent /*=NULL*/)
	: CDialogEx(TeachAddFont::IDD, pParent)
	, m_TeachingText(_T(""))
{
	bCheckOne = TRUE;

	m_clrBorder = RGB(126, 27, 29);
	m_clrBody	= RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("ADD FONT");

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_HITS_LOGO );

	m_SegmentNum.	SetColor(RGB(239, 239, 239), RGB(30, 30, 255));
	m_ReadingText.	SetColor(RGB(239, 239, 239), RGB(255, 30, 30));
	m_Teaching_Text.SetColor(RGB(239, 239, 239), RGB(0, 0, 0));

	m_Btn_Apply.SetFontHeight(12);
	m_Btn_Apply.SetButtonColor( RGB(175, 37, 40) );
	m_Btn_Apply.SetBold(FALSE);

	m_Btn_Cancell.SetFontHeight(12);
	m_Btn_Cancell.SetButtonColor( RGB(109, 107, 109) );
	m_Btn_Cancell.SetBold(FALSE);
}

TeachAddFont::~TeachAddFont()
{
}

void TeachAddFont::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,	IDC_LABEL_SEGMENTNUM,		m_SegmentNum);
	DDX_Control(pDX,	IDC_LABEL_READING_TEXT,		m_ReadingText);
	DDX_Control(pDX,	IDOK,						m_Btn_Apply);
	DDX_Control(pDX,	IDCANCEL,					m_Btn_Cancell);
	DDX_Control(pDX,	IDC_LABEL_TEACHING_TEXT,	m_Teaching_Text);
	DDX_Text(pDX,		IDC_EDIT_TEACHING_TEXT,		m_TeachingText);
}


BEGIN_MESSAGE_MAP(TeachAddFont, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK,			&TeachAddFont::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,		&TeachAddFont::OnBnClickedCancel)
END_MESSAGE_MAP()


void TeachAddFont::OnPaint()
{
	if(bCheckOne)
	{
		CString strBuff;
		strBuff.Format(_T("Segment Num : %d"), nSegmentIndex);
		GetDlgItem(IDC_LABEL_SEGMENTNUM		)->SetWindowText(strBuff);
		strBuff.Format(_T("Reading Text : %s"), strReadingText);
		GetDlgItem(IDC_LABEL_READING_TEXT	)->SetWindowText(strBuff);
		// strBuff.Format(_T("%s"), m_TeachingText);
		// GetDlgItem(IDC_EDIT_TEACHING_TEXT)->SetWindowText(strBuff);

		bCheckOne = !bCheckOne;
	}

	CPaintDC dc(this); // device context for painting
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

	dc.FillSolidRect( rcClient.left, rcSplit2.top-rcClient.top, rcClient.Width(), rcClient.bottom, m_clrBody );

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


void TeachAddFont::OnBnClickedOk()
{
	UpdateData(TRUE);
	if( 1 < m_TeachingText.GetLength() )
	{
		AfxMessageBox(_T("문자열 Teaching이 불가능 합니다.\n1개의 문자만 입력하세요!"));
		UpdateData(FALSE);
		return;
	}
	UpdateData(FALSE);

	CDialogEx::OnOK();
}

void TeachAddFont::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
