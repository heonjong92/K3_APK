// TeachOCRReergistration.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachOCRReRegistration.h"
#include "afxdialogex.h"

#include "VisionSystem.h"
#include "ModelInfo.h"
#include "LanguageInfo.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>
#include <XImageView\xImageViewCtrl.h>
#include <XGraphic/xGraphicObject.h>
#include <algorithm>
#include <direct.h>


IMPLEMENT_DYNAMIC(TeachOCRReRegistration, CDialogEx)

TeachOCRReRegistration::TeachOCRReRegistration(CWnd* pParent /*=NULL*/)
	: CDialogEx(TeachOCRReRegistration::IDD, pParent)
{
	m_SegmentfileData.clear();

	nCharacterIndex = 0;
	nSegmentNumber	= 0;

	m_clrBorder = RGB(126,  27,  29);
	m_clrBody	= RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("OCR Font Re-Registration Manager");
	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_HITS_LOGO );
}

TeachOCRReRegistration::~TeachOCRReRegistration()
{
	for(int i = 0; i < DLG_VIEW_MAX_NUMBER; i++)
	{
		m_ImageObjectRefont[i].Destroy();
	}
}

void TeachOCRReRegistration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,	IDC_TAB_OCRTEXT,				m_FontCharacterTab);
	DDX_Control(pDX,	IDC_COMBO_OCRTEXT,				m_FontCharacterCombobox);
	DDX_Control(pDX,	IDC_BUTTON_LISTPREV,			m_btnListPrev);
	DDX_Control(pDX,	IDC_BUTTON_LISTNEXT,			m_btnListNext);
	DDX_Control(pDX,	IDC_BUTTON_SELECTALLLITS,		m_btnSelectAllList);
	DDX_Control(pDX,	IDC_BUTTON_SELECTDATAALLCLEAR,	m_btnDataAllClear);
	DDX_Control(pDX,	IDC_BUTTON_RECREATEFONTFILE,	m_btnFontReRegistration);
	DDX_Control(pDX,	IDOK,							m_btnClose);
}


BEGIN_MESSAGE_MAP(TeachOCRReRegistration, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OCRTEXT,		&TeachOCRReRegistration::OnTcnSelchangeTabOcrtext)
	ON_BN_CLICKED(IDC_BUTTON_LISTPREV,				&TeachOCRReRegistration::OnBnClickedButtonListprev)
	ON_BN_CLICKED(IDC_BUTTON_LISTNEXT,				&TeachOCRReRegistration::OnBnClickedButtonListnext)
	ON_CBN_SELCHANGE(IDC_COMBO_OCRTEXT,				&TeachOCRReRegistration::OnCbnSelchangeComboOcrtext)
	ON_BN_CLICKED(IDC_BUTTON_SELECTALLLITS,			&TeachOCRReRegistration::OnBnClickedButtonSelectalllits)
	ON_BN_CLICKED(IDC_BUTTON_SELECTDATAALLCLEAR,	&TeachOCRReRegistration::OnBnClickedButtonSelectdataallclear)
	ON_BN_CLICKED(IDC_BUTTON_RECREATEFONTFILE,		&TeachOCRReRegistration::OnBnClickedButtonRecreatefontfile)
	ON_BN_CLICKED(IDOK,								&TeachOCRReRegistration::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL TeachOCRReRegistration::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set Button
	m_btnListPrev.			SetFontHeight( 18 );
	m_btnListNext.			SetFontHeight( 18 );
	m_btnSelectAllList.		SetFontHeight( 16 );
	m_btnDataAllClear.		SetFontHeight( 15 );
	m_btnFontReRegistration.SetFontHeight( 16 );
	m_btnClose.				SetFontHeight( 16 );

	m_btnListPrev.			SetColor( RGB(0, 204, 0), RGB(51, 255, 51), RGB(0, 204, 0), PDC_LIGHTGRAY, PDC_WHITE );
	m_btnListNext.			SetColor( RGB(0, 204, 0), RGB(51, 255, 51), RGB(0, 204, 0), PDC_LIGHTGRAY, PDC_WHITE );
	m_btnFontReRegistration.SetColor( DEF_BTN_COLOR_SAVE );
	m_btnClose.				SetColor( DEF_BTN_COLOR_MEASURE );

	m_btnListPrev.			SetBold(TRUE);
	m_btnListNext.			SetBold(TRUE);
	m_btnSelectAllList.		SetBold(TRUE);
	m_btnDataAllClear.		SetBold(TRUE);
	m_btnFontReRegistration.SetBold(TRUE);
	m_btnClose.				SetBold(TRUE);

	// Set List
	SegmentImageDataLoad();
	ViewListUpdate();

	return TRUE;
}

