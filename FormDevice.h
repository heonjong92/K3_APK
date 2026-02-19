#pragma once
#include "UIExt/FlatTabWnd.h"
#include "UIExt/FlatButton.h"
#include "DevTabSystem.h"
#include "DevTabCameraLight.h"

class CAPKView;
class CFormDevice : public CFormView//, public CSystemConfig::INotifyChangeAccessRight
{
	DECLARE_DYNCREATE(CFormDevice)

protected:
	CFormDevice();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormDevice();
public:
	enum { IDD = IDD_DEVICE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CAPKView*	m_pMainView;

	int	m_nSelectRecipeIndex;

	UIExt::CFlatTabWnd	m_wndTabDevSetting;
	UIExt::CFlatButton	m_btnSave;

	CDevTabSystem m_wndTabSystem;
	CDevTabCameraLight m_wndTabCameraLight;

	CComboBox	m_wndSelectRecipe;

public:
	void SetMainView( CAPKView* pView );
	void UpdateLanguage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnFlatTabWndSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshDialog(WPARAM wparam = NULL, LPARAM lparam = NULL);
	afx_msg LRESULT OnRefreshSystem(WPARAM wparam = NULL, LPARAM lparam = NULL);

public:
	afx_msg void OnBnClickedBtnSave();
	//BOOL PreTranslateMessage(MSG* msg) override;
};

