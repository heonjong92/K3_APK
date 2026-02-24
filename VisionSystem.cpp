
#include "stdafx.h"
#include "APK.h"
#include "VisionSystem.h"

#include "Util/xLogMsgCache.h"
#include <XUtil/xUtils.h>

#include "ModelInfo.h"
#include "LightInfo.h"
#include "SystemConfig.h"
#include "JobNumberData.h"
#include "FormInspection.h"

#include "InspectProcess.h"
#include "InspectSaveProcess.h"
#include "InspectSaveMergeProcess.h"
#include "InspectionVision.h"

#include "VisionInterfaces.h"
#include "APKView.h"
#include "SplashWnd.h"

#include "VisionStatusProvider.h"
#include "StringMacro.h"

#include <XImage/xImageObject.h>
#include <XGraphic/xGraphicObject.h>

using namespace VisionDevice;
using namespace Euresys::Open_eVision_2_11;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVisionSystem::CVisionSystem(void) :
	m_FrameGrabberInterface(this)
	, m_LightControllerInterface(this)
	, m_AreaScanCamera(this)
	, m_3DAreaScanCamera(this)
	, m_3DLineScanCamera(this)
	, m_LightControllerForArea(this)
	, m_LightControllerForArea2(this)
	, m_LightControllerForArea3(this)
{
	m_hWndUI		= NULL;
	m_RunStatus		= RunStatusStop;

	for( int i=0; i<IDX_AREA_MAX; ++i )
	{
		m_pGOArea[i] = NULL;
		m_bAreaGrabFinish[i] = FALSE;
		m_bInspectionFinish[i] = FALSE;

		m_nImageW[i] = 0;
		m_nImageH[i] = 0;
		m_nImageWBytes[i] = 0;
		m_pImageBuffer[i] = NULL;

		m_dwErrorCode[i] = 0x00000000;

		m_GrabStatus[i] = GrabStatusIdle;
	}

	for( int i=0; i<InspectTypeMax; ++i )
	{
		m_strLotID[i] = _T("");
		m_nBoxNo[i] = 0;
	}
	
	for (int i = 0; i < ACM_MAX; ++i)
	{
		m_bAcm[i] = FALSE;
	}

	m_bMeasureBusy = FALSE;

	// Tray Ocr Iss
	m_bISSOn = FALSE;

	// Stacker Ocr Z Axis
	m_nStackerZAxis = 0;

	// 3D Area Chip Count
	m_n3DAreaShot = 0;
	m_nMMI3DChipCnt = 0;
	m_b3DChipCntResult = TRUE;
	
	// Tray Lift
	m_nMMITrayQuantity_Lift = 0;

	// Tray Mixing
	m_nMMITrayQuantity_Mixing = 0;

	// 2D Area Chip Count
	m_bChipCnt = FALSE;
	m_bChipCntHandlerStart = FALSE;
	m_nChipGrabCnt = 0;
	m_nMMIChipCnt = 0;
	m_nChipInspCnt = 0;
	m_bChipCntResult = TRUE;

	// Desiccant
	m_bDesiccantInspLeft = FALSE;
	m_bDesiccantInspRight = FALSE;
	m_fDesiPosDistLeft = 0.f;
	m_fDesiPosDistRight = 0.f;
	m_bNgRetryLeft = FALSE;
	m_bNgRetryRight = FALSE;

	// Banding
	m_nBandingDir = 0;
	m_nBandingPos = 0;

	// HIC
	m_nHICNgKind = HIC_DISCOLOR_ERROR;

	// Label
	m_bLabelROITeaching = FALSE;

	m_pNotifyVisionStatusChanged = NULL;

	m_pMainView = CAPKView::GetPtr();
}

CVisionSystem::~CVisionSystem(void)
{
	for(int i = 0; i < IDX_AREA_MAX; i++)
	{
		m_ImageObjectArea[i].Destroy();
	}

	for (int i = 0; i < CHIP_GRAB_CNT_MAX; i++)
	{
		m_ImageObjectAreaChipCnt[i].Destroy();
	}

	for (int i = 0; i < 2; i++)
	{
		m_ImageObjectArea_3DChip[i].Destroy();
	}

//	m_Timer.StopTimer();
	ServerClose();
}

void CVisionSystem::Initialize( HWND hWndUI )
{
	CSplashWnd::ShowMessage( CSplashWnd::MessageTypeNormal, _T("System Initialization...") );
	WriteMessage( LogTypeNormal, _T("System Initialization..."));
	bProgramStartInitialize = TRUE;

	m_hWndUI = hWndUI;
	ASSERT( m_hWndUI );

	m_pInspectProcess			= new VisionProcess::CInspectProcess();
	m_pInspectSaveProcess		= new VisionProcess::CInspectSaveProcess();
	m_pInspectSaveMergeProcess	= new VisionProcess::CInspectSaveMergeProcess();
	m_pInspectVision			= new VisionProcess::CInspectionVision();

	// Stop
	m_pInspectProcess->Stop();
	m_pInspectSaveProcess->Stop();
	m_pInspectSaveMergeProcess->Stop();

	// start
	m_pInspectProcess->Start(m_hWndUI);
	m_pInspectSaveProcess->Start(m_hWndUI);
	m_pInspectSaveMergeProcess->Start(m_hWndUI);

	// Dongle
	CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("Dongle Connect Check..."));
	DongleConnet();

	// Recipe Load
	CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("System Data Load..."));
	RecipeLoad();
	
	// Systemcfg Load
	CSystemConfig::Instance()->Load();

#if defined(RELEASE_1G) || defined(RELEASE_SG) ||defined(RELEASE_4G) || defined(RELEASE_6G)
	CModelInfo::Instance()->LoadSimilarFontInfo();
#endif

	// Camera
	CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("Camera Connect Check..."));
	CameraConnect();

	// Create Buffer
	for( int i=0; i<IDX_AREA_MAX; ++i )
		CreateImageGrabBuffers( CamTypeAreaScan, nAREA_CAM_SIZE_X[i], nAREA_CAM_SIZE_Y[i], i );

#ifndef RELEASE_SG
	// 조명 컨트롤러
	CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("Light Ctrl Connect Check..."));
	LightCtrlConnect();
#endif

	// Server Open
	CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("Server Open Start..."));
	ServerOpen();

	bProgramStartInitialize = FALSE;

	m_stLabelData.clear();
	m_stLabelData_Manual.clear();

#if defined(RELEASE_1G)
	CreateImageGrabBuffers_Chip(nAREA_CAM_SIZE_X[IDX_AREA4], nAREA_CAM_SIZE_Y[IDX_AREA4]);
	CreateImageGrabBuffers_3DChip(nAREA_CAM_SIZE_X[IDX_AREA2], nAREA_CAM_SIZE_Y[IDX_AREA2]);
#endif

//	m_Timer.RegisterCallBack(_OnTimer, this);
//	if (!m_Timer.StartTimer(100, FALSE))
//	{
//		WriteMessage(LogTypeError, _T("Heartbeat Timer Start... Failure"));
//	}
//	else
//	{
//		WriteMessage(LogTypeNotification, _T("Heartbeat Timer Start...OK"));
//	}
}

//BOOL WINAPI CVisionSystem::_OnTimer(LPVOID lpParam)
//{
//	CVisionSystem* pThis = (CVisionSystem*)lpParam;
//	return pThis->OnTimer();
//}
//
//BOOL CVisionSystem::OnTimer()
//{
//	static int nAliveCnt = 0;
//	if (nAliveCnt > 9)
//	{
//		nAliveCnt = 0;
//
//		CameraOnOff(InspectTypeHIC, TRUE);
//	}
//	nAliveCnt++;
//
//	return TRUE;
//}

void CVisionSystem::RecipeLoad()
{
	CString strModelName = _T("");
	int nModelNumber = -1;

#ifdef RELEASE_1G
	strModelName = CModelInfo::GetLastModelNameTrayOcr();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_TRAYOCR);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, TRAYOCR_KIND))
		{
			m_pInspectVision->Load(strModelName, TRAYOCR_KIND);
		}
	}
	strModelName = CModelInfo::GetLastModelName3DChipCnt();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_3DCHIPCNT);
	if (!strModelName.IsEmpty())
	{
		CModelInfo::Instance()->Load(strModelName, TOPCHIPCNT_KIND);
	}
	strModelName = CModelInfo::GetLastModelNameChipOcr();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_CHIPOCR);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, CHIPOCR_KIND))
		{
			m_pInspectVision->Load(strModelName, CHIPOCR_KIND);
		}
	}
	strModelName = CModelInfo::GetLastModelNameChip();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_CHIP);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, CHIP_KIND))
		{
			m_pInspectVision->Load(strModelName, CHIP_KIND);
		}
	}
	strModelName = CModelInfo::GetLastModelNameMixing();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_MIXING);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, MIXING_KIND))
		{
			m_pInspectVision->Load(strModelName, MIXING_KIND);
		}
	}
	strModelName = CModelInfo::GetLastModelNameLift();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_LIFTINFO);
	if (!strModelName.IsEmpty())
	{
		CModelInfo::Instance()->Load(strModelName, LIFT_KIND);
	}
	
#elif RELEASE_SG
	strModelName = CModelInfo::GetLastModelNameStackerOcr();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_STACKER_OCR);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, STACKEROCR_KIND))
		{
			m_pInspectVision->Load(strModelName, STACKEROCR_KIND);
		}
	}
	
#elif RELEASE_2G
	strModelName = CModelInfo::GetLastModelNameBanding();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_BANDING);
	if (!strModelName.IsEmpty())
	{
		CModelInfo::Instance()->Load(strModelName, BANDING_KIND);
	}

	strModelName = CModelInfo::GetLastModelNameHIC();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_HIC);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, HIC_KIND))
		{
			m_pInspectVision->Load(strModelName, HIC_KIND);
		}
	}

#elif RELEASE_3G
	strModelName = CModelInfo::GetLastModelNameDesiccantCutting();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_DESICCANT_CUTTING);
	if (!strModelName.IsEmpty())
	{
		CModelInfo::Instance()->Load(strModelName, DESICCANT_KIND);
	}

	strModelName = CModelInfo::GetLastModelNameDesiccantMaterial();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_DESICCANT_MATERIAL);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, SUBMATERIAL_KIND))
		{
			m_pInspectVision->Load(strModelName, SUBMATERIAL_KIND);
		}
	}

	strModelName = CModelInfo::GetLastModelNameDesiccantMaterialTray();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, SUBMATERIALTRAY_KIND))
		{
			m_pInspectVision->Load(strModelName, SUBMATERIALTRAY_KIND);
		}
	}

#elif RELEASE_4G
	strModelName = CModelInfo::GetLastModelNameMBB();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_MBB);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, MBB_KIND))
		{
			m_pInspectVision->Load(strModelName, MBB_KIND);
		}
	}

	strModelName = CModelInfo::GetLastModelNameLabel();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_LABEL);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, LABEL_KIND))
		{
			m_pInspectVision->Load_Label(strModelName);
		}
	}

#elif RELEASE_5G
	strModelName = CModelInfo::GetLastModelNameBoxQuality();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_BOX);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, BOXQUALITY_KIND))
		{
			m_pInspectVision->Load(strModelName, BOXQUALITY_KIND);
		}
	}

	strModelName = CModelInfo::GetLastModelNameSealingQuality();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_SEALING);
	if (!strModelName.IsEmpty())
	{
		CModelInfo::Instance()->Load(strModelName, SEALINGQUALITY_KIND);
	}

#elif RELEASE_6G
	strModelName = CModelInfo::GetLastModelNameLabel();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_LABEL);
	if (!strModelName.IsEmpty())
	{
		if (CModelInfo::Instance()->Load(strModelName, LABEL_KIND))
		{
			m_pInspectVision->Load_Label(strModelName);
		}
	}

	strModelName = CModelInfo::GetLastModelNameTape();
	nModelNumber = CJobNumberData::Instance()->GetJobNumber(strModelName, TEACH_TAB_IDX_TAPE);
	if (!strModelName.IsEmpty())
	{
		CModelInfo::Instance()->Load(strModelName, BOXTAPE_KIND);
	}

#endif
}

void CVisionSystem::JobNumberLoad()
{
	CJobNumberData::Instance()->Load();
}

CxImageObject* CVisionSystem::GetImageObject( eCamType nCamType, int nIdx )
{
	switch(nCamType)
	{
	case CamTypeAreaScan :
		ASSERT(nIdx >= 0 && nIdx < IDX_AREA_MAX);
		return &m_ImageObjectArea[nIdx];
		break;
	}

	return NULL;
}

CxImageObject* CVisionSystem::GetImageObject_3DChip(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < 2);
	return &m_ImageObjectArea_3DChip[nIdx];

	return NULL;
}

CxImageObject* CVisionSystem::GetImageObject_Chip(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < CHIP_GRAB_CNT_MAX);
	return &m_ImageObjectAreaChipCnt[nIdx];

	return NULL;
}

const eRunStatus CVisionSystem::GetRunStatus()
{
	CxCriticalSection::Owner Lock(m_csRunStatus);
	return m_RunStatus;
}

void CVisionSystem::SetRunStatus(eRunStatus status)
{
	CxCriticalSection::Owner Lock(m_csRunStatus);
	m_RunStatus = status;
}

BOOL CVisionSystem::OnModelChange(int nRecipeNumber, eTeachTabIndex eTabIdx)
{
	CString strRecipe = CJobNumberData::Instance()->GetModelName(nRecipeNumber, eTabIdx);
	if(strRecipe.IsEmpty())
	{
		WRITE_LOG(WL_ERROR, _T("CVisionSystem::OnModelChange - Recipe Name is Empty"));
		return FALSE;
	}

	BOOL bLoaded = FALSE;
	CString strKind = _T("");

	if(!strRecipe.IsEmpty())
	{
#ifdef RELEASE_1G
		if (eTabIdx == TEACH_TAB_IDX_TRAYOCR)				strKind = TRAYOCR_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_3DCHIPCNT)		strKind = TOPCHIPCNT_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_CHIPOCR)			strKind = CHIPOCR_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_CHIP)				strKind = CHIP_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_MIXING)			strKind = MIXING_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_LIFTINFO)			strKind = LIFT_KIND;
		
#elif RELEASE_SG
		strKind = STACKEROCR_KIND;

#elif RELEASE_2G
		(eTabIdx == TEACH_TAB_IDX_BANDING) ? strKind = BANDING_KIND : strKind = HIC_KIND;

#elif RELEASE_3G
		if (eTabIdx == TEACH_TAB_IDX_DESICCANT_CUTTING)				strKind = DESICCANT_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_DESICCANT_MATERIAL)		strKind = SUBMATERIAL_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY)	strKind = SUBMATERIALTRAY_KIND;

#elif RELEASE_4G
		(eTabIdx == TEACH_TAB_IDX_MBB) ? strKind = MBB_KIND : strKind = LABEL_KIND;

#elif RELEASE_5G
		if (eTabIdx == TEACH_TAB_IDX_BOX)				strKind = BOXQUALITY_KIND;
		else if (eTabIdx == TEACH_TAB_IDX_SEALING)		strKind = SEALINGQUALITY_KIND;

#elif RELEASE_6G
		(eTabIdx == TEACH_TAB_IDX_TAPE) ? strKind = BOXTAPE_KIND : strKind = LABEL_KIND;

#endif	

		if( CModelInfo::Instance()->Load(strRecipe, strKind) )
		{
			bLoaded = TRUE;
#ifndef DEBUG
	#ifdef RELEASE_1G
			if (eTabIdx == TEACH_TAB_IDX_TRAYOCR)
			{
				m_FrameGrabberInterface.SetPageCount(CameraTypeLine);
			}
			else if (eTabIdx == TEACH_TAB_IDX_MIXING)
			{
				m_FrameGrabberInterface.SetPageCount(CameraTypeLine2);
			}
			else if (eTabIdx == TEACH_TAB_IDX_LIFTINFO)
			{
				m_FrameGrabberInterface.SetPageCount(CameraTypeLine3);
				m_FrameGrabberInterface.SetPageCount(CameraTypeLine4);
			}
	#elif RELEASE_SG
			//else if (eTabIdx == TEACH_TAB_IDX_STACKER_OCR)
			if(eTabIdx == TEACH_TAB_IDX_STACKER_OCR)
			{
				m_FrameGrabberInterface.SetPageCount(CameraTypeLine);
			}
	#endif
#endif

			// Matching
			VisionProcess::CInspectionVision* pInspectionVision = CVisionSystem::Instance()->GetInspectVisionModule();

#ifdef RELEASE_1G
			if (eTabIdx == TEACH_TAB_IDX_TRAYOCR)				pInspectionVision->Load(strRecipe, TRAYOCR_KIND);
			else if (eTabIdx == TEACH_TAB_IDX_CHIPOCR)			pInspectionVision->Load(strRecipe, CHIPOCR_KIND);
			else if (eTabIdx == TEACH_TAB_IDX_CHIP)				pInspectionVision->Load(strRecipe, CHIP_KIND);
			else if (eTabIdx == TEACH_TAB_IDX_MIXING) 			pInspectionVision->Load(strRecipe, MIXING_KIND);

			// Chip
			if (eTabIdx == TEACH_TAB_IDX_CHIP)
			{
				SetTrayMapGridReset();

				CModelInfo::stChip&	stChip = CModelInfo::Instance()->GetChip();
				SetTrayMapGridIndex(stChip.nMatrix_Y, stChip.nMatrix_X);
			}

#elif RELEASE_SG
			if (eTabIdx == TEACH_TAB_IDX_STACKER_OCR)
			{
				pInspectionVision->Load(strRecipe, STACKEROCR_KIND);
			}

#elif RELEASE_2G
//			if (eTabIdx == TEACH_TAB_IDX_HIC)
//			{
//				pInspectionVision->Load(strRecipe, HIC_KIND);
//			}

#elif RELEASE_3G
//			if (eTabIdx == TEACH_TAB_IDX_DESICCANT_MATERIAL)
//			{
//				pInspectionVision->Load(strRecipe, SUBMATERIAL_KIND);
//			}

#elif RELEASE_4G
			if (eTabIdx == TEACH_TAB_IDX_MBB)				pInspectionVision->Load(strRecipe, MBB_KIND);
			else if (eTabIdx == TEACH_TAB_IDX_LABEL)		pInspectionVision->Load_Label(strRecipe);

#elif RELEASE_5G
			if (eTabIdx == TEACH_TAB_IDX_BOX)
			{
				pInspectionVision->Load(strRecipe, BOXQUALITY_KIND);
			}

#elif RELEASE_6G
			if (eTabIdx == TEACH_TAB_IDX_LABEL)
			{
				pInspectionVision->Load_Label(strRecipe);
			}
#endif
		}
	}

	return bLoaded;
}

BOOL CVisionSystem::Run()
{
	if(GetRunStatus() != RunStatusStop)
		return FALSE;

	SetRunStatus(RunStatusAutoRun);

	PostUIMsgUpdateViewerResolution();

	m_bMeasureBusy = TRUE;

	for( int i=0; i<IDX_AREA_MAX; ++i )
		ResetAllResult( CamTypeAreaScan, i );

	return TRUE;
}

void CVisionSystem::ResetAllResult( eCamType tpCam, int nViewIndex )
{
	m_pMainView->ResetGraphic( tpCam, nViewIndex );
	m_pMainView->SetTitleColor( nViewIndex );
	m_bInspectionFinish[nViewIndex] = FALSE;
}

BOOL CVisionSystem::Stop()
{
	CxCriticalSection::Owner Lock(m_csRunStatus);
	if(GetRunStatus() == RunStatusStop)
		return FALSE;

	SetRunStatus(RunStatusStop);

	m_bMeasureBusy = FALSE;

//	m_Timer.StopTimer();

	return TRUE;
}

void CVisionSystem::WriteMessage(eLogType type, LPCTSTR lpszFormat, ...)
{
	if(lpszFormat == NULL)
		return;

	va_list argList;
	va_start(argList, lpszFormat);
	
	CxLogMsgCache* pMsgCache = CxLogMsgCache::Instance();
	TCHAR* lptszBuffer = pMsgCache->GetMsgString();
	VERIFY(_vsntprintf_s(lptszBuffer, pMsgCache->GetMsgBufferSize(), _TRUNCATE, lpszFormat, argList) <= pMsgCache->GetMsgBufferSize());

	va_end(argList);

	::PostMessage(m_hWndUI, WM_LOG_MESSAGE, WPARAM(type), LPARAM(lptszBuffer));

	switch (type)
	{
	default:
	case LogTypeNormal:
	case LogTypeNotification:
		WRITE_LOG(WL_MSG, lptszBuffer);
		break;
	case LogTypeError:
		WRITE_LOG(WL_ERROR, lptszBuffer);
		break;
	}

	TRACE(CString(lptszBuffer)+_T("\n"));
}

void CVisionSystem::SetGraphicObjects( eCamType nCamType, int nIdx, CxGraphicObject* pGO )
{
	switch (nCamType)
	{
	case CamTypeAreaScan:
		m_pGOArea[nIdx] = pGO;
		break;
	}
}

