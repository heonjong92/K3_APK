#include "stdafx.h"
#include "APK.h"

#include "InspectSaveMergeProcess.h"

#include "VisionSystem.h"
#include "ModelInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionProcess;

CInspectSaveMergeProcess::CInspectSaveMergeProcess(void) : m_nNumThreads(4)
	, m_evtTerminateThreads(NULL, TRUE, FALSE)
{
	m_WorkerThreads.resize(m_nNumThreads);

	for(int i = 0; i < m_nNumThreads; i++)
		m_WorkerThreads[i] = NULL;
}

CInspectSaveMergeProcess::~CInspectSaveMergeProcess(void)
{
	Stop();
}

CInspectSaveMergeProcess::WorkerQueue::WorkerQueue()
{
	m_hSemaphore = NULL;
}

CInspectSaveMergeProcess::WorkerQueue::~WorkerQueue()
{
	Clear();
}

BOOL CInspectSaveMergeProcess::WorkerQueue::Push(MergeItem& item)
{
	{
		CxCriticalSection::Owner Lock(m_csQueue);
		m_QueueMerge.push(item);

		if((int)m_QueueMerge.size() > 100)
		{
			////ASSERT(FALSE);
			TRACE(_T("CInspectSaveMergeProcess::WorkerQueue::Push - Overflow!!!!\n"));
			CVisionSystem::Instance()->WriteMessage(LogTypeError, _T("CInspectSaveMergeProcess::WorkerQueue::Push - Overflow!!!!\n"));
			///return FALSE;
		}
		TRACE(_T("CInspectSaveMergeProcess::WorkerQueue::Push - SIZE: %d\n"), (int)m_QueueMerge.size());
	}

	if(!::ReleaseSemaphore(m_hSemaphore, 1, NULL)) 
	{ 
		TRACE( _T("CInspectSaveMergeProcess::WorkerQueue::Push - FATAL ERROR: signal semaphore failed!!\n"));
		return FALSE;     
	} 
	return TRUE;
}

CInspectSaveMergeProcess::MergeItem CInspectSaveMergeProcess::WorkerQueue::Pop()
{
	MergeItem item;
	memset(&item, 0, sizeof(MergeItem));

	CxCriticalSection::Owner Lock(m_csQueue);
	if(m_QueueMerge.empty())
		return item;

	item = m_QueueMerge.front();
	m_QueueMerge.pop();

	TRACE(_T("CInspectSaveMergeProcess::WorkerQueue::Pop - SIZE: %d\n"), (int)m_QueueMerge.size());

	return item;
}

void CInspectSaveMergeProcess::WorkerQueue::Start()
{
	if(m_hSemaphore)
		return;

	m_hSemaphore = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	ASSERT(m_hSemaphore);
	if(!m_hSemaphore)
	{
		TRACE(_T("CInspectSaveMergeProcess::WorkerQueue::Start() - CreateSemaphore Fail: %d\n"), GetLastError());
	}
}

void CInspectSaveMergeProcess::WorkerQueue::Clear()
{
	CxCriticalSection::Owner Lock(m_csQueue);
	while(!m_QueueMerge.empty())
	{
		m_QueueMerge.pop();
	}

	if(m_hSemaphore)
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
}

void CInspectSaveMergeProcess::WorkerThread::WaitForShutdownToComplete()
{
	if(!Wait(1000))
	{
		TRACE("CInspectSaveMergeProcess::WorkerThread - Force Terminate\n");
		Terminate();
	}

   if(m_hThread != 0)
   {
      ::CloseHandle(m_hThread);
	  m_hThread = 0;
   }
}

void CInspectSaveMergeProcess::WorkerThread::DoWork(MergeItem& item)
{
	TRACE(_T("DO WORK - [%d]\n"), m_nThreadId);

	CVisionSystem* pSystem = CVisionSystem::Instance();

	pSystem->StartSaveImageMerge(item.bXMerge);

	m_bWorking = 0;
	TRACE(_T("END WORK - [%d]\n"), m_nThreadId);
}

int CInspectSaveMergeProcess::WorkerThread::Run()
{
	HANDLE hEvents[2] = 
	{ 
		m_pProcess->m_evtTerminateThreads.GetEvent(),
		m_pProcess->m_WorkerQueue.GetSemaphoreHandle()
	};

	if(hEvents[0] == NULL || hEvents[1] == NULL)
	{
		TRACE(_T("CInspectSaveMergeProcess::WorkerThread::Run() - ERROR: Invalid Event Object\n"));
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
				MergeItem item = m_pProcess->m_WorkerQueue.Pop();
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
	TRACE(_T("CInspectSaveMergeProcess::WorkerThread::Run() - Exit Thread\n"));

	return 0;
}

VisionProcess::CInspectSaveMergeProcess::WorkerThread::WorkerThread(const WorkerThread & rhs)
{
}

CInspectSaveMergeProcess::WorkerThread::WorkerThread(CInspectSaveMergeProcess* pProcess, int nThreadId) : m_pProcess(pProcess)
	, m_nThreadId(nThreadId)
	, m_bWorking(0)
{
}

CInspectSaveMergeProcess::WorkerThread::~WorkerThread()
{
}

void CInspectSaveMergeProcess::WorkerThread::Init()
{
}

BOOL CInspectSaveMergeProcess::Start(HWND hWndUI)
{
	TRACE("CInspectSaveMergeProcess::Start...\n");

	m_WorkerQueue.Start();
	m_evtTerminateThreads.Reset();

	if(m_WorkerThreads[0])
	{
		TRACE(_T("CInspectSaveMergeProcess::Start - Already started...\n"));
		return FALSE;
	}
	
	for(int i = 0; i < m_nNumThreads; ++i)
	{
		WorkerThread* pThread = new WorkerThread(this, i); 
		m_WorkerThreads[i] = pThread;
		pThread->Init();
		pThread->Start();
	}

	TRACE("CInspectSaveMergeProcess::Start - Started\n");
	return TRUE;
}

void CInspectSaveMergeProcess::Stop()
{
	TRACE("CInspectSaveMergeProcess::Stop...\n");
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
	TRACE("CInspectSaveMergeProcess::Stop - Stopped...\n");
}

BOOL CInspectSaveMergeProcess::PushMergeItem(MergeItem& item)
{
	if( !m_WorkerQueue.Push(item) )
	{
		return FALSE;
	}

	return TRUE;
}

long CInspectSaveMergeProcess::GetWorkingThreadCount()
{
	long count = 0;
	for(int i=0; i<m_nNumThreads; i++)
	{
		if(m_WorkerThreads[i]->m_bWorking)
			count++;
	}

	return count;
}
