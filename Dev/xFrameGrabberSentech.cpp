#include "stdafx.h"

#include "xFrameGrabberSentech.h"
#include "../VisionSystem.h"

#include <XUtil/xUtils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionDevice;

CFrameGrabberSentech::CFrameGrabberSentech(void) 
{
	for( int i=0; i<CameraTypeMax; ++i )
	{
		m_bOpen[i] = FALSE;
		m_bLive[i] = FALSE;

		m_nSizeX[i] = 0;
		m_nSizeY[i] = 0;

		m_nCameraIndex[i] = -1;
	}

	m_pIStDeviceReleasable = NULL;

	m_bRotate = FALSE;
}

CFrameGrabberSentech::~CFrameGrabberSentech(void)
{	
	for( int i=0; i<CameraTypeMax; ++i )
	{
		Close(i);
	}
}

BOOL CFrameGrabberSentech::FindCameraIndex( CString strSerialNo, OUT UINT& nViewIndex )
{
	CSystemConfig::CameraSerialNo& SerialNoConfig = CSystemConfig::Instance()->GetCameraSerialNo();
	for( int i=0; i<CameraTypeMax; ++i )
	{
		if( strSerialNo == SerialNoConfig.strSerialNo[i] )
		{
			nViewIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFrameGrabberSentech::Open(UINT nInIndex, OUT UINT& nViewIndex )
{
	BOOL bReval = FALSE;
	try
	{		
		const uint32_t nCount = StSystemVendor_Count;
		for (uint32_t i = StSystemVendor_Default; i < nCount; ++i)
		{
			EStSystemVendor_t eStSystemVendor = (EStSystemVendor_t)i;
			try
			{
				m_objIStSystemPtrList.Register(CreateIStSystem(eStSystemVendor, StInterfaceType_All));
			}
			catch (const GenICam::GenericException &e)
			{
				if (eStSystemVendor == StSystemVendor_Default)
				{
					e.GetDescription();
				}
			}
		}

 		IStSystem *pIStSystem = m_objIStSystemPtrList[0];
		m_pIStDevice[nInIndex].Reset( pIStSystem->CreateFirstIStDevice() );

		m_pIStDataStream[nInIndex] = m_pIStDevice[nInIndex]->CreateIStDataStream(0);
		if (m_pIStDataStream[nInIndex] != NULL)
		{
			if( FindCameraIndex( (CString)m_pIStDevice[nInIndex]->GetIStDeviceInfo()->GetSerialNumber(), nViewIndex ) )
			{
				SetCameraIndex( nViewIndex, nInIndex );

				//Register callback function to receive images.
				if(nViewIndex == 0)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction, (void*)this);
				else if(nViewIndex == 1)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction2, (void*)this);
				else if(nViewIndex == 2)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction3, (void*)this);
				else if(nViewIndex == 3)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction4, (void*)this);
				else if(nViewIndex == 4)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction5, (void*)this);
				else if(nViewIndex == 5)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction6, (void*)this);
				else if(nViewIndex == 6)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction7, (void*)this);
				else if(nViewIndex == 7)
					RegisterCallback(m_pIStDataStream[nInIndex], &OnStCallbackCFunction8, (void*)this);
			}
			else
			{
				return FALSE;
			}
		}

		m_bOpen[nInIndex] = TRUE;

		CNodeMapPtr pINodeMapRemote(m_pIStDevice[nInIndex]->GetRemoteIStPort()->GetINodeMap());

		// Get Camera Type 
		CString strTLType = (CString)m_pIStDevice[nInIndex]->GetIStDeviceInfo()->GetTLType();

		bReval = TRUE;
	}
    catch (const GenICam::GenericException &e)
    {
		CString str = (CString)e.GetDescription();
		m_bOpen[nInIndex] = FALSE;
		bReval = FALSE;
    }

	return bReval;
}

void CFrameGrabberSentech::Close( UINT nIndex )
{
	nIndex = GetCameraIndex(nIndex);
	try
	{
		if( nIndex >= CameraTypeMax || nIndex < 0 )
			return;

		if( !m_bOpen[nIndex])
			return;

		m_bOpen[nIndex] = FALSE;

		if( m_bLive[nIndex] )
		{
			m_pIStDevice[nIndex]->AcquisitionStop();
			m_pIStDataStream[nIndex]->StopAcquisition();
		}

		m_pIStDataStream[nIndex].Reset(NULL);
		m_pIStDevice[nIndex].Reset(NULL);
	}
	catch (const GenICam::GenericException &e)
	{
		CString strDescription = (CString)e.GetDescription();
		CString strErrorMessge;
		strErrorMessge.Format(_T("Camera(%d) Close Error"), nIndex);

		CVisionSystem::Instance()->WriteMessage( LogTypeError, strErrorMessge );
	}
}

