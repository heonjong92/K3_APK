#pragma once

#include "../Common/xSingletoneObject.h"
#include <XUtil/xCriticalSection.h>
#include <queue>

class CxLogMsgCache : public CxSingleton<CxLogMsgCache>
{
protected:
	const int				m_nMsgBufferSize;
	const int				m_nMaxCacheSize;
	CxCriticalSection		m_csLogMsgStorage;
	std::queue<TCHAR*>		m_LogMsgStorage;
public:
	TCHAR* GetMsgString();
	void ReleaseMsgString( TCHAR* pMsgStr );

	const int GetMsgBufferSize() const { return m_nMsgBufferSize; }

	BOOL IsEmpty();

	CxLogMsgCache(void);
	virtual ~CxLogMsgCache(void);
};

