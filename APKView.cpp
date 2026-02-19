
// APKView.cpp : CAPKView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "APK.h"
#endif

#include "APKDoc.h"
#include "APKView.h"
#include "APKTypes.h"
#include "SplashWnd.h"

#include "UIExt/ResourceManager.h"

#include "FormInspection.h"
#include "FormTeaching.h"
#include "FormDevice.h"

#include "VisionSystem.h"
#include "SystemConfig.h"

#include "EditJobNumberDlg.h"
#include "AccessRightsDlg.h"
#include "ChangePasswordDlg.h"
#include "ConfirmExitDlg.h"

#include <XImageView\xImageViewCtrl.h>
#include <XGraphic/xGraphicObject.h>
#include "Util/xLogMsgCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CAPKView

IMPLEMENT_DYNCREATE(CAPKView, CView)

BEGIN_MESSAGE_MAP(CAPKView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()

	ON_COMMAND(ID_MENU_INSPECTION, OnMenuInspection)
	ON_COMMAND(ID_MENU_TEACHING, OnMenuTeaching)
	ON_COMMAND(ID_MENU_DEVICE_SETTINGS, OnMenuDeviceSettings)
	ON_COMMAND(ID_MENU_JOB_NUMBER, OnMenuJobNumber)
	ON_COMMAND(ID_MENU_ACCESS_RIGHTS, OnMenuAccessRights)
	ON_COMMAND(ID_MENU_EXIT, OnMenuExit)
	ON_COMMAND(ID_MENU_LANGUAGE, OnMenuLanguage)

	//////////////////////////////////////////////////////////////////////////
	ON_MESSAGE(WM_LOG_MESSAGE, OnLogMessage)
	ON_MESSAGE(WM_MODEL_CHANGE_STATUS, OnModelChangeStatus)

	ON_MESSAGE(WM_UPDATE_IMAGE_VIEW, OnUpdateImageView)
	ON_MESSAGE(WM_UPDATE_VIEWER_RESOLUTION, OnUpdateImageViewerResolution)
	ON_MESSAGE(WM_UPDATE_MAP_RESET, OnSetObjectInfoReset)
	ON_MESSAGE(WM_UPDATE_MAP_SETTING, OnSetObjectInfoIndex)
	
END_MESSAGE_MAP()

// CAPKView 생성/소멸
CAPKView* CAPKView::m_pThis = NULL;

CAPKView::CAPKView()
{
	m_pThis = this;

	m_pFormInspection	= NULL;
	m_pFormTeaching		= NULL;
	m_pFormDevice		= NULL;

	m_ViewMode = ViewModeAll;
}

CAPKView::~CAPKView()
{
}

BOOL CAPKView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_BORDER;

	return CView::PreCreateWindow(cs);
}

// CAPKView 그리기

void CAPKView::OnDraw(CDC* /*pDC*/)
{
	CAPKDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rc;
	GetClientRect(rc);
}

// CAPKView 진단

#ifdef _DEBUG
void CAPKView::AssertValid() const
{
	CView::AssertValid();
}

void CAPKView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAPKDoc* CAPKView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAPKDoc)));
	return (CAPKDoc*)m_pDocument;
}
#endif //_DEBUG

// CAPKView 메시지 처리기
int CAPKView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSplashWnd::ShowSplashScreen(this);

	ModifyStyle( 0, WS_CLIPCHILDREN );

	CreateViewModeInspect2D();

	m_wndTitleBar.Create(this);
	m_wndMenuBar.Create(this);
	m_wndStatusBar.Create(this);
	m_wndSideBar.Create(this);

	m_pFormInspection = (CFormInspection*) m_wndSideBar.CreateFormView( RUNTIME_CLASS(CFormInspection) );
	m_pFormInspection->SetMainView(this);
	m_pFormTeaching = (CFormTeaching*) m_wndSideBar.CreateFormView( RUNTIME_CLASS(CFormTeaching) );
	m_pFormTeaching->SetMainView(this);
	m_pFormDevice = (CFormDevice*) m_wndSideBar.CreateFormView( RUNTIME_CLASS(CFormDevice) );
	m_pFormDevice->SetMainView(this);

	ChangeViewMode(ViewModeAll);

	CVisionSystem::Instance()->Initialize(GetSafeHwnd());

	for( int i=0; i<IDX_AREA_MAX; ++i )
	{
		CVisionSystem::Instance()->SetGraphicObjects( CamTypeAreaScan, i, &m_wndAreaImageView[i].GetGraphicObject() );
	}

	m_MenuStatus = MenuStatusInspection;
	m_wndMenuBar.SetMenuChecked(CMenuBar::MenuIndexInspection);
	m_pFormInspection->ShowWindow( SW_SHOW );

	UpdateAccessRight();

	CSplashWnd::HideSplashScreen( 1000 );

	SetImageViewerResoultion();

	if(m_wndShowError.Create(CShowErrorDlg::IDD, this) == -1)
		return -1;
	if(m_wndWaitMessage.Create(CWaitMessageDlg::IDD, this) == -1)
		return -1;

	return 0;
}

void CAPKView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CRect rc;
	GetClientRect(rc);
	ArrangeWindows(rc.Width(), rc.Height());
}

void CAPKView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;

	if ( !m_wndTitleBar )
		return;

	for ( int i=0 ; i<IDX_AREA_MAX ; i++ )
	{
		if (m_wndAreaImageView[i].IsMaximized())
			m_wndAreaImageView[i].ShowNormalWindow();
	}

	ArrangeWindows( cx, cy );
}

BOOL CAPKView::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, UIExt::CResourceManager::Instance()->GetMainBodyColor() );

	return TRUE;
}

