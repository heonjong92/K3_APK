#pragma once
#include "UIExt/ObjectInfoCtrlEx.h"
#include "ModelInfo.h"

// CInspTabObj 대화 상자입니다.
class CAPKView;
class CInspTabObj : public CDialogEx
{
	DECLARE_DYNAMIC(CInspTabObj)

public:
	CInspTabObj(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspTabObj();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSP_TAB_OBJ };

private:
	static CInspTabObj*	m_pTabObject;

public:
	static CInspTabObj* GetPtr() { return m_pTabObject; }

protected:
	CAPKView*	m_pMainView;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	UIExt::CObjectInfoCtrlEx	m_wndObjectInfoCtrl;
	void ArrangeWindows( int cx, int cy );		// 이미지 뷰 정렬
	void SetObjectInfoIndexOneshot( int nCol, int nRow );

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnLButtonDownOnObjectTool( WPARAM wParam, LPARAM lParam );

	int m_nObjectMapRow, m_nObjectMapCol;
	int m_nObjectMapLineRow, m_nObjectMapLineCol;

public:		
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);  

	void SetMainView( CAPKView* pView ) { m_pMainView = pView; }

	void UpdateUI();
	void Refresh();

	void SetObjectInfoIndex( int nCol, int nRow );
	void SetObjectInfoResult( int nCol, int nRow, int nResult );
	void SetObjectInfoReset();

	void SetObjectTabMatrix() { Refresh(); };
};
