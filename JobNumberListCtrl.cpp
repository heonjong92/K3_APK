#include "stdafx.h"
#include "JobNumberListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace JobNumberListInnerControl
{
class CInPlaceCombo : public CComboBox
{
public:
		
// Implementation
	
	// Returns the instance of the class
	static CInPlaceCombo* GetInstance(); 

	// Deletes the instance of the class
	static void DeleteInstance(); 

	// Creates the Windows combo control and attaches it to the object, if needed and shows the combo ctrl
	BOOL ShowComboCtrl(DWORD dwStyle, const CRect& rCellRect, CWnd* pParentWnd, UINT uiResourceID,
					   int iRowIndex, int iColumnIndex, CStringList* pDropDownList, CString strCurSelecetion = _T(""), int iCurSel = -1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(CInPlaceCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCloseup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

// Implementation
	// Constructor
	CInPlaceCombo();

	// Hide the copy constructor and operator =
	CInPlaceCombo (CInPlaceCombo&) {}

	void operator = (CInPlaceCombo) {}

	// Destructor
	virtual ~CInPlaceCombo();

// Attributes

	// Index of the item in the list control
	int m_iRowIndex;

	// Index of the subitem in the list control
	int m_iColumnIndex;

	// To indicate whether ESC key was pressed
	BOOL m_bESC;
	
	// Singleton instance
	static CInPlaceCombo* m_pInPlaceCombo;

	// Previous selected string value in the combo control
	CString m_strWindowText;

	// List of items to be shown in the drop down
	CStringList m_DropDownList;
};

CInPlaceCombo* CInPlaceCombo::m_pInPlaceCombo = NULL; 

CInPlaceCombo::CInPlaceCombo()
{
	m_iRowIndex = -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
}

CInPlaceCombo::~CInPlaceCombo()
{
}

BEGIN_MESSAGE_MAP(CInPlaceCombo, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceCombo)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCombo message handlers

int CInPlaceCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);
	
	SetFocus();

	ResetContent(); 
	for (POSITION Pos_ = m_DropDownList.GetHeadPosition(); Pos_ != NULL;)
	{
		AddString((LPCTSTR) (m_DropDownList.GetNext(Pos_)));
	}

	return 0;
}

BOOL CInPlaceCombo::PreTranslateMessage(MSG* pMsg) 
{
	// If the message if for "Enter" or "Esc"
	// Do not process
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			// DO NOT process further
			return TRUE;				
		}
	}
	
	return CComboBox::PreTranslateMessage(pMsg);
}

void CInPlaceCombo::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	// Get the current selection text
	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iRowIndex;
	dispinfo.item.iSubItem = m_iColumnIndex;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : str.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	// Close the control
	PostMessage(WM_CLOSE);
}

void CInPlaceCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// If the key is "Esc" set focus back to the list control
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return;
	}
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CInPlaceCombo::OnCloseup() 
{
	// Set the focus to the parent list control
	GetParent()->SetFocus();
}

CInPlaceCombo* CInPlaceCombo::GetInstance()
{
	if(m_pInPlaceCombo == NULL)
	{
		m_pInPlaceCombo = new CInPlaceCombo;
	}
	return m_pInPlaceCombo;
}

void CInPlaceCombo::DeleteInstance()
{
	delete m_pInPlaceCombo;
	m_pInPlaceCombo = NULL;
}

BOOL CInPlaceCombo::ShowComboCtrl(DWORD dwStyle, const CRect &rCellRect, CWnd* pParentWnd, UINT uiResourceID,
								  int iRowIndex, int iColumnIndex, CStringList* pDropDownList, 
								  CString strCurSelecetion /*= ""*/, int iCurSel /*= -1*/)
{

	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_bESC = FALSE;
	
	m_DropDownList.RemoveAll(); 
	m_DropDownList.AddTail(pDropDownList);

	BOOL bRetVal = TRUE;

	if (-1 != iCurSel)
	{
		GetLBText(iCurSel, m_strWindowText);
	}
	else if (!strCurSelecetion.IsEmpty()) 
	{
		m_strWindowText = strCurSelecetion;
	}
	
	if (NULL == m_pInPlaceCombo->m_hWnd) 
	{
		bRetVal = m_pInPlaceCombo->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}

	SetCurSel(iCurSel);

	return bRetVal;
}

