// SystemStatusCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../resource.h"
#include "SystemStatusCtrl.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace UIExt;

WCHAR* g_wszTitleText[2] = { L"AUTO RUN", L"STOP" };

using namespace Gdiplus;
// CSystemStatusCtrl

IMPLEMENT_DYNAMIC(CSystemStatusCtrl, CGdipStatic)

CSystemStatusCtrl::CSystemStatusCtrl()
{
	m_colorBody[0] = Color(60,157,38);
	m_colorBody[1] = Color(196,36,52);

	//m_pImageIcon = GdipLoadImageFromRes(AfxGetResourceHandle(), _T("PNG"), PNG_ICON_SYSTEM_STATUS);
	m_pImageIcon = NULL;
	m_Status = StatusStop;
	if (m_pImageIcon)
	{
		m_sizeIcon.cy = m_pImageIcon->GetHeight();
		m_sizeIcon.cx = m_pImageIcon->GetWidth() / 4;
		ASSERT( (m_pImageIcon->GetWidth() / m_pImageIcon->GetHeight()) == 4 );
	}
	else
	{
		m_sizeIcon.cx = m_sizeIcon.cy = 0;
	}
}

CSystemStatusCtrl::~CSystemStatusCtrl()
{
	if (m_pImageIcon)
	{
		delete m_pImageIcon;
		m_pImageIcon = NULL;
	}
}


BEGIN_MESSAGE_MAP(CSystemStatusCtrl, CGdipStatic)
END_MESSAGE_MAP()

void CSystemStatusCtrl::SetStatus( Status status )
{
	m_Status = status;

	if (GetSafeHwnd())
		Invalidate();
}

void CSystemStatusCtrl::OnDraw(Gdiplus::Graphics& g, Gdiplus::Rect rect)
{
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintAntiAlias);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	Rect rectBody = rect;

	//rectBody.Inflate(-2, -2);
	//Pen penBorder(Color(100,0,0,0), 3);
	//g.DrawRectangle( &penBorder, rectBody );

	if (m_pImageIcon)
	{
		int nIconOffset = (rectBody.Height - m_sizeIcon.cy) / 2;

		Rect rectIcon;
		rectIcon.X = rectBody.X + nIconOffset;
		rectIcon.Y = rectBody.Y + nIconOffset;
		rectIcon.Width = m_sizeIcon.cx;
		rectIcon.Height = m_sizeIcon.cy;

		ImageAttributes ImgAttr;
		ColorMatrix ClrMatrix = 
		{ 
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.7f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
			ColorAdjustTypeBitmap);

		g.DrawImage(m_pImageIcon, rectIcon, m_sizeIcon.cx*m_Status, 0, m_sizeIcon.cx, m_sizeIcon.cy, UnitPixel, &ImgAttr);
	}

	RectF rectTextF;
	rectTextF.X = (float)rectBody.X;
	rectTextF.Y = (float)rectBody.Y;
	rectTextF.Width = (float)rectBody.Width;
	rectTextF.Height = (float)rectBody.Height;

	WCHAR* wszTitle = g_wszTitleText[m_Status];
	Color colorText(220, 255,255,255);
	Color colorShadowText(100, 0,0,0);
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	Gdiplus::Font fontText(
		CResourceManager::Instance()->GetSystemFontFamily(), 
		rectTextF.Height*0.41f, FontStyleBold, UnitPixel);

	rectTextF.X += 2.f;
	rectTextF.Y += 2.f;
	g.DrawString( wszTitle, -1, &fontText, rectTextF, &stringFormat, &SolidBrush(colorShadowText) );
	rectTextF.X -= 2.f;
	rectTextF.Y -= 2.f;
	g.DrawString( wszTitle, -1, &fontText, rectTextF, &stringFormat, &SolidBrush(colorText) );
}

void CSystemStatusCtrl::OnGdipEraseBkgnd(Gdiplus::Graphics& g, Gdiplus::Rect rect)
{
	Color colorBody = m_colorBody[m_Status];
	SolidBrush brushBody(colorBody);
	g.FillRectangle( &brushBody, rect );
}

void CSystemStatusCtrl::PreSubclassWindow()
{
	ModifyStyleEx( WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );

	CGdipStatic::PreSubclassWindow();
}