void TeachOCRReRegistration::OnPaint()
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

	dc.FillSolidRect( rcClient.left, rcSplit2.top-rcClient.top, rcClient.Width(), rcClient.bottom, m_clrBody );

	dc.FillSolidRect( rcTitle, m_clrBorder );
	dc.FillSolidRect( rcClient.left, rcClient.bottom-nBorderWidth, rcClient.Width(), nBorderWidth, m_clrBorder );
	dc.FillSolidRect( rcClient.left, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );
	dc.FillSolidRect( rcClient.right-nBorderWidth, rcTitle.bottom, nBorderWidth, rcClient.bottom-rcTitle.bottom, m_clrBorder );

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

	UpdateLanguage();
}

void TeachOCRReRegistration::UpdateLanguage()
{
	if (CLanguageInfo::Instance()->m_nLangType == 0)
	{
		GetDlgItem(IDC_BUTTON_SELECTALLLITS		)->SetWindowText(_T("목록 전체 선택"));
		GetDlgItem(IDC_BUTTON_SELECTDATAALLCLEAR)->SetWindowText(_T("선택 목록 초기화"));
		GetDlgItem(IDC_BUTTON_RECREATEFONTFILE	)->SetWindowText(_T("Font 파일 재 생성"));
		GetDlgItem(IDOK							)->SetWindowText(_T("닫기"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SELECTALLLITS		)->SetWindowText(_T("Select All Lists"));
		GetDlgItem(IDC_BUTTON_SELECTDATAALLCLEAR)->SetWindowText(_T("Select Data All Clear"));
		GetDlgItem(IDC_BUTTON_RECREATEFONTFILE	)->SetWindowText(_T("Font File Re-Registration"));
		GetDlgItem(IDOK							)->SetWindowText(_T("Close"));
	}
}

HBRUSH TeachOCRReRegistration::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:

	case IDC_TAB_OCRTEXT:

		pDC->SetBkMode(TRANSPARENT);
		return m_brushBody;
		break;
	}

	return hbr;
}

