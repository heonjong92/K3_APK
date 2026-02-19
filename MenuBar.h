#pragma once

#include "UIExt/GdipWndExt.h"

// CMenuBar

#define	ID_MENU_INSPECTION			(100)
#define ID_MENU_TEACHING			(101)
#define ID_MENU_DEVICE_SETTINGS		(102)
#define ID_MENU_JOB_NUMBER			(103)
#define ID_MENU_ACCESS_RIGHTS		(104)
#define ID_MENU_EXIT				(105)
#define ID_MENU_LANGUAGE			(106)

class CMenuBar : public UIExt::CGdipBaseWnd
{
	DECLARE_DYNAMIC(CMenuBar)

public:
	CMenuBar();
	virtual ~CMenuBar();

	BOOL Create(CWnd* pParentWnd = NULL);

	enum MenuIndex { 
		MenuIndexNone = -1,
		MenuIndexInspection = 0,
		MenuIndexTeaching = 1,
		MenuIndexDeviceSettings = 2,
		MenuIndexLanguege = 3,
		MenuIndexJobNumber = 4,
		MenuIndexAccess = 5,
		MenuIndexExit = 6,
		MenuIndexCount = 7};

	void SetMenuChecked( MenuIndex index );

	void EnableMenuItem( MenuIndex index, BOOL bEnabled );

protected:

	Gdiplus::Rect m_rectButtons[MenuIndexCount];
	BOOL m_bIsButtonDown;

	BOOL			m_bMenuEnabled[MenuIndexCount];

	MenuIndex		m_eButtonHoverIndex;
	MenuIndex		m_eButtonPressIndex;
	MenuIndex		m_eButtonCheckedIndex;
	int				m_nLanguageIndex;

	int DrawButton(Gdiplus::Graphics& g, Gdiplus::Rect& rc, int nPosX, MenuIndex index, 
					 Gdiplus::Font* pFont, Gdiplus::Brush* pBrush, Gdiplus::StringFormat* pStringFormat,
					 int nMarginX);
	void DrawSplitter( Gdiplus::Graphics& g, int nX, int nTop, int nBottom, Gdiplus::Pen* pPen );
	void DrawCloseButton( Gdiplus::Graphics& g, Gdiplus::Rect& rc );

	void RedrawButton(MenuIndex index);

	CString GetButtonName( MenuIndex index );
	Gdiplus::Image* m_pImageIcons;
	int m_nIconWidth;
	int m_nIconHeight;
	int m_nIconCount;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


