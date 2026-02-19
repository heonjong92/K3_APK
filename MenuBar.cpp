// MenuBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "MenuBar.h"
#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"
#include "Resource.h"

#include "LanguageInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Gdiplus;
// CMenuBar

IMPLEMENT_DYNAMIC(CMenuBar, UIExt::CGdipBaseWnd)

CMenuBar::CMenuBar()
{
	m_pImageIcons = ::GdipLoadImageFromRes(AfxGetResourceHandle(), _T("PNG"), PNG_MENU_ICONS);
	m_nIconHeight = m_pImageIcons->GetHeight();
	m_nIconWidth = m_nIconHeight;
	m_nIconCount = m_pImageIcons->GetWidth() / m_nIconWidth;

	m_nLanguageIndex = CLanguageInfo::Instance()->ReadLanguageIni();

	for (int i=0 ; i<MenuIndexCount ; i++)
	{
		m_rectButtons[i].X = 0;
		m_rectButtons[i].Y = 0;
		m_rectButtons[i].Width = 0;
		m_rectButtons[i].Height = 0;
	}

	m_eButtonCheckedIndex = MenuIndexNone;
	m_eButtonHoverIndex = MenuIndexNone;
	m_eButtonPressIndex = MenuIndexNone;

	for (int i=0 ; i<MenuIndexCount ; i++)
	{
		m_bMenuEnabled[i] = TRUE;
	}

	m_bIsButtonDown = FALSE;
}

CMenuBar::~CMenuBar()
{
	if (m_pImageIcons)
	{
		delete m_pImageIcons;
		m_pImageIcons = NULL;
	}
}

BEGIN_MESSAGE_MAP(CMenuBar, UIExt::CGdipBaseWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CMenuBar::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("MenuBar"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1);
}

CString CMenuBar::GetButtonName( MenuIndex index )
{
	CString strLanguage = _T("");
	switch (index)
	{
		case MenuIndexInspection:
			strLanguage = _T("검  사");
			if (m_nLanguageIndex == 1) strLanguage = _T("Inspection");
			return strLanguage;
		case MenuIndexTeaching:
			strLanguage = _T("티  칭");
			if (m_nLanguageIndex == 1) strLanguage = _T("Teaching");
			return strLanguage;
		case MenuIndexDeviceSettings:
			strLanguage = _T("장  치");
			if (m_nLanguageIndex == 1) strLanguage = _T("Device");
			return strLanguage;
		case MenuIndexJobNumber:
			strLanguage = _T("캡  처");
			if (m_nLanguageIndex == 1) strLanguage = _T("Capture");
			return strLanguage;
		case MenuIndexAccess:
			strLanguage = _T("권  한");
			if (m_nLanguageIndex == 1) strLanguage = _T("AccessRight");
			return strLanguage;
		case MenuIndexExit:
			strLanguage = _T("종  료");
			if (m_nLanguageIndex == 1) strLanguage = _T("Exit");
			return strLanguage;
		case MenuIndexLanguege:
			strLanguage = _T("KOREA");
			if (m_nLanguageIndex == 1) strLanguage = _T("ENGLISH");
			return strLanguage;
	}
	return _T("");
}

void CMenuBar::SetMenuChecked(MenuIndex index)
{
	if(m_eButtonCheckedIndex != index)
	{
		MenuIndex oldMenuIndex = m_eButtonCheckedIndex;
		m_eButtonCheckedIndex = index;
		RedrawButton(oldMenuIndex);
		RedrawButton(m_eButtonCheckedIndex);
	}
}

void CMenuBar::EnableMenuItem(MenuIndex index, BOOL bEnabled)
{
	m_bMenuEnabled[index] = bEnabled;
	if (GetSafeHwnd())
		Invalidate();
}

