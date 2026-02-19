#include "stdafx.h"
#include "xLogMsgCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CxLogMsgCache::CxLogMsgCache(void) : m_nMaxCacheSize(30), m_nMsgBufferSize(5000)//1024
{
}


CxLogMsgCache::~CxLogMsgCache(void)
{
	while (!m_LogMsgStorage.empty())
	{
		TCHAR* pRetStr = m_LogMsgStorage.front();
		delete[] pRetStr;
		m_LogMsgStorage.pop();
	}
}

TCHAR* CxLogMsgCache::GetMsgString()
{
	CxCriticalSection::Owner Lock(m_csLogMsgStorage);

	if(m_LogMsgStorage.empty())
		return new TCHAR[m_nMsgBufferSize];

	TCHAR* pRetStr = m_LogMsgStorage.front();
	m_LogMsgStorage.pop();
	return pRetStr;
}

void CxLogMsgCache::ReleaseMsgString( TCHAR* pMsgStr )
{
	CxCriticalSection::Owner Lock(m_csLogMsgStorage);

	if ( (int)m_LogMsgStorage.size() > m_nMaxCacheSize )
	{
		delete[] pMsgStr;
		return;
	}

	m_LogMsgStorage.push( pMsgStr );
}

BOOL CxLogMsgCache::IsEmpty()
{
	CxCriticalSection::Owner Lock(m_csLogMsgStorage);

	return m_LogMsgStorage.empty() ? TRUE : FALSE;
}
