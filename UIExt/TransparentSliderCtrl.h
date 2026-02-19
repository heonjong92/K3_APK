#pragma once

// CTransparentSliderCtrl

namespace UIExt {

class CTransparentSliderCtrl : public CSliderCtrl
{
	DECLARE_DYNAMIC(CTransparentSliderCtrl)

public:
	CTransparentSliderCtrl();
	virtual ~CTransparentSliderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

} // namespace UIExt