int CMenuBar::DrawButton(Gdiplus::Graphics& g, Rect& rect, int nPosX, MenuIndex index, 
						   Gdiplus::Font* pFont, Gdiplus::Brush* pBrush, Gdiplus::StringFormat* pStringFormat, 
						   int nMarginX)
{
	m_rectButtons[index].X = nPosX;
	m_rectButtons[index].Y = rect.Y;
	m_rectButtons[index].Height = rect.Height;

	BOOL bIsChecked = FALSE;
	BOOL bIsPressed = FALSE;
	BOOL bIsHovered = FALSE;
	if ( index == m_eButtonCheckedIndex )
	{
		bIsChecked = TRUE;
	}
	if ( index == m_eButtonHoverIndex )
	{
		bIsHovered = TRUE;
	}
	if ( index == m_eButtonPressIndex )
	{
		bIsHovered = FALSE;
		bIsPressed = TRUE;
	}

	DWORD dwActiveBtnBgColor = UIExt::CResourceManager::Instance()->GetActiveMenuBackgroundColor();
	DWORD dwFgColor = UIExt::CResourceManager::Instance()->GetMenuForegroundColor();
	DWORD dwOverlayColor = UIExt::CResourceManager::Instance()->GetMenuButtonOverlayColor();
	Color colorButton = Color(0, GetRValue(dwOverlayColor), GetGValue(dwOverlayColor), GetBValue(dwOverlayColor));
	if (bIsChecked || bIsPressed)
		colorButton = Color(180, GetRValue(dwActiveBtnBgColor), GetGValue(dwActiveBtnBgColor), GetBValue(dwActiveBtnBgColor));

	if ( bIsHovered )
	{
		colorButton = Color(colorButton.GetA()+10, colorButton.GetR(), colorButton.GetG(), colorButton.GetB());
	}
	else if ( bIsPressed )
	{
		BYTE r = colorButton.GetR() > 50 ? colorButton.GetR() - 50 : 0;
		BYTE g = colorButton.GetG() > 50 ? colorButton.GetG() - 50 : 0;
		BYTE b = colorButton.GetB() > 50 ? colorButton.GetB() - 50 : 0;
		colorButton = Color(colorButton.GetA(), r, g, b);
	}

	nPosX += nMarginX;

	int nOffsetY = 0;
	if (bIsPressed) nOffsetY++;

	float fIconX, fIconY, fIconW, fIconH;
	fIconW = (float)m_nIconWidth;
	fIconH = (float)m_nIconHeight;
	fIconX = (float)nPosX;
	fIconY = ((float)rect.Height - fIconH) / 2 + rect.Y + nOffsetY;
	RectF rectIcon(fIconX, fIconY, fIconW, fIconH);
	rectIcon.X = (float)nPosX;

	nPosX += GetRoundLong(fIconW);

	RectF rectText;
	rectText.X = (float)nPosX;
	rectText.Width = rect.GetRight() - (float)nPosX;
	rectText.Y = (float)rect.Y;
	rectText.Height = (float)rect.Height;

	CString strText;
	strText = GetButtonName(index);
	BSTR bstrText = strText.AllocSysString();

	RectF rectTextB;
	g.MeasureString(bstrText, -1, pFont, rectText, &rectTextB);
	rectText.Width = rectTextB.Width;
	rectText.Width += (float)nMarginX;

	nPosX += GetRoundLong(rectText.Width);
	m_rectButtons[index].Width = nPosX - m_rectButtons[index].X;

	float fAlpha = (bIsChecked || bIsPressed || bIsHovered) ? 0.9f : 0.8f;
	DWORD dwDisabledColor = UIExt::CResourceManager::Instance()->GetMenuDisabledTextColor();
	Color colorDisabledText(80, GetRValue(dwDisabledColor), GetGValue(dwDisabledColor), GetBValue(dwDisabledColor));
	SolidBrush brushDisabledText( colorDisabledText );
	if (!m_bMenuEnabled[index])
	{
		fAlpha = 0.4f;
		pBrush = &brushDisabledText;
	}
	ImageAttributes ImgAttr;
	float fR = GetRValue(dwFgColor) / 255.f;
	float fG = GetGValue(dwFgColor) / 255.f;
	float fB = GetBValue(dwFgColor) / 255.f;
	ColorMatrix ClrMatrix = 
	{ 
		fR, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, fG, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, fB, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, fAlpha, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	Rect rectBody;
	rectBody.X = (int)m_rectButtons[index].X;
	rectBody.Y = (int)m_rectButtons[index].Y;
	rectBody.Width = GetRoundLong(m_rectButtons[index].Width);
	rectBody.Height = GetRoundLong(m_rectButtons[index].Height);
	SolidBrush brushBody(colorButton);
	g.FillRectangle( &brushBody, rectBody );

	rectText.Y += nOffsetY;
	g.DrawImage( m_pImageIcons, rectIcon, index*fIconW, 0.f, fIconW, fIconH, UnitPixel, &ImgAttr);
	g.DrawString(bstrText, -1, pFont, rectText, pStringFormat, pBrush);


	SysFreeString(bstrText);

	return nPosX;
}

void CMenuBar::DrawSplitter( Gdiplus::Graphics& g, int nX, int nTop, int nBottom, Gdiplus::Pen* pPen )
{
	g.DrawLine( pPen, nX, nTop, nX, nBottom );
}

void CMenuBar::DrawCloseButton( Gdiplus::Graphics& g, Rect& rect )
{
	Rect rectButton;

	rectButton.Height = rect.Height;
	rectButton.Width = rectButton.Height;
	rectButton.X = rect.GetRight()- rectButton.Width;
	rectButton.Y = rect.Y;

	BOOL bIsPressed = FALSE;
	BOOL bIsHovered = FALSE;
	if ( MenuIndexExit == m_eButtonHoverIndex )
	{
		bIsHovered = TRUE;
	}
	if ( MenuIndexExit == m_eButtonPressIndex )
	{
		bIsHovered = FALSE;
		bIsPressed = TRUE;
	}

	DWORD dwOverlayColor = UIExt::CResourceManager::Instance()->GetMenuButtonOverlayColor();
	DWORD dwBgColor = UIExt::CResourceManager::Instance()->GetMenuCloseButtonBackgroundColor();
	DWORD dwFgColor = UIExt::CResourceManager::Instance()->GetMenuForegroundColor();
	Color colorButton = Color(10, GetRValue(dwOverlayColor), GetGValue(dwOverlayColor), GetBValue(dwOverlayColor));
	if (bIsHovered || bIsPressed)
		colorButton = Color(180, GetRValue(dwBgColor), GetGValue(dwBgColor), GetBValue(dwBgColor));

	if ( bIsHovered )
	{
		colorButton = Color(colorButton.GetA()+10, colorButton.GetR(), colorButton.GetG(), colorButton.GetB());
	}
	else if ( bIsPressed )
	{
		BYTE r = colorButton.GetR() > 50 ? colorButton.GetR() - 50 : 0;
		BYTE g = colorButton.GetG() > 50 ? colorButton.GetG() - 50 : 0;
		BYTE b = colorButton.GetB() > 50 ? colorButton.GetB() - 50 : 0;
		colorButton = Color(colorButton.GetA(), r, g, b);
	}

	int nOffsetY = 0;
	if (bIsPressed) nOffsetY++;

	g.FillRectangle( &SolidBrush(colorButton), rectButton );

	float fAlpha = (bIsPressed || bIsHovered) ? 0.9f : 0.8f;
	ImageAttributes ImgAttr;
	float fR = GetRValue(dwFgColor) / 255.f;
	float fG = GetGValue(dwFgColor) / 255.f;
	float fB = GetBValue(dwFgColor) / 255.f;
	ColorMatrix ClrMatrix = 
	{ 
		fR, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, fG, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, fB, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, fAlpha, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	float fIconX, fIconY, fIconW, fIconH;
	fIconW = (float)m_nIconWidth;
	fIconH = (float)m_nIconHeight;
	fIconX = (rectButton.Width - fIconW) / 2 + rectButton.X;
	fIconY = (rectButton.Height - fIconH) / 2 + rectButton.Y + nOffsetY;
	RectF rectIcon(fIconX, fIconY, fIconW, fIconH);
	g.DrawImage( m_pImageIcons, rectIcon, MenuIndexExit*fIconW, 0.f, fIconW, fIconH, UnitPixel, &ImgAttr);

	m_rectButtons[MenuIndexExit] = rectButton;
}

void CMenuBar::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	DWORD dwFgColor = UIExt::CResourceManager::Instance()->GetMenuForegroundColor();
	Color colorText(180, GetRValue(dwFgColor), GetGValue(dwFgColor), GetBValue(dwFgColor));
	SolidBrush brushText( colorText );
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentNear );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();

	Gdiplus::Font fontText(pFontFamily, 13, FontStyleBold, UnitPixel);

	int nPosX = rect.X;

	const int nButtonMarginX = 15;

	Pen penSplitter(Color(10, 255,255,255));
	nPosX = DrawButton( g, rect, nPosX, MenuIndexInspection, &fontText, &brushText, &stringFormat, nButtonMarginX );
	DrawSplitter( g, nPosX, rect.GetTop(), rect.GetBottom(), &penSplitter );
	nPosX = DrawButton( g, rect, nPosX, MenuIndexTeaching, &fontText, &brushText, &stringFormat, nButtonMarginX );
	DrawSplitter( g, nPosX, rect.GetTop(), rect.GetBottom(), &penSplitter );
	nPosX = DrawButton( g, rect, nPosX, MenuIndexDeviceSettings, &fontText, &brushText, &stringFormat, nButtonMarginX );
	DrawSplitter( g, nPosX, rect.GetTop(), rect.GetBottom(), &penSplitter );
	nPosX = DrawButton( g, rect, nPosX, MenuIndexAccess, &fontText, &brushText, &stringFormat, nButtonMarginX );
	DrawSplitter( g, nPosX, rect.GetTop(), rect.GetBottom(), &penSplitter );
	nPosX = DrawButton( g, rect, nPosX, MenuIndexLanguege, &fontText, &brushText, &stringFormat, nButtonMarginX );
	DrawSplitter( g, nPosX, rect.GetTop(), rect.GetBottom(), &penSplitter );
	nPosX = DrawButton(g, rect, nPosX, MenuIndexJobNumber, &fontText, &brushText, &stringFormat, nButtonMarginX);
	DrawSplitter(g, nPosX, rect.GetTop(), rect.GetBottom(), &penSplitter);

	DrawCloseButton( g, rect );
}