void CAPKView::OnDestroy()
{
	CVisionSystem::Instance()->Shutdown();

	MSG msg;
	while(::PeekMessage(&msg, GetSafeHwnd(), WM_LOG_MESSAGE, WM_LOG_MESSAGE, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CView::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////
// 화면 구성
void CAPKView::ArrangeWindows( int cx, int cy )
{
	LockWindowUpdate();

	const int nTitleHeight = 30;
	const int nMenuHeight = 30;
	const int nTopBorder = 0;
	const int nStatusHeight = 40;
	const int nBorder = 2;
	const int nSideBarWidth = 453;

	HDWP hDWP = BeginDeferWindowPos(4+2+2);//5+3+1+3+1+1+1);		// 사이즈 조정할 윈도우 갯수: 5(타이틀,메뉴,상태표시줄,사이드바,스트립)+3(2D 뷰)+1(2D DOT)+3(3D 뷰)+1(3D측정뷰)+1(VRS)+1(장치설정)

	hDWP = DeferWindowPos( hDWP, m_wndTitleBar.GetSafeHwnd(),      NULL, 0,		0,							cx, nTitleHeight,	0 );
	hDWP = DeferWindowPos( hDWP, m_wndMenuBar.GetSafeHwnd(),       NULL, 0,		nTitleHeight+nTopBorder,	cx, nMenuHeight,	0 );
	hDWP = DeferWindowPos( hDWP, m_wndStatusBar.GetSafeHwnd(),     NULL, 0,		cy-nStatusHeight,			cx, nStatusHeight,	0 );
	hDWP = DeferWindowPos( hDWP, m_wndSideBar.GetSafeHwnd(),       NULL, cx-nSideBarWidth,		nTitleHeight+nTopBorder+nMenuHeight+nBorder,
																			nSideBarWidth,		cy-(nTitleHeight+nTopBorder+nMenuHeight+nBorder+nBorder+nStatusHeight), 0 );

	CRect rcMainView(0, nTitleHeight+nTopBorder+nMenuHeight, cx-nSideBarWidth, cy-nStatusHeight);
	CRect rcMaximizeWnd(rcMainView.left+nBorder, rcMainView.top+nBorder, rcMainView.right-nBorder, rcMainView.bottom-nBorder);

	int nX, nY;
	int nStart	= IDX_AREA1;
	int nEnd	= IDX_AREA_MAX;
#ifdef RELEASE_1G
	if (m_ViewMode == ViewModeAll)
	{
		nX = 2;
		nY = 3;
	}
	else
	{
		nX = 1;
		nY = 1;
		switch (m_ViewMode)
		{
		case ViewModeTrayOcr:		nStart = IDX_AREA1;		nEnd = IDX_AREA2;								break;
		case ViewMode3DChipCnt:		nStart = IDX_AREA3;		nEnd = IDX_AREA4;								break;
		case ViewModeChipOcr:		nStart = IDX_AREA4;		nEnd = IDX_AREA5;								break;
		case ViewModeChip:			nStart = IDX_AREA4;		nEnd = IDX_AREA5;								break;
		case ViewModeMixing:		nStart = IDX_AREA5;		nEnd = IDX_AREA6;								break;
		case ViewModeLift:			nStart = IDX_AREA6;		nEnd = IDX_AREA_MAX;	nX = 1;		nY = 2;		break;
			break;
		default:
			break;
		}
	}
	
#elif RELEASE_2G
	if (m_ViewMode == ViewModeAll)
	{
		nX = 1;
		nY = 2;
	}
	else
	{
		nX = 1;
		nY = 1;
		switch (m_ViewMode)
		{
		case ViewModeBanding:		nStart = IDX_AREA1;		nEnd = IDX_AREA2;		break;
		case ViewModeHIC:			nStart = IDX_AREA2;		nEnd = IDX_AREA_MAX; 	break;
			break;
		default:
			break;
		}
	}
	
#elif RELEASE_3G
	if (m_ViewMode == ViewModeAll)
	{
		nX = 2;
		nY = 3;
	}
	else
	{
		nX = 2;
		nY = 2;
		switch (m_ViewMode)
		{
		case ViewModeDesiccantCutting:		nStart = IDX_AREA1;		nEnd = IDX_AREA5;					break;
		case ViewModeDesiccantMaterial:		nStart = IDX_AREA5;		nEnd = IDX_AREA_MAX; 	nY = 1; 	break;
			break;
		default:
			break;
		}
	}
	
#elif RELEASE_4G
	if (m_ViewMode == ViewModeAll)
	{
		nX = 1;
		nY = 2;
	}
	else
	{
		nX = 1;
		nY = 1;
		switch (m_ViewMode)
		{
		case ViewModeMBB:		nStart = IDX_AREA1;		nEnd = IDX_AREA2;		break;
		case ViewModeLabel:		nStart = IDX_AREA2;		nEnd = IDX_AREA_MAX; 	break;
			break;
		default:
			break;
		}
	}

#elif RELEASE_5G
	if (m_ViewMode == ViewModeAll)
	{
		nX = 1;
		nY = 2;
	}
	else
	{
		nX = 1;
		nY = 1;
		switch (m_ViewMode)
		{
		case ViewModeBox:		nStart = IDX_AREA1;		nEnd = IDX_AREA2;		break;
		case ViewModeSealing:	nStart = IDX_AREA2;		nEnd = IDX_AREA_MAX; 	break;
			break;
		default:
			break;
		}
	}

#elif RELEASE_SG
	nX = 1;
	nY = 1;
	
#elif RELEASE_6G
	if (m_ViewMode == ViewModeAll)
	{
		nX = 1;
		nY = 2;
	}
	else
	{
		nX = 1;
		nY = 1;
		switch (m_ViewMode)
		{
		case ViewModeLabel:		nStart = IDX_AREA1;		nEnd = IDX_AREA2;		break;
		case ViewModeBoxTape:	nStart = IDX_AREA2;		nEnd = IDX_AREA_MAX; 	break;
			break;
		default:
			break;
		}
	}
	
#endif

	int nViewTotalHeight	= rcMainView.Height() - nBorder*(nY+1);
	int nViewTotalWidth		= rcMainView.Width() - nBorder*(nX+1);
	int nViewWidth			= nViewTotalWidth / nX;
	int nXPos				= rcMainView.left + nBorder;
	int nViewAreaHeight		= nViewTotalHeight / nY;
	int nViewHeightHalf		= nViewAreaHeight * 2 / 3;
	int nYOffset			= 0;
	int nHeightOffset		= 0;

	int nCountX = 0;
	int nCountY = 0;
	for (int i = nStart; i < nEnd; ++i)
	{
#ifdef RELEASE_1G
		if (i == IDX_AREA2)
			continue;

#elif RELEASE_3G
		if(i == IDX_AREA5)
			nViewWidth = nViewTotalWidth + nBorder;

#endif

		hDWP = DeferWindowPos(hDWP, m_wndAreaImageView[i].GetSafeHwnd(), NULL, nXPos, rcMainView.top + nBorder + (nViewAreaHeight + nBorder) * nCountY, nViewWidth, nViewAreaHeight, 0);
		nXPos += nViewWidth + nBorder;

		nCountX++;

		if( nCountX >= nX )
		{
			nXPos = rcMainView.left + nBorder;
			nCountX = 0;
			nCountY++;
		}
	}

	DWORD dwBtnTypesMini = MBT_ZOOM_INOUT|MBT_MEASURE|MBT_LOAD|MBT_SAVE|MBT_MAXIMIZE;

	for(int i=0 ; i<IDX_AREA_MAX; i++ )
	{
		m_wndAreaImageView[i].SetMiniButtonTypeAll(dwBtnTypesMini);
		m_wndAreaImageView[i].SetMaximizedWindowPos( rcMaximizeWnd );
	}


	EndDeferWindowPos(hDWP);

	UnlockWindowUpdate();
}

BOOL CAPKView::CreateViewModeInspect2D()
{
	DWORD dwBtnTypes = MBT_ZOOM_INOUT|MBT_MEASURE|MBT_LOAD|MBT_SAVE|MBT_MAXIMIZE;

	DWORD dwTitleBgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	DWORD dwTitleFgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();
	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor     = UIExt::CResourceManager::Instance()->GetViewerBodyColor();
	DWORD dwBtnColor      = UIExt::CResourceManager::Instance()->GetViewerActiveButtonColor();

	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.fnOnDrawExt			= On2DImageViewDrawExt;
	regCB.fnOnEvent				= On2DImageViewEvent;
	regCB.fnOnFireMouseEvent	= On2DImageViewMouseEvent;
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = 
	regCB.lpUsrData[2] = regCB.lpUsrData[3] = 
	regCB.lpUsrData[4] = this;
	const int nActiveRowIndex = 0;

	for(int i = 0; i < IDX_AREA_MAX; i++)
	{
		m_wndAreaImageView[i].Create(&m_ImageViewManagerArea, this);
		m_wndAreaImageView[i].ShowWindow(SW_HIDE);
		m_wndAreaImageView[i].ShowTitleWindow(TRUE);
		m_wndAreaImageView[i].SetAnimateWindow(FALSE);
		m_wndAreaImageView[i].SetMiniButtonTypeAll(dwBtnTypes);
		m_wndAreaImageView[i].SetImageObject(CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, i));
		m_wndAreaImageView[i].SetRegisterCallBack(i, &regCB);

		m_wndAreaImageView[i].SetTitleColor(dwTitleBgColor, dwTitleFgColor, dwTitleFgColor);
		m_wndAreaImageView[i].SetStatusColor(dwStatusBgColor, dwStatusFgColor);
		m_wndAreaImageView[i].SetButtonColor(RGB(255,255,255), dwBtnColor, (DWORD)-1, (DWORD)-1);
		m_wndAreaImageView[i].SetBodyColor(dwBodyColor);

		m_wndAreaImageView[i].SetTitle(GetImageViewerTitle(CamTypeAreaScan, i), FALSE);

		m_wndAreaImageView[i].SetRealSizePerPixel((float)10.0571);

		m_wndAreaImageView[i].ShowScrollBar(TRUE);
	}

	return TRUE;
}

BOOL APIENTRY CAPKView::On2DImageViewEvent(ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData)
{
	CAPKView* pView = (CAPKView*)lpUsrData;

	if(evt == ImageViewEvent::ButtonEventLoadClick)
	{
		CVisionSystem* pSystem = CVisionSystem::Instance();

		eCamType tpCam = CamTypeAreaScan;
		int nViewIdx = nIndexData;

		CxImageObject* pImageObject = pSystem->GetImageObject(tpCam, nViewIdx);
		CString strFilter;
		strFilter = _T("Bitmap Files(*.bmp)|*.bmp|All Files(*.*)|*.*|");
		CFileDialog FileDialog(TRUE, _T("*.bmp"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter, AfxGetMainWnd());
	
		if(FileDialog.DoModal() == IDOK)
		{
			if(!pImageObject->LoadFromFile( FileDialog.GetPathName()))
			{
				::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), _T("이미지를 열 수 없습니다!"), _T("오류"), MB_OK|MB_ICONSTOP);
				return TRUE;
			}

//			pImageObject->ChangeBufferAlignment(32);
			if( tpCam == CamTypeAreaScan )
				pView->m_wndAreaImageView[nViewIdx].SetImageObject(pImageObject);

			pView->ResetGraphic( tpCam, nViewIdx);


			////////////////////////////////////////////////////////////////////////////////////////////////////
			// Label Manual ZPL Data Load				
#if defined(RELEASE_4G) || defined(RELEASE_6G)

#ifdef RELEASE_4G
			if(nIndexData == IDX_AREA2	&& CVisionSystem::Instance()->GetRunStatus() == RunStatusStop )	
#elif RELEASE_6G
			if(nIndexData == IDX_AREA1 && CVisionSystem::Instance()->GetRunStatus() == RunStatusStop )	
#endif
			{
				CString strDataPath	= FileDialog.GetPathName().Left(FileDialog.GetPathName().GetLength() - 3) + _T("ini");

				CFileStatus fs;
				if( CFile::GetStatus(strDataPath, fs) ) CVisionSystem::Instance()->LoadLabelData_Manual(FALSE, strDataPath	);
				else									CVisionSystem::Instance()->LoadLabelData_Manual(TRUE				);
			}
#endif		
			////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		return TRUE;
	}

	return FALSE;
}

 //// 2D View 마우스 이벤트
BOOL APIENTRY CAPKView::On2DImageViewMouseEvent( DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData )
{
	CAPKView* pView = (CAPKView*)lpUsrData;

	return FALSE;
}

#include <XGraphic/IxDeviceContext.h>
void APIENTRY CAPKView::On2DImageViewDrawExt( IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData )
{
	return;
	CAPKView* pView = (CAPKView*)lpUsrData;
	{
		CxImageObject* pImgObj = pView->m_wndAreaImageView[nIndexData].GetVisibleImageObject();

		if ( pImgObj )
		{
			CPen DotPen;
			DotPen.CreatePen( PS_SOLID, 1, RGB(0xff, 0, 0) );

			CPen* pOldPen = pDC->SelectObject( &DotPen );
			int nOldBkMode = pDC->SetBkMode( TRANSPARENT );

			//
			int nWidth = pImgObj->GetWidth();
			int nHeight = pImgObj->GetHeight();;

			CPoint ptCenter = pIDC->ImagePosToScreenPos( nWidth/2, nHeight/2 );
			CPoint ptImgStartPoint = pIDC->ImagePosToScreenPos( 0, 0 );
			CPoint ptImgEndPoint = pIDC->ImagePosToScreenPos( nWidth, nHeight );

			// 
			float fCalibrationX = CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[nIndexData][CalibrationDirX];
			float fCalibrationY = CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[nIndexData][CalibrationDirY];

			int nInterval = 1000; // um
			int nScale = 5;
			nScale += GetRoundLong(pIDC->GetZoomRatio());

			int nNumX = GetRoundLong(nWidth / (nInterval/fCalibrationX) / 2);
			int nNumY = GetRoundLong(nHeight / (nInterval/fCalibrationY) / 2);

			// Draw Y
			pDC->MoveTo( ptCenter.x, 0 );
			pDC->LineTo( ptCenter.x, ptCenter.y*2 );

			// Draw X
			pDC->MoveTo( 0, ptCenter.y );
			pDC->LineTo( ptCenter.x*2, ptCenter.y );

			// Draw Y - X
// 			for( int i=0; i<nNumY; ++i )
// 			{
// 				// 중심 기준 - 방향
// 				CPoint ptCenter2 = pIDC->ImagePosToScreenPos( nWidth/2, nHeight/2-GetRoundLong(nInterval/fCalibrationY)*(i+1) );
// 
// 				pDC->MoveTo( ptCenter2.x-nScale, ptCenter2.y );
// 				pDC->LineTo( ptCenter2.x+nScale, ptCenter2.y );
// 			
// 				ptCenter2 = pIDC->ImagePosToScreenPos( nWidth/2, nHeight/2+GetRoundLong(nInterval/fCalibrationY)*(i+1) );
// 
// 				// 중심 기준 + 방향
// 				pDC->MoveTo( ptCenter2.x-nScale, ptCenter2.y );
// 				pDC->LineTo( ptCenter2.x+nScale, ptCenter2.y );
// 			}
			
			// Draw X - Y
// 			for( int i=0; i<nNumX; ++i )
// 			{
// 				// 중심 기준 - 방향
// 				CPoint ptCenter2 = pIDC->ImagePosToScreenPos( nWidth/2 - GetRoundLong(nInterval/fCalibrationX)*(i+1), nHeight/2 );
// 
// 				pDC->MoveTo( ptCenter2.x, ptCenter.y-nScale );
// 				pDC->LineTo( ptCenter2.x, ptCenter.y+nScale );
// 
// 				// 중심 기준 + 방향
// 				ptCenter2 = pIDC->ImagePosToScreenPos( nWidth/2 +GetRoundLong(nInterval/fCalibrationX)*(i+1), nHeight/2 );
// 
// 				pDC->MoveTo( ptCenter2.x, ptCenter.y-nScale );
// 				pDC->LineTo( ptCenter2.x, ptCenter.y+nScale );
// 			}

			pDC->SelectObject( pOldPen );
			pDC->SetBkMode( nOldBkMode );
		}
	}
}

void CAPKView::ShowViewMode( BOOL bShow )
{
	int nCmdShow = bShow ? SW_SHOW : SW_HIDE;

	if (bShow)
	{
		if( m_ViewMode == ViewModeAll )
		{
			for ( int i=0 ; i<IDX_AREA_MAX; i++ )
			{
				m_wndAreaImageView[i].ShowWindow(nCmdShow);
			}
		}
		else
		{
			int nStartIndex, nEndIndex;

			switch( m_ViewMode )
			{
#ifdef RELEASE_1G
			case ViewModeTrayOcr:				nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA2;			break;
			case ViewMode3DChipCnt:				nStartIndex = IDX_AREA2;			nEndIndex = IDX_AREA4;			break;
			case ViewModeChipOcr:				nStartIndex = IDX_AREA4;			nEndIndex = IDX_AREA5;			break;
			case ViewModeChip:					nStartIndex = IDX_AREA4;			nEndIndex = IDX_AREA5;			break;
			case ViewModeMixing:				nStartIndex = IDX_AREA5;			nEndIndex = IDX_AREA6;			break;
			case ViewModeLift:					nStartIndex = IDX_AREA6;			nEndIndex = IDX_AREA_MAX;		break;
			
#elif RELEASE_2G
			case ViewModeBanding:				nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA2;			break;
			case ViewModeHIC:					nStartIndex = IDX_AREA2;			nEndIndex = IDX_AREA_MAX;		break;

#elif RELEASE_3G
			case ViewModeDesiccantCutting:		nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA5;			break;
			case ViewModeDesiccantMaterial:		nStartIndex = IDX_AREA5;			nEndIndex = IDX_AREA_MAX;		break;

#elif RELEASE_4G
			case ViewModeMBB:					nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA2;			break;
			case ViewModeLabel:					nStartIndex = IDX_AREA2;			nEndIndex = IDX_AREA_MAX;		break;

#elif RELEASE_5G
			case ViewModeBox:					nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA2;			break;
			case ViewModeSealing:				nStartIndex = IDX_AREA2;			nEndIndex = IDX_AREA_MAX;		break;

#elif RELEASE_SG
			case ViewModeAll:					nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA_MAX;		break;

#elif RELEASE_6G
			case ViewModeLabel:					nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA2;			break;
			case ViewModeBoxTape:				nStartIndex = IDX_AREA2;			nEndIndex = IDX_AREA_MAX;		break;

#endif
			default:							nStartIndex = IDX_AREA1;			nEndIndex = IDX_AREA_MAX;		break;
			}

			for ( int i=nStartIndex ; i<nEndIndex; i++ )
			{
				m_wndAreaImageView[i].ShowWindow(nCmdShow);
			}
		}

		for ( int i=0 ; i<IDX_AREA_MAX; i++ )
		{
			if (m_wndAreaImageView[i].IsMaximized())
			{
				m_wndAreaImageView[i].ShowNormalWindow();
			}
		}
	}
	else
	{
		for ( int i=0 ; i<IDX_AREA_MAX; i++ )
		{
			m_wndAreaImageView[i].ShowWindow(nCmdShow);
		}
	}
}

void CAPKView::ChangeViewMode(eViewMode viewMode)
{
	LockWindowUpdate();

	ShowViewMode(FALSE);
	SetViewMode( viewMode );

	CRect rc;
	GetClientRect(rc);

	ArrangeWindows(rc.Width(), rc.Height());

	ShowViewMode(TRUE);
	UnlockWindowUpdate();

	UpdateImageView(TRUE);
}

//////////////////////////////////////////////////////////////////////////
void CAPKView::UpdateAccessRight()
{
	CString strAccessName(_T(""));

	switch(CSystemConfig::Instance()->GetAccessRight())
	{
	case AccessRightOperator:
		strAccessName = _T("Operator");
		if(m_MenuStatus != MenuStatusInspection)
			OnMenuInspection();
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexDeviceSettings, FALSE);
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexTeaching, FALSE);
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexJobNumber, TRUE);
		break;

	case AccessRightEngineer:
		strAccessName = _T("Engineer");
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexDeviceSettings, TRUE);
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexTeaching, TRUE);
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexJobNumber, TRUE);
		break;

	case AccessRightProgrammer:
		strAccessName = _T("Programmer");
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexDeviceSettings, TRUE);
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexTeaching, TRUE);
		m_wndMenuBar.EnableMenuItem(CMenuBar::MenuIndexJobNumber, TRUE);
		break;
	}

	m_wndTitleBar.SetAccessName( strAccessName );
}

