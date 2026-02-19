// StripInfoCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StripInfoCtrl.h"
#include "ResourceManager.h"

#include "../VisionSystem.h"
//#include "../MagazineResult.h"
//#include "../ModelInfo.h"
//#include "../Inspect2DResult.h"


#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CStripInfoCtrl

const UINT UIExt::WM_STRIPINFO_DOT_SELECTED = ::RegisterWindowMessage( _T("UIEXT_STRIPINFO_DOT_SELECTED") );
const UINT UIExt::WM_STRIPINFO_UNIT_SELECTED = ::RegisterWindowMessage( _T("UIEXT_STRIPINFO_UNIT_SELECTED") );

using namespace UIExt;

IMPLEMENT_DYNAMIC(CStripInfoCtrl, CGdipStatic)

CStripInfoCtrl::CStripInfoCtrl()
{
	m_nColumnCount = 14;
	m_nRowCount = 4;
	m_strStripId = _T("");
	m_nActiveRowIndex = 0;

	m_ArrayDot.resize(m_nColumnCount);
	for (int i=0 ; i<m_nColumnCount ; i++)
	{
		m_ArrayDot[i] = Rect(0,0,0,0);
	}

	m_ArrayRowTip.resize(m_nRowCount);
	for(int j = 0; j < m_nRowCount; j++)
	{
		for(int i = 0; i < m_nColumnCount; i++)
		{
			m_MapUnit.insert(MapUnit::value_type( std::make_pair(i,j), Rect(0,0,0,0)));
			m_MapUnitType2D.insert(MapUnitType2D::value_type(std::make_pair(i,j), UnitType2DNone));
		}
		m_ArrayRowTip[j] = Rect(0,0,0,0);
	}

	m_HoverIndexUnit.first = m_HoverIndexUnit.second = -1;
	m_PressIndexUnit.first = m_PressIndexUnit.second = -1;

	m_bIsButtonDown = FALSE;

	// 스트립 색상 정의
	m_pBrushOK = ::new SolidBrush( Color(0, 220, 0) );
	m_pBrushNG = ::new SolidBrush( Color(255, 0, 0) );

	m_pBrushGrab1 = ::new SolidBrush( Color(38,38,38) );		// (38, 89, 160, 255) 정도의 값을 사용
	m_pBrushGrab2 = ::new SolidBrush( Color(255,255,255) );		

	m_pBrushScanning = ::new SolidBrush(Color(130,255,130));
	m_pBrushMeasured = ::new SolidBrush(Color(255,130,130));
}

CStripInfoCtrl::~CStripInfoCtrl()
{
	::delete m_pBrushOK;
	::delete m_pBrushNG;

	::delete m_pBrushGrab1;
	::delete m_pBrushGrab2;

	::delete m_pBrushScanning;
	::delete m_pBrushMeasured;
}


