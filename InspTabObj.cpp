// InspTabObj.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "InspTabObj.h"
#include "afxdialogex.h"

#include "VisionSystem.h"
#include "APKView.h"
#include "ModelInfo.h"

// CInspTabObj 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspTabObj, CDialogEx)

CInspTabObj* CInspTabObj::m_pTabObject = NULL;

CInspTabObj::CInspTabObj(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInspTabObj::IDD, pParent)
{
	m_pTabObject = this;
	m_nObjectMapRow		= 0;
	m_nObjectMapCol		= 0;
	m_nObjectMapLineRow = 0;
	m_nObjectMapLineCol = 0;
}

CInspTabObj::~CInspTabObj()
{
}

void CInspTabObj::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInspTabObj, CDialogEx)
	ON_REGISTERED_MESSAGE(UIExt::WM_OBJECTINFO_UNIT_SELECTED_EX, OnLButtonDownOnObjectTool)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CInspTabObj 메시지 처리기입니다.
BOOL CInspTabObj::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ObjectInfoCtrl Init
	m_wndObjectInfoCtrl.Create(_T("Object "), WS_CHILD|WS_VISIBLE|SS_NOTIFY, CRect(0,0,0,0), this );

	m_wndObjectInfoCtrl.SetIdText( _T("CHIP COUNT MATRIX") );

	m_wndObjectInfoCtrl.EnableWindow(TRUE);

	UpdateUI();

	return TRUE;
}

void CInspTabObj::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}

void CInspTabObj::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(bShow)
		Refresh();
}

void CInspTabObj::Refresh()
{
	CModelInfo::stChip& m_stChip = CModelInfo::Instance()->GetChip();

	if( ( m_stChip.nMatrix_X		!= m_nObjectMapRow		|| m_stChip.nMatrix_Y		!= m_nObjectMapCol		) ||
		( m_stChip.nChipFovCnt_X	!= m_nObjectMapLineRow	|| m_stChip.nChipFovCnt_Y	!= m_nObjectMapLineCol	) )
		UpdateUI();
}

void CInspTabObj::UpdateUI()
{
	CModelInfo::stChip& m_stChip = CModelInfo::Instance()->GetChip();

	m_nObjectMapRow		= m_stChip.nMatrix_X;
	m_nObjectMapCol		= m_stChip.nMatrix_Y;
	m_nObjectMapLineRow = m_stChip.nChipFovCnt_X;
	m_nObjectMapLineCol = m_stChip.nChipFovCnt_Y;

	SetObjectInfoReset();									// Object (2) 리셋 초기화
	SetObjectInfoIndex( m_nObjectMapRow, m_nObjectMapCol );	// Object (1) 배열변경
	SetObjectInfoReset();									// Object (2) 리셋 초기화
	
	SetObjectInfoIndexOneshot(m_nObjectMapLineRow, m_nObjectMapLineCol);
}

BOOL CInspTabObj::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	
	int nOffsetXY = 3;
	m_wndObjectInfoCtrl.MoveWindow( nOffsetXY, nOffsetXY, rc.Width()-(nOffsetXY*2), rc.Height()-(nOffsetXY*2) );

	pDC->FillSolidRect(rc, RGB(255, 255, 255));

	return TRUE;
}

void CInspTabObj::ArrangeWindows( int cx, int cy )
{
	int iScreenWidth = cx ;
	int iScreenHeight = cy ;

	HDWP hDWP = BeginDeferWindowPos( 1 );
	CRect	rcViewArea( 5, 5, iScreenWidth, iScreenHeight );

	hDWP = DeferWindowPos( hDWP, m_wndObjectInfoCtrl.GetSafeHwnd(), NULL, 
		rcViewArea.left, rcViewArea.top , rcViewArea.Width(), rcViewArea.Height(), 0 );

	EndDeferWindowPos(hDWP);

}

void CInspTabObj::SetObjectInfoIndex( int nCol, int nRow )
{
	if( NULL != m_wndObjectInfoCtrl.GetSafeHwnd() )
	{
		m_wndObjectInfoCtrl.SetIndexData(nRow, nCol);
		m_wndObjectInfoCtrl.SetDirection(UIExt::CObjectInfoCtrlEx::Direction_R2L);
		m_wndObjectInfoCtrl.UpdateWindow();
	}
}

void CInspTabObj::SetObjectInfoIndexOneshot( int nCol, int nRow )
{
	if( NULL != m_wndObjectInfoCtrl.GetSafeHwnd() )
	{
		m_wndObjectInfoCtrl.SetIndexDataOneshot(nRow, nCol);
		m_wndObjectInfoCtrl.UpdateWindow();
	}
}

void CInspTabObj::SetObjectInfoResult( int nCol, int nRow, int nResult )
{
	UIExt::CObjectInfoCtrlEx::UnitTypeResult UnitResult = (UIExt::CObjectInfoCtrlEx::UnitTypeResult)nResult;

	if( NULL != m_wndObjectInfoCtrl.GetSafeHwnd() )
	{
		m_wndObjectInfoCtrl.SetUnitTypeResult(nCol,nRow,UnitResult);
	}
}

void CInspTabObj::SetObjectInfoReset()
{
	if( NULL != m_wndObjectInfoCtrl.GetSafeHwnd() )
		m_wndObjectInfoCtrl.Reset();
}

LRESULT CInspTabObj::OnLButtonDownOnObjectTool(WPARAM wParam, LPARAM lParam)
{
#ifdef RELEASE_1G
	int nRow = (int)LOWORD(wParam);
	int nCol = (int)HIWORD(wParam);

	int nGrabIndex = -1, nImgCol = 0, nImgRow = 0, OffsetX = 0, OffsetY = 0;
	CVisionSystem::Instance()->GetMetrixInfoGrab(nGrabIndex, nImgCol, nImgRow, OffsetX, OffsetY, nCol, nRow);

	CVisionSystem::Instance()->SetChipImage(nGrabIndex);

//	CVisionSystem::Instance()->BeginStopWatch(IDX_AREA4);
//	CVisionSystem::Instance()->StartInspection(InspectTypeChip, IDX_AREA4, nGrabIndex, TRUE);
#endif

	return 0;
}
