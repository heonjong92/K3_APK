// StripInfoCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ObjectInfoCtrlEx.h"
#include "ResourceManager.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CStripInfoCtrl

const UINT UIExt::WM_OBJECTINFO_DOT_SELECTED_EX			= ::RegisterWindowMessage( _T("UIEXT_OBJECTINFO_DOT_SELECTED"		) );
const UINT UIExt::WM_OBJECTINFO_UNIT_SELECTED_EX		= ::RegisterWindowMessage( _T("UIEXT_OBJECTINFO_UNIT_SELECTED"		) );
const UINT UIExt::WM_OBJECTINFO_UNIT_STATE_CHANGE_EX	= ::RegisterWindowMessage( _T("UIEXT_OBJECTINFO_UNIT_STATE_CHANGE"	) );
const UINT UIExt::WM_OBJECTINFO_UNIT_UNSELECTED_EX		= ::RegisterWindowMessage( _T("UIEXT_OBJECTINFO_UNIT_UNSELECTED"	) );

using namespace UIExt;

IMPLEMENT_DYNAMIC(CObjectInfoCtrlEx, CGdipStatic)

CObjectInfoCtrlEx::CObjectInfoCtrlEx()
{
	m_bUseDot			= TRUE;
	m_nColumnCount		= 14;
	m_nRowCount			= 4;
	m_strStripId		= _T("");
	m_bIs3DMode			= FALSE;
	m_nActiveRowIndex	= 0;
	m_objDirection		= Direction_L2R;

	m_ArrayDot.resize(m_nColumnCount);
	for (int i=0 ; i<m_nColumnCount ; i++)
		m_ArrayDot[i] = Rect(0,0,0,0);
	
	m_ArrayRowTip.resize(m_nRowCount);
	for (int j=0 ; j<m_nRowCount ; j++)
	{
		for (int i=0 ; i<m_nColumnCount ; i++)
		{
			m_MapUnit.insert( MapUnit::value_type( std::make_pair(i,j), Rect(0,0,0,0) ) );
			m_MapUnitTypeResult.insert( MapUnitTypeResult::value_type( std::make_pair(i,j), UnitType_None ) );
			//m_MapUnitType3D.insert( MapUnitType3D::value_type( std::make_pair(i,j), UnitType3DNone ) );
		}
		m_ArrayRowTip[j] = Rect(0,0,0,0);
	}

	m_nHoverIndexDot		= -1;
	m_HoverIndexUnit.first	= m_HoverIndexUnit.second = -1;
	m_nPressIndexDot		= -1;
	m_PressIndexUnit.first = m_PressIndexUnit.second = -1;

	m_bIsButtonDown = FALSE;

	m_pBrush_None = ::new SolidBrush(Color(150, 160, 150));
	m_pBrush_OK = ::new SolidBrush(Color(51, 255, 51));
	m_pBrushEmpty = ::new SolidBrush(Color(100, 100, 255));
	m_pBrushError = ::new SolidBrush(Color(240, 20, 70));

	m_pBrushGrab1 = ::new SolidBrush(Color(160, 170, 160));
	m_pBrushGrab2 = ::new SolidBrush(Color(180, 190, 180));
	m_pBrushGrab3 = ::new SolidBrush(Color(200, 210, 200));
	m_pBrushGrab4 = ::new SolidBrush(Color(220, 230, 220));

	m_pBrushScanning = ::new SolidBrush(Color(130, 255, 130));
	m_pBrushMeasured = ::new SolidBrush(Color(255, 130, 130));

	m_pToolTip = new CToolTipCtrl;
	//m_pToolTip->Create( this, TTS_ALWAYSTIP | TTS_NOPREFIX );
	m_pToolTip->Create(this, TTS_ALWAYSTIP | TTS_BALLOON);  

	m_pToolTip->SetDelayTime(10000);
	//m_pToolTip->Activate(TRUE);
	m_nRowCountOneShot = 5;
	m_nColumnCountOneShot = 6;
}

CObjectInfoCtrlEx::~CObjectInfoCtrlEx()
{
	::delete m_pBrush_None;		
	::delete m_pBrush_OK;	
	::delete m_pBrushEmpty;	
	::delete m_pBrushError;

	::delete m_pBrushGrab1;
	::delete m_pBrushGrab2;
	::delete m_pBrushGrab3;
	::delete m_pBrushGrab4;

	::delete m_pBrushScanning;
	::delete m_pBrushMeasured;
}


