// TitleBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TitleBar.h"
#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"
#include "Resource.h"
#include "VersionInfo.h"

#include <XUtil/xUtils.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Gdiplus;
// CTitleBar

IMPLEMENT_DYNAMIC(CTitleBar, UIExt::CGdipBaseWnd) 

TCHAR g_szJobNumber[] = _T("Job Number: ");
TCHAR g_szModelName[] = _T("Model Name: ");	
TCHAR g_szAccess[] = _T("Access: ");

CTitleBar::CTitleBar()
{
	m_pLogoImage = ::GdipLoadImageFromRes(AfxGetResourceHandle(), _T("PNG"), PNG_INSP_LOGO);

#ifdef RELEASE_1G
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PTC201 DEBUG - TRAY SEPARATOR]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PTC201 - TRAY SEPARATOR]"));
	#endif
	
#elif RELEASE_SG
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PTC201S DEBUG - TRAY STACKER]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PTC201S - TRAY STACKER]"));
	#endif

#elif RELEASE_2G
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PBH202 DEBUG - BANDING]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PBH202 - BANDING]"));
	#endif

#elif RELEASE_3G
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PDK203 DEBUG - SUB MATERIAL]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PDK203 - SUB MATERIAL]"));
	#endif

#elif RELEASE_4G
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PMB204 DEBUG - MBB SEALING]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PMB204 - MBB SEALING]"));
	#endif

#elif RELEASE_5G
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PMF205 DEBUG - MBB FOLDING]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PMF205 - MBB FOLDING]"));
	#endif

#elif RELEASE_6G
	#ifdef DEBUG
		m_strTitle.Format(_T("Auto Packing System [PWF207 DEBUG - BOX WING FOLDING]"));
	#else
		m_strTitle.Format(_T("Auto Packing System [PWF207 - BOX WING FOLDING]"));
	#endif

#endif

	m_nJobNumber = 0;
	m_strModelName = _T("");
	m_strAccessName = _T("OPERATOR");
}

CTitleBar::~CTitleBar()
{
	delete m_pLogoImage;
}

BEGIN_MESSAGE_MAP(CTitleBar, UIExt::CGdipBaseWnd)
END_MESSAGE_MAP()

BOOL CTitleBar::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("TitleBar"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1);
}

void CTitleBar::SetModelName( LPCTSTR lpszModelName )
{
	m_strModelName = lpszModelName;
	Invalidate();
}

void CTitleBar::SetJobNumber( UINT nJobNumber )
{
	m_nJobNumber = nJobNumber;
	Invalidate();
}

void CTitleBar::SetAccessName( LPCTSTR lpszAccessName )
{
	m_strAccessName = lpszAccessName;
	Invalidate();
}

void CTitleBar::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	Rect rectLogo(rect.X, rect.Y, rect.Height, rect.Height);
	int nIW = 0;
	int nIH = 0;

	if(m_pLogoImage != NULL)
	{
		nIW = m_pLogoImage->GetWidth();
		nIH = m_pLogoImage->GetHeight();
	}

	int nIconSize = 28;

	int nIX = (rectLogo.Width - nIconSize) / 2 + rectLogo.X;
	int nIY = (rectLogo.Height - nIconSize) / 2 + rectLogo.Y;

	//g.FillRectangle( &SolidBrush(Color(10,255,255,255)), rectLogo );

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.6f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	if(m_pLogoImage != NULL)
		g.DrawImage( m_pLogoImage, Rect(nIX, nIY, nIconSize, nIconSize), 0, 0, nIW, nIH, UnitPixel, &ImgAttr );
	//g.DrawImage( m_pLogoImage, Rect(nIX, nIY, nIconSize, nIconSize), 0, 0, nIW, nIH, UnitPixel, 0 );

	const int nOffsetX = 5;
	int nTitleX = nIX+nIconSize+nOffsetX;

	BSTR bstrTitle = m_strTitle.AllocSysString();
	RectF rectTitle(
		(REAL)nTitleX, 
		(REAL)rect.Y+2, 
		(REAL)rect.Width/2-nTitleX, 
		(REAL)rect.Height-2);

	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentNear );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	DWORD dwFgColor = UIExt::CResourceManager::Instance()->GetTitleForegroundColor();

	SolidBrush brushTitle( Color(180, GetRValue(dwFgColor), GetGValue(dwFgColor), GetBValue(dwFgColor)) );

	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();

	g.SetTextRenderingHint(TextRenderingHintAntiAlias);

	Gdiplus::Font fontTitle(pFontFamily, 18, FontStyleBold, UnitPixel);
	g.DrawString( bstrTitle, -1, &fontTitle, rectTitle, &stringFormat, &brushTitle );
	SysFreeString( bstrTitle );

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	int nItemWidth = 1280/2;
	RectF rectModelName((float)rect.GetRight()-nItemWidth*1.3f, (float)rect.Y, (float)nItemWidth, (float)rect.Height);
	RectF rectJobNumber((float)rect.GetRight()-nItemWidth*0.9f, (float)rect.Y, (float)nItemWidth*0.8f, (float)rect.Height);
	RectF rectAccess((float)rect.GetRight()-nItemWidth*0.7f, (float)rect.Y, (float)nItemWidth*0.8f, (float)rect.Height);
	RectF rectVersion((float)rect.GetRight()-nItemWidth*0.4f, (float)rect.Y, (float)nItemWidth*1.2f, (float)rect.Height);
