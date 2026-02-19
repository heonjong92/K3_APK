#pragma once
#include <afxwin.h>
#include <atlimage.h>

class CImageToolTip : public CToolTipCtrl
{
public:
	CImageToolTip();
	virtual ~CImageToolTip();

	BOOL Create(CWnd* pParentWnd);
	void SetImage(LPCTSTR lpszPath);
	void SetText(const CString& strText);
	void ShowTip(CPoint pt);
	void HideTip();

protected:
	afx_msg void OnPaint();
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

private:
	CImage m_img;
	CString m_strText;
	CWnd* m_pParent;
};