BOOL CFrameGrabberSentech::Live( UINT nIndex )
{	
	nIndex = GetCameraIndex(nIndex);
	try
	{
		if( nIndex >= CameraTypeMax || nIndex < 0 )
			return FALSE;

		if (m_bLive[nIndex])
			return TRUE;

		m_pIStDevice[nIndex]->AcquisitionStop();
		m_pIStDataStream[nIndex]->StopAcquisition();

		m_pIStDataStream[nIndex]->StartAcquisition();
		m_pIStDevice[nIndex]->AcquisitionStart();

		m_bLive[nIndex] = TRUE;
	}
	catch (const GenICam::GenericException &e)
	{
		CString strDescription = (CString)e.GetDescription();
		CString strErrorMessge;
		strErrorMessge.Format(_T("Camera(%d) Live Error"), nIndex);

		CVisionSystem::Instance()->WriteMessage( LogTypeError, strErrorMessge );

		return FALSE;
	}

	return TRUE;
}

BOOL CFrameGrabberSentech::Grab( UINT nIndex )
{	
	nIndex = GetCameraIndex(nIndex);
	try
	{
		if( nIndex >= CameraTypeMax || nIndex < 0 )
			return FALSE;

		m_pIStDevice[nIndex]->AcquisitionStop();
		m_pIStDataStream[nIndex]->StopAcquisition();

		m_pIStDataStream[nIndex]->StartAcquisition(1);
		m_pIStDevice[nIndex]->AcquisitionStart();
	}
	catch (const GenICam::GenericException &e)
	{
		CString strDescription = (CString)e.GetDescription();
		CString strErrorMessge;
		strErrorMessge.Format(_T("Camera(%d) Grab Error"), nIndex);

		CVisionSystem::Instance()->WriteMessage( LogTypeError, strErrorMessge );

		return FALSE;
	}

	return TRUE;
}

BOOL CFrameGrabberSentech::Idle( UINT nIndex )
{
	nIndex = GetCameraIndex(nIndex);
	try
	{
		if( nIndex >= CameraTypeMax || nIndex < 0 )
			return FALSE;

		if( !m_bLive[nIndex] )
			return TRUE;

		m_pIStDevice[nIndex]->AcquisitionStop();
		m_pIStDataStream[nIndex]->StopAcquisition();

		m_bLive[nIndex] = FALSE;
	}
	catch (const GenICam::GenericException &e)
	{
		CString strDescription = (CString)e.GetDescription();
		CString strErrorMessge;
		strErrorMessge.Format(_T("Camera(%d) Idle Error"), nIndex);

		CVisionSystem::Instance()->WriteMessage( LogTypeError, strErrorMessge );

		return FALSE;
	}
	
	return TRUE;
}

BOOL CFrameGrabberSentech::IsOpen( UINT nIndex )
{
	nIndex = GetCameraIndex(nIndex);

	if( nIndex >= CameraTypeMax || nIndex < 0 )
		return FALSE;

	// SetEnumeration( m_pIStDevice[nIndex]->GetRemoteIStPort()->GetINodeMap(), nIndex, "EventNotification", "On" );

	return m_bOpen[nIndex]; 
}

BOOL CFrameGrabberSentech::IsLive( UINT nIndex )
{
	nIndex = GetCameraIndex(nIndex);

	if( nIndex >= CameraTypeMax || nIndex < 0 )
		return FALSE;

	return m_bLive[nIndex];
}

void CFrameGrabberSentech::OnGrabFinish( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer )
{
	TRACE( _T("CFrameGrabberSentech::OnGrabFinish - Warning! Index: %dx%d, Channel=%d, Depth=%d, %p\n"), nWidth, nHeight, nChannel, pBuffer );
}

BOOL CFrameGrabberSentech::ExposureChange(int nIndex, double dExposure)
{
	if (!IsOpen(nIndex))
		return FALSE;

	CNodeMapPtr pINodeMapRemote(m_pIStDevice[nIndex]->GetRemoteIStPort()->GetINodeMap());

	GenApi::CNodePtr pINode(pINodeMapRemote->GetNode("ExposureTime"));

	//Checks if a node is writable
	bool isWritable = GenApi::IsWritable(pINode);
	//Checks if a node is readable
	bool isReadable = GenApi::IsReadable(pINode);
	//Checks if a node is available
	bool isAvailable = GenApi::IsAvailable(pINode);

	//SmartPointer for IFloat interface pointer
	GenApi::CFloatPtr pIFloat(pINode);

	//Get minimum value allowed
	double dblMin = pIFloat->GetMin();

	//Get maximum value allowed
	double dblMax = pIFloat->GetMax();

	//Get node value
	double dblExposureTime = pIFloat->GetValue();

	//Set node value
	pIFloat->SetValue(dExposure);

	return TRUE;
}

BOOL CFrameGrabberSentech::GrabChange(BOOL bUseSoft,int nIndex,int nTriggerType)
{
	if( !IsOpen(nIndex) )
		return FALSE;

	// Get the INodeMap interface pointer for the camera settings.
	CNodeMapPtr pINodeMapRemote(m_pIStDevice[nIndex]->GetRemoteIStPort()->GetINodeMap());

	// Set the TriggerSelector to FrameStart.
	try
	{
		SetEnumeration(pINodeMapRemote, nIndex, "TriggerSelector", "FrameStart");
	}
	catch (const  GenICam::GenericException &)
	{
		//If "FrameStart" is not supported, use "ExposureStart".
		SetEnumeration(pINodeMapRemote, nIndex, "TriggerSelector", "ExposureStart");
	}

	if (bUseSoft)
	{
//		m_pIStDevice[nIndex]->AcquisitionStop();
//		m_pIStDataStream[nIndex]->StopAcquisition();
		// Set the TriggerMode to On.
		if(!m_bLive[nIndex])
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerMode", "Off");
		// Set the TriggerSource to Software.
		//SetEnumeration(pINodeMapRemote, "TriggerSource", "Software");
		// Get the ICommand interface pointer for the TriggerSoftware node.
		//CCommandPtr pICommandTriggerSoftware(pINodeMapRemote->GetNode("TriggerSoftware"));
	}
	else
	{
		
		// Set the TriggerMode to On.
		if (nTriggerType == 2)
		{
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerMode", "On");
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerSource", "Line0");
			m_pIStDevice[nIndex]->AcquisitionStop();
			m_pIStDataStream[nIndex]->StopAcquisition();

			m_pIStDataStream[nIndex]->StartAcquisition();
			m_pIStDevice[nIndex]->AcquisitionStart();
		}
		else if (nTriggerType == 3)
		{
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerMode", "On");
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerSource", "Line1");
			m_pIStDevice[nIndex]->AcquisitionStop();
			m_pIStDataStream[nIndex]->StopAcquisition();

			m_pIStDataStream[nIndex]->StartAcquisition();
			m_pIStDevice[nIndex]->AcquisitionStart();
		}
		else if (nTriggerType == 4)
		{
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerMode", "On");
			SetEnumeration(pINodeMapRemote, nIndex, "TriggerSource", "Line2");
			m_pIStDevice[nIndex]->AcquisitionStop();
			m_pIStDataStream[nIndex]->StopAcquisition();

			m_pIStDataStream[nIndex]->StartAcquisition();
			m_pIStDevice[nIndex]->AcquisitionStart();
		}
		
	}

	return TRUE;
}

void CFrameGrabberSentech::SetEnumeration(INodeMap *pINodeMap, int nIndex, const char *szEnumerationName, const char *szValueName)
{
	try
	{
		// Get the IEnumeration interface pointer.
		CEnumerationPtr pIEnumeration(pINodeMap->GetNode(szEnumerationName));

		// Get the IEnumEntry interface pointer for the specified name.
		CEnumEntryPtr pIEnumEntry(pIEnumeration->GetEntryByName(szValueName));

		// Get the integer value corresponding to the set value name using the IEnumEntry interface pointer.
		// Update the settings using the IEnumeration interface pointer.

		pIEnumeration->SetIntValue(pIEnumEntry->GetValue());
	}
	catch (const GenICam::GenericException &e)
	{
		CString strDescription = (CString)e.GetDescription();
		CString strErrorMessge;
		strErrorMessge.Format(_T("Camera(%d) SetEnumeration Error"), nIndex);

		CVisionSystem::Instance()->WriteMessage( LogTypeError, strErrorMessge );
	}
}