// 	RectF rectPanelID((float)rect.GetRight()-nItemWidth*5.4f, (float)rect.Y, (float)nItemWidth, (float)rect.Height);
// 	RectF rectGlassCode((float)rect.GetRight()-nItemWidth*4.4f, (float)rect.Y, (float)nItemWidth*0.8f, (float)rect.Height);
// 	RectF rectJudge((float)rect.GetRight()-nItemWidth*3.7f, (float)rect.Y, (float)nItemWidth*0.8f, (float)rect.Height);

	Gdiplus::Font fontItem1(pFontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::Font fontItem2(pFontFamily, 13, FontStyleBold, UnitPixel);

//	CString strModelName;
//	strModelName.Format( _T("%s%s"), g_szModelName, m_strModelName );
//	BSTR bstrModelName = strModelName.AllocSysString();
//	g.DrawString( bstrModelName, -1, &fontItem1, rectModelName, &stringFormat, &brushTitle );
//	SysFreeString( bstrModelName );
//
//	CString strJobNumber;
//	strJobNumber.Format( _T("%s%d"), g_szJobNumber, m_nJobNumber );
//	BSTR bstrJobNumber = strJobNumber.AllocSysString();
//	g.DrawString( bstrJobNumber, -1, &fontItem2, rectJobNumber, &stringFormat, &brushTitle );
//	SysFreeString( bstrJobNumber );

	CString strAccess;
	strAccess.Format( _T("%s%s"), g_szAccess, m_strAccessName );
	BSTR bstrAccess = strAccess.AllocSysString();
	g.DrawString( bstrAccess, -1, &fontItem2, rectAccess, &stringFormat, &brushTitle );
	SysFreeString( bstrAccess );

	CString strVersion;
	strVersion = GetCompileTime();
	BSTR bstrVersion = strVersion.AllocSysString();
	g.DrawString( bstrVersion, -1, &fontItem2, rectVersion, &stringFormat, &brushTitle );
	SysFreeString( bstrVersion );
}

void CTitleBar::OnGdipEraseBkgnd(Gdiplus::Graphics& g, Rect rect)
{
	DWORD dwBgColor = UIExt::CResourceManager::Instance()->GetTitleBackgroundColor();

	g.FillRectangle( 
		&SolidBrush(Color(GetRValue(dwBgColor),GetGValue(dwBgColor),GetBValue(dwBgColor))), 
		rect );
}

CString CTitleBar::GetCompileTime()
{
	///////////////////////   GetCompile Time   ////////////////////////////////////////////////////
	CString strMonth[]={L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};

	CString date,time;
	CString strVersion;
	date.Format(L"%s", _T(__DATE__));
	time.Format(L"%s", _T(__TIME__));
	for (UINT nIdx = 0; nIdx < 12; nIdx++)
	{
		if (date.Left(3) == strMonth[nIdx])
		{
			CString strCompileTime;
			CTime tmCompile( _ttoi(date.Right(4)), nIdx + 1, _ttoi(date.Mid(4,2)), _ttoi(time.Left(2)), _ttoi(time.Mid(3,2)), _ttoi(time.Right(2)) );
			strCompileTime = tmCompile.Format(_T("%Y-%m-%d %H:%M:%S"));
			strVersion.Format(_T("[ Ver : %s ] [ %s ]"), _T(FILE_VERSION_STR), strCompileTime);
			return strVersion;
		}
	}
	return strVersion;
}