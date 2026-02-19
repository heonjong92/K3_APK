// FormInspection.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "APK.h"
#include "FormInspection.h"
#include "APKView.h"
#include "VisionSystem.h"
#include "SystemConfig.h"
#include "IDefectInfo.h"
#include "LanguageInfo.h"

#include "UIExt/ResourceManager.h"
#include <XGraphic\xGraphicObject.h>

// CFormInspection

IMPLEMENT_DYNCREATE(CFormInspection, CFormView)

CFormInspection::CFormInspection()
	: CFormView(CFormInspection::IDD)
	, m_pMainView(NULL)
{
}

CFormInspection::~CFormInspection()
{
}

void CFormInspection::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control	(pDX,	IDC_MODE_TAB,				m_wndTabInsp);
	DDX_Control(pDX,	IDC_SYSTEM_STATUS,			m_wndSystemStatusCtrl);
	DDX_Control(pDX,	IDC_BTN_GRAB,				m_btnAutoRun);
	DDX_Control(pDX,	IDC_BTN_IDLE,				m_btnStop);
	DDX_Control(pDX,	IDC_BTN_GRAPHIC_RESET,		m_btnGraphicReset);
	DDX_Control	(pDX,	IDC_LABEL_TEXT,				m_wndLabelText);
}

BEGIN_MESSAGE_MAP(CFormInspection, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_GRAB,				&CFormInspection::OnBnClickedBtnAutorun)
	ON_BN_CLICKED(IDC_BTN_IDLE,				&CFormInspection::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_GRAPHIC_RESET,	&CFormInspection::OnBnClickedBtnGraphicReset)
	ON_STN_CLICKED(IDC_MODE_TAB,			&CFormInspection::OnStnClickedModeTab)
END_MESSAGE_MAP()


// CFormInspection 진단입니다.

#ifdef _DEBUG
void CFormInspection::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormInspection::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormInspection 메시지 처리기입니다.
void CFormInspection::SetMainView( CAPKView* pView )
{ 
	m_pMainView = pView;

	m_wndModeTabLog.SetMainView(pView);
	m_wndModeTabObj.SetMainView(pView);
}

void CFormInspection::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;
}

void CFormInspection::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		int nTabIndex = (int)m_wndTabInsp.GetActiveTab();

		switch (nTabIndex)
		{
		case INSP_TAB_IDX_LOG:
			//m_wndModeTabLog.Refresh();
			break;
#ifdef RELEASE_1G
		case INSP_TAB_IDX_OBJ:
			m_wndModeTabObj.Refresh();
			break;
#endif
		}
	}
	else
	{
		int	 nTabIndex = (int)m_wndTabInsp.GetActiveTab();

		switch (nTabIndex)
		{
		case INSP_TAB_IDX_LOG:
			break;
#ifdef RELEASE_1G
		case INSP_TAB_IDX_OBJ:
			break;
#endif
		}
	}
}

BOOL CFormInspection::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetSideBarBodyColor();
	pDC->FillSolidRect( rc, dwBodyColor );
	return TRUE;
}

HBRUSH CFormInspection::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CFormInspection::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	UpdateData(FALSE);

	m_btnAutoRun.SetColor( RGB(120, 120, 120), RGB(120, 120, 120), RGB(60, 157, 38), RGB(180, 180, 180), RGB(255, 255, 255) );
	m_btnStop.SetColor( RGB(120, 120, 120), RGB(120, 120, 120), RGB(196, 36, 52), RGB(180, 180, 180), RGB(255, 255, 255) );

	CRect rcSystem, rcClient;
	CWnd* pSystemStatus = GetDlgItem(IDC_SYSTEM_STATUS);
	if (!pSystemStatus || !*pSystemStatus)
		return;

	pSystemStatus->GetWindowRect(rcSystem);
	ScreenToClient(rcSystem);

	UnlockWindowUpdate();

	// TAB INITIAL
	m_wndTabInsp.SetTabTextHeight( 12 );
	m_wndTabInsp.SetTabDirection(UIExt::CFlatTabWnd::TabDirectionHorzLeftTop, 36, 0);
	m_wndTabInsp.SetTabWidth( 80 );
	m_wndTabInsp.SetBackgroundColor(RGB(240,240,240));

	if (!m_wndModeTabLog.Create(CInspTabLog::IDD, &m_wndTabInsp))
		return;
	m_wndTabInsp.AddTab( _T("Log"), &m_wndModeTabLog );

#ifdef RELEASE_1G
	if (!m_wndModeTabObj.Create(CInspTabObj::IDD, &m_wndTabInsp))
		return;
	m_wndTabInsp.AddTab( _T("Object"), &m_wndModeTabObj );
#endif

	CWnd* pInspTabWnd = GetDlgItem(IDC_MODE_TAB);
	if (pInspTabWnd && *pInspTabWnd)
	{
		CRect rcWnd;
		pInspTabWnd->GetWindowRect(rcWnd);
		ScreenToClient( rcWnd );

		if ( m_wndTabInsp )
		{
			pInspTabWnd->MoveWindow( rcWnd );
			pInspTabWnd->RedrawWindow();
		}
	}

	m_wndTabInsp.SelectTab( INSP_TAB_IDX_LOG );

