#pragma once
#include "UIExt/FlatTabWnd.h"
#include "UIExt/FlatButton.h"

#include "TeachTab1GTrayOcr.h"
#include "TeachTab1G3DChipCnt.h"
#include "TeachTab1GChipOcr.h"
#include "TeachTab1GChip.h"
#include "TeachTab1GMixing.h"
#include "TeachTab1GLift.h"
#include "TeachTabSGStackerOcr.h"
#include "TeachTab2GBanding.h"
#include "TeachTab2GHIC.h"
#include "TeachTab3GDesiccantCutting.h"
#include "TeachTab3GDesiccantMaterial.h"
#include "TeachTab4GMBB.h"
#include "TeachTab5GBoxQuality.h"
#include "TeachTab5GSealingQuality.h"
#include "TeachTab6GBoxTape.h"
#include "TeachTabLabel.h"

#include "AccessRightsDlg.h"

class CAPKView;
class CFormTeaching : public CFormView//, public CSystemConfig::INotifyChangeAccessRight
{
	DECLARE_DYNCREATE(CFormTeaching)

protected:
	CFormTeaching();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormTeaching();
public:
	enum { IDD = IDD_TEACHING };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CAPKView*	m_pMainView;

	UIExt::CFlatTabWnd	m_wndTabTeaching;

	CTeachTab1GTrayOcr					m_wndTab1G_TrayOcr;
	CTeachTab1G3DChipCnt 				m_wndTab1G_3DChipCnt;
	CTeachTab1GChipOcr					m_wndTab1G_ChipOcr;
	CTeachTab1GChip						m_wndTab1G_Chip;
	CTeachTab1GMixing					m_wndTab1G_Mixing;
	CTeachTab1GLift						m_wndTab1G_Lift;
	CTeachTabSGStackerOcr				m_wndTabSG_StackerOcr;
	CTeachTab2GBanding					m_wndTab2G_Banding;
	CTeachTab2GHIC						m_wndTab2G_HIC;
	CTeachTab3GDesiccantCutting			m_wndTab3G_Desiccant;
	CTeachTab3GDesiccantMaterial		m_wndTab3G_Material;
	CTeachTab4GMBB						m_wndTab4G_MBB;
	CTeachTab5GBoxQuality				m_wndTab5G_BoxQuality;
	CTeachTab5GSealingQuality			m_wndTab5G_SealingQuality;
	CTeachTab6GBoxTape					m_wndTab6G_BoxTape;
	CTeachTabLabel						m_wndTabLabel;

public:
	void SetMainView( CAPKView* pView );

	void UpdateLanguage();
	void UpdateUI( BOOL bAll=FALSE );

	void UpdateRecipeList();
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
	afx_msg LRESULT OnRefreshDialog(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnRefresh3DChipCnt(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnRefreshLift(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnRefreshChip(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnRefreshMBB(WPARAM wparam, LPARAM lparam);
	afx_msg void OnCbnSelchangeComboRecipe();
	afx_msg void OnBnClickedBtnSave();
};

