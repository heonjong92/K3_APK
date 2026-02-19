// CounterCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CounterCtrl.h"

using namespace Gdiplus;
using namespace UIExt;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CCounterCtrl

IMPLEMENT_DYNAMIC(CCounterCtrl, CLedTextCtrl)

CCounterCtrl::CCounterCtrl()
{
	m_bIsCacheBuild = FALSE;
	m_nBorderWidth = 1;
	m_colBorderColor = 0xFF303030;//Color.Gray;
	m_nCornerRadius = 5;
	m_nCharacterNumber = 6;
	m_fBevelRate = 0.25f;
	m_colFadedColor = 0xFF202020;//Color.DimGray;
	//m_colCustomBk1 = Color::Black;
	//m_colCustomBk2 = 0xFF202020;//Color.DimGray;
	m_colCustomBk1 = Color(35, 47, 56);
	m_colCustomBk2 = Color(39, 53, 63);
	m_fWidthSegWidthRatio = 0.2f;
	m_fWidthIntervalRatio = 0.05f;
	m_enumAlign = TA_Right;
	m_bRoundRect = FALSE;
	m_bGradientBackground = FALSE;
	m_bShowHighlight = FALSE;
	m_nHighlightOpaque = 50;
	m_colForeColor = Color(200,200,200);
	m_colBackColor = Color::Transparent;
}

CCounterCtrl::~CCounterCtrl()
{
}


BEGIN_MESSAGE_MAP(CCounterCtrl, CLedTextCtrl)
END_MESSAGE_MAP()

void CCounterCtrl::SetCount( int nCount )
{
	CString strCount;
	strCount.Format( _T("%d"), nCount );

	SetText( strCount );
	Invalidate();
}


