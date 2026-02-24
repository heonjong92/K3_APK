
// APKView.h : CAPKView 클래스의 인터페이스
//

#pragma once

#include "TitleBar.h"
#include "MenuBar.h"
#include "BottomStatusBar.h"
#include "SideBar.h"

#include <XUtil/xThread.h>
#include <XImage/xImageObject.h>
#include <XImageView/xImageViewCtrl.h>

#include <XGraphic\xDataTypes.h>

#include "ShowErrorDlg.h"
#include "WaitMessageDlg.h"

#define WM_REFRESH_DIALOG		(WM_USER + 100)
#define WM_REFRESH_SYSTEM		(WM_USER + 300)

class CAPKDoc;
class CFormInspection;
class CFormTeaching;
class CFormDevice;
class CFormMaint;
class CDecoderCortex;

typedef void (APIENTRY *FnOnPointPositionTeached)( CPoint& pt, int nIndex, LPVOID lpUsrData );

class CAPKView : public CView
{
protected: // serialization에서만 만들어집니다.
	CAPKView();
	DECLARE_DYNCREATE(CAPKView)

// 특성입니다.
public:
	CAPKDoc* GetDocument() const;

	enum MenuStatus { MenuStatusInspection, MenuStatusTeaching, MenuStatusDeviceSettings };

// 변수 선언
protected:
	CTitleBar			m_wndTitleBar;
	CMenuBar			m_wndMenuBar;
	CBottomStatusBar	m_wndStatusBar;
	CSideBar			m_wndSideBar;

	CFormInspection*	m_pFormInspection;
	CFormTeaching*		m_pFormTeaching;
	CFormDevice*		m_pFormDevice;

	MenuStatus			m_MenuStatus;

	CxImageViewCtrl		m_wndAreaImageView[IDX_AREA_MAX];
	CxImageViewManager	m_ImageViewManagerArea;

	CShowErrorDlg		m_wndShowError;
	CWaitMessageDlg		m_wndWaitMessage;

	eViewMode			m_ViewMode;

	// Teach
	BOOL				m_bTrackerMode;

// 함수 선언
protected:
	void SwitchMenu( MenuStatus NewMenuStatus );
	CFormView* GetMenuForm( MenuStatus menu );

	void ArrangeWindows( int cx, int cy );		// 화면 구성
	void ShowViewMode( BOOL bShow );

	BOOL CreateViewModeInspect2D();

	static BOOL APIENTRY On2DImageViewEvent( ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData );
	static BOOL APIENTRY On2DImageViewMouseEvent( DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData );
	static void APIENTRY On2DImageViewDrawExt( IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData );

private:
	static CAPKView* m_pThis;

public:
	static CAPKView* GetPtr() { return m_pThis; }

	//////////////////////
	void ChangeViewMode(eViewMode viewMode);

	void UpdateAccessRight();
	void UpdateRecipeInformation(UINT nJobNumber);

	MenuStatus GetMenuStatus() const { return m_MenuStatus; }
	void OnChangeRunningStatus();

	void ShowError( BOOL bShow, LPCTSTR lpszTitle=_T(""), LPCTSTR lpszMessage=_T(""), LPCTSTR lpszDetail=_T("") );		// 에러 발생시 경고 메시지, 자동운전이 풀어지고 stop 상태로 변함 (뻘건색)
	void ShowWaitMessage( BOOL bShow, LPCTSTR lpszTitle=_T(""), LPCTSTR lpszMessage=_T("") );							// 창을 닫는 버튼 없음, 나중에 꼭 hide 해 줘야 함(퍼런색)

	void ForceStop();

	void UpdateImageView(BOOL bZoomFit = FALSE);
	void ResetGraphic();
	void ResetGraphic(eCamType tpCam, int nIdx);

	CxImageViewCtrl* GetImageView( eCamType tpCam, int nIndex = 0 );
	CxGraphicObject* GetGraphicObject( eCamType tpCam, int nIndex = 0 );

	void SetImageViewerResoultion();

	void SetTitleColor( int nViewIndex, BOOL bOK=TRUE, BOOL bDefault=TRUE );

	void UpdateUI();
	CString GetDesktopPath();

	// Teaching
	void SetTrackerMode( BOOL bTrackerMode, int nIndex=0, FnOnConfirmTracker fnOnConfirmTracker=NULL, LPVOID lpContext=NULL, CRect reTrackerArea=CRect(0,0,0,0) );
	
	void SetViewMode( eViewMode viewMode ) { m_ViewMode = viewMode; }
	eViewMode GetViewMode() { return m_ViewMode; }

	CxImageViewCtrl* GetImageView(int nIndex) { return &m_wndAreaImageView[nIndex]; }

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CAPKView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

	// 기본 메시지
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

	// 메인 메뉴 메시지
	afx_msg void OnMenuInspection();
	afx_msg void OnMenuTeaching();
	afx_msg void OnMenuDeviceSettings();
	afx_msg void OnMenuJobNumber();
	afx_msg void OnMenuAccessRights();
	afx_msg void OnMenuExit();
	afx_msg void OnMenuLanguage();

	afx_msg LRESULT OnLogMessage( WPARAM wParam, LPARAM lParam );

	// UI Update 관련
	afx_msg LRESULT OnModelChangeStatus( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateImageView( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateImageViewerResolution( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetObjectInfoReset(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetObjectInfoIndex(WPARAM wParam, LPARAM lParam);

	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // APKView.cpp의 디버그 버전
inline CAPKDoc* CAPKView::GetDocument() const
   { return reinterpret_cast<CAPKDoc*>(m_pDocument); }
#endif

