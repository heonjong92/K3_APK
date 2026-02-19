#pragma once

#include <XUtil/xThread.h>
#include <XUtil/xCriticalSection.h>
#include <XUtil/xEvent.h>
#include <queue>

class CxImageObject;
class CxGraphicObject;

namespace VisionProcess {

class CThreadWorker
{
protected:

	struct InspectItem
	{
		CxImageObject*	pImgObj;
		UINT			nUnitIndex;
	};

	class WorkerQueue
	{
	protected:
		HANDLE					m_hSemaphore;
		CxCriticalSection		m_csQueue;
		CxCriticalSection		m_csResult;
		std::queue<InspectItem>	m_Queue;
		int						m_nResultCount;
	public:
		HANDLE GetSemaphoreHandle() { return m_hSemaphore; }
		void SetResultCount( int nCount );
		int DecrementResultCount();
		WorkerQueue();
		~WorkerQueue();
		void Start();
		BOOL Push(InspectItem& item);
		InspectItem Pop();
		void Clear();
	};

	class WorkerThread : public CxThread
	{
	protected:
		CThreadWorker*	m_pThreadWorker;
		int				m_nThreadId;
	public:
		void WaitForShutdownToComplete();

		virtual int Run() override;
		void DoWork( InspectItem& item );

		WorkerThread(const WorkerThread &rhs);
		WorkerThread &operator=(const WorkerThread &rhs);
		explicit WorkerThread(CThreadWorker* pWorker, int nThreadId);
		virtual ~WorkerThread();
	};

	WorkerQueue		m_WorkerQueue;

	friend class WorkerThread;
	std::vector<WorkerThread *> m_WorkerThreads;
	const int		m_nNumThreads;
	CxEvent			m_evtTerminateThreads;
	CxEvent			m_evtCompleteThreadWork;

	virtual void OnThreadWork(InspectItem& item) = 0;

	void InitThreadWorker( int nWorkItemCount );
	void PushThreadWorkItem(InspectItem& item);
	void WaitThreadWorkComplete(DWORD dwTimeout);

public:
	CThreadWorker(void);
	virtual ~CThreadWorker(void);

	BOOL StartThread();
	void StopThread();
};

}