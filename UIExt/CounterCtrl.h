#pragma once

#include "LedTextCtrl.h"

// CCounterCtrl

namespace UIExt
{

class CCounterCtrl : public CLedTextCtrl
{
	DECLARE_DYNAMIC(CCounterCtrl)

public:
	CCounterCtrl();
	virtual ~CCounterCtrl();

	void SetCount( int nCount );

protected:
	DECLARE_MESSAGE_MAP()
};

}