void CVisionSystem::Shutdown()
{
	GetLogSystem().Stop();

	// Camera
	for (int i=0; i<IDX_AREA_MAX; ++i )
	{
		if(m_AreaScanCamera.IsOpen(i))
			m_AreaScanCamera.Close(i);
	}

#ifdef RELEASE_1G
	m_3DAreaScanCamera.Close(0);
	m_3DLineScanCamera.Close(0);
	m_3DLineScanCamera.Close(1);
	m_3DLineScanCamera.Close(2);
	m_3DLineScanCamera.Close(3);

#endif // RELEASE_1G

	// LFine
	if( m_LightControllerForArea.IsOpen() )
		m_LightControllerForArea.Close();
	if( m_LightControllerForArea2.IsOpen() )
		m_LightControllerForArea2.Close();
	if (m_LightControllerForArea3.IsOpen())
		m_LightControllerForArea3.Close();

	if( m_pInspectProcess )
	{
		m_pInspectProcess->Stop();
		delete m_pInspectProcess;
	}

	if( m_pInspectSaveProcess )
	{
		m_pInspectSaveProcess->Stop();
		delete m_pInspectSaveProcess;
	}
	
	if (m_pInspectSaveMergeProcess)
	{
		m_pInspectSaveMergeProcess->Stop();
		delete m_pInspectSaveMergeProcess;
	}

	if( m_pInspectVision )
	{
		delete m_pInspectVision;
	}

	// Ethernet
	for (int i = 0; i < SOCKET_MAX; ++i)
		CVisionStatusProvider::Instance()->Stop(i);
}

void CVisionSystem::OnGrabFinishAreaScanCamera( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer )
{
#ifdef RELEASE_1G
	int nViewerIndex = nChannel + 1; // 1G Image Viewer4에 나와야 되므로 nChannel에 1 더함

	if (m_bChipCnt)
	{
		WRITE_LOG(WL_ERROR, _T("OnGrabFinishAreaScanCamera->m_bChipCnt start [%d]"), m_nChipGrabCnt);

//		m_pNotifyVisionStatusChanged->SendPacket(_T("14VIE0013|1|^"), SOCKET_CHIPCNT);

		BYTE* pDstBuf = (BYTE*)m_ImageObjectAreaChipCnt[m_nChipGrabCnt].GetImageBuffer();
		int nWidthBytes = m_ImageObjectAreaChipCnt[m_nChipGrabCnt].GetWidthBytes();
		memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes*nHeight);

		m_ImageObjectArea[nViewerIndex].CopyImage(&m_ImageObjectAreaChipCnt[m_nChipGrabCnt]);
		PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewerIndex);

		if (GetRunStatus() == RunStatusStop) return;

		VisionProcess::CInspectProcess::InspectItem pQueueData;
		pQueueData.pImgObj = &m_ImageObjectAreaChipCnt[m_nChipGrabCnt];
		pQueueData.pGO = m_pGOArea[nViewerIndex];
		pQueueData.nViewIndex = nViewerIndex;
		pQueueData.inspecttype = InspectTypeChip;
		pQueueData.nGrabCnt = m_nChipGrabCnt;

		if (!m_pInspectProcess->PushInspectItem(pQueueData))
			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d][%d]"), nViewerIndex, m_nChipGrabCnt);

		WRITE_LOG(WL_ERROR, _T("OnGrabFinishAreaScanCamera->m_bChipCnt End [%d]"), m_nChipGrabCnt);
		m_nChipGrabCnt++;
	}
	else
	{
		if (!CreateImageGrabBuffers(CamTypeAreaScan, nWidth, nHeight, nViewerIndex))
			return;

		BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[nViewerIndex].GetImageBuffer();
		int nWidthBytes = m_ImageObjectArea[nViewerIndex].GetWidthBytes();
		memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes*nHeight);

		PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewerIndex);

		if (GetRunStatus() == RunStatusStop) return;

		VisionProcess::CInspectProcess::InspectItem pQueueData;
		pQueueData.pImgObj = &m_ImageObjectArea[nViewerIndex];
		pQueueData.pGO = m_pGOArea[nViewerIndex];
		pQueueData.nViewIndex = nViewerIndex;
		pQueueData.inspecttype =InspectTypeChipOcr;
		pQueueData.nGrabCnt = 0;

		if (!m_pInspectProcess->PushInspectItem(pQueueData))
			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nViewerIndex);
	}

#elif RELEASE_3G
	int nViewerIndex = IDX_AREA5;

	if (nChannel == 0)
	{
		if(m_bDesiccantInspLeft)		nViewerIndex = nChannel + 2;
		else							nViewerIndex = nChannel;
	}
	else if (nChannel == 1)
	{
		if (m_bDesiccantInspRight)		nViewerIndex = nChannel + 2;
		else							nViewerIndex = nChannel;
	}
	else if (nChannel == 2)
		nViewerIndex = IDX_AREA5; // InspectTypeMaterial

	if (!CreateImageGrabBuffers(CamTypeAreaScan, nWidth, nHeight, nViewerIndex))
		return;

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[nViewerIndex].GetImageBuffer();
	int nWidthBytes = m_ImageObjectArea[nViewerIndex].GetWidthBytes();
	memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes*nHeight);

	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewerIndex);

	if (GetRunStatus() == RunStatusStop) return;

	VisionProcess::CInspectProcess::InspectItem pQueueData;
	pQueueData.pImgObj = &m_ImageObjectArea[nViewerIndex];
	pQueueData.pGO = m_pGOArea[nViewerIndex];
	pQueueData.nViewIndex = nViewerIndex;
	pQueueData.inspecttype = (InspectType)(nViewerIndex);

	if (!m_pInspectProcess->PushInspectItem(pQueueData))
		WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nViewerIndex);

#elif RELEASE_2G || RELEASE_6G
	if (nChannel == 0)
	{
//		if (!CreateImageGrabBuffers(CamTypeAreaScan, nWidth, nHeight, nChannel))
//			return;
		m_ImageObjectArea[nChannel].Create(nWidth, nHeight, 8, 1);
	}
	else
	{
		m_ImageObjectArea[nChannel].Create(nWidth, nHeight, 8, 3, 0, CxImageObject::ChannelSeqBGR);
	}

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[nChannel].GetImageBuffer();
	int nWidthBytes = m_ImageObjectArea[nChannel].GetWidthBytes();
	memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes*nHeight);

	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nChannel);

	if (GetRunStatus() == RunStatusStop) return;

	VisionProcess::CInspectProcess::InspectItem pQueueData;
	pQueueData.pImgObj = &m_ImageObjectArea[nChannel];
	pQueueData.pGO = m_pGOArea[nChannel];
	pQueueData.nViewIndex = nChannel;
	pQueueData.inspecttype = (InspectType)nChannel;

	if (!m_pInspectProcess->PushInspectItem(pQueueData))
		WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nChannel);
	
#elif RELEASE_4G || RELEASE_5G
	if (!CreateImageGrabBuffers(CamTypeAreaScan, nWidth, nHeight, nChannel))
		return;

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[nChannel].GetImageBuffer();
	int nWidthBytes = m_ImageObjectArea[nChannel].GetWidthBytes();
	memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes*nHeight);

	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nChannel);

	if (GetRunStatus() == RunStatusStop) return;

	VisionProcess::CInspectProcess::InspectItem pQueueData;
	pQueueData.pImgObj = &m_ImageObjectArea[nChannel];
	pQueueData.pGO = m_pGOArea[nChannel];
	pQueueData.nViewIndex = nChannel;
	pQueueData.inspecttype = (InspectType)nChannel;

	if (!m_pInspectProcess->PushInspectItem(pQueueData))
		WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nChannel);

#endif
}

void CVisionSystem::OnGrabFinish3DAreaScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer)
{
#ifdef RELEASE_1G
	nChannel += m_n3DAreaShot;

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea_3DChip[m_n3DAreaShot].GetImageBuffer();
	int nWidthBytes = m_ImageObjectArea_3DChip[m_n3DAreaShot].GetWidthBytes();
	memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes*nHeight);

	m_ImageObjectArea[IDX_AREA2].CopyImage(&m_ImageObjectArea_3DChip[m_n3DAreaShot]);
	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nChannel);

	if (m_n3DAreaShot == 1)
	{
		CModelInfo::st3DChipCnt& st3DChip = CModelInfo::Instance()->Get3DChipCnt();

		int nSrcH1 = nHeight - st3DChip.nImageCut;
		int nSrcH2 = nHeight - st3DChip.nImageCut2;
		int nDstH = nSrcH1 + nSrcH2;

		m_ImageObjectArea[IDX_AREA3].Create(nWidth, nDstH, 8, 1);

		//////////////////////////////////////////////////////////////////////////
		if (st3DChip.bEqualize)
		{
			EImageBW8 ImageSrc1, ImageSrc2;
			ImageSrc1.SetImagePtr(m_ImageObjectArea_3DChip[0].GetWidth(), m_ImageObjectArea_3DChip[0].GetHeight(), m_ImageObjectArea_3DChip[0].GetImageBuffer(), m_ImageObjectArea_3DChip[0].GetWidthBytes() * 8);
			ImageSrc2.SetImagePtr(m_ImageObjectArea_3DChip[1].GetWidth(), m_ImageObjectArea_3DChip[1].GetHeight(), m_ImageObjectArea_3DChip[1].GetImageBuffer(), m_ImageObjectArea_3DChip[1].GetWidthBytes() * 8);

			EasyImage::Equalize(&ImageSrc1, &ImageSrc1);
			EasyImage::Equalize(&ImageSrc2, &ImageSrc2);
		}

		//////////////////////////////////////////////////////////////////////////
		// Shift X
		BYTE* pBufDst = (BYTE*)m_ImageObjectArea[IDX_AREA3].GetImageBuffer();
		BYTE* pBufSrc1 = (BYTE*)m_ImageObjectArea_3DChip[0].GetImageBuffer();
		BYTE* pBufSrc2 = (BYTE*)m_ImageObjectArea_3DChip[1].GetImageBuffer();

		int nShiftX = st3DChip.nShiftX;
		for (UINT y = 0; y < nHeight; ++y)
		{
			BYTE* pSrcShift = pBufSrc1 + y * nWidth;
			BYTE* pDstShift = pBufSrc1 + y * nWidth;

			if (nShiftX > 0)
			{
				memset(pDstShift, 0, nShiftX);
				memcpy(pDstShift + nShiftX, pSrcShift, nWidth - nShiftX);
			}
			else if (nShiftX < 0)
			{
				int nShiftAbs = -nShiftX;
				memcpy(pDstShift, pSrcShift + nShiftAbs, nWidth - nShiftAbs);
				memset(pDstShift + (nWidth - nShiftAbs), 0, nShiftAbs);
			}
			else
			{
				memcpy(pDstShift, pSrcShift, nWidth);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Merge
		for (int y = 0; y < nSrcH1; ++y)
		{
			BYTE* pSrcLine = pBufSrc1 + y * nWidth;
			BYTE* pDstLine = pBufDst + y * nWidth;
			memcpy(pDstLine, pSrcLine, nWidth);
		}

		for (int y = 0; y < nSrcH2; ++y)
		{
			BYTE* pSrcLine = pBufSrc2 + (st3DChip.nImageCut2 + y) * nWidth;
			BYTE* pDstLine = pBufDst + (nSrcH1 + y) * nWidth;
			memcpy(pDstLine, pSrcLine, nWidth);
		}

		if (GetRunStatus() != RunStatusStop)// return;
		{
			VisionProcess::CInspectProcess::InspectItem pQueueData;
			pQueueData.pImgObj = &m_ImageObjectArea[IDX_AREA3];
			pQueueData.pGO = m_pGOArea[IDX_AREA3];
			pQueueData.nViewIndex = IDX_AREA3;
			pQueueData.inspecttype = InspectType3DChipCnt;
			pQueueData.nGrabCnt = m_n3DAreaShot;

			if (!m_pInspectProcess->PushInspectItem(pQueueData))
				WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nChannel);
		}
	}

	// View
	if (m_n3DAreaShot == 0)
	{
		if (GetRunStatus() != RunStatusStop)
			m_pNotifyVisionStatusChanged->SendPacket(_T("12VIE0013|1|^"), SOCKET_3DCHIPCNT);
	
		m_n3DAreaShot++;
	}
	else
		m_n3DAreaShot = 0;
#endif
}

void CVisionSystem::OnGrabFinish3DLineScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer)
{
	WRITE_LOG(WL_ERROR, _T("OnGrabFinish3DLineScanCamera[%d]"), nChannel);

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	if (!CreateImageGrabBuffers_Line(nWidth, nHeight, nChannel, FALSE))
		return;

	WRITE_LOG(WL_ERROR, _T("OnGrabFinish3DLineScanCamera Memcpy start C,W,H [%d][%d][%d] // Viewer[%d][%d]"),
		nChannel, nWidth, nHeight, m_ImageObjectArea[nChannel].GetWidth(), m_ImageObjectArea[nChannel].GetHeight());

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[nChannel].GetImageBuffer();
	int nWidthBytes = m_ImageObjectArea[nChannel].GetWidthBytes();
	memcpy(pDstBuf, pBuffer, (size_t)nWidthBytes * nHeight);

	WRITE_LOG(WL_ERROR, _T("OnGrabFinish3DLineScanCamera Memcpy End[%d]"), nChannel);

	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nChannel);

	if (GetRunStatus() == RunStatusStop) return;
	
	int nInspType = InspectTypeMax;
	switch (nChannel)
	{
	case 0:
#ifdef RELEASE_SG
				 nInspType = InspectTypeStackerOcr;		break;
#elif RELEASE_1G
				 nInspType = InspectTypeTrayOcr;		break;
	case 4:		 nInspType = InspectTypeMixing;			break;
	case 5:		 nInspType = InspectTypeLiftFront;		break;
	case 6:		 nInspType = InspectTypeLiftRear;		break;
#endif
	default:
		break;
	}

	VisionProcess::CInspectProcess::InspectItem pQueueData;
	pQueueData.pImgObj = &m_ImageObjectArea[nChannel];
	pQueueData.pGO = m_pGOArea[nChannel];
	pQueueData.nViewIndex = nChannel;
	pQueueData.inspecttype = (InspectType)nInspType;
	pQueueData.nGrabCnt = 0;

	if (!m_pInspectProcess->PushInspectItem(pQueueData))
		WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nChannel);

#endif
}

//////////////////////////////////////////////////////////////////////////
void CVisionSystem::PostUIMsgModelChangeStatus(BOOL bIsFinish, int nRecipeNumber, BOOL bModelChangeOK)
{
	::PostMessage(m_hWndUI, WM_MODEL_CHANGE_STATUS, MAKEWPARAM(bIsFinish, nRecipeNumber), MAKELPARAM(bModelChangeOK, 0));
}

void CVisionSystem::PostUIMsgUpdateImageView(BOOL bZoomFit, eCamType nCamtype, int nIndex/*=0*/ )
{
	::PostMessage(m_hWndUI, WM_UPDATE_IMAGE_VIEW, MAKEWPARAM(bZoomFit, 0), MAKELPARAM(nIndex, nCamtype));
}

void CVisionSystem::PostUIMsgUpdateViewerResolution()
{
	::PostMessage(m_hWndUI, WM_UPDATE_VIEWER_RESOLUTION, MAKEWPARAM(0, 0), MAKELPARAM(0, 0));
}

//////////////////////////////////////////////////////////////////////////
BOOL CVisionSystem::CreateImageGrabBuffers( eCamType nCamtype, int nWidth, int nHeight, int nViewIndex )
{
	if( nWidth <= 0 || nHeight <= 0 )
		return FALSE;

	BOOL bOK = TRUE;

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	switch (nViewIndex)
	{
	case IDX_AREA1:
#ifdef RELEASE_SG
						nWidth = CModelInfo::Instance()->GetStackerOcr().lPageCount;	break;

#elif RELEASE_1G
						nHeight = CModelInfo::Instance()->GetTrayOcr().lPageCount;		break;
	case IDX_AREA5:		nWidth = CModelInfo::Instance()->GetMixing().lPageCount;		break;
	case IDX_AREA6:
	case IDX_AREA7:		nWidth = CModelInfo::Instance()->GetLiftInfo().lPageCount;		break;
#endif
	default:
		break;
	}
#endif 

	switch( nCamtype )
	{
	case CamTypeAreaScan:

		if( m_ImageObjectArea[nViewIndex].GetWidth()  == nWidth		&&
			m_ImageObjectArea[nViewIndex].GetHeight() == nHeight	&&
			m_ImageObjectArea[nViewIndex].GetChannelSeq() != CxImageObject::ChannelSeqBGR ) 
		{
			bOK = TRUE;
			break;
		}

		eCamChannel ecamchannel = CamChannel8BIT;
		CxImageObject::ChannelSeqModel echannelseqmodel = CxImageObject::ChannelSeqUnknown;

#if defined(RELEASE_2G) || defined(RELEASE_6G)
		if (m_ImageObjectArea[nViewIndex].GetWidth() == nWidth &&
			m_ImageObjectArea[nViewIndex].GetHeight() == nHeight &&
			m_ImageObjectArea[nViewIndex].GetChannelSeq() != CxImageObject::ChannelSeqBGR)
		{
			bOK = TRUE;
			break;
		}

		if (nViewIndex == IDX_AREA2)
		{
			ecamchannel = CamChannel24BIT;
			echannelseqmodel = CxImageObject::ChannelSeqBGR;
		}
#endif

		CxImageObject vImageCutY;
		if (!vImageCutY.Create(nWidth, nHeight, 8, ecamchannel, 0, echannelseqmodel))
		{
			WRITE_LOG( WL_ERROR, _T("CreateImageGrabBuffers[%d] : vImageCutY Create Fail"), nViewIndex );
			bOK = FALSE;
			break;
		}

		if( !m_ImageObjectArea[nViewIndex].Clone( &vImageCutY ) )
		{
			WRITE_LOG( WL_ERROR, _T("CreateImageGrabBuffers[%d] : m_ImageObjectArea Clone Fail"), nViewIndex );
			bOK = FALSE;
			break;
		}

		vImageCutY.Destroy();

		PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewIndex);

		WRITE_LOG( WL_MSG, _T("CreateImageGrabBuffers[%d] : CamTypeAreaScan Finish"), nViewIndex );
		break;
	}

	WRITE_LOG( WL_MSG, _T("CreateImageGrabBuffers[%d] :: End"), nViewIndex );

	return bOK;
}

BOOL CVisionSystem::CreateImageGrabBuffers_Line(int nWidth, int nHeight, int nViewIndex, BOOL bManual)
{
	if (nWidth <= 0 || nHeight <= 0)
		return FALSE;

	BOOL bOK = TRUE;

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	if (bManual)
	{
		switch (nViewIndex)
		{
		case IDX_AREA1:

#ifdef RELEASE_SG
							nHeight = CModelInfo::Instance()->GetStackerOcr().lPageCount;	break;
#elif RELEASE_1G
							nHeight = CModelInfo::Instance()->GetTrayOcr().lPageCount;		break;
		case IDX_AREA5:		nWidth = CModelInfo::Instance()->GetMixing().lPageCount;		break;
		case IDX_AREA6:
		case IDX_AREA7:		nWidth = CModelInfo::Instance()->GetLiftInfo().lPageCount;		break;
#endif		
		default:
			break;
		}
	}
#endif 
	eCamChannel ecamchannel = CamChannel8BIT;

	CxImageObject vImageCutY;
	if (!vImageCutY.Create(nWidth, nHeight, 8, 1, 0))
	{
		WRITE_LOG(WL_ERROR, _T("CreateImageGrabBuffers_Line[%d] : vImageCutY Create Fail"), nViewIndex);
		bOK = FALSE;
	}

	if (!m_ImageObjectArea[nViewIndex].Clone(&vImageCutY))
	{
		WRITE_LOG(WL_ERROR, _T("CreateImageGrabBuffers_Line[%d] : m_ImageObjectArea Clone Fail"), nViewIndex);
		bOK = FALSE;
	}

	vImageCutY.Destroy();

	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewIndex);

	WRITE_LOG(WL_MSG, _T("CreateImageGrabBuffers_Line[%d] :: End"), nViewIndex);

	return bOK;
}

BOOL CVisionSystem::CreateImageGrabBuffers_3DChip(int nWidth, int nHeight)
{
	if (nWidth <= 0 || nHeight <= 0)
		return FALSE;

	for (int i = 0; i < 2; i++)
	{
		m_ImageObjectArea_3DChip[i].Create(nWidth, nHeight, 8, 1, 0);
	}

	WRITE_LOG(WL_MSG, _T("CreateImageGrabBuffers_Chip :: End"));

	return TRUE;
}

BOOL CVisionSystem::CreateImageGrabBuffers_Chip(int nWidth, int nHeight)
{
	if (nWidth <= 0 || nHeight <= 0)
		return FALSE;

	for (int i = 0; i < CHIP_GRAB_CNT_MAX; i++)
	{
		m_ImageObjectAreaChipCnt[i].Create(nWidth, nHeight, 8, 1, 0);
	}

	WRITE_LOG(WL_MSG, _T("CreateImageGrabBuffers_Chip :: End"));

	return TRUE;
}