class CInPlaceEdit : public CEdit
{

public:
	
// Implementation
	
	// Returns the instance of the class
	static CInPlaceEdit* GetInstance(); 

	// Deletes the instance of the class
	static void DeleteInstance(); 

	// Creates the Windows edit control and attaches it to the object
	// Shows the edit ctrl
	BOOL ShowEditCtrl(DWORD dwStyle, const RECT& rCellRect, CWnd* pParentWnd, 
					  UINT uiResourceID, int iRowIndex, int iColumnIndex,
					  CString& strValidChars, CString& rstrCurSelection);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL


// Attributes

protected:	
	// Generated message map functions
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnPaste(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

// Implementation

	// Constructor
	CInPlaceEdit();

	// Hide the copy constructor and operator =
	CInPlaceEdit (CInPlaceEdit&) {}

	void operator = (CInPlaceEdit) {}
	
	// Destructor
	virtual ~CInPlaceEdit();

// Attributes

	// Index of the item in the list control
	int m_iRowIndex;

	// Index of the subitem in the list control
	int m_iColumnIndex;

	// To indicate whether ESC key was pressed
	BOOL m_bESC;
	
	// Valid characters
	CString m_strValidChars;

	// Singleton instance
	static CInPlaceEdit* m_pInPlaceEdit;

	// Previous string value in the edit control
	CString m_strWindowText;
};

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit

CInPlaceEdit* CInPlaceEdit::m_pInPlaceEdit = NULL;  

CInPlaceEdit::CInPlaceEdit()
{
	m_iRowIndex= -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
	m_strValidChars.Empty();
}

CInPlaceEdit::~CInPlaceEdit()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers

LRESULT CInPlaceEdit::OnPaste(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_strValidChars.IsEmpty())
	{
		return 0;	
	}

    CString strFromClipboard;

	// get the text from clipboard
	if(OpenClipboard()) {
		HANDLE l_hData = GetClipboardData(CF_TEXT);
		if(NULL == l_hData) {
			return 0;
		}
		
		char *l_pBuffer = (char*)GlobalLock(l_hData);
		if(NULL != l_pBuffer) {
			strFromClipboard = l_pBuffer;
		}

		GlobalUnlock(l_hData);
		CloseClipboard();
	}

	// Validate the characters before pasting 
	for(int iCounter_ = 0; iCounter_ < strFromClipboard.GetLength(); iCounter_++)
	{
		if (-1 == m_strValidChars.Find(strFromClipboard.GetAt(iCounter_)))
		{
			return 0;
		}
	}
		
	//let the individual control handle other processing
	return CEdit::Default();	
}

void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here

	// Get the text in the edit ctrl
	CString strEdit;
	GetWindowText(strEdit);

	// Send Notification to parent of edit ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iRowIndex;
	dispinfo.item.iSubItem = m_iColumnIndex;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)strEdit);
	dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : strEdit.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}
#define CTRL_C	0x3
#define CTRL_V	0x16
#define CTRL_X	0x18
void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

 	if ((m_strValidChars.IsEmpty()) || ((-1 != m_strValidChars.Find(static_cast<TCHAR> (nChar))) || 
		(nChar == VK_BACK) || (nChar == CTRL_C) || (nChar == CTRL_V) || (nChar == CTRL_X)))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
}

BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message && (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	ShowWindow(SW_SHOW);
	SetWindowText(m_strWindowText);
	SetSel(0, -1);
	SetFocus();
	
	  
	return 0;
}

