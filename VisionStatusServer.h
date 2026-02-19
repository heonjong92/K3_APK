#pragma once

#include <XUtil/Comm/xServerSocketTCP.h>
#include <XUtil/xCriticalSection.h>
#include <XUtil/xThread.h>
#include <map>

class CVisionStatusServer : public CxServerSocketTCP
{
protected:
	virtual void ProcessBuffer( ServerThreadContext* pContext, char* pReceiveBuf, int nRecvBytes ) override;

	struct ClientInfo
	{
		ServerThreadContext*	pContext;
		DWORD					TimeStamp;
		BOOL					IsValid;
	};

	typedef std::map<CString, ClientInfo> ClientMapper;
	ClientMapper		m_ClientMapper;
	CxCriticalSection	m_csClientMapper;

	void UpdateTimeStamp( ServerThreadContext* pContext );

	const DWORD m_dwAliveTimeout;

	class CCheckAliveClientThread : public CxThread
	{
	protected:
		volatile BOOL		m_bTerminate;

		CVisionStatusServer*	m_pServer;

		ClientMapper&		m_ClientMapper;
		CxCriticalSection&	m_csClientMapper;
		const DWORD&		m_dwAliveTimeout;
		virtual int Run() override;

		void CheckInactiveClient();
	public:
		CCheckAliveClientThread( ClientMapper& mapper, CxCriticalSection& cs, const DWORD& timeout, CVisionStatusServer* pServer );
		virtual ~CCheckAliveClientThread();
		void InitiateShutdown();
		void WaitForShutdownToComplete();

		void Start();
	};

	CCheckAliveClientThread		m_CheckAliveClientThread;
public:
	const int GetClientCount();
	virtual void OnStopServer() override;
	virtual void OnStartServer() override;
	virtual void OnPacketReceived( ServerThreadContext* pContext, CxIoBuffer* PacketBuffer ) override;

	virtual void OnClientConnected( ServerThreadContext* pContext, CxString strClientIP, int nPort ) override;
	virtual void OnClientDisconnected( CxString strClientIP, int nPort ) override;

	void SendPacketToClients( char *pOutBuffer, int nOutBufferSize );

	CVisionStatusServer(void);
	virtual ~CVisionStatusServer(void);
};