BEGIN_MESSAGE_MAP(CStripInfoCtrl, CGdipStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CStripInfoCtrl::SetIdText( LPCTSTR lpszId )
{
	m_strStripId = lpszId;
	if (GetSafeHwnd())
		Invalidate();
}

void CStripInfoCtrl::SetIndexData( int nRowCount, int nColumnCount )
{
	if ((m_nColumnCount == nColumnCount) && (m_nRowCount == nRowCount))
		return;

	m_nColumnCount = nColumnCount;
	m_nRowCount = nRowCount;
	m_nActiveRowIndex = 0;

	if (m_nColumnCount <= 0) m_nColumnCount = 1;
	if (m_nRowCount <= 0) m_nRowCount = 1;
	if (m_nColumnCount > 40) m_nColumnCount = 40;
	if (m_nRowCount > 20) m_nRowCount = 20;

	m_ArrayDot.clear();
	m_MapUnit.clear();
	m_MapUnitType2D.clear();
	m_ArrayRowTip.clear();

	m_ArrayDot.resize(m_nColumnCount);
	for(int i = 0 ; i < m_nColumnCount; i++)
	{
		m_ArrayDot[i] = Rect(0,0,0,0);
	}

	m_ArrayRowTip.resize(m_nRowCount);
	for(int j = 0; j < m_nRowCount; j++)
	{
		for(int i = 0; i < m_nColumnCount; i++)
		{
			m_MapUnit.insert(MapUnit::value_type( std::make_pair(i,j), Rect(0,0,0,0)));
			m_MapUnitType2D.insert(MapUnitType2D::value_type(std::make_pair(i,j), UnitType2DNone));
		}
		m_ArrayRowTip[j] = Rect(0,0,0,0);
	}

	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

void CStripInfoCtrl::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.SetSmoothingMode( SmoothingModeAntiAlias );
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	const int nMarginXY = 0;
	const int nStripIdHeight = 20;
	const int nLegendHeight = 20;

	DWORD dwBgColor = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	Color colorBody;
	colorBody.SetFromCOLORREF(dwBgColor);
	g.FillRectangle( &SolidBrush(colorBody), rect.X, rect.Y, rect.Width+1, nStripIdHeight+nMarginXY );

	rect.X += nMarginXY;
	rect.Width -= nMarginXY*2+1;
	rect.Y += nMarginXY;
	rect.Height -= nMarginXY*2+1;

	Rect rectLegendBody;
	rectLegendBody.X = rect.X;
	rectLegendBody.Y = rect.GetTop();
	rectLegendBody.Width = rect.Width + 1;
	rectLegendBody.Height = nStripIdHeight + nLegendHeight + nMarginXY;
	

	rect.Y += nStripIdHeight;
	rect.Height -= nStripIdHeight;

	RectF rectStripIdF;
	rectStripIdF.X = (float)rect.X + 5.f;
	rectStripIdF.Y = (float)rect.Y-nStripIdHeight;
	rectStripIdF.Width = (float)rect.Width;
	rectStripIdF.Height = (float)nStripIdHeight;

	DrawStripId( g, rectStripIdF );

	// 범례 그리기
	DrawLegend( g, rectLegendBody );

	rect.Y += nLegendHeight;
	rect.Height -= nLegendHeight;

	Rect rectStripBody;
	rectStripBody.X = rect.X;
	rectStripBody.Y = rect.Y;
	rectStripBody.Width = rect.Width;
	rectStripBody.Height = rect.Height-1;
	rectStripBody.Inflate( -2, -2 );

	DrawStrip( g, rectStripBody );
}

void CStripInfoCtrl::DrawStrip( Graphics& g, Rect& rectStripBody )
{
	DWORD dwTitleColor = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	SolidBrush brushUnitBody( Color(160,160,160) );
	SolidBrush brushTitleBody( Color(GetRValue(dwTitleColor), GetGValue(dwTitleColor), GetBValue(dwTitleColor)) );
	SolidBrush brushTitle( Color(160,255,255,255) );
	SolidBrush brushActiveRowText( Color(160, 255, 255, 0) );

	Pen penSelectUnit( Color(255,255,0), 2.f );

	Color colorOverlayDot(100, 255, 255, 255);
	Color colorOverlayUnit(80, 255, 255, 255);

	Color colorOverlayRowTip(40, 255, 255, 255);

	int nRowCount = m_nRowCount+1;
	int nColCount = m_nColumnCount+1;

	float fStepX = (float)rectStripBody.Width / nColCount;
	float fFirstStepX = fStepX * 0.5f;
	fStepX = (rectStripBody.Width - fFirstStepX) / (nColCount-1);

	float fStepY = (float)rectStripBody.Height / nRowCount;
	float fFirstStepY = fStepY * 0.7f;
	fStepY = (rectStripBody.Height - fFirstStepY) / (nRowCount-1);

	float fPosX = (float)rectStripBody.X;
	float fPosY = (float)rectStripBody.Y;

	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	Gdiplus::FontFamily fontFamily(L"Arial");
	Gdiplus::Font fontTitle(&fontFamily/*UIExt::CResourceManager::Instance()->GetSystemFontFamily()*/, fFirstStepY * 0.4f, FontStyleBold);

	WCHAR wszTitle[10];
	Rect rectUnit;
	UnitIndex indexUnit;
	USES_CONVERSION;

	for(int i = 0; i < nRowCount; i++)
	{
		BOOL bIsInvalidRowIndex = (IsInvalidRowIndex(i-1)) ? TRUE : FALSE;

		for(int j = 0; j < nColCount; j++)
		{
			rectUnit.X = (int)(fPosX+0.5f) + 1;
			rectUnit.Y = (int)(fPosY+0.5f) + 1;
			if (j==nColCount-1)
				rectUnit.Width = (int)(fPosX+fFirstStepX+0.5f) + 1 - rectUnit.X - 1;
			else
				rectUnit.Width = (int)(fPosX+fStepX+0.5f) + 1 - rectUnit.X - 1;
				
			if (i==0)
				rectUnit.Height = (int)(fPosY+fFirstStepY+0.5f) + 1 - rectUnit.Y - 1;
			else
				rectUnit.Height = (int)(fPosY+fStepY+0.5f) + 1 - rectUnit.Y - 1;

			indexUnit.first = j;//nColCount-1-j-1;
			indexUnit.second = nRowCount - 1 - i; //i-1;

			if ((i==0) || (j==nColCount-1))
			{
				if ((i!=0) || (j!=nColCount-1))
				{
					g.FillRectangle( &brushTitleBody, rectUnit );
					if ((j == nColCount-1) && (m_nActiveRowIndex == indexUnit.second))
					{
						SolidBrush brushOverlay(colorOverlayRowTip);
						g.FillRectangle( &brushOverlay, rectUnit );
					}
				}

				if ((i==0) && (j!=nColCount-1))
				{
					wsprintfW(wszTitle, L"%d", indexUnit.first+1);
					g.DrawString(wszTitle, -1, &fontTitle, RectF((float)rectUnit.X, (float)rectUnit.Y+1.f, (float)rectUnit.Width, (float)rectUnit.Height), &stringFormat, &brushTitle);
				}
				if ((j==nColCount-1) && (i!=0))
				{
					m_ArrayRowTip[indexUnit.second] = rectUnit;
					wsprintfW(wszTitle, L"%C", L'A'+indexUnit.second);
					if (m_nActiveRowIndex == indexUnit.second)
					{
						g.DrawString(wszTitle, -1, &fontTitle, RectF((float)rectUnit.X, (float)rectUnit.Y+1.f, (float)rectUnit.Width, (float)rectUnit.Height), &stringFormat, &brushActiveRowText);
					}
					else
					{
						g.DrawString(wszTitle, -1, &fontTitle, RectF((float)rectUnit.X, (float)rectUnit.Y+1.f, (float)rectUnit.Width, (float)rectUnit.Height), &stringFormat, &brushTitle);
					}
				}
			}
			else
			{
				MapUnit::iterator iterFind = m_MapUnit.find(indexUnit);
				ASSERT(iterFind != m_MapUnit.end());
				iterFind->second = rectUnit;

				if (bIsInvalidRowIndex)
				{
					g.FillRectangle( &brushUnitBody, rectUnit );
				}
				else
				{
					Gdiplus::Font fontLegend(UIExt::CResourceManager::Instance()->GetSystemFontFamily(), 9, FontStyleBold );
					SolidBrush brushUnit(Color(180, 0, 0, 0));
					StringFormat stringFormat;
					stringFormat.SetAlignment(StringAlignmentCenter);
					stringFormat.SetLineAlignment(StringAlignmentCenter);
					RectF r(Gdiplus::REAL(rectUnit.X), Gdiplus::REAL(rectUnit.Y), Gdiplus::REAL(rectUnit.Width), Gdiplus::REAL(rectUnit.Height));

					switch(m_MapUnitType2D.at(indexUnit))
					{
					case UnitType2DGrab1:
						g.FillRectangle(m_pBrushGrab1, rectUnit);
						break;
					case UnitType2DGrab2:
						g.FillRectangle(m_pBrushGrab2, rectUnit);
						break;
					case UnitType2DOK:
						g.FillRectangle(m_pBrushOK, rectUnit);
						break;
					case UnitType2DNG:
						g.FillRectangle(m_pBrushNG, rectUnit);
						break;
					case UnitType2DNone:
						g.FillRectangle(&brushUnitBody, rectUnit);
						break;
					}
				}

				if (m_PressIndexUnit == indexUnit)
				{
					g.DrawRectangle( &penSelectUnit, rectUnit );
				}

				if (bIsInvalidRowIndex)
				{
					g.DrawLine( &Pen(Color(60,60,60)), rectUnit.GetLeft(), rectUnit.GetTop(), rectUnit.GetRight(), rectUnit.GetBottom() );
					g.DrawLine( &Pen(Color(60,60,60)), rectUnit.GetLeft(), rectUnit.GetBottom(), rectUnit.GetRight(), rectUnit.GetTop() );
				}
			}

			if (m_HoverIndexUnit == indexUnit)
			{
				SolidBrush brushOverlay(colorOverlayUnit);
				MapUnit::iterator iterFind = m_MapUnit.find(indexUnit);
				if (iterFind != m_MapUnit.end())
				{
					const Rect& rect = iterFind->second;
					g.FillRectangle( &brushOverlay, rect );
				}
			}

			if ( j==nColCount-1 )
				fPosX += fFirstStepX;
			else
				fPosX += fStepX;
		}
		if (i==0)
			fPosY += fFirstStepY;
		else
			fPosY += fStepY;
		fPosX = (float)rectStripBody.X;
	}
}

CString CStripInfoCtrl::GetLegendText( UnitType2D type )
{
	switch (type)
	{
	case UnitType2DOK:
		return _T("OK");
	case UnitType2DNG:
		return _T("NG");
	default:
		return _T("");
	}
	return _T("");
}

void CStripInfoCtrl::DrawLegend( Graphics& g, Rect& rectLegendBody )
{
	g.SetSmoothingMode( SmoothingModeAntiAlias );
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	Rect rectArrow = rectLegendBody;
	rectArrow.Height = 20;
	rectArrow.X = rectArrow.GetRight() - 60;
	rectArrow.Width = 60;

	rectArrow.Inflate( -15, 0 );

	Pen penArrow(Color(180, 255, 255, 255), 6.f);
	penArrow.SetStartCap( LineCapFlat );
	penArrow.SetEndCap( LineCapArrowAnchor );
	g.DrawLine( &penArrow, rectArrow.X, rectArrow.Y+rectArrow.Height/2, rectArrow.GetRight(), rectArrow.Y+rectArrow.Height/2 );

	int nItemHeight = 20;
	int nItemGapH = 2;
	int nItemGapW = 5;
	int nItemTextW = 30;

	RectF rectLegendItemF;
	rectLegendItemF.X = (float)rectLegendBody.X + 3.f;
	rectLegendItemF.Width = (float)nItemTextW;
	rectLegendItemF.Y = (float)rectLegendBody.GetBottom() - nItemHeight;
	rectLegendItemF.Height = (float) nItemHeight;
	rectLegendItemF.Inflate( -2, -2 );

	RectF rectColorF;
	rectColorF = rectLegendItemF;
	rectColorF.Width = rectLegendItemF.Height;

	Pen penBorder(Color(0,0,0));
	
	SolidBrush brushLegend(Color(180, 255, 255, 255));
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentNear );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	Gdiplus::Font fontLegend( UIExt::CResourceManager::Instance()->GetSystemFontFamily(), 9, FontStyleBold );
	CString strText;
	BSTR bstrText;

	// rectColorF.Y -= (float)nItemHeight;
	g.FillRectangle( m_pBrushOK, rectColorF );
	//g.DrawRectangle( &penBorder, rectColorF );
	rectLegendItemF.X += rectColorF.GetRight() + (float)nItemGapW;

	strText = GetLegendText(UnitType2DOK);
	bstrText = strText.AllocSysString();
	g.DrawString( bstrText, -1, &fontLegend, rectLegendItemF, &stringFormat, &brushLegend );
	::SysFreeString( bstrText );

	rectColorF.X = rectLegendItemF.GetRight() + (float)nItemGapW * 2;
	g.FillRectangle( m_pBrushNG, rectColorF );
	//g.DrawRectangle( &penBorder, rectColorF );
	rectLegendItemF.X = rectColorF.GetRight() + (float)nItemGapW;

	///strText = GetLegendText(UnitType2DNG);
	strText = GetLegendText(UnitType2DNG);
	bstrText = strText.AllocSysString();
	g.DrawString( bstrText, -1, &fontLegend, rectLegendItemF, &stringFormat, &brushLegend );
	::SysFreeString( bstrText );
}