BOOL CVisionSystem::ResetImageGrabBuffer_3DChip()
{
	TRACE(_T("ResetImageGrabBuffer_3DChip\n"));

	for (int i = 0; i < 2; ++i)
	{
		if (!m_ImageObjectArea_3DChip[i].IsValid())
			return FALSE;

		BYTE* pBuffer = (BYTE*)m_ImageObjectArea_3DChip[i].GetImageBuffer();
		int nBufferSize = m_ImageObjectArea_3DChip[i].GetWidthBytes() * m_ImageObjectArea_3DChip[i].GetHeight();
		ZeroMemory(pBuffer, nBufferSize);
	}

	return TRUE;
}

BOOL CVisionSystem::ResetImageGrabBuffer_Chip()
{
	TRACE(_T("ResetImageGrabBuffer_Chip\n"));

	for (int i = 0; i < CHIP_GRAB_CNT_MAX; ++i)
	{
		if (!m_ImageObjectAreaChipCnt[i].IsValid())
			return FALSE;

		BYTE* pBuffer = (BYTE*)m_ImageObjectAreaChipCnt[i].GetImageBuffer();
		int nBufferSize = m_ImageObjectAreaChipCnt[i].GetWidthBytes() * m_ImageObjectAreaChipCnt[i].GetHeight();
		ZeroMemory(pBuffer, nBufferSize);
	}

	return TRUE;
}

void CVisionSystem::SetTrayMapGridReset()
{
	::PostMessage(m_hWndUI, WM_UPDATE_MAP_RESET, WPARAM(0), LPARAM(0));
}

void CVisionSystem::SetTrayMapGridIndex(int nCol, int nRow)
{
	::PostMessage(m_hWndUI, WM_UPDATE_MAP_SETTING, MAKEWPARAM(nCol, nRow), MAKELPARAM(0, 0));
}

void CVisionSystem::OnDisconnectSerialDevice( SerialDevice device, DWORD dwError )
{
	switch (device)
	{
	case IDeviceNotify::SerialDeviceLedCtrlLine1:
		WriteMessage(LogTypeError, _T("LedCtrlLine1 Disconnected"));
		m_LightControllerForArea.Close();
		break;
	case IDeviceNotify::SerialDeviceLedCtrlLine2:
		WriteMessage(LogTypeError, _T("LedCtrlLine2 Disconnected"));
		m_LightControllerForArea2.Close();
		break;
	case IDeviceNotify::SerialDeviceLedCtrlLine3:
		WriteMessage(LogTypeError, _T("LedCtrlLine3 Disconnected"));
		m_LightControllerForArea3.Close();
		break;
	default:
		break;
	}
}

BOOL CVisionSystem::StartInspection( InspectType inspecttype, int nViewIndex, int nGrabCnt, BOOL bManual )
{
	m_pGOArea[nViewIndex]->Reset();
	m_dwErrorCode[nViewIndex] = 0x00000000;

	BOOL bResultOK = FALSE;
	bResultOK = m_pInspectVision->_Inspection( m_pGOArea[nViewIndex], inspecttype, nViewIndex, nGrabCnt, bManual );

	InspectionFinish( inspecttype, nViewIndex );

	m_pMainView->SetTitleColor(nViewIndex, bResultOK, FALSE);

#ifdef RELEASE_2G
	if (inspecttype == InspectTypeBanding)
		nGrabCnt = m_nBandingDir * 3 + m_nBandingPos;
#endif

#ifdef RELEASE_1G
//	if(inspecttype != InspectTypeTrayOcr)
	SaveOriginImage( nViewIndex, inspecttype, nGrabCnt);

#else
	SaveOriginImage(nViewIndex, inspecttype, nGrabCnt);

#endif

 	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewIndex);

	return bResultOK;
}

void CVisionSystem::InspectionFinish(InspectType inspecttype, int nViewIndex)
{
	m_bInspectionFinish[nViewIndex] = TRUE;

	switch (inspecttype)
	{
#ifdef RELEASE_1G
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:
		if ((m_bInspectionFinish[IDX_AREA6] & m_bInspectionFinish[IDX_AREA7]) == TRUE)
		{
			m_bInspectionFinish[IDX_AREA6] = m_bInspectionFinish[IDX_AREA7] = FALSE;
			InspectionResult(inspecttype, nViewIndex);
		}
		break;
#endif
	case InspectTypeMax:
		break;

	default:

#ifdef RELEASE_1G
		if ((inspecttype == InspectTypeChipOcr))
			LightOnOff(inspecttype, FALSE);

#elif RELEASE_2G
//		if ((inspecttype == InspectTypeHIC))
//			LightOnOff(inspecttype, FALSE);
		
//		if(inspecttype == InspectTypeBanding)
			LightOnOff(inspecttype, FALSE);

//		if((inspecttype == InspectTypeBanding) && m_nBandingPos != 0)
//			LightOnOff(inspecttype, FALSE);


#elif RELEASE_3G
		// Desiccant Position 검사일 때는 DEE에서 Light Off
		if (inspecttype == InspectTypeDesiccantLeft || inspecttype == InspectTypeDesiccantRight || inspecttype == InspectTypeMaterial)
			LightOnOff(inspecttype, FALSE);

#elif RELEASE_5G
		// 안 꺼질 때가 있어서 2번 해보자!!
		LightOnOff(inspecttype, FALSE);
		LightOnOff(inspecttype, FALSE);
#else
		LightOnOff(inspecttype, FALSE);
		
#endif
		m_bInspectionFinish[nViewIndex] = FALSE;
		InspectionResult(inspecttype, nViewIndex);
		break;
	}

	WriteMessage(LogTypeNormal, _T("InspectionFinish[%d] :: End [ %3.3f sec ]"), nViewIndex, GetStopWatch(nViewIndex));
}

void CVisionSystem::InspectionResult(InspectType inspecttype, int nViewIndex)
{
	BOOL bOK = Result_OK(nViewIndex, inspecttype);

	ResultOKNG_MMI(inspecttype, bOK);
}

void CVisionSystem::SaveTrayOcr()
{
#ifdef RELEASE_1G
	CString strFileName;
	strFileName = CSystemConfig::Instance()->GetTrayOcrImageFile();

	///////////////////////////////////////////////////////////////////////////////////
	// ----- Set File Path -----
	CString strDriver = CSystemConfig::Instance()->GetSaveLogPath();
	
	CTime tmCurrent = CTime::GetCurrentTime();
	CString strImgSaveDay, strImgSaveTime;
	strImgSaveDay.Format(_T("%04d%02d%02d"), tmCurrent.GetYear(), tmCurrent.GetMonth(), tmCurrent.GetDay());
	strImgSaveTime.Format(_T("%02d%02d%02d"), tmCurrent.GetHour(), tmCurrent.GetMinute(), tmCurrent.GetSecond());
	
	// ----- Set LotID BoxNo -----
	CString strLotID = CVisionSystem::Instance()->GetLotID(InspectTypeTrayOcr);
	if (strLotID.IsEmpty()) strLotID = _T("LOT ID Empty");
	
	short sBoxNo = CVisionSystem::Instance()->GetBoxNo(InspectTypeTrayOcr);

	CString strImgBasePath;
	strImgBasePath.Format(_T("%s:\\HITS\\ImageLog\\%s\\%s\\%03d\\OK"), strDriver, strImgSaveDay, strLotID, sBoxNo);
	MakeDirectory(strImgBasePath);

	CString strRecipeName = CModelInfo::Instance()->GetModelNameTrayOcr();
	if (strRecipeName.IsEmpty()) strRecipeName = _T("Recipe Empty");

	CString strImgPathName;
	strImgPathName.Format(_T("%s\\%s_TrayOcr_%s.bmp"), strImgBasePath, strImgSaveTime, strRecipeName);

	// 3. 파일 이동
	if (!MoveFileEx(strFileName, strImgPathName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
	{
		DWORD dw = GetLastError();
		CString msg;
		msg.Format(_T("MoveFileEx 실패 (Err=%lu)"), dw);
		WRITE_LOG(WL_ERROR, msg);
		return;
	}

	// 4. OpenCV로 BMP 읽기
	USES_CONVERSION;
	cv::String str = CT2A(strImgPathName);
	cv::Mat mat = cv::imread(str);
	if (mat.empty())
	{
		WRITE_LOG(WL_ERROR, _T("BMP 로드 실패"));
		return;
	}

	// ----- Merge Image Save -----
	CString strSendPath, strMergePath, strMergePathName;
	strSendPath = CSystemConfig::Instance()->GetSendPath();

	strMergePath.Format(_T("%s\\%s\\%s\\%03d\\"), strSendPath, strImgSaveDay, strLotID, sBoxNo);
	MakeDirectory(strMergePath);

	strMergePathName.Format(_T("%s\\%s_TrayOcr_%s.jpg"), strMergePath, strImgSaveTime, strRecipeName);

	// JPG 품질 설정 (90%)
	std::vector<int> params;
	params.push_back(cv::IMWRITE_JPEG_QUALITY);
	params.push_back(90);

	cv::String str2 = CT2A(strMergePathName);
	if (!cv::imwrite(str2, mat, params))
	{
		WRITE_LOG(WL_ERROR, _T("JPG 저장 실패"));
		return;
	}

#endif
}

void CVisionSystem::SaveOriginImage( int nViewIndex, InspectType inspecttype, int nGrabCnt )
{
 	if( GetRunStatus() == RunStatusStop )
 		return;

 	if( !CSystemConfig::Instance()->GetOriginImgSave() )
 		return;

	WriteMessage(LogTypeNormal, _T("Func In -> Thread Origin Image Save [%d] :: Start"), nViewIndex + 1);

	VisionProcess::CInspectSaveProcess::SaveItem pQueueData;
	pQueueData.pImgObj		= &m_ImageObjectArea[nViewIndex];
	pQueueData.inspecttype	= inspecttype;
	pQueueData.nViewIndex	= nViewIndex;
	pQueueData.nGrabCnt		= nGrabCnt;

	if( !m_pInspectSaveProcess->PushSaveItem(pQueueData) )
		WRITE_LOG( WL_ERROR, _T("m_pInspectSaveProcess->PushSaveItem :: Fail[%d]"), nViewIndex + 1 );

	WriteMessage(LogTypeNormal, _T("Func Out <- Thread Origin Image Save [%d] :: End"), nViewIndex + 1 );
}

void CVisionSystem::StartManualLabelInspect(int nViewIndex, InspectType inspecttype)
{
#if defined(RELEASE_4G) || defined(RELEASE_6G)
	VisionProcess::CInspectProcess::InspectItem pQueueData;
	pQueueData.pImgObj = &m_ImageObjectArea[nViewIndex];
	pQueueData.pGO = m_pGOArea[nViewIndex];
	pQueueData.nViewIndex = nViewIndex;
	pQueueData.inspecttype = InspectTypeLabel;
	pQueueData.nGrabCnt = 0;

	if (!m_pInspectProcess->PushInspectItem(pQueueData))
		WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), nViewIndex);
#endif
}

void CVisionSystem::StartSaveImage( CxImageObject *pImgObj, InspectType inspecttype, int nViewIndex, int nGrabCnt)
{
	m_pInspectVision->SaveImage( pImgObj, inspecttype, nViewIndex, nGrabCnt);
}

void CVisionSystem::StartSaveImageMerge( BOOL bXMerge )
{
	CString strPath = m_pInspectVision->m_strMergePath;
	m_pInspectVision->MergeImage(strPath, bXMerge);

	m_pInspectVision->m_strMergePath = _T("");
}

BOOL CVisionSystem::OCRMeasureInspection(CxGraphicObject* pGO, CxImageObject* pImgObj, VisionProcess::TrayOCRData& stTrayData, BOOL bSave /*=FALSE*/)
{
#ifdef RELEASE_1G
	CModelInfo::stTrayOcr& stTrayModel = CModelInfo::Instance()->GetTrayOcr();
	m_pInspectVision->EVisionOCR_TrayOCR(pGO, pImgObj, IDX_AREA1, stTrayData, stTrayModel, bSave);
#elif RELEASE_SG
	CModelInfo::stStackerOcr& stStackerModel = CModelInfo::Instance()->GetStackerOcr();
	m_pInspectVision->EVisionOCR_StackerOCR(pGO, pImgObj, IDX_AREA1, stTrayData, stStackerModel, bSave);
#endif

	return TRUE;
}

BOOL CVisionSystem::OCRMeasureInspection(CxGraphicObject* pGO, CxImageObject* pImgObj, VisionProcess::ChipOCRData& stChipData, BOOL bSave /*=FALSE*/)
{
#ifdef RELEASE_1G
	CModelInfo::stChipOcr& stChipModel = CModelInfo::Instance()->GetChipOcr();
	m_pInspectVision->EVisionOCR_ChipOCR(pGO, pImgObj, stChipData, stChipModel.rcInspArea, stChipModel.strChipOcrInfo, IDX_AREA4, bSave);

#endif
	return TRUE;
}

BOOL CVisionSystem::OCRMeasureInspection(EImageBW8 SegmentImage, VisionProcess::LabelOCRData& LabelData, std::vector<READ_DATA_Label> &MasterData, CString strFontPatch, VisionProcess::SegmentReadingOption stReadingOption)
{
	BOOL bResultOK = m_pInspectVision->EVisionOCRRead(SegmentImage, LabelData, MasterData, strFontPatch, stReadingOption);
	return bResultOK;
}

void CVisionSystem::OCRImagePreview(CxImageObject* pImgObj)
{
	m_pInspectVision->OCRImagePreview(pImgObj);
}

void CVisionSystem::OCRImagePreview_stacker(CxImageObject* pImgObj)
{
	m_pInspectVision->OCRImagePreview_stacker(pImgObj);
}

BOOL CVisionSystem::CheckExistDrive(CString strDriveIndex)
{
	strDriveIndex += _T(":");

	if( GetDriveType( strDriveIndex ) == DRIVE_FIXED )
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////
#include <Open_eVision_2_11.h>
BOOL CVisionSystem::TestEvisionDongle()
{
	//using namespace Euresys::Open_eVision_2_11;
	try
	{
		Easy::CheckLicenses();
	}
	catch (EException& e)
	{
		e.what();
		m_bIsValidEvisionDongle = FALSE;
		return FALSE;
	}
	m_bIsValidEvisionDongle = TRUE;
	return TRUE;
}

void CVisionSystem::GetMetrixInfoCount( int nGrabIndex, int &nScanColCnt, int &nScanRowCnt, int &nInspCol, int &nInspRow, int &nResultMatrix_OffsetX, int &nResultMatrix_OffsetY )
{
	// X 좌표는 Row를, Y 좌표는 Col을 의미
	// Row = X 좌표
	// Col = Y 좌표

	CModelInfo::stChip& sChip = CModelInfo::Instance()->GetChip();

	// ----- 촬영 횟수 -----
	// ----- X = Row → -----
	int nTrayMatrix_X = sChip.nMatrix_X;  // X 좌표 (Row 수)
	int nChipRow = sChip.nChipFovCnt_X;   // X 방향 칩 FOV 개수

	int nNumberOfShoots_X = (int)floor(nTrayMatrix_X / nChipRow);
	if (nTrayMatrix_X % nChipRow != 0) nNumberOfShoots_X++;
	nScanRowCnt = nNumberOfShoots_X;  // Row count (X)

	// ----- Y = Col ↓ -----
	int nTrayMatrix_Y = sChip.nMatrix_Y;  // Y 좌표 (Col 수)
	int nChipCol = sChip.nChipFovCnt_Y;   // Y 방향 칩 FOV 개수

	int nNumberOfShoots_Y = (int)floor(nTrayMatrix_Y / nChipCol);
	if (nTrayMatrix_Y % nChipCol != 0) nNumberOfShoots_Y++;
	nScanColCnt = nNumberOfShoots_Y;  // Col count (Y)

	// ----- Scan 위치 파악 -----
	int nScanLine_Col, nScanLine_Row;
	nScanLine_Row = (int)floor(nGrabIndex / nNumberOfShoots_Y);  // Row 계산 (X)
	if (nScanLine_Row % 2 == 0)
		nScanLine_Col = nGrabIndex - (nNumberOfShoots_Y * nScanLine_Row);  // Column 계산 (Y)
	else
		nScanLine_Col = (nNumberOfShoots_Y * (nScanLine_Row + 1)) - (nGrabIndex + 1);

	// ----- Insp Row Col (좌표계 비교시 X, Y 반전) -----
	if (nNumberOfShoots_X - 1 == nScanLine_Row || nNumberOfShoots_Y - 1 == nScanLine_Col)
	{
		if (nNumberOfShoots_X - 1 == nScanLine_Row)
			nInspRow = sChip.nMatrix_X - (nScanLine_Row * sChip.nChipFovCnt_X);  // X 좌표 (Row)
		else
			nInspRow = sChip.nChipFovCnt_X;

		if (nNumberOfShoots_Y - 1 == nScanLine_Col)
			nInspCol = sChip.nMatrix_Y - (nScanLine_Col * sChip.nChipFovCnt_Y);  // Y 좌표 (Col)
		else
			nInspCol = sChip.nChipFovCnt_Y;
	}
	else
	{
		nInspRow = sChip.nChipFovCnt_X;  // X 좌표 (Row)
		nInspCol = sChip.nChipFovCnt_Y;  // Y 좌표 (Col)
	}

	// ----- Insp Start Offset -----
	nResultMatrix_OffsetX = sChip.nChipFovCnt_X * nScanLine_Row;  // X 좌표 (Row)
	nResultMatrix_OffsetY = sChip.nChipFovCnt_Y * nScanLine_Col;  // Y 좌표 (Col)
}

void CVisionSystem::GetMetrixInfoGrab( int &nGrabIndex, int &nImgCol, int &nImgRow, int &OffsetX, int &OffsetY, int nCol, int nRow  )
{
	CModelInfo::stChip& m_stChip = CModelInfo::Instance()->GetChip();

	int nGrabCntX = m_stChip.nMatrix_X / m_stChip.nChipFovCnt_X;
	if( m_stChip.nMatrix_X % m_stChip.nChipFovCnt_X != 0 ) nGrabCntX += 1;

	int nGrabCntY = m_stChip.nMatrix_Y / m_stChip.nChipFovCnt_Y;
	if( m_stChip.nMatrix_Y % m_stChip.nChipFovCnt_Y != 0 ) nGrabCntY += 1;

	int nGrabNum = 0;
	int nMaxCol = 0, nMaxRow = 0;
	for(int nGrabX = 0; nGrabX < nGrabCntX; nGrabX++)
	{
		for(int nGrabY = 0; nGrabY < nGrabCntY; nGrabY++)
		{
			int nScanColCnt = 0, nScanRowCnt = 0, nInspCol = 0, nInspRow = 0, nResultMatrix_OffsetX = 0, nResultMatrix_OffsetY = 0;
			GetMetrixInfoCount( nGrabNum, nScanColCnt, nScanRowCnt, nInspCol, nInspRow, nResultMatrix_OffsetX, nResultMatrix_OffsetY );

			if( (nResultMatrix_OffsetX <= nRow && nRow < nResultMatrix_OffsetX + nInspRow)	&&
				(nResultMatrix_OffsetY <= nCol && nCol < nResultMatrix_OffsetY + nInspCol)	)
			{
				nGrabIndex	= nGrabNum;
				nImgCol		= nInspCol;
				nImgRow		= nInspRow;
				OffsetX		= nResultMatrix_OffsetX;
				OffsetY		= nResultMatrix_OffsetY;
				break;
			}
			else nGrabNum++;
		}
	}
}

void CVisionSystem::SetChipImage(int nGrabIndex)
{
#ifdef RELEASE_1G
	if (m_ImageObjectAreaChipCnt[nGrabIndex].IsValid())
	{
		m_ImageObjectArea[IDX_AREA4].Clone(&m_ImageObjectAreaChipCnt[nGrabIndex]);
		
		m_pMainView->GetImageView(IDX_AREA4)->GetGraphicObject().Reset();
		m_pMainView->GetImageView(IDX_AREA4)->ImageUpdate();
	}
#endif
}

BOOL CVisionSystem::Result_OK( int nViewIndex, InspectType inspecttype )
{
	BOOL bOK = FALSE;
#ifdef RELEASE_1G
	if (inspecttype == InspectTypeLiftFront || inspecttype == InspectTypeLiftRear)
	{
		bOK = (RESULT_OK(m_dwErrorCode[IDX_AREA6]) && RESULT_OK(m_dwErrorCode[IDX_AREA7]));
	}
	else
	{
		bOK = RESULT_OK(m_dwErrorCode[nViewIndex]);
	}
#else
	bOK = RESULT_OK(m_dwErrorCode[nViewIndex]);
#endif

	eBypass ebypass;
	switch( inspecttype )
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:		ebypass = (eBypass)CModelInfo::Instance()->GetTrayOcr().nUseBypass_TrayOcr;			break;
	case InspectType3DChipCnt:		ebypass = (eBypass)CModelInfo::Instance()->Get3DChipCnt().nUseBypass_3DChipCnt;
									if (ebypass != BYPASS_OFF)
										m_b3DChipCntResult = TRUE;
									break;
	case InspectTypeChipOcr:		ebypass = (eBypass)CModelInfo::Instance()->GetChipOcr().nUseBypass_ChipOcr;			break;
	case InspectTypeChip:			ebypass = (eBypass)CModelInfo::Instance()->GetChip().nUseBypass_Chip;
									if(ebypass != BYPASS_OFF)
										m_bChipCntResult = TRUE;
									break;
	case InspectTypeMixing:			ebypass = (eBypass)CModelInfo::Instance()->GetMixing().nUseBypass_Mixing;			break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:		ebypass = (eBypass)CModelInfo::Instance()->GetLiftInfo().nUseBypass_Lift;			break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:		ebypass = eBypass::BYPASS_OFF;														break;

#elif RELEASE_2G
	case InspectTypeBanding:		ebypass = (eBypass)CModelInfo::Instance()->GetBandingInfo().nUseBypass_Banding;		break;
	case InspectTypeHIC:			ebypass = (eBypass)CModelInfo::Instance()->GetHICInfo().nUseBypass_HIC;				break;

#elif RELEASE_3G
	case InspectTypePositionLeft:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionLeft;		break;
	case InspectTypePositionRight:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionRight;	break;
	case InspectTypeDesiccantLeft:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingLeft;		break;
	case InspectTypeDesiccantRight:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingRight;		break;
	case InspectTypeMaterial:		ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantMaterialInfo().nUseBypass_Material;					break;

#elif RELEASE_4G
	case InspectTypeMBB:			ebypass = (eBypass)CModelInfo::Instance()->GetMBBInfo().nUseBypass_MBB;				break;
	case InspectTypeLabel:			ebypass = (eBypass)CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label;			break;

#elif RELEASE_5G
	case InspectTypeBox:			ebypass = (eBypass)CModelInfo::Instance()->GetBoxInfo().nUseBypass_Box;				break;
	case InspectTypeSealing:		ebypass = (eBypass)CModelInfo::Instance()->GetSealingInfo().nUseBypass_Sealing;		break;

#elif RELEASE_6G
	case InspectTypeLabel:			ebypass = (eBypass)CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label;			break;
	case InspectTypeTape:			ebypass = (eBypass)CModelInfo::Instance()->GetTapeInfo().nUseBypass_Tape;			break;

#endif
	default:
		break;
	}

	// BYPASS_OFF=0, BYPASS_ON, BYPASS_ON_INSP
	if( ebypass != BYPASS_OFF ) bOK = TRUE;

	// Bypass Mode One Pass Mode
	if( ebypass == BYPASS_ON_INSP ) 
	{
		SetBypassMode(inspecttype, BYPASS_OFF);		
	}

	return bOK;
}