void CAPKView::UpdateRecipeInformation(UINT nJobNumber)
{
	CString strModelName = CModelInfo::Instance()->GetModelName();

	m_wndTitleBar.SetModelName(strModelName);
	m_wndTitleBar.SetJobNumber(nJobNumber);

	CRect rc;
	GetClientRect(rc);
	ArrangeWindows( rc.Width(), rc.Height() );
}

void CAPKView::OnChangeRunningStatus()
{
	switch (CVisionSystem::Instance()->GetRunStatus())
	{
	case RunStatusAutoRun:
		m_wndMenuBar.EnableMenuItem( CMenuBar::MenuIndexDeviceSettings, FALSE );
		m_wndMenuBar.EnableMenuItem( CMenuBar::MenuIndexTeaching, FALSE );
		m_wndMenuBar.EnableMenuItem( CMenuBar::MenuIndexJobNumber, TRUE);
		break;

	case RunStatusStop:
		if (CSystemConfig::Instance()->GetAccessRight() != AccessRightOperator)
		{
			m_wndMenuBar.EnableMenuItem( CMenuBar::MenuIndexDeviceSettings, TRUE );
			m_wndMenuBar.EnableMenuItem( CMenuBar::MenuIndexTeaching, TRUE );
			m_wndMenuBar.EnableMenuItem( CMenuBar::MenuIndexJobNumber, TRUE );
		}
		break;
	}
}