int TeachOCRReRegistration::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwLineBtnTypes = MBT_ZOOM_INOUT;//|MBT_MEASURE|MBT_LOAD|MBT_SAVE;
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.fnOnDrawExt			= ImageViewDrawExtRefont;
	regCB.fnOnEvent				= ImageViewEventRefont;
	regCB.fnOnFireMouseEvent	= ImageViewMouseEventRefont;
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwTitleBgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	DWORD dwTitleFgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();
	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor     = UIExt::CResourceManager::Instance()->GetViewerBodyColor();
	DWORD dwBtnColor      = UIExt::CResourceManager::Instance()->GetViewerActiveButtonColor();


	int nX = 5, nY = 2, nOffset = 2;

	CRect reViewSize;	
	GetClientRect(reViewSize);
	reViewSize.DeflateRect(12, 65, 12, (int)((float)reViewSize.Height() / 5.5f));

	int nViewWidth		= (int)( (float)(reViewSize.Width()  - (nOffset * (nX-1)) ) / (float)nX ); 
	int nViewHeight		= (int)( (float)(reViewSize.Height() - (nOffset * (nY-1)) ) / (float)nY );


	CRect reDlgViewSize[DLG_VIEW_MAX_NUMBER];
	int nPos_X, nPos_Y;

	for(int nViewY = 0; nViewY < nY; nViewY++)
	{	
		if(nViewY == 0)  nPos_Y = reViewSize.top;
		else 
		{
			nPos_Y += nViewHeight;
			nPos_Y += nOffset;	
		}

		for(int nViewX = 0; nViewX < nX; nViewX++)
		{
			if(nViewX == 0)  nPos_X = reViewSize.left;
			else 
			{
				nPos_X += nViewWidth;
				nPos_X += nOffset;	
			}

			CRect reViewBuff;
			reViewBuff.left		= nPos_X;
			reViewBuff.top		= nPos_Y;
			reViewBuff.right	= reViewBuff.left + nViewWidth;
			reViewBuff.bottom	= reViewBuff.top  + nViewHeight;

			reDlgViewSize[nViewX + (nViewY * nX)] = reViewBuff;
		}
	}


	CString strBuff;
	for(int i = 0; i < DLG_VIEW_MAX_NUMBER; i ++)
	{
		m_ImageViewRefont[i].Create(&m_ImageViewManagerRefont[i], this);
		m_ImageViewRefont[i].ShowWindow(SW_SHOW);
		m_ImageViewRefont[i].ShowTitleWindow(TRUE);
		m_ImageViewRefont[i].SetAnimateWindow(FALSE);
		m_ImageViewRefont[i].SetMiniButtonTypeAll(dwLineBtnTypes);
		m_ImageViewRefont[i].SetImageObject(&m_ImageObjectRefont[i], FALSE);
		m_ImageViewRefont[i].SetRegisterCallBack(i, &regCB);
		m_ImageViewRefont[i].SetTitleColor(dwTitleBgColor, dwTitleFgColor, dwTitleFgColor);
		m_ImageViewRefont[i].SetStatusColor(dwStatusBgColor, dwStatusFgColor);
		m_ImageViewRefont[i].SetButtonColor(RGB(255,255,255), dwBtnColor, (DWORD)-1, (DWORD)-1);
		m_ImageViewRefont[i].SetBodyColor(dwBodyColor);
		strBuff.Format(_T("View : %02d"), i+1);
		m_ImageViewRefont[i].SetTitle(strBuff, FALSE);
		m_ImageViewRefont[i].SetRealSizePerPixel(18.0001f);
		m_ImageViewRefont[i].ShowScrollBar(TRUE);	
	}


	LockWindowUpdate();

	HDWP hDWP = BeginDeferWindowPos(1);
	for(int i = 0; i < DLG_VIEW_MAX_NUMBER; i ++)
		hDWP = DeferWindowPos( hDWP, m_ImageViewRefont[i].GetSafeHwnd(), NULL, reDlgViewSize[i].left, reDlgViewSize[i].top, reDlgViewSize[i].Width(), reDlgViewSize[i].Height(), 0 );
	
	EndDeferWindowPos(hDWP);
	
	UnlockWindowUpdate();

	return 0;
}

void APIENTRY TeachOCRReRegistration::ImageViewDrawExtRefont( IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData )
{
	TeachOCRReRegistration* pReFontDlg = (TeachOCRReRegistration*)lpUsrData;
	return;
}

BOOL APIENTRY TeachOCRReRegistration::ImageViewEventRefont(ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData)
{	
	TeachOCRReRegistration* pReFontDlg = (TeachOCRReRegistration*)lpUsrData;
	// if(evt == ImageViewEvent::ButtonEventLoadClick)
	// {
	// 	return TRUE;
	// }
	return FALSE;
}