void CMenuBar::OnGdipEraseBkgnd(Gdiplus::Graphics& g, Rect rect)
{
	DWORD dwBgColor = UIExt::CResourceManager::Instance()->GetMenuBackgroundColor();
	g.FillRectangle( 
		&SolidBrush(Color(GetRValue(dwBgColor), GetGValue(dwBgColor), GetBValue(dwBgColor))), 
		rect );
}

void CMenuBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( ::GetCapture() != m_hWnd ) ::SetCapture( m_hWnd );

	for ( int i=0 ; i<MenuIndexCount ; i++ )
	{
		if ( m_bMenuEnabled[i] && (m_rectButtons[i].Contains(point.x, point.y)) )
		{
			m_eButtonPressIndex = (MenuIndex)i;
			RedrawButton( m_eButtonPressIndex );
			break;
		}
	}

	m_bIsButtonDown = TRUE;


	UIExt::CGdipBaseWnd::OnLButtonDown(nFlags, point);
}

void CMenuBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( ::GetCapture() == m_hWnd )
		::ReleaseCapture();

	MenuIndex eReleaseIndex = MenuIndexNone;
	for ( int i=0 ; i<MenuIndexCount ; i++ )
	{
		if ( m_bMenuEnabled[i] && (m_rectButtons[i].Contains(point.x, point.y)) )
		{
			eReleaseIndex = (MenuIndex)i;
			break;
		}
	}

	if (m_eButtonPressIndex == eReleaseIndex)
	{
		switch (eReleaseIndex)
		{
		case MenuIndexInspection:
			GetParent()->PostMessage( WM_COMMAND, (WPARAM)ID_MENU_INSPECTION );
			break;
		case MenuIndexTeaching:
			GetParent()->PostMessage( WM_COMMAND, (WPARAM)ID_MENU_TEACHING );
			break;
		case MenuIndexDeviceSettings:
			GetParent()->PostMessage( WM_COMMAND, (WPARAM)ID_MENU_DEVICE_SETTINGS );
			break;
		case MenuIndexJobNumber:
			GetParent()->PostMessage( WM_COMMAND, (WPARAM)ID_MENU_JOB_NUMBER );
			break;
		case MenuIndexAccess:
			GetParent()->PostMessage( WM_COMMAND, (WPARAM)ID_MENU_ACCESS_RIGHTS );
			break;
		case MenuIndexExit:
			GetParent()->PostMessage( WM_COMMAND, (WPARAM)ID_MENU_EXIT );
			break;
		case MenuIndexLanguege:
			m_nLanguageIndex++;
			if (m_nLanguageIndex >= 2) m_nLanguageIndex = 0;
			CLanguageInfo::Instance()->WriteLanguageIni(m_nLanguageIndex);
			GetParent()->PostMessage(WM_COMMAND, (WPARAM)ID_MENU_LANGUAGE);

			Invalidate();
			UpdateWindow();
			break;
		}
	}

	if (m_eButtonPressIndex != MenuIndexNone)
	{
		MenuIndex oldIndex = m_eButtonPressIndex;
		m_eButtonPressIndex = MenuIndexNone;
		RedrawButton( oldIndex );
	}

	m_eButtonHoverIndex = MenuIndexNone;

	m_bIsButtonDown = FALSE;

	UIExt::CGdipBaseWnd::OnLButtonUp(nFlags, point);
}