CInPlaceEdit* CInPlaceEdit::GetInstance()
{
	if(m_pInPlaceEdit == NULL)
	{
		m_pInPlaceEdit = new CInPlaceEdit;
	}
	return m_pInPlaceEdit;
}

void CInPlaceEdit::DeleteInstance()
{
	delete m_pInPlaceEdit;
	m_pInPlaceEdit = NULL;
}

BOOL CInPlaceEdit::ShowEditCtrl(DWORD dwStyle, const RECT &rCellRect, CWnd* pParentWnd, 
								UINT uiResourceID, int iRowIndex, int iColumnIndex,
								CString& strValidChars, CString& rstrCurSelection)
{
	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_strValidChars = strValidChars;
	m_strWindowText = rstrCurSelection;
	m_bESC = FALSE;

	if (NULL == m_pInPlaceEdit->m_hWnd) 
	{
		return m_pInPlaceEdit->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}	

	return TRUE;
}

} // namespace JobNumberListInnerControl

//#defines
#define FIRST_COLUMN				0
#define MIN_COLUMN_WIDTH			10
#define MAX_DROP_DOWN_ITEM_COUNT	10

using namespace JobNumberListInnerControl;

/////////////////////////////////////////////////////////////////////////////
// CJobNumberListCtrl

CJobNumberListCtrl::CJobNumberListCtrl()
{
	m_ComboSupportColumnsList.RemoveAll();
	m_ReadOnlyColumnsList.RemoveAll();
	m_strValidEditCtrlChars.Empty();
	m_dwEditCtrlStyle = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL;
	m_dwDropDownCtrlStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
							CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;

	m_dwRowColor1 = RGB(230,230,255);
	m_dwRowColor2 = RGB(255,255,255);
}

CJobNumberListCtrl::~CJobNumberListCtrl()
{
	CInPlaceCombo::DeleteInstance();
	CInPlaceEdit::DeleteInstance();  
}


BEGIN_MESSAGE_MAP(CJobNumberListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CJobNumberListCtrl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobNumberListCtrl message handlers

CInPlaceCombo* CJobNumberListCtrl::ShowInPlaceList(int iRowIndex, int iColumnIndex, CStringList& rComboItemsList, 
											   CString strCurSelecetion /*= ""*/, int iSel /*= -1*/)
{
	// The returned obPointer should not be saved
	
	// Make sure that the item is visible
	if (!EnsureVisible(iRowIndex, TRUE))
	{
		return NULL;
	}

	// Make sure that iColumnIndex is valid 
	CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*> (GetDlgItem(FIRST_COLUMN));

	int iColumnCount = pHeader->GetItemCount();

	if (iColumnIndex >= iColumnCount || GetColumnWidth(iColumnIndex) < MIN_COLUMN_WIDTH) 
	{
		return NULL;
	}

	// Calculate the rectangle specifications for the combo box
	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);

	int iHeight = obCellRect.Height();  
	int iCount = (int)rComboItemsList.GetCount();

	iCount = (iCount < MAX_DROP_DOWN_ITEM_COUNT) ? 
		iCount + MAX_DROP_DOWN_ITEM_COUNT : (MAX_DROP_DOWN_ITEM_COUNT + 1); 

	obCellRect.bottom += iHeight * iCount; 

	// Create the in place combobox
	CInPlaceCombo* pInPlaceCombo = CInPlaceCombo::GetInstance();
	pInPlaceCombo->ShowComboCtrl(m_dwDropDownCtrlStyle, obCellRect, this, 0, iRowIndex, iColumnIndex, &rComboItemsList, 
								 strCurSelecetion, iSel);
	
	return pInPlaceCombo;
}

CInPlaceEdit* CJobNumberListCtrl::ShowInPlaceEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection)
{
	// Create an in-place edit control
	CInPlaceEdit* pInPlaceEdit = CInPlaceEdit::GetInstance();
		
	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);
			
	pInPlaceEdit->ShowEditCtrl(m_dwEditCtrlStyle, obCellRect, this, 0, 
							   iRowIndex, iColumnIndex,
							   m_strValidEditCtrlChars, rstrCurSelection);

	return pInPlaceEdit;
}

