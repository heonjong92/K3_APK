#include "stdafx.h"
#include "APK.h"

#include "InspectProcess.h"

#include "VisionSystem.h"
#include "ModelInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionProcess;

CInspectProcess::CInspectProcess(void) : m_nNumThreads(4)
	, m_evtTerminateThreads(NULL, TRUE, FALSE)
{
	m_WorkerThreads.resize(m_nNumThreads);

	for(int i = 0; i < m_nNumThreads; i++)
		m_WorkerThreads[i] = NULL;
}

CInspectProcess::~CInspectProcess(void)
{
	Stop();
}

CInspectProcess::WorkerQueue::WorkerQueue()
{
	m_hSemaphore = NULL;
}

CInspectProcess::WorkerQueue::~WorkerQueue()
{
	Clear();
}

BOOL CInspectProcess::WorkerQueue::Push(InspectItem& item)
{
	{
		CxCriticalSection::Owner Lock(m_csQueue);
		m_Queue.push(item);

		if((int)m_Queue.size() > 100)
		{
			////ASSERT(FALSE);
			TRACE(_T("CInspectProcess::WorkerQueue::Push - Overflow!!!!\n"));
			CVisionSystem::Instance()->WriteMessage(LogTypeError, _T("CInspectProcess::WorkerQueue::Push - Overflow!!!!\n"));
			///return FALSE;
		}
		TRACE(_T("CInspectProcess::WorkerQueue::Push - SIZE: %d\n"), (int)m_Queue.size());
	}

	if(!::ReleaseSemaphore(m_hSemaphore, 1, NULL)) 
	{ 
		TRACE( _T("CInspectProcess::WorkerQueue::Push - FATAL ERROR: signal semaphore failed!!\n"));
		return FALSE;     
	} 
	return TRUE;
}

CInspectProcess::InspectItem CInspectProcess::WorkerQueue::Pop()
{
	InspectItem item;
	memset(&item, 0, sizeof(InspectItem));

	CxCriticalSection::Owner Lock(m_csQueue);
	if(m_Queue.empty())
		return item;

	item = m_Queue.front();
	m_Queue.pop();

	TRACE(_T("CInspectProcess::WorkerQueue::Pop - SIZE: %d\n"), (int)m_Queue.size());

	return item;
}

void CInspectProcess::WorkerQueue::Start()
{
	if(m_hSemaphore)
		return;

	m_hSemaphore = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	ASSERT(m_hSemaphore);
	if(!m_hSemaphore)
	{
		TRACE(_T("CInspectProcess::WorkerQueue::Start() - CreateSemaphore Fail: %d\n"), GetLastError());
	}
}

void CInspectProcess::WorkerQueue::Clear()
{
	CxCriticalSection::Owner Lock(m_csQueue);
	while(!m_Queue.empty())
	{
		m_Queue.pop();
	}

	if(m_hSemaphore)
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
}

void CInspectProcess::WorkerThread::WaitForShutdownToComplete()
{
	if(!Wait(5000))
	{
		TRACE("CInspectProcess::WorkerThread - Wait timeout (5s), retrying...\n");
		if(!Wait(30000))
		{
			TRACE("CInspectProcess::WorkerThread - Force Terminate (last resort)\n");
			Terminate();
		}
	}

   if(m_hThread != 0)
   {
      ::CloseHandle(m_hThread);
	  m_hThread = 0;
   }
}

void CInspectProcess::WorkerThread::DoWork(InspectItem& item)
{
	TRACE(_T("DO WORK - [%d]\n"), m_nThreadId);
	m_bWorking = 1;

	CVisionSystem* pSystem = CVisionSystem::Instance();
	pSystem->StartInspection( item.inspecttype, item.nViewIndex, item.nGrabCnt, FALSE );

	m_bWorking = 0;
	TRACE(_T("END WORK - [%d]\n"), m_nThreadId);
}

int CInspectProcess::WorkerThread::Run()
{
	HANDLE hEvents[2] = 
	{ 
		m_pProcess->m_evtTerminateThreads.GetEvent(),
		m_pProcess->m_WorkerQueue.GetSemaphoreHandle()
	};

	if(hEvents[0] == NULL || hEvents[1] == NULL)
	{
		TRACE(_T("CInspectProcess::WorkerThread::Run() - ERROR: Invalid Event Object\n"));
		return 0;
	}

	while(TRUE)
	{
		DWORD dwWaitResult = ::WaitForMultipleObjects(2, hEvents, FALSE, 500);

		if(dwWaitResult == WAIT_TIMEOUT)
			continue;

		switch(dwWaitResult -WAIT_OBJECT_0)
		{
		case 0:		// Terminate
			goto goto_TerminateThread;
			break;
		case 1:		// Work!!
			{
				InspectItem item = m_pProcess->m_WorkerQueue.Pop();
				DoWork(item);
			}
			break;
		default:	// error
			ASSERT(FALSE);
			goto goto_TerminateThread;
			break;
		}
	}

goto_TerminateThread:
	TRACE(_T("CInspectProcess::WorkerThread::Run() - Exit Thread\n"));

	return 0;
}

CInspectProcess::WorkerThread::WorkerThread(CInspectProcess* pProcess, int nThreadId) : m_pProcess(pProcess)
	, m_nThreadId(nThreadId)
	, m_bWorking(0)
{
}

CInspectProcess::WorkerThread::~WorkerThread()
{
}

void CInspectProcess::WorkerThread::Init()
{
}

BOOL CInspectProcess::Start(HWND hWndUI)
{
	TRACE("CInspectProcess::Start...\n");

	m_WorkerQueue.Start();
	m_evtTerminateThreads.Reset();

	if(m_WorkerThreads[0])
	{
		TRACE(_T("CInspectProcess::Start - Already started...\n"));
		return FALSE;
	}
	
	for(int i = 0; i < m_nNumThreads; ++i)
	{
		WorkerThread* pThread = new WorkerThread(this, i); 
		m_WorkerThreads[i] = pThread;
		pThread->Init();
		pThread->Start();
	}

	TRACE("CInspectProcess::Start - Started\n");
	return TRUE;
}

void CInspectProcess::Stop()
{
	TRACE("CInspectProcess::Stop...\n");
	m_evtTerminateThreads.Set();

	for(int i = 0; i < m_nNumThreads; ++i)
	{
		if(m_WorkerThreads[i])
		{
			m_WorkerThreads[i]->WaitForShutdownToComplete();
			delete m_WorkerThreads[i];
			m_WorkerThreads[i] = NULL;
		}
	}

	m_WorkerQueue.Clear();
	TRACE("CInspectProcess::Stop - Stopped...\n");
}

BOOL CInspectProcess::PushInspectItem(InspectItem& item)
{
	if( !m_WorkerQueue.Push(item) )
	{
		return FALSE;
	}

	return TRUE;
}

long CInspectProcess::GetWorkingThreadCount()
{
	long count = 0;
	for(int i=0; i<m_nNumThreads; i++)
	{
		if(m_WorkerThreads[i] && m_WorkerThreads[i]->m_bWorking)
			count++;
	}

	return count;
}
