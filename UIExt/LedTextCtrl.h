#if !defined(AFX_LEDTEXTWND_H__7E875FA3_F845_41E1_A650_A00B2925FD24__INCLUDED_)
#define AFX_LEDTEXTWND_H__7E875FA3_F845_41E1_A650_A00B2925FD24__INCLUDED_

#include "GdipWndExt.h"

#define MAX_CACHE_PATH				10

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LedTextWnd.h : header file
//

namespace UIExt {
/////////////////////////////////////////////////////////////////////////////
// CLedTextCtrl window

class CLedTextCtrl : public CGdipStatic
{
public:
	enum TextAlignment { TA_Left, TA_Right };
	
// Construction
public:
	CLedTextCtrl();

// Attributes
protected:
	const float WIDTHHEIGHTRATIO;
	
	Gdiplus::GraphicsPath		m_CachedPaths[MAX_CACHE_PATH];
	BOOL						m_bIsCacheBuild;
	int							m_nBorderWidth;
	Gdiplus::Color				m_colBorderColor;
	int							m_nCornerRadius;
	int							m_nCharacterNumber;
	float						m_fBevelRate;
	Gdiplus::Color				m_colFadedColor;
	Gdiplus::Color				m_colCustomBk1;
	Gdiplus::Color				m_colCustomBk2;
	float						m_fWidthSegWidthRatio;
	float						m_fWidthIntervalRatio;
	TextAlignment				m_enumAlign;
	BOOL						m_bRoundRect;
	BOOL						m_bGradientBackground;
	BOOL						m_bShowHighlight;
	BYTE						m_nHighlightOpaque;

	Gdiplus::Color				m_colForeColor;
	Gdiplus::Color				m_colBackColor;

	CString						m_strText;

protected:
	virtual void OnDraw(Graphics& g, Rect rect) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

// Operations
protected:
	
	void CalculateDrawingParams( const CRect& rcClientRect, float &fSegmentWidth, float &fSegmentInterval );
	void DrawHighlight( const CRect& rcClientRect, Gdiplus::Graphics& graphics );
	void DrawRoundRect( Gdiplus::Graphics& graphics, Gdiplus::Rect& rect, float radius, Gdiplus::Color col1,
						Gdiplus::Color col2, Gdiplus::Color colBorder, int nBorderWidth, BOOL bGradient, BOOL bDrawBorder);
	void DrawNormalRect( Gdiplus::Graphics& graphics, Gdiplus::Rect& rect, Gdiplus::Color col1, Gdiplus::Color col2,
						Gdiplus::Color colBorder, int nBorderWidth, BOOL bGradient, BOOL bDrawBorder );

	virtual void DrawBackground( const CRect& rcClientRect, Gdiplus::Graphics& graphics );
	virtual void DrawChars( const CRect& rcClientRect, Gdiplus::Graphics& graphics, float fSegmentWidth, float fSegmentInterval );

	void DrawSegment( const CRect& rcClientRect, Gdiplus::Graphics& graphics, Gdiplus::Rect& rectBound, Gdiplus::Color colSegment, int nIndex,
								float fBevelRate, float fSegmentWidth, float fSegmentInterval );
	void DrawSingleCharWithFadedBk( const CRect& rcClientRect, Gdiplus::Graphics& graphics, Gdiplus::Rect& rectBound, Gdiplus::Color colCharacter,
						  Gdiplus::Color colFaded, TCHAR cCharacter, float fBevelRate, float fSegmentWidth, float fSegmentInterval );
	void DrawSingleChar( const CRect& rcClientRect, Gdiplus::Graphics& graphics, Gdiplus::Rect& rectBound, Gdiplus::Color colCharacter, char cCharacter,
						   float fBevelRate, float fSegmentWidth, float fSegmentInterval);

	void CreateCache( Gdiplus::Rect& rectBound, float fBevelRate, float fSegmentWidth, float fSegmentInterval );
	void DestoryCache();

	void SetText( LPCTSTR lpszText );
	
public:
	int GetBorderWidth() const { return m_nBorderWidth; }
	void SetBorderWidth( int nBorderWidth );
	DWORD GetBorderColor() const { return m_colBorderColor.ToCOLORREF(); }
	void SetBorderColor( DWORD dwColor );
	int GetHighlightOpaque() const { return m_nHighlightOpaque; }
	void SetHighlightOpaque( int nHighlightOpaque );
	BOOL IsShowHighlight() const { return m_bShowHighlight; }
	void SetShowHighlight( BOOL bShow );
	int GetCornerRadius() const { return m_nCornerRadius; }
	void SetCornerRadius( int nCornerRadius );
	BOOL IsGradientBackground() const { return m_bGradientBackground; }
	void SetGradientBackground( BOOL bSet );
	DWORD GetBackColor1() const { return m_colCustomBk1.ToCOLORREF(); }
	void SetBackColor1( DWORD dwColor );
	DWORD GetBackColor2() const { return m_colCustomBk1.ToCOLORREF(); }
	void SetBackColor2( DWORD dwColor );
	BOOL IsRoundCorner() const { return m_bRoundRect; }
	void SetRoundCorner( BOOL bSet );
	int GetSegmentIntervalRatio() { return (int)((m_fWidthIntervalRatio - 0.01f) * 1000); }
	void SetSegmentIntervalRatio( int nSeqInterRatio );
	TextAlignment GetTextAlignment() const { return m_enumAlign; }
	void SetTextAlignment( TextAlignment eAlign );
	int GetSegmentWidthRatio() const { return (int)((m_fWidthSegWidthRatio - 0.1f) * 500); }
	void SetSegmentWidthRatio( int nSeqWidthRatio );
	int GetTotalCharCount() const { return m_nCharacterNumber; }
	void SetTotalCharCount( int nCharCount );
	float GetBevelRate() const { return m_fBevelRate * 2; }
	void SetBevelRate( float fBevelRate );
	DWORD GetFadedColor() const { return m_colFadedColor.ToCOLORREF(); }
	void SetFadedColor( DWORD dwColor );

	DWORD GetForeColor() const { return m_colForeColor.ToCOLORREF(); }
	void SetForeColor( DWORD dwColor );

	void AdjustWnd(BOOL bAlignRight=FALSE);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLedTextCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLedTextCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLedTextCtrl)
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnPaint();
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnDestroy();
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEDTEXTWND_H__7E875FA3_F845_41E1_A650_A00B2925FD24__INCLUDED_)
