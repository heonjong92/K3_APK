#include "stdafx.h"
#include "VisionStatusProvider.h"

#include "VisionSystem.h"
#include "VisionStatusPacket.h"
#include "ModelInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CVisionStatusProvider::CVisionServer::OnStopServer()
{
	CVisionStatusServer::OnStopServer();
	m_bIsStarted = FALSE;
}

void CVisionStatusProvider::CVisionServer::OnStartServer()
{
	CVisionStatusServer::OnStartServer();
	m_bIsStarted = TRUE;
}

void CVisionStatusProvider::CVisionServer::OnClientConnected( ServerThreadContext* pContext, CxString strClientIP, int nPort )
{
	CVisionStatusServer::OnClientConnected(pContext, strClientIP, nPort);

	m_pOwner->SendVisionStatusPacket();
}

CVisionStatusProvider::CVisionStatusProvider(void)// : m_Server(this)
{
	for (int i = 0; i < SOCKET_MAX; ++i) {
		m_Server[i] = new CVisionServer(this);
	}
}


CVisionStatusProvider::~CVisionStatusProvider(void)
{
}

void CVisionStatusProvider::SendVisionStatusPacket()
{
//	WRITE_LOG( WL_DEBUG, _T("SendVisionStatusPacket() - in"));
	VisionStatusPacket packet;
	memset( &packet, 0, sizeof(VisionStatusPacket) );
	packet.Length = sizeof(VisionStatusPacket);
	USES_CONVERSION;
	CVisionSystem* pVisionSystem = CVisionSystem::Instance();
	CModelInfo* pModelInfo = CModelInfo::Instance();
	switch (pVisionSystem->GetRunStatus())
	{
	case RunStatusStop:
		packet.Type = VisionStatusStop;
		WRITE_LOG(WL_DEBUG, _T("SendVisionStatusPacket() - packet.Type = VisionStatusStop"));
		break;
	case RunStatusAutoRun:
		packet.Type = VisionStatusReady;

		WRITE_LOG(WL_DEBUG, _T("SendVisionStatusPacket() - packet.Type = VisionStatusError"));
		break;
	}

//	m_Server.SendPacketToClients((char*)&packet, packet.Length);
}

void CVisionStatusProvider::SendPacket(CString strPacket, int n)
{
	CString strText;
	strText.Format(_T("%s"), strPacket);

	CString strSize = _T("SIZE");
	int nPos = strText.Find(strSize);
	int nTotalLength = strText.GetLength();

	if (nPos != -1)
	{
		CString strLength;
		strLength.Format(_T("%04d"), nTotalLength);

		strText = strText.Left(nPos) + strLength + strText.Mid(nPos + strSize.GetLength());
	}

	USES_CONVERSION;
	m_Server[n]->SendPacketToClients(T2A(strText), nTotalLength);

	CString strMsg;
	strMsg.Format(_T("ProcessBuffer [Send][%d] : %s"), n + 5000, strText);

	CVisionSystem::Instance()->WriteMessage(LogTypeNotification, strMsg);
}

void CVisionStatusProvider::OnVisionStatusChanged()
{
	SendVisionStatusPacket();
}

BOOL CVisionStatusProvider::Start(UINT nPort, int n)
{
	return m_Server[n]->StartServer(nPort);
}

void CVisionStatusProvider::Stop(int n)
{
	m_Server[n]->StopServer();
}

BOOL CVisionStatusProvider::IsStarted(int n)
{
	return m_Server[n]->IsStarted();
}