BEGIN_MESSAGE_MAP(CObjectInfoCtrlEx, CGdipStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CObjectInfoCtrlEx::SetIdText( LPCTSTR lpszId )
{
	m_strStripId = lpszId;

	if (GetSafeHwnd())
		Invalidate();
}

void CObjectInfoCtrlEx::SetIndexDataOneshot( int nRowCount, int nColumnCount )
{
	if ((m_nRowCountOneShot == nRowCount) && (m_nColumnCountOneShot == nColumnCount))
		return;

	m_nRowCountOneShot = nRowCount;
	m_nColumnCountOneShot = nColumnCount;
}

void CObjectInfoCtrlEx::SetIndexData( int nRowCount, int nColumnCount )
{
	if ((m_nColumnCount == nColumnCount) && (m_nRowCount == nRowCount))
		return;

	m_nColumnCount	= nColumnCount;
	m_nRowCount		= nRowCount;
	m_nActiveRowIndex = 0;

	if (m_nColumnCount	<= 0) m_nColumnCount	= 1;
	if (m_nRowCount		<= 0) m_nRowCount		= 1;
	if (m_nColumnCount	> MAX_COL_COUNT) m_nColumnCount = MAX_COL_COUNT;
	if (m_nRowCount		> MAX_ROW_COUNT) m_nRowCount	= MAX_ROW_COUNT;

	m_ArrayDot.			clear();
	m_MapUnit.			clear();
	m_MapUnitTypeResult.clear();
	m_ArrayRowTip.		clear();
	m_DotPosition.		clear();
	m_MapGroupIndex.	clear();

	m_ArrayDot.resize(m_nColumnCount);
	for (int i=0 ; i<m_nColumnCount ; i++)
		m_ArrayDot[i] = Rect(0,0,0,0);
	
	m_ArrayRowTip.resize(m_nRowCount);
	for (int j=0 ; j<m_nRowCount ; j++)
	{
		for (int i=0 ; i<m_nColumnCount ; i++)
		{
			m_MapUnit.insert( MapUnit::value_type( std::make_pair(i,j), Rect(0,0,0,0) ) );
			m_MapUnitTypeResult.insert( MapUnitTypeResult::value_type( std::make_pair(i,j), UnitType_None ) );
			m_MapGroupIndex.insert( MapGroupIndex::value_type( std::make_pair(i,j), -1 ) );
		}

		m_ArrayRowTip[j] = Rect(0,0,0,0);
	}

	if (GetSafeHwnd())
		Invalidate();
}

void CObjectInfoCtrlEx::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.SetSmoothingMode( SmoothingModeAntiAlias );
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	const int nMarginXY			= 0;
	const int nStripIdHeight	= 20;
	//const int nLegendWidth	= 60;
	const int nLegendWidth		= 60;

	DWORD dwBgColor = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();

	Color colorBody;
	colorBody.SetFromCOLORREF(dwBgColor);
	g.FillRectangle( &SolidBrush(colorBody), rect.X, rect.Y, rect.Width+1, nStripIdHeight+nMarginXY );

	rect.X		+= nMarginXY;
	rect.Width	-= nMarginXY*2+1;
	rect.Y		+= nMarginXY;
	rect.Height -= nMarginXY*2+1;

	Rect rectLegendBody;
	rectLegendBody.X		= rect.GetRight() - nLegendWidth;
	rectLegendBody.Y		= rect.GetTop();
	rectLegendBody.Width	= nLegendWidth;
	rectLegendBody.Height	= rect.Height;
	//rect.DeflateRect(0, nStripIdHeight, nLegendWidth, 0);
	rect.Width	-= nLegendWidth;
	rect.Y		+= nStripIdHeight; 
	rect.Height -= nStripIdHeight;

	RectF rectStripIdF;
	rectStripIdF.X		= (float)rect.X + 5.f;
	rectStripIdF.Y		= (float)rect.Y-nStripIdHeight;
	rectStripIdF.Width	= (float)rect.Width;
	rectStripIdF.Height = (float)nStripIdHeight;

	DrawObjectId( g, rectStripIdF  );

	DrawLegend(  g, rectLegendBody );

	Rect rectStripBody;
	rectStripBody.X			= rect.X;
	rectStripBody.Y			= rect.Y;
	rectStripBody.Width		= rect.Width;
	rectStripBody.Height	= rect.Height-1;
	rectStripBody.Inflate( -2, -2 );
	DrawStrip( g, rectStripBody );
}

