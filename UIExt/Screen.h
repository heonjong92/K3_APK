// Screen.h: interface for the CScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREEN_H__6CD94CC3_EA33_4735_AC5D_28B5EA713651__INCLUDED_)
#define AFX_SCREEN_H__6CD94CC3_EA33_4735_AC5D_28B5EA713651__INCLUDED_

#include <Afxwin.h>
#include <EImage.h>

#define HDIB HANDLE 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScreen  
{
public:
	void SetCapturePath( CWnd *wnd, CString strPath, CString strPanelID  );
	void ScreenCapture(CWnd *wnd,CString iname);

	CScreen();
	virtual ~CScreen();
};

#endif // !defined(AFX_SCREEN_H__6CD94CC3_EA33_4735_AC5D_28B5EA713651__INCLUDED_)
