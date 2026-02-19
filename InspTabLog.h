#pragma once
#include "UIExt/IOStatusCtrlEx.h"
#include "UIExt/FlatLabel.h"
#include "UIExt/OutputListBox.h"
#include "UIExt/ResourceManager.h"

// CInspTabLog 대화 상자입니다.
class CAPKView;
class CInspTabLog : public CDialogEx
{
	DECLARE_DYNAMIC(CInspTabLog)

public:
	CInspTabLog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspTabLog();
	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSP_TAB_LOG };

protected:
	CAPKView*	m_pMainView;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	UIExt::COutputListBox m_wndListResult;

	CArray<CString, LPCTSTR> m_InitLogMessages;
	CArray<eLogType, eLogType> m_InitLogMessageTypes;
	CArray<SYSTEMTIME, SYSTEMTIME> m_InitLogSysTimes;

	CBrush			m_BrushBody;
	COLORREF		m_dwBodyColor;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }

	void AddLogMessage(LPCTSTR lpszLog, eLogType type);
	BOOL PreTranslateMessage(MSG* msg) override;
};