BOOL APIENTRY TeachOCRReRegistration::ImageViewMouseEventRefont( DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData )
{
	TeachOCRReRegistration* pReFontDlg = (TeachOCRReRegistration*)lpUsrData;
	int nViewIdx = nIndexData;

	CPoint ptImagePoint = pReFontDlg->m_ImageViewRefont[nViewIdx].GetIDeviceContext()->MousePosToImagePos(point.x, point.y);

	if( dwMsg == WM_LBUTTONDBLCLK )
	{	 
		DWORD dwTitleBgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
		DWORD dwTitleFgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();

		DWORD dwTitleColor = dwTitleBgColor;

		if( (int)pReFontDlg->m_SegmentfileData.at(pReFontDlg->nCharacterIndex).vbCheck.size() <= pReFontDlg->nSegmentNumber + nIndexData ) return FALSE;
		
		if( !pReFontDlg->m_SegmentfileData.at(pReFontDlg->nCharacterIndex).vbCheck.at(pReFontDlg->nSegmentNumber + nIndexData) ) dwTitleColor = PDC_GREEN;
		pReFontDlg->m_SegmentfileData.at(pReFontDlg->nCharacterIndex).vbCheck.at(pReFontDlg->nSegmentNumber + nIndexData) = 
			!pReFontDlg->m_SegmentfileData.at(pReFontDlg->nCharacterIndex).vbCheck.at(pReFontDlg->nSegmentNumber + nIndexData);

		pReFontDlg->m_ImageViewRefont[nViewIdx].SetTitleColor( dwTitleColor, dwTitleFgColor, dwTitleFgColor);	

		return TRUE;
	}

	return FALSE;
}	

void TeachOCRReRegistration::SegmentImageDataLoad()
{
	//Data Clear
	m_SegmentfileData.clear();
	nCharacterIndex = 0;
	nSegmentNumber = 0;

	// Font Segment Image Load
	CString strSegmentPath;

	// 1G Chip OCR 추가로 Segment Image Load 경로 변경 250120 황대은J
	switch (m_nOcrMode)
	{
	case OCR_TRAY:
		strSegmentPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Tray Font Segment");
		break;
	case OCR_CHIP:
		strSegmentPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Chip Font Segment");
		break;
	case OCR_LABEL:
		strSegmentPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Label Font Segment");
		break;
	}

	if (!IsExistFile((LPCTSTR)strSegmentPath))
	{
		//	WRITE_LOG(WL_ERROR, _T("No Font File in ReadOcr()"));
		//	return;
	}	


	CFileFind Folderfinder, ImageFilefinder;  
	BOOL bFind = Folderfinder.FindFile(strSegmentPath + _T("\\*.*"));

	while(bFind)  
	{ 	 
		bFind = Folderfinder.FindNextFile();
		if(Folderfinder.IsDirectory())	 
		{
			CString strFolderName = Folderfinder.GetFileName();
			if( strFolderName == _T(".") || strFolderName == _T("..") ) continue;	

			// 240404 HJ : Font 폴더 이름 ACSII Code 값만 저장하면 알아보기 어렵다고 변경 요청
			CString strASCII;
			AfxExtractSubString(strASCII,	strFolderName, 0, '_');	// 65_A면 65만 사용

			int nASCIICode = _ttoi(strASCII);
			CString strCharacter = (CString)(char)nASCIICode;

			BOOL bImageFind = ImageFilefinder.FindFile(strSegmentPath + _T("\\") + strFolderName + _T("\\*.bmp"));
			while(bImageFind)  
			{ 	
				bImageFind = ImageFilefinder.FindNextFile();  
				if(ImageFilefinder.IsArchived())	 
				{  
					CString strImageFileName = ImageFilefinder.GetFileName();
					if( strImageFileName == _T(".") || strImageFileName == _T("..") ) continue;	


					int nCharacterNum = -1;
					for(int nCheckIndex = 0; nCheckIndex < (int)m_SegmentfileData.size(); nCheckIndex++)
					{
						if( m_SegmentfileData.at(nCheckIndex).strCharacterName == strCharacter ) 
						{
							nCharacterNum = nCheckIndex;
							break;
						}else continue;
					}


					if(nCharacterNum == -1) // New Data
					{
						stOCRSegmentData DataBuff;
						DataBuff.clear();

						DataBuff.strCharacterName = strCharacter;
						m_SegmentfileData.push_back(DataBuff);

						nCharacterNum = (int)m_SegmentfileData.size() - 1;
					}


					CString strTemp_Path = strSegmentPath + _T("\\") + strFolderName + _T("\\") + strImageFileName;

					CString strTemp_CharacterSize;
					AfxExtractSubString(strTemp_CharacterSize,	strImageFileName,		0, '.');
					AfxExtractSubString(strTemp_CharacterSize,	strTemp_CharacterSize,	2, '_');

					m_SegmentfileData.at(nCharacterNum).vstrPath.			push_back(strTemp_Path);
					m_SegmentfileData.at(nCharacterNum).vstrCharacterSize.	push_back(strTemp_CharacterSize);
					m_SegmentfileData.at(nCharacterNum).vbCheck.			push_back(FALSE);
				}									
			}	
		}
	}
	

	for(int nFolderIndex = 0; nFolderIndex < (int)m_SegmentfileData.size(); nFolderIndex++)
	{
		CString str = m_SegmentfileData.at(nFolderIndex).strCharacterName;
		m_FontCharacterTab.InsertItem(nFolderIndex, str);
		m_FontCharacterCombobox.AddString(str);
	}

	m_FontCharacterCombobox.SetCurSel(0);
}

