#pragma once

#include "GdipWndExt.h"

#include <map>
#include <vector>

// CObjectInfoCtrlEx

#define MAX_COL_COUNT 64	//X
#define MAX_ROW_COUNT 64	//Y

namespace UIExt
{

extern const UINT WM_OBJECTINFO_DOT_SELECTED_EX;
extern const UINT WM_OBJECTINFO_UNIT_SELECTED_EX;
extern const UINT WM_OBJECTINFO_UNIT_STATE_CHANGE_EX;
extern const UINT WM_OBJECTINFO_UNIT_UNSELECTED_EX;

class CObjectInfoCtrlEx : public CGdipStatic
{
	DECLARE_DYNAMIC(CObjectInfoCtrlEx)

	enum ObjectDirection { Direction_L2R = 0, Direction_R2L };
	enum UnitTypeResult { UnitType_None, UnitType_OK, UnitType_Empty, UnitType_Error };


protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

	typedef std::pair<int, int> UnitIndex;			// (col, row)

	typedef std::vector<Gdiplus::Rect> ArrayRect;
	typedef std::vector<UnitIndex> DotPosition;
	typedef std::map<UnitIndex, Gdiplus::Rect> MapUnit;
	typedef std::map<UnitIndex, UnitTypeResult> MapUnitTypeResult;
	typedef std::map<UnitIndex, short> MapUnitSideMask;
	typedef std::map<UnitIndex, int> MapGroupIndex;
	typedef std::vector<int>	ArrayRowIndex;

	int			m_nRowCount;
	int			m_nColumnCount;
	int			m_nActiveRowIndex;
	BOOL		m_bUseDot;
	CString		m_strStripId;

	ArrayRect			m_ArrayDot;
	ArrayRect			m_ArrayRowTip;
	ArrayRowIndex		m_ArrayInvalidRowIndex;
	MapUnit				m_MapUnit;
	MapUnitTypeResult	m_MapUnitTypeResult;
	MapGroupIndex		m_MapGroupIndex;

	DotPosition		m_DotPosition;

	int			m_nHoverIndexDot;
	UnitIndex	m_HoverIndexUnit;

	int			m_nPressIndexDot;
	UnitIndex	m_PressIndexUnit;

	BOOL		m_bIsButtonDown;

	BOOL		m_bIs3DMode;

	Gdiplus::SolidBrush*	m_pBrushGrab1;
	Gdiplus::SolidBrush*	m_pBrushGrab2;
	Gdiplus::SolidBrush*	m_pBrushGrab3;
	Gdiplus::SolidBrush*	m_pBrushGrab4;

	//Gdiplus::SolidBrush*	m_pBrushOK;
	//Gdiplus::SolidBrush*	m_pBrushReject;
	//Gdiplus::SolidBrush*	m_pBrushNG_Shift;
	//Gdiplus::SolidBrush*	m_pBrushNG_Tilt;
	//Gdiplus::SolidBrush*	m_pBrushNG_Damaged;
	//Gdiplus::SolidBrush*	m_pBrushNG_Double;
	//Gdiplus::SolidBrush*	m_pBrushRejectX;

	//Gdiplus::SolidBrush*	m_pBrushDot;
	//Gdiplus::SolidBrush*	m_pBrushSkip;

	Gdiplus::SolidBrush*	m_pBrush_None;
	Gdiplus::SolidBrush*	m_pBrush_OK;
	Gdiplus::SolidBrush*	m_pBrushEmpty;
	Gdiplus::SolidBrush*	m_pBrushError;

	Gdiplus::SolidBrush*	m_pBrushScanning;
	Gdiplus::SolidBrush*	m_pBrushMeasured;

	void DrawStrip( Graphics& g, Rect& rectStripBody );
	void DrawLegend( Graphics& g, Rect& rectLegendBody );
	void DrawObjectId( Graphics& g, RectF& rectObjectIdF );

	BOOL HitTest( CPoint point, int& nIndexDot, UnitIndex& IndexUnit );

	CString GetLegendText(UnitTypeResult type);

	void DrawDotCircles( Graphics& g, SolidBrush* pBrush, SolidBrush* pBrushDot, Rect rectDot, int nColumnIndex );
	void DrawDotCircles( Graphics& g, Pen* pPen, Rect rectDot );

	BOOL IsInvalidRowIndex(int nRowIndex);

	ObjectDirection m_objDirection;
	CToolTipCtrl*		m_pToolTip;
	CString				m_strToolTipText;

	int			m_nRowCountOneShot;
	int			m_nColumnCountOneShot;

public:
	void SetIdText( LPCTSTR lpszId );
	void SetIndexData( int nRowCount, int nColumnCount );
	void SetDirection(ObjectDirection objDir) { m_objDirection = objDir; }
	void UseDot(BOOL bUse);
	void EnableWindow( BOOL bEnable );
	BOOL IsWindowEnabled() const;
	BOOL IsSelectDot() const;
	BOOL IsSelectUnit() const;
	int GetSelectedDotIndex() const;
	void GetSelectedUnitIndex( int& nColumn, int& nRow ) const;
	void ResetSelectedItem();

	void SetInvalidRowIndex( int nRowIndex );

	void Reset();

	void SetUnitTypeResult( int nColumn, int nRow, UnitTypeResult type );
	void SetUnitTypeResult( int nColumn, int nRow, UnitTypeResult type,int nGroupIndex );

	void SetActiveRowIndex( int nRowIndex );
	const int GetActiveRowIndex() const { return m_nActiveRowIndex; }

	int GetGroupIndex( int nCol, int nRow );
	void SetIndexDataOneshot( int nRowCount, int nColumnCount );

public:
	CObjectInfoCtrlEx();
	virtual ~CObjectInfoCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

} // namespace UIExt