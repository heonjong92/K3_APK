#if !defined(AFX_NEWLISTCTRL_H__EFBAE959_0734_42C5_AD30_3B85FF01B8E8__INCLUDED_)
#define AFX_NEWLISTCTRL_H__EFBAE959_0734_42C5_AD30_3B85FF01B8E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
// NewListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewListCtrl window

/**
	@struct			_ChangeBKPoint
	@brief			리스트 컨트롤에서 색을 변경시켜줄 좌표
	@remark			SetItemText를 했을 때 변경시켜줄 Item 번호와 그 SubItem 번호를 저장해둔다.
*/
typedef struct _ChangeBKPoint
{
	int nItem;			///< 아이템 번호
	int nSubItem;		///< 서브아이템(Column)번호
} ChangeBKPoint;

/**
	@class			CNewListCtrl
	@brief			리스트 색상이 변하는 리스트 컨트롤
	@remark			특정한 데이터가 일정값 이하일 때, 그 부분의 배경색과 글자색이 바뀌는 기능이 추가되었다.
*/
class CNewListCtrl : public CListCtrl
{
// Construction
public:
	CNewListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ResetList();
	void InsertNewItem(int nItem, LPCTSTR lpszItem);
	void InsertNewColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	void SetItemText(int nItem, int nSubItem, LPCTSTR lpszText, BOOL bIsBKChange, BOOL bIsBKChange2, BOOL bIsBKChange3);
	virtual ~CNewListCtrl();

public:
	void SetItemHeight(int nHeight);
	int GetItemCnt()									{ return m_nItemCnt; }
	int GetSubItemCnt()									{ return m_nSubItemCnt; }

	void SetTextColor(COLORREF dwTextColor)				{ m_dwTextColor = dwTextColor; }
	void SetOddBKColor(COLORREF dwOddBKColo)			{ m_dwOddBKColor = dwOddBKColo; }
	void SetEvenBKColor(COLORREF dwEvenBKColor)			{ m_dwEvenBKColor = dwEvenBKColor; }
	void SetChangeTextColor(COLORREF dwChangeTextColor)	{ m_dwChangeTextColor = dwChangeTextColor; }
	void SetChangeBKColor(COLORREF dwChangeBKColor)		{ m_dwChangeBKColor = dwChangeBKColor; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewListCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	COLORREF	m_dwTextColor;			///< 텍스트 색
	
	COLORREF	m_dwOddBKColor;			///< 홀수열 배경색
	COLORREF	m_dwEvenBKColor;		///< 짝수열 배경색

	COLORREF	m_dwChangeTextColor;	///< 변경된 텍스트색
	COLORREF	m_dwChangeBKColor;		///< 변경된 배경색
	COLORREF	m_dwChangeBKColor2;
	COLORREF	m_dwChangeBKColor3;

	int			m_nItemCnt;				///< 아이템 개수
	int			m_nSubItemCnt;			///< 열 개수

	CImageList*	m_pImageList;			///< 이미지 리스트(리스트 컨트롤 높이 조절용)

	std::vector<ChangeBKPoint>	m_vChangeBKPoint;		///< 배경색 변경시켜줄 좌표
	std::vector<ChangeBKPoint>	m_vChangeBKPoint2;		///< 배경색 변경시켜줄 좌표
	std::vector<ChangeBKPoint>	m_vChangeBKPoint3;		///< 배경색 변경시켜줄 좌표
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWLISTCTRL_H__EFBAE959_0734_42C5_AD30_3B85FF01B8E8__INCLUDED_)
