#include "stdafx.h"
#include "VisionStatusServer.h"

#include "VisionStatusPacket.h"

#include "VisionSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CVisionStatusServer::CCheckAliveClientThread::CCheckAliveClientThread( ClientMapper& mapper, CxCriticalSection& cs, const DWORD& timeout, CVisionStatusServer* pServer ) :
	m_ClientMapper(mapper),
	m_csClientMapper(cs),
	m_pServer(pServer),
	m_dwAliveTimeout(timeout),
	m_bTerminate(FALSE)
{
}

CVisionStatusServer::CCheckAliveClientThread::~CCheckAliveClientThread()
{
}

int CVisionStatusServer::CCheckAliveClientThread::Run()
{
	TRACE( _T("CVisionStatusServer::CCheckAliveClientThread: Started...\r\n") );
	int nCount = 0;
	volatile BOOL bCheck = FALSE;
	while (!m_bTerminate)
	{
		if ( nCount >= 9 )
		{
			nCount = 0;
			
			CheckInactiveClient();
		}
		else
			nCount++;

//		Sleep(10);
	}
	TRACE( _T("CVisionStatusServer::CCheckAliveClientThread: Terminate...\r\n") );
	return 0;
}

void CVisionStatusServer::CCheckAliveClientThread::InitiateShutdown()
{
	m_bTerminate = TRUE;
}

void CVisionStatusServer::CCheckAliveClientThread::WaitForShutdownToComplete()
{
	InitiateShutdown();
	if ( !Wait( 500 ) )
	{
		TRACE( _T("CVisionStatusServer::CCheckAliveClientThread: Force Terminate\r\n") );
		Terminate();
	}

	if (m_hThread != 0)
	{
		::CloseHandle(m_hThread);
		m_hThread = 0;
	}
}

void CVisionStatusServer::CCheckAliveClientThread::CheckInactiveClient()
{
	CxCriticalSection::Owner Lock(m_csClientMapper);

	DWORD dwTick = GetTickCount();
	for (ClientMapper::iterator iter = m_ClientMapper.begin() ; iter != m_ClientMapper.end() ; ++iter)
	{
		const CString& strKey = iter->first;
		ClientInfo& cInfo = iter->second;
		
		if (cInfo.IsValid && ((dwTick - cInfo.TimeStamp) > m_dwAliveTimeout))
		{
//			cInfo.IsValid = FALSE;
//			TRACE( _T("CVisionStatusServer::CCheckAliveClientThread::CheckInactiveClient - %s:%d is inactive!\n"), cInfo.pContext->strClientIP, cInfo.pContext->nClientPort );
//			m_pServer->DisconnectConnection(cInfo.pContext);
//			iter = m_ClientMapper.erase(iter);
		}
		else
		{
			
		}
	}
}

void CVisionStatusServer::CCheckAliveClientThread::Start()
{
	CxThread::Start();
	::SetThreadPriority( m_hThread, THREAD_PRIORITY_LOWEST );
}

CVisionStatusServer::CVisionStatusServer(void) : 
	m_dwAliveTimeout(100000),
	m_CheckAliveClientThread(m_ClientMapper, m_csClientMapper, m_dwAliveTimeout, this)
{
}

CVisionStatusServer::~CVisionStatusServer(void)
{
}

void CVisionStatusServer::ProcessBuffer( ServerThreadContext* pContext, char* pReceiveBuf, int nRecvBytes )
{
	if (!pContext->IoBuffer.Add(pReceiveBuf, nRecvBytes))
	{
		TRACE( _T("CVisionStatusServer::ProcessBuffer overflow!\n") );
		pContext->IoBuffer.Clear();
	}

	while (pContext->IoBuffer.GetSize() > 0)
	{
		char* pBuffer = const_cast<char*>( pContext->IoBuffer.GetBuffer() );
		int bufferSize = pContext->IoBuffer.GetSize();

		int startPos = 0;
		int endPos = -1;

		// '^' 문자를 찾기 위한 루프
		for (int i = 0; i < bufferSize; i++)
		{
			if (pBuffer[i] == '^') // 구분자 발견
			{
				endPos = i;
				int messageLength = endPos - startPos + 1; // '^' 포함

				if (messageLength > 0)
				{
					CVisionSystem::Instance()->ProcessBuffer(pContext, pBuffer + startPos, messageLength);
				}

				startPos = endPos + 1; // 다음 메시지 시작 위치 설정
			}
		}

		// 남은 데이터 처리
		if (startPos < bufferSize)
		{
			pContext->IoBuffer.Remove(0, startPos);
		}
		else
		{
			pContext->IoBuffer.Clear();
			break;
		}
	}
}