void TeachOCRReRegistration::ViewListUpdate()
{

	// 1G Chip OCR 추가로 Segment Image Load 경로 변경 250120 황대은J
	CString strLogoPath;
	switch (m_nOcrMode)
	{
	case OCR_TRAY:
		strLogoPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Tray Font Segment\\Amkor Logo.bmp");
		break;
	case OCR_CHIP:
		strLogoPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Chip Font Segment\\Amkor Logo.bmp");
		break;
	case OCR_LABEL:
		strLogoPath = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Label Font Segment\\Amkor Logo.bmp");
		break;
	}

	DWORD dwTitleBgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	DWORD dwTitleFgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();
	DWORD dwTitleColor;

	CFileStatus fs;
	if (m_SegmentfileData.empty()) // 폴더 내 티칭 이미지 없으면 프로그램 종료되어 추가 250109 황대은J
	{
		//AfxMessageBox(_T("폰트 티칭 이미지가 없습니다."), MB_ICONERROR);
		return;
	}

	int nTotalSegmentNum =  (int)m_SegmentfileData.at(nCharacterIndex).vstrPath.size();
	for(int nViewNum = 0; nViewNum < DLG_VIEW_MAX_NUMBER; nViewNum++)
	{
		if( nSegmentNumber + nViewNum < nTotalSegmentNum )
		{
			// Image Load
			if( CFile::GetStatus(m_SegmentfileData.at(nCharacterIndex).vstrPath.at(nSegmentNumber + nViewNum), fs) )
				m_ImageObjectRefont[nViewNum].LoadFromFile( m_SegmentfileData.at(nCharacterIndex).vstrPath.at(nSegmentNumber + nViewNum) );
			else 
			{
				AfxMessageBox(_T("Segment File Data가 변경되었습니다.\nRe-Registration Manager를 다시 열어주세요."), MB_ICONERROR);
				return;
			}


			// Image Check
			if( m_SegmentfileData.at(nCharacterIndex).vbCheck.at(nSegmentNumber + nViewNum) )	dwTitleColor = PDC_GREEN;
			else																				dwTitleColor = dwTitleBgColor;
			m_ImageViewRefont[nViewNum].SetTitleColor( dwTitleColor, dwTitleFgColor, dwTitleFgColor);

			CString strTitle;
			strTitle.Format(_T("%s : %d"), m_SegmentfileData.at(nCharacterIndex).strCharacterName, nSegmentNumber + nViewNum);
			m_ImageViewRefont[nViewNum].SetTitle(strTitle, FALSE);
		}
		else
		{
			if( CFile::GetStatus(strLogoPath, fs) ) m_ImageObjectRefont[nViewNum].LoadFromFile( strLogoPath );
			else									m_ImageObjectRefont[nViewNum].Create(100, 100, 8, 1);

			dwTitleColor = dwTitleBgColor;
			m_ImageViewRefont[nViewNum].SetTitleColor( dwTitleColor, dwTitleFgColor, dwTitleFgColor);
			m_ImageViewRefont[nViewNum].SetTitle(_T(""), FALSE);
		}

		m_ImageViewRefont[nViewNum].ImageUpdate();
	}


 	if( nSegmentNumber - DLG_VIEW_MAX_NUMBER >= 0				)	GetDlgItem(IDC_BUTTON_LISTPREV)->EnableWindow(TRUE );
 	else															GetDlgItem(IDC_BUTTON_LISTPREV)->EnableWindow(FALSE);
	if( nSegmentNumber + DLG_VIEW_MAX_NUMBER < nTotalSegmentNum	)	GetDlgItem(IDC_BUTTON_LISTNEXT)->EnableWindow(TRUE );
	else															GetDlgItem(IDC_BUTTON_LISTNEXT)->EnableWindow(FALSE);
}