BOOL CVisionSystem::ResultOKNG_MMI( InspectType inspecttype, BOOL bOK )
{
	if(GetRunStatus() == RunStatusStop) return TRUE;

	WRITE_LOG( WL_MSG, _T("ResultOKNG_MMI[%s] : %s"), ForLog(inspecttype), bOK?_T("OK") : _T("NG") );
	
	//////////////////////////////////////////////////////////////////////////
	// Auto Calibration
	BOOL bAcm = FALSE;
	for (int i = 0; i < ACM_MAX; ++i)
	{
		bAcm |= m_bAcm[i];

		if (m_bAcm[i])
		{
			m_bAcm[i] = FALSE;
			LightOnOff(inspecttype, FALSE);
		}
	}
	//////////////////////////////////////////////////////////////////////////

	CString strVisionCode = _T("00");
	CString strCommandCode = bAcm ? _T("ACM") : _T("VIE");
	CString strSendBuf = _T("");
	int nLength = 0;
	int nPort = SOCKET_MAX;

	switch( inspecttype )
	{
#ifdef RELEASE_SG
	case InspectTypeStackerOcr:			strVisionCode = _T("10");	nPort = SOCKET_STACKEROCR;	break;
	
#elif RELEASE_1G
	case InspectTypeTrayOcr:			strVisionCode = _T("11");	nPort = SOCKET_TRAYOCR;		break;
	case InspectType3DChipCnt:			strVisionCode = _T("12");	nPort = SOCKET_3DCHIPCNT;	break;
	case InspectTypeChipOcr:			strVisionCode = _T("13");	nPort = SOCKET_CHIPOCR;		break;
	case InspectTypeChip:				strVisionCode = _T("14");	nPort = SOCKET_CHIPCNT;		break;
	case InspectTypeMixing:				strVisionCode = _T("15");	nPort = SOCKET_MIXING;		break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:			strVisionCode = _T("16");	nPort = SOCKET_LIFT;		break;

#elif RELEASE_2G
	case InspectTypeBanding:			strVisionCode = _T("21");	nPort = SOCKET_BANDING;		break;
	case InspectTypeHIC:				strVisionCode = _T("22");	nPort = SOCKET_HIC;			break;

#elif RELEASE_3G
	case InspectTypePositionRight:
	case InspectTypeDesiccantRight:		strVisionCode = _T("31");	nPort = SOCKET_CUTTING1;	break;
	case InspectTypePositionLeft:
	case InspectTypeDesiccantLeft:		strVisionCode = _T("32");	nPort = SOCKET_CUTTING2;	break;
	case InspectTypeMaterial:			strVisionCode = _T("33");	nPort = SOCKET_MATERIAL;	break;
	//case InspectTypeMaterialTray:		

#elif RELEASE_4G
	case InspectTypeMBB:				strVisionCode = _T("41");	nPort = SOCKET_MBB;			break;
	case InspectTypeLabel:				strVisionCode = _T("42");	nPort = SOCKET_MBB_LABEL;	break;

#elif RELEASE_5G
	case InspectTypeBox:				strVisionCode = _T("51");	nPort = SOCKET_BOX;			break;
	case InspectTypeSealing:			strVisionCode = _T("52");	nPort = SOCKET_SEALING;		break;

#elif RELEASE_6G
	case InspectTypeLabel:				strVisionCode = _T("61");	nPort = SOCKET_BOX_LABEL;	break;
	case InspectTypeTape:				strVisionCode = _T("62");	nPort = SOCKET_TAPE;		break;

#endif
	default:
		break;
	}

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	CString strLotID = _T("LOTTESTDATA"); // 추후 검사 Data 받고

	if( (inspecttype == InspectTypeLabel) && bOK)
	{
		strSendBuf.Format(_T("%s%sSIZE|%d|%s|^"), strVisionCode, strCommandCode, bOK, strLotID);
		CStringA strA(strSendBuf);
		nLength = strA.GetLength();
		strSendBuf.Format(_T("%s%s%04d|%d|%s|^"), strVisionCode, strCommandCode, nLength, bOK, strLotID);
	}
	else
	{
		strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, bOK);
		CStringA strA(strSendBuf);
		nLength = strA.GetLength();
		strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, bOK);
	}

#elif RELEASE_3G
	float fLength = 0.f; // 추후 검사 Data 받고
	if (inspecttype == InspectTypePositionLeft)					fLength = m_fDesiPosDistLeft; 
	else if (inspecttype == InspectTypePositionRight)			fLength = m_fDesiPosDistRight;
	
	BOOL bNgRetry =	FALSE;
	if (inspecttype == InspectTypePositionLeft)					bNgRetry = m_bNgRetryLeft;
	else if (inspecttype == InspectTypePositionRight)			bNgRetry = m_bNgRetryRight;

	if ((inspecttype == InspectTypePositionLeft) || (inspecttype == InspectTypePositionRight)) 
	{
		if (bOK == TRUE)
		{
			// | 결과 | 종류 | 길이 |
			strSendBuf.Format(_T("%s%sSIZE|%d|0|%.3f|^"), strVisionCode, strCommandCode, bOK, fLength);
			CStringA strA(strSendBuf);
			nLength = strA.GetLength();
			strSendBuf.Format(_T("%s%s%04d|%d|0|%.3f|^"), strVisionCode, strCommandCode, nLength, bOK, fLength);
		} 
		else
		{
			// | 결과 | 종류 | Retry |
			strSendBuf.Format(_T("%s%sSIZE|%d|0|%d|^"), strVisionCode, strCommandCode, bOK, bNgRetry);
			CStringA strA(strSendBuf);
			nLength = strA.GetLength();
			strSendBuf.Format(_T("%s%s%04d|%d|0|%d|^"), strVisionCode, strCommandCode, nLength, bOK, bNgRetry);
		}
	}
	else if ((inspecttype == InspectTypeDesiccantLeft) || (inspecttype == InspectTypeDesiccantRight))
	{
		strSendBuf.Format(_T("%s%sSIZE|%d|1|^"), strVisionCode, strCommandCode, bOK);
		CStringA strA(strSendBuf);
		nLength = strA.GetLength();
		strSendBuf.Format(_T("%s%s%04d|%d|1|^"), strVisionCode, strCommandCode, nLength, bOK);
	}
	else
	{
		strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, bOK);
		CStringA strA(strSendBuf);
		nLength = strA.GetLength();
		strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, bOK);
	}
#elif RELEASE_2G

	if (inspecttype == InspectTypeHIC)
	{
		if (bOK == FALSE)
		{
			switch (m_nHICNgKind)
			{
			case HIC_DISCOLOR_ERROR:
			{
				strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, bOK);
				CStringA strA(strSendBuf);
				nLength = strA.GetLength();
				strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, bOK); // ex) 22VIE0013|0|^, HIC 원형 변색
				break;
			}
			case HIC_CIRCLE_FINDING_ERROR:
			{
				strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, 2);
				CStringA strA(strSendBuf);
				nLength = strA.GetLength();
				strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, 2); // ex) 22VIE0013|2|^, HIC 잘못 꺼냈을 때
				break;
			}
			}
		}
		else
		{
			strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, bOK);
			CStringA strA(strSendBuf);
			nLength = strA.GetLength();
			strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, bOK);
		}
	}

	else
	{
		strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, bOK);
		CStringA strA(strSendBuf);
		nLength = strA.GetLength();
		strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, bOK);
	}

#elif RELEASE_SG
	CString strStackerLotID = _T("LOTTESTDATA"); // 추후 검사 Data 받고

	strSendBuf.Format(_T("%s%sSIZE|%s|^"), strVisionCode, strCommandCode, strStackerLotID);
	CStringA strA(strSendBuf);
	nLength = strA.GetLength();
	strSendBuf.Format(_T("%s%s%04d|%s|^"), strVisionCode, strCommandCode, nLength, strStackerLotID);

#else
	strSendBuf.Format(_T("%s%sSIZE|%d|^"), strVisionCode, strCommandCode, bOK);
	CStringA strA(strSendBuf);
	nLength = strA.GetLength();
	strSendBuf.Format(_T("%s%s%04d|%d|^"), strVisionCode, strCommandCode, nLength, bOK);
	
#endif

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);

	return TRUE; //bSendOK;
}

BOOL CVisionSystem::SetBypassMode( InspectType inspecttype, eBypass eBypassMode )
{
	CString strLogData;
	switch( inspecttype )
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetTrayOcr().nUseBypass_TrayOcr], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetTrayOcr().nUseBypass_TrayOcr = eBypassMode; break;
		break;
	case InspectType3DChipCnt:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->Get3DChipCnt().nUseBypass_3DChipCnt], strBypassName[eBypassMode]);
		CModelInfo::Instance()->Get3DChipCnt().nUseBypass_3DChipCnt = eBypassMode; break;
		break;
	case InspectTypeChipOcr:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetChipOcr().nUseBypass_ChipOcr], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetChipOcr().nUseBypass_ChipOcr = eBypassMode; break;
	case InspectTypeChip:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetChip().nUseBypass_Chip], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetChip().nUseBypass_Chip = eBypassMode; break;
	case InspectTypeMixing:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetMixing().nUseBypass_Mixing], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetMixing().nUseBypass_Mixing = eBypassMode; break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetLiftInfo().nUseBypass_Lift], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetLiftInfo().nUseBypass_Lift = eBypassMode; break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:
		strLogData = _T("");
		break;

#elif RELEASE_2G
	case InspectTypeBanding:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetBandingInfo().nUseBypass_Banding], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetBandingInfo().nUseBypass_Banding = eBypassMode; break;
	case InspectTypeHIC:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetHICInfo().nUseBypass_HIC], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetHICInfo().nUseBypass_HIC = eBypassMode; break;

#elif RELEASE_3G
	case InspectTypePositionLeft:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionLeft], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionLeft = eBypassMode; break;
	case InspectTypePositionRight:
		strLogData.Format(_T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionRight], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionRight = eBypassMode; break;
	case InspectTypeDesiccantLeft:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingLeft], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingLeft = eBypassMode; break;
	case InspectTypeDesiccantRight:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingRight], strBypassName[eBypassMode]);
		CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingRight = eBypassMode; break;
	case InspectTypeMaterial:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetDesiccantMaterialInfo().nUseBypass_Material], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetDesiccantMaterialInfo().nUseBypass_Material = eBypassMode; break;	

#elif RELEASE_4G
	case InspectTypeMBB:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetMBBInfo().nUseBypass_MBB], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetMBBInfo().nUseBypass_MBB = eBypassMode; break;
	case InspectTypeLabel:
 		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label], strBypassName[eBypassMode]);			
 		CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label = eBypassMode;	break;

#elif RELEASE_5G
	case InspectTypeBox:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetBoxInfo().nUseBypass_Box], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetBoxInfo().nUseBypass_Box = eBypassMode; break;
	case InspectTypeSealing:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetSealingInfo().nUseBypass_Sealing], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetSealingInfo().nUseBypass_Sealing = eBypassMode; break;

#elif RELEASE_6G
	case InspectTypeLabel:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label = eBypassMode; 	break;
	case InspectTypeTape:
		strLogData.Format( _T("[Bypass][%s→%s]"), strBypassName[CModelInfo::Instance()->GetTapeInfo().nUseBypass_Tape], strBypassName[eBypassMode]);			
		CModelInfo::Instance()->GetTapeInfo().nUseBypass_Tape = eBypassMode; 	break;

#endif
	default:
		break;
	}

	WriteLogforTeaching( inspecttype, strLogData );
	CModelInfo::Instance()->SaveBypassMode( inspecttype );

	return TRUE;
}

void CVisionSystem::WriteLogforTeaching( InspectType InspType, LPCTSTR lpszFormat, ... )
{
	if(lpszFormat == NULL)
		return;

	LPCTSTR lpszInspTap;
	switch (InspType)
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:					lpszInspTap = _T("[TrayOcr]");		break;
	case InspectType3DChipCnt:					lpszInspTap = _T("[3DChipCnt]");	break;
	case InspectTypeChipOcr:					lpszInspTap = _T("[ChipOcr]");		break;
	case InspectTypeChip:						lpszInspTap = _T("[Chip]");			break;
	case InspectTypeMixing:						lpszInspTap = _T("[Mixing]");		break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:					lpszInspTap = _T("[Lift]");			break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:					lpszInspTap = _T("[StackerOcr]");	break;

#elif RELEASE_2G
	case InspectTypeBanding:					lpszInspTap = _T("[Banding]");		break;
	case InspectTypeHIC:						lpszInspTap = _T("[HIC]");			break;

#elif RELEASE_3G
	case InspectTypePositionLeft:				lpszInspTap = _T("[PositionLeft]");		break;
	case InspectTypePositionRight:				lpszInspTap = _T("[PositionRight]");	break;
	case InspectTypeDesiccantLeft:				lpszInspTap = _T("[DesiccantLeft]");	break;
	case InspectTypeDesiccantRight:				lpszInspTap = _T("[DesiccantRight]");	break;
	case InspectTypeMaterial:					lpszInspTap = _T("[Material]");			break;

#elif RELEASE_4G
	case InspectTypeMBB:						lpszInspTap = _T("[MBB]");			break;
	case InspectTypeLabel:						lpszInspTap = _T("[Label]");		break;

#elif RELEASE_5G
	case InspectTypeBox:						lpszInspTap = _T("[Box]");			break;
	case InspectTypeSealing:					lpszInspTap = _T("[Sealing]");		break;

#elif RELEASE_6G
	case InspectTypeLabel:						lpszInspTap = _T("[Label]");		break;
	case InspectTypeTape:						lpszInspTap = _T("[Tape]");			break;

#endif

	default:									lpszInspTap = _T("[Error]");		break;
	}

	CString strLog;
	strLog.Format(_T("%s%s"), lpszInspTap, lpszFormat);

	va_list argList;
	va_start(argList, strLog);

	CxLogMsgCache* pMsgCache = CxLogMsgCache::Instance();
	TCHAR* lptszBuffer = pMsgCache->GetMsgString();
	VERIFY(_vsntprintf_s(lptszBuffer, pMsgCache->GetMsgBufferSize(), _TRUNCATE, strLog, argList) <= pMsgCache->GetMsgBufferSize());

	va_end(argList);

	::PostMessage(m_hWndUI, WM_LOG_MESSAGE, WPARAM(LogTypeNormal), LPARAM(lptszBuffer));

	WRITE_LOG(WL_TEACHING, lptszBuffer);

	TRACE(CString(lptszBuffer)+_T("\n"));
}

void CVisionSystem::LoadLabelData_Manual( BOOL bReset, CString strPath /*= _T("")*/ )
{
	m_stLabelData_Manual.		clear();	
	strLabelData_Manual_Path.	Empty();

	if( bReset ) 
	{
		m_pMainView->UpdateUI();
		return;
	}

	if( !IsExistFile((LPCTSTR)strPath) ) return;
	strLabelData_Manual_Path = strPath;

	TCHAR szTemp[10000];
	CString strItem;

	READ_DATA_Label stTemp;
	for (int nIndex = 0;;nIndex++)
	{	
		CString strApp;
		strApp.Format(_T("ZPL_DATA_%d"), nIndex);

		stTemp.clear();

		::GetPrivateProfileString( strApp, _T("CODE"), _T("None"), szTemp, MAX_PATH, strPath );		
		stTemp.strCode = szTemp;
		if(stTemp.strCode == _T("None")) break;

		::GetPrivateProfileString( strApp, _T("DATA_TYPE"), _T("0"), szTemp, MAX_PATH, strPath );
		switch ((int)_tcstod( szTemp, NULL ))
		{
		case READ_DATA_Label::OTHER_INFO:			stTemp.DataType = READ_DATA_Label::OTHER_INFO;				break;
		case READ_DATA_Label::TEXT_SEGMENT:			stTemp.DataType = READ_DATA_Label::TEXT_SEGMENT;			break;
		case READ_DATA_Label::BARCODE_SEGMENT:		stTemp.DataType = READ_DATA_Label::BARCODE_SEGMENT;			break;	
		case READ_DATA_Label::BARCODE_TEXT_SEGMENT:	stTemp.DataType = READ_DATA_Label::BARCODE_TEXT_SEGMENT;	break;
		}

		::GetPrivateProfileString( strApp, _T("STRING"), _T("None"), szTemp, MAX_PATH, strPath );
		stTemp.strString = szTemp;
		if(stTemp.strString == _T("None")) break;

		::GetPrivateProfileString( strApp, _T("POSITION_X"), _T("0"), szTemp, MAX_PATH, strPath );
		stTemp.cPosition.x = (int)_tcstod( szTemp, NULL );
		::GetPrivateProfileString( strApp, _T("POSITION_Y"), _T("0"), szTemp, MAX_PATH, strPath );
		stTemp.cPosition.y = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strApp, _T("SIZE_W"), _T("0"), szTemp, MAX_PATH, strPath );
		stTemp.cSize.cx = (int)_tcstod( szTemp, NULL );
		::GetPrivateProfileString( strApp, _T("SIZE_H"), _T("0"), szTemp, MAX_PATH, strPath );
		stTemp.cSize.cy = (int)_tcstod( szTemp, NULL );

		m_stLabelData_Manual.push_back(stTemp);

		if( 1000 < nIndex ) break;
	}

	m_pMainView->UpdateUI();
}

