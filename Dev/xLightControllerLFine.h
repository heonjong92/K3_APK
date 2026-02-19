#pragma once

#include <XUtil/Comm/xSerialComm.h>
#include <XUtil/Comm/xIoBuffer.h>
#include <XUtil/xCriticalSection.h>

namespace VisionDevice {

class CxLightControllerLFine : protected CxSerialComm
{
protected:
	BOOL	m_bIsOpen;
	BYTE	m_cLightValue[3];
	CxIoBuffer	m_ReceiveBuffer;
	BOOL	m_bSentCommand;
	CxCriticalSection	m_csWriteLock;

	virtual void OnCommClose ( DWORD dwRet ) override {};
	virtual void OnCommReceive ( ReceivedPacket* pPacket ) override {};
	virtual void OnCommDisconnectDevice( DWORD dwRet ) override {};

	virtual void OnReceive( BYTE* pData, int nSize ) override;
public:
	CxLightControllerLFine(void);
	virtual ~CxLightControllerLFine(void);

	BOOL Open( long lPort );
	void Close();
	BOOL IsOpen() { return CxSerialComm::IsOpen(); }

	BOOL LightEnable( int nChannel, BOOL bEnable, DWORD dwTimeout );
	BOOL LightOn( int nChannel, UINT nValue, DWORD dwTimeout );
	BOOL LightOff( int nChannel, DWORD dwTimeout );
};

}