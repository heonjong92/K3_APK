#include "stdafx.h"

#include "VisionInterfaces.h"
#include "VisionSystem.h"
#include "SystemConfig.h"
#include "LightInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionDevice;
using namespace VisionInterface;

void FrameGrabber::ChangeExposure(CameraType camType, double dExposure)
{
	m_pSystem->m_AreaScanCamera.ExposureChange((int)camType, dExposure);
}

void FrameGrabber::SetPageCount(CameraType camType)
{
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	switch (camType)
	{
	case CameraTypeLine:		m_pSystem->m_3DLineScanCamera.SetPageCount((UINT)CameraTypeLine);		break;
	
#ifdef RELEASE_1G
	case CameraTypeLine2:		m_pSystem->m_3DLineScanCamera.SetPageCount((UINT)CameraTypeLine2);		break;
	case CameraTypeLine3:		m_pSystem->m_3DLineScanCamera.SetPageCount((UINT)CameraTypeLine3);		break;
	case CameraTypeLine4:		m_pSystem->m_3DLineScanCamera.SetPageCount((UINT)CameraTypeLine4);		break;

#endif
	default:
		break;
	}
#endif
}

void FrameGrabber::Live( CameraType camType, int nGrabLength/*=0*/ )
{
	switch (camType)
	{
#ifdef RELEASE_1G
	case CameraTypeArea:
		if (!m_pSystem->m_AreaScanCamera.IsOpen( camType ))
			return;
		if (m_pSystem->m_AreaScanCamera.IsLive( camType ))
			return;
		if (m_pSystem->m_AreaScanCamera.Live( camType ))
			m_pSystem->m_GrabStatus[camType] = GrabStatusLive;
		break;
	case CameraType3DArea:
		if (!m_pSystem->m_3DAreaScanCamera.IsOpen( 0 ))
			return;
		if (m_pSystem->m_3DAreaScanCamera.IsLive( 0 ))
			return;
		if (m_pSystem->m_3DAreaScanCamera.Live( 0 ))
			m_pSystem->m_GrabStatus[camType] = GrabStatusLive;
		break;
	case CameraTypeLine:
	case CameraTypeLine2:
	case CameraTypeLine3:
	case CameraTypeLine4:
		if (!m_pSystem->m_3DLineScanCamera.IsOpen(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.IsLive(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.Live(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusLive;
		break;
		
#elif RELEASE_SG
	case CameraTypeLine:
		if (!m_pSystem->m_3DLineScanCamera.IsOpen(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.IsLive(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.Live(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusLive;
		break;

#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	case CameraTypeArea:
	case CameraTypeArea2:
		if (!m_pSystem->m_AreaScanCamera.IsOpen(camType))
			return;
		if (m_pSystem->m_AreaScanCamera.IsLive(camType))
			return;
		if (m_pSystem->m_AreaScanCamera.Live(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusLive;
		break;

#elif RELEASE_3G
	case CameraTypeArea:
	case CameraTypeArea2:
	case CameraTypeArea3:
		if (!m_pSystem->m_AreaScanCamera.IsOpen(camType))
			return;
		if (m_pSystem->m_AreaScanCamera.IsLive(camType))
			return;
		if (m_pSystem->m_AreaScanCamera.Live(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusLive;
		break;

#endif
	default:
		break;
	}
}

void FrameGrabber::Grab( CameraType camType, int nGrabLength/*=0*/  )
{
	switch (camType)
	{
#ifdef RELEASE_1G
	case CameraTypeArea:
		if (!m_pSystem->m_AreaScanCamera.IsOpen( camType ))
			return;
		if (m_pSystem->m_AreaScanCamera.IsLive( camType ))
			return;
		if( m_pSystem->m_AreaScanCamera.Grab( camType ) )
			m_pSystem->m_GrabStatus[camType] = GrabStatusGrab;
		break;
	case CameraType3DArea:
		if (!m_pSystem->m_3DAreaScanCamera.IsOpen( 0 ))
			return;
		if (m_pSystem->m_3DAreaScanCamera.IsLive( 0 ))
			return;
		if (m_pSystem->m_3DAreaScanCamera.Grab( 0 ))
			m_pSystem->m_GrabStatus[camType] = GrabStatusGrab;
		break;
	case CameraTypeLine:
	case CameraTypeLine2:
	case CameraTypeLine3:
	case CameraTypeLine4:
		if (!m_pSystem->m_3DLineScanCamera.IsOpen(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.IsLive(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.Grab(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusGrab;
		break;

#elif RELEASE_SG
	case CameraTypeLine:
		if (!m_pSystem->m_3DLineScanCamera.IsOpen(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.IsLive(camType))
			return;
		if (m_pSystem->m_3DLineScanCamera.Grab(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusGrab;
		break;
		
#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	case CameraTypeArea:
	case CameraTypeArea2:
		if (!m_pSystem->m_AreaScanCamera.IsOpen(camType))
			return;
//		if (m_pSystem->m_AreaScanCamera.IsLive(camType))
//			return;
		if (m_pSystem->m_AreaScanCamera.Grab(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusGrab;
		break;

#elif RELEASE_3G
	case CameraTypeArea:
	case CameraTypeArea2:
	case CameraTypeArea3:
		if (!m_pSystem->m_AreaScanCamera.IsOpen(camType))
			return;
//		if (m_pSystem->m_AreaScanCamera.IsLive(camType))
//			return;
		if (m_pSystem->m_AreaScanCamera.Grab(camType))
			m_pSystem->m_GrabStatus[camType] = GrabStatusGrab;
		break;

#endif
	default:
		break;
	}
}

void FrameGrabber::Idle( CameraType camType )
{
	switch (camType)
	{
#ifdef RELEASE_1G
	case CameraTypeArea:
		m_pSystem->m_AreaScanCamera.Idle( camType );
		m_pSystem->m_GrabStatus[camType] = GrabStatusIdle;
		break;
	case CameraType3DArea:
		m_pSystem->m_3DAreaScanCamera.Idle( 0 );
		m_pSystem->m_GrabStatus[camType] = GrabStatusIdle;
		break;
	case CameraTypeLine:
	case CameraTypeLine2:
	case CameraTypeLine3:
	case CameraTypeLine4:
		m_pSystem->m_3DLineScanCamera.Idle(camType);
		m_pSystem->m_GrabStatus[camType] = GrabStatusIdle;
		break;

#elif RELEASE_SG
	case CameraTypeLine:
		m_pSystem->m_3DLineScanCamera.Idle(camType);
		m_pSystem->m_GrabStatus[camType] = GrabStatusIdle;
		break;
		
#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	case CameraTypeArea:
	case CameraTypeArea2:
		m_pSystem->m_AreaScanCamera.Idle(camType);
		m_pSystem->m_GrabStatus[camType] = GrabStatusIdle;
		break;

#elif RELEASE_3G
	case CameraTypeArea:
	case CameraTypeArea2:
	case CameraTypeArea3:
		m_pSystem->m_AreaScanCamera.Idle(camType);
		m_pSystem->m_GrabStatus[camType] = GrabStatusIdle;
		break;

#endif
	default:
		break;
	}
}

BOOL FrameGrabber::IsLive( CameraType camType )
{
	if (m_pSystem->m_GrabStatus[camType] == GrabStatusLive)
		return TRUE;

	return FALSE;
}

BOOL FrameGrabber::IsOpen( CameraType camType )
{
	switch (camType)
	{
#ifdef RELEASE_1G
	case CameraTypeArea:
		return m_pSystem->m_AreaScanCamera.IsOpen( camType );
		break;
	case CameraType3DArea:
		return m_pSystem->m_3DAreaScanCamera.IsOpen( 0 );
		break;
	case CameraTypeLine:
	case CameraTypeLine2:
	case CameraTypeLine3:
	case CameraTypeLine4:
		return m_pSystem->m_3DLineScanCamera.IsOpen( camType );
		break;

#elif RELEASE_SG
	case CameraTypeLine:
		return m_pSystem->m_3DLineScanCamera.IsOpen( camType );
		break;

#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	case CameraTypeArea:
	case CameraTypeArea2:
		return m_pSystem->m_AreaScanCamera.IsOpen((int)camType);
		break;

#elif RELEASE_3G
	case CameraTypeArea:
	case CameraTypeArea2:
	case CameraTypeArea3:
		return m_pSystem->m_AreaScanCamera.IsOpen((int)camType);
		break;

#endif

	default:
		break;
	}

	return FALSE;
}

BOOL LightController::TurnOn( LedCtrlType type, int nChannel, int nBrightness )
{
	BOOL bRet = FALSE;

	switch (type)
	{
	case LedCtrlTypeArea1:
		if (m_pSystem->m_LightControllerForArea.IsOpen())
			bRet = m_pSystem->m_LightControllerForArea.LightOn( nChannel, nBrightness, 0 );
		else
			WRITE_LOG(WL_DEBUG, _T("Light Controller is not Opend[LedCtrlTypeArea1]"));
		break;
	case LedCtrlTypeArea2:
		if (m_pSystem->m_LightControllerForArea2.IsOpen())
			bRet = m_pSystem->m_LightControllerForArea2.LightOn( nChannel, nBrightness, 0 );
		else
			WRITE_LOG(WL_DEBUG, _T("Light Controller is not Opend[LedCtrlTypeArea2]"));
		break;
	case LedCtrlTypeArea3:
		if (m_pSystem->m_LightControllerForArea3.IsOpen())
			bRet = m_pSystem->m_LightControllerForArea3.LightOn(nChannel, nBrightness, 0);
		else
			WRITE_LOG(WL_DEBUG, _T("Light Controller is not Opend[LedCtrlTypeArea3]"));
		break;
	}

	return bRet;
}

BOOL LightController::TurnOff( LedCtrlType type, int nChannel )
{
	BOOL bRet = FALSE;

	switch (type)
	{
	case LedCtrlTypeArea1:
		if (m_pSystem->m_LightControllerForArea.IsOpen())
		{
			TRACE( _T("m_LightControllerForArea Turn off\n") );
			bRet = m_pSystem->m_LightControllerForArea.LightOff( nChannel, 0 );
		}
		break;
	case LedCtrlTypeArea2:
		if (m_pSystem->m_LightControllerForArea2.IsOpen())
		{
			TRACE( _T("m_LightControllerForArea2 Turn off\n") );
			bRet = m_pSystem->m_LightControllerForArea2.LightOff( nChannel, 0 );
		}
		break;
	case LedCtrlTypeArea3:
		if (m_pSystem->m_LightControllerForArea3.IsOpen())
		{
			TRACE(_T("m_LightControllerForArea3 Turn off\n"));
			bRet = m_pSystem->m_LightControllerForArea3.LightOff(nChannel, 0);
		}
		break;
	}

	return bRet;
}

BOOL LightController::IsConnected( LedCtrlType type )
{
	switch (type)
	{
	case LedCtrlTypeArea1:
		if (m_pSystem->m_LightControllerForArea.IsOpen())
		{
			return TRUE;
		}
		break;
	case LedCtrlTypeArea2:
		if (m_pSystem->m_LightControllerForArea2.IsOpen())
		{
			return TRUE;
		}
		break;
	case LedCtrlTypeArea3:
		if (m_pSystem->m_LightControllerForArea3.IsOpen())
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL LightController::ChangePort( LedCtrlType ledCtrl, int nPort )
{
	switch (ledCtrl)
	{
	case LedCtrlTypeArea1:
		if (m_pSystem->m_LightControllerForArea.IsOpen())
		{
			m_pSystem->m_LightControllerForArea.Close();
		}
		return m_pSystem->m_LightControllerForArea.Open(nPort);

	case LedCtrlTypeArea2:
		if (m_pSystem->m_LightControllerForArea2.IsOpen())
		{
			m_pSystem->m_LightControllerForArea2.Close();
		}
		return m_pSystem->m_LightControllerForArea2.Open(nPort);

	case LedCtrlTypeArea3:
		if (m_pSystem->m_LightControllerForArea3.IsOpen())
		{
			m_pSystem->m_LightControllerForArea3.Close();
		}
		return m_pSystem->m_LightControllerForArea3.Open(nPort);
	}

	return FALSE;
}

BOOL LightController::ClosePort( LedCtrlType ledCtrl )
{
	switch (ledCtrl)
	{
	case LedCtrlTypeArea1:
		if (m_pSystem->m_LightControllerForArea.IsOpen())
		{
			m_pSystem->m_LightControllerForArea.Close();
		}
		return TRUE;

	case LedCtrlTypeArea2:
		if (m_pSystem->m_LightControllerForArea2.IsOpen())
		{
			m_pSystem->m_LightControllerForArea2.Close();
		}
		return TRUE;

	case LedCtrlTypeArea3:
		if (m_pSystem->m_LightControllerForArea3.IsOpen())
		{
			m_pSystem->m_LightControllerForArea3.Close();
		}
		return TRUE;
	}

	return FALSE;
}