/////////////////////////////////////////////////////////
// 메뉴 변경
CFormView* CAPKView::GetMenuForm( MenuStatus menu )
{
	switch (menu)
	{
	case MenuStatusInspection:
		return m_pFormInspection;
	case MenuStatusTeaching:
		return m_pFormTeaching;
	case MenuStatusDeviceSettings:
		return m_pFormDevice;
	default:
		return NULL;
	}
	return NULL;
}

void CAPKView::SwitchMenu(MenuStatus NewMenuStatus)
{
	if(m_MenuStatus != NewMenuStatus)
	{
		GetMenuForm(m_MenuStatus)->ShowWindow(SW_HIDE);
		m_MenuStatus = NewMenuStatus;
		GetMenuForm(m_MenuStatus)->ShowWindow(SW_SHOW);
	}
}

void CAPKView::OnMenuInspection()
{
	if (m_MenuStatus == MenuStatusInspection)
		return;

	SwitchMenu( MenuStatusInspection );
	m_wndMenuBar.SetMenuChecked( CMenuBar::MenuIndexInspection );
}

void CAPKView::OnMenuTeaching()
{
	if(m_MenuStatus == MenuStatusTeaching)
		return;

	SwitchMenu(MenuStatusTeaching);
	m_wndMenuBar.SetMenuChecked(CMenuBar::MenuIndexTeaching);
}