void CVisionStatusServer::OnStopServer()
{
	m_ClientMapper.clear();
	m_CheckAliveClientThread.WaitForShutdownToComplete();
}

void CVisionStatusServer::OnStartServer()
{
	m_CheckAliveClientThread.Start();
}

void CVisionStatusServer::OnPacketReceived( ServerThreadContext* pContext, CxIoBuffer* PacketBuffer )
{
	char* pBuffer = const_cast<char*> (PacketBuffer->GetBuffer());

//	if (PacketBuffer->GetSize() == AlivePacketSize)
//	{
//		UpdateTimeStamp(pContext);
//		TRACE( _T("[%p] %s:%d client alive!\n"), pContext, pContext->strClientIP, pContext->nClientPort );
//		//DisconnectConnection(pContext);
//	}
}

void CVisionStatusServer::UpdateTimeStamp( ServerThreadContext* pContext )
{
	CString strKey;
	strKey.Format( _T("%s:%d"), pContext->strClientIP, pContext->nClientPort );
	CxCriticalSection::Owner Lock(m_csClientMapper);
	ClientMapper::iterator iterFind = m_ClientMapper.find( strKey );
	if (iterFind == m_ClientMapper.end())
	{
		TRACE( _T("CVisionStatusServer::UpdateTimeStamp - Fatal Error: Unknown client!\n") );
		ASSERT(FALSE);
		return;
	}

	iterFind->second.TimeStamp = GetTickCount();
}

void CVisionStatusServer::OnClientConnected( ServerThreadContext* pContext, CxString strClientIP, int nPort )
{
	CString strKey;
	strKey.Format( _T("%s:%d"), strClientIP, nPort );

	CxCriticalSection::Owner Lock(m_csClientMapper);
	ClientMapper::iterator iterFind = m_ClientMapper.find( strKey );
	if (iterFind != m_ClientMapper.end())
	{
		TRACE( _T("CVisionStatusServer::OnClientConnected - Fatal Error: Already connected client!\n") );
		DisconnectConnection(pContext);
		return;
	}
	TRACE( _T("CVisionStatusServer::OnClientConnected - [%p] %s:%d client connected\n"), pContext, strClientIP, nPort );
	ClientInfo cInfo;
	cInfo.pContext = pContext;
	cInfo.IsValid = TRUE;
	cInfo.TimeStamp = GetTickCount();
	m_ClientMapper.insert( ClientMapper::value_type(strKey, cInfo) );

	CString strConnect;
	strConnect.Format( _T("Connect Client [%s]"), strKey);
	CVisionSystem::Instance()->WriteMessage(LogTypeNotification, strConnect);
}

void CVisionStatusServer::OnClientDisconnected( CxString strClientIP, int nPort )
{
	CString strKey;
	strKey.Format( _T("%s:%d"), strClientIP, nPort );

	CxCriticalSection::Owner Lock(m_csClientMapper);
	ClientMapper::iterator iterFind = m_ClientMapper.find( strKey );
	if (iterFind == m_ClientMapper.end())
	{
		TRACE( _T("CVisionStatusServer::OnClientConnected - Fatal Error: Already disconnected client!\n") );
		return;
	}
	TRACE( _T("CVisionStatusServer::OnClientDisconnected - %s:%d client disconnected\n"), strClientIP, nPort );
	m_ClientMapper.erase(iterFind);

	CString strDisConnect;
	strDisConnect.Format(_T("DisConnect Client [%s]"), strKey);
	CVisionSystem::Instance()->WriteMessage(LogTypeError, strDisConnect);
}

void CVisionStatusServer::SendPacketToClients( char *pOutBuffer, int nOutBufferSize )
{
	CxCriticalSection::Owner Lock(m_csClientMapper);
	for (ClientMapper::iterator iter=m_ClientMapper.begin() ; iter != m_ClientMapper.end() ; ++iter)
	{
		if (!iter->second.IsValid)
		{
			DisconnectConnection(iter->second.pContext);
		}
		else
		{
			SendBuffer( iter->second.pContext->hSocket, pOutBuffer, nOutBufferSize );
		}
	}
}									 

const int CVisionStatusServer::GetClientCount()
{
	CxCriticalSection::Owner Lock(m_csClientMapper);

	return (int)m_ClientMapper.size();
}