void CObjectInfoCtrlEx::DrawDotCircles( Graphics& g, SolidBrush* pBrush, SolidBrush* pBrushDot, Rect rectDot, int nColumnIndex )
{
	ASSERT( m_nRowCount > 0 );

	rectDot.Inflate(-1, 0);

	rectDot.X += 1;
	rectDot.Width -= (m_nRowCount-1);

	int nDW = rectDot.Width / m_nRowCount;
	Rect rectD	= rectDot;
	rectD.Width = nDW;
	rectD.Y		+= (rectDot.Height - nDW) / 2;
	rectD.Height = nDW;

	UnitIndex index;
	DotPosition::iterator iterFindDot;
	for (int i=0 ; i<m_nRowCount ; i++)
	{
		index = std::make_pair(m_nColumnCount-nColumnIndex-1, m_nRowCount-i-1);
		iterFindDot = std::find(m_DotPosition.begin(), m_DotPosition.end(), index);

		if (iterFindDot == m_DotPosition.end())
			g.FillEllipse( pBrush, rectD );
		else
			g.FillEllipse( pBrushDot, rectD );

		rectD.X += nDW + 1;
	}
}

void CObjectInfoCtrlEx::DrawDotCircles( Graphics& g, Pen* pPen, Rect rectDot )
{
	ASSERT( m_nRowCount > 0 );

	rectDot.Inflate(-1, 0);

	rectDot.X += 1;
	rectDot.Width -= (m_nRowCount-1);

	int nDW = rectDot.Width / m_nRowCount;
	Rect rectD		= rectDot;
	rectD.Width		= nDW;
	rectD.Y			+= (rectDot.Height - nDW) / 2;
	rectD.Height	= nDW;

	for (int i=0 ; i<m_nRowCount ; i++)
	{
		g.DrawEllipse( pPen, rectD );
		rectD.X += nDW + 1;
	}
}

