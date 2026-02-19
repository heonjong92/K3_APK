#pragma once

#include "GdiplusExt.h"

namespace UIExt
{

class CGdipBaseWnd : public CWnd
{
	DECLARE_DYNAMIC(CGdipBaseWnd)

protected:
	CDC			m_MemDC;
	CBitmap*	m_pBitmap;

public:
	CGdipBaseWnd();
	virtual ~CGdipBaseWnd();

protected:
	virtual void OnDraw( Graphics& g, Rect rect );
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect );

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};

class CGdipStatic : public CGdipBaseWnd
{
	DECLARE_DYNAMIC(CGdipStatic)

// Constructors
public:
	CGdipStatic();
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

// Operations
	HICON SetIcon(HICON hIcon);
	HICON GetIcon() const;

	HENHMETAFILE SetEnhMetaFile(HENHMETAFILE hMetaFile);
	HENHMETAFILE GetEnhMetaFile() const;
	HBITMAP SetBitmap(HBITMAP hBitmap);
	HBITMAP GetBitmap() const;
	HCURSOR SetCursor(HCURSOR hCursor);
	HCURSOR GetCursor();

// Overridables (for owner draw only)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CGdipStatic();
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	afx_msg void OnUpdateUIState(UINT nAction, UINT nUIElement);
};

inline CGdipStatic::CGdipStatic()
	{ }
inline HICON CGdipStatic::SetIcon(HICON hIcon)
	{ ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, STM_SETICON, (WPARAM)hIcon, 0L); }
inline HICON CGdipStatic::GetIcon() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, STM_GETICON, 0, 0L); }
inline HENHMETAFILE CGdipStatic::SetEnhMetaFile(HENHMETAFILE hMetaFile)
	{ ASSERT(::IsWindow(m_hWnd)); return (HENHMETAFILE)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_ENHMETAFILE, (LPARAM)hMetaFile); }
inline HENHMETAFILE CGdipStatic::GetEnhMetaFile() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HENHMETAFILE)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0L); }
inline HBITMAP CGdipStatic::SetBitmap(HBITMAP hBitmap)
	{ ASSERT(::IsWindow(m_hWnd)); return (HBITMAP)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap); }
inline HBITMAP CGdipStatic::GetBitmap() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HBITMAP)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_BITMAP, 0L); }
inline HCURSOR CGdipStatic::SetCursor(HCURSOR hCursor)
	{ ASSERT(::IsWindow(m_hWnd)); return (HCURSOR)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor); }
inline HCURSOR CGdipStatic::GetCursor()
	{ ASSERT(::IsWindow(m_hWnd)); return (HCURSOR)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_CURSOR, 0L); }


} // namespace UIExt