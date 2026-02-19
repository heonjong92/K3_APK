#pragma once

#include "UIExt/GdipWndExt.h"
#include <list>

// CSplashWnd

class CSplashWnd : public UIExt::CGdipBaseWnd
{
	DECLARE_DYNAMIC(CSplashWnd)

public:
	enum MessageType { MessageTypeNormal, MessageTypeError };

protected:
	Gdiplus::Image*	m_pImageBackground;
#ifdef _UNICODE
	typedef std::wstring MsgString;
#else
	typedef std::string MsgString;
#endif
	typedef std::list<MsgString>		Messages;
	typedef std::list<MessageType>		MessagesType;

	Messages		m_Messages;
	MessagesType	m_Types;
	//std::list<MessageType>	m_Types;

	int m_nProgressStep;
	int m_nProgressMax;

	CSplashWnd();
public:
	virtual ~CSplashWnd();
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static void HideSplashScreen( int nDelay );
	static void ShowMessage(MessageType type, LPCTSTR lpszFormat, ...);
	static void SetProgress(int nStep, int nMax);
	

protected:
	virtual void OnDraw( Graphics& g, Rect rect ) override;
	virtual void OnGdipEraseBkgnd( Graphics& g, Rect rect ) override;

	BOOL Create(CWnd* pParentWnd);

	void DrawProgress( Graphics& g, Rect& rectProgress );

	static CSplashWnd*	m_pSplashWnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


