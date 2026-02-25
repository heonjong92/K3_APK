// TeachTabGrind.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachThresholdPreview.h"
#include "afxdialogex.h"

#include "APKView.h"
#include "InspectionVision.h"
#include "LanguageInfo.h"
#include "ModelInfo.h"
#include "TeachTab1GTrayOcr.h"
#include "VisionSystem.h"

#include "UIExt/ResourceManager.h"
#include <XGraphic/xGraphicObject.h>

// CTeachThresholdPreview 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachThresholdPreview, CDialog)

CTeachThresholdPreview::CTeachThresholdPreview(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachThresholdPreview::IDD, pParent)
{
	m_clrBorder = RGB(126, 27, 29);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("Threshold Setting");

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_INSP_LOGO );
}

CTeachThresholdPreview::~CTeachThresholdPreview()
{
	if (m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = NULL;
	}
}

void CTeachThresholdPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnExit);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnSave);
	DDX_Text(pDX,	IDC_EDIT_THRESHOLD, m_stChipOcr.nThreshold );
	DDV_MinMaxInt(pDX, m_stChipOcr.nThreshold, 0, 255);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD, m_wndSliderPadThreshold);
}

BEGIN_MESSAGE_MAP(CTeachThresholdPreview, CDialog)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_BTN_SAVE, &CTeachThresholdPreview::OnBnClickedBtnSave)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDCANCEL, &CTeachThresholdPreview::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_THRESHOLD, &CTeachThresholdPreview::OnEnChangeEditPadGain)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CTeachThresholdPreview 메시지 처리기입니다.
BOOL CTeachThresholdPreview::OnInitDialog()
{
	CDialog::OnInitDialog();

	// BTN INITIAL
	DWORD dNormal = RGB(120, 120, 120);
	DWORD dActive = RGB(60,157,38);
	dNormal = RGB(175, 37, 40);
	InitButton(&m_btnExit, dNormal, dActive);

	m_wndSliderPadThreshold.SetRange( 0, 255 );

	CreateViewModeTeachMapping();

	UpdateLanguage();

	return TRUE;
}

void CTeachThresholdPreview::UpdateLanguage()
{
}

BOOL CTeachThresholdPreview::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_SPACE || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return 0;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTeachThresholdPreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	using namespace Gdiplus;

	CRect rcClient;
	GetClientRect( rcClient );

	CRect rcSplit2;
	GetDlgItem(IDC_SPLIT2)->GetWindowRect(rcSplit2);
	ScreenToClient(rcSplit2);

	int nTopOffset = 36;
	int nBorderWidth = m_nBorderWidth;

	CRect rcTitle = rcClient;
	rcTitle.bottom = rcTitle.top + nTopOffset;

	dc.FillSolidRect( rcClient.left, rcClient.top, rcClient.Width(), rcSplit2.top-rcClient.top, m_clrBody );

	dc.FillSolidRect( rcTitle, m_clrBorder );

	dc.FillSolidRect( rcClient.left, rcClient.bottom-nBorderWidth, rcClient.Width(), nBorderWidth, m_clrBorder );
	dc.FillSolidRect( rcClient.left, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );
	dc.FillSolidRect( rcClient.right-nBorderWidth, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );

	//dc.FillSolidRect( rcClient.left, rcSplit2.top, rcClient.Width(), nBorderWidth, m_clrBorder );

	const int nLeftOffsetX = 8;

	Graphics g(dc.GetSafeHdc());

	int nIW = m_pIconImage->GetWidth();
	int nIH = m_pIconImage->GetHeight();
	int nIX = rcTitle.left + nLeftOffsetX;
	int nIY = rcTitle.top + (rcTitle.Height() - nIH) / 2;

	ImageAttributes ImgAttr;
	ColorMatrix ClrMatrix = 
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.7f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);

	g.DrawImage( m_pIconImage, Rect(nIX, nIY, nIW, nIH), 0, 0, nIW, nIH, UnitPixel, &ImgAttr );

	Color colorText(180, 0,0,0);
	SolidBrush brushText( colorText );
	StringFormat stringFormat;
	stringFormat.SetAlignment( StringAlignmentCenter );
	stringFormat.SetLineAlignment( StringAlignmentCenter );
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit|StringFormatFlagsNoWrap);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter );

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	stringFormat.SetAlignment( StringAlignmentNear );
	SolidBrush brushTitle(Color(200,255,255,255));
	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();
	Gdiplus::Font fontTitle(pFontFamily, 15, FontStyleBold, UnitPixel);
	BSTR bstrTitle = m_strTitle.AllocSysString();
	g.DrawString( bstrTitle, -1, &fontTitle, RectF((float)rcTitle.left+nIX+nIW+3, (float)rcTitle.top, (float)rcTitle.Width()-8, (float)rcTitle.Height()), &stringFormat, &brushTitle );
	SysFreeString( bstrTitle );
}