void CStripInfoCtrl::DrawStripId( Graphics& g, RectF& rectStripIdF )
{
	FontFamily* fontFamily = UIExt::CResourceManager::Instance()->GetSystemFontFamily();
	Gdiplus::Font fontStrip(fontFamily, 8);
	CString strStripId = _T("PANNEL ID: ");
	strStripId += m_strStripId;
	BSTR bstrStripId = strStripId.AllocSysString();
	SolidBrush brushStripId(Color(210, 255, 255, 255));
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	g.DrawString(bstrStripId, -1, &fontStrip, rectStripIdF, &stringFormat, &brushStripId);
	::SysFreeString(bstrStripId);
}

void CStripInfoCtrl::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	DWORD dwBody = RGB(60, 60, 60);
	Color colorBody;
	colorBody.SetFromCOLORREF(dwBody);

	g.FillRectangle( &SolidBrush(colorBody), rect );
}

void CStripInfoCtrl::PreSubclassWindow()
{
	ModifyStyle( 0, SS_NOTIFY );

	CGdipStatic::PreSubclassWindow();
}

void CStripInfoCtrl::EnableWindow( BOOL bEnable )
{
	if (bEnable)
	{
		ModifyStyle( 0, SS_NOTIFY );
	}
	else
	{
		ModifyStyle( SS_NOTIFY, 0 );
	}
}

