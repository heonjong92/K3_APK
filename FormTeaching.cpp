// FormTeaching.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "APK.h"
#include "FormTeaching.h"
#include "APKView.h"
#include "AddNewRecipeDlg.h"

#include "LightInfo.h"
#include "JobNumberData.h"
#include "VisionSystem.h"
#include "InspectionVision.h"
#include "LanguageInfo.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>

// CFormTeaching
IMPLEMENT_DYNCREATE(CFormTeaching, CFormView)

CFormTeaching::CFormTeaching()
	: CFormView(CFormTeaching::IDD)
	, m_pMainView(NULL)
{
}

CFormTeaching::~CFormTeaching()
{
}

void CFormTeaching::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEACH_TAB, m_wndTabTeaching);
}


BEGIN_MESSAGE_MAP(CFormTeaching, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_REGISTERED_MESSAGE(UIExt::WM_FLAT_TABWND_SELCHANGED, OnFlatTabWndSelChanged)
	ON_MESSAGE(WM_REFRESH_DIALOG, OnRefreshDialog)
END_MESSAGE_MAP()


// CFormTeaching 진단입니다.

#ifdef _DEBUG
void CFormTeaching::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormTeaching::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CFormTeaching 메시지 처리기입니다.
void CFormTeaching::SetMainView( CAPKView* pView )
{ 
	m_pMainView = pView;
	m_wndTab1G_TrayOcr.SetMainView(pView);
	m_wndTab1G_3DChipCnt.SetMainView(pView);
	m_wndTab1G_ChipOcr.SetMainView(pView);
	m_wndTab1G_Chip.SetMainView(pView);
	m_wndTab1G_Mixing.SetMainView(pView);
	m_wndTab1G_Lift.SetMainView(pView);
	m_wndTabSG_StackerOcr.SetMainView(pView);
	m_wndTab2G_Banding.SetMainView(pView);
	m_wndTab2G_HIC.SetMainView(pView);
	m_wndTab3G_Desiccant.SetMainView(pView);
	m_wndTab3G_Material.SetMainView(pView);
	m_wndTab4G_MBB.SetMainView(pView);
	m_wndTab5G_BoxQuality.SetMainView(pView);
	m_wndTab5G_SealingQuality.SetMainView(pView);
	m_wndTab6G_BoxTape.SetMainView(pView);
	m_wndTabLabel.SetMainView(pView);
}

//////////////////////////////////////////////////////////
int CFormTeaching::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	return 0;
}

void CFormTeaching::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;
}