LRESULT CTeachThresholdPreview::OnNcHitTest(CPoint point)
{
	int nTopOffset = 36;
	CRect rc;
	GetClientRect(rc);
	ClientToScreen(rc);
	rc.bottom = rc.top + nTopOffset;
	if (rc.PtInRect(point))
		return HTCAPTION;

	return CDialog::OnNcHitTest(point);
}

void CTeachThresholdPreview::InitButton(UIExt::CFlatButton *pBtn, DWORD dNormal, DWORD dActive)
{
	pBtn->SetColor(dNormal, dNormal, dActive, RGB(180, 180, 180), RGB(255, 255, 255));
	pBtn->SetFontHeight( 12 );
	pBtn->SetBold(TRUE);
}

BOOL CTeachThresholdPreview::CreateViewModeTeachMapping()
{
	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor     = UIExt::CResourceManager::Instance()->GetViewerBodyColor();

	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = 
		regCB.lpUsrData[2] = regCB.lpUsrData[3] = 
		regCB.lpUsrData[4] = this;

	for(int i = 0; i < IDX_2D_MAPPING_MAX; i++)
	{
		m_wndMappingImageView[i].Create(&m_ImageViewManagerMappingTeach, this);
		m_wndMappingImageView[i].ShowWindow(SW_SHOW);
		m_wndMappingImageView[i].SetAnimateWindow(FALSE);
		m_wndMappingImageView[i].SetImageObject(&m_ImageObjectMappingTeach[i]);
		m_wndMappingImageView[i].SetRegisterCallBack(i, &regCB);

		m_wndMappingImageView[i].SetStatusColor(dwStatusBgColor, dwStatusFgColor);
		m_wndMappingImageView[i].SetBodyColor(dwBodyColor);

		m_wndMappingImageView[i].ShowTitleWindow(FALSE);
		m_wndMappingImageView[i].ShowStatusWindow(FALSE);
		m_wndMappingImageView[i].ShowScaleBar(FALSE);
		m_wndMappingImageView[i].ShowScrollBar(TRUE);
		m_wndMappingImageView[i].EnableMouseControl(TRUE);

		m_wndMappingImageView[i].SetSyncManager(&m_SyncManager);

		LockWindowUpdate();

		CRect rc;
		switch( i )
		{
		case IDX_2D_MAPPING1:	GetDlgItem(IDC_PICTURE_PREVIEW)->GetWindowRect(rc);						break;
		case IDX_2D_MAPPING2:	GetDlgItem(IDC_PICTURE_PREVIEW_THRESHOLD)->GetWindowRect(rc);			break;
		}
		ScreenToClient(rc);

		HDWP hDWP = BeginDeferWindowPos(IDX_2D_MAPPING_MAX);	
		hDWP = DeferWindowPos( hDWP, m_wndMappingImageView[i].GetSafeHwnd(), NULL, rc.left, rc.top, rc.Width(), rc.Height(), 0 );
		EndDeferWindowPos(hDWP);

		UnlockWindowUpdate();
	}

	return TRUE;
}

void CTeachThresholdPreview::SetTrackerMode( BOOL bTrackerMode, int nIndex/*=0*/, FnOnConfirmTracker fnOnConfirmTracker/*=NULL*/, LPVOID lpContext/*=NULL*/ )
{
	if (bTrackerMode)
	{
		m_wndMappingImageView[nIndex].SetTrackerMode(TRUE);
		m_wndMappingImageView[nIndex].SetOnConfirmTracker( fnOnConfirmTracker, lpContext );
	}
	else
	{
		for( int i=0; i<IDX_2D_MAPPING_MAX; ++i )
		{
			m_wndMappingImageView[i].SetTrackerMode(FALSE);
			m_wndMappingImageView[i].SetOnConfirmTracker( NULL, NULL );
		}
	}
}

void CTeachThresholdPreview::OnBnClickedBtnSave()
{
#ifdef RELEASE_1G
	WRITE_LOG( WL_BTN, _T("CTeachThresholdPreview::OnBnClickedBtnSave") );

	UpdateData(TRUE);

	CString strMessage = _T("Do you want Save?");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strMessage = _T("저장 하시겟습니까?");

	if (IDYES != AfxMessageBox(strMessage, MB_YESNO))
		return;

	CModelInfo::stChipOcr& ChipOcr = CModelInfo::Instance()->GetChipOcr();

	CString strLog;
	strLog.Format(_T("[Threshold][%d→%d]"), ChipOcr.nThreshold, m_stChipOcr.nThreshold);
	if (ChipOcr.nThreshold != m_stChipOcr.nThreshold) CVisionSystem::Instance()->WriteLogforTeaching(InspectTypeChipOcr, strLog);

	Save();

//	m_pTeachTabView->Refresh();
	CDialog::OnOK();

#endif // RELEASE_1G
}

