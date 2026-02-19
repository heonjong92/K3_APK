// TransparentSliderCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TransparentSliderCtrl.h"

using namespace UIExt;
// CTransparentSliderCtrl

IMPLEMENT_DYNAMIC(CTransparentSliderCtrl, CSliderCtrl)

CTransparentSliderCtrl::CTransparentSliderCtrl()
{

}

CTransparentSliderCtrl::~CTransparentSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CTransparentSliderCtrl, CSliderCtrl)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CTransparentSliderCtrl::OnNMCustomdraw)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CTransparentSliderCtrl 메시지 처리기입니다.

void CTransparentSliderCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	CDC *pDC = CDC::FromHandle(pNMCD->hdc);
	switch (pNMCD->dwDrawStage)
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW ;
			break;
			//return;  
		case CDDS_ITEMPREPAINT:
		if (pNMCD->dwItemSpec == TBCD_THUMB)
		{
			*pResult = CDRF_DODEFAULT;
			break;
		}
		if (pNMCD->dwItemSpec == TBCD_CHANNEL)
		{
			*pResult = CDRF_DODEFAULT;
			break;
			/*
			CClientDC clientDC(GetParent());
			CRect crect;
			CRect wrect;
			GetClientRect(crect);
			GetWindowRect(wrect);
			GetParent()->ScreenToClient(wrect);
			if (m_dcBk.m_hDC == NULL)
			{
				m_dcBk.CreateCompatibleDC(&clientDC);
				m_bmpBk.CreateCompatibleBitmap(&clientDC, crect.Width(), crect.Height());
				m_bmpBkOld = m_dcBk.SelectObject(&m_bmpBk);
				m_dcBk.BitBlt(0, 0, crect.Width(), crect.Height(), &clientDC, wrect.left, wrect.top, SRCCOPY);
			}
			//This bit does the tics marks transparently.
			//create a memory dc to hold a copy of the oldbitmap data that includes the tics,
			//because when we add the background in we will lose the tic marks
			CDC SaveCDC;
			CBitmap SaveCBmp, maskBitmap;
			//set the colours for the monochrome mask bitmap
			COLORREF crOldBack = pDC->SetBkColor(RGB(0,0,0));
			COLORREF crOldText = pDC->SetTextColor(RGB(255,255,255));
			CDC maskDC;
			int iWidth = crect.Width();
			int iHeight = crect.Height();
			SaveCDC.CreateCompatibleDC(pDC);
			SaveCBmp.CreateCompatibleBitmap(&SaveCDC, iWidth, iHeight);
			CBitmap* SaveCBmpOld = (CBitmap *)SaveCDC.SelectObject(SaveCBmp);
			//fill in the memory dc for the mask
			maskDC.CreateCompatibleDC(&SaveCDC);
			//create a monochrome bitmap
			maskBitmap.CreateBitmap(iWidth, iHeight, 1, 1, NULL);
			//select the mask bitmap into the dc
			CBitmap* OldmaskBitmap = maskDC.SelectObject(&maskBitmap);
			//copy the oldbitmap data into the bitmap, this includes the tics.
			SaveCDC.BitBlt(0, 0, iWidth, iHeight, pDC, crect.left, crect.top, SRCCOPY);
			//now copy the background into the slider
			BitBlt(lpcd->hdc, 0, 0, iWidth, iHeight, m_dcBk.m_hDC, 0, 0, SRCCOPY);
			// Blit the mask based on background colour
			maskDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCCOPY);
			// Blit the image using the mask
			pDC->BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCINVERT);
			pDC->BitBlt(0, 0, iWidth, iHeight, &maskDC, 0, 0, SRCAND);
			pDC->BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCINVERT);
			//restore and clean up
			pDC->SetBkColor(crOldBack);
			pDC->SetTextColor(crOldText);
			DeleteObject(SelectObject(SaveCDC, SaveCBmpOld));
			DeleteDC(SaveCDC);
			DeleteObject(maskDC.SelectObject(OldmaskBitmap));
			DeleteDC(maskDC);
			*pResult = 0;
			break;
			*/
		}
	}
}


BOOL CTransparentSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