int CVisionSystem::TimeDelay(DWORD dwTimeout)
{
#define	SYSTIME_PER_MSEC	10000
	FILETIME f;
	::GetSystemTimeAsFileTime(&f);
	DWORD tmStart = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);

	MSG msg;
	BOOL bStatus = FALSE;
	while (!bStatus)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		::GetSystemTimeAsFileTime(&f);
		DWORD dwCurTimeCheck = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);

		if( (dwCurTimeCheck - tmStart) > dwTimeout )
		{
			return 0;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CVisionSystem::OnPacketReceived( ClientThreadContext* pContext, CxIoBuffer* pPacketBuffer )
{
}

void CVisionSystem::ProcessBuffer( ServerThreadContext* pContext, char* pReceiveBuf, int nRecvBytes )
{
	CString strReceiveBuf, strSendBuf, strLog;
	strReceiveBuf = pReceiveBuf;
	strSendBuf = strLog = _T("");
	BOOL bSendBuf = TRUE;

	int nStringLength;
	nStringLength = static_cast<int>(strlen(pReceiveBuf));

	if( nRecvBytes < nStringLength )
	{
		strReceiveBuf = strReceiveBuf.Left(nRecvBytes);	
		nStringLength = strReceiveBuf.GetLength();
	}
	else if(nRecvBytes > nStringLength) 
		nRecvBytes = strReceiveBuf.GetLength();

#ifndef DEBUG
	if( GetRunStatus() != RunStatusAutoRun ) return;
#endif
	WriteMessage(LogTypeNormal, _T("[Packet Receive] : [%d][%s]"), nRecvBytes, strReceiveBuf);	
	if( nStringLength < 9 ) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	CString strVisionCode	= strReceiveBuf.Mid(0,2);
	CString strCommandCode	= strReceiveBuf.Mid(2,3);
	CString strLength		= strReceiveBuf.Mid(5,4);

	BOOL bOK = TRUE;
	if( nStringLength != _ttoi(strLength)	)	bOK = FALSE; // Handler 에서 보내준 정보
	if( nStringLength != nRecvBytes			)	bOK = FALSE; // Socket에서 받은정보 확인
	if( !bOK ) WriteMessage(LogTypeError, _T("[Packet Bytes: %d] [String Length: %d] [Handler Data: %d]"), nRecvBytes, nStringLength, _ttoi(strLength));	

	VisionInterface::FrameGrabber& FrameGrabber = CVisionSystem::Instance()->GetFrameGrabberInterface();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if( strCommandCode == _T("SST") ) // 11SST0026|20210625112936|^
	{
		WriteMessage(LogTypeNormal, _T("SST [Receive] : %s"), strReceiveBuf);

		CString strTimeData;
		AfxExtractSubString(strTimeData, strReceiveBuf, 1, '|');	
		SetSystemTime(strTimeData);
 	}

#ifdef RELEASE_1G
	else if (strCommandCode == _T("ISS")) // 11ISS0000|LOTNO004ATK3|000|^
	{
		WriteMessage(LogTypeNormal, _T("ISS [Receive] : %s"), strReceiveBuf);

		CString strLotID, strBoxNo;

		AfxExtractSubString(strLotID, strReceiveBuf, 1, '|');
		AfxExtractSubString(strBoxNo, strReceiveBuf, 2, '|');

		SetLotID(InspectTypeTrayOcr, strLotID);
		SetBoxNo(InspectTypeTrayOcr, _ttoi(strBoxNo));

		SaveTrayOcr();
	}
#endif

	////////////////////// RBD //////////////////////
#ifndef RELEASE_SG
	else if (strCommandCode == _T("RBD")) // 12RBD0099|LOTNO004ATK3|000|^
	{
		WriteMessage(LogTypeNormal, _T("RBD [Receive] : %s"), strReceiveBuf);
		int nIndex = 0;
#ifdef RELEASE_1G
		nIndex = 1;
#elif RELEASE_2G
		nIndex = 2;
#elif RELEASE_3G
		nIndex = 3;
#elif RELEASE_4G
		nIndex = 4;
#elif RELEASE_5G
		nIndex = 5;
#elif RELEASE_6G
		nIndex = 6;
#endif
		ReworkBoxData(strReceiveBuf, nIndex);
//		RemoveBoxData(strReceiveBuf, nIndex);
	}
#endif // !RELEASE_SG

	////////////////////// DES //////////////////////
	else if (strCommandCode == _T("DES")) // 12DES0046|101320800,FH33780|LOTNO004ATK3|000|^
	{
		WriteMessage(LogTypeNormal, _T("DES [Receive] : %s"), strReceiveBuf);
#ifdef RELEASE_SG
		DES201S(strReceiveBuf, strVisionCode);
#elif RELEASE_1G
		DES201(strReceiveBuf, strVisionCode);
#elif RELEASE_2G
		DES202(strReceiveBuf, strVisionCode);
#elif RELEASE_3G
		DES203(strReceiveBuf, strVisionCode);
#elif RELEASE_4G
		DES204(strReceiveBuf, strVisionCode);
#elif RELEASE_5G
		DES205(strReceiveBuf, strVisionCode);
#elif RELEASE_6G
		DES207(strReceiveBuf, strVisionCode);
#endif
	}

	////////////////////// VIS //////////////////////
	else if (strCommandCode == _T("VIS")) // 12VIS0016|0600|^
	{
		WriteMessage(LogTypeNormal, _T("VIS [Receive] : %s"), strReceiveBuf);
#ifdef RELEASE_SG
		VIS201S(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_1G
		VIS201(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_2G
		VIS202(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_3G
		VIS203(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_4G
		VIS204(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_5G
		VIS205(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_6G
		VIS207(strReceiveBuf, strVisionCode, FrameGrabber);
#endif
	}

	else if (strCommandCode == _T("DEE")) // 51DEE0013|1|^
	{
		WriteMessage(LogTypeNormal, _T("DEE [Receive] : %s"), strReceiveBuf);
#ifdef RELEASE_SG
		DEE201S(strReceiveBuf, strVisionCode);
#elif RELEASE_1G
		DEE201(strReceiveBuf, strVisionCode);
#elif RELEASE_2G
		DEE202(strReceiveBuf, strVisionCode);
#elif RELEASE_3G
		DEE203(strReceiveBuf, strVisionCode);
#elif RELEASE_4G
		DEE204(strReceiveBuf, strVisionCode);
#elif RELEASE_5G
		DEE205(strReceiveBuf, strVisionCode);
#elif RELEASE_6G
		DEE207(strReceiveBuf, strVisionCode);
#endif
	}

	else if (strCommandCode == _T("ACM")) // 11ACM0013|1|^
	{
	WriteMessage(LogTypeNormal, _T("ACM [Receive] : %s"), strReceiveBuf);
#ifdef RELEASE_1G
	ACM201(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_3G
	ACM203(strReceiveBuf, strVisionCode, FrameGrabber);
#elif RELEASE_4G
	ACM204(strReceiveBuf, strVisionCode, FrameGrabber);
#endif
	}
	////////////////////// VIE //////////////////////
	else if (strCommandCode == _T("VIE"))
	{
		WriteMessage(LogTypeNormal, _T("VIE [Receive] : %s"), strReceiveBuf);
	}
}

//////////////////////////////////////////////////
void CVisionSystem::SetSystemTime(CString strTimeSink)
{
	if(strTimeSink.GetLength() != 14) 
	{
		WriteMessage( LogTypeError, _T("System - [TIME Data Error : %s]"), strTimeSink );
		return;
	}

	SYSTEMTIME Time_System;
	SYSTEMTIME Time_Sink;

	GetLocalTime(&Time_System);
	Time_Sink = Time_System;

	CString strBuf, strYear, strMonth, strDay, strHour, strMinute, strSecond;
	for(int nIndex = 0; nIndex < strTimeSink.GetLength(); nIndex++)
	{	
		strBuf = strTimeSink.GetAt(nIndex);

		if		(nIndex <  4)	strYear		+= strBuf; // 0 1 2 3
		else if (nIndex <  6)	strMonth	+= strBuf; // 4 5
		else if (nIndex <  8)	strDay		+= strBuf; // 6 7
		else if (nIndex < 10)	strHour		+= strBuf; // 8 9
		else if (nIndex < 12)	strMinute	+= strBuf; // 10 11
		else if (nIndex < 14)	strSecond	+= strBuf; // 12 13
	}

	Time_Sink.wYear		= (WORD)_ttoi(strYear	);
	Time_Sink.wMonth	= (WORD)_ttoi(strMonth	);
	Time_Sink.wDay		= (WORD)_ttoi(strDay	);
	Time_Sink.wHour		= (WORD)_ttoi(strHour	);
	Time_Sink.wMinute	= (WORD)_ttoi(strMinute	);
	Time_Sink.wSecond	= (WORD)_ttoi(strSecond	);

	SetLocalTime(&Time_Sink);
	WriteMessage(LogTypeNotification, _T("System - Set Time Sink Success [%s/%s/%s %s:%s:%s]"), strYear, strMonth, strDay, strHour, strMinute, strSecond);
}

void CVisionSystem::CameraExposureChange(CameraType camType, double dExposure)
{
	m_FrameGrabberInterface.ChangeExposure(camType, dExposure);
}

void CVisionSystem::CameraOnOff(InspectType inspecttype, BOOL bOn)
{
	WRITE_LOG(WL_MSG, _T("CameraOnOff[%s] :: %s"), ForLog(inspecttype), bOn ? _T("On") : _T("Off"));

	if (bOn)
	{
		switch (inspecttype)
		{
#ifdef RELEASE_1G
		case InspectTypeTrayOcr:			m_FrameGrabberInterface.Grab(CameraTypeLine);			break;
		case InspectType3DChipCnt:			m_FrameGrabberInterface.Grab(CameraType3DArea);			break;
		case InspectTypeChipOcr:
		case InspectTypeChip:				m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypeMixing:				m_FrameGrabberInterface.Grab(CameraTypeLine2);			break;
		case InspectTypeLiftFront:			m_FrameGrabberInterface.Grab(CameraTypeLine3);			break;
		case InspectTypeLiftRear:			m_FrameGrabberInterface.Grab(CameraTypeLine4);			break;

#elif RELEASE_SG
		case InspectTypeStackerOcr:			m_FrameGrabberInterface.Grab(CameraTypeLine);			break;

#elif RELEASE_2G
		case InspectTypeBanding:			m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypeHIC:				m_FrameGrabberInterface.Grab(CameraTypeArea2);			break;

#elif RELEASE_3G
		case InspectTypePositionLeft:		m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypePositionRight:		m_FrameGrabberInterface.Grab(CameraTypeArea2);			break;
		case InspectTypeDesiccantLeft:		m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypeDesiccantRight:		m_FrameGrabberInterface.Grab(CameraTypeArea2);			break;
		case InspectTypeMaterial:			m_FrameGrabberInterface.Grab(CameraTypeArea3);			break;

#elif RELEASE_4G
		case InspectTypeMBB:				m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypeLabel:				m_FrameGrabberInterface.Grab(CameraTypeArea2);			break;

#elif RELEASE_5G
		case InspectTypeBox:				m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypeSealing:			m_FrameGrabberInterface.Grab(CameraTypeArea2);			break;

#elif RELEASE_6G
		case InspectTypeLabel:				m_FrameGrabberInterface.Grab(CameraTypeArea);			break;
		case InspectTypeTape:				m_FrameGrabberInterface.Grab(CameraTypeArea2);			break;

#endif
		default:
			break;
		}
	}
	else
	{
		switch (inspecttype)
		{
#ifdef RELEASE_1G
		case InspectTypeTrayOcr:			m_FrameGrabberInterface.Idle(CameraTypeLine);			break;
		case InspectType3DChipCnt:			m_FrameGrabberInterface.Idle(CameraType3DArea);			break;
		case InspectTypeChipOcr:
		case InspectTypeChip:				m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypeMixing:				m_FrameGrabberInterface.Idle(CameraTypeLine2);			break;
		case InspectTypeLiftFront:			m_FrameGrabberInterface.Idle(CameraTypeLine3);			break;
		case InspectTypeLiftRear:			m_FrameGrabberInterface.Idle(CameraTypeLine4);			break;

#elif RELEASE_SG
		case InspectTypeStackerOcr:			m_FrameGrabberInterface.Idle(CameraTypeLine);			break;

#elif RELEASE_2G
		case InspectTypeBanding:			m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypeHIC:				m_FrameGrabberInterface.Idle(CameraTypeArea2);			break;

#elif RELEASE_3G
		case InspectTypePositionLeft:		m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypePositionRight:		m_FrameGrabberInterface.Idle(CameraTypeArea2);			break;
		case InspectTypeDesiccantLeft:		m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypeDesiccantRight:		m_FrameGrabberInterface.Idle(CameraTypeArea2);			break;
		case InspectTypeMaterial:			m_FrameGrabberInterface.Idle(CameraTypeArea3);			break;

#elif RELEASE_4G
		case InspectTypeMBB:				m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypeLabel:				m_FrameGrabberInterface.Idle(CameraTypeArea2);			break;

#elif RELEASE_5G
		case InspectTypeBox:				m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypeSealing:			m_FrameGrabberInterface.Idle(CameraTypeArea2);			break;

#elif RELEASE_6G
		case InspectTypeLabel:				m_FrameGrabberInterface.Idle(CameraTypeArea);			break;
		case InspectTypeTape:				m_FrameGrabberInterface.Idle(CameraTypeArea2);			break;

#endif
		default:
			break;
		}
	}
}

void CVisionSystem::LightOnOff(InspectType inspecttype, BOOL bOn, int nGrabCnt)
{
	WRITE_LOG(WL_MSG, _T("LightOnOff[%s] :: %s"), ForLog(inspecttype), bOn ? _T("On") : _T("Off"));

	if (bOn)
	{
		CLightInfo::LightControllerLFine& LightCtrlAreaScanCofig = CLightInfo::Instance()->GetLightCtrlAreaScanConfig();

		int LightValue, LightValue2, LightValue3, LightValue4;
		LightValue = LightValue2 = LightValue3 = LightValue4 = 0;

		switch (inspecttype)
		{

#ifdef RELEASE_1G
		case InspectTypeChipOcr:
			LightValue = CModelInfo::Instance()->GetChipOcr().nValueCh1; // Flat Dome
			LightValue2 = CModelInfo::Instance()->GetChipOcr().nValueCh2; // Led
			LightValue3 = CModelInfo::Instance()->GetChipOcr().nValueCh3; // Led

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue2);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue3);
			break;

		case InspectTypeChip:
			LightValue = CModelInfo::Instance()->GetChip().nValueCh1; // Flat Dome
			LightValue2 = CModelInfo::Instance()->GetChip().nValueCh2; // Led
			LightValue3 = CModelInfo::Instance()->GetChip().nValueCh3; // Led

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue2);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue3);
			break;

#elif RELEASE_SG
		case InspectTypeStackerOcr:
			break;

#elif RELEASE_2G
		case InspectTypeBanding:
			LightValue = CModelInfo::Instance()->GetBandingInfo().nValueCh1; // Led

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			break;

		case InspectTypeHIC:
			LightValue = CModelInfo::Instance()->GetHICInfo().nValueCh1; // Led

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			break;

#elif RELEASE_3G
		case InspectTypePositionLeft: // Left
			LightValue = CModelInfo::Instance()->GetDesiccantCuttingInfo().nValueCh1; // Top

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);
			break;

		case InspectTypePositionRight: // Right
			LightValue = CModelInfo::Instance()->GetDesiccantCuttingInfo().nValueCh3; // Top

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2, LightValue);

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2, LightValue);
			break;

		case InspectTypeDesiccantLeft: // Left
			LightValue = CModelInfo::Instance()->GetDesiccantCuttingInfo().nValueCh2; // Back

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			break;

		case InspectTypeDesiccantRight: // Right
			LightValue = CModelInfo::Instance()->GetDesiccantCuttingInfo().nValueCh4; // Back

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			break;

		case InspectTypeMaterial:
			LightValue = CModelInfo::Instance()->GetDesiccantMaterialInfo().nValueCh1;
			LightValue2 = CModelInfo::Instance()->GetDesiccantMaterialInfo().nValueCh2;

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH2, LightValue2);
			break;

#elif RELEASE_4G
		case InspectTypeMBB:
			LightValue = CModelInfo::Instance()->GetMBBInfo().nValueCh1;

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);
			break;

		case InspectTypeLabel:
			LightValue = CModelInfo::Instance()->GetLabelInfo().nValueCh1;

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4, LightValue);
			break;

#elif RELEASE_5G
		case InspectTypeBox:
			LightValue = CModelInfo::Instance()->GetBoxInfo().nValueCh1;

			WRITE_LOG(WL_MSG, _T("LightValue [%s] :: %d"), ForLog(inspecttype), LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);
			break;

		case InspectTypeSealing:
			LightValue = CModelInfo::Instance()->GetSealingInfo().nValueCh1;

			WRITE_LOG(WL_MSG, _T("LightValue [%s] :: %d"), ForLog(inspecttype), LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4, LightValue);

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4, LightValue);
			break;

#elif RELEASE_6G
		case InspectTypeLabel:
			LightValue = CModelInfo::Instance()->GetLabelInfo().nValueCh1;

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1, LightValue);
			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2, LightValue);
			break;

		case InspectTypeTape:
			LightValue = CModelInfo::Instance()->GetTapeInfo().nValueCh1;

			m_LightControllerInterface.TurnOn(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3, LightValue);
			break;

#endif
		default:
			break;

		}

		TimeDelay(500);
	}
	else
	{
		switch (inspecttype)
		{
#ifdef RELEASE_1G
		case InspectTypeChipOcr:
		case InspectTypeChip:			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);		break;

#elif RELEASE_SG
		case InspectTypeStackerOcr:																								break;

#elif RELEASE_2G
		case InspectTypeBanding:		m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);		break;
		case InspectTypeHIC:			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);		break;

#elif RELEASE_3G
		case InspectTypePositionLeft:	m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);		break;
		case InspectTypePositionRight: 	m_LightControllerInterface.TurnOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH2);		break;
		case InspectTypeDesiccantLeft:	m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);		break;
		case InspectTypeDesiccantRight: m_LightControllerInterface.TurnOff(LedCtrlTypeArea2, USE_LFINE_LCP100_CTRL_CH3);		break;
		case InspectTypeMaterial:		m_LightControllerInterface.TurnOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH2);		break;

#elif RELEASE_4G
		case InspectTypeMBB: 			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);		break;
		case InspectTypeLabel:			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4);		break;

#elif RELEASE_5G
		case InspectTypeBox:			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2); 		break;
		case InspectTypeSealing:		m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH4); 		break;

#elif RELEASE_6G
		case InspectTypeLabel: 			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH1);
										m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH2);		break;
		case InspectTypeTape: 			m_LightControllerInterface.TurnOff(LedCtrlTypeArea1, USE_LFINE_LCP100_CTRL_CH3);		break;

#endif
		default:
			break;
		}
	}
}

void CVisionSystem::DongleConnet()
{
	// eVision
	if (!TestEvisionDongle())
	{
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("EvisionDongle Connect.. Failure"));
		WriteMessage(LogTypeError, _T("EvisionDongle Connect.. Failure"));
	}
	else
	{
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("EvisionDongle Connect.. OK"));
		WriteMessage(LogTypeNotification, _T("EvisionDongle Connect.. OK"));
	}

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	#ifndef _CodeUsb
	// Code
	if (m_Decoder.InitializeDecoder() == 0)
	{
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("CodeCorp Dongle Connect.. OK"));
		WriteMessage(LogTypeNotification, _T("CodeCorp Dongle Connect.. OK"));
	}
	else
	{
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("CodeCorp Dongle Connect.. Failure"));
		WriteMessage(LogTypeError, _T("CodeCorp Dongle Connect.. Failure"));
	}
	#endif
#endif
}

void CVisionSystem::CameraConnect()
{
#ifdef RELEASE_1G
	for (int i = 0; i < CameraTypeMax; ++i)
	{
		UINT nViewIndex;

		switch (i)
		{
		case CameraTypeArea:
			if (m_AreaScanCamera.Open(i, nViewIndex))
			{
				WriteMessage(LogTypeNotification, _T("FrameGrabber(Sentech) Open... OK[CAM%d]"), (nViewIndex + 1));
				CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("FrameGrabber(Sentech) Open... OK[CAM%d]"), (nViewIndex + 1));
			}
			else
			{
				WriteMessage(LogTypeError, _T("FrameGrabber(Sentech) Open... Failure"));
				CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("FrameGrabber(Sentech) Open... Failure"));
			}
			break;

		case CameraType3DArea:
			if (m_3DAreaScanCamera.Open(0, nViewIndex))
			{
				WriteMessage(LogTypeNotification, _T("FrameGrabber(MechEye) Open... OK[CAM%d]"), (nViewIndex + 1));
				CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("FrameGrabber(MechEye) Open... OK[CAM%d]"), (nViewIndex + 1));
			}
			else
			{
				WriteMessage(LogTypeError, _T("FrameGrabber(MechEye) Open... Failure"));
				CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("FrameGrabber(MechEye) Open... Failure"));
			}
			break;

		default:
			if (m_3DLineScanCamera.Open(i, nViewIndex))
			{
				WriteMessage(LogTypeNotification, _T("FrameGrabber(PanSight) Open... OK[CAM%d]"), (nViewIndex + 1));
				CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("FrameGrabber(PanSight) Open... OK[CAM%d]"), (nViewIndex + 1));
			}
			else
			{
				WriteMessage(LogTypeError, _T("FrameGrabber(PanSight) Open... Failure"));
				CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("FrameGrabber(PanSight) Open... Failure"));
			}
			break;

		}
	}
#elif RELEASE_SG
	for (int i = 0; i < CameraTypeMax; ++i)
	{
		UINT nViewIndex;
		if (m_3DLineScanCamera.Open(i, nViewIndex))
		{
			WriteMessage(LogTypeNotification, _T("FrameGrabber(PanSight) Open... OK[CAM%d]"), (nViewIndex + 1));
			CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("FrameGrabber(PanSight) Open... OK[CAM%d]"), (nViewIndex + 1));
		}
		else
		{
			WriteMessage(LogTypeError, _T("FrameGrabber(PanSight) Open... Failure"));
			CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("FrameGrabber(PanSight) Open... Failure"));
		}
	}

#else
	for (int i = 0; i < CameraTypeMax; ++i)
	{
		UINT nViewIndex;
		if (m_AreaScanCamera.Open(i, nViewIndex))
		{
			WriteMessage(LogTypeNotification, _T("FrameGrabber(Sentech) Open[%d]... OK[CAM%d]"), i, (nViewIndex + 1));
			CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("FrameGrabber(Sentech) Open... OK[CAM%d]"), (nViewIndex + 1));
		}
		else
		{
			WriteMessage(LogTypeError, _T("FrameGrabber(Sentech) Open[%d]... Failure"), i);
			CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("FrameGrabber(Sentech) Open[%d]... Failure"), i);
		}
	}
