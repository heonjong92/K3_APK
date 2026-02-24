// DisplayTimeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../resource.h"
#include "DisplayTimeCtrl.h"
#include "GdiplusExt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;
using namespace Gdiplus;
// CDisplayTimeCtrl

IMPLEMENT_DYNAMIC(CDisplayTimeCtrl, CGdipBaseWnd)

CDisplayTimeCtrl::CDisplayTimeCtrl()
{
	m_pIconTime = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_ICON_TIME );
}

CDisplayTimeCtrl::~CDisplayTimeCtrl()
{
	if (m_pIconTime)
	{
		delete m_pIconTime;
		m_pIconTime = NULL;
	}
}


BEGIN_MESSAGE_MAP(CDisplayTimeCtrl, CGdipBaseWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDisplayTimeCtrl::Create(CWnd* pParentWnd/*= NULL*/)
{
	return CGdipBaseWnd::Create(NULL, _T("DisplayTimeCtrl"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), pParentWnd, -1);
}

void CDisplayTimeCtrl::OnDraw(Graphics& g, Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	CTime time = CTime::GetCurrentTime();

	CString strTime = time.Format(_T("%H:%M:%S"));
	CString strDate = time.Format(_T("%Y-%m-%d"));
	CString strDT;
	strDT.Format(_T("%s\n%s"), strTime.GetString(), strDate.GetString());

	BSTR bstrDT = strDT.AllocSysString();

	FontFamily fontFamily(L"Verdana");
	Gdiplus::Font fontTime(&fontFamily, 17, FontStyleRegular, UnitPixel);
	Gdiplus::Font fontDate(&fontFamily, 12, FontStyleRegular, UnitPixel);

	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit);
	stringFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	RectF rectText((float)rect.X, (float)rect.Y, (float)rect.Width, (float)rect.Height);

	RectF rectBB;
	g.MeasureString(bstrDT, -1, &fontTime, rectText, &stringFormat, &rectBB);

	int nIW = m_pIconTime->GetWidth();
	int nIH = m_pIconTime->GetHeight();

	rectText.X += nIW * 0.85f / 2.f;

	RectF rectIcon(rectBB.X - nIW * 0.85f / 2, rectBB.Y + (rectBB.Height - nIH) / 2, (float)nIW, (float)nIH);

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix =
	{
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.6f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

	g.DrawImage(m_pIconTime,
		Rect((int)rectIcon.X, (int)rectIcon.Y, nIW, nIH),
		0, 0, nIW, nIH, UnitPixel, &ImgAttr);

	// ---- (A) 날짜를 시간보다 작게: 두 번 그리기 ----
	// 시간/날짜 각각의 높이를 측정해서 세로 중앙 정렬
	BSTR bstrTime = strTime.AllocSysString();
	BSTR bstrDate = strDate.AllocSysString();

	RectF bbTime, bbDate;
	g.MeasureString(bstrTime, -1, &fontTime, rectText, &stringFormat, &bbTime);
	g.MeasureString(bstrDate, -1, &fontDate, rectText, &stringFormat, &bbDate);

	float totalH = bbTime.Height + bbDate.Height;
	float startY = rectText.Y + (rectText.Height - totalH) / 2.0f;

	// 시간
	RectF rtTime(rectText.X, startY, rectText.Width, bbTime.Height);
	StringFormat fmtCenter;
	fmtCenter.SetAlignment(StringAlignmentCenter);
	fmtCenter.SetLineAlignment(StringAlignmentCenter);
	fmtCenter.SetFormatFlags(StringFormatFlagsLineLimit | StringFormatFlagsNoWrap);

	g.DrawString(bstrTime, -1, &fontTime, rtTime, &fmtCenter, &SolidBrush(Color(180, 255, 255, 255)));

	// 날짜
	RectF rtDate(rectText.X, startY + bbTime.Height, rectText.Width, bbDate.Height);
	g.DrawString(bstrDate, -1, &fontDate, rtDate, &fmtCenter, &SolidBrush(Color(140, 255, 255, 255)));

	SysFreeString(bstrTime);
	SysFreeString(bstrDate);

	SysFreeString(bstrDT);
}

void CDisplayTimeCtrl::OnGdipEraseBkgnd(Graphics& g, Rect rect)
{
	g.FillRectangle( &SolidBrush(Color(67,72,78)), rect );
}


void CDisplayTimeCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 )
		Invalidate();

	CGdipBaseWnd::OnTimer(nIDEvent);
}


int CDisplayTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGdipBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1, 500, NULL);

	return 0;
}


void CDisplayTimeCtrl::OnDestroy()
{
	CGdipBaseWnd::OnDestroy();

	KillTimer(1);
}
