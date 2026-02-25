// FormDevice.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "APK.h"
#include "FormDevice.h"
#include "APKView.h"
#include "LanguageInfo.h"

#include "UIExt/ResourceManager.h"

// CFormDevice
IMPLEMENT_DYNCREATE(CFormDevice, CFormView)

CFormDevice::CFormDevice()
	: CFormView(CFormDevice::IDD)
	, m_pMainView(NULL)
{

}

CFormDevice::~CFormDevice()
{
}

void CFormDevice::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEV_TAB, m_wndTabDevSetting);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnSave);
}

BEGIN_MESSAGE_MAP(CFormDevice, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormDevice::OnBnClickedBtnSave)
	ON_REGISTERED_MESSAGE(UIExt::WM_FLAT_TABWND_SELCHANGED, OnFlatTabWndSelChanged)
	ON_MESSAGE(WM_REFRESH_DIALOG, OnRefreshDialog)
	ON_MESSAGE(WM_REFRESH_SYSTEM, OnRefreshSystem)
END_MESSAGE_MAP()


// CFormTeaching 진단입니다.

#ifdef _DEBUG
void CFormDevice::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormDevice::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CFormDevice 메시지 처리기입니다.
void CFormDevice::SetMainView( CAPKView* pView )
{ 
	m_pMainView = pView;
	m_wndTabSystem.SetMainView(pView);
	m_wndTabCameraLight.SetMainView(pView);
}

//////////////////////////////////////////////////////////
int CFormDevice::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	return 0;
}

void CFormDevice::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;
}

void CFormDevice::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		int nTabIndex = (int)m_wndTabDevSetting.GetActiveTab();

		switch (nTabIndex)
		{
		case 0:
			m_wndTabSystem.Refresh();
			break;
		case 1:
			m_wndTabCameraLight.Refresh();
			break;
		}	
	}
	else
	{
		int	 nTabIndex = (int)m_wndTabDevSetting.GetActiveTab();

		switch (nTabIndex)
		{
		case 0:
			break;
		case 1:
			m_wndTabCameraLight.Cleanup();
			break;
		}
	}
}

LRESULT CFormDevice::OnFlatTabWndSelChanged(WPARAM wParam, LPARAM lParam)
{
	int nTabIndex = LOWORD(wParam);

	switch (nTabIndex)
	{
	case 0:
		m_wndTabSystem.Refresh();
		break;
	case 1:
		m_wndTabCameraLight.Refresh();
		break;
	}

	return 0;
}

LRESULT CFormDevice::OnRefreshDialog(WPARAM wparam, LPARAM lparam)
{
	m_wndTabCameraLight.Refresh();

	return 0;
}

LRESULT CFormDevice::OnRefreshSystem(WPARAM wparam, LPARAM lparam)
{
	m_wndTabSystem.Refresh();

	return 0;
}

BOOL CFormDevice::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetSideBarBodyColor();
	pDC->FillSolidRect( rc, dwBodyColor );
	return TRUE;
}

HBRUSH CFormDevice::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		break;
	}

	return hbr;
}

void CFormDevice::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	UpdateData(FALSE);

	m_wndTabDevSetting.SetTabTextHeight( 12 );
	m_wndTabDevSetting.SetTabDirection(UIExt::CFlatTabWnd::TabDirectionHorzLeftTop, 36, 0);
	m_wndTabDevSetting.SetTabWidth( 100 );
	m_wndTabDevSetting.SetBackgroundColor(RGB(240,240,240));

	if (!m_wndTabSystem.Create(CDevTabSystem::IDD, &m_wndTabDevSetting))
		return;
	if (!m_wndTabCameraLight.Create(CDevTabCameraLight::IDD, &m_wndTabDevSetting))
		return;

	m_wndTabDevSetting.AddTab( _T("System"),			&m_wndTabSystem );
	m_wndTabDevSetting.AddTab( _T("Camera / Light"),	&m_wndTabCameraLight );

	CWnd* pDevTabWnd = GetDlgItem(IDC_DEV_TAB);
	if (pDevTabWnd && *pDevTabWnd)
	{
		CRect rcWnd;
		pDevTabWnd->GetWindowRect(rcWnd);
		ScreenToClient( rcWnd );

		if ( m_wndTabDevSetting )
		{
			pDevTabWnd->MoveWindow( rcWnd );
			pDevTabWnd->RedrawWindow();
		}
	}

	m_wndTabDevSetting.SelectTab( 0 );

	DWORD dwNormalBtnColor = RGB(97, 135, 169);
	m_btnSave.SetColor(dwNormalBtnColor, dwNormalBtnColor, RGB(80, 106, 126), RGB(180, 180, 180), RGB(255, 255, 255));
	m_btnSave.SetFontHeight( 14 );
	m_btnSave.SetBold(TRUE);

	UpdateLanguage();
}

void CFormDevice::UpdateLanguage()
{
	m_wndTabSystem.UpdateLanguage();
	// 	m_btnSave.SetWindowText(CLanguageInfo::Instance()->ReadString(36));
	// #ifdef RELEASE_GD
	// //	m_wndTabCameraArea.UpdateLanguage();
	// #endif
	// 	m_wndTabSystem.UpdateLanguage();
}

void CFormDevice::OnBnClickedBtnSave()
{
	WRITE_LOG( WL_BTN, _T("CFormDevice::OnBnClickedBtnSave :: Start") );

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장을 하시겠습니까?");

	if ( IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;	

	m_pMainView->ShowWaitMessage(TRUE, _T("Recipe Save"), _T("Recipe Saving..."));
	int nTabIndex = (int)m_wndTabDevSetting.GetActiveTab();

	switch (nTabIndex)		// Tab별로 저장되게 되어 있음
	{
	case 0:
		m_wndTabSystem.Save();
		break;
	case 1:
		m_wndTabCameraLight.Save();
		break;
	}

	m_pMainView->ShowWaitMessage(FALSE);

	WRITE_LOG( WL_MSG, _T("CFormDevice::OnBnClickedBtnSave :: End") );
}