void CFormTeaching::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		m_pMainView->ResetGraphic();

		int nTabIndex = (int)m_wndTabTeaching.GetActiveTab();

		switch (nTabIndex)
		{
			m_pMainView->ChangeViewMode(ViewModeAll);

#ifdef RELEASE_1G
		case TEACH_TAB_IDX_TRAYOCR:				m_pMainView->ChangeViewMode(ViewModeTrayOcr);				m_wndTab1G_TrayOcr.Refresh();			break;
		case TEACH_TAB_IDX_3DCHIPCNT:			m_pMainView->ChangeViewMode(ViewMode3DChipCnt);				m_wndTab1G_3DChipCnt.Refresh();			break;
		case TEACH_TAB_IDX_CHIPOCR:				m_pMainView->ChangeViewMode(ViewModeChipOcr);				m_wndTab1G_ChipOcr.Refresh();			break;
		case TEACH_TAB_IDX_CHIP:				m_pMainView->ChangeViewMode(ViewModeChip);					m_wndTab1G_Chip.Refresh();				break;
		case TEACH_TAB_IDX_MIXING:				m_pMainView->ChangeViewMode(ViewModeMixing);				m_wndTab1G_Mixing.Refresh();			break;
		case TEACH_TAB_IDX_LIFTINFO:			m_pMainView->ChangeViewMode(ViewModeLift);					m_wndTab1G_Lift.Refresh();				break;
		
#elif RELEASE_SG
		case TEACH_TAB_IDX_STACKER_OCR:			m_pMainView->ChangeViewMode(ViewModeAll);					m_wndTabSG_StackerOcr.Refresh();		break;

#elif RELEASE_2G
		case TEACH_TAB_IDX_BANDING:				m_pMainView->ChangeViewMode(ViewModeBanding);				m_wndTab2G_Banding.Refresh();			break;
		case TEACH_TAB_IDX_HIC:					m_pMainView->ChangeViewMode(ViewModeHIC);					m_wndTab2G_HIC.Refresh();				break;

#elif RELEASE_3G
		case TEACH_TAB_IDX_DESICCANT_CUTTING:	m_pMainView->ChangeViewMode(ViewModeDesiccantCutting);		m_wndTab3G_Desiccant.Refresh();			break;
		case TEACH_TAB_IDX_DESICCANT_MATERIAL:	m_pMainView->ChangeViewMode(ViewModeDesiccantMaterial);		m_wndTab3G_Material.Refresh();			break;

#elif RELEASE_4G
		case TEACH_TAB_IDX_MBB:					m_pMainView->ChangeViewMode(ViewModeMBB);					m_wndTab4G_MBB.Refresh();				break;
		case TEACH_TAB_IDX_LABEL:				m_pMainView->ChangeViewMode(ViewModeLabel);					m_wndTabLabel.Refresh();				break;

#elif RELEASE_5G
		case TEACH_TAB_IDX_BOX:					m_pMainView->ChangeViewMode(ViewModeBox);					m_wndTab5G_BoxQuality.Refresh();		break;
		case TEACH_TAB_IDX_SEALING:				m_pMainView->ChangeViewMode(ViewModeSealing);				m_wndTab5G_SealingQuality.Refresh();	break;

#elif RELEASE_6G
		case TEACH_TAB_IDX_LABEL:				m_pMainView->ChangeViewMode(ViewModeLabel);					m_wndTabLabel.Refresh();				break;
		case TEACH_TAB_IDX_TAPE:				m_pMainView->ChangeViewMode(ViewModeBoxTape);				m_wndTab6G_BoxTape.Refresh();			break;

#endif
		}
	}
	else
	{
		int nTabIndex = (int)m_wndTabTeaching.GetActiveTab();
		switch (nTabIndex)
		{
		case 0:
//			m_wndTab1G_Chip.Cleanup();
			break;
		}

		m_pMainView->ChangeViewMode(ViewModeAll);
	}
}

