#include "stdafx.h"
#include "APK.h"

#include "InspectSaveProcess.h"

#include "VisionSystem.h"
#include "ModelInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionProcess;

CInspectSaveProcess::CInspectSaveProcess(void) : m_nNumThreads(4)
	, m_evtTerminateThreads(NULL, TRUE, FALSE)
{
	m_WorkerThreads.resize(m_nNumThreads);

	for(int i = 0; i < m_nNumThreads; i++)
		m_WorkerThreads[i] = NULL;
}

CInspectSaveProcess::~CInspectSaveProcess(void)
{
	Stop();
}

CInspectSaveProcess::WorkerQueue::WorkerQueue()
{
	m_hSemaphore = NULL;
}

CInspectSaveProcess::WorkerQueue::~WorkerQueue()
{
	Clear();
}

BOOL CInspectSaveProcess::WorkerQueue::Push(SaveItem& item)
{
	{
		CxCriticalSection::Owner Lock(m_csQueue);
		m_Queue.push(item);

		if((int)m_Queue.size() > 100)
		{
			////ASSERT(FALSE);
			TRACE(_T("CInspectSaveProcess::WorkerQueue::Push - Overflow!!!!\n"));
			CVisionSystem::Instance()->WriteMessage(LogTypeError, _T("CInspectSaveProcess::WorkerQueue::Push - Overflow!!!!\n"));
			///return FALSE;
		}
		TRACE(_T("CInspectSaveProcess::WorkerQueue::Push - SIZE: %d\n"), (int)m_Queue.size());
	}

	if(!::ReleaseSemaphore(m_hSemaphore, 1, NULL)) 
	{ 
		TRACE( _T("CInspectSaveProcess::WorkerQueue::Push - FATAL ERROR: signal semaphore failed!!\n"));
		return FALSE;     
	} 
	return TRUE;
}

CInspectSaveProcess::SaveItem CInspectSaveProcess::WorkerQueue::Pop()
{
	SaveItem item;
	memset(&item, 0, sizeof(SaveItem));

	CxCriticalSection::Owner Lock(m_csQueue);
	if(m_Queue.empty())
		return item;

	item = m_Queue.front();
	m_Queue.pop();

	TRACE(_T("CInspectSaveProcess::WorkerQueue::Pop - SIZE: %d\n"), (int)m_Queue.size());

	return item;
}

void CInspectSaveProcess::WorkerQueue::Start()
{
	if(m_hSemaphore)
		return;

	m_hSemaphore = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	ASSERT(m_hSemaphore);
	if(!m_hSemaphore)
	{
		TRACE(_T("CInspectSaveProcess::WorkerQueue::Start() - CreateSemaphore Fail: %d\n"), GetLastError());
	}
}

void CInspectSaveProcess::WorkerQueue::Clear()
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

void CInspectSaveProcess::WorkerThread::WaitForShutdownToComplete()
{
	if(!Wait(5000))
	{
		TRACE("CInspectSaveProcess::WorkerThread - Wait timeout (5s), retrying...\n");
		if(!Wait(30000))
		{
			TRACE("CInspectSaveProcess::WorkerThread - Force Terminate (last resort)\n");
			Terminate();
		}
	}

   if(m_hThread != 0)
   {
      ::CloseHandle(m_hThread);
	  m_hThread = 0;
   }
}

void CInspectSaveProcess::WorkerThread::DoWork(SaveItem& item)
{
	TRACE(_T("DO WORK - [%d]\n"), m_nThreadId);
	m_bWorking = 1;

	CVisionSystem* pSystem = CVisionSystem::Instance();
	pSystem->StartSaveImage( item.pImgObj, item.inspecttype, item.nViewIndex, item.nGrabCnt );

	m_bWorking = 0;
	TRACE(_T("END WORK - [%d]\n"), m_nThreadId);
}

int CInspectSaveProcess::WorkerThread::Run()
{
	HANDLE hEvents[2] = 
	{ 
		m_pProcess->m_evtTerminateThreads.GetEvent(),
		m_pProcess->m_WorkerQueue.GetSemaphoreHandle()
	};

	if(hEvents[0] == NULL || hEvents[1] == NULL)
	{
		TRACE(_T("CInspectSaveProcess::WorkerThread::Run() - ERROR: Invalid Event Object\n"));
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
				SaveItem item = m_pProcess->m_WorkerQueue.Pop();
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
	TRACE(_T("CInspectSaveProcess::WorkerThread::Run() - Exit Thread\n"));

	return 0;
}

CInspectSaveProcess::WorkerThread::WorkerThread(CInspectSaveProcess* pProcess, int nThreadId) : m_pProcess(pProcess)
	, m_nThreadId(nThreadId)
	, m_bWorking(0)
{
}

CInspectSaveProcess::WorkerThread::~WorkerThread()
{
}

void CInspectSaveProcess::WorkerThread::Init()
{
}

BOOL CInspectSaveProcess::Start(HWND hWndUI)
{
	TRACE("CInspectSaveProcess::Start...\n");

	m_WorkerQueue.Start();
	m_evtTerminateThreads.Reset();

	if(m_WorkerThreads[0])
	{
		TRACE(_T("CInspectSaveProcess::Start - Already started...\n"));
		return FALSE;
	}
	
	for(int i = 0; i < m_nNumThreads; ++i)
	{
		WorkerThread* pThread = new WorkerThread(this, i); 
		m_WorkerThreads[i] = pThread;
		pThread->Init();
		pThread->Start();
	}

	TRACE("CInspectSaveProcess::Start - Started\n");
	return TRUE;
}

void CInspectSaveProcess::Stop()
{
	TRACE("CInspectSaveProcess::Stop...\n");
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
	TRACE("CInspectSaveProcess::Stop - Stopped...\n");
}

BOOL CInspectSaveProcess::PushSaveItem(SaveItem& item)
{
	if( !m_WorkerQueue.Push(item) )
	{
		return FALSE;
	}

	return TRUE;
}

long CInspectSaveProcess::GetWorkingThreadCount()
{
	long count = 0;
	for(int i=0; i<m_nNumThreads; i++)
	{
		if(m_WorkerThreads[i] && m_WorkerThreads[i]->m_bWorking)
			count++;
	}

	return count;
}
