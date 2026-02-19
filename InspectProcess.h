#pragma once

#include <XUtil/xThread.h>
#include <XUtil/xCriticalSection.h>
#include <XUtil/xEvent.h>
#include <queue>


class CxImageObject;
class CxGraphicObject;

namespace VisionProcess {

class CInspectionVision;
class CInspectProcess
{
public:
	struct InspectItem
	{
		CxImageObject*		pImgObj;
		CxGraphicObject*	pGO;
		UINT				nViewIndex;
		InspectType			inspecttype;
		int					nGrabCnt;
	};

protected:
	class WorkerQueue
	{
	protected:
		HANDLE					m_hSemaphore;
		CxCriticalSection		m_csQueue;
		std::queue<InspectItem>	m_Queue;

	public:
		HANDLE GetSemaphoreHandle() { return m_hSemaphore; }
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
		CInspectProcess*	m_pProcess;
		int					m_nThreadId;

	public:
		int m_bWorking;
		void WaitForShutdownToComplete();

		virtual int Run() override;
		void DoWork( InspectItem& item );

		void Init();

		WorkerThread(const WorkerThread &rhs);
		WorkerThread &operator=(const WorkerThread &rhs);
		explicit WorkerThread(CInspectProcess* pProcess, int nThreadId);
		virtual ~WorkerThread();
	};

	WorkerQueue		m_WorkerQueue;

	friend class WorkerThread;
	std::vector<WorkerThread *> m_WorkerThreads;
	const int		m_nNumThreads;
	CxEvent			m_evtTerminateThreads;

public:
	CInspectProcess(void);
	virtual ~CInspectProcess(void);

	BOOL Start( HWND hWndUI );
	void Stop();

	BOOL PushInspectItem(InspectItem& item);

	long GetWorkingThreadCount();
};

} // namespace VisionProcess