#pragma once

#include "GdipWndExt.h"

#include <map>
#include <vector>

// CStripInfoCtrl

namespace UIExt
{

extern const UINT WM_STRIPINFO_DOT_SELECTED;
extern const UINT WM_STRIPINFO_UNIT_SELECTED;

class CStripInfoCtrl : public CGdipStatic
{
	DECLARE_DYNAMIC(CStripInfoCtrl)

	enum UnitType2D { UnitType2DNone, UnitType2DGrab1, UnitType2DGrab2, UnitType2DOK, UnitType2DNG };

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

	typedef std::pair<int, int> UnitIndex;			// (col, row)

	typedef std::vector<Gdiplus::Rect> ArrayRect;
	typedef std::vector<UnitIndex> DotPosition;
	typedef std::map<UnitIndex, Gdiplus::Rect> MapUnit;
	typedef std::map<UnitIndex, UnitType2D> MapUnitType2D;
	typedef std::map<UnitIndex, short> MapUnitSideMask;
	typedef std::vector<int>	ArrayRowIndex;

	int			m_nRowCount;
	int			m_nColumnCount;
	int			m_nActiveRowIndex;
	CString		m_strStripId;

	ArrayRect			m_ArrayDot;
	ArrayRect			m_ArrayRowTip;
	ArrayRowIndex		m_ArrayInvalidRowIndex;
	MapUnit				m_MapUnit;
	MapUnitType2D		m_MapUnitType2D;

	UnitIndex	m_HoverIndexUnit;
	UnitIndex	m_PressIndexUnit;

	BOOL		m_bIsButtonDown;

	Gdiplus::SolidBrush*	m_pBrushGrab1;
	Gdiplus::SolidBrush*	m_pBrushGrab2;
	Gdiplus::SolidBrush*	m_pBrushOK;
	Gdiplus::SolidBrush*	m_pBrushNG;

	Gdiplus::SolidBrush*	m_pBrushScanning;
	Gdiplus::SolidBrush*	m_pBrushMeasured;

	void DrawStrip( Graphics& g, Rect& rectStripBody );
	void DrawLegend( Graphics& g, Rect& rectLegendBody );
	void DrawStripId( Graphics& g, RectF& rectStripIdF );

	BOOL HitTest( CPoint point, UnitIndex& IndexUnit );

	CString GetLegendText(UnitType2D type);

	BOOL IsInvalidRowIndex(int nRowIndex);
public:
	void SetIdText( LPCTSTR lpszId );
	void SetIndexData( int nRowCount, int nColumnCount );
	void EnableWindow( BOOL bEnable );
	BOOL IsWindowEnabled() const;
	BOOL IsSelectUnit() const;
	void GetSelectedUnitIndex( int& nColumn, int& nRow ) const;
	void ResetSelectedItem();

	void SetInvalidRowIndex( int nRowIndex );

	void Reset();

	void SetUnitType2D( int nColumn, int nRow, UnitType2D type );

	void SetActiveRowIndex( int nRowIndex );
	const int GetActiveRowIndex() const { return m_nActiveRowIndex; }

public:
	CStripInfoCtrl();
	virtual ~CStripInfoCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

} // namespace UIExt