BOOL CStripInfoCtrl::IsWindowEnabled() const
{
	return (GetStyle() & SS_NOTIFY) == SS_NOTIFY ? TRUE : FALSE;
}

BOOL CStripInfoCtrl::IsSelectUnit() const
{
	return ((m_PressIndexUnit.first >= 0) && (m_PressIndexUnit.second >= 0)) ? TRUE : FALSE;
}

void CStripInfoCtrl::GetSelectedUnitIndex( int& nColumn, int& nRow ) const
{
	nColumn = m_PressIndexUnit.first;
	nRow = m_PressIndexUnit.second;
}

void CStripInfoCtrl::Reset()
{
	m_HoverIndexUnit.first = m_HoverIndexUnit.second = -1;
	m_PressIndexUnit.first = m_PressIndexUnit.second = -1;

	for (MapUnitType2D::iterator iter = m_MapUnitType2D.begin(); iter != m_MapUnitType2D.end(); ++iter)
	{
		iter->second = UnitType2DNone;
	}

	m_ArrayInvalidRowIndex.clear();

	ResetSelectedItem();
	if (GetSafeHwnd())
		Invalidate();
}

void CStripInfoCtrl::SetInvalidRowIndex(int nRowIndex)
{
	ArrayRowIndex::iterator iterFind = std::find(m_ArrayInvalidRowIndex.begin(), m_ArrayInvalidRowIndex.end(), nRowIndex);
	if(iterFind == m_ArrayInvalidRowIndex.end())
		m_ArrayInvalidRowIndex.push_back(nRowIndex);
}

