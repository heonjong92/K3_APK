// NewListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "NewListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewListCtrl

CNewListCtrl::CNewListCtrl() : m_pImageList(NULL)
{
	m_dwTextColor = RGB(0, 0, 0);
	m_dwChangeTextColor = RGB(255, 255, 255);
	m_dwOddBKColor = RGB(255, 255, 255);
	m_dwEvenBKColor = RGB(211, 211, 211);
	m_dwChangeBKColor = RGB(255, 0, 0);
	m_dwChangeBKColor2 = RGB(0, 0, 255);
	m_dwChangeBKColor3 = RGB(0, 128, 64);

	m_nItemCnt = 0;
	m_nSubItemCnt = 0;

	ResetList();
}

CNewListCtrl::~CNewListCtrl()
{
	ResetList();

	if ( m_pImageList != NULL )
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
		m_pImageList = NULL;
	}
}


BEGIN_MESSAGE_MAP(CNewListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CNewListCtrl)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDrawList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewListCtrl message handlers
void CNewListCtrl::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	
	switch( pLVCD->nmcd.dwDrawStage )
	{
	case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;

	case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
		}
		break;

	case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
		{
			int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
			int nSubItem = static_cast<int>(pLVCD->iSubItem);
			
			if ( nItem % 2 == 0 )
			{
				pLVCD->clrText = m_dwTextColor;
				pLVCD->clrTextBk = m_dwEvenBKColor;
			}
			else
			{
				pLVCD->clrText = m_dwTextColor;
				pLVCD->clrTextBk = m_dwOddBKColor;
			}

			std::vector<ChangeBKPoint>::iterator iterStart = m_vChangeBKPoint.begin();
			std::vector<ChangeBKPoint>::iterator iterEnd = m_vChangeBKPoint.end();
			std::vector<ChangeBKPoint>::iterator iter;
//			for( iter = iterStart ; iter != iterEnd ; std::advance(iter, 1) )
			for( iter = iterStart ; iter != iterEnd ; ++iter )
			{
				if( nItem == (*iter).nItem && nSubItem == (*iter).nSubItem )
				{
					pLVCD->clrText = m_dwChangeTextColor;
					pLVCD->clrTextBk = m_dwChangeBKColor;
				}
			}

			iterStart = m_vChangeBKPoint2.begin();
			iterEnd = m_vChangeBKPoint2.end();
//			for( iter = iterStart ; iter != iterEnd ; std::advance(iter, 1) )
			for( iter = iterStart ; iter != iterEnd ; ++iter )
			{
				if( nItem == (*iter).nItem && nSubItem == (*iter).nSubItem )
				{
					pLVCD->clrText = m_dwChangeTextColor;
					pLVCD->clrTextBk = m_dwChangeBKColor2;
				}
			}

			iterStart = m_vChangeBKPoint3.begin();
			iterEnd = m_vChangeBKPoint3.end();
//			for( iter = iterStart ; iter != iterEnd ; std::advance(iter, 1) )
			for( iter = iterStart ; iter != iterEnd ; ++iter )
			{
				if( nItem == (*iter).nItem && nSubItem == (*iter).nSubItem )
				{
					pLVCD->clrText = m_dwChangeTextColor;
					pLVCD->clrTextBk = m_dwChangeBKColor3;
				}
			}
		}
		break;

	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

void CNewListCtrl::InsertNewColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	this->InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);

	++m_nSubItemCnt;

	Invalidate(FALSE);
}

void CNewListCtrl::InsertNewItem(int nItem, LPCTSTR lpszItem)
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	CString str;
	str = lpszItem;
	lvi.pszText = str.GetBuffer();
	
	CListCtrl::InsertItem(&lvi);

	++m_nItemCnt;

	Invalidate(FALSE);
}


void CNewListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText, BOOL bIsBKChange, BOOL bIsBKChange2, BOOL bIsBKChange3 )
{
	ChangeBKPoint stChangeBK;

	if( bIsBKChange && !bIsBKChange2 )
	{
		stChangeBK.nItem = nItem;
		stChangeBK.nSubItem = nSubItem;
		m_vChangeBKPoint.push_back(stChangeBK);
	}
	else if( !bIsBKChange && bIsBKChange2 )
	{
		stChangeBK.nItem = nItem;
		stChangeBK.nSubItem = nSubItem;
		m_vChangeBKPoint2.push_back(stChangeBK);
	}
	else if( bIsBKChange3 )
	{
		stChangeBK.nItem = nItem;
		stChangeBK.nSubItem = nSubItem;
		m_vChangeBKPoint3.push_back(stChangeBK);
	}

	CListCtrl::SetItemText(nItem, nSubItem, lpszText);

	Invalidate(FALSE);
}

void CNewListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
	
	ResetList();
	
	if ( m_pImageList != NULL )
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
		m_pImageList = NULL;
	}
}

void CNewListCtrl::ResetList()
{
	m_vChangeBKPoint.clear();
	m_vChangeBKPoint2.clear();
	m_vChangeBKPoint3.clear();

	m_nItemCnt = 0;
}

void CNewListCtrl::SetItemHeight(int nHeight)
{
	if ( m_pImageList != NULL )
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
		m_pImageList = NULL;
	}

	m_pImageList = new CImageList;
	m_pImageList->Create(1, nHeight, ILC_COLOR24, 1, 0);

	SetImageList(m_pImageList, LVSIL_SMALL);
}
