// BottomStatusBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "BottomStatusBar.h"
#include "UIExt/GdiplusExt.h"
#include "UIExt/ResourceManager.h"
#include "Resource.h"
#include "VisionSystem.h"

using namespace Gdiplus;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CBottomStatusBar

IMPLEMENT_DYNAMIC(CBottomStatusBar, UIExt::CGdipBaseWnd)

CBottomStatusBar::CBottomStatusBar()
{
	m_pImageHITSLogo = ::GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_HITS_LOGO );
}

CBottomStatusBar::~CBottomStatusBar()
{
	if (m_pImageHITSLogo)
		delete m_pImageHITSLogo;
	m_pImageHITSLogo = NULL;
}

BEGIN_MESSAGE_MAP(CBottomStatusBar, UIExt::CGdipBaseWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CBottomStatusBar::NotifyAlive()
{
	//if (m_wndHeartBeatCtrl)
	//	m_wndHeartBeatCtrl.Beat();
}

BOOL CBottomStatusBar::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("BottomStatusBar"), WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1);
}

void CBottomStatusBar::OnDraw(Gdiplus::Graphics& g, Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	rect.X += 10;
	rect.Width -= 20;

	int nIW = m_pImageHITSLogo->GetWidth();
	int nIH = m_pImageHITSLogo->GetHeight();

	Rect rectLogo( rect.X, rect.Y, nIW, rect.Height );

	int nIX = (rectLogo.Width - nIW) / 2 + rectLogo.X;
	int nIY = (rectLogo.Height - nIH) / 2 + rectLogo.Y;

	DWORD dwFgColor = UIExt::CResourceManager::Instance()->GetBottomStatusBarForegroundColor();
	float fR = GetRValue(dwFgColor) / 255.f;
	float fG = GetGValue(dwFgColor) / 255.f;
	float fB = GetBValue(dwFgColor) / 255.f;
	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		fR, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, fG, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, fB, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.7f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	g.DrawImage( m_pImageHITSLogo, Rect(nIX, nIY, nIW, nIH), 0, 0, nIW, nIH, UnitPixel, &ImgAttr );
}

void CBottomStatusBar::OnGdipEraseBkgnd(Gdiplus::Graphics& g, Rect rect)
{
	DWORD dwBgColor = UIExt::CResourceManager::Instance()->GetBottomStatusBarBackgroundColor();
	g.FillRectangle( 
		&SolidBrush(Color(GetRValue(dwBgColor),GetGValue(dwBgColor),GetBValue(dwBgColor))), 
		rect );
}

void CBottomStatusBar::OnSize(UINT nType, int cx, int cy)
{
	CGdipBaseWnd::OnSize(nType, cx, cy);

	if(cx <= 0 || cy <= 0)
		return;

	const int nDisplayTimeWidth = 130;
	const int nIOWidth = 280;
	const int nIntervalX = 5;
	const int nMarginXY = 5;

	const int nCtrlHeight = cy-nMarginXY *2;
	const int nHeartBeatWidth = nCtrlHeight;

	int nRightOffset = cx - nMarginXY;
	m_wndDisplayTimeCtrl.MoveWindow( nRightOffset-nDisplayTimeWidth, nMarginXY, nDisplayTimeWidth, nCtrlHeight );
}

int CBottomStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(UIExt::CGdipBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndDisplayTimeCtrl.Create(this);

	return 0;
}
