#include "stdafx.h"
#include "JobHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CJobHandler::CJobHandler()
{
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	
	::InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION );
	::SetSecurityDescriptorDacl( &sd, TRUE, NULL, FALSE );
	::SetSecurityDescriptorGroup( &sd, NULL, FALSE ); 
	::SetSecurityDescriptorSacl( &sd, FALSE, NULL, FALSE );
	
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = TRUE;

	m_hJob = ::CreateJobObject( &sa, _T("JobHandler@Yaong") );

	if( m_hJob )
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		::SetInformationJobObject( m_hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli) );
	}
}

CJobHandler::~CJobHandler()
{
	if( m_hJob )
		::CloseHandle( m_hJob );
	m_hJob = NULL;
}

BOOL CJobHandler::AssignProcess( HANDLE hProcess )
{
	if( !m_hJob ) return FALSE;
	if( !hProcess ) return FALSE;

	return ::AssignProcessToJobObject( m_hJob, hProcess );
}