void TeachOCRReRegistration::OnTcnSelchangeTabOcrtext(NMHDR *pNMHDR, LRESULT *pResult)
{
	nCharacterIndex = m_FontCharacterTab.GetCurSel();
	m_FontCharacterCombobox.SetCurSel(nCharacterIndex);
	nSegmentNumber	= 0;

	ViewListUpdate();

	*pResult = 0;
}

void TeachOCRReRegistration::OnCbnSelchangeComboOcrtext()
{
	nCharacterIndex = m_FontCharacterCombobox.GetCurSel();
	m_FontCharacterTab.SetCurSel(nCharacterIndex);
	nSegmentNumber	= 0;

	ViewListUpdate();

	//UpdateData(TRUE);
	//UpdateData(FALSE);
}

void TeachOCRReRegistration::OnBnClickedButtonListprev()
{
	nSegmentNumber -= DLG_VIEW_MAX_NUMBER;
	ViewListUpdate();
}

void TeachOCRReRegistration::OnBnClickedButtonListnext()
{
	nSegmentNumber += DLG_VIEW_MAX_NUMBER;
	ViewListUpdate();
}

void TeachOCRReRegistration::OnBnClickedButtonSelectalllits()
{
	BOOL bAllCheck = TRUE;
	for (int nSize = 0; nSize < m_FontCharacterTab.GetItemCount(); nSize++)
	{
		for (int nRunNum = 0; nRunNum < 2; nRunNum++)
		{
			for (int nViewIndex = 0; nViewIndex < (int)m_SegmentfileData.at(/*nCharacterIndex*/nSize).vstrPath.size(); /*Dlg_View_Max_Number;*/ nViewIndex++)
			{
				if (!(/*nSegmentNumber +*/ nViewIndex < (int)m_SegmentfileData.at(/*nCharacterIndex*/nSize).vstrPath.size())) continue;

				switch (nRunNum)
				{
				case 0:
					if (m_SegmentfileData.at(/*nCharacterIndex*/nSize).vbCheck.at(/*nSegmentNumber +*/ nViewIndex)) continue;
					else																				bAllCheck = FALSE;
					break;

				case 1:
					m_SegmentfileData.at(/*nCharacterIndex*/nSize).vbCheck.at(/*nSegmentNumber +*/ nViewIndex) = !bAllCheck;
					break;
				}
			}
		}
	}
	ViewListUpdate();
}

void TeachOCRReRegistration::OnBnClickedButtonSelectdataallclear()
{
	for(int nCharacterNum = 0; nCharacterNum < (int)m_SegmentfileData.size(); nCharacterNum++)
	{
		for(int nCheckNum = 0; nCheckNum < (int)m_SegmentfileData.at(nCharacterNum).vbCheck.size(); nCheckNum++)
		{
			m_SegmentfileData.at(nCharacterNum).vbCheck.at(nCheckNum) = FALSE;
		}
	}

	ViewListUpdate();
}