void CObjectInfoCtrlEx::DrawStrip( Graphics& g, Rect& rectStripBody )
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

	int nRowCount = m_nRowCount+1;			//Y
	int nColCount = m_nColumnCount+1;		//X

	float fStepX = (float)rectStripBody.Width / nColCount;		//1개의 width
	float fFirstStepX = fStepX * 1.0f;// * 0.5f
	fStepX = (rectStripBody.Width - fFirstStepX) / (nColCount-1);

	m_bUseDot = FALSE;
	if (m_bUseDot)
	{
		float fStepT = (float)rectStripBody.Height / (nRowCount+1) * 0.7f;
		Rect rectDotBody;
		rectDotBody = rectStripBody;
		rectStripBody.Y += (int)fStepT;
		rectStripBody.Height -= (int)fStepT;
		rectDotBody.Width -= (int)fFirstStepX + 2;
		rectDotBody.Height = (int)fStepT-2;
		rectDotBody.X += 1;
		rectDotBody.Y += 1;

		int nDotIndex;

		float fPosX = (float)rectDotBody.X;
		Rect rectDot;
		rectDot.Y = rectDotBody.Y;
		rectDot.Height = (int)(fStepT+0.5f)-4;
		for ( int i=0 ; i<nColCount-1 ; i++ )
		{
			rectDot.X = (int)(fPosX+0.5f);
			rectDot.Width = (int)(fPosX+fStepX+0.5f) - rectDot.X - 1;

			nDotIndex = nColCount-1-1-i;
			Rect& rect = m_ArrayDot.at(nDotIndex);
			rect = rectDot;

			DrawDotCircles( g, &brushUnitBody, m_pBrush_None, rectDot, i );

			if (m_nHoverIndexDot == nDotIndex)
			{
				SolidBrush brushOverlay(colorOverlayDot);
				DrawDotCircles( g, &brushOverlay, &brushOverlay, rectDot, i );
			}

			if (m_nPressIndexDot == nDotIndex)
			{
				//g.DrawEllipse( &penSelectUnit, rectDot );
				DrawDotCircles( g, &penSelectUnit, rectDot );
			}
			
			fPosX += fStepX;
		}
	}

	float fStepY = (float)rectStripBody.Height / nRowCount;
	float fFirstStepY = fStepY * 0.9f;
	fStepY = (rectStripBody.Height - fFirstStepY) / (nRowCount-1);

	float fPosX = (float)rectStripBody.X;
	float fPosY = (float)rectStripBody.Y;

	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	Gdiplus::FontFamily fontFamily(L"Arial");
	Gdiplus::Font fontTitle(&fontFamily/*UIExt::CResourceManager::Instance()->GetSystemFontFamily()*/, fFirstStepY * 0.25f, FontStyleBold);

	WCHAR wszTitle[10];
	Rect rectUnit;
	UnitIndex indexUnit;
	USES_CONVERSION;

	int nRemainX = (nColCount-1) % m_nColumnCountOneShot;
	if (nRemainX == 0)
		nRemainX = m_nColumnCountOneShot;

	int nRemainY = (nRowCount - 1) % m_nRowCountOneShot;
	if (nRemainY == 0)
		nRemainY = m_nRowCountOneShot;

	int nShiftX = m_nColumnCountOneShot - nRemainX;
	int nShiftY = m_nRowCountOneShot - nRemainY;

	for ( int i=0 ; i<nRowCount ; i++ )
	{
		BOOL bIsInvalidRowIndex = (!m_bIs3DMode && IsInvalidRowIndex(i-1)) ? TRUE : FALSE;
		for ( int j=0 ; j<nColCount ; j++ )
		{
			rectUnit.X = (int)(fPosX+0.5f) + 1;
			rectUnit.Y = (int)(fPosY+0.5f) + 1;

			// X축
			if (j == nColCount - 1)
			{
				rectUnit.Width = (int)(fPosX + fFirstStepX + 0.5f) + 1	- rectUnit.X - 1;
			}
			else
			{
				int nMappedX = (j + nShiftX) % m_nColumnCountOneShot;

				if (nMappedX == m_nColumnCountOneShot - 1)
					rectUnit.Width = (int)(fPosX + fStepX + 0.5f) + 1 - rectUnit.X - 1 - 1;
				else
					rectUnit.Width = (int)(fPosX + fStepX + 0.5f) + 1 - rectUnit.X - 1;
			}

			// Y축
			if (i==0)
				rectUnit.Height = (int)(fPosY+fFirstStepY+0.5f) + 1 - rectUnit.Y - 1;
			else
			{
				int nMappedY = (i -1 + nShiftY) % m_nRowCountOneShot;

				if (nMappedY == m_nRowCountOneShot - 1)
					rectUnit.Height = (int)(fPosY + fStepY + 0.5f) + 1 - rectUnit.Y - 1 - 1;
				else
					rectUnit.Height = (int)(fPosY + fStepY + 0.5f) + 1 - rectUnit.Y - 1;
			}

			if( m_objDirection == Direction_L2R)
				indexUnit.first = j;
			else
				indexUnit.first = nColCount-1-j-1;

			if (m_objDirection == Direction_L2R)
				indexUnit.second = i - 1;
			else
				indexUnit.second = m_nRowCount - i;

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
					//wsprintfW(wszTitle, L"%C", L'A'+indexUnit.second);
					wsprintfW(wszTitle, L"%d", indexUnit.second+1);
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
					switch (m_MapUnitTypeResult.at(indexUnit))
					{
					case UnitType_None:
						g.FillRectangle( m_pBrush_None,			rectUnit );
						break;

					case UnitType_OK:
						g.FillRectangle( m_pBrush_OK,			rectUnit );
						break;

					case UnitType_Empty:
						g.FillRectangle( m_pBrushEmpty,		rectUnit );
						break;

					case UnitType_Error:
						g.FillRectangle( m_pBrushError,		rectUnit );
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

CString CObjectInfoCtrlEx::GetLegendText( UnitTypeResult type )
{
	switch (type)
	{
	case UnitType_None:
		return _T("None");

	case UnitType_OK:
		return _T("OK");

	case UnitType_Empty:
		return _T("Empty");

	default:
		return _T("Error");
	}
}

void CObjectInfoCtrlEx::DrawLegend( Graphics& g, Rect& rectLegendBody )
{
	g.SetSmoothingMode( SmoothingModeAntiAlias );
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	Rect rectArrow = rectLegendBody;
	rectArrow.Height = 20;

	rectArrow.Inflate( -15, 0 );

// 	Pen penArrow(Color(180, 255, 255, 255), 6.f);
// 	penArrow.SetStartCap( LineCapFlat );
// 	penArrow.SetEndCap( LineCapArrowAnchor );
// 	g.DrawLine( &penArrow, rectArrow.X, rectArrow.Y+rectArrow.Height/2, rectArrow.GetRight(), rectArrow.Y+rectArrow.Height/2 );

	int nItemHeight = rectLegendBody.Height / 8 -1;	//10 -> 8

	RectF rectLegendItemF;
	rectLegendItemF.X = (float)rectLegendBody.X + 15.f;
	rectLegendItemF.Width = (float)rectLegendBody.Width;
	rectLegendItemF.Y = (float)rectLegendBody.GetBottom() - nItemHeight*1.2f;
	rectLegendItemF.Height = (float)nItemHeight/2.5f;
	rectLegendItemF.Inflate( -2, -2 );

	RectF rectColorF;
	rectColorF = rectLegendItemF;
	rectColorF.Width = 30;//rectLegendItemF.Height;
	rectColorF.X = (float)rectLegendBody.X +(rectLegendBody.Width - rectColorF.Width)/2.0f -1;

	Pen penBorder(Color(0,0,0));
	
	SolidBrush brushLegend(Color(180, 255, 255, 255));
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	Gdiplus::Font fontLegend( UIExt::CResourceManager::Instance()->GetSystemFontFamily(), 9, FontStyleBold );
	CString strText;
	BSTR bstrText;


	// ----- UnitType_None -----
	g.FillRectangle( m_pBrush_None, rectColorF );
	rectLegendItemF.X -= rectColorF.Width / 2;
	rectLegendItemF.Y += rectColorF.Height;

	strText		= GetLegendText(UnitType_None);
	bstrText	= strText.AllocSysString();
	g.DrawString( bstrText, -1, &fontLegend, rectLegendItemF, &stringFormat, &brushLegend );
	::SysFreeString( bstrText );

	// ----- UnitType_Error -----
	rectColorF.Y -= (float)nItemHeight;
	g.FillRectangle( m_pBrushError, rectColorF );
	rectLegendItemF.Y -= (float)nItemHeight;

	strText = GetLegendText(UnitType_Error);
	bstrText = strText.AllocSysString();
	g.DrawString( bstrText, -1, &fontLegend, rectLegendItemF, &stringFormat, &brushLegend );
	::SysFreeString( bstrText );

	// ----- UnitType_Empty -----
	rectColorF.Y -= (float)nItemHeight;
	g.FillRectangle( m_pBrushEmpty, rectColorF );
	rectLegendItemF.Y -= (float)nItemHeight;

	strText = GetLegendText(UnitType_Empty);
	bstrText = strText.AllocSysString();
	g.DrawString( bstrText, -1, &fontLegend, rectLegendItemF, &stringFormat, &brushLegend );
	::SysFreeString( bstrText );

	// ----- UnitType_OK -----
	rectColorF.Y -= (float)nItemHeight;
	g.FillRectangle( m_pBrush_OK, rectColorF );
	rectLegendItemF.Y -= (float)nItemHeight;

	strText = GetLegendText(UnitType_OK);
	bstrText = strText.AllocSysString();
	g.DrawString( bstrText, -1, &fontLegend, rectLegendItemF, &stringFormat, &brushLegend );
	::SysFreeString( bstrText );
}

void CObjectInfoCtrlEx::DrawObjectId( Graphics& g, RectF& rectObjectIdF )
{
	FontFamily* fontFamily = UIExt::CResourceManager::Instance()->GetSystemFontFamily();
	Gdiplus::Font fontObject(fontFamily, 8);
	CString strObjectId = _T("");
//	CString strObjectId = _T("Object ID: ");
	strObjectId += m_strStripId;
	BSTR bstrObjectId = strObjectId.AllocSysString();
	SolidBrush brushObjectId(Color(210, 255, 255, 255));
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	g.DrawString(bstrObjectId, -1, &fontObject, rectObjectIdF, &stringFormat, &brushObjectId);
	::SysFreeString(bstrObjectId);
}

void CObjectInfoCtrlEx::OnGdipEraseBkgnd( Graphics& g, Rect rect )
{
	DWORD dwBody = RGB(60, 60, 60);
	Color colorBody;
	colorBody.SetFromCOLORREF(dwBody);

	g.FillRectangle( &SolidBrush(colorBody), rect );
}

void CObjectInfoCtrlEx::PreSubclassWindow()
{
	ModifyStyle( 0, SS_NOTIFY );

	CGdipStatic::PreSubclassWindow();
}

void CObjectInfoCtrlEx::EnableWindow( BOOL bEnable )
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

BOOL CObjectInfoCtrlEx::IsWindowEnabled() const
{
	return (GetStyle() & SS_NOTIFY) == SS_NOTIFY ? TRUE : FALSE;
}

BOOL CObjectInfoCtrlEx::IsSelectDot() const
{
	return m_nPressIndexDot >= 0 ? TRUE : FALSE;
}
BOOL CObjectInfoCtrlEx::IsSelectUnit() const
{
	return ((m_PressIndexUnit.first >= 0) && (m_PressIndexUnit.second >= 0)) ? TRUE : FALSE;
}

int CObjectInfoCtrlEx::GetSelectedDotIndex() const
{
	return m_nPressIndexDot;
}

void CObjectInfoCtrlEx::GetSelectedUnitIndex( int& nColumn, int& nRow ) const
{
	nColumn = m_PressIndexUnit.first;
	nRow = m_PressIndexUnit.second;
}

void CObjectInfoCtrlEx::Reset()
{
	//m_DotPosition.clear();

	for (MapUnitTypeResult::iterator iter = m_MapUnitTypeResult.begin(); iter != m_MapUnitTypeResult.end(); ++iter)
	{
		iter->second = UnitType_None;
	}

	m_ArrayInvalidRowIndex.clear();

	ResetSelectedItem();
	if (GetSafeHwnd())
		Invalidate();
}

void CObjectInfoCtrlEx::SetInvalidRowIndex(int nRowIndex )
{
	ArrayRowIndex::iterator iterFind = std::find(m_ArrayInvalidRowIndex.begin(), m_ArrayInvalidRowIndex.end(), nRowIndex);
	if (iterFind == m_ArrayInvalidRowIndex.end())
		m_ArrayInvalidRowIndex.push_back(nRowIndex);
}

BOOL CObjectInfoCtrlEx::IsInvalidRowIndex(int nRowIndex)
{
	ArrayRowIndex::iterator iterFind = std::find(m_ArrayInvalidRowIndex.begin(), m_ArrayInvalidRowIndex.end(), nRowIndex);
	if (iterFind == m_ArrayInvalidRowIndex.end())
		return FALSE;
	return TRUE;
}

void CObjectInfoCtrlEx::SetUnitTypeResult( int nColumn, int nRow, UnitTypeResult type )
{
	UnitIndex index = std::make_pair(nColumn, nRow);
	MapUnitTypeResult::iterator iterFindType = m_MapUnitTypeResult.find( index );
	if (iterFindType == m_MapUnitTypeResult.end())
		return;

	UnitTypeResult& unitType = iterFindType->second;
	unitType = type;

	if (GetSafeHwnd())
	{
		MapUnit::iterator iterFindUnit = m_MapUnit.find( index );
		if (iterFindUnit != m_MapUnit.end())
		{
			Gdiplus::Rect& rect = iterFindUnit->second;
			CRect rcUnit;
			rcUnit.left = rect.GetLeft();
			rcUnit.top = rect.GetTop();
			rcUnit.right = rect.GetRight();
			rcUnit.bottom = rect.GetBottom();
			rcUnit.InflateRect( 5, 5, 5, 5 );
			InvalidateRect( rcUnit );
		}
	}
}

void CObjectInfoCtrlEx::SetUnitTypeResult( int nColumn, int nRow, UnitTypeResult type,int nGroupIndex )
{
	UnitIndex index = std::make_pair(nColumn, nRow);

	MapUnitTypeResult::iterator iterFindType = m_MapUnitTypeResult.find( index );
	if (iterFindType == m_MapUnitTypeResult.end())
		return;

	UnitTypeResult& unitType = iterFindType->second;
	unitType = type;


	MapGroupIndex::iterator iterFindIndex = m_MapGroupIndex.find( index );
	if (iterFindIndex == m_MapGroupIndex.end())
		return;

	int& nIndex = iterFindIndex->second;
	nIndex = nGroupIndex;


	if (GetSafeHwnd())
	{
		MapUnit::iterator iterFindUnit = m_MapUnit.find( index );
		if (iterFindUnit != m_MapUnit.end())
		{
			Gdiplus::Rect& rect = iterFindUnit->second;
			CRect rcUnit;
			rcUnit.left = rect.GetLeft();
			rcUnit.top = rect.GetTop();
			rcUnit.right = rect.GetRight();
			rcUnit.bottom = rect.GetBottom();
			rcUnit.InflateRect( 5, 5, 5, 5 );
			InvalidateRect( rcUnit );
		}
	}
}

int CObjectInfoCtrlEx::GetGroupIndex( int nCol, int nRow )
{
	UnitIndex index = std::make_pair(nCol, nRow);

	MapGroupIndex::iterator iterFindIndex = m_MapGroupIndex.find( index );
	if (iterFindIndex == m_MapGroupIndex.end())
		return -1;

	int nIndex = iterFindIndex->second;
	
	return nIndex;
}


void CObjectInfoCtrlEx::SetActiveRowIndex( int nRowIndex )
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


void CObjectInfoCtrlEx::ResetSelectedItem()
{
	int nOldPressIndexDot = m_nPressIndexDot;
	UnitIndex oldPressIndexUnit = m_PressIndexUnit;
	m_nPressIndexDot = -1;
	m_PressIndexUnit.first = m_PressIndexUnit.second = -1;

	if (nOldPressIndexDot >= 0)
	{
		Rect& rect = m_ArrayDot.at(nOldPressIndexDot);
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
	}
	else if ((oldPressIndexUnit.first >= 0) && (oldPressIndexUnit.second >= 0))
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

BOOL CObjectInfoCtrlEx::HitTest( CPoint point, int& nIndexDot, UnitIndex& IndexUnit )
{
	nIndexDot = -1;
	IndexUnit.first = IndexUnit.second = -1;
	for (int i=0 ; i<(int)m_ArrayDot.size() ; i++)
	{
		Rect& rect = m_ArrayDot.at(i);
		if (rect.Contains(point.x, point.y))
		{
			nIndexDot = i;
			return TRUE;
		}
	}

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

void CObjectInfoCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	return;

	UnitIndex HoverUnit;
	int nHoverDot;
	if (HitTest(point, nHoverDot, HoverUnit))
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

	if (!m_bIs3DMode && IsInvalidRowIndex(HoverUnit.second))
	{
		return;
	}

	if ((m_nHoverIndexDot != nHoverDot) || 
		(m_HoverIndexUnit != HoverUnit))
	{
		UnitIndex oldUnit = m_HoverIndexUnit;
		int nOldDot = m_nHoverIndexDot;
		if (!m_bIs3DMode)
			m_nHoverIndexDot = nHoverDot;
		m_HoverIndexUnit = HoverUnit;
		if (nOldDot >= 0)
		{
			if (!m_bIs3DMode)
			{
				Rect& rect = m_ArrayDot.at(nOldDot);
				CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
				rc.InflateRect(2, 2, 2, 2);
				InvalidateRect( rc );
			}
		}
		else if ((oldUnit.first >= 0) && (oldUnit.second >= 0))
		{
			Rect& rect = m_MapUnit.at(oldUnit);
			CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
			rc.InflateRect(2, 2, 2, 2);
			InvalidateRect( rc );
		}

		if (m_nHoverIndexDot >= 0)
		{
			if (!m_bIs3DMode)
			{
				Rect& rect = m_ArrayDot.at(m_nHoverIndexDot);
				CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
				rc.InflateRect(2, 2, 2, 2);
				InvalidateRect( rc );
			}
		}
		else if ((m_HoverIndexUnit.first >= 0) && (m_HoverIndexUnit.second >= 0))
		{
			Rect& rect = m_MapUnit.at(m_HoverIndexUnit);
			CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
			rc.InflateRect(2, 2, 2, 2);
			InvalidateRect( rc );
	
		}
	}
	
	UnitIndex index = std::make_pair((int)m_HoverIndexUnit.first, (int)m_HoverIndexUnit.second);
	MapUnitTypeResult::iterator iterFindType = m_MapUnitTypeResult.find( index );
	if (iterFindType == m_MapUnitTypeResult.end())
		return;

	UnitTypeResult& unitType = iterFindType->second;
	MapGroupIndex::iterator iterFindIndex = m_MapGroupIndex.find( index );
	if (iterFindIndex != m_MapGroupIndex.end())
	{
		int nIndex = iterFindIndex->second; // NG Value		
		m_strToolTipText.Format(_T("[%d,%d] %s"), (int)m_HoverIndexUnit.first, (int)m_HoverIndexUnit.second, GetLegendText(unitType) );
		//CString strpoint=_T("");
		//switch(unitType)
		//{
		//case UnitTypeOK:
		//	m_strToolTipText.Format( _T("%s : %d"), GetLegendText(unitType), nIndex );
		//	break;
		//case UnitTypeReject:
		//	m_strToolTipText.Format( _T("%s : %d"), GetLegendText(unitType), nIndex );
		//	break;
		//case UnitTypeNG_Shift:
		//	m_strToolTipText.Format( _T("%s : %d um"), GetLegendText(unitType), nIndex );
		//	break;
		//case UnitTypeNG_Tilt:
		//	m_strToolTipText.Format( _T("%s : %.2f deg"), GetLegendText(unitType), (float)(nIndex / 1000.0) );
		//	break;
		//case UnitTypeNG_Damaged:
		//	strpoint = _T("%");
		//	m_strToolTipText.Format( _T("%s : %.1f %s"), GetLegendText(unitType), (float)(nIndex / 1000.0), strpoint);
		//	break;
		//case UnitTypeNG_Double:
		//	strpoint = _T("%");
		//	m_strToolTipText.Format( _T("%s : %.1f %s"), GetLegendText(unitType), (float)(nIndex / 1000.0), strpoint );
		//	break;
		//case UnitTypeRejectX:
		//	m_strToolTipText.Format( _T("%s : %d"), GetLegendText(unitType), nIndex );
		//	break;
		//case UnitTypeNone:
		//	m_strToolTipText.Format( _T("%s : %d"), GetLegendText(unitType), (float)(nIndex) );
		//	break;		
		//}

		//m_pToolTip->AddTool(this, m_strToolTipText, CRect(point.x-1, point.y-1, point.x+1, point.y+1), TRUE);	
		//m_pToolTip->Update();		
	}

	

	CGdipStatic::OnMouseMove(nFlags, point);
}

void CObjectInfoCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nPressIndexDot;
	UnitIndex PressIndexUnit;

	if (HitTest(point, nPressIndexDot, PressIndexUnit))
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

	if ((PressIndexUnit.first >= 0) && (PressIndexUnit.second >= 0))
	{
		Rect& rect = m_MapUnit.at(PressIndexUnit);
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
		//
		::SendMessage( GetParent()->GetSafeHwnd(), UIExt::WM_OBJECTINFO_UNIT_SELECTED_EX, (WPARAM)MAKEWPARAM(PressIndexUnit.first, PressIndexUnit.second), (LPARAM)GetSafeHwnd() );
	}
	m_bIsButtonDown = TRUE;

	CGdipStatic::OnLButtonDown(nFlags, point);
}

void CObjectInfoCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( ::GetCapture() == m_hWnd )
	{
		::ReleaseCapture();
	}

	m_nHoverIndexDot = -1;
	m_HoverIndexUnit.first = m_HoverIndexUnit.second = -1;

	m_bIsButtonDown = FALSE;



	CGdipStatic::OnLButtonUp(nFlags, point);
}

void CObjectInfoCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nPressIndexDot;
	UnitIndex PressIndexUnit;

	if (HitTest(point, nPressIndexDot, PressIndexUnit))
	{
		if ( ::GetCapture() != m_hWnd )
		{
			::SetCapture( m_hWnd );
		}
	}
	else
	{
		CGdipStatic::OnLButtonDblClk(nFlags, point);
		return;
	}

	if ((PressIndexUnit.first >= 0) && (PressIndexUnit.second >= 0))
	{
		Rect& rect = m_MapUnit.at(PressIndexUnit);
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
		//
		::SendMessage( GetParent()->GetSafeHwnd(), UIExt::WM_OBJECTINFO_UNIT_STATE_CHANGE_EX, (WPARAM)MAKEWPARAM(PressIndexUnit.first, PressIndexUnit.second), (LPARAM)GetSafeHwnd() );
	}

	CGdipStatic::OnLButtonDblClk(nFlags, point);
}


BOOL UIExt::CObjectInfoCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pToolTip->RelayEvent(pMsg);


	return CGdipStatic::PreTranslateMessage(pMsg);
}


void UIExt::CObjectInfoCtrlEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	int nPressIndexDot;
	UnitIndex PressIndexUnit;

	if (HitTest(point, nPressIndexDot, PressIndexUnit))
	{
		if ( ::GetCapture() != m_hWnd )
		{
			::SetCapture( m_hWnd );
		}
	}
	else
	{
		CGdipStatic::OnRButtonDown(nFlags, point);
		return;
	}

	if ((PressIndexUnit.first >= 0) && (PressIndexUnit.second >= 0))
	{
		Rect& rect = m_MapUnit.at(PressIndexUnit);
		CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
		rc.InflateRect(2, 2, 2, 2);
		InvalidateRect( rc );
		//
		::SendMessage( GetParent()->GetSafeHwnd(), UIExt::WM_OBJECTINFO_UNIT_UNSELECTED_EX, (WPARAM)MAKEWPARAM(PressIndexUnit.first, PressIndexUnit.second), (LPARAM)GetSafeHwnd() );
	}

	CGdipStatic::OnRButtonDown(nFlags, point);
}
