#include "stdafx.h"
#include "ThreadWorker.h"

using namespace VisionProcess;

CThreadWorker::CThreadWorker(void) : m_nNumThreads(4)
	, m_evtTerminateThreads(NULL, TRUE, FALSE)
	, m_evtCompleteThreadWork(NULL, TRUE, FALSE)
{
	m_WorkerThreads.resize(m_nNumThreads);
	for (int i=0 ; i<m_nNumThreads ; i++)
	{
		m_WorkerThreads[i] = NULL;
	}

}


CThreadWorker::~CThreadWorker(void)
{
	StopThread();
}


CThreadWorker::WorkerQueue::WorkerQueue()
{
	m_hSemaphore = NULL;
}

CThreadWorker::WorkerQueue::~WorkerQueue()
{
	Clear();
}

void CThreadWorker::WorkerQueue::SetResultCount( int nCount )
{
	CxCriticalSection::Owner Lock(m_csResult);
	m_nResultCount = nCount;
}

int CThreadWorker::WorkerQueue::DecrementResultCount()
{
	CxCriticalSection::Owner Lock(m_csResult);
	--m_nResultCount;
	return m_nResultCount;
}

BOOL CThreadWorker::WorkerQueue::Push(InspectItem& item)
{
	{
		CxCriticalSection::Owner Lock(m_csQueue);
		m_Queue.push(item);

		if ((int)m_Queue.size() > 100)
		{
			ASSERT(FALSE);
			TRACE( _T("CThreadWorker::WorkerQueue::Push - Overflow!!!!\n") );
			return FALSE;
		}
		TRACE( _T("CThreadWorker::WorkerQueue::Push - SIZE: %d\n"), (int)m_Queue.size() );
	}

	if ( !::ReleaseSemaphore( m_hSemaphore, 1, NULL ) ) 
	{ 
		TRACE( _T("CThreadWorker::WorkerQueue::Push - FATAL ERROR: signal semaphore failed!!\n") );
		return FALSE;     
	} 
	return TRUE;
}

CThreadWorker::InspectItem CThreadWorker::WorkerQueue::Pop()
{
	InspectItem item;
	memset( &item, 0, sizeof(InspectItem) );

	CxCriticalSection::Owner Lock(m_csQueue);
	if (m_Queue.empty())
		return item;

	item = m_Queue.front();
	m_Queue.pop();

	TRACE( _T("CThreadWorker::WorkerQueue::Pop - SIZE: %d\n"), (int)m_Queue.size() );

	return item;
}

void CThreadWorker::WorkerQueue::Start()
{
	if (m_hSemaphore)
		return;
	m_hSemaphore = ::CreateSemaphore( NULL, 0, LONG_MAX, NULL );
	ASSERT( m_hSemaphore );
	if ( !m_hSemaphore )
	{
		TRACE( _T("CThreadWorker::WorkerQueue::Start() - CreateSemaphore Fail: %d\n"), GetLastError() );
	}
}

void CThreadWorker::WorkerQueue::Clear()
{
	{
		CxCriticalSection::Owner Lock(m_csQueue);
		while (!m_Queue.empty())
		{
			m_Queue.pop();
		}
	}
	if (m_hSemaphore)
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
}

void CThreadWorker::WorkerThread::WaitForShutdownToComplete()
{
	if ( !Wait( 1000 ) )
	{
		TRACE( "CThreadWorker::WorkerThread - Force Terminate\n" );
		Terminate();
	}

   if (m_hThread != 0)
   {
      ::CloseHandle(m_hThread);
	  m_hThread = 0;
   }
}

void CThreadWorker::WorkerThread::DoWork(InspectItem& item)
{
	TRACE( _T("DO WORK - [%d]\n"), m_nThreadId );
	m_pThreadWorker->OnThreadWork(item);
	//m_InspectBall.InspectOneUnit(item.pImgObj, item.nUnitIndex);
	TRACE( _T("END WORK - [%d]\n"), m_nThreadId );
}

int CThreadWorker::WorkerThread::Run()
{
	HANDLE hEvents[2] = {m_pThreadWorker->m_evtTerminateThreads.GetEvent(), m_pThreadWorker->m_WorkerQueue.GetSemaphoreHandle()};

	if(hEvents[0] == NULL || hEvents[1] == NULL)
	{
		TRACE( _T("CThreadWorker::WorkerThread::Run() - ERROR: Invalid Event Object\n") );
		return 0;
	}

	while(TRUE)
	{
		DWORD dwWaitResult = ::WaitForMultipleObjects(2, hEvents, FALSE, 500);
		if(dwWaitResult == WAIT_TIMEOUT)
			continue;

		switch(dwWaitResult-WAIT_OBJECT_0)
		{
		case 0:	// Terminate
			goto goto_TerminateThread;
			break;
		case 1:	// Work!!
			{
				InspectItem item = m_pThreadWorker->m_WorkerQueue.Pop();
				DoWork(item);
				if(m_pThreadWorker->m_WorkerQueue.DecrementResultCount() <= 0)
					m_pThreadWorker->m_evtCompleteThreadWork.Set();
			}
			break;
		default: // error
			ASSERT( FALSE );
			goto goto_TerminateThread;
			break;
		}
	}

goto_TerminateThread:
	TRACE(_T("CThreadWorker::WorkerThread::Run() - Exit Thread\n"));
	return 0;
}

CThreadWorker::WorkerThread::WorkerThread(CThreadWorker* pWorker, int nThreadId) : m_pThreadWorker(pWorker)
	, m_nThreadId(nThreadId)
{
}

CThreadWorker::WorkerThread::~WorkerThread()
{

}

BOOL CThreadWorker::StartThread()
{
	TRACE( "CThreadWorker::StartThread - Start...\n" );

	m_WorkerQueue.Start();

	m_evtTerminateThreads.Reset();

	if ( m_WorkerThreads[0] )
	{
		TRACE( _T("CThreadWorker::StartThread - Already started...\n") );
		return FALSE;
	}
	
	for ( int i = 0 ; i < m_nNumThreads ; ++i )
	{
		WorkerThread *pThread = new WorkerThread( this, i ); 
		m_WorkerThreads[i] = pThread;
		pThread->Start();
	}

	TRACE( "CThreadWorker::StartThread - Started\n" );

	return TRUE;
}

void CThreadWorker::StopThread()
{
	TRACE( "CThreadWorker::StopThread - Stop...\n" );
	m_evtTerminateThreads.Set();

	for (int i = 0; i < m_nNumThreads; ++i)
	{
		if (m_WorkerThreads[i])
		{
			m_WorkerThreads[i]->WaitForShutdownToComplete();
			delete m_WorkerThreads[i];
			m_WorkerThreads[i] = NULL;
		}
	}

	m_WorkerQueue.Clear();
	TRACE( "CThreadWorker::StopThread - Stopped...\n" );
}

void CThreadWorker::InitThreadWorker( int nWorkItemCount )
{
	m_evtCompleteThreadWork.Reset();
	m_WorkerQueue.SetResultCount(nWorkItemCount);
}

void CThreadWorker::PushThreadWorkItem(InspectItem& item)
{
	m_WorkerQueue.Push(item);
}

void CThreadWorker::WaitThreadWorkComplete(DWORD dwTimeout)
{
	::WaitForSingleObject(m_evtCompleteThreadWork.GetEvent(), dwTimeout);
}