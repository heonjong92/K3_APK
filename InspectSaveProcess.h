#pragma once

#include <XUtil/xThread.h>
#include <XUtil/xCriticalSection.h>
#include <XUtil/xEvent.h>
#include <queue>

#include <XGraphic\xDataTypes.h>


class CxImageObject;
class CxGraphicObject;

namespace VisionProcess {

class CInspectProcess;
class CInspectSaveProcess
{
public:
	struct SaveItem
	{
		CxImageObject*		pImgObj;
		InspectType			inspecttype;
		UINT				nViewIndex;
		int					nGrabCnt;
	};

protected:
	class WorkerQueue
	{
	protected:
		HANDLE					m_hSemaphore;
		CxCriticalSection		m_csQueue;
		std::queue<SaveItem>	m_Queue;

	public:
		HANDLE GetSemaphoreHandle() { return m_hSemaphore; }
		WorkerQueue();
		~WorkerQueue();
		void Start();
		BOOL Push(SaveItem& item);
		SaveItem Pop();
		void Clear();
	};

	class WorkerThread : public CxThread
	{
	protected:
		CInspectSaveProcess*	m_pProcess;
		int					m_nThreadId;

	public:
		int m_bWorking;
		void WaitForShutdownToComplete();

		virtual int Run() override;
		void DoWork( SaveItem& item );

		void Init();

		WorkerThread(const WorkerThread &rhs);
		WorkerThread &operator=(const WorkerThread &rhs);
		explicit WorkerThread(CInspectSaveProcess* pProcess, int nThreadId);
		virtual ~WorkerThread();
	};

	WorkerQueue		m_WorkerQueue;

	friend class WorkerThread;
	std::vector<WorkerThread *> m_WorkerThreads;
	const int		m_nNumThreads;
	CxEvent			m_evtTerminateThreads;

public:
	CInspectSaveProcess(void);
	virtual ~CInspectSaveProcess(void);

	BOOL Start( HWND hWndUI );
	void Stop();

	BOOL PushSaveItem(SaveItem& item);

	long GetWorkingThreadCount();
};

} // namespace VisionProcess