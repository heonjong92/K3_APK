#pragma once

#include "Common/xSingletoneObject.h"
class CJobHandler : public CxSingleton<CJobHandler>
{
private:
	HANDLE		m_hJob;
public:
	CJobHandler(void);
	~CJobHandler(void);

	BOOL AssignProcess( HANDLE hProcess );
};

