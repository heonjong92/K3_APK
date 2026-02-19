// SideBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "SideBar.h"

#include "UIExt/GdiplusExt.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <afxpriv.h>

using namespace Gdiplus;
// CSideBar

IMPLEMENT_DYNAMIC(CSideBar, CWnd)

CSideBar::CSideBar()
{
}

CSideBar::~CSideBar()
{
}


BEGIN_MESSAGE_MAP(CSideBar, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CSideBar::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CWnd::Create(NULL, _T("SideBar"), WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1);
}

CFormView* CSideBar::CreateFormView( CRuntimeClass* pClass )
{
	CCreateContext cc;
	cc.m_pNewViewClass = pClass;
	cc.m_pCurrentDoc = NULL;
	cc.m_pNewDocTemplate = NULL;
	cc.m_pLastView = NULL;
	cc.m_pCurrentFrame = NULL;

	CCreateContext* pContext = &cc;

	CWnd* pWnd = NULL;
	if (pContext->m_pNewViewClass != NULL)
	{
		pWnd = (CWnd*)pContext->m_pNewViewClass->CreateObject();
		if (pWnd == NULL)
		{
			TRACE1("Error: Dynamic create of view %Fs failed\n", pContext->m_pNewViewClass->m_lpszClassName);
			return NULL;
		}
		ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CWnd)));

		if (!pWnd->Create(NULL, NULL, WS_CHILD|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0, pContext))
		{
			TRACE0("Error: couldn't create view \n");
			return NULL;
		}
	}

	CFormView* pView = NULL;
	if (pWnd != NULL)
	{
		pView = (CFormView*)pWnd;
		m_FormViews.Add( pView );
	}

	return pView;
}

void CSideBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if ( cx <= 0 || cy <= 0 )
		return;

	CFormView* pView;
	for (INT_PTR nI=0 ; nI<m_FormViews.GetCount() ; nI++)
	{
		pView = m_FormViews.GetAt(nI);
		pView->SetWindowPos(NULL, 0, 0, cx, cy, SWP_FRAMECHANGED);
	}
}
