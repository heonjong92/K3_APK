#pragma once

#include "VisionStatusServer.h"
#include "Common/xSingletoneObject.h"
#include "VisionSystem.h"

class CVisionStatusProvider : public CxSingleton<CVisionStatusProvider>, public CVisionSystem::IVisionStatusChanged
{
protected:
	class CVisionServer : public CVisionStatusServer
	{
	protected:
		CVisionStatusProvider* m_pOwner;
		BOOL m_bIsStarted;
	public:
		CVisionServer(CVisionStatusProvider* pOwner) : m_pOwner(pOwner), m_bIsStarted(FALSE) {}
		virtual ~CVisionServer() {}

		BOOL IsStarted() const { return m_bIsStarted; }

		virtual void OnStopServer() override;
		virtual void OnStartServer() override;
		virtual void OnClientConnected( ServerThreadContext* pContext, CxString strClientIP, int nPort ) override;
	};

	CVisionServer*	m_Server[SOCKET_MAX];

	void OnVisionStatusChanged();
public:
	void SendVisionStatusPacket();
	void SendPacket(CString strPacket, int n);

	BOOL Start(UINT nPort, int n);
	void Stop(int n);
	BOOL IsStarted(int n);

	CVisionStatusProvider(void);
	virtual ~CVisionStatusProvider(void);
};

