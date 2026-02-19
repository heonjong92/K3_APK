#include "stdafx.h"
#include "CustomToolTip.h"

BEGIN_MESSAGE_MAP(CImageToolTip, CWnd)
	ON_WM_PAINT()
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CImageToolTip::CImageToolTip()
{
	m_pParent = nullptr;
}

CImageToolTip::~CImageToolTip()
{
}

BOOL CImageToolTip::Create(CWnd* pParentWnd)
{
	m_pParent = pParentWnd;
	CString strClass = AfxRegisterWndClass(CS_SAVEBITS, ::LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_INFOBK + 1), nullptr);
	return CWnd::CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, strClass, nullptr,
		WS_POPUP, CRect(0, 0, 0, 0), pParentWnd, 0);
}

void CImageToolTip::SetImage(LPCTSTR lpszPath)
{
	m_img.Destroy();
	m_img.Load(lpszPath);
}

void CImageToolTip::SetText(const CString& strText)
{
	m_strText = strText;
}

void CImageToolTip::ShowTip(CPoint pt)
{
	if (m_img.IsNull())
		return;

	int width = m_img.GetWidth() + 10;
	int height = m_img.GetHeight() + 10;

	pt.x = 10;  // 모니터 좌상단
	pt.y = 10;

	CRect rc(pt.x, pt.y, pt.x + width, pt.y + height);
	MoveWindow(rc);
	ShowWindow(SW_SHOW); // 그리기는 OnPaint에서

//	SetFocus();
}

void CImageToolTip::HideTip()
{
	ShowWindow(SW_HIDE);
}

void CImageToolTip::OnPaint()
{
	CPaintDC dc(this); // 안전하게 OnPaint에서만 DC 사용
	CRect rc;
	GetClientRect(&rc);

	const int nRadius = 10;
	CBrush brush(RGB(255, 255, 230));
	CBrush* pOldBrush = dc.SelectObject(&brush);
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = dc.SelectObject(&pen);

	dc.RoundRect(rc, CPoint(nRadius, nRadius));

	if (!m_img.IsNull())
		m_img.Draw(dc, CPoint(5, 5));

	// 텍스트 영역 계산
//	rc.top += m_img.IsNull() ? 5 : (m_img.GetHeight() + 10);
//	dc.SetTextColor(RGB(0, 0, 80));
//	dc.SetBkMode(TRANSPARENT);
//	dc.DrawText(m_strText, rc, DT_LEFT | DT_WORDBREAK);

	// 객체 복원
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

//void CImageToolTip::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	if (nChar == VK_F4)
//	{
//		if (IsWindowVisible()) // 툴팁이 떠 있을 때만 무시
//			return; // 메시지 소모 → Alt+F4 무시
//	}
//
//	if (nChar == VK_ESCAPE)
//	{
//		HideTip();
//	}
//
//	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
//}