void CTeachThresholdPreview::Save()
{
	UpdateData(TRUE);

	CModelInfo::stChipOcr& stChipOcr = CModelInfo::Instance()->GetChipOcr();
	stChipOcr = m_stChipOcr;

#ifdef RELEASE_1G
	CModelInfo::Instance()->Save( TEACH_TAB_IDX_CHIPOCR );

#endif // RELEASE_1G
//	Refresh();

	UpdateData(FALSE);

	return;
}

void CTeachThresholdPreview::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		Cleanup();
		Refresh();
	}
}

void CTeachThresholdPreview::Refresh()
{
	CModelInfo::stChipOcr& stChipOcr = CModelInfo::Instance()->GetChipOcr();
	m_stChipOcr = stChipOcr;

	m_wndSliderPadThreshold.SetPos( stChipOcr.nThreshold );

	UpdateUI();

	UpdateData(FALSE);

	DisableWnd();
}

void CTeachThresholdPreview::Cleanup()
{
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
}

void CTeachThresholdPreview::DisableWnd()
{
	if(CVisionSystem::Instance()->GetRunStatus() == RunStatusStop)
		return;
	
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow( FALSE );
}

void CTeachThresholdPreview::UpdateUI()
{
	UpdatePreview();
	
	UpdateLanguage();

	UpdateData(FALSE);
}

void CTeachThresholdPreview::UpdatePreview()
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
	{
		CVisionSystem::Instance()->WriteMessage(LogTypeError, _T("eVision Dongle USB Open.. Failure (CTeachThresholdPreview::UpdatePreview)"));
		return;
	}

#ifdef RELEASE_1G
	CxImageObject* pImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, IDX_AREA4);

	if (pImgObj == NULL || pImgObj->GetImageBuffer() == NULL)
	{
		CVisionSystem::Instance()->WriteMessage(LogTypeError, _T("UpdatePreview[%d] - pSrcImgObj(NULL)"), IDX_AREA4);
		return;
	}
	EImageBW8 ImageX;	
	ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes()*8);
	if (&ImageX == NULL)
		return;

	for(int i = 0; i < IDX_2D_MAPPING_MAX; i++)
	{
		m_ImageObjectMappingTeach[i].Create(pImgObj->GetWidth(), pImgObj->GetHeight(), 8, 1);
	}

	m_ImageObjectMappingTeach[0].Clone(pImgObj);
	//
	EImageBW8 ImageX1;	
	ImageX1.SetImagePtr(
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetWidth(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetHeight(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetImageBuffer(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetWidthBytes()*8);

	m_wndMappingImageView[IDX_2D_MAPPING1].ImageUpdate();
	m_wndMappingImageView[IDX_2D_MAPPING1].ZoomFit( FALSE );

	//
	UpdatePreviewGainOffset();

#endif // RELEASE_1G
}

void CTeachThresholdPreview::UpdatePreviewGainOffset()
{
	if( !m_ImageObjectMappingTeach[IDX_2D_MAPPING1].IsValid() )
		return;

	if( !m_ImageObjectMappingTeach[IDX_2D_MAPPING2].IsValid() )
		return;

	EImageBW8 ImageX1;
	ImageX1.SetImagePtr(
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetWidth(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetHeight(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetImageBuffer(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING1].GetWidthBytes()*8);

	EImageBW8 ImageX2;	
	ImageX2.SetImagePtr(
		m_ImageObjectMappingTeach[IDX_2D_MAPPING2].GetWidth(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING2].GetHeight(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING2].GetImageBuffer(), 
		m_ImageObjectMappingTeach[IDX_2D_MAPPING2].GetWidthBytes()*8);

	EasyImage::Threshold( &ImageX1, &ImageX2, m_stChipOcr.nThreshold );

	m_wndMappingImageView[IDX_2D_MAPPING2].ImageUpdate();
//	m_wndMappingImageView[IDX_2D_MAPPING2].ZoomFit( FALSE );
}

void CTeachThresholdPreview::OnBnClickedCancel()
{
	WRITE_LOG( WL_BTN, _T("CTeachThresholdPreview::OnBnClickedCancel") );

	CDialog::OnCancel();
}

HBRUSH CTeachThresholdPreview::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_SLIDER_THRESHOLD:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
	}

	return hbr;
}

void CTeachThresholdPreview::OnEnChangeEditPadGain()
{
	UpdateData( TRUE );

	m_wndSliderPadThreshold.SetPos( m_stChipOcr.nThreshold );

	UpdatePreviewGainOffset();

	UpdateData( FALSE );
}

void CTeachThresholdPreview::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( pScrollBar )
	{
		int nMinPos, nMaxPos;
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
		pSlider->GetRange(nMinPos, nMaxPos); 
		int nPos = pSlider->GetPos();

		CString strV;
		switch ( (short)pSlider->GetDlgCtrlID() )
		{
		case IDC_SLIDER_THRESHOLD:
			strV.Format( _T("%d"), nPos );
			GetDlgItem(IDC_EDIT_THRESHOLD)->SetWindowText( strV );	
			break;
		
			UpdateData(FALSE);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}