#pragma once

#include "UIExt/SystemStatusCtrl.h"
#include "UIExt/FlatTabWnd.h"
#include "UIExt/FlatButton.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/OutputListBox.h"
#include "InspTabLog.h"
#include "InspTabObj.h"

#include <XImage/xImageObject.h>
#include <XImageView/xImageViewCtrl.h>


#define STATIC_CUR_DEFECT_LIST_PAGE_NO (WM_USER+0x0201)
enum RESULT_BUTTON_ID { RESULT_BUTTON_FIRST = 6666, RESULT_BUTTON_END, RESULT_BUTTON_PAGEUP, RESULT_BUTTON_PAGEDOWN };
#define btStyle		WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON

class CAPKView;
class CFormInspection : public CFormView//, public CSystemConfig::INotifyChangeAccessRight
{
	DECLARE_DYNCREATE(CFormInspection)

protected:
	CFormInspection();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormInspection();

public:
	enum { IDD = IDD_INSPECTION };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CAPKView*	m_pMainView;

	UIExt::CFlatTabWnd m_wndTabInsp;
	UIExt::CSystemStatusCtrl m_wndSystemStatusCtrl;

	UIExt::CFlatLabel m_wndLabelText;

	UIExt::CFlatButton m_btnAutoRun;
	UIExt::CFlatButton m_btnStop;
	UIExt::CFlatButton m_btnGraphicReset;

	CArray<CString, LPCTSTR> m_InitLogMessages;
	CArray<eLogType, eLogType> m_InitLogMessageTypes;

	CInspTabLog m_wndModeTabLog;
	CInspTabObj m_wndModeTabObj;

public:
	void SetMainView( CAPKView* pView );

	void AddLogMessage(LPCTSTR lpszLog, eLogType type);

	void ForceStop();
	void UpdateLanguage();
	void SetObjectInfoReset();
	void SetObjectInfoIndex(int nCol, int nRow);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

public:
	afx_msg void OnBnClickedBtnAutorun();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnGraphicReset();
	afx_msg void OnStnClickedModeTab();
};

