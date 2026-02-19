// AccessRightsDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "AccessRightsDlg.h"
#include "afxdialogex.h"
#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"

#include "SystemConfig.h"
#include "ChangePasswordDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSystemConfig::INotifyChangeAccessRight::INotifyChangeAccessRight()
{
	CSystemConfig::Instance()->AddChangeAccessRightListener(this);
}

CSystemConfig::INotifyChangeAccessRight::~INotifyChangeAccessRight()
{
	CSystemConfig::Instance()->RemoveChangeAccessRightListener(this);
}

eAccessRight GetAccessRight( int nType )
{
	switch (nType)
	{
	case 0:
		return AccessRightOperator;
	case 1:
		return AccessRightEngineer;
	case 2:
		return AccessRightProgrammer;
	}
	return AccessRightOperator;
}

int GetAccessType( eAccessRight access )
{
	switch (access)
	{
	case AccessRightOperator:
		return 0;
	case AccessRightEngineer:
		return 1;
	case AccessRightProgrammer:
		return 2;
	}
	return 3;
}

IMPLEMENT_DYNAMIC(CAccessRightsDlg, CDialog)

CAccessRightsDlg::CAccessRightsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAccessRightsDlg::IDD, pParent)
	, m_nAccessRightType(0)
	, m_strPassword(_T(""))
{
	m_clrBorder = RGB(126, 27, 29);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("Access Rights");

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_ACCESS_RIGHT );
}

CAccessRightsDlg::~CAccessRightsDlg()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CAccessRightsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_ACCESSRIGHTS1, m_nAccessRightType);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_strPassword);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CAccessRightsDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CHANGE_PASSWD, &CAccessRightsDlg::OnBnClickedBtnChangePasswd)
	ON_BN_CLICKED(IDOK, &CAccessRightsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_ACCESSRIGHTS1, &CAccessRightsDlg::OnBnClickedRadioAccessRights)
	ON_BN_CLICKED(IDC_RADIO_ACCESSRIGHTS2, &CAccessRightsDlg::OnBnClickedRadioAccessRights)
	ON_BN_CLICKED(IDC_RADIO_ACCESSRIGHTS3, &CAccessRightsDlg::OnBnClickedRadioAccessRights)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


void CAccessRightsDlg::OnPaint()
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

BOOL CAccessRightsDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}


HBRUSH CAccessRightsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_RADIO_ACCESSRIGHTS1:
	case IDC_RADIO_ACCESSRIGHTS2:
	case IDC_RADIO_ACCESSRIGHTS3:
		pDC->SetBkMode(TRANSPARENT);
		return m_brushBody;
		break;
	}

	return hbr;
}

BOOL CAccessRightsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	eAccessRight access = CSystemConfig::Instance()->GetAccessRight();
	m_nAccessRightType = GetAccessType(access);
	UpdateData(FALSE);

	DWORD dwBtnColor = RGB(175, 37, 40);
	m_btnOK.SetFontHeight(12);
	m_btnOK.SetButtonColor( dwBtnColor );
	m_btnOK.SetBold(FALSE);
	m_btnCancel.SetFontHeight(12);
	m_btnCancel.SetButtonColor( dwBtnColor );
	m_btnCancel.SetBold(FALSE);

	switch (m_nAccessRightType)
	{
	case 0:
		GetDlgItem(IDC_EDIT_PASSWD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CHANGE_PASSWD)->EnableWindow(FALSE);
		break;
	case 1:
		GetDlgItem(IDC_EDIT_PASSWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CHANGE_PASSWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PASSWD)->SetWindowText(CSystemConfig::Instance()->GetPassword(access));
		break;
	case 2:
		GetDlgItem(IDC_EDIT_PASSWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CHANGE_PASSWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PASSWD)->SetWindowText(CSystemConfig::Instance()->GetPassword(access));
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAccessRightsDlg::OnBnClickedOk()
{
	WRITE_LOG( WL_BTN, _T("CAccessRightsDlg::OnBnClickedOk") );

	UpdateData(TRUE);
	
	eAccessRight access = GetAccessRight(m_nAccessRightType);
	if (access == AccessRightOperator)
	{
		WRITE_LOG( WL_MSG, _T("Access Right -> Operator") );
		CSystemConfig::Instance()->SetAccessRight( access );
		CDialog::OnOK();
		return;
	}
	if (CSystemConfig::Instance()->IsValidPassword(access, m_strPassword))
	{
		switch (access)
		{
		case AccessRightEngineer:
			WRITE_LOG( WL_MSG, _T("Access Right -> Engineer") );
			break;
		case AccessRightProgrammer:
			WRITE_LOG( WL_MSG, _T("Access Right -> Programmer") );
			break;
		}
		CSystemConfig::Instance()->SetAccessRight( access );
		CDialog::OnOK();
		return;
	}
	WRITE_LOG( WL_MSG, _T("비밀번호가 일치하지 않습니다.") );
	AfxMessageBox( _T("비밀번호가 일치하지 않습니다."), MB_OK|MB_ICONERROR );
}

void CAccessRightsDlg::OnBnClickedBtnChangePasswd()
{
	WRITE_LOG( WL_BTN, _T("CAccessRightsDlg::OnBnClickedBtnChangePasswd") );

	UpdateData(TRUE);
	eAccessRight access = GetAccessRight(m_nAccessRightType);
	if (CSystemConfig::Instance()->IsValidPassword(access, m_strPassword))
	{
		CChangePasswordDlg ChangePasswdDlg(this);
		if (ChangePasswdDlg.DoModal() == IDOK)
		{
			switch (access)
			{
			case AccessRightEngineer:
				WRITE_LOG( WL_MSG, _T("Engineer Password has been changed.") );
				break;
			case AccessRightProgrammer:
				WRITE_LOG( WL_MSG, _T("Programmer Password has been changed.") );
				break;
			}
			m_strPassword = ChangePasswdDlg.m_strNewPassword;
			UpdateData(FALSE);
			CSystemConfig::Instance()->ChangePassword(access, m_strPassword);
		}
	}
	else
	{
		WRITE_LOG( WL_MSG, _T("비밀번호가 일치하지 않습니다.") );
		AfxMessageBox( _T("비밀번호가 일치하지 않습니다."), MB_OK|MB_ICONERROR );
	}
}

void CAccessRightsDlg::OnBnClickedRadioAccessRights()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_PASSWD)->SetWindowText(_T(""));

	eAccessRight access = CSystemConfig::Instance()->GetAccessRight();

	if (access != AccessRightOperator)
	{
		if (access == GetAccessRight(m_nAccessRightType))
		{
			GetDlgItem(IDC_EDIT_PASSWD)->SetWindowText(CSystemConfig::Instance()->GetPassword(access));
		}
	}

	switch (m_nAccessRightType)
	{
	case 0:
		GetDlgItem(IDC_EDIT_PASSWD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CHANGE_PASSWD)->EnableWindow(FALSE);
		break;
	case 1:
		GetDlgItem(IDC_EDIT_PASSWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CHANGE_PASSWD)->EnableWindow(TRUE);
		break;
	case 2:
		GetDlgItem(IDC_EDIT_PASSWD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CHANGE_PASSWD)->EnableWindow(TRUE);
		break;
	}
}


LRESULT CAccessRightsDlg::OnNcHitTest(CPoint point)
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
