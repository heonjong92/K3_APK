#include "stdafx.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;

CResourceManager::CResourceManager(void)
{
	NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	USES_CONVERSION;

	WCHAR* wszFaceName;
	CString strX = ncm.lfMessageFont.lfFaceName;
	if ( strX.IsEmpty() )
		wszFaceName = L"Arial";
	else
		wszFaceName = (WCHAR*)T2W(ncm.lfMessageFont.lfFaceName);

	m_pFontFamilySystem = ::new Gdiplus::FontFamily( wszFaceName );

	m_dwTitleBgColor          = RGB(73,40,33);
	m_dwTitleFgColor          = RGB(255, 255, 255);
	m_dwMenuBgColor           = RGB(128,79,74);
	m_dwMenuFgColor           = RGB(255,255,255);
	m_dwMenuActiveBgColor     = RGB(200, 30, 30);
	m_dwMenuCloseBtnBgColor   = RGB(200, 30, 30);
	m_dwMenuBtnOverlayColor   = RGB(255, 255, 255);
	m_dwMenuDisabledTextColor = RGB(255,255,255);
	m_dwViewerTitleBgColor    = RGB(89,101,127);
	m_dwViewerTitleFgColor    = RGB(255,255,255);
	m_dwViewerBtnActiveColor  = RGB(200,30,30);
	m_dwViewerStatusBgColor   = RGB(227,231,236);
	m_dwViewerStatusFgColor   = RGB(80,80,80);
	m_dwViewerBodyColor       = RGB(210,215,223);
	m_dwBtmStatusFgColor      = RGB(255,255,255);
	m_dwBtmStatusBgColor      = RGB(67,72,78);
	m_dwMainBodyColor         = RGB(150,150,150);

	m_dwSideBarBodyColor      = RGB(241,242,245);//RGB(210,215,223);
	m_dwSideBarTitleBgColor   = RGB(30,50,70);
	m_dwSideBarTitleFgColor	  = RGB(255,255,255);
	m_dwSideBarItemBgColor    = RGB(180,180,180);
	m_dwSideBarItemFgColor    = RGB(25,25,25);
}

CResourceManager::~CResourceManager(void)
{
	if ( m_pFontFamilySystem )
	{
		::delete m_pFontFamilySystem;
		m_pFontFamilySystem = NULL;
	}
}

Gdiplus::FontFamily* CResourceManager::SystemFontFamily()
{
	return CResourceManager::Instance()->m_pFontFamilySystem;
}

Gdiplus::FontFamily* CResourceManager::GetSystemFontFamily()
{
	return m_pFontFamilySystem;
}