BOOL CStripInfoCtrl::IsInvalidRowIndex(int nRowIndex)
{
	ArrayRowIndex::iterator iterFind = std::find(m_ArrayInvalidRowIndex.begin(), m_ArrayInvalidRowIndex.end(), nRowIndex);
	if(iterFind == m_ArrayInvalidRowIndex.end())
		return FALSE;
	return TRUE;
}

void CStripInfoCtrl::SetUnitType2D(int nColumn, int nRow, UnitType2D type)
{
	UnitIndex index = std::make_pair(nColumn, nRow);
	MapUnitType2D::iterator iterFindType = m_MapUnitType2D.find(index);

	if(iterFindType == m_MapUnitType2D.end())
		return;

	UnitType2D& unitType = iterFindType->second;
	unitType = type;

	if(GetSafeHwnd())
	{
		MapUnit::iterator iterFindUnit = m_MapUnit.find(index);
		if(iterFindUnit != m_MapUnit.end())
		{
			Gdiplus::Rect& rect = iterFindUnit->second;
			CRect rcUnit;
			rcUnit.left         = rect.GetLeft();
			rcUnit.top          = rect.GetTop();
			rcUnit.right        = rect.GetRight();
			rcUnit.bottom       = rect.GetBottom();
			rcUnit.InflateRect(5, 5, 5, 5);
			InvalidateRect(rcUnit);
		}
	}
}

void CStripInfoCtrl::SetActiveRowIndex( int nRowIndex )
{
	if (nRowIndex < 0 || nRowIndex >= m_nRowCount)
		return;
	if (m_nActiveRowIndex == nRowIndex)
		return;

	int nOldActiveRowIndex = m_nActiveRowIndex;
	m_nActiveRowIndex = nRowIndex;
	if (nOldActiveRowIndex != m_nActiveRowIndex)
	{
		Rect& rect = m_ArrayRowTip[nOldActiveRowIndex];
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
	}
	{
		Rect& rect = m_ArrayRowTip[m_nActiveRowIndex];
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
	}
}

void CStripInfoCtrl::ResetSelectedItem()
{
	UnitIndex oldPressIndexUnit = m_PressIndexUnit;
	m_PressIndexUnit.first = m_PressIndexUnit.second = -1;

	if ((oldPressIndexUnit.first >= 0) && (oldPressIndexUnit.second >= 0))
	{
		Rect& rect = m_MapUnit.at(oldPressIndexUnit);
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
	}

	int nOldActiveRowIndex = m_nActiveRowIndex;
	m_nActiveRowIndex = 0;
	if (nOldActiveRowIndex != m_nActiveRowIndex)
	{
		Rect& rect = m_ArrayRowTip[nOldActiveRowIndex];
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
	}
	{
		Rect& rect = m_ArrayRowTip[m_nActiveRowIndex];
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
	}
}