void CAPKView::OnMenuDeviceSettings()
{
	if(m_MenuStatus == MenuStatusDeviceSettings)
		return;

	SwitchMenu(MenuStatusDeviceSettings);
	m_wndMenuBar.SetMenuChecked(CMenuBar::MenuIndexDeviceSettings);
}

void CAPKView::OnMenuJobNumber()
{
//	CEditJobNumberDlg EditJobNumberDlg(this);
//	if( EditJobNumberDlg.DoModal() == IDOK )
//	{
//		m_wndTitleBar.SetJobNumber(EditJobNumberDlg.m_nRecipeJobNumber);
//	}
	CString strDesktop = GetDesktopPath();
	CString strPath = strDesktop + _T("\\Capture");
	MakeDirectory((LPCTSTR)strPath);

	CTime time = CTime::GetCurrentTime();
	CString strFile;
	strFile.Format(_T("%s\\Screen_%04d%02d%02d_%02d%02d%02d.png"), strPath, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	HDC hScreenDC = ::GetDC(NULL);
	HDC hMemDC = ::CreateCompatibleDC(hScreenDC);

	HBITMAP hBitmap = ::CreateCompatibleBitmap(hScreenDC, width, height);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	::BitBlt(hMemDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

	CImage image;
	image.Attach(hBitmap);
	image.Save(strFile);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(NULL, hScreenDC);

	ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);
}

void CAPKView::OnMenuAccessRights()
{
	int nStatus = CVisionSystem::Instance()->GetRunStatus();

	if( nStatus == RunStatusStop )
	{
		CAccessRightsDlg AccessRightsDlg(this);
		if (AccessRightsDlg.DoModal() == IDOK)
		{
			UpdateAccessRight();
#ifdef RELEASE_2G
			::SendMessage(m_pFormTeaching->GetSafeHwnd(), WM_REFRESH_DIALOG, NULL, NULL);

#elif RELEASE_1G
			::SendMessage(m_pFormTeaching->GetSafeHwnd(), WM_REFRESH_LIFT, NULL, NULL);
			::SendMessage(m_pFormTeaching->GetSafeHwnd(), WM_REFRESH_3DCHIPCNT, NULL, NULL);
			::SendMessage(m_pFormTeaching->GetSafeHwnd(), WM_REFRESH_CHIP, NULL, NULL);

#elif RELEASE_4G
			::SendMessage(m_pFormTeaching->GetSafeHwnd(), WM_REFRESH_MBB, NULL, NULL);

#endif
			::SendMessage(m_pFormDevice->GetSafeHwnd(),	WM_REFRESH_DIALOG, NULL, NULL);
			::SendMessage(m_pFormDevice->GetSafeHwnd(), WM_REFRESH_SYSTEM, NULL, NULL);
		}
	}
}

 //////////////////////////////////////////////////////////////////////////
 // 프로그램 종료
void CAPKView::OnMenuExit()
{
	CConfirmExitDlg ConfirmExitDlg(this);
	if(ConfirmExitDlg.DoModal() == IDOK)
		((CAPKApp*)AfxGetApp())->ExitProgram();
}

//////////////////////////////////////////////////////////////////////////
// 언어 변환
void CAPKView::OnMenuLanguage()
{
	m_pFormInspection->UpdateLanguage();
	m_pFormTeaching->UpdateLanguage();
	m_pFormDevice->UpdateLanguage();
}

LRESULT CAPKView::OnLogMessage(WPARAM wParam, LPARAM lParam)
{
	eLogType type = (eLogType)wParam;
	TCHAR* lptszBuffer = (TCHAR*)lParam;

	// add log
	if (m_pFormInspection && *m_pFormInspection)
		m_pFormInspection->AddLogMessage( lptszBuffer, type );
	CxLogMsgCache::Instance()->ReleaseMsgString( lptszBuffer );

	return 0;
}

void CAPKView::ShowError( BOOL bShow, LPCTSTR lpszTitle/*=_T("")*/, LPCTSTR lpszMessage/*=_T("")*/, LPCTSTR lpszDetail/*=_T("")*/ )
{
	if (!m_wndShowError)
		return;

	if (bShow)
	{
		if (m_wndShowError.IsWindowVisible())
			m_wndShowError.ShowWindow(SW_HIDE);

		m_wndShowError.SetTitle(lpszTitle);
		m_wndShowError.SetMessage(lpszMessage);
		m_wndShowError.SetDetailMessage(lpszDetail);
		m_wndShowError.ShowWindow(SW_SHOW);

		if (CVisionSystem::Instance()->GetRunStatus() != RunStatusStop)
			ForceStop();
	}
	else
	{
		if (m_wndShowError.IsWindowVisible())
		{
			m_wndShowError.ShowWindow(SW_HIDE);
		}
	}
}

void CAPKView::ShowWaitMessage( BOOL bShow, LPCTSTR lpszTitle/*=_T("")*/, LPCTSTR lpszMessage/*=_T("")*/ )
{
	if (!m_wndWaitMessage)
		return;

	if (bShow)
	{
		if (m_wndWaitMessage.IsWindowVisible())
			m_wndWaitMessage.ShowWindow(SW_HIDE);
		m_wndWaitMessage.SetTitle(lpszTitle);
		m_wndWaitMessage.SetMessage(lpszMessage);
		m_wndWaitMessage.ShowWindow(SW_SHOW);
		m_wndWaitMessage.RedrawWindow();
	}
	else
	{
		if (m_wndWaitMessage.IsWindowVisible())
		{
			m_wndWaitMessage.ShowWindow(SW_HIDE);
		}
	}
}

void CAPKView::ForceStop()
{
	m_pFormInspection->ForceStop();
}

void CAPKView::UpdateImageView(BOOL bZoomFit/*=FALSE*/)
{
	CxImageViewCtrl* pViewCtrls[] =
	{
#ifdef RELEASE_1G
		&m_wndAreaImageView[IDX_AREA1],
		&m_wndAreaImageView[IDX_AREA2],
		&m_wndAreaImageView[IDX_AREA3],
		&m_wndAreaImageView[IDX_AREA4],
		&m_wndAreaImageView[IDX_AREA5],
		&m_wndAreaImageView[IDX_AREA6],
		&m_wndAreaImageView[IDX_AREA7],
		
#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
		&m_wndAreaImageView[IDX_AREA1],
		&m_wndAreaImageView[IDX_AREA2],
		
#elif RELEASE_3G
		&m_wndAreaImageView[IDX_AREA1],
		&m_wndAreaImageView[IDX_AREA2],
		&m_wndAreaImageView[IDX_AREA3],
		&m_wndAreaImageView[IDX_AREA4],
		&m_wndAreaImageView[IDX_AREA5],
		
#elif RELEASE_SG
		&m_wndAreaImageView[IDX_AREA1],
		
#endif
	};

	for (size_t i=0 ; i<sizeof(pViewCtrls)/sizeof(CxImageViewCtrl*) ; i++)
	{
		if (pViewCtrls[i]->IsWindowVisible())
		{
			pViewCtrls[i]->ImageUpdate();
			if (bZoomFit)
			{
				pViewCtrls[i]->ZoomFit();
			}
		}
	}
}

void CAPKView::ResetGraphic()
{
	CxImageViewCtrl* pViewCtrls[] =
	{
#ifdef RELEASE_1G
		&m_wndAreaImageView[IDX_AREA1],
		&m_wndAreaImageView[IDX_AREA2],
		&m_wndAreaImageView[IDX_AREA3],
		&m_wndAreaImageView[IDX_AREA4],
		&m_wndAreaImageView[IDX_AREA5],
		&m_wndAreaImageView[IDX_AREA6],
		&m_wndAreaImageView[IDX_AREA7],
		
#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
		&m_wndAreaImageView[IDX_AREA1],
		&m_wndAreaImageView[IDX_AREA2],
		
#elif RELEASE_3G
		&m_wndAreaImageView[IDX_AREA1],
		&m_wndAreaImageView[IDX_AREA2],
		&m_wndAreaImageView[IDX_AREA3],
		&m_wndAreaImageView[IDX_AREA4],
		&m_wndAreaImageView[IDX_AREA5],
		
#elif RELEASE_SG
		&m_wndAreaImageView[IDX_AREA1],
		
#endif
	};

	for (size_t i=0 ; i<sizeof(pViewCtrls)/sizeof(CxImageViewCtrl*) ; i++)
	{
		pViewCtrls[i]->GetGraphicObject().Reset();
		if (pViewCtrls[i]->IsWindowVisible())
		{
			pViewCtrls[i]->ImageUpdate();
		}
	}
}

void CAPKView::ResetGraphic(eCamType tpCam, int nIdx)
{
	if( tpCam == CamTypeAreaScan && (nIdx < 0 || nIdx >= IDX_AREA_MAX))
		return;

	switch (tpCam)
	{
	case CamTypeAreaScan:
		m_wndAreaImageView[nIdx].GetGraphicObject().Reset();
		if (m_wndAreaImageView[nIdx].IsWindowVisible())
			m_wndAreaImageView[nIdx].ImageUpdate();
		break;

	default:
		break;
	}
}

CxImageViewCtrl* CAPKView::GetImageView( eCamType tpCam, int nIndex /*= 0*/ )
{
	switch (tpCam)
	{
	case CamTypeAreaScan:
		if (nIndex < 0 || nIndex >= IDX_AREA_MAX)
			break;
		return &m_wndAreaImageView[nIndex];
		break;
	}

	ASSERT(FALSE);
	return NULL;
}

CxGraphicObject* CAPKView::GetGraphicObject( eCamType tpCam, int nIndex /*= 0*/ )
{
	CxImageViewCtrl* pImageView = GetImageView( tpCam, nIndex );
	if (!pImageView)
		return NULL;
	return &pImageView->GetGraphicObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// UI Update 관련
LRESULT CAPKView::OnModelChangeStatus( WPARAM wParam, LPARAM lParam )
{
	BOOL bBeginModelChange = LOWORD(wParam) == 0 ? TRUE : FALSE;
	UINT nJobNumber = (UINT)HIWORD(wParam);
	if (bBeginModelChange)
	{
		TRACE( _T("Begin Model Change\n") );
		WRITE_LOG( WL_DEBUG, _T("In View(%d) : Begin Model Change - 151119 Added"), bBeginModelChange);
	}
	else
	{
		if (lParam == TRUE)
		{
			TRACE( _T("Finish Model Change\n") );
			WRITE_LOG( WL_DEBUG, _T("In View(%d) : Finish Model Change - 151025 Added"), bBeginModelChange);
//			UpdateRecipeInformation(nJobNumber);
		}
		else
		{
			TRACE( _T("Model Change Failure\n") );
			WRITE_LOG( WL_DEBUG, _T("In View(%d) : Model Change Failure - 151119 Added"), bBeginModelChange);
		}

		ResetGraphic();
	}

	return 0;
}

//
LRESULT CAPKView::OnUpdateImageView(WPARAM wParam, LPARAM lParam)
{
	int nIndex			= (int)LOWORD(lParam);
	eCamType nCamType	= (eCamType)HIWORD(lParam);
	BOOL bZoomFit		= (BOOL)LOWORD(wParam);

	if(nCamType == CamTypeAreaScan)
	{
		ASSERT(0 <= nIndex && nIndex <= IDX_AREA_MAX);
		if(bZoomFit)
			m_wndAreaImageView[nIndex].ZoomFit();
		m_wndAreaImageView[nIndex].ImageUpdate();
	}

	return 0;
}

LRESULT CAPKView::OnSetObjectInfoReset(WPARAM wParam, LPARAM lParam)
{
	if (m_pFormInspection && *m_pFormInspection)
		m_pFormInspection->SetObjectInfoReset();

	return 0;
}

LRESULT CAPKView::OnSetObjectInfoIndex(WPARAM wParam, LPARAM lParam)
{
	int nCol = (int)LOWORD(wParam);
	int nRow = (int)HIWORD(wParam);

	if (m_pFormInspection && *m_pFormInspection)
		m_pFormInspection->SetObjectInfoIndex(nCol, nRow);

	return 0;
}

LRESULT CAPKView::OnUpdateImageViewerResolution( WPARAM wParam, LPARAM lParam)
{
	SetImageViewerResoultion();

	return 0;
}

void CAPKView::SetImageViewerResoultion()
{
	for ( int i=0; i<IDX_AREA_MAX; ++i )
	{
		m_wndAreaImageView[i].SetRealSizePerPixelW(CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[i][CalibrationDirX]);
		m_wndAreaImageView[i].SetRealSizePerPixelH(CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[i][CalibrationDirY]);

#ifdef RELEASE_1G
		if (i >= IDX_AREA3)
		{
			m_wndAreaImageView[i].SetRealSizePerPixelW(CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[i-1][CalibrationDirX]);
			m_wndAreaImageView[i].SetRealSizePerPixelH(CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[i-1][CalibrationDirY]);
		}
		else
		{
			m_wndAreaImageView[i].SetRealSizePerPixelW(CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[i][CalibrationDirX]);
			m_wndAreaImageView[i].SetRealSizePerPixelH(CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[i][CalibrationDirY]);
		}
#endif
	}
}

void CAPKView::SetTitleColor( int nViewIndex, BOOL bOK/*=TRUE*/, BOOL bDefault/*=TRUE*/ )
{
	DWORD dwTitleBgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	DWORD dwTitleFgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();

	DWORD dwTitleColor = dwTitleBgColor;
	if( !bDefault )
		dwTitleColor= (bOK ? PDC_GREEN:PDC_LIGHTRED);

	m_wndAreaImageView[nViewIndex].SetTitleColor( dwTitleColor, dwTitleFgColor, dwTitleFgColor);
}

void CAPKView::SetTrackerMode( BOOL bTrackerMode, int nIndex/*=0*/, FnOnConfirmTracker fnOnConfirmTracker/*=NULL*/, LPVOID lpContext/*=NULL*/, CRect reTrackerArea/*=CRect(0,0,0,0)*/ )
{
	m_bTrackerMode = bTrackerMode;
	if (m_bTrackerMode)
	{
		if (nIndex < 0 || nIndex >= IDX_AREA_MAX)
			return;

		m_wndAreaImageView[nIndex].SetTrackerMode(TRUE);
		m_wndAreaImageView[nIndex].SetOnConfirmTracker( fnOnConfirmTracker, lpContext );
		//		m_wndLineImageView[nIndex].ShowMaximizedWindow();
		m_wndAreaImageView[nIndex].SetTrackerPosition(reTrackerArea);
	}
	else
	{
		CxImageViewCtrl* pViewCtrls[] =
		{
#ifdef RELEASE_1G
			&m_wndAreaImageView[IDX_AREA1],
			&m_wndAreaImageView[IDX_AREA2],
			&m_wndAreaImageView[IDX_AREA3],
			&m_wndAreaImageView[IDX_AREA4],
			&m_wndAreaImageView[IDX_AREA5],
			&m_wndAreaImageView[IDX_AREA6],
			&m_wndAreaImageView[IDX_AREA7],
			
#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
			&m_wndAreaImageView[IDX_AREA1],
			&m_wndAreaImageView[IDX_AREA2],
			
#elif RELEASE_3G
			&m_wndAreaImageView[IDX_AREA1],
			&m_wndAreaImageView[IDX_AREA2],
			&m_wndAreaImageView[IDX_AREA3],
			&m_wndAreaImageView[IDX_AREA4],
			&m_wndAreaImageView[IDX_AREA5],
			
#elif RELEASE_SG
			&m_wndAreaImageView[IDX_AREA1],
			
#endif
		};

		for (size_t i=0 ; i<sizeof(pViewCtrls)/sizeof(CxImageViewCtrl*) ; i++)
		{
			pViewCtrls[i]->SetTrackerMode(FALSE);
			pViewCtrls[i]->SetOnConfirmTracker( NULL, NULL );
			//			pViewCtrls[i]->ShowNormalWindow();
		}
	}
}

void CAPKView::UpdateUI()
{
	m_pFormTeaching->UpdateUI();
}

CString CAPKView::GetDesktopPath()
{
	TCHAR szPath[MAX_PATH] = { 0 };

	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, szPath);

	return CString(szPath);
}