void TeachOCRReRegistration::OnBnClickedButtonRecreatefontfile()
{
	CString strMessage = _T("Font 파일를 다시 만드시겠습니까?");
	if (CLanguageInfo::Instance()->m_nLangType == 1)
		strMessage = _T("Do you want to recreate the font file?");

	if(AfxMessageBox(strMessage, MB_YESNO) != IDYES ) return;

	USES_CONVERSION;

	// 1G Chip OCR 추가로 Font Save 경로 변경, 250120 황대은J
	CString strFontPatch;

	switch (m_nOcrMode)
	{
	case OCR_TRAY:
		strFontPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TrayInputFont.OCR");
		break;
	case OCR_CHIP:
		strFontPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\ChipOCRFont.OCR");
		break;
	case OCR_LABEL:
		strFontPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\LabelOCRFont.OCR");
		break;
	}

	CString strFontCopyPatch	= CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Font Data Backup");

	if (!IsExistFile((LPCTSTR)strFontPatch))
	{
		//	WRITE_LOG(WL_ERROR, _T("No Font File in ReadOcr()"));
		//	return;
	}		

	// Time Data
	CTime	cTimeData = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%02d%02d%02d%02d%02d%02d"), cTimeData.GetYear(), cTimeData.GetMonth(), cTimeData.GetDay(), cTimeData.GetHour(), cTimeData.GetMinute(), cTimeData.GetSecond());	

	// Font File Copy
	if (!IsExistFile((LPCTSTR)strFontCopyPatch)) MakeDirectory( (LPCTSTR)strFontCopyPatch );
#ifdef RELEASE_1G
	//const CModelInfo::stTrayOcr& TrayOcrInfo = CModelInfo::Instance()->GetTrayOcr();

	switch (m_nOcrMode)
	{
	case OCR_TRAY:
		strFontCopyPatch += _T("\\Re-Registration_TrayInputFont_") + strTime + _T(".OCR");
		::CopyFile(strFontPatch, strFontCopyPatch, TRUE);
		break;
	case OCR_CHIP:
		strFontCopyPatch += _T("\\Re-Registration_ChipOcrFont_") + strTime + _T(".OCR");
		::CopyFile(strFontPatch, strFontCopyPatch, TRUE);
	}
	EOCR eOCRFont;
	eOCRFont.NewFont(5, 5);
	eOCRFont.SetTextColor(EOCRColor_WhiteOnBlack);
	eOCRFont.SetCharSpacing(1);
	eOCRFont.SetNoiseArea(20);
	eOCRFont.SetRemoveNarrowOrFlat(FALSE);
	eOCRFont.SetCutLargeChars(TRUE);
	eOCRFont.SetRemoveBorder(TRUE);
	eOCRFont.SetSegmentationMode(ESegmentationMode_RepasteObjects);
	eOCRFont.SetMatchingMode(EMatchingMode_Rms);

#else
	strFontCopyPatch += _T("\\Re-Registration_LabelOCRFont_") + strTime + _T(".OCR");
	::CopyFile(strFontPatch, strFontCopyPatch, TRUE);

	EOCR eOCRFont;
	eOCRFont.NewFont(5, 5);
	eOCRFont.SetTextColor(EOCRColor_BlackOnWhite);
	eOCRFont.SetCharSpacing(1);
	eOCRFont.SetNoiseArea(20);
	eOCRFont.SetRemoveNarrowOrFlat(FALSE);
	eOCRFont.SetCutLargeChars(TRUE);
	eOCRFont.SetRemoveBorder(FALSE);
	eOCRFont.SetSegmentationMode(ESegmentationMode_RepasteObjects);
	eOCRFont.SetMatchingMode(EMatchingMode_Rms);
#endif


	EImageBW8 eEBW8Image;
	for (int nCharacterNum = 0; nCharacterNum < (int)m_SegmentfileData.size(); nCharacterNum++)
	{
		for(int nCheckNum = 0; nCheckNum < (int)m_SegmentfileData.at(nCharacterNum).vbCheck.size(); nCheckNum++)
		{
			if (!m_SegmentfileData.at(nCharacterNum).vbCheck.at(nCheckNum))
			{
				::DeleteFile(m_SegmentfileData.at(nCharacterNum).vstrPath.at(nCheckNum));
				continue;
			}
			// Segment Image Load
			eEBW8Image.Load(T2A(m_SegmentfileData.at(nCharacterNum).vstrPath.at(nCheckNum)));

			// OCR Data Setting
			CString strSizeData = m_SegmentfileData.at(nCharacterNum).vstrCharacterSize.at(nCheckNum);		
			int nThreshold = EasyImage::AutoThreshold(&eEBW8Image, EThresholdMode_MinResidue).Value;
			if(nThreshold ==   0) nThreshold = 1;
			if(nThreshold == 255) nThreshold = 254;		

			// Chip OCR의 경우 Font가 White, Black인 경우가 둘 다 있어서 추가, 250120 황대은
			if (m_nOcrMode == OCR_CHIP) 
			{
				if (strSizeData.Mid(12, 1) == 'B')
					eOCRFont.SetTextColor(EOCRColor_BlackOnWhite);
				else
					eOCRFont.SetTextColor(EOCRColor_WhiteOnBlack);
			}

			eOCRFont.SetMinCharWidth	( (INT32)_ttoi(strSizeData.Mid(0, 3))	);
			eOCRFont.SetMaxCharWidth	( (INT32)_ttoi(strSizeData.Mid(3, 3))	);
			eOCRFont.SetMinCharHeight	( (INT32)_ttoi(strSizeData.Mid(6, 3))	);
			eOCRFont.SetMaxCharHeight	( (INT32)_ttoi(strSizeData.Mid(9, 3))	);
			eOCRFont.SetThreshold		( (INT32)nThreshold						);
			eOCRFont.BuildObjects		( &eEBW8Image							);
			eOCRFont.FindAllChars		( &eEBW8Image							);

			int nSegmentCount = eOCRFont.GetNumChars();

			if(nSegmentCount == 1)
			{
				UINT32 nSelectIndex;
				eOCRFont.HitChars( eOCRFont.CharGetOrgX(0), eOCRFont.CharGetOrgY(0), nSelectIndex, 1.00f, 1.00f, 0.00f, 0.00f);

				wchar_t OCRCharCode	= m_SegmentfileData.at(nCharacterNum).strCharacterName[0];
				eOCRFont.LearnPattern(&eEBW8Image, nSelectIndex, OCRCharCode, EOCRClass_Digit);
			}
		}
	}

	eOCRFont.Save(T2A(strFontPatch));
}

void TeachOCRReRegistration::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

BOOL TeachOCRReRegistration::DeleteFolderContents(const CString& folderPath)
{
	USES_CONVERSION;
	CFileFind finder;
	CString searchPath = folderPath + _T("\\*.*");

	BOOL isFileFound = finder.FindFile(searchPath);

	while (isFileFound) 
	{
		isFileFound = finder.FindNextFile();

		// "."와 ".."을 건너뜀
		if (finder.IsDots())
			continue;

		CString filePath = finder.GetFilePath();

		if (finder.IsDirectory()) 
		{
			// 하위 디렉터리를 재귀적으로 삭제
			if (!DeleteFolderContents(filePath)) 
				return false;

			// 빈 디렉터리 삭제
			if (_rmdir(T2A(filePath)) != 0) 
				return false; // 디렉터리 삭제 실패
		}
		else
		{
			if (filePath.Find(_T("Amkor Logo.bmp")) != -1)
				continue;

			CFile::Remove(filePath); // 파일 삭제
		}
	}

	finder.Close();
	return true;
}