BOOL CStripInfoCtrl::HitTest( CPoint point, UnitIndex& IndexUnit )
{
	IndexUnit.first = IndexUnit.second = -1;
	for (MapUnit::iterator iter=m_MapUnit.begin() ; iter != m_MapUnit.end() ; ++iter)
	{
		Rect& rect = iter->second;
		if (rect.Contains(point.x, point.y))
		{
			const std::pair<int, int>& key = iter->first;
			//TRACE( _T("UNIT: %d, %d\r\n"), key.first, key.second );
			IndexUnit.first = key.first;
			IndexUnit.second = key.second;
			return TRUE;
		}
	}

	return FALSE;
}

void CStripInfoCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	UnitIndex HoverUnit;
	if (HitTest(point, HoverUnit))
	{
		if (::GetCapture() != m_hWnd)
		{
			::SetCapture(m_hWnd);
		}
	}
	else
	{
		if (!m_bIsButtonDown)
		{
			if (::GetCapture() == m_hWnd)
			{
				::ReleaseCapture();
			}
		}
	}

	if (m_HoverIndexUnit != HoverUnit)
	{
		UnitIndex oldUnit = m_HoverIndexUnit;
		m_HoverIndexUnit = HoverUnit;

		if ((oldUnit.first >= 0) && (oldUnit.second >= 0))
		{
			Rect& rect = m_MapUnit.at(oldUnit);
			CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
			rc.InflateRect(2, 2, 2, 2);
			InvalidateRect( rc );
		}

		if ((m_HoverIndexUnit.first >= 0) && (m_HoverIndexUnit.second >= 0))
		{
			Rect& rect = m_MapUnit.at(m_HoverIndexUnit);
			CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
			rc.InflateRect(2, 2, 2, 2);
			InvalidateRect( rc );
		}
	}


	CGdipStatic::OnMouseMove(nFlags, point);
}

void CStripInfoCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	UnitIndex PressIndexUnit;

	if (HitTest(point, PressIndexUnit))
	{
		if ( ::GetCapture() != m_hWnd )
		{
			::SetCapture( m_hWnd );
		}
	}
	else
	{
		CGdipStatic::OnLButtonDown(nFlags, point);
		return;
	}

	if (PressIndexUnit != m_PressIndexUnit)
	{
		UnitIndex oldPressIndexUnit = m_PressIndexUnit;
		m_PressIndexUnit = PressIndexUnit;

		if ((oldPressIndexUnit.first >= 0) && (oldPressIndexUnit.second >= 0))
		{
			Rect& rect = m_MapUnit.at(oldPressIndexUnit);
			CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
			rc.InflateRect(2, 2, 2, 2);
			InvalidateRect( rc );
		}

		if ((m_PressIndexUnit.first >= 0) && (m_PressIndexUnit.second >= 0))
		{
			Rect& rect = m_MapUnit.at(m_PressIndexUnit);
			CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
			rc.InflateRect(2, 2, 2, 2);

			if (m_PressIndexUnit.second != m_nActiveRowIndex)
			{
				int nOldActiveRowIndex = m_nActiveRowIndex;
				m_nActiveRowIndex = m_PressIndexUnit.second;
				{
					Rect& rect = m_ArrayRowTip[nOldActiveRowIndex];
					CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
					rc.InflateRect(2, 2, 2, 2);
					InvalidateRect( rc );
				}
				{
					Rect& rect = m_ArrayRowTip[m_nActiveRowIndex];
					CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
					rc.InflateRect(2, 2, 2, 2);
					InvalidateRect( rc );
				}

			}
			InvalidateRect( rc );
			::SendMessage( GetParent()->GetSafeHwnd(), UIExt::WM_STRIPINFO_UNIT_SELECTED, (WPARAM)MAKEWPARAM(m_PressIndexUnit.first, m_PressIndexUnit.second), 0 );
		}
	}

	m_bIsButtonDown = TRUE;

	CGdipStatic::OnLButtonDown(nFlags, point);
}

void CStripInfoCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( ::GetCapture() == m_hWnd )
	{
		::ReleaseCapture();
	}

	m_HoverIndexUnit.first = m_HoverIndexUnit.second = -1;

	m_bIsButtonDown = FALSE;

	CGdipStatic::OnLButtonUp(nFlags, point);
}