void CMenuBar::RedrawButton(MenuIndex index)
{
	if (index == MenuIndexNone || index == MenuIndexCount)
		return;

	CRect rcButton;
	rcButton.left = (long)m_rectButtons[index].GetLeft();
	rcButton.top = (long)m_rectButtons[index].GetTop();
	rcButton.right = (long)m_rectButtons[index].GetRight();
	rcButton.bottom = (long)m_rectButtons[index].GetBottom();

	rcButton.InflateRect( 2, 2, 2, 2 );

	InvalidateRect( rcButton );
}

void CMenuBar::OnMouseMove(UINT nFlags, CPoint point)
{
	MenuIndex eHoverIndex = MenuIndexNone;
	for ( int i=0 ; i<MenuIndexCount ; i++ )
	{
		if ( m_bMenuEnabled[i] && (m_rectButtons[i].Contains(point.x, point.y)) )
		{
			eHoverIndex = (MenuIndex)i;
			if ( ::GetCapture() != m_hWnd )
			{
				::SetCapture(m_hWnd);
			}
			break;
		}
	}

	if ( eHoverIndex != m_eButtonHoverIndex )
	{
		if (eHoverIndex == MenuIndexNone)
		{
			if ( !m_bIsButtonDown && ::GetCapture() == m_hWnd )
			{
				::ReleaseCapture();
			}
		}
		MenuIndex oldIndex = m_eButtonHoverIndex;
		m_eButtonHoverIndex = eHoverIndex;
		RedrawButton(oldIndex);
		RedrawButton(m_eButtonHoverIndex);
	}

	UIExt::CGdipBaseWnd::OnMouseMove(nFlags, point);
}
