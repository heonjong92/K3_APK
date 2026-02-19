// OutputListBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OutputListBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;

// COutputListBox

IMPLEMENT_DYNAMIC(COutputListBox, CListBox)

COutputListBox::COutputListBox()
{
}

COutputListBox::~COutputListBox()
{
}

BEGIN_MESSAGE_MAP(COutputListBox, CListBox)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// COutputListBox 메시지 처리기입니다.

void COutputListBox::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//ModifyStyle( WS_HSCROLL, 0, SWP_FRAMECHANGED );
	CListBox::OnWindowPosChanging(lpwndpos);
}

int COutputListBox::AddString( LPCTSTR lpszItem, ItemType type/*=ItemTypeNormal*/ )
{
	if ( GetCount() > 100 )
	{
		DeleteString( 0 );
	}

	int nIdx = CListBox::AddString( lpszItem );

	SetItemData( nIdx, type );
	
	CClientDC dc(this);
	CDC* pDC = &dc;
	CFont* pOldFont = pDC->SelectObject( &afxGlobalData.fontRegular );
	int cxExtentMax = 0;
	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;
	
	CRect rc;
	GetClientRect(rc);
	for (int i = 0; i < GetCount(); i ++)
	{
		CString strItem;
		GetText(i, strItem);

		CRect rect = rc;
		pDC->DrawText(strItem, rect, nFormat|DT_CALCRECT);

		//cxExtentMax = max(cxExtentMax, (int) pDC->GetTextExtent(strItem).cx);
		cxExtentMax = max( cxExtentMax, (int)rect.Width() );
	}

	pDC->SelectObject(pOldFont);
	
	SetHorizontalExtent( cxExtentMax );

	SetTopIndex( GetCount()-1 );

	return nIdx;
}

void COutputListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);

	CString strText;
	GetText(lpMeasureItemStruct->itemID, strText);
	ASSERT(TRUE != strText.IsEmpty());
	CRect rect;
	GetItemRect(lpMeasureItemStruct->itemID, &rect);

	CDC* pDC = GetDC(); 
	CFont* pOldFont = pDC->SelectObject( &afxGlobalData.fontRegular );
	CSize sizeText = pDC->GetTextExtent(strText);
	lpMeasureItemStruct->itemHeight = sizeText.cy;
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
}

void COutputListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ((int)lpDrawItemStruct->itemID < 0)
		return; 

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CFont* pOldFont = pDC->SelectObject( &afxGlobalData.fontRegular );

	COLORREF dwOldTextColor = pDC->GetTextColor();
	int nOldBkMode = pDC->GetBkMode();

	ItemType type = (ItemType)lpDrawItemStruct->itemData;
	COLORREF dwTextColor;
	COLORREF dwBodyColor;

	switch (type)
	{
	case ItemTypeNotification:
		dwTextColor = RGB(40, 40, 255);
		dwBodyColor = RGB(217, 217, 255);
		break;
	case ItemTypeError:
		dwTextColor = RGB(170, 0, 26);
		dwBodyColor = RGB(252, 218, 220);
		break;
	default:
		dwTextColor = RGB(80, 80, 80);
		dwBodyColor = RGB(226, 230, 235);
		break;
	}

	if ( !(lpDrawItemStruct->itemState & ODS_SELECTED) && !(lpDrawItemStruct->itemAction & ODA_FOCUS) )
	{
		dwBodyColor = ::GetSysColor(COLOR_WINDOW);
	}

	CBrush brushBody(dwBodyColor);

	pDC->FillRect( &lpDrawItemStruct->rcItem, &brushBody );

	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
	{
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
	}

	pDC->SetBkMode(TRANSPARENT);

	if (lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
	}
	else
	{
		pDC->SetTextColor(dwTextColor);
	}

	CString strText;
	GetText(lpDrawItemStruct->itemID, strText);
	CRect rect = lpDrawItemStruct->rcItem;

	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;
	
	pDC->DrawText(strText, -1, &rect, nFormat | DT_CALCRECT);
	pDC->DrawText(strText, -1, &rect, nFormat);

	pDC->SetTextColor(dwOldTextColor); 
	pDC->SetBkMode(nOldBkMode);
	pDC->SelectObject(pOldFont);
}

void COutputListBox::PreSubclassWindow()
{
	ASSERT( (GetStyle() & LBS_OWNERDRAWVARIABLE) == LBS_OWNERDRAWVARIABLE );
	ASSERT( (GetStyle() & LBS_HASSTRINGS) == LBS_HASSTRINGS );

	CListBox::PreSubclassWindow();
}

void COutputListBox::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);

	SetCurSel(-1);
}

BOOL COutputListBox::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect( rc );
	pDC->FillSolidRect( rc, ::GetSysColor(COLOR_WINDOW) );
	return TRUE;
}