LRESULT CFormTeaching::OnFlatTabWndSelChanged(WPARAM wParam, LPARAM lParam)
{
	int nTabIndex = LOWORD(wParam);

	switch (nTabIndex)
	{
		m_pMainView->ChangeViewMode(ViewModeAll);
#ifdef RELEASE_1G
	case TEACH_TAB_IDX_TRAYOCR:					m_pMainView->ChangeViewMode(ViewModeTrayOcr);				m_wndTab1G_TrayOcr.Refresh();			break;
	case TEACH_TAB_IDX_3DCHIPCNT:				m_pMainView->ChangeViewMode(ViewMode3DChipCnt);				m_wndTab1G_3DChipCnt.Refresh();			break;
	case TEACH_TAB_IDX_CHIPOCR:					m_pMainView->ChangeViewMode(ViewModeChipOcr);				m_wndTab1G_ChipOcr.Refresh();			break;
	case TEACH_TAB_IDX_CHIP:					m_pMainView->ChangeViewMode(ViewModeChip);					m_wndTab1G_Chip.Refresh();				break;
	case TEACH_TAB_IDX_MIXING:					m_pMainView->ChangeViewMode(ViewModeMixing);				m_wndTab1G_Mixing.Refresh();			break;
	case TEACH_TAB_IDX_LIFTINFO:				m_pMainView->ChangeViewMode(ViewModeLift);					m_wndTab1G_Lift.Refresh();				break;

#elif RELEASE_SG
	case TEACH_TAB_IDX_STACKER_OCR:				m_pMainView->ChangeViewMode(ViewModeAll);					m_wndTabSG_StackerOcr.Refresh();		break;

#elif RELEASE_2G
	case TEACH_TAB_IDX_BANDING:					m_pMainView->ChangeViewMode(ViewModeBanding);				m_wndTab2G_Banding.Refresh();			break;
	case TEACH_TAB_IDX_HIC:						m_pMainView->ChangeViewMode(ViewModeHIC);					m_wndTab2G_HIC.Refresh();				break;

#elif RELEASE_3G
	case TEACH_TAB_IDX_DESICCANT_CUTTING:		m_pMainView->ChangeViewMode(ViewModeDesiccantCutting);		m_wndTab3G_Desiccant.Refresh();			break;
	case TEACH_TAB_IDX_DESICCANT_MATERIAL:		m_pMainView->ChangeViewMode(ViewModeDesiccantMaterial);		m_wndTab3G_Material.Refresh();			break;

#elif RELEASE_4G
	case TEACH_TAB_IDX_MBB:						m_pMainView->ChangeViewMode(ViewModeMBB);					m_wndTab4G_MBB.Refresh();				break;
	case TEACH_TAB_IDX_LABEL:					m_pMainView->ChangeViewMode(ViewModeLabel);					m_wndTabLabel.Refresh();				break;

#elif RELEASE_5G
	case TEACH_TAB_IDX_BOX:						m_pMainView->ChangeViewMode(ViewModeBox);					m_wndTab5G_BoxQuality.Refresh();		break;
	case TEACH_TAB_IDX_SEALING:					m_pMainView->ChangeViewMode(ViewModeSealing);				m_wndTab5G_SealingQuality.Refresh();	break;

#elif RELEASE_6G
	case TEACH_TAB_IDX_LABEL:					m_pMainView->ChangeViewMode(ViewModeLabel);					m_wndTabLabel.Refresh();				break;
	case TEACH_TAB_IDX_TAPE:					m_pMainView->ChangeViewMode(ViewModeBoxTape);				m_wndTab6G_BoxTape.Refresh();			break;

#endif
	}

	return 0;
}

BOOL CFormTeaching::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetSideBarBodyColor();
	pDC->FillSolidRect( rc, dwBodyColor );
	return TRUE;
}

HBRUSH CFormTeaching::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CFormTeaching::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	UpdateData(FALSE);

	m_wndTabTeaching.SetTabTextHeight( 12 );
	m_wndTabTeaching.SetTabDirection(UIExt::CFlatTabWnd::TabDirectionHorzLeftTop, 36, 0);
#ifdef RELEASE_1G
	m_wndTabTeaching.SetTabWidth( 70 );
#else
	m_wndTabTeaching.SetTabWidth( 70 );
#endif
	m_wndTabTeaching.SetBackgroundColor(RGB(240,240,240));

