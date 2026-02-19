
// APK.h : APK 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include <xUtil/Log/xLogSystem.h>

// CAPKApp:
// 이 클래스의 구현에 대해서는 APK.cpp을 참조하십시오.
//


//////////////////////////////////////////////////////////////////////////
///// Code 동글 없이 실행할 경우는 define문 주석 풀어줌  (원복 : 주석)
///// 4G, 6G 이외에 모든 그룹은 define문 주석 풀어줌
//////////////////////////////////////////////////////////////////////////
#if defined(RELEASE_4G) || defined(RELEASE_6G)
//	#define _CodeUsb
#else
	#define _CodeUsb
#endif

class CAPKApp : public CWinApp
{
public:
	CAPKApp();

protected:
	HANDLE m_hMutexOneInstance;
	CxLogSystem	m_LogSystem;

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void ExitProgram();

	CxLogSystem& GetLogSystem() { return m_LogSystem; }

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAPKApp theApp;
extern CEvent g_eventForWatch;

extern long GetRoundLong( double dbScr );
extern short GetRoundShort(double dbScr);
extern float GetRoundFloat( double dbScr );
extern LPCTSTR GetImageViewerTitle(eCamType nCamType, UINT nIndex);
extern void MakeDirectory(LPCTSTR lpszDir);
extern LPCTSTR ForLog( InspectType inspecttype );

CxLogSystem& GetLogSystem();
