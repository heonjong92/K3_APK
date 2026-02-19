#pragma once


// COutputListBox

namespace UIExt {

class COutputListBox : public CListBox
{
	DECLARE_DYNAMIC(COutputListBox)

public:
	enum ItemType { ItemTypeNormal, ItemTypeNotification, ItemTypeError };
	COutputListBox();
	virtual ~COutputListBox();

	int AddString( LPCTSTR lpszItem, ItemType type=ItemTypeNormal );

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

}