#endif
}

void CVisionSystem::LightCtrlConnect()
{
	CSystemConfig::LightControllerLFine& LightCtrlAreaScanConfig = CSystemConfig::Instance()->GetLightCtrlAreaScanConfig();

	if (m_LightControllerForArea.Open(LightCtrlAreaScanConfig.nPort[LedCtrlTypeArea1]))
	{
		WriteMessage(LogTypeNotification, _T("LED Controller LFine1 Open... OK"));
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("LED Controller LFine1 Open... OK"));
	}
	else
	{
		WriteMessage(LogTypeError, _T("LED Controller LFine1 Open... Failure"));
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("LED Controller LFine1 Open... Failure"));
	}

#if defined(RELEASE_2G) || defined(RELEASE_3G)
	BOOL bTurnOn2 = FALSE;
	if (m_LightControllerForArea2.Open(LightCtrlAreaScanConfig.nPort[LedCtrlTypeArea2]))
	{
		WriteMessage(LogTypeNotification, _T("LED Controller LFine2 Open... OK"));
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("LED Controller LFine2 Open... OK"));
	}
	else
	{
		WriteMessage(LogTypeError, _T("LED Controller LFine2 Open... Failure"));
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("LED Controller LFine2 Open... Failure"));
	}

	#if defined(RELEASE_3G)
	BOOL bTurnOn3 = FALSE;
	if (m_LightControllerForArea3.Open(LightCtrlAreaScanConfig.nPort[LedCtrlTypeArea3]))
	{
		WriteMessage(LogTypeNotification, _T("LED Controller LFine3 Open... OK"));
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("LED Controller LFine3 Open... OK"));

		m_LightControllerInterface.TurnOff(LedCtrlTypeArea3, USE_LFINE_LCP100_CTRL_CH1);
	}
	else
	{
		WriteMessage(LogTypeError, _T("LED Controller LFine3 Open... Failure"));
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("LED Controller LFine3 Open... Failure"));
	}
	#endif

#endif
}

void CVisionSystem::ServerOpen()
{
	// Ethernet
	int nStart, nEnd, nPort;
	nStart = nEnd = 0;

#ifdef RELEASE_SG
	nStart = SOCKET_STACKEROCR;
	nEnd = SOCKET_STACKEROCR;

#elif RELEASE_1G
	nStart = SOCKET_TRAYOCR;
	nEnd = SOCKET_LIFT;

#elif RELEASE_2G
	nStart = SOCKET_BANDING;
	nEnd = SOCKET_HIC;

#elif RELEASE_3G
	nStart = SOCKET_CUTTING1;
	nEnd = SOCKET_MATERIAL;

#elif RELEASE_4G
	nStart = SOCKET_MBB;
	nEnd = SOCKET_MBB_LABEL;

#elif RELEASE_5G
	nStart = SOCKET_BOX;
	nEnd = SOCKET_SEALING;

#elif RELEASE_6G
	nStart = SOCKET_BOX_LABEL;
	nEnd = SOCKET_TAPE;

#endif

	m_pNotifyVisionStatusChanged = CVisionStatusProvider::Instance();
	for (int i = nStart; i <= nEnd; ++i)
	{
		nPort = 5000 + i;
		CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("Vision Server Open[Port:%d]..."), nPort);
		if (!CVisionStatusProvider::Instance()->Start(nPort, i))
		{
			WriteMessage(LogTypeError, _T("Vision Server[Port:%d] Open... Failure"), nPort);
			CSplashWnd::ShowMessage(CSplashWnd::MessageTypeError, _T("Vision Server[Port:%d] Open... Failure"), nPort);
		}
		else
		{
			WriteMessage(LogTypeNotification, _T("Vision Server[Port:%d] Open... OK"), nPort);
			CSplashWnd::ShowMessage(CSplashWnd::MessageTypeNormal, _T("Vision Server[Port:%d] Open... OK"), nPort);
		}
	}
}

void CVisionSystem::ServerClose()
{
	// Ethernet
	int nStart, nEnd, nPort;
	nStart = nEnd = 0;

#ifdef RELEASE_SG
	nStart = SOCKET_STACKEROCR;
	nEnd = SOCKET_STACKEROCR;

#elif RELEASE_1G
	nStart = SOCKET_TRAYOCR;
	nEnd = SOCKET_LIFT;

#elif RELEASE_2G
	nStart = SOCKET_BANDING;
	nEnd = SOCKET_HIC;

#elif RELEASE_3G
	nStart = SOCKET_CUTTING1;
	nEnd = SOCKET_MATERIAL;

#elif RELEASE_4G
	nStart = SOCKET_MBB;
	nEnd = SOCKET_MBB_LABEL;

#elif RELEASE_5G
	nStart = SOCKET_BOX;
	nEnd = SOCKET_SEALING;

#elif RELEASE_6G
	nStart = SOCKET_BOX_LABEL;
	nEnd = SOCKET_TAPE;

#endif

	m_pNotifyVisionStatusChanged = CVisionStatusProvider::Instance();
	for (int i = nStart; i <= nEnd; ++i)
	{
		nPort = 5000 + i;
		CVisionStatusProvider::Instance()->Stop(i);
		
		WriteMessage(LogTypeNotification, _T("Vision Server[Port:%d] Close... OK"), nPort);
	}
}

BOOL CVisionSystem::CheckCamera(CameraType nCamType)
{
	if (!m_FrameGrabberInterface.IsOpen(nCamType))
	{
		CString strErrorString;

		switch (nCamType)
		{
#ifdef RELEASE_1G
		case CameraTypeArea:		strErrorString.Format(_T("Chip Camera Connect Error!"));			break;
		case CameraType3DArea:		strErrorString.Format(_T("3D Area Camera Connect Error!"));			break;
		case CameraTypeLine:		strErrorString.Format(_T("TrayOcr Camera Connect Error!"));			break;
		case CameraTypeLine2:		strErrorString.Format(_T("Mixing Camera Connect Error!"));			break;
		case CameraTypeLine3:		strErrorString.Format(_T("Lift Camera Connect Error!"));			break;
		case CameraTypeLine4:		strErrorString.Format(_T("Lift Camera Connect Error!"));			break;

#elif RELEASE_SG
		case CameraTypeLine:		strErrorString.Format(_T("Stacker Camera Connect Error!"));			break;

#elif RELEASE_2G
		case CameraTypeArea:		strErrorString.Format(_T("HIC Camera Connect Error!"));				break;
		case CameraTypeArea2:		strErrorString.Format(_T("Banding Camera Connect Error!"));			break;

#elif RELEASE_3G
		case CameraTypeArea:		strErrorString.Format(_T("Cutting Camera Connect Error!"));			break;
		case CameraTypeArea2:		strErrorString.Format(_T("Cutting Camera Connect Error!"));			break;
		case CameraTypeArea3:		strErrorString.Format(_T("Material Camera Connect Error!"));		break;

#elif RELEASE_4G
		case CameraTypeArea:		strErrorString.Format(_T("MBB Camera Connect Error!"));				break;
		case CameraTypeArea2:		strErrorString.Format(_T("Label Camera Connect Error!"));			break;

#elif RELEASE_5G
		case CameraTypeArea:		strErrorString.Format(_T("Box Camera Connect Error!"));				break;
		case CameraTypeArea2:		strErrorString.Format(_T("Sealing Camera Connect Error!"));			break;

#elif RELEASE_6G
		case CameraTypeArea:		strErrorString.Format(_T("HIC Camera Connect Error!"));				break;
		case CameraTypeArea2:		strErrorString.Format(_T("Banding Camera Connect Error!"));			break;

#endif
		}

		WRITE_LOG(WL_ERROR, strErrorString);
		return FALSE;
	}

	return TRUE;
}

BOOL CVisionSystem::CheckLedCtrl(LedCtrlType nCtrlType)
{
	if (!m_LightControllerInterface.IsConnected(nCtrlType))
	{
		CString strErrorString;

		switch (nCtrlType)
		{
		case LedCtrlTypeArea1:		strErrorString.Format(_T("LedCtrl 1 Connect Error!"));			break;
		case LedCtrlTypeArea2:		strErrorString.Format(_T("LedCtrl 2 Connect Error!"));			break;
		case LedCtrlTypeArea3:		strErrorString.Format(_T("LedCtrl 3 Connect Error!"));			break;
		}

		WRITE_LOG(WL_ERROR, strErrorString);
		return FALSE;
	}

	return TRUE;
}

BOOL CVisionSystem::CheckSystemCondition()
{
	//////////
	BOOL bCheckFrameGrabber = TRUE;

	for (int nCamIndex = 0; nCamIndex < CameraTypeMax; nCamIndex++)
		bCheckFrameGrabber &= CheckCamera(CameraType(nCamIndex));

	if (!bCheckFrameGrabber) WRITE_LOG(WL_ERROR, _T("FrameGrabber Condition Error!!"));

	//////////
	BOOL bCheckLightController = TRUE;
	int nMax = 0;

#ifdef RELEASE_2G
	nMax = LedCtrlTypeArea3;

#elif RELEASE_1G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	nMax = LedCtrlTypeArea2;

#elif RELEASE_3G
	nMax = LedCtrlTypeAreaMax;

#endif

	for (int nLedCtrlIndex = 0; nLedCtrlIndex < nMax; nLedCtrlIndex++)
		bCheckLightController &= CheckLedCtrl(LedCtrlType(nLedCtrlIndex));

	if (!bCheckLightController) WRITE_LOG(WL_ERROR, _T("LightController Condition Error!!"));


	//////////
	if (!bCheckFrameGrabber || !bCheckLightController)
	{
		ShowErrorDlg(_T("Auto Run Error"), _T("Vision Autorun Change Error"),
			_T("Autorun operation is not possible.\n\n1. Check camera connection status.\n2. Check light controller connection status."));

		WriteMessage(LogTypeError, _T("Vision Autorun Change Error!!"));
	}

	///
	m_n3DAreaShot = 0;

	return bCheckFrameGrabber && bCheckLightController;
}

BOOL CVisionSystem::ShowErrorDlg(CString strTitle, CString strMessage, CString strDetail)
{
	m_pMainView->ShowError(TRUE, strTitle, strMessage, strDetail);
	return TRUE;
}

void CVisionSystem::SetPreviewGainOffset(int nViewIndex, BOOL bPreview)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
	{
		AfxMessageBox(_T("OpenEvision Dongle이 없습니다."), MB_OK | MB_ICONERROR);
		return;
	}

	if (bPreview)
	{
		if (m_ImageObjectArea[nViewIndex].GetImageBuffer() != NULL)
		{
			m_ImageObjectTemp.Clone(&m_ImageObjectArea[nViewIndex]);

			CxImageObject pPreviewImgObj;
#ifdef RELEASE_3G
			m_pInspectVision->PreviewImage_ForDesiccantPosition(&m_ImageObjectArea[nViewIndex], pPreviewImgObj);
#else
			m_pInspectVision->PreviewImage_ForSelingQuality(&m_ImageObjectArea[nViewIndex], pPreviewImgObj);
#endif
			m_ImageObjectArea[nViewIndex].Clone(&pPreviewImgObj);
		}
	}
	else
	{
		if (m_ImageObjectTemp.GetImageBuffer() != NULL)
		{
			m_ImageObjectArea[nViewIndex].Clone(&m_ImageObjectTemp);
			m_ImageObjectTemp.Destroy();
		}
	}

	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, nViewIndex);
}

void CVisionSystem::DES201S(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_SG
	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf, strZAxis;
	AfxExtractSubString(strZAxis, strReceiveBuf, 1, '|');

	if (strVisionCode == _T("10")) // Stacker Ocr
	{
		SetLotID(InspectTypeStackerOcr, _T("NONE"));
		SetBoxNo(InspectTypeStackerOcr, 0);

		m_nStackerZAxis = _ttoi(strZAxis);

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_STACKEROCR);
	}
#endif
}

void CVisionSystem::DES201(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_1G
	CString strRecipeInfo, strLotID, strBoxNo;
	AfxExtractSubString(strRecipeInfo, strReceiveBuf, 1, '|');
	AfxExtractSubString(strLotID, strReceiveBuf, 2, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 3, '|');

	CString strRecipeSID, strRecipePID;
	AfxExtractSubString(strRecipeSID, strRecipeInfo, 0, ',');
	AfxExtractSubString(strRecipePID, strRecipeInfo, 1, ',');

	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf;

	if (strVisionCode == _T("11")) // Tray Ocr
	{
		if (CModelInfo::Instance()->GetModelNameTrayOcr() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_TRAYOCR), TEACH_TAB_IDX_TRAYOCR))
				WriteMessage(LogTypeNotification, _T("Tray Ocr Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Tray Ocr Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeTrayOcr, strLotID);
		SetBoxNo(InspectTypeTrayOcr, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_TRAYOCR);
	}
	else if (strVisionCode == _T("12")) // 3D Chip Cnt
	{
		if (CModelInfo::Instance()->GetModelName3DChipCnt() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_3DCHIPCNT), TEACH_TAB_IDX_3DCHIPCNT))
				WriteMessage(LogTypeNotification, _T("3D ChipCnt Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("3D ChipCnt Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}
		ResetImageGrabBuffer_3DChip();

		m_b3DChipCntResult = TRUE; // 최종 결과 초기화

		SetLotID(InspectType3DChipCnt, strLotID);
		SetBoxNo(InspectType3DChipCnt, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_3DCHIPCNT);
	}
	else if (strVisionCode == _T("13")) // Chip OCR 
	{
		if (CModelInfo::Instance()->GetModelNameChipOcr() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_CHIPOCR), TEACH_TAB_IDX_CHIPOCR))
				WriteMessage(LogTypeNotification, _T("Chip Ocr Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Chip Ocr Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}
		m_bChipCnt = FALSE;

		SetLotID(InspectTypeChipOcr, strLotID);
		SetBoxNo(InspectTypeChipOcr, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CHIPOCR);

		LightOnOff(InspectTypeChipOcr, TRUE);
	}
	else if (strVisionCode == _T("14")) // Chip Cnt
	{
		if (CModelInfo::Instance()->GetModelNameChip() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_CHIP), TEACH_TAB_IDX_CHIP))
				WriteMessage(LogTypeNotification, _T("Chip Cnt Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Chip Cnt Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		m_bChipCnt = TRUE;
		m_bChipCntHandlerStart = TRUE;
		m_nChipGrabCnt = 0;
		m_nChipInspCnt = 0;
		m_bChipCntResult = TRUE; // 최종 결과 초기화

		ResetImageGrabBuffer_Chip();

		SetLotID(InspectTypeChip, strLotID);
		SetBoxNo(InspectTypeChip, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CHIPCNT);

		// DES에서 켜놓고 DEE에 끄는 방식으로 변경.
		LightOnOff(InspectTypeChip, TRUE);
		SetTrayMapGridReset();
	}
	else if (strVisionCode == _T("15")) // Mixing
	{
		if (CModelInfo::Instance()->GetModelNameMixing() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_MIXING), TEACH_TAB_IDX_MIXING))
				WriteMessage(LogTypeNotification, _T("Mixing Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Mixing Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeMixing, strLotID);
		SetBoxNo(InspectTypeMixing, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MIXING);
	}
	else if (strVisionCode == _T("16")) // Lift
	{
		if (CModelInfo::Instance()->GetModelNameLift() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_LIFTINFO), TEACH_TAB_IDX_LIFTINFO))
				WriteMessage(LogTypeNotification, _T("Lift Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Lift Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeLiftFront, strLotID);
		SetBoxNo(InspectTypeLiftFront, _ttoi(strBoxNo));
		SetLotID(InspectTypeLiftRear, strLotID);
		SetBoxNo(InspectTypeLiftRear, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_LIFT);
	}
#endif
}

void CVisionSystem::DES202(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_2G
	CString strRecipeInfo, strLotID, strBoxNo;
	AfxExtractSubString(strRecipeInfo, strReceiveBuf, 1, '|');
	AfxExtractSubString(strLotID, strReceiveBuf, 2, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 3, '|');

	CString strRecipeSID(strRecipeInfo);

	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf;

	if (strVisionCode == _T("21")) // Banding
	{
		CString strVision = _T("VISION");
		if (CModelInfo::Instance()->GetModelNameBanding() != strVision)//strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strVision/*strRecipeSID*/, TEACH_TAB_IDX_BANDING), TEACH_TAB_IDX_BANDING))
				WriteMessage(LogTypeNotification, _T("Banding Model Change OK [Receive] : %s"), strVision); // strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Banding Model Change Error [Receive] : %s"), strVision); // strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeBanding, strLotID);
		SetBoxNo(InspectTypeBanding, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BANDING);

		LightOnOff(InspectTypeBanding, TRUE);
	}
	else if (strVisionCode == _T("22")) // HIC
	{
		if (CModelInfo::Instance()->GetModelNameHIC() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_HIC), TEACH_TAB_IDX_HIC))
				WriteMessage(LogTypeNotification, _T("HIC Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("HIC Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeHIC, strLotID);
		SetBoxNo(InspectTypeHIC, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_HIC);

		LightOnOff(InspectTypeHIC, TRUE);
	}
#endif
}

void CVisionSystem::DES203(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_3G
	CString strRecipeInfo, strLotID, strBoxNo, strInspectionKind, strSubMaterialTraySID;
	AfxExtractSubString(strRecipeInfo, strReceiveBuf, 1, '|');
	AfxExtractSubString(strLotID, strReceiveBuf, 2, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 3, '|');
	
	switch(_ttoi(strVisionCode))
	{
	case 31:
	case 32:
		AfxExtractSubString(strInspectionKind, strReceiveBuf, 4, '|');		break;
	case 33:
		AfxExtractSubString(strSubMaterialTraySID, strReceiveBuf, 4, '|');	break;
	}

	CString strRecipeSID(strRecipeInfo);

	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf;

	//CString strBandingData, strBandingCode, strDesiccantSeqCode, strDesiccantPosCode, strHICPosCode;
	//strBandingData = strBandingCode = strDesiccantSeqCode = strDesiccantPosCode = strHICPosCode = _T("");

	//	AfxExtractSubString(strBandingData, strRecipeInfo, 2, ',');
	//	AfxExtractSubString(strBandingCode, strRecipeInfo, 3, ',');
	//	AfxExtractSubString(strDesiccantSeqCode, strRecipeInfo, 4, ',');
	//	AfxExtractSubString(strDesiccantPosCode, strRecipeInfo, 5, ',');
	//	AfxExtractSubString(strHICPosCode, strRecipeInfo, 6, ',');

	if (strVisionCode == _T("31")) // Desiccant Cutting Right // Desiccant2
	{
		if (CModelInfo::Instance()->GetModelNameDesiccantCutting() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_DESICCANT_CUTTING), TEACH_TAB_IDX_DESICCANT_CUTTING))
				WriteMessage(LogTypeNotification, _T("Desiccant Cutting Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Desiccant Cutting Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypePositionRight, strLotID);
		SetLotID(InspectTypeDesiccantRight, strLotID);
		SetBoxNo(InspectTypePositionRight, _ttoi(strBoxNo));
		SetBoxNo(InspectTypeDesiccantRight, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		// strInspectionKind == "0" : Cutting 전
		// strInspectionKind == "1" : Cutting 후
		if (strInspectionKind == _T("0"))	m_bDesiccantInspRight = FALSE;
		else								m_bDesiccantInspRight = TRUE;

		if (!m_bDesiccantInspRight) // Right Position 검사일 때
			LightOnOff(InspectTypePositionRight, TRUE);

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CUTTING1);
	}
	else if (strVisionCode == _T("32")) // Desiccant Cutting Left // Desiccant1
	{
		if (CModelInfo::Instance()->GetModelNameDesiccantCutting() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_DESICCANT_MATERIAL), TEACH_TAB_IDX_DESICCANT_MATERIAL))
				WriteMessage(LogTypeNotification, _T("Desiccant Cutting Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Desiccant Cutting Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypePositionLeft, strLotID);
		SetLotID(InspectTypeDesiccantLeft, strLotID);
		SetBoxNo(InspectTypePositionLeft, _ttoi(strBoxNo));
		SetBoxNo(InspectTypeDesiccantLeft, _ttoi(strBoxNo));

		// strInspectionKind == "0" : Cutting 전
		// strInspectionKind == "1" : Cutting 후
		if (strInspectionKind == _T("0"))	m_bDesiccantInspLeft = FALSE;
		else								m_bDesiccantInspLeft = TRUE;

		if (!m_bDesiccantInspLeft) // Left Position 검사일 때
			LightOnOff(InspectTypePositionLeft, TRUE);

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CUTTING2);
	}
	else if (strVisionCode == _T("33")) // 부자재 유무
	{
		if (CModelInfo::Instance()->GetModelNameDesiccantMaterialTray() != strSubMaterialTraySID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strSubMaterialTraySID, TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY), TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY))
				WriteMessage(LogTypeNotification, _T("Material Change OK [Receive] : %s"), strSubMaterialTraySID);
			else
			{
				WriteMessage(LogTypeError, _T("Material Change Error [Receive] : %s"), strSubMaterialTraySID);
				bSendBuf = FALSE;
			}
		}

		if (CModelInfo::Instance()->GetModelNameDesiccantMaterial() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_DESICCANT_MATERIAL), TEACH_TAB_IDX_DESICCANT_MATERIAL))
				WriteMessage(LogTypeNotification, _T("Material Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Material Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}
		//SetLotID(InspectTypeMaterialTray, strLotID);
		//SetBoxNo(InspectTypeMaterialTray, _ttoi(strBoxNo));
		SetLotID(InspectTypeMaterial, strLotID);
		SetBoxNo(InspectTypeMaterial, _ttoi(strBoxNo));

		//			m_nBandingCode = _ttoi(strBandingCode);
		//			m_nDesiccantSeqCode = _ttoi(strDesiccantSeqCode);
		//			m_nDesiccantPosCode = _ttoi(strDesiccantPosCode);
		//			m_nHICPosCode = _ttoi(strHICPosCode);

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MATERIAL);
	}
