#include "stdafx.h"
#include "GdipWndExt.h"

#include <afxpriv.h>
#include <afxocc.h>
// CGdipBaseWnd

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;

IMPLEMENT_DYNAMIC(CGdipBaseWnd, CWnd)
CGdipBaseWnd::CGdipBaseWnd()
{
	m_pBitmap = NULL;
}

CGdipBaseWnd::~CGdipBaseWnd()
{
	if (m_pBitmap)
		delete m_pBitmap;
	m_pBitmap = NULL;
}


BEGIN_MESSAGE_MAP(CGdipBaseWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

#include <vector>
void CGdipBaseWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetWindowRect( &rcClient );
	ScreenToClient(rcClient);

	BOOL bInit = FALSE;
	if ( m_MemDC.GetSafeHdc() == NULL )
	{
		m_MemDC.CreateCompatibleDC( &dc );
		bInit = TRUE;
	}

	if ( !m_pBitmap )
	{
		m_pBitmap = new CBitmap;
		ASSERT( m_pBitmap );
		VERIFY( m_pBitmap->CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() ) );
	}

	if (bInit)
	{
		OnEraseBkgnd(NULL);
	}

	CBitmap* pOldBitmap = m_MemDC.SelectObject( m_pBitmap );

	if ((GetExStyle() & WS_EX_TRANSPARENT) == WS_EX_TRANSPARENT)
	{
		CRect rectWindow;
		GetWindowRect(&rectWindow);
		GetParent()->ScreenToClient(&rectWindow);

		CDC *pParentDC = GetParent()->GetDC();
		m_MemDC.BitBlt(0, 0, rectWindow.Width(), rectWindow.Height(),
			pParentDC, rectWindow.left, rectWindow.top, SRCCOPY);
		VERIFY(ReleaseDC(pParentDC));
	}

	Graphics g(m_MemDC.GetSafeHdc());

	OnDraw( g, Rect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()) );

	m_MemDC.SetMapMode( MM_TEXT );
	m_MemDC.SetViewportOrg( 0, 0 );

	dc.BitBlt( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), 
		&m_MemDC, 0, 0, SRCCOPY );

	m_MemDC.SelectObject( pOldBitmap );	
}

BOOL CGdipBaseWnd::OnEraseBkgnd(CDC* pDC)
{
	if ( !m_pBitmap ) return CWnd::OnEraseBkgnd(pDC);
	if ( !m_MemDC.GetSafeHdc() ) return CWnd::OnEraseBkgnd(pDC);

	if ((GetExStyle() & WS_EX_TRANSPARENT) == WS_EX_TRANSPARENT)
		return TRUE;

	CBitmap * pOldBitmap = m_MemDC.SelectObject(m_pBitmap);
	Graphics g(m_MemDC.GetSafeHdc());

	CRect rcClient;
	GetWindowRect( &rcClient );
	ScreenToClient(rcClient);

	OnGdipEraseBkgnd( g, Rect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()) );

	m_MemDC.SelectObject(pOldBitmap);

	return TRUE;
}

void CGdipBaseWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if ( cx <= 0 || cy <= 0 ) return;

	if ( m_pBitmap != NULL ) delete m_pBitmap;
	m_pBitmap = new CBitmap;
	ASSERT( m_pBitmap );
	CClientDC dc(this);
	CRect rc;
	GetWindowRect(rc);
	VERIFY( m_pBitmap->CreateCompatibleBitmap(&dc, rc.Width(), rc.Height()) );
}

void CGdipBaseWnd::OnDraw( Graphics& g, Rect rect ) {}
void CGdipBaseWnd::OnGdipEraseBkgnd( Graphics& g, Rect rect ) {}

int CGdipBaseWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CGdipBaseWnd::OnDestroy()
{
	CWnd::OnDestroy();
}


IMPLEMENT_DYNAMIC(CGdipStatic, CGdipBaseWnd)

CGdipStatic::~CGdipStatic()
{
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CGdipStatic, CGdipBaseWnd)
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()

BOOL CGdipStatic::Create(LPCTSTR lpszText, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;
	return pWnd->Create(_T("STATIC"), lpszText, dwStyle, rect, pParentWnd, nID);
}

// Derived class is responsible for implementing all of these handlers
//   for owner/self draw controls
void CGdipStatic::DrawItem(LPDRAWITEMSTRUCT)
{
	ASSERT(FALSE);
}

BOOL CGdipStatic::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam,
	LRESULT* pResult)
{
	if (message != WM_DRAWITEM)
		return CGdipBaseWnd::OnChildNotify(message, wParam, lParam, pResult);

	ASSERT(pResult == NULL);       // no return value expected
	UNUSED(pResult); // unused in release builds
	DrawItem((LPDRAWITEMSTRUCT)lParam);
	return TRUE;
}

void CGdipStatic::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	if ( nAction != UIS_SET )
		CGdipBaseWnd::OnUpdateUIState(nAction, nUIElement);
}