// Screen.cpp: implementation of the CScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Screen.h"
#include "../TRIPK.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CScreen::CScreen()
{
}

CScreen::~CScreen()
{
}

void CScreen::SetCapturePath( CWnd *wnd, CString strPath, CString strPanelID )
{
#ifdef _DEBUG
	return;
#endif

	CTime time(CTime::GetCurrentTime());
	CString strDay, strTime;
	strDay.Format(_T("%04d%02d%02d"), time.GetYear(), time.GetMonth(), time.GetDay() );
	strTime.Format(_T("%02d%02d%02d"), time.GetHour(), time.GetMinute(), time.GetSecond() );
		
	MakeDirectory(strPath);

	CString strDirPath;
	strDirPath.Format(_T("%s%s\\"), strPath, strDay );
	MakeDirectory(strDirPath);

	CString strFileName;
	strFileName.Format(_T("%s_%s.jpg"), strTime, strPanelID );

	ScreenCapture(wnd, (strDirPath + strFileName));
}

void CScreen::ScreenCapture(CWnd *wnd, CString iname)
{
	BOOL bTest = TRUE;

	CClientDC dc( wnd->GetDesktopWindow() );	
    CDC memDC;
    memDC.CreateCompatibleDC( &dc );

    int width = GetSystemMetrics( SM_CXSCREEN );;
    int height = GetSystemMetrics( SM_CYSCREEN );;
	
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap( &dc, width, height );
	
    memDC.SelectObject( &bitmap );                    
    memDC.BitBlt( 0, 0, width, height, &dc, 0, 0, SRCCOPY ); 
	
    BITMAP bm;
    bitmap.GetBitmap( &bm );
	
	if( &bm == NULL )	return;
	
    DWORD dwCount = bm.bmWidthBytes * bm.bmHeight;
    BYTE* pBuff   = new BYTE[dwCount];
	
	DWORD dwRead  = bitmap.GetBitmapBits( dwCount, pBuff );
	
	EImageC24A m_ImageSrc;
	EImageC24 m_ImageDest;

	m_ImageSrc.SetSize(width, height);
	m_ImageDest.SetSize(width, height);
	m_ImageSrc.SetImagePtr(width, height, pBuff, 0);
	ImgConvert(&m_ImageSrc, &m_ImageDest);
	USES_CONVERSION;
	m_ImageDest.Save( T2A((LPTSTR)(LPCTSTR)iname) );

	delete[] pBuff;
		
	memDC.DeleteDC();
	bitmap.DeleteObject();
}