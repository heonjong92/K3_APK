#pragma once

#include "../Common/xSingletoneObject.h"
#include <gdiplus.h>

namespace UIExt
{

class CResourceManager : public CxSingleton<CResourceManager>
{
protected:
	Gdiplus::FontFamily*	m_pFontFamilySystem;

	COLORREF				m_dwTitleBgColor;
	COLORREF				m_dwTitleFgColor;
	COLORREF				m_dwMenuBgColor;
	COLORREF				m_dwMenuFgColor;
	COLORREF				m_dwMenuActiveBgColor;
	COLORREF				m_dwMenuCloseBtnBgColor;
	COLORREF				m_dwMenuBtnOverlayColor;
	COLORREF				m_dwMenuDisabledTextColor;
	COLORREF				m_dwViewerTitleBgColor;
	COLORREF				m_dwViewerTitleFgColor;
	COLORREF				m_dwViewerBtnActiveColor;
	COLORREF				m_dwViewerStatusBgColor;
	COLORREF				m_dwViewerStatusFgColor;
	COLORREF				m_dwViewerBodyColor;
	COLORREF				m_dwBtmStatusFgColor;
	COLORREF				m_dwBtmStatusBgColor;
	COLORREF				m_dwMainBodyColor;

	COLORREF				m_dwSideBarBodyColor;
	COLORREF				m_dwSideBarTitleBgColor;
	COLORREF				m_dwSideBarTitleFgColor;
	COLORREF				m_dwSideBarItemBgColor;
	COLORREF				m_dwSideBarItemFgColor;

public:
	Gdiplus::FontFamily* GetSystemFontFamily();
	static Gdiplus::FontFamily* SystemFontFamily();

	DWORD GetMainBodyColor() const { return m_dwMainBodyColor; }

	DWORD GetTitleBackgroundColor() const { return m_dwTitleBgColor; }
	DWORD GetTitleForegroundColor() const { return m_dwTitleFgColor; }

	DWORD GetMenuBackgroundColor() const { return m_dwMenuBgColor; }
	DWORD GetMenuForegroundColor() const { return m_dwMenuFgColor; }
	DWORD GetActiveMenuBackgroundColor() const { return m_dwMenuActiveBgColor; }
	DWORD GetMenuCloseButtonBackgroundColor() const { return m_dwMenuCloseBtnBgColor; }
	DWORD GetMenuButtonOverlayColor() const { return m_dwMenuBtnOverlayColor; }
	DWORD GetMenuDisabledTextColor() const { return m_dwMenuDisabledTextColor; }
	
	DWORD GetViewerTitleBackgroundColor() const { return m_dwViewerTitleBgColor; }
	DWORD GetViewerTitleForegroundColor() const { return m_dwViewerTitleFgColor; }
	DWORD GetViewerActiveButtonColor() const { return m_dwViewerBtnActiveColor; }
	DWORD GetViewerStatusBackgroundColor() const { return m_dwViewerStatusBgColor; }
	DWORD GetViewerStatusForegroundColor() const { return m_dwViewerStatusFgColor; }
	DWORD GetViewerBodyColor() const { return m_dwViewerBodyColor; }

	DWORD GetBottomStatusBarBackgroundColor() const { return m_dwBtmStatusBgColor; }
	DWORD GetBottomStatusBarForegroundColor() const { return m_dwBtmStatusFgColor; }

	DWORD GetSideBarBodyColor() const { return m_dwSideBarBodyColor; }
	DWORD GetSideBarTitleBackgroundColor() const { return m_dwSideBarTitleBgColor; }
	DWORD GetSideBarTitleForegroundColor() const { return m_dwSideBarTitleFgColor; }
	DWORD GetSideBarItemBackgroundColor() const { return m_dwSideBarItemBgColor; }
	DWORD GetSideBarItemForegroundColor() const { return m_dwSideBarItemFgColor; }

public:
	CResourceManager(void);
	virtual ~CResourceManager(void);
};

} // namespace UIExt