void CJobNumberListCtrl::OnHScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnHScroll(iSBCode, iPos, pScrollBar);
}

void CJobNumberListCtrl::OnVScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnVScroll(iSBCode, iPos, pScrollBar);
}

void CJobNumberListCtrl::OnLButtonDown(UINT iFlags, CPoint obPoint) 
{
	// TODO: Add your message handler code here and/or call default

	int iColumnIndex = -1;
	int iRowIndex = -1;

	// Get the current column and row
	if (!HitTestEx(obPoint, &iRowIndex, &iColumnIndex))
	{
		return;
	}

	CListCtrl::OnLButtonDown(iFlags, obPoint);
	
	// If column is not read only then
	// If the SHIFT or CTRL key is down call the base class
	// Check the high bit of GetKeyState to determine whether SHIFT or CTRL key is down
	if ((GetKeyState(VK_SHIFT) & 0x80) || (GetKeyState(VK_CONTROL) & 0x80))
	{
		return;
	}

	// Get the current selection before creating the in place combo box
	CString strCurSelection = GetItemText(iRowIndex, iColumnIndex);
	
	if (-1 != iRowIndex)
	{
		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				// If combo box is supported
				// Create and show the in place combo box
				if (IsCombo(iColumnIndex))
				{
					CStringList obComboItemsList;
										
					GetParent()->SendMessage(WM_SET_ITEMS, (WPARAM)iColumnIndex, (LPARAM)&obComboItemsList);  
					
					CInPlaceCombo* pInPlaceComboBox = ShowInPlaceList(iRowIndex, iColumnIndex, obComboItemsList, strCurSelection);
					ASSERT(pInPlaceComboBox); 
					
					// Set the selection to previous selection
					pInPlaceComboBox->SelectString(-1, strCurSelection);
				}
				// If combo box is not read only
				// Create and show the in place edit control
				else if (!IsReadOnly(iColumnIndex))
				{
					CInPlaceEdit* pInPlaceEdit = ShowInPlaceEdit(iRowIndex, iColumnIndex, strCurSelection);
				}
			}
		}
	}  
}

bool CJobNumberListCtrl::HitTestEx(CPoint &obPoint, int* pRowIndex, int* pColumnIndex) const
{
	if (!pRowIndex || !pColumnIndex)
	{
		return false;
	}

	// Get the row index
	*pRowIndex = HitTest(obPoint, NULL);

	if (pColumnIndex)
	{
		*pColumnIndex = 0;
	}

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		return false;
	}

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	int iColumnCount = pHeader->GetItemCount();

	// Get bounding rect of item and check whether obPoint falls in it.
	CRect obCellRect;
	GetItemRect(*pRowIndex, &obCellRect, LVIR_BOUNDS);
	
	if (obCellRect.PtInRect(obPoint))
	{
		// Now find the column
		for (*pColumnIndex = 0; *pColumnIndex < iColumnCount; (*pColumnIndex)++)
		{
			int iColWidth = GetColumnWidth(*pColumnIndex);
			
			if (obPoint.x >= obCellRect.left && obPoint.x <= (obCellRect.left + iColWidth))
			{
				return true;
			}
			obCellRect.left += iColWidth;
		}
	}
	return false;
}

void CJobNumberListCtrl::SetComboColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ComboSupportColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ComboSupportColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ComboSupportColumnsList.RemoveAt(Pos); 
	}
}

void CJobNumberListCtrl::SetReadOnlyColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ReadOnlyColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ReadOnlyColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ReadOnlyColumnsList.RemoveAt(Pos); 
	}
}

bool CJobNumberListCtrl::IsReadOnly(int iColumnIndex)
{
	if (m_ReadOnlyColumnsList.Find(iColumnIndex))
	{
		return true;
	}
	
	return false;
}

bool CJobNumberListCtrl::IsCombo(int iColumnIndex)
{
	if (m_ComboSupportColumnsList.Find(iColumnIndex))
	{
		return true;
	}

	return false;
}

void CJobNumberListCtrl::CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect)
{
	GetItemRect(iRowIndex, &robCellRect, LVIR_BOUNDS);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (robCellRect.right > rcClient.right) 
	{
		robCellRect.right = rcClient.right;
	}

	ScrollToView(iColumnIndex, robCellRect); 
}

void CJobNumberListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	// Update the item text with the new text
	SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);

	GetParent()->SendMessage(WM_VALIDATE, GetDlgCtrlID(), (LPARAM)pDispInfo); 
	
	*pResult = 0;
}

void CJobNumberListCtrl::SetValidEditCtrlCharacters(CString &rstrValidCharacters)
{
	m_strValidEditCtrlChars = rstrValidCharacters;
}

void CJobNumberListCtrl::EnableHScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_HSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_HSCROLL;
	}	
}

void CJobNumberListCtrl::EnableVScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_VSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_VSCROLL;
	}
}

void CJobNumberListCtrl::ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& robCellRect)
{
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);

	int iColumnWidth = GetColumnWidth(iColumnIndex);

	// Get the column iOffset
	int iOffSet = 0;
	for (int iIndex_ = 0; iIndex_ < iColumnIndex; iIndex_++)
	{
		iOffSet += GetColumnWidth(iIndex_);
	}

	// If x1 of cell rect is < x1 of ctrl rect or
	// If x1 of cell rect is > x1 of ctrl rect or **Should not ideally happen**
	// If the width of the cell extends beyond x2 of ctrl rect then
	// Scroll

	CSize obScrollSize(0, 0);

	if (((iOffSet + robCellRect.left) < rcClient.left) || 
		((iOffSet + robCellRect.left) > rcClient.right))
	{
		obScrollSize.cx = iOffSet + robCellRect.left;
	}
	else if ((iOffSet + robCellRect.left + iColumnWidth) > rcClient.right)
	{
		obScrollSize.cx = iOffSet + robCellRect.left + iColumnWidth - rcClient.right;
	}

	Scroll(obScrollSize);
	robCellRect.left -= obScrollSize.cx;
	
	// Set the width to the column width
	robCellRect.left += iOffSet;
	robCellRect.right = robCellRect.left + iColumnWidth;
}

void CJobNumberListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (IsReadOnly(pDispInfo->item.iSubItem))
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CJobNumberListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	int iRow = (int)lplvcd->nmcd.dwItemSpec;
	
	switch(lplvcd->nmcd.dwDrawStage)
	{
    case CDDS_PREPAINT :
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
		
    case CDDS_ITEMPREPAINT:
		{
			lplvcd->clrText = RGB(0,0,0);
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}
		
		// Modify sub item text and/or background
    case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			if(iRow %2)
			{
				lplvcd->clrTextBk = m_dwRowColor2;
			}
			else{
				lplvcd->clrTextBk = m_dwRowColor1;
			}			
			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

BOOL CJobNumberListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(rect);
	
	POINT pt;  
	
	CBrush Brush1( m_dwRowColor1 );
	CBrush Brush2( m_dwRowColor2 );
	
	int nChunkHeight = GetCountPerPage();
	
	pDC->FillRect( &rect, &Brush2 );
	
	for( int i=0 ; i<=nChunkHeight ; i++ )
	{
		GetItemPosition( i, &pt );
		rect.top = pt.y;
		GetItemPosition( i+1, &pt );
		rect.bottom = pt.y;
		pDC->FillRect( &rect, i%2 ? &Brush2 : &Brush1 );
	}
	
	Brush1.DeleteObject();
	Brush2.DeleteObject();
	
	return FALSE;
}