#endif
}

void CVisionSystem::DES204(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_4G
	CString strRecipeInfo, strLotID, strBoxNo;
	AfxExtractSubString(strRecipeInfo, strReceiveBuf, 1, '|');
	AfxExtractSubString(strLotID, strReceiveBuf, 2, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 3, '|');

	CString strRecipeSID(strRecipeInfo);

	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf;

	if (strVisionCode == _T("41")) // MBB
	{
		if (CModelInfo::Instance()->GetModelNameMBB() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_MBB), TEACH_TAB_IDX_MBB))
				WriteMessage(LogTypeNotification, _T("MBB Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("MBB Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeMBB, strLotID);
		SetBoxNo(InspectTypeMBB, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MBB);
	}
	else if (strVisionCode == _T("42")) // Label
	{
		if (CModelInfo::Instance()->GetModelNameLabel() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_LABEL), TEACH_TAB_IDX_LABEL))
				WriteMessage(LogTypeNotification, _T("Label Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Label Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeLabel, strLotID);
		SetBoxNo(InspectTypeLabel, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MBB_LABEL);
	}
#endif
}

void CVisionSystem::DES205(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_5G
	CString strRecipeInfo, strLotID, strBoxNo;
	AfxExtractSubString(strRecipeInfo, strReceiveBuf, 1, '|');
	AfxExtractSubString(strLotID, strReceiveBuf, 2, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 3, '|');

	CString strRecipeSID(strRecipeInfo);

	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf;

	if (strVisionCode == _T("51")) // Box Quality
	{
		//if (CModelInfo::Instance()->GetModelNameBoxQuality() != strRecipeSID)
		//{
		//	if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_BOX), TEACH_TAB_IDX_BOX))
		//		WriteMessage(LogTypeNotification, _T("Box Quality Model Change OK [Receive] : %s"), strRecipeSID);
		//	else
		//	{
		//		WriteMessage(LogTypeError, _T("Box Quality Model Change Error [Receive] : %s"), strRecipeSID);
		//		bSendBuf = FALSE;
		//	}
		//}

		SetLotID(InspectTypeBox, strLotID);
		SetBoxNo(InspectTypeBox, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BOX);

		LightOnOff(InspectTypeBox, TRUE);
	}
	else if (strVisionCode == _T("52")) // Sealing Quality
	{
		if (CModelInfo::Instance()->GetModelNameSealingQuality() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_SEALING), TEACH_TAB_IDX_SEALING))
				WriteMessage(LogTypeNotification, _T("Sealing Quality Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Sealing Quality Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeSealing, strLotID);
		SetBoxNo(InspectTypeSealing, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_SEALING);

		LightOnOff(InspectTypeSealing, TRUE);
	}
#endif
}

void CVisionSystem::DES207(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_6G
	CString strRecipeInfo, strLotID, strBoxNo;
	AfxExtractSubString(strRecipeInfo, strReceiveBuf, 1, '|');
	AfxExtractSubString(strLotID, strReceiveBuf, 2, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 3, '|');

	CString strRecipeSID(strRecipeInfo);

	CString strCommandCode(_T("DES"));
	BOOL bSendBuf = TRUE;

	CString strSendBuf;

	if (strVisionCode == _T("61")) // Box Label
	{
		if (CModelInfo::Instance()->GetModelNameLabel() != strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipeSID, TEACH_TAB_IDX_LABEL), TEACH_TAB_IDX_LABEL))
				WriteMessage(LogTypeNotification, _T("Label Model Change OK [Receive] : %s"), strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Label Model Change Error [Receive] : %s"), strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeLabel, strLotID);
		SetBoxNo(InspectTypeLabel, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BOX_LABEL);
	}
	else if (strVisionCode == _T("62")) // Tape
	{
		CString strVision = _T("VISION");
		if (CModelInfo::Instance()->GetModelNameTape() != strVision)//strRecipeSID)
		{
			if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strVision, TEACH_TAB_IDX_TAPE), TEACH_TAB_IDX_TAPE))
				WriteMessage(LogTypeNotification, _T("Tape Model Change OK [Receive] : %s"), strVision); // strRecipeSID);
			else
			{
				WriteMessage(LogTypeError, _T("Tape Model Change Error [Receive] : %s"), strVision); // strRecipeSID);
				bSendBuf = FALSE;
			}
		}

		SetLotID(InspectTypeTape, strLotID);
		SetBoxNo(InspectTypeTape, _ttoi(strBoxNo));

		if (bSendBuf)	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		else			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");

		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_TAPE);
	}
#endif
}

void CVisionSystem::VIS201S(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_SG
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;

	if (strVisionCode == _T("10")) // Stacker Ocr
	{
		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine) || !FrameGrabber.IsOpen(CameraTypeLine))
		{
			WRITE_LOG(WL_DEBUG, _T("3DLine Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_STACKEROCR);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : Stacker Ocr Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_STACKEROCR);

		BeginStopWatch(IDX_AREA1);

		CameraOnOff(InspectTypeStackerOcr, TRUE);
	}
#endif
}

void CVisionSystem::VIS201(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_1G
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;
	CString strVISData;

	AfxExtractSubString(strVISData, strReceiveBuf, 1, '|');

	if (strVisionCode == _T("11")) // Tray Ocr
	{
		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine) || !FrameGrabber.IsOpen(CameraTypeLine))
		{
			WRITE_LOG(WL_DEBUG, _T("3DLine Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_TRAYOCR);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : Tray Ocr Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_TRAYOCR);

		BeginStopWatch(IDX_AREA1);

//		VisionProcess::CInspectProcess::InspectItem pQueueData;
//		pQueueData.pImgObj = &m_ImageObjectArea[0];
//		pQueueData.pGO = m_pGOArea[0];
//		pQueueData.nViewIndex = 0;
//		pQueueData.inspecttype = (InspectType)0;
//		pQueueData.nGrabCnt = 0;
//
//		if (!m_pInspectProcess->PushInspectItem(pQueueData))
//			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), 0);

		CameraOnOff(InspectTypeTrayOcr, TRUE);
	}
	else if (strVisionCode == _T("12")) // 3D Chip Cnt
	{
		if (!m_3DAreaScanCamera.IsOpen(0) || !FrameGrabber.IsOpen(CameraType3DArea))
		{
			WRITE_LOG(WL_DEBUG, _T("3DArea Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_3DCHIPCNT);
			return;
		}

		if (m_n3DAreaShot == 0)
		{
			m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);
			m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA3);
		}
		m_nMMI3DChipCnt = _ttoi(strVISData);

		// Light & Grab
		strLog.Format(_T("case : 3D Chip Cnt Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_3DCHIPCNT);

		if (m_n3DAreaShot == 1)
			BeginStopWatch(IDX_AREA3);

		CameraOnOff(InspectType3DChipCnt, TRUE);
	}
	else if (strVisionCode == _T("13")) // Chip OCR 
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CHIPOCR);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

		m_AreaScanCamera.m_bRotate = TRUE;

		// Light & Grab
		strLog.Format(_T("case : Chip Ocr Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CHIPOCR);

		BeginStopWatch(IDX_AREA4);

		CameraOnOff(InspectTypeChipOcr, TRUE);
	}
	else if (strVisionCode == _T("14")) // Chip Cnt
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CHIPCNT);
			return;
		}
//		else
//		{
//			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
//			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CHIPCNT);
//		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

		m_AreaScanCamera.m_bRotate = FALSE;
		m_nMMIChipCnt = _ttoi(strVISData); // Chip Count 수량

		// Light & Grab
		strLog.Format(_T("case : Chip Cnt Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		BeginStopWatch(IDX_AREA4);

		m_pNotifyVisionStatusChanged->SendPacket(_T("14VIS0013|1|^"), SOCKET_CHIPCNT);
		CameraOnOff(InspectTypeChip, TRUE);
	}
	else if (strVisionCode == _T("15")) // Mixing
	{
		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine2) || !FrameGrabber.IsOpen(CameraTypeLine2))
		{
			WRITE_LOG(WL_DEBUG, _T("3DLine2 Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MIXING);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA5);

		m_nMMITrayQuantity_Mixing = _ttoi(strVISData);

		// Light & Grab
		strLog.Format(_T("case : Mixing Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MIXING);

		BeginStopWatch(IDX_AREA5);

		CameraOnOff(InspectTypeMixing, TRUE);
	}
	else if (strVisionCode == _T("16")) // Lift
	{
		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine3) || !FrameGrabber.IsOpen(CameraTypeLine3))
		{
			WRITE_LOG(WL_DEBUG, _T("3DLine3 Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_LIFT);
			return;
		}
		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine4) || !FrameGrabber.IsOpen(CameraTypeLine4))
		{
			WRITE_LOG(WL_DEBUG, _T("3DLine4 Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_LIFT);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA6);
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA7);

		if(m_bAcm[ACM_LIFT] == FALSE)
			m_nMMITrayQuantity_Lift = _ttoi(strVISData);

		// Light & Grab
		strLog.Format(_T("case : Lift Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_LIFT);

		BeginStopWatch(IDX_AREA6);
		BeginStopWatch(IDX_AREA7);

		CameraOnOff(InspectTypeLiftFront, TRUE);
		CameraOnOff(InspectTypeLiftRear, TRUE);
	}
#endif
}

void CVisionSystem::VIS202(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_2G
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;
	CString strVISData;
	AfxExtractSubString(strVISData, strReceiveBuf, 1, '|');

	CString strBandDir, strBandPos;
	AfxExtractSubString(strBandDir, strVISData, 0, ',');
	AfxExtractSubString(strBandPos, strVISData, 1, ',');

	if (strVisionCode == _T("21")) // Banding
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BANDING);
			return;
		}
		m_nBandingDir = _ttoi(strBandDir);
		m_nBandingPos = _ttoi(strBandPos);

//		if(m_nBandingPos == 2)
			LightOnOff(InspectTypeBanding, TRUE);

		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : Banding Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BANDING);

		BeginStopWatch(IDX_AREA1);

		CameraOnOff(InspectTypeBanding, TRUE);
	}
	else if (strVisionCode == _T("22")) // HIC
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea2))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera2 Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_HIC);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

		// Light & Grab
		strLog.Format(_T("case : HIC Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_HIC);

		BeginStopWatch(IDX_AREA2);

		LightOnOff(InspectTypeHIC, TRUE);
		CameraOnOff(InspectTypeHIC, TRUE);
	}
#endif
}

void CVisionSystem::VIS203(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_3G
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;
	CString strVISData;
	AfxExtractSubString(strVISData, strReceiveBuf, 2, '|');

	if (strVisionCode == _T("31")) // Desiccant Cutting Right // Desiccant2
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea2))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|") + strVISData + _T("|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CUTTING1);
			return;
		}
		if (strVISData == _T("0"))	m_bDesiccantInspRight = FALSE;
		else						m_bDesiccantInspRight = TRUE;

		if (m_bDesiccantInspRight) 	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4); // Cut
		else 						m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2); // Pos

		// Light & Grab
		if (m_bDesiccantInspRight) 	strLog.Format(_T("case : Right Cutting Inspect Start"));
		else						strLog.Format(_T("case : Right Position Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|") + strVISData + _T("|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CUTTING1);

		if (m_bDesiccantInspRight)
		{
			BeginStopWatch(IDX_AREA4);

			LightOnOff(InspectTypePositionRight, FALSE);
			LightOnOff(InspectTypeDesiccantRight, TRUE);
			CameraOnOff(InspectTypeDesiccantRight, TRUE);
		}
		else
		{
			BeginStopWatch(IDX_AREA2);

			CameraOnOff(InspectTypePositionRight, TRUE);
		}
	}
	else if (strVisionCode == _T("32")) // Desiccant Cutting Left // Desiccant1
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera2 Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|") + strVISData + _T("|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CUTTING2);
			return;
		}
		if (strVISData == _T("0"))	m_bDesiccantInspLeft = FALSE;
		else						m_bDesiccantInspLeft = TRUE;

		if (m_bDesiccantInspLeft) 	m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA3); // Cut
		else 						m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1); // Pos

		// Light & Grab
		if (m_bDesiccantInspLeft) 	strLog.Format(_T("case : Left Cutting2 Inspect Start [%d]"), m_bDesiccantInspLeft);
		else						strLog.Format(_T("case : Left Position2 Inspect Start [%d]"), m_bDesiccantInspLeft);
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|") + strVISData + _T("|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_CUTTING2);

		if (m_bDesiccantInspLeft)
		{
			BeginStopWatch(IDX_AREA3);

			LightOnOff(InspectTypePositionLeft, FALSE);
			LightOnOff(InspectTypeDesiccantLeft, TRUE);
			CameraOnOff(InspectTypeDesiccantLeft, TRUE);
		}
		else
		{
			BeginStopWatch(IDX_AREA1);

			CameraOnOff(InspectTypePositionLeft, TRUE);
		}
	}
	else if (strVisionCode == _T("33")) // SubMaterial
	{
		m_nMMITrayQuantity_SubMaterial = _ttoi(strVISData);
		if (!FrameGrabber.IsOpen(CameraTypeArea3))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera3 Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MATERIAL);
			return;
		}

		// Light & Grab
		strLog.Format(_T("case : Material Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA5);

		BeginStopWatch(IDX_AREA5);

		LightOnOff(InspectTypeMaterial, TRUE);
		CameraOnOff(InspectTypeMaterial, TRUE);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MATERIAL);
	}
#endif
}

void CVisionSystem::VIS204(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_4G
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;

	if (strVisionCode == _T("41")) // MBB
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MBB);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : MBB Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MBB);

		BeginStopWatch(IDX_AREA1);

		LightOnOff(InspectTypeMBB, TRUE);
		CameraOnOff(InspectTypeMBB, TRUE);
	}
	else if (strVisionCode == _T("42")) // Label
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea2))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera2 Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MBB_LABEL);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

		// Light & Grab
		strLog.Format(_T("case : Label Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_MBB_LABEL);

		BeginStopWatch(IDX_AREA2);

		LightOnOff(InspectTypeLabel, TRUE);
		CameraOnOff(InspectTypeLabel, TRUE);
	}
#endif
}

void CVisionSystem::VIS205(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_5G
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;

	if (strVisionCode == _T("51")) // Box Quality
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BOX);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : Box Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BOX);

		BeginStopWatch(IDX_AREA1);

		CameraOnOff(InspectTypeBox, TRUE);
	}
	else if (strVisionCode == _T("52")) // Sealing Quality
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea2))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_SEALING);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

		// Light & Grab
		strLog.Format(_T("case : Sealing Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_SEALING);

		BeginStopWatch(IDX_AREA2);

		CameraOnOff(InspectTypeSealing, TRUE);
	}
#endif
}

void CVisionSystem::VIS207(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_6G
	CString strCommandCode(_T("VIS"));
	CString strSendBuf;
	CString strLog;

	if (strVisionCode == _T("61")) // Box Label
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BOX_LABEL);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : Label Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		BeginStopWatch(IDX_AREA1);

		LightOnOff(InspectTypeLabel, TRUE);
		CameraOnOff(InspectTypeLabel, TRUE);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_BOX_LABEL);
	}
	else if (strVisionCode == _T("62")) // Tape
	{
		if (!FrameGrabber.IsOpen(CameraTypeArea2))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera2 Open Fail !!"));

			strSendBuf = strVisionCode + strCommandCode + _T("SIZE|0|^");
			m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_TAPE);
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);

		// Light & Grab
		strLog.Format(_T("case : Tape Inspect Start"));
		WriteMessage(LogTypeNormal, strLog);

		strSendBuf = strVisionCode + strCommandCode + _T("SIZE|1|^");
		m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, SOCKET_TAPE);

		BeginStopWatch(IDX_AREA2);

		LightOnOff(InspectTypeTape, TRUE);
		CameraOnOff(InspectTypeTape, TRUE);
	}
#endif
}

