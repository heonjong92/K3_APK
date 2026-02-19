#pragma once

#include <XUtil/xThread.h>
#include <XUtil/xCriticalSection.h>
#include <XUtil/xEvent.h>
#include <queue>


class CxImageObject;
class CxGraphicObject;

namespace VisionProcess {

class CInspectProcess;
class CInspectSaveMergeProcess
{
public:
	struct MergeItem
	{
		BOOL bXMerge; // 받을게 없는데 음.
	};

protected:
	class WorkerQueue
	{
	protected:
		HANDLE					m_hSemaphore;
		CxCriticalSection		m_csQueue;
		std::queue<MergeItem>	m_QueueMerge;

	public:
		HANDLE GetSemaphoreHandle() { return m_hSemaphore; }
		WorkerQueue();
		~WorkerQueue();
		void Start();
		BOOL Push(MergeItem& item);
		MergeItem Pop();
		void Clear();
	};

	class WorkerThread : public CxThread
	{
	protected:
		CInspectSaveMergeProcess*	m_pProcess;
		int					m_nThreadId;

	public:
		int m_bWorking;
		void WaitForShutdownToComplete();

		virtual int Run() override;
		void DoWork(MergeItem& item );

		void Init();

		WorkerThread(const WorkerThread &rhs);
		WorkerThread &operator=(const WorkerThread &rhs);
		explicit WorkerThread(CInspectSaveMergeProcess* pProcess, int nThreadId);
		virtual ~WorkerThread();
	};

	WorkerQueue		m_WorkerQueue;

	friend class WorkerThread;
	std::vector<WorkerThread *> m_WorkerThreads;
	const int		m_nNumThreads;
	CxEvent			m_evtTerminateThreads;

public:
	CInspectSaveMergeProcess(void);
	virtual ~CInspectSaveMergeProcess(void);

	BOOL Start( HWND hWndUI );
	void Stop();

	BOOL PushMergeItem(MergeItem& item);

	long GetWorkingThreadCount();
};

} // namespace VisionProcess