#ifdef RELEASE_1G
	if (!m_wndTab1G_TrayOcr.Create(CTeachTab1GTrayOcr::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab1G_3DChipCnt.Create(CTeachTab1G3DChipCnt::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab1G_ChipOcr.Create(CTeachTab1GChipOcr::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab1G_Chip.Create(CTeachTab1GChip::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab1G_Mixing.Create(CTeachTab1GMixing::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab1G_Lift.Create(CTeachTab1GLift::IDD, &m_wndTabTeaching))
		return;

	m_wndTabTeaching.AddTab(_T("TrayOcr"), &m_wndTab1G_TrayOcr);
	m_wndTabTeaching.AddTab(_T("3D ChipCnt"), &m_wndTab1G_3DChipCnt);
	m_wndTabTeaching.AddTab(_T("ChipOcr"), &m_wndTab1G_ChipOcr);
	m_wndTabTeaching.AddTab(_T("Chip"), &m_wndTab1G_Chip);
	m_wndTabTeaching.AddTab(_T("Mixing"), &m_wndTab1G_Mixing);
	m_wndTabTeaching.AddTab(_T("Lift"), &m_wndTab1G_Lift);
	
#elif RELEASE_SG
	if (!m_wndTabSG_StackerOcr.Create(CTeachTabSGStackerOcr::IDD, &m_wndTabTeaching))
		return;
		
	m_wndTabTeaching.AddTab(_T("Stacker Ocr"), &m_wndTabSG_StackerOcr);

#elif RELEASE_2G
	if (!m_wndTab2G_Banding.Create(CTeachTab2GBanding::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab2G_HIC.Create(CTeachTab2GHIC::IDD, &m_wndTabTeaching))
		return;

	m_wndTabTeaching.AddTab(_T("Banding"), &m_wndTab2G_Banding);
	m_wndTabTeaching.AddTab(_T("HIC"	), &m_wndTab2G_HIC);

#elif RELEASE_3G
	if (!m_wndTab3G_Desiccant.Create(CTeachTab3GDesiccantCutting::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab3G_Material.Create(CTeachTab3GDesiccantMaterial::IDD, &m_wndTabTeaching))
		return;

	m_wndTabTeaching.AddTab(_T("Cutting"	), &m_wndTab3G_Desiccant);
	m_wndTabTeaching.AddTab( _T("Material"	), &m_wndTab3G_Material);

#elif RELEASE_4G
	if (!m_wndTab4G_MBB.Create(CTeachTab4GMBB::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTabLabel.Create(CTeachTabLabel::IDD, &m_wndTabTeaching))
		return;

	m_wndTabTeaching.AddTab( _T("MBB"	), &m_wndTab4G_MBB );
	m_wndTabTeaching.AddTab( _T("Label"	), &m_wndTabLabel );

#elif RELEASE_5G
	if (!m_wndTab5G_BoxQuality.Create(CTeachTab5GBoxQuality::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab5G_SealingQuality.Create(CTeachTab5GSealingQuality::IDD, &m_wndTabTeaching))
		return;

	m_wndTabTeaching.AddTab( _T("Box"), &m_wndTab5G_BoxQuality );
	m_wndTabTeaching.AddTab( _T("Sealing"), &m_wndTab5G_SealingQuality );

#elif RELEASE_6G
	if (!m_wndTabLabel.Create(CTeachTabLabel::IDD, &m_wndTabTeaching))
		return;
	if (!m_wndTab6G_BoxTape.Create(CTeachTab6GBoxTape::IDD, &m_wndTabTeaching))
		return;

	m_wndTabTeaching.AddTab( _T("Label"	), &m_wndTabLabel );
	m_wndTabTeaching.AddTab( _T("Tape"	), &m_wndTab6G_BoxTape );
#endif

	CWnd* pTeachTabWnd = GetDlgItem(IDC_TEACH_TAB);
	if (pTeachTabWnd && *pTeachTabWnd)
	{
		CRect rcWnd;
		pTeachTabWnd->GetWindowRect(rcWnd);
		ScreenToClient( rcWnd );

		if ( m_wndTabTeaching )
		{
			pTeachTabWnd->MoveWindow( rcWnd );
			pTeachTabWnd->RedrawWindow();
		}
	}

	m_wndTabTeaching.SelectTab( 0 );
	m_pMainView->ChangeViewMode(ViewModeAll);

	CRect rc;
	GetClientRect(rc);
	int cx = rc.Width();
	int cy = rc.Height();

	UpdateLanguage();
}

void CFormTeaching::UpdateLanguage()
{
#ifdef RELEASE_1G
	m_wndTab1G_TrayOcr.UpdateLanguage();
	m_wndTab1G_3DChipCnt.UpdateLanguage();
	m_wndTab1G_ChipOcr.UpdateLanguage();
	m_wndTab1G_Chip.UpdateLanguage();
	m_wndTab1G_Mixing.UpdateLanguage();
	m_wndTab1G_Lift.UpdateLanguage();

#elif RELEASE_SG
	m_wndTabSG_StackerOcr.UpdateLanguage();
	
#elif RELEASE_2G
	m_wndTab2G_Banding.UpdateLanguage();
	m_wndTab2G_HIC.UpdateLanguage();

#elif RELEASE_3G
	m_wndTab3G_Desiccant.UpdateLanguage();
	m_wndTab3G_Material.UpdateLanguage();

#elif RELEASE_4G
	m_wndTab4G_MBB.UpdateLanguage();
	m_wndTabLabel.UpdateLanguage();

#elif RELEASE_5G
	m_wndTab5G_BoxQuality.UpdateLanguage();
	m_wndTab5G_SealingQuality.UpdateLanguage();

#elif RELEASE_6G
	m_wndTab6G_BoxTape.UpdateLanguage();
	m_wndTabLabel.UpdateLanguage();

#endif
}


void CFormTeaching::UpdateUI( BOOL bAll/*=FALSE*/ )
{
	if( bAll )
	{
#ifdef RELEASE_3G
		m_wndTab3G_Material.UpdateUI();
#elif RELEASE_4G || RELEASE_6G
		m_wndTabLabel.UpdateUI();
#endif
	}
	else
	{
		int nTabIndex = (int)m_wndTabTeaching.GetActiveTab();
		switch (nTabIndex)		// Tab별로 저장되게 되어 있음
		{
#ifdef RELEASE_1G
		case TEACH_TAB_IDX_TRAYOCR:						m_wndTab1G_TrayOcr.UpdateUI();				break;
		case TEACH_TAB_IDX_3DCHIPCNT:																break;
		case TEACH_TAB_IDX_CHIPOCR:						m_wndTab1G_ChipOcr.UpdateUI();				break;
		case TEACH_TAB_IDX_CHIP: 																	break;
		case TEACH_TAB_IDX_MIXING:						m_wndTab1G_Mixing.UpdateUI();				break;
		case TEACH_TAB_IDX_LIFTINFO:																break;

#elif RELEASE_SG
		case TEACH_TAB_IDX_STACKER_OCR:					m_wndTabSG_StackerOcr.UpdateUI();			break;

#elif RELEASE_2G
		case TEACH_TAB_IDX_BANDING:																	break;
		case TEACH_TAB_IDX_HIC:																		break;

#elif RELEASE_3G
		case TEACH_TAB_IDX_DESICCANT_CUTTING:														break;
		case TEACH_TAB_IDX_DESICCANT_MATERIAL:			m_wndTab3G_Material.UpdateUI();				break;

#elif RELEASE_4G
		case TEACH_TAB_IDX_MBB:																		break;
		case TEACH_TAB_IDX_LABEL:						m_wndTabLabel.UpdateUI();					break;

#elif RELEASE_5G
		case TEACH_TAB_IDX_BOX: 						m_wndTab5G_BoxQuality.UpdateUI();			break;
		case TEACH_TAB_IDX_SEALING: 					m_wndTab5G_SealingQuality.UpdateUI();		break;

#elif RELEASE_6G
		case TEACH_TAB_IDX_LABEL:						m_wndTabLabel.UpdateUI();					break;
		case TEACH_TAB_IDX_TAPE:																	break;

#endif
		default:
			break;
		}
	}
}

LRESULT CFormTeaching::OnRefreshDialog(WPARAM wparam, LPARAM lparam)
{
#ifdef RELEASE_1G
	m_wndTab1G_3DChipCnt.EnableCutShift();
	m_wndTab1G_Lift.EnableLift();
	m_wndTab1G_Chip.EnableChip();

#elif RELEASE_2G
	m_wndTab2G_HIC.EnableHICLight();

#elif RELEASE_3G
	m_wndTab3G_Material.UpdateUI();

#elif RELEASE_4G
	m_wndTab4G_MBB.EnableMBB();

#elif RELEASE_5G
#elif RELEASE_6G
#endif
	return 0;
}