void CVisionSystem::DEE201S(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_SG
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 10:	nPort = SOCKET_STACKEROCR;	break;
	default: 								break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;

	//m_strTraySID = CModelInfo::Instance()->GetModelNameStackerOcr(); // 임시
	strResultData.Format(_T("%d|"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData + m_strTraySID + _T("|^");

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);
#endif
}

void CVisionSystem::DEE201(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_1G
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 11:	nPort = SOCKET_TRAYOCR;												break;
	case 12:	nPort = SOCKET_3DCHIPCNT; 	bResult = m_b3DChipCntResult;			break;
	case 13:	nPort = SOCKET_CHIPOCR;												break;
	case 14:	nPort = SOCKET_CHIPCNT;		bResult = m_bChipCntResult;
											m_bChipCntHandlerStart = FALSE;
											LightOnOff(InspectTypeChip, FALSE);		break;
	case 15:	nPort = SOCKET_MIXING;												break;
	case 16:	nPort = SOCKET_LIFT;												break;
	default:																		break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;

	strResultData.Format(_T("%d|^"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData;

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);

	// ChipCount 결과 OK일 때 Image Merge
	if (_ttoi(strVisionCode) == 14 && bResult == TRUE)
	{
		VisionProcess::CInspectSaveMergeProcess::MergeItem pQueueData;
		pQueueData.bXMerge = FALSE;

		if (!m_pInspectSaveMergeProcess->PushMergeItem(pQueueData))
			WRITE_LOG(WL_ERROR, _T("m_pInspectSaveMergeProcess->PushMergeItem :: Fail"));
	}
#endif
}

void CVisionSystem::DEE202(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_2G
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 21:	nPort = SOCKET_BANDING;		LightOnOff(InspectTypeBanding, FALSE);		break;
	case 22:	nPort = SOCKET_HIC;			LightOnOff(InspectTypeHIC, FALSE);			break;
	default:
		break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;
	
	strResultData.Format(_T("%d|^"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData;

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);
#endif
}

void CVisionSystem::DEE203(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_3G
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 31:	nPort = SOCKET_CUTTING1;
		if (!m_bDesiccantInspRight)			LightOnOff(InspectTypePositionRight, FALSE);
		else								LightOnOff(InspectTypeDesiccantRight, FALSE);	break;

	case 32:	nPort = SOCKET_CUTTING2;
		if (!m_bDesiccantInspLeft)			LightOnOff(InspectTypePositionLeft, FALSE);				
		else								LightOnOff(InspectTypeDesiccantLeft, FALSE);	break;

	case 33:	nPort = SOCKET_MATERIAL;	LightOnOff(InspectTypeMaterial, FALSE);			break;
	default:																				break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;

	strResultData.Format(_T("%d|^"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData;
	
	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);
#endif
}

void CVisionSystem::DEE204(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_4G
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 41:	nPort = SOCKET_MBB;			LightOnOff(InspectTypeMBB, FALSE);		break;
	case 42:	nPort = SOCKET_MBB_LABEL;	LightOnOff(InspectTypeLabel, FALSE);	break;
	default:																		break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;

	strResultData.Format(_T("%d|^"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData;

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);
#endif
}

void CVisionSystem::DEE205(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_5G
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 51:	nPort = SOCKET_BOX;			LightOnOff(InspectTypeBox, FALSE);			break;
	case 52:	nPort = SOCKET_SEALING;		LightOnOff(InspectTypeSealing, FALSE);		break;
	default:																			break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;

	strResultData.Format(_T("%d|^"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData;

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);

#endif
}

void CVisionSystem::DEE207(const CString& strReceiveBuf, const CString& strVisionCode)
{
#ifdef RELEASE_6G
	int nPort = SOCKET_MAX;
	BOOL bResult = TRUE;

	switch (_ttoi(strVisionCode))
	{
	case 61:	nPort = SOCKET_BOX_LABEL;	LightOnOff(InspectTypeLabel, FALSE);		break;
	case 62:	nPort = SOCKET_TAPE;		LightOnOff(InspectTypeTape, FALSE);			break;
	default:																			break;
	}

	CString strCommandCode(_T("DEE"));
	CString strResultData, strSendBuf;

	strResultData.Format(_T("%d|^"), bResult);
	strSendBuf = strVisionCode + strCommandCode + _T("SIZE|") + strResultData;

	m_pNotifyVisionStatusChanged->SendPacket(strSendBuf, nPort);
#endif
}

void CVisionSystem::SetDepthOffset(int nBase)
{
	CreateImageGrabBuffers(CamTypeAreaScan, nAREA_CAM_SIZE_X[IDX_AREA1], nAREA_CAM_SIZE_Y[IDX_AREA1], IDX_AREA1);

	const vector<float> dase = m_3DLineScanCamera.GetDepthData();
	vector<float> data = dase;
	vector<unsigned char> grayData(5000*1440);//m_3DLineScanCamera.m_nDepthHeight * m_3DLineScanCamera.m_nDepthWidth);

	float zMax = 10; //pDepth[0];
	float zMin = 245;// pDepth[0];

	CSystemConfig::Cam3DScaleValue& Cam3DScaleValue = CSystemConfig::Instance()->GetCam3DScaleValue();
	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();
	float fLow, fHigh;
	if (nBase != 0)
	{
		fLow = Cam3DScaleValue.fScaleLowValues[0] + stStackerOcr.nDepthOffset;
		fHigh = Cam3DScaleValue.fScaleHighValues[0] + stStackerOcr.nDepthOffset;
	}
	else
	{
		fLow = Cam3DScaleValue.fScaleLowValues[0];
		fHigh = Cam3DScaleValue.fScaleHighValues[0];
	}

	int iDataSize = 5000 * 1440; //  m_3DLineScanCamera.m_nDepthHeight * m_3DLineScanCamera.m_nDepthWidth;
	for (int i = 0; i < iDataSize; i++)
	{
		if ((data[i] < fLow) || (data[i] > fHigh))
			data[i] = 0;

		if ((data[i] < zMin) && (data[i] > fLow))
			zMin = data[i];
		if ((data[i] > zMax) && (data[i] < fHigh))
			zMax = data[i];
	}

	for (int i = 0; i < iDataSize; i++)
	{
		grayData[i] = Rescale(data[i], zMax, zMin);
	}

	cv::Mat grayImage(/*m_3DLineScanCamera.m_nDepthHeight, m_3DLineScanCamera.m_nDepthWidth,*/ 5000, 1440, CV_8UC1, grayData.data());
	cv::Mat rotateImage(/*m_3DLineScanCamera.m_nDepthHeight, m_3DLineScanCamera.m_nDepthWidth,*/ 5000, 1440, CV_8UC1);

	if (stStackerOcr.bRotateImage)
		cv::rotate(grayImage, rotateImage, cv::ROTATE_90_COUNTERCLOCKWISE);
	else
		cv::rotate(grayImage, rotateImage, cv::ROTATE_90_CLOCKWISE);

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[IDX_AREA1].GetImageBuffer();
	memcpy(pDstBuf, rotateImage.data, grayData.size());
	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, IDX_AREA1);
}

void CVisionSystem::SetDepthOffset_Size(int nWidth, int nHeight)
{
	CreateImageGrabBuffers(CamTypeAreaScan, nWidth, nHeight, IDX_AREA1);

	const vector<float> dase = m_3DLineScanCamera.GetDepthData();
	vector<float> data = dase;
	vector<unsigned char> grayData(nWidth * nHeight);//m_3DLineScanCamera.m_nDepthHeight * m_3DLineScanCamera.m_nDepthWidth);

	float zMax = 10; //pDepth[0];
	float zMin = 245;// pDepth[0];

	CSystemConfig::Cam3DScaleValue& Cam3DScaleValue = CSystemConfig::Instance()->GetCam3DScaleValue();
	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();
	float fLow, fHigh;
	fLow = Cam3DScaleValue.fScaleLowValues[0] + stStackerOcr.nDepthOffset;
	fHigh = Cam3DScaleValue.fScaleHighValues[0] + stStackerOcr.nDepthOffset;

	int iDataSize = nWidth * nHeight; //  m_3DLineScanCamera.m_nDepthHeight * m_3DLineScanCamera.m_nDepthWidth;
	for (int i = 0; i < iDataSize; i++)
	{
		if ((data[i] < fLow) || (data[i] > fHigh))
			data[i] = 0;

		if ((data[i] < zMin) && (data[i] > fLow))
			zMin = data[i];
		if ((data[i] > zMax) && (data[i] < fHigh))
			zMax = data[i];
	}

	for (int i = 0; i < iDataSize; i++)
	{
		grayData[i] = Rescale(data[i], zMax, zMin);
	}

	cv::Mat grayImage(nWidth, nHeight, CV_8UC1, grayData.data());
	cv::Mat rotateImage(nWidth, nHeight, CV_8UC1);

	if (stStackerOcr.bRotateImage)
		cv::rotate(grayImage, rotateImage, cv::ROTATE_90_COUNTERCLOCKWISE);
	else
		cv::rotate(grayImage, rotateImage, cv::ROTATE_90_CLOCKWISE);

	BYTE* pDstBuf = (BYTE*)m_ImageObjectArea[IDX_AREA1].GetImageBuffer();
	memcpy(pDstBuf, rotateImage.data, grayData.size());
	PostUIMsgUpdateImageView(FALSE, CamTypeAreaScan, IDX_AREA1);
}

UINT8 CVisionSystem::Rescale(float value, float max, float min)
{
	if (value < min)
		return 0;
	else if (value > max)
		return 255;
	else if (abs(max - min) < 0.00001)
		return 0;

	return (unsigned char)(((value - min) / (max - min)) * 255);
}

BOOL CVisionSystem::SaveDepthToBin(const CString& strPath)
{
	const vector<float> depth = m_3DLineScanCamera.GetDepthData();

	FILE* fp = nullptr;
	errno_t err = _wfopen_s(&fp, strPath, L"wb");
	if (err != 0 || fp == nullptr)
	{
		WRITE_LOG(WL_ERROR, _T("Bin 파일 저장"));
		return FALSE;
	}

	fwrite(depth.data(), sizeof(float), depth.size(), fp);
	fclose(fp);
	return true;
}

BOOL CVisionSystem::LoadDepthFromBin(const CString& strPath)
{
	FILE* fp = nullptr;
	errno_t err = _wfopen_s(&fp, strPath, L"rb");
	if (err != 0 || fp == nullptr)
	{
		WRITE_LOG(WL_ERROR, _T("Bin 열기 실패"));
		return FALSE;
	}

	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	rewind(fp);

	int count = fileSize / sizeof(float);
	m_3DLineScanCamera.m_depthData.resize(count);

	fread(m_3DLineScanCamera.m_depthData.data(), sizeof(float), count, fp);
	fclose(fp);
	return true;
}

void CVisionSystem::RemoveBoxData(const CString& strReceiveBuf, int nIndex)
{
	CString strLog, strLotID, strBoxNo;
	AfxExtractSubString(strLotID, strReceiveBuf, 1, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 2, '|');

	// Date
	CTime tmCurrent = CTime::GetCurrentTime();
	CString strImgSaveDay;
	strImgSaveDay.Format(_T("%04d%02d%02d"), tmCurrent.GetYear(), tmCurrent.GetMonth(), tmCurrent.GetDay());

	// Remove Box Data
	strLog.Format(_T("Remove Box Data : [%s][%s][%s]"), strImgSaveDay, strLotID, strBoxNo);
	WriteMessage(LogTypeNormal, strLog);

	// Start
	CFileFind finder;
	CString strDeleteFolder, strDeleteFile;
	for (int i = 1; i <= nIndex; i++)
	{
		strDeleteFolder.Format(_T("\\\\192.168.11.3%d\\ImageLog\\%s\\%s\\%s"), i, strImgSaveDay, strLotID, strBoxNo );

		// Server Delete
		DeleteFolderRecursive(strDeleteFolder);

		strLog.Format(_T("Group Image Folder Delete End - [%s]"), strDeleteFolder);
		WriteMessage(LogTypeNormal, strLog);
	}
	finder.Close();

	// Server Delete
	strDeleteFolder.Format(_T("\\\\192.168.11.99\\VisionImageLog\\%s\\%s\\%s"), strImgSaveDay, strLotID, strBoxNo);

	DeleteFolderRecursive(strDeleteFolder);

	strLog.Format(_T("Server Image Folder Delete End - [%s]"), strDeleteFolder);
	WriteMessage(LogTypeNormal, strLog);
	finder.Close();
}

void CVisionSystem::ReworkBoxData(const CString& strReceiveBuf, int nIndex)
{
	CString strLog, strLotID, strBoxNo;
	AfxExtractSubString(strLotID, strReceiveBuf, 1, '|');
	AfxExtractSubString(strBoxNo, strReceiveBuf, 2, '|');

	// Date
	CTime tmCurrent = CTime::GetCurrentTime();
	CString strImgSaveDay, strImgSaveTime;
	strImgSaveDay.Format(_T("%04d%02d%02d"), tmCurrent.GetYear(), tmCurrent.GetMonth(), tmCurrent.GetDay());
	strImgSaveTime.Format(_T("%02d%02d%02d"), tmCurrent.GetHour(), tmCurrent.GetMinute(), tmCurrent.GetSecond());

	// Log
	strLog.Format(_T("Rework Box Folder : [%s][%s][%s]"), strImgSaveDay, strLotID, strBoxNo);
	WriteMessage(LogTypeNormal, strLog);

	// Group 서버들
	for (int i = 1; i <= nIndex; i++)
	{
		CString strOldFolder, strNewFolder;
		strOldFolder.Format(_T("\\\\192.168.11.3%d\\ImageLog\\%s\\%s\\%s"),	i, strImgSaveDay, strLotID, strBoxNo);
		strNewFolder = strOldFolder + _T("_Rework_") + strImgSaveTime;

		// 폴더 이름 변경
		if (MoveFile(strOldFolder, strNewFolder))
		{
			strLog.Format(_T("Rename OK - [%s] -> [%s]"), strOldFolder, strNewFolder);
			WriteMessage(LogTypeNotification, strLog);
		}
		else
		{
			DWORD err = GetLastError();
			strLog.Format(_T("Rename FAIL - [%s] (Error:%d)"), strOldFolder, err);
			WriteMessage(LogTypeError, strLog);
		}
	}

	// 11.99 서버
	CString strOldFolder, strNewFolder;
	strOldFolder.Format(_T("\\\\192.168.11.99\\VisionImageLog\\%s\\%s\\%s"), strImgSaveDay, strLotID, strBoxNo);
	strNewFolder = strOldFolder + _T("_Rework_") + strImgSaveTime;

	if (MoveFile(strOldFolder, strNewFolder))
	{
		strLog.Format(_T("Rename OK - [%s] -> [%s]"), strOldFolder, strNewFolder);
		WriteMessage(LogTypeNotification, strLog);
	}
	else
	{
		DWORD err = GetLastError();
		strLog.Format(_T("Rename FAIL - [%s] (Error:%d)"), strOldFolder, err);
		WriteMessage(LogTypeError, strLog);
	}
}

BOOL CVisionSystem::DeleteFolderRecursive(LPCTSTR folderPath)
{
	WIN32_FIND_DATA fd;
	TCHAR searchPath[MAX_PATH];
	TCHAR fullPath[MAX_PATH];

	// 검색 패턴 만들기
	_stprintf_s(searchPath, _T("%s\\*"), folderPath);

	HANDLE hFind = FindFirstFile(searchPath, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{
		if (_tcscmp(fd.cFileName, _T(".")) == 0 ||
			_tcscmp(fd.cFileName, _T("..")) == 0)
			continue;

		_stprintf_s(fullPath, _T("%s\\%s"), folderPath, fd.cFileName);

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 하위 폴더 삭제
			DeleteFolderRecursive(fullPath);
		}
		else
		{
			// 파일 속성 초기화 후 삭제
			SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(fullPath);
		}

	} while (FindNextFile(hFind, &fd));

	FindClose(hFind);

	// 마지막으로 자기 자신 삭제
	SetFileAttributes(folderPath, FILE_ATTRIBUTE_NORMAL);
	return RemoveDirectory(folderPath);
}

void CVisionSystem::TestMerge()
{
//	VisionProcess::CInspectSaveMergeProcess::MergeItem pData;
//	pData.bXMerge = FALSE;
//
//	if (!m_pInspectSaveMergeProcess->PushMergeItem(pData))
//		WRITE_LOG(WL_ERROR, _T("m_pInspectSaveMergeProcess->PushMergeItem :: Fail"));

//	if (!m_Timer.StartTimer(1000, FALSE))
//	{
//		WriteMessage(LogTypeError, _T("Timer Start... Failure"));
//	}
//	else
//	{
//		WriteMessage(LogTypeNotification, _T("Timer Start...OK"));
//	}
}

void CVisionSystem::ACM201(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_1G
	CString strCommandCode(_T("ACM"));
	CString strSendBuf;
	CString strLog;
	CString strRecipe = _T("CALIBRATION");

	if (strVisionCode == _T("12")) // 3D Chip Cnt
	{
		if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipe, TEACH_TAB_IDX_TRAYOCR), TEACH_TAB_IDX_TRAYOCR))
			WriteMessage(LogTypeNotification, _T("Tray Ocr Model Change OK [Receive] : %s"), strRecipe);
		else
		{
			WriteMessage(LogTypeError, _T("Tray Ocr Model Change Error [Receive] : %s"), strRecipe);
			return;
		}

		if (!m_3DAreaScanCamera.IsOpen(0) || !FrameGrabber.IsOpen(CameraType3DArea))
		{
			WRITE_LOG(WL_DEBUG, _T("3DArea Camera Open Fail !!"));
			return;
		}

		if (m_n3DAreaShot == 0)
		{
			m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2);
			m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA3);
		}

		// Light & Grab
		strLog.Format(_T("case : 3D Chip Cnt ACM Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_bAcm[ACM_3DCHIPCNT] = TRUE;

		if (m_n3DAreaShot == 1)
			BeginStopWatch(IDX_AREA3);

		CameraOnOff(InspectType3DChipCnt, TRUE);
	}
	else if (strVisionCode == _T("13")) // Chip OCR 
	{
		if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipe, TEACH_TAB_IDX_CHIPOCR), TEACH_TAB_IDX_CHIPOCR))
			WriteMessage(LogTypeNotification, _T("Chip Ocr Model Change OK [Receive] : %s"), strRecipe);
		else
		{
			WriteMessage(LogTypeError, _T("Chip Ocr Model Change Error [Receive] : %s"), strRecipe);
			return;
		}

		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));
			return;
		}
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA4);

		m_bChipCnt = FALSE;

		// Light & Grab
		strLog.Format(_T("case : Chip ACM Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_bAcm[ACM_CHIP] = TRUE;

		BeginStopWatch(IDX_AREA4);

//		VisionProcess::CInspectProcess::InspectItem pQueueData;
//		pQueueData.pImgObj = &m_ImageObjectArea[IDX_AREA4];
//		pQueueData.pGO = m_pGOArea[IDX_AREA4];
//		pQueueData.nViewIndex = IDX_AREA4;
//		pQueueData.inspecttype = InspectTypeChipOcr;
//		pQueueData.nGrabCnt = 0;
//
//		if (!m_pInspectProcess->PushInspectItem(pQueueData))
//			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), IDX_AREA4);

		LightOnOff(InspectTypeChipOcr, TRUE);
		CameraOnOff(InspectTypeChipOcr, TRUE);
	}
	else if (strVisionCode == _T("16")) // Lift
	{
		if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipe, TEACH_TAB_IDX_LIFTINFO), TEACH_TAB_IDX_LIFTINFO))
			WriteMessage(LogTypeNotification, _T("Lift Model Change OK [Receive] : %s"), strRecipe);
		else
		{
			WriteMessage(LogTypeError, _T("Lift Model Change Error [Receive] : %s"), strRecipe);
			return;
		}

//		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine3) || !FrameGrabber.IsOpen(CameraTypeLine3))
//		{
//			WRITE_LOG(WL_DEBUG, _T("3DLine3 Camera Open Fail !!"));
//			return;
//		}
//		if (!m_3DLineScanCamera.IsOpen(CameraTypeLine4) || !FrameGrabber.IsOpen(CameraTypeLine4))
//		{
//			WRITE_LOG(WL_DEBUG, _T("3DLine4 Camera Open Fail !!"));
//			return;
//		}
//		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA6);
//		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA7);

		// Light & Grab
		strLog.Format(_T("case : Lift ACM Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_bAcm[ACM_LIFT] = TRUE;

//		BeginStopWatch(IDX_AREA6);
//		BeginStopWatch(IDX_AREA7);
//
//		CameraOnOff(InspectTypeLiftFront, TRUE);
//		CameraOnOff(InspectTypeLiftRear, TRUE);
	}
#endif
}

void CVisionSystem::ACM203(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_3G
	CString strCommandCode(_T("ACM"));
	CString strSendBuf;
	CString strLog;
	CString strRecipe = _T("CALIBRATION");

	if (strVisionCode == _T("31")) // Desiccant Cutting Right // Desiccant2
	{
		if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipe, TEACH_TAB_IDX_DESICCANT_CUTTING), TEACH_TAB_IDX_DESICCANT_CUTTING))
			WriteMessage(LogTypeNotification, _T("Desiccant Cutting Model Change OK [Receive] : %s"), strRecipe);
		else
		{
			WriteMessage(LogTypeError, _T("Desiccant Cutting Change Error [Receive] : %s"), strRecipe);
			return;
		}

		if (!FrameGrabber.IsOpen(CameraTypeArea2))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));
			return;
		}

		m_bDesiccantInspRight = FALSE;
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA2); // Pos

		// Light & Grab
		strLog.Format(_T("case : Right Position ACM Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_bAcm[ACM_DESI_31_R] = TRUE;

		BeginStopWatch(IDX_AREA2);

		LightOnOff(InspectTypePositionRight, TRUE);
		CameraOnOff(InspectTypePositionRight, TRUE);

//		VisionProcess::CInspectProcess::InspectItem pQueueData;
//		pQueueData.pImgObj = &m_ImageObjectArea[IDX_AREA2];
//		pQueueData.pGO = m_pGOArea[IDX_AREA2];
//		pQueueData.nViewIndex = IDX_AREA2;
//		pQueueData.inspecttype = InspectTypePositionRight;
//		pQueueData.nGrabCnt = 0;
//
//		if (!m_pInspectProcess->PushInspectItem(pQueueData))
//			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), IDX_AREA2);
	}
	else if (strVisionCode == _T("32")) // Desiccant Cutting Left // Desiccant1
	{
		if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipe, TEACH_TAB_IDX_DESICCANT_CUTTING), TEACH_TAB_IDX_DESICCANT_CUTTING))
			WriteMessage(LogTypeNotification, _T("Desiccant Cutting Model Change OK [Receive] : %s"), strRecipe);
		else
		{
			WriteMessage(LogTypeError, _T("Desiccant Cutting Change Error [Receive] : %s"), strRecipe);
			return;
		}

		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera2 Open Fail !!"));
			return;
		}

		m_bDesiccantInspLeft = FALSE;
		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1); // Pos

		// Light & Grab
		strLog.Format(_T("case : Left Position2 ACM Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_bAcm[ACM_DESI_32_L] = TRUE;

		BeginStopWatch(IDX_AREA1);

//		VisionProcess::CInspectProcess::InspectItem pQueueData;
//		pQueueData.pImgObj = &m_ImageObjectArea[IDX_AREA1];
//		pQueueData.pGO = m_pGOArea[IDX_AREA1];
//		pQueueData.nViewIndex = IDX_AREA1;
//		pQueueData.inspecttype = InspectTypePositionLeft;
//		pQueueData.nGrabCnt = 0;
//
//		if (!m_pInspectProcess->PushInspectItem(pQueueData))
//			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), IDX_AREA1);

		LightOnOff(InspectTypePositionLeft, TRUE);
		CameraOnOff(InspectTypePositionLeft, TRUE);
	}
#endif
}

void CVisionSystem::ACM204(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber)
{
#ifdef RELEASE_4G
	CString strCommandCode(_T("ACM"));
	CString strSendBuf;
	CString strLog;
	CString strRecipe = _T("CALIBRATION");

	if (strVisionCode == _T("41")) // MBB
	{
		if (OnModelChange(CJobNumberData::Instance()->GetJobNumber(strRecipe, TEACH_TAB_IDX_MBB), TEACH_TAB_IDX_MBB))
			WriteMessage(LogTypeNotification, _T("MBB Model Change OK [Receive] : %s"), strRecipe);
		else
		{
			WriteMessage(LogTypeError, _T("MBB Model Change Error [Receive] : %s"), strRecipe);
			return;
		}

		if (!FrameGrabber.IsOpen(CameraTypeArea))
		{
			WRITE_LOG(WL_DEBUG, _T("Area Camera Open Fail !!"));
			return;
		}

		m_pMainView->ResetGraphic(CamTypeAreaScan, IDX_AREA1);

		// Light & Grab
		strLog.Format(_T("case : MBB ACM Start"));
		WriteMessage(LogTypeNormal, strLog);

		m_bAcm[ACM_MBB] = TRUE;

		BeginStopWatch(IDX_AREA1);

//		VisionProcess::CInspectProcess::InspectItem pQueueData;
//		pQueueData.pImgObj = &m_ImageObjectArea[IDX_AREA1];
//		pQueueData.pGO = m_pGOArea[IDX_AREA1];
//		pQueueData.nViewIndex = IDX_AREA1;
//		pQueueData.inspecttype = InspectTypeMBB;
//		pQueueData.nGrabCnt = 0;
//
//		if (!m_pInspectProcess->PushInspectItem(pQueueData))
//			WRITE_LOG(WL_ERROR, _T("m_pInspectProcess->PushInspectItem :: Fail[%d]"), IDX_AREA1);

		LightOnOff(InspectTypeMBB, TRUE);
		CameraOnOff(InspectTypeMBB, TRUE);
	}
#endif
}