#pragma once

#include "Dev/xFrameGrabberSentech.h"
#include "Dev/xFrameGrabberMechEye.h"
#include "Dev/xFrameGrabberPanSight.h"
#include "Dev/xLightControllerLFine.h"

class IDeviceNotify
{
public:
	enum SerialDevice { 
		SerialDeviceLedCtrlLine1,
		SerialDeviceLedCtrlLine2,
		SerialDeviceLedCtrlLine3 };

	// Camera
	virtual void OnGrabFinishAreaScanCamera( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer ) = 0; // Area
	virtual void OnGrabFinish3DAreaScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) = 0;
	virtual void OnGrabFinish3DLineScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) = 0;

	// 조명 컨트롤러 접속 해제 이벤트
	virtual void OnDisconnectSerialDevice(SerialDevice device, DWORD dwError) = 0;
};

//////////////////////////////////////////////////////////////////////////
class CFrameGrabberSentechGigE : public VisionDevice::CFrameGrabberSentech
{
protected:
	IDeviceNotify* m_pNotify;
public:
	CFrameGrabberSentechGigE(IDeviceNotify* pNotify) : m_pNotify(pNotify) {}
	virtual ~CFrameGrabberSentechGigE() {}
	virtual void OnGrabFinish( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer ) override
	{
		ASSERT( m_pNotify );
		m_pNotify->OnGrabFinishAreaScanCamera( nWidth, nHeight, nChannel, (BYTE*)pBuffer );
	}
};

class CFrameGrabberMechEyeGigE : public VisionDevice::CxFrameGrabberMechEye
{
protected:
	IDeviceNotify* m_pNotify;
public:
	CFrameGrabberMechEyeGigE(IDeviceNotify* pNotify) : m_pNotify(pNotify) {}
	virtual ~CFrameGrabberMechEyeGigE() {}
	virtual void OnGrabFinish(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) override
	{
		ASSERT(m_pNotify);
		m_pNotify->OnGrabFinish3DAreaScanCamera(nWidth, nHeight, nChannel, (BYTE*)pBuffer);
	}
};

class CFrameGrabberPanSightGigE : public VisionDevice::CxFrameGrabberPanSight
{
protected:
	IDeviceNotify* m_pNotify;
public:
	CFrameGrabberPanSightGigE(IDeviceNotify *pNotify) : m_pNotify(pNotify) {}
	virtual ~CFrameGrabberPanSightGigE() {}
	
	virtual void OnGrabFinish(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) override
	{
		ASSERT(m_pNotify);
		m_pNotify->OnGrabFinish3DLineScanCamera(nWidth, nHeight, nChannel, (void*)pBuffer);
	}
};

class CLightControllerForAreaScan : public VisionDevice::CxLightControllerLFine
{
protected:
	IDeviceNotify* m_pNotify;
public:
	CLightControllerForAreaScan(IDeviceNotify* pNotify) : m_pNotify(pNotify) {}
	virtual ~CLightControllerForAreaScan() {}
	virtual void OnCommDisconnectDevice( DWORD dwRet ) override
	{
		ASSERT(m_pNotify);
		m_pNotify->OnDisconnectSerialDevice(IDeviceNotify::SerialDeviceLedCtrlLine1, dwRet);
	}
};

class CLightControllerForAreaScan2 : public VisionDevice::CxLightControllerLFine
{
protected:
	IDeviceNotify* m_pNotify;
public:
	CLightControllerForAreaScan2(IDeviceNotify* pNotify) : m_pNotify(pNotify) {}
	virtual ~CLightControllerForAreaScan2() {}
	virtual void OnCommDisconnectDevice( DWORD dwRet ) override
	{
		ASSERT(m_pNotify);
		m_pNotify->OnDisconnectSerialDevice(IDeviceNotify::SerialDeviceLedCtrlLine2, dwRet);
	}
};

class CLightControllerForAreaScan3 : public VisionDevice::CxLightControllerLFine
{
protected:
	IDeviceNotify* m_pNotify;
public:
	CLightControllerForAreaScan3(IDeviceNotify* pNotify) : m_pNotify(pNotify) {}
	virtual ~CLightControllerForAreaScan3() {}
	virtual void OnCommDisconnectDevice(DWORD dwRet) override
	{
		ASSERT(m_pNotify);
		m_pNotify->OnDisconnectSerialDevice(IDeviceNotify::SerialDeviceLedCtrlLine3, dwRet);
	}
};