#ifdef _DEBUG
	CSystemConfig::Instance()->SetAccessRight( AccessRightProgrammer );
#else
	CSystemConfig::Instance()->SetAccessRight( AccessRightEngineer );
#endif
	m_pMainView->UpdateAccessRight();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();

	m_btnAutoRun.SetCheck(FALSE);
	m_btnStop.SetCheck(TRUE);

	UpdateLanguage();
}

void CFormInspection::UpdateLanguage()
{
	m_btnAutoRun.SetWindowText(CLanguageInfo::Instance()->ReadString(1));
	m_btnStop.SetWindowText(CLanguageInfo::Instance()->ReadString(2));
	m_btnGraphicReset.SetWindowText(CLanguageInfo::Instance()->ReadString(3));
}

void CFormInspection::OnBnClickedBtnAutorun()
{
	WRITE_LOG( WL_BTN, _T("CFormInspection::OnBnClickedBtnAutorun") );

#ifndef DEBUG
	if (!CVisionSystem::Instance()->CheckSystemCondition())
	{
		m_btnAutoRun.SetCheck(BST_UNCHECKED);
		m_btnStop.SetCheck(BST_CHECKED);

		return;
	}
#endif

	m_btnAutoRun.SetCheck(BST_CHECKED);
	m_btnStop.SetCheck(BST_UNCHECKED);

	m_pMainView->ResetGraphic();

	CSystemConfig::Instance()->SetAccessRight(AccessRightOperator);
	m_pMainView->UpdateAccessRight();

	CVisionSystem::Instance()->Run();

	m_wndSystemStatusCtrl.SetStatus(UIExt::CSystemStatusCtrl::StatusReady);

#ifdef RELEASE_1G
	m_wndTabInsp.SelectTab( INSP_TAB_IDX_OBJ );
#endif
}

void CFormInspection::OnBnClickedBtnStop()
{
	WRITE_LOG( WL_BTN, _T("CFormInspection::OnBnClickedBtnStop") );
	
	m_btnAutoRun.SetCheck(BST_UNCHECKED);
	m_btnStop.SetCheck(BST_CHECKED);

	CSystemConfig::Instance()->SetAccessRight(AccessRightEngineer);
	m_pMainView->UpdateAccessRight();

	CVisionSystem::Instance()->Stop();

	m_wndSystemStatusCtrl.SetStatus(UIExt::CSystemStatusCtrl::StatusStop);
}

void CFormInspection::ForceStop()
{
	WRITE_LOG( WL_SYSTEM, _T("CFormInspection::ForceStop") );

	m_btnAutoRun.SetCheck(BST_UNCHECKED);
	m_btnStop.SetCheck(BST_CHECKED);

	CVisionSystem::Instance()->Stop();

	m_wndSystemStatusCtrl.SetStatus(UIExt::CSystemStatusCtrl::StatusStop);

	m_pMainView->OnChangeRunningStatus();
}

void CFormInspection::SetObjectInfoReset()
{
	m_wndModeTabObj.SetObjectInfoReset();
}

void CFormInspection::SetObjectInfoIndex(int nCol, int nRow)
{
	m_wndModeTabObj.SetObjectInfoIndex(nRow, nCol);
}

void CFormInspection::AddLogMessage(LPCTSTR lpszLog, eLogType type)
{
	m_wndModeTabLog.AddLogMessage( lpszLog, type );
}

void CFormInspection::OnBnClickedBtnGraphicReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for( int i=0; i<IDX_AREA_MAX; ++i )
		CVisionSystem::Instance()->ResetAllResult( CamTypeAreaScan, i );

	m_btnGraphicReset.SetCheck(BST_UNCHECKED);

	CInspTabObj* pTabObject = CInspTabObj::GetPtr();
	pTabObject->SetObjectInfoReset();

//#ifdef RELEASE_2G
//	CVisionSystem::Instance()->LightOnOff(InspectTypeHIC, TRUE);
//	CVisionSystem::Instance()->CameraOnOff(InspectTypeHIC, TRUE);
//
//	CVisionSystem::Instance()->TestMerge();
//#endif

//	CxString strDelete = _T("D:\\test");
//	DeleteDirectory(strDelete);

//	CString command, path;
//	path.Format(_T("C:\\3DViewer\\point_cloud_00000.txt"));
//	command.Format(_T("python C:\\3DViewer\\visualize.py \"%s\""), path);
//	system(CT2A(command.GetString()));  // Python 실행

//	CString exePath = _T("C:\\3DViewer\\dist\\visualize.exe");
//	CString txtPath = _T("C:\\3DViewer\\point_cloud_00000.txt");
//
//	// 실행 명령어
//	CString command;
//	command.Format(_T("\"%s\" \"%s\""), exePath, txtPath);
//
//	// ShellExecute로 실행 (CMD 창 없이)
//	ShellExecute(NULL, _T("open"), exePath, txtPath, NULL, SW_HIDE);

