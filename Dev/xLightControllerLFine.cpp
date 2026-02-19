#include "stdafx.h"
#include "xLightControllerLFine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SYSTIME_PER_MSEC 10000

using namespace VisionDevice;

CxLightControllerLFine::CxLightControllerLFine(void)
{
	m_bIsOpen = FALSE;
	m_cLightValue[0] = m_cLightValue[1] = m_cLightValue[2] = 0x00;
	m_ReceiveBuffer.Create( 4096 );
	m_bSentCommand = FALSE;
}

CxLightControllerLFine::~CxLightControllerLFine(void)
{
	Close();
}

void CxLightControllerLFine::OnReceive( BYTE* pData, int nSize )
{
	if (nSize > 0)
	{
		if (pData[0] == 'A')
		{
			m_bSentCommand = TRUE;
		}
	}
}

BOOL CxLightControllerLFine::Open( long lPort )
{
	if (m_bIsOpen)
	{
		TRACE( _T("CxLightControllerLFine::Open - Error Already open port!\n") );
		return FALSE;
	}

	if (!CxSerialComm::Create(lPort, CBR_9600))
	{
		TRACE( _T("CxLightControllerLFine::Open - Cannot create serial object!\n") );
		return FALSE;
	}

	BOOL bOpen = CxSerialComm::Open();
	if (!bOpen)
	{
		CxSerialComm::Destroy();
		return FALSE;
	}

	for (int i = 0; i < USE_LFINE_LCP100_CTRL_CH_MAX; ++i)
		LightEnable(i, TRUE, 0);

	m_bIsOpen = TRUE;
	return TRUE;
}

void CxLightControllerLFine::Close()
{
	if (!m_bIsOpen)
		return;

	for( int i=0; i<USE_LFINE_LCP100_CTRL_CH_MAX; ++i )
	{
		LightOff( i, 1000 );
		LightEnable( i, FALSE, 0 );
	}

	CxSerialComm::Close();
	CxSerialComm::Destroy();

	m_bIsOpen = FALSE;
}

BOOL CxLightControllerLFine::LightEnable( int nChannel, BOOL bEnable, DWORD dwTimeout )
{
	ASSERT( nChannel >= USE_LFINE_LCP100_CTRL_CH1 && nChannel < USE_LFINE_LCP100_CTRL_CH_MAX );
	CxCriticalSection::Owner Lock(m_csWriteLock);

//	nChannel += 1;

	char szCmd[256];
	_snprintf_s(szCmd, 256, "%c%c%c%c", 0x02, (nChannel + 0x30), 'o', 0x03);

	int nByteToWrite = (int)strlen(szCmd);

	m_bSentCommand = FALSE;
	int nWritten = CxSerialComm::WriteCommBlock( (BYTE*)szCmd, nByteToWrite );

	if (nWritten != nByteToWrite)
		return FALSE;

	if (dwTimeout == 0)
		return TRUE;

	FILETIME f;
	::GetSystemTimeAsFileTime(&f);
	DWORD tmStart = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);

	MSG msg;
	while (!m_bSentCommand)
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
			TRACE( _T("CxLightControllerLFine: Timeout\n") );
			return FALSE;
		}
	}

	return m_bSentCommand;
}

BOOL CxLightControllerLFine::LightOn( int nChannel, UINT nValue, DWORD dwTimeout )
{
	ASSERT( nChannel >= USE_LFINE_LCP100_CTRL_CH1 && nChannel < USE_LFINE_LCP100_CTRL_CH_MAX );
	CxCriticalSection::Owner Lock(m_csWriteLock);
	
//	nChannel += 1;

 	char szCmd[256];
	_snprintf_s(szCmd, 256, "%c%c%c%4.4d%c", 0x02, (nChannel + 0x30), 'w', nValue, 0x03);

	int nByteToWrite = (int)strlen(szCmd);

	m_bSentCommand = FALSE;
	int nWritten = CxSerialComm::WriteCommBlock( (BYTE*)szCmd, nByteToWrite );

	if (nWritten != nByteToWrite)
		return FALSE;

	if (dwTimeout == 0)
		return TRUE;

	FILETIME f;
	::GetSystemTimeAsFileTime(&f);
	DWORD tmStart = (DWORD)((f.dwLowDateTime | (((LONGLONG)f.dwHighDateTime) << 32)) / SYSTIME_PER_MSEC);

	MSG msg;
	while (!m_bSentCommand)
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
			TRACE( _T("CxLightControllerLFine: Timeout\n") );
			return FALSE;
		}
	}

	return m_bSentCommand;
}

BOOL CxLightControllerLFine::LightOff( int nChannel, DWORD dwTimeout )
{
	return LightOn( nChannel, 0, dwTimeout );
}