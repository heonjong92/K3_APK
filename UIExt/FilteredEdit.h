#pragma once


namespace UIExt
{
// CFilteredEdit
extern const UINT WM_FILTERED_EDIT_MODIFIED;
class CFilteredEdit : public CEdit
{
	DECLARE_DYNAMIC(CFilteredEdit)
protected:
	CBrush		m_BgBrush;
	COLORREF	m_dwTextColor;
	COLORREF	m_dwActiveTextColor;
	COLORREF	m_dwBorderColor;
	COLORREF	m_dwBgColor;

	BOOL		m_bNowEditing;

	CString		m_strAllowString;
	CString		m_strNoneAllowString;

	void DrawEditFrame( CDC* pDC );

public:
	CFilteredEdit();
	virtual ~CFilteredEdit();

	void SetColor( COLORREF dwBorderColor, COLORREF dwBgColor );
	void SetFilter( LPCTSTR lpszFilter );
	void SetInvertFilter( LPCTSTR lpszFilter );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnPaint();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


} // UIExt