//#ifdef RELEASE_1G
//	CString strName;
//	CFolderPickerDialog folderPickerDialog(NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, AfxGetMainWnd(), 0);
//	if (folderPickerDialog.DoModal() == IDOK)
//	{
//		CVisionSystem::Instance()->m_nMMIChipCnt = 10;
//		CVisionSystem::Instance()->m_nChipInspCnt = 0;
//		CVisionSystem::Instance()->m_bChipCntHandlerStart = TRUE;
//
//		strName = folderPickerDialog.GetPathName();
//		for (int i = 0; i <10; ++i)
//		{
//			CString strImgFolder, strImgName;
//			strImgName.Format(_T("\\%d.bmp"), i);
//			strImgFolder = strName + strImgName;
//
//			CxImageObject *pSrcImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, IDX_AREA4);
//			pSrcImgObj->LoadFromFile(strImgFolder);
//
//			CxImageObject *pImgObj = CVisionSystem::Instance()->GetImageObject_Chip(i);
//			pImgObj->Clone(pSrcImgObj);
//
//			CVisionSystem::Instance()->BeginStopWatch(IDX_AREA4);
//			CVisionSystem::Instance()->StartInspection(InspectTypeChip, IDX_AREA4, i, TRUE);
//		}
//	}
//
//	CVisionSystem::Instance()->m_bChipCntHandlerStart = FALSE;
//
//	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("MergeImage - Start"));
//
//	VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();
//	pInspectionVision->m_strMergePath = _T("D:\\06. Share\\20251112\\TESTLOTID\\001\\JPG 변환");
//	CVisionSystem::Instance()->TestMerge();
//
//	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("MergeImage - End"));
//
//#endif

//	CModelInfo::st3DChipCnt& st3DChip = CModelInfo::Instance()->Get3DChipCnt();
//	int nHeight = 1024;
//	int nWidth = 1280;
//
//	int nSrcH1 = nHeight - st3DChip.nImageCut;
//	int nSrcH2 = nHeight - st3DChip.nImageCut2;
//	int nDstH = nSrcH1 + nSrcH2;
//
//	CxImageObject *pImgObj_Dst = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, IDX_AREA3);
//	CxImageObject *pImgObj_Src1 = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, IDX_AREA1);
//	CxImageObject *pImgObj_Src2 = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, IDX_AREA2);
//	pImgObj_Dst->Create(nWidth, nDstH, 8, 1);
//
//	BYTE* pBufDst = (BYTE*)pImgObj_Dst->GetImageBuffer();
//	BYTE* pBufSrc1 = (BYTE*)pImgObj_Src1->GetImageBuffer();
//	BYTE* pBufSrc2 = (BYTE*)pImgObj_Src2->GetImageBuffer();
//
//	//////////////////////////////////////////////////////////////////////////
//	// Shift X
//	int nShiftX = st3DChip.nShiftX;
//	for (int y = 0; y < nHeight; ++y)
//	{
//		BYTE* pSrcShift = pBufSrc1 + y * nWidth;
//		BYTE* pDstShift = pBufSrc1 + y * nWidth;
//
//		if (nShiftX > 0)
//		{
//			memset(pDstShift, 0, nShiftX);
//			memcpy(pDstShift + nShiftX, pSrcShift, nWidth - nShiftX);
//		}
//		else if (nShiftX < 0)
//		{
//			int nShiftAbs = -nShiftX;
//			memcpy(pDstShift, pSrcShift + nShiftAbs, nWidth - nShiftAbs);
//			memset(pDstShift + (nWidth - nShiftAbs), 0, nShiftAbs);
//		}
//		else
//		{
//			memcpy(pDstShift, pSrcShift, nWidth);
//		}
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	// Merge
//	for (int y = 0; y < nSrcH1; ++y)
//	{
//		BYTE* pSrcLine = pBufSrc1 + y * nWidth;
//		BYTE* pDstLine = pBufDst + y * nWidth;
//		memcpy(pDstLine, pSrcLine, nWidth);
//	}
//
//	for (int y = 0; y < nSrcH2; ++y)
//	{
//		BYTE* pSrcLine = pBufSrc2 + (st3DChip.nImageCut2 + y) * nWidth;
//		BYTE* pDstLine = pBufDst + (nSrcH1 + y) * nWidth;
//		memcpy(pDstLine, pSrcLine, nWidth);
//	}
}

void CFormInspection::OnStnClickedModeTab()
{			
	eAccessRight	access		= CSystemConfig::Instance()->GetAccessRight();	
	int				nTabIndex	= (int)m_wndTabInsp.GetActiveTab();
	BOOL			bShowUI_Log	= FALSE;
	BOOL			bShowUI_Obj	= FALSE;
	CRect			reLable		= CRect(7,810,445,865);

	if( nTabIndex < 0 ) return;
}
