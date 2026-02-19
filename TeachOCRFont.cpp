// TeachOCRFont.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachOCRFont.h"
#include "afxdialogex.h"

#include "VisionSystem.h"
#include "InspectionVision.h"
#include "TeachTabLabel.h"
#include "TeachOCRReRegistration.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>
#include <XImageView\xImageViewCtrl.h>
#include <XGraphic/xGraphicObject.h>
#include <algorithm>

#include <Open_eVision_2_11.h>

IMPLEMENT_DYNAMIC(TeachOCRFont, CDialogEx)

using namespace VisionProcess;
using namespace Euresys::Open_eVision_2_11;

TeachOCRFont::TeachOCRFont(CWnd* pParent /*=NULL*/)
	: CDialogEx(TeachOCRFont::IDD, pParent)
{
	m_SegmentFileData.	 clear();
	m_OCRFontLoadData.	 clear();
	m_OCRFontReadingData.clear();

	strFontPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\LabelOCRFont.OCR");

	m_clrBorder = RGB(126,  27,  29);
	m_clrBody	= RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush( m_clrBody );

	m_strTitle = _T("OCR Font Manager");
	strImgSubCode[0] = _T("_CenterCut");
	strImgSubCode[1] = _T("_Cut");
	strImgSubCode[2] = _T("_Org");

	m_pIconImage = GdipLoadImageFromRes( AfxGetResourceHandle(), _T("PNG"), PNG_MENU_ICONS ); // PNG_HITS_LOGO
	m_nMenuIconSize = m_pIconImage->GetHeight();

	m_FontSizeViewPosition= CPoint(0, 0);
}

TeachOCRFont::~TeachOCRFont()
{
	CString strTeachingOCRPatch;
	strTeachingOCRPatch = strFontPatch;
	m_ImageObjectForOcr.Destroy();
}

void TeachOCRFont::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control	(pDX, IDC_FONTTEACH_COMBO_OPTION1,				m_Combo_Option1				); 
	DDX_Control	(pDX, IDC_FONTTEACH_COMBO_OPTION2,				m_Combo_Option2				); 
	DDX_Control	(pDX, IDC_FONTTEACH_COMBO_OPTION3,				m_Combo_Option3				);
	DDX_Control	(pDX, IDC_FONTTEACH_COMBO_OPTION4,				m_Combo_Option4				);
	DDX_Control	(pDX, IDC_FONTTEACH_COMBO_OPTION5,				m_Combo_Option5				);

	DDX_Control (pDX, IDC_CHECK_MANUAL_TEACHING_MODE,			m_Check_ManualTeachingMode	);
	DDX_Control	(pDX, IDC_FONTTEACH_COMBO_MANUAL_OPTION,		m_Combo_ManualOption		);

	DDX_Control	(pDX, IDC_FONTTEACH_BTN_FONTSAVE,				m_btnFontSave				);
	DDX_Control	(pDX, IDC_FONTTEACH_BTN_VIEWREFRESH,			m_btnViewRefresh			);
	DDX_Control	(pDX, IDC_FONTTEACH_BTN_TEACHING_DATA_CLEAR,	m_btnDataClear				);
	DDX_Control	(pDX, IDC_FONTTEACH_BTN_REREGISTER_FONT,		m_btnReRegistration			);
	DDX_Control	(pDX, IDOK,										m_btnClose					);

}


BEGIN_MESSAGE_MAP(TeachOCRFont, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &TeachOCRFont::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_FONTSAVE,				&TeachOCRFont::OnBnClickedFontteachBtnFontsave			)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_VIEWREFRESH,			&TeachOCRFont::OnBnClickedFontteachBtnViewrefresh		)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_TEACHING_DATA_CLEAR,	&TeachOCRFont::OnBnClickedFontteachBtnTeachingDataClear	)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_REREGISTER_FONT,		&TeachOCRFont::OnBnClickedFontteachBtnReregisterFont	)
	ON_BN_CLICKED(IDC_BUTTON_RETRYPREV,						&TeachOCRFont::OnBnClickedButtonRetryprev				)
	ON_BN_CLICKED(IDC_BUTTON_RETRYNEXT,						&TeachOCRFont::OnBnClickedButtonRetrynext				)
	ON_CBN_SELCHANGE(IDC_FONTTEACH_COMBO_OPTION1,			&TeachOCRFont::OnCbnSelchangeFontteachComboOption1		)
	ON_CBN_SELCHANGE(IDC_FONTTEACH_COMBO_OPTION2,			&TeachOCRFont::OnCbnSelchangeFontteachComboOption2		)
	ON_CBN_SELCHANGE(IDC_FONTTEACH_COMBO_OPTION3,			&TeachOCRFont::OnCbnSelchangeFontteachComboOption3		)
	ON_CBN_SELCHANGE(IDC_FONTTEACH_COMBO_OPTION4,			&TeachOCRFont::OnCbnSelchangeFontteachComboOption4		)
	ON_CBN_SELCHANGE(IDC_FONTTEACH_COMBO_OPTION5,			&TeachOCRFont::OnCbnSelchangeFontteachComboOption5		)

	ON_BN_CLICKED(IDC_CHECK_MANUAL_TEACHING_MODE,			&TeachOCRFont::OnBnClickedCheckManualTeachingMode		)
	ON_CBN_SELCHANGE(IDC_FONTTEACH_COMBO_MANUAL_OPTION,		&TeachOCRFont::OnCbnSelchangeFontteachComboManualOption	)

	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_MANUAL_OPTION_APPLY,	&TeachOCRFont::OnBnClickedFontteachBtnManualOptionApply)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_MANUAL_OPTION_SAVE,		&TeachOCRFont::OnBnClickedFontteachBtnManualOptionSave)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_MANUAL_OPTION_DELETE,	&TeachOCRFont::OnBnClickedFontteachBtnManualOptionDelete)
END_MESSAGE_MAP()

BOOL TeachOCRFont::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set Button
	m_btnFontSave.			SetFontHeight( 15 );
	m_btnViewRefresh.		SetFontHeight( 15 );
	m_btnDataClear.			SetFontHeight( 15 );
	m_btnReRegistration.	SetFontHeight( 15 );
	m_btnClose.				SetFontHeight( 20 );

	m_btnFontSave.			SetColor( DEF_BTN_COLOR_SAVE );
	m_btnReRegistration.	SetColor( RGB(51, 0, 153), RGB(102, 51, 204), RGB(153, 102, 255), PDC_LIGHTGRAY, PDC_WHITE );
	m_btnClose.				SetColor( DEF_BTN_COLOR_MEASURE );

	m_btnFontSave.			SetBold(TRUE);
	m_btnViewRefresh.		SetBold(TRUE);
	m_btnDataClear.			SetBold(TRUE);
	m_btnReRegistration.	SetBold(TRUE);
	m_btnClose.				SetBold(TRUE);

	m_Combo_Option1.SetCurSel(0);

	m_Combo_Option2.AddString(_T("Min Threshold"));
	m_Combo_Option2.AddString(_T("Max Threshold"));
	m_Combo_Option2.SetCurSel(0);

	m_Combo_Option3.AddString(_T("None"));
	m_Combo_Option3.AddString(_T("Use Image Filtering"));
	m_Combo_Option3.SetCurSel(0);

	m_Combo_Option4.AddString(_T("None"));
	m_Combo_Option4.AddString(_T("Use FontSize Filtering"));
	m_Combo_Option4.SetCurSel(0);

	m_Combo_Option5.AddString(_T("None"));
	m_Combo_Option5.AddString(_T("Use LargeChars Cut"));
	m_Combo_Option5.SetCurSel(0);

	SegmentFileDataLoad();

	return TRUE; 
}

void TeachOCRFont::OnPaint()
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

	int nIW = m_nMenuIconSize;
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

	g.DrawImage( m_pIconImage, Rect(nIX, nIY, nIW, nIH), m_nMenuIconSize * 1, 0, nIW, nIH, UnitPixel, &ImgAttr );

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

int TeachOCRFont::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwLineBtnTypes = MBT_ZOOM_INOUT|MBT_MEASURE|MBT_LOAD|MBT_SAVE;
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.fnOnDrawExt			= ImageViewDrawExt_OCR;
	regCB.fnOnEvent				= ImageViewEvent_OCR;
	regCB.fnOnFireMouseEvent	= ImageViewMouseEvent_OCR;
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwTitleBgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	DWORD dwTitleFgColor  = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();
	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor     = UIExt::CResourceManager::Instance()->GetViewerBodyColor();
	DWORD dwBtnColor      = UIExt::CResourceManager::Instance()->GetViewerActiveButtonColor();

	m_ImageViewForOcr.Create(&m_ImageViewManagerForOcr, this);
	m_ImageViewForOcr.ShowWindow(SW_SHOW);
	m_ImageViewForOcr.ShowTitleWindow(TRUE);
	m_ImageViewForOcr.SetAnimateWindow(FALSE);
	m_ImageViewForOcr.SetMiniButtonTypeAll(dwLineBtnTypes);
	m_ImageViewForOcr.SetImageObject(&m_ImageObjectForOcr, FALSE);
	m_ImageViewForOcr.SetRegisterCallBack(0, &regCB);
	m_ImageViewForOcr.SetTitleColor(dwTitleBgColor, dwTitleFgColor, dwTitleFgColor);
	m_ImageViewForOcr.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForOcr.SetButtonColor(RGB(255,255,255), dwBtnColor, (DWORD)-1, (DWORD)-1);
	m_ImageViewForOcr.SetBodyColor(dwBodyColor);
	m_ImageViewForOcr.SetTitle(_T("View : OCR Font"), FALSE);
	m_ImageViewForOcr.SetRealSizePerPixel(18.0001f);
	m_ImageViewForOcr.ShowScrollBar(TRUE);

	LockWindowUpdate();
	HDWP hDWP = BeginDeferWindowPos(0);
	CRect reViewSize;
	GetClientRect(reViewSize);
	reViewSize.DeflateRect(10, 41, 10, reViewSize.Height()/3);
	hDWP = DeferWindowPos( hDWP, m_ImageViewForOcr.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0 );

	EndDeferWindowPos(hDWP);
	UnlockWindowUpdate();

	return 0;
}

HBRUSH TeachOCRFont::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_FONTTEACH_GROUP_SEGMENTINFO:

	case IDC_FONTTEACH_LABEL_POSNUM:

	case IDC_FONTTEACH_LABEL_MINWIDTH:
	case IDC_FONTTEACH_LABEL_MAXWIDTH:
	case IDC_FONTTEACH_LABEL_MINHEIGHT:
	case IDC_FONTTEACH_LABEL_MAXHEIGHT:

	case IDC_CHECK_MANUAL_TEACHING_MODE:

		pDC->SetBkMode(TRANSPARENT);
		return m_brushBody;
		break;
	}

	return hbr;
}

void APIENTRY TeachOCRFont::ImageViewDrawExt_OCR( IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData )
{
	TeachOCRFont* pOCRFontDlg = (TeachOCRFont*)lpUsrData;
	return;
}

BOOL APIENTRY TeachOCRFont::ImageViewEvent_OCR(ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData)
{
	if(evt == ImageViewEvent::ButtonEventLoadClick)
	{
		TeachOCRFont* pOCRFontDlg = (TeachOCRFont*)lpUsrData;

		CString strSegmentFolderPath; 
		strSegmentFolderPath.Format(_T("D:\\Label_Segment"));
	
		CString strFilter;
		strFilter = _T("Bitmap Files(*.bmp)|*.bmp|All Files(*.*)|*.*|");
		CFileDialog FileDialog(TRUE, _T("*.bmp"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter, pOCRFontDlg);
		FileDialog.m_ofn.lpstrInitialDir = strSegmentFolderPath;


		if(FileDialog.DoModal() == IDOK)
		{
			CxImageObject* pImageObject = pOCRFontDlg->GetObject_TeachOCR();

			if(!pImageObject->LoadFromFile( FileDialog.GetPathName()))
			{
				::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), _T("Cannot open image!"), _T("Error"), MB_OK|MB_ICONSTOP);
				return TRUE;
			}

			pOCRFontDlg->m_Combo_Option1.SetCurSel(0);

			pOCRFontDlg->m_ImageViewForOcr.SetImageObject(pImageObject, FALSE);	
			pOCRFontDlg->m_ImageViewForOcr.GetGraphicObject().Reset();		

			pOCRFontDlg->m_OCRFontLoadData.clear();
			pOCRFontDlg->m_OCRFontReadingData.clear();

			CString strImageFilePath = FileDialog.GetPathName();
			
			pOCRFontDlg->CheckImageNameFormat(strImageFilePath);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL APIENTRY TeachOCRFont::ImageViewMouseEvent_OCR( DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData )
{
	TeachOCRFont* pOCRFontDlg = (TeachOCRFont*)lpUsrData;
	CPoint ptImagePoint = pOCRFontDlg->m_ImageViewForOcr.GetIDeviceContext()->MousePosToImagePos(point.x, point.y);

	
	//if( dwMsg == WM_LBUTTONDOWN  )
	//{							  
	//	pOCRFontDlg->m_FontSizeViewPosition = ptImagePoint;
	//	pOCRFontDlg->ViewGraphicObjectUpdate();
	//}

	if( dwMsg == WM_LBUTTONDBLCLK )
	{	 
		for( int nAreaNum = 0; nAreaNum < (int)pOCRFontDlg->m_OCRFontReadingData.vreReadingArea.size(); nAreaNum++)
		{
			if( pOCRFontDlg->m_OCRFontReadingData.vreReadingArea.at(nAreaNum).PtInRect( ptImagePoint ) )
			{
				pOCRFontDlg->AddFontData(nAreaNum, ptImagePoint);
				break;
			}
		}
	}

	return FALSE;
}

void TeachOCRFont::SegmentFileDataLoad()
{
	m_SegmentFileData.clear();

	CString strSegmentPath;
	strSegmentPath = _T("D:\\Label_Segment");
	if (IsExistFile((LPCTSTR)strSegmentPath))
	{
		CFileFind Filefinder;
		BOOL bFind = Filefinder.FindFile(strSegmentPath + _T("\\*.*"));
		while(bFind)  
		{ 	
			bFind = Filefinder.FindNextFile();
			if(Filefinder.IsArchived())	 
			{
				CString strFileName = Filefinder.GetFileName();
				if( strFileName == _T(".") || strFileName == _T("..") ) continue;	

				stSegmentFileData DataBuff;
				DataBuff.clear();

				AfxExtractSubString(DataBuff.strSegmentLineNum,	strFileName, 1, '_');
				DataBuff.strFilePath = strSegmentPath + _T("\\") + strFileName;
				m_SegmentFileData.push_back(DataBuff);
			}
		}
	}
}

void TeachOCRFont::CheckImageNameFormat(CString strFilePath)
{
	BOOL bCheckFilePath = FALSE;
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		if( m_SegmentFileData.at(nIndex).strFilePath == strFilePath )
		{
			bCheckFilePath = TRUE;
			break;
		}
	}

	if(!bCheckFilePath)
	{
		AfxMessageBox(_T("Image File 경로를 확인하세요!\nD:/Label_Segment Folder Image만 사용 가능합니다."), MB_ICONERROR);
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	m_OCRFontLoadData.clear();
	m_OCRFontLoadData.strFilePath = strFilePath;

	try // ----- Check Image Name Format -----			
	{	
		CString strDelimiter = _T("Segment_");
		int nFindIndex = strFilePath.Find( strDelimiter );
		if( nFindIndex == -1 ) throw FALSE;

		CString strFileName = strFilePath.Mid( nFindIndex + (int)strDelimiter.GetLength(), strFilePath.GetLength() );
		AfxExtractSubString( strFileName, strFileName, 0, '.' );

		int nImageType = 0;
		for( int nIndex = 0; nIndex < LABLE_INSPECTION_MAX_IMAGE_TYPE; nIndex++ )
		{
			nFindIndex = strFileName.Find( strImgSubCode[nIndex] );
			if( nFindIndex != -1 )
			{
				strFileName.Replace(strImgSubCode[nIndex], _T(""));
				nImageType = nIndex;
				break;
			}
		}

		if( strFileName.GetLength() != 15 ) throw FALSE;

		CString strOCRFontSize;
		AfxExtractSubString(m_OCRFontLoadData.strSegmentLineNum,	strFileName, 0, '_');	
		AfxExtractSubString(strOCRFontSize,							strFileName, 1, '_');
		m_OCRFontLoadData.strMinWidth = strOCRFontSize.Mid(0, 3);
		m_OCRFontLoadData.strMaxWidth = strOCRFontSize.Mid(3, 3);
		m_OCRFontLoadData.strMinHeight= strOCRFontSize.Mid(6, 3);
		m_OCRFontLoadData.strMaxHeight= strOCRFontSize.Mid(9, 3);

		if( m_OCRFontLoadData.strSegmentLineNum .IsEmpty() ||
			m_OCRFontLoadData.strMinWidth 		.IsEmpty() || m_OCRFontLoadData.strMaxWidth .IsEmpty() ||
			m_OCRFontLoadData.strMinHeight		.IsEmpty() || m_OCRFontLoadData.strMaxHeight.IsEmpty() ) throw FALSE;

		m_OCRFontLoadData.bSegmentFileLoad = TRUE;
		m_ImageViewForOcr.SetTitle(strFilePath, FALSE);

		OCRSegmentImgTypeCheck(nImageType);
		OCRSegmentReading();
		////////////////////////////////////////////////////////////////////////////////////////////////////	
	}
	catch (BOOL bLoadResult)
	{
		if(!bLoadResult)
		{
			m_OCRFontLoadData.bSegmentFileLoad = FALSE;
			AfxMessageBox(_T("Segment Image Name Error!!\nImage File을 확인하세요!!"), MB_ICONERROR);	
		}
	}
}

void TeachOCRFont::AddFontData(int nSelectIndex, CPoint ptSelectPos)
{
	TeachAddFont AddFontDlg;

	AddFontDlg.nSegmentIndex	= nSelectIndex;
	AddFontDlg.strReadingText	= m_OCRFontReadingData.vstrReadingText.at(nSelectIndex);
	AddFontDlg.m_TeachingText	= m_OCRFontReadingData.vstrTeachingText.at(nSelectIndex);

	if (AddFontDlg.DoModal() != IDOK) return;

	CString strTeachingData = AddFontDlg.m_TeachingText;
	if(!strTeachingData.IsEmpty())
	{
		m_OCRFontReadingData.vbTeaching.at(nSelectIndex) = TRUE;
		m_OCRFontReadingData.vstrTeachingText.at(nSelectIndex) = strTeachingData;
	}
	else
	{
		m_OCRFontReadingData.vbTeaching.at(nSelectIndex) = FALSE;
		m_OCRFontReadingData.vstrTeachingText.at(nSelectIndex) = _T("");
	}

	ViewGraphicObjectUpdate();
}

void TeachOCRFont::OCRSegmentImgTypeCheck(int nImageType)
{
	m_Combo_Option1.ResetContent();
	m_nImageType = -1;

	m_Combo_Option2.SetCurSel(0);
	m_Combo_Option3.SetCurSel(0);
	m_Combo_Option4.SetCurSel(0);
	m_Combo_Option5.SetCurSel(0);

	m_Check_ManualTeachingMode.SetCheck(FALSE);
	OnBnClickedCheckManualTeachingMode();
	
	std::vector<int> SamePosIndex; SamePosIndex.clear();
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
		if( m_SegmentFileData.at(nIndex).strSegmentLineNum == m_OCRFontLoadData.strSegmentLineNum )
			SamePosIndex.push_back(nIndex);

	if( !(0 < (int)SamePosIndex.size() && (int)SamePosIndex.size() <= LABLE_INSPECTION_MAX_IMAGE_TYPE) ) return;

	for(int nIndex = 0; nIndex < (int)SamePosIndex.size(); nIndex++)
	{
		CString strPathBuff = m_SegmentFileData.at( SamePosIndex.at(nIndex) ).strFilePath;

		for( int nSubCodeIndex = 0; nSubCodeIndex < LABLE_INSPECTION_MAX_IMAGE_TYPE; nSubCodeIndex++ )
		{
			int nFindIndex = strPathBuff.Find( strImgSubCode[nSubCodeIndex] );
			if(nFindIndex != -1)
			{
				switch (nSubCodeIndex)
				{
				case 0: m_Combo_Option1.AddString(_T("Center Cut Img"));	break;
				case 1: m_Combo_Option1.AddString(_T("Algorithm Cut Img"));	break;
				case 2: m_Combo_Option1.AddString(_T("Original Img"));		break;
				default: break;
				}
				break;
			}
		}
	}	

	//////////////////////////////////////////////////
	int nComboCount = (int)m_Combo_Option1.GetCount();

	if( nComboCount  < 0 ) return;
	if( nComboCount == 1 )
	{
		m_Combo_Option1.SetCurSel(0);
		m_nImageType = 0;
	}

	//////////////////////////////////////////////////
	CString strLodeImgType;
	switch (nImageType)
	{
	case 0:		strLodeImgType = _T("Center Cut Img");		break;
	case 1:		strLodeImgType = _T("Algorithm Cut Img");	break;
	case 2:		strLodeImgType = _T("Original Img");		break;
	default:	strLodeImgType = _T("Error");				break;
	}

	//////////////////////////////////////////////////
	for(int nIndex = 0; nIndex < nComboCount; nIndex++)
	{
		CString strComboText;
		m_Combo_Option1.GetLBText(nIndex, strComboText);

		if( strLodeImgType == strComboText )
		{
			m_Combo_Option1.SetCurSel(nIndex);
			m_nImageType = nIndex;
			break;
		}
	}
}

void TeachOCRFont::OCRSegmentReading()
{
	USES_CONVERSION;
	if( &m_ImageObjectForOcr == NULL || m_ImageObjectForOcr.GetImageBuffer() == NULL ) return;

	// ----- UI Info Update -----
	CString strBuff;
	strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strSegmentLineNum));
	GetDlgItem(IDC_FONTTEACH_EDIT_POSNUM	)->SetWindowText(strBuff);
	
	// ----- OCR Reading -----
	EImageBW8 BW8SegmentImg, BW8Buff;
	BW8SegmentImg.SetImagePtr(m_ImageObjectForOcr.GetWidth(), m_ImageObjectForOcr.GetHeight(), m_ImageObjectForOcr.GetImageBuffer(), 0);

	BW8Buff.SetSize(BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());
	EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentImg, &BW8Buff);

	m_LabelData.clear();
	m_LabelData.reLineArea = CRect(0, 0, BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());

	std::vector<READ_DATA_Label> MasterData; MasterData.clear();
	VisionProcess::SegmentReadingOption stReadingOption; stReadingOption.clear();
	SetReadingOption(stReadingOption, FALSE);

	if( !CVisionSystem::Instance()->OCRMeasureInspection(BW8Buff, m_LabelData, MasterData, strFontPatch, stReadingOption) )
	{
		AfxMessageBox(_T("Segment Reading Error!!"), MB_ICONERROR);	

		WRITE_LOG(WL_ERROR, _T("No Font File in ReadOcr()"));
		return;
	}

	// ----- Read Data Setting -----
	m_OCRFontReadingData.clear();
	m_OCRFontReadingData.strReadingResult = m_LabelData.strLineText;
	m_OCRFontReadingData.vstrReadingText  = m_LabelData.strSegmentText;
	m_OCRFontReadingData.vreReadingArea	  = m_LabelData.reSegmentArea;
	for( int nIndex = 0; nIndex < (int)m_LabelData.reSegmentArea.size(); nIndex++ )
	{
		m_OCRFontReadingData.vbTeaching.push_back(FALSE);
		m_OCRFontReadingData.vstrTeachingText.push_back(_T(""));
	}


	OCRSegmentBtnCheck();
	ViewGraphicObjectUpdate();
}

void TeachOCRFont::OCRSegmentBtnCheck()
{
	USES_CONVERSION;

	CString strSegmentNum;
	GetDlgItem(IDC_FONTTEACH_EDIT_POSNUM)->GetWindowText(strSegmentNum);
	int nSegmentNum = _ttoi(strSegmentNum);

	BOOL bPrevBtnActivate = FALSE, bNextBtmActivate = FALSE;
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		int nLoadDataNum = _ttoi( m_SegmentFileData.at(nIndex).strSegmentLineNum );

		if(!bPrevBtnActivate)
			if( nSegmentNum > nLoadDataNum ) bPrevBtnActivate = TRUE;

		if(!bNextBtmActivate)
			if( nSegmentNum < nLoadDataNum ) bNextBtmActivate = TRUE;

		if(bPrevBtnActivate && bNextBtmActivate) break;
	}

	GetDlgItem(IDC_BUTTON_RETRYPREV)->EnableWindow(bPrevBtnActivate);
	GetDlgItem(IDC_BUTTON_RETRYNEXT)->EnableWindow(bNextBtmActivate);
}

void TeachOCRFont::ViewGraphicObjectUpdate()
{
	m_ImageViewForOcr.GetGraphicObject().Reset();

	COLORBOX	clrBox;
	COLORTEXT	clrText;
	COLORREF	clrColor;

	for(int nSegmentNum = 0; nSegmentNum < (int)m_OCRFontReadingData.vreReadingArea.size(); nSegmentNum++)
	{		
		CRect reBuff = m_OCRFontReadingData.vreReadingArea.at(nSegmentNum);

		if(m_OCRFontReadingData.vbTeaching.at(nSegmentNum)) 
		{
			clrColor = PDC_YELLOW;

			clrText.SetText( m_OCRFontReadingData.vstrTeachingText.at(nSegmentNum) );
			clrText.CreateObject( PDC_RED, reBuff.left, reBuff.top - 5, 20, TRUE, CxGOText::TextAlignment::TextAlignmentRight );
			m_ImageViewForOcr.GetGraphicObject().AddDrawText( clrText );
		}
		else clrColor = PDC_LIGHTGREEN;

		clrBox.CreateObject( clrColor, reBuff, PS_SOLID, 2 );
		m_ImageViewForOcr.GetGraphicObject().AddDrawBox( clrBox );

		clrText.SetText( _T("%d"),nSegmentNum );
		clrText.CreateObject( clrColor, reBuff.CenterPoint().x, reBuff.CenterPoint().y, 20, TRUE, CxGOText::TextAlignment::TextAlignmentCenter );
		m_ImageViewForOcr.GetGraphicObject().AddDrawText( clrText );

		clrText.SetText( m_OCRFontReadingData.vstrReadingText.at(nSegmentNum) );
		clrText.CreateObject( PDC_BLUE, reBuff.right, reBuff.top - 5, 20, TRUE, CxGOText::TextAlignment::TextAlignmentLeft );
		m_ImageViewForOcr.GetGraphicObject().AddDrawText( clrText );
	}

	clrText.SetText( m_OCRFontReadingData.strReadingResult );
	clrText.CreateObject( PDC_GREEN, 10, 20, 20, FALSE );
	m_ImageViewForOcr.GetGraphicObject().AddDrawText( clrText, 0 );

	if( TRUE ) // Font Size Area View
	{
		CString strBuff;
		CSize MinSize, MaxSize;
		GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH	)->GetWindowText( strBuff );	MinSize.cx = _ttoi(strBuff);
		GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT	)->GetWindowText( strBuff );	MinSize.cy = _ttoi(strBuff);
		GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH	)->GetWindowText( strBuff );	MaxSize.cx = _ttoi(strBuff);
		GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT	)->GetWindowText( strBuff );	MaxSize.cy = _ttoi(strBuff);	

		m_FontSizeViewPosition.x = 100;
		m_FontSizeViewPosition.y = (int)( (float)m_ImageObjectForOcr.GetHeight() / 2.f );

		CRect reMinArea, reMaxArea;
		reMinArea.SetRectEmpty();	reMinArea.OffsetRect(m_FontSizeViewPosition.x, m_FontSizeViewPosition.y);
		reMaxArea.SetRectEmpty();  	reMaxArea.OffsetRect(m_FontSizeViewPosition.x, m_FontSizeViewPosition.y);
		reMinArea.DeflateRect( -(int)((float)MinSize.cx/2.f), -(int)((float)MinSize.cy/2.f), -(int)((float)MinSize.cx/2.f), -(int)((float)MinSize.cy/2.f) );
		reMaxArea.DeflateRect( -(int)((float)MaxSize.cx/2.f), -(int)((float)MaxSize.cy/2.f), -(int)((float)MaxSize.cx/2.f), -(int)((float)MaxSize.cy/2.f) );

		clrBox.CreateObject( PDC_ORANGERED, reMinArea, PS_SOLID, 2 );
		m_ImageViewForOcr.GetGraphicObject().AddDrawBox( clrBox );

		clrBox.CreateObject( PDC_ORANGE, reMaxArea, PS_SOLID, 2 );
		m_ImageViewForOcr.GetGraphicObject().AddDrawBox( clrBox );
	}

	m_ImageViewForOcr.ImageUpdate();
}

BOOL TeachOCRFont::SetReadingOption(VisionProcess::SegmentReadingOption &stReadingOption, BOOL bTeachingMode)
{
	int nImageCount = (int)m_Combo_Option1.GetCount();
	if (nImageCount == 1)	stReadingOption.nImageType = 5;
	else					stReadingOption.nImageType = (int)m_Combo_Option1.GetCurSel();

	stReadingOption.bUseMaxThreshold			= (int)m_Combo_Option2.GetCurSel();
	stReadingOption.bUseImageFiltering			= (int)m_Combo_Option3.GetCurSel();
	stReadingOption.bUseFontSizeFiltering		= (int)m_Combo_Option4.GetCurSel();
	stReadingOption.bUseeVisionLargeCharsCut	= (int)m_Combo_Option5.GetCurSel();

	stReadingOption.bUseFontsizeManualSetting	= m_Check_ManualTeachingMode.GetCheck();
	if(m_Check_ManualTeachingMode.GetCheck())
	{
		CString strBuff;
		GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH )->GetWindowText( strBuff );	stReadingOption.FontMinSize.cx	= _ttoi(strBuff);
		GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT)->GetWindowText( strBuff );	stReadingOption.FontMinSize.cy	= _ttoi(strBuff);
		GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH )->GetWindowText( strBuff );	stReadingOption.FontMaxSize.cx	= _ttoi(strBuff);
		GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT)->GetWindowText( strBuff );	stReadingOption.FontMaxSize.cy	= _ttoi(strBuff);
	}

	stReadingOption.bTeachingMode = bTeachingMode;
	
	return TRUE;
}

void TeachOCRFont::SetManualOptionList()
{
	CString strBuff;
	for( int nOptionNo = 0; nOptionNo < OP_MAX; nOptionNo++ )
	{
		CModelInfo::stLabelManualOptionInfo& stLabelOptionInfo = CModelInfo::Instance()->GetLabelManualOptionInfo( (Labe_Reading_Option)nOptionNo );
		if( /*stLabelOptionInfo.nImageTypeIndex == -1 ||*/ stLabelOptionInfo.nSegmentLine_Num == -1 )	strBuff.Format(_T( "Option_%02d_None" ), nOptionNo+1);
		else																							strBuff.Format(_T( "Option_%02d  //  Position_%02d" ), nOptionNo+1, stLabelOptionInfo.nSegmentLine_Num);

		m_Combo_ManualOption.AddString(strBuff); 
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void TeachOCRFont::OnCbnSelchangeFontteachComboOption1()
{
	int nSelectNum = m_Combo_Option1.GetCurSel();
	if( m_nImageType == nSelectNum || m_nImageType == -1 || 4 < m_nImageType ) return;	

	CString strLineNum = _T("Error");
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		if( m_SegmentFileData.at(nIndex).strFilePath == m_OCRFontLoadData.strFilePath )
		{
			strLineNum = m_SegmentFileData.at(nIndex).strSegmentLineNum;
			break;
		}
	}
	if( strLineNum == _T("Error") ) return;

	std::vector<int> SameLinePos; SameLinePos.clear();
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		if( m_SegmentFileData.at(nIndex).strSegmentLineNum == strLineNum )
			SameLinePos.push_back(nIndex);
	}

	for(int nIndex = 0; nIndex < (int)SameLinePos.size(); nIndex++)
	{
		int nFindIndex = m_SegmentFileData.at(SameLinePos.at(nIndex)).strFilePath.Find( strImgSubCode[nSelectNum] );
		if( nFindIndex == -1 ) continue;

		SegmentImageChange( m_SegmentFileData.at(SameLinePos.at(nIndex)).strFilePath );
		break;
	}
}

void TeachOCRFont::OnCbnSelchangeFontteachComboOption2()
{
	int nSelectNum = m_Combo_Option2.GetCurSel();

	if( !(nSelectNum == 0 || nSelectNum == 1) )
		AfxMessageBox(_T("Combo Box Data Error!!"), MB_ICONERROR);

	OCRSegmentReading();
}

void TeachOCRFont::OnCbnSelchangeFontteachComboOption3()
{
	int nSelectNum = m_Combo_Option3.GetCurSel();

	if( !(nSelectNum == 0 || nSelectNum == 1) )
		AfxMessageBox(_T("Combo Box Data Error!!"), MB_ICONERROR);

	OCRSegmentReading();
}

void TeachOCRFont::OnCbnSelchangeFontteachComboOption4()
{
	int nSelectNum = m_Combo_Option4.GetCurSel();

	if( !(nSelectNum == 0 || nSelectNum == 1) )
		AfxMessageBox(_T("Combo Box Data Error!!"), MB_ICONERROR);

	OCRSegmentReading();
}

void TeachOCRFont::OnCbnSelchangeFontteachComboOption5()
{
	int nSelectNum = m_Combo_Option5.GetCurSel();

	if( !(nSelectNum == 0 || nSelectNum == 1) )
		AfxMessageBox(_T("Combo Box Data Error!!"), MB_ICONERROR);

	OCRSegmentReading();
}

void TeachOCRFont::OnBnClickedCheckManualTeachingMode()
{
	m_Combo_ManualOption.ResetContent();
	m_Combo_Option4.SetCurSel(0);

	GetDlgItem(IDC_FONTTEACH_COMBO_OPTION4				)->EnableWindow( !m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_COMBO_MANUAL_OPTION		)->EnableWindow(  m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_APPLY	)->EnableWindow(  m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_SAVE		)->EnableWindow(  m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_DELETE	)->EnableWindow(  m_Check_ManualTeachingMode.GetCheck() );

	GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH	)->SendMessage( EM_SETREADONLY, !m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT	)->SendMessage( EM_SETREADONLY, !m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH	)->SendMessage( EM_SETREADONLY, !m_Check_ManualTeachingMode.GetCheck() );
	GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT	)->SendMessage( EM_SETREADONLY, !m_Check_ManualTeachingMode.GetCheck() );
	
	if( !m_Check_ManualTeachingMode.GetCheck() )
	{
		CString strBuff;
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMinWidth));		GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMinHeight));	GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMaxWidth));		GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMaxHeight));	GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT	)->SetWindowText( strBuff );	

		OCRSegmentReading();
	}
	else SetManualOptionList();
}

void TeachOCRFont::OnCbnSelchangeFontteachComboManualOption()
{
	int nOptionIndex = m_Combo_ManualOption.GetCurSel();
	if( nOptionIndex < 0 ) return;
	
	GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_APPLY	)->EnableWindow( TRUE );
	GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_SAVE		)->EnableWindow( TRUE );
	GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_DELETE	)->EnableWindow( TRUE );
	
	CString strBuff;
	CModelInfo::stLabelManualOptionInfo& stLabelOptionInfo = CModelInfo::Instance()->GetLabelManualOptionInfo( (Labe_Reading_Option)nOptionIndex );

	if( stLabelOptionInfo.nImageTypeIndex == -1 || stLabelOptionInfo.nSegmentLine_Num == -1 )
	{
		m_Combo_Option2.SetCurSel(0);
		m_Combo_Option3.SetCurSel(0);
		m_Combo_Option5.SetCurSel(0);

		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMinWidth));		GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMinHeight));	GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMaxWidth));		GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), _ttoi(m_OCRFontLoadData.strMaxHeight));	GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT	)->SetWindowText( strBuff );

		OCRSegmentReading();
	}
	else if( _ttoi(m_OCRFontLoadData.strSegmentLineNum) == stLabelOptionInfo.nSegmentLine_Num )
	{
		// stLabelOptionInfo.nImageTypeIndex;	// No Check
		// stLabelOptionInfo.nSegmentLine_Num;	// No Check
		m_Combo_Option2.SetCurSel( (int)stLabelOptionInfo.bUseMaxThreshold			);
		m_Combo_Option3.SetCurSel( (int)stLabelOptionInfo.bUseImageFiltering		);	
		m_Combo_Option5.SetCurSel( (int)stLabelOptionInfo.bUseeVisionLargeCharsCut	);

		strBuff.Format(_T("%d"), stLabelOptionInfo.FontMinSize.cx);	GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), stLabelOptionInfo.FontMinSize.cy);	GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), stLabelOptionInfo.FontMaxSize.cx);	GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH	)->SetWindowText( strBuff );
		strBuff.Format(_T("%d"), stLabelOptionInfo.FontMaxSize.cy);	GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT	)->SetWindowText( strBuff );

		OCRSegmentReading();
	}
	else
	{
		GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_APPLY	)->EnableWindow( FALSE );
		GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_SAVE		)->EnableWindow( FALSE );
//		GetDlgItem(IDC_FONTTEACH_BTN_MANUAL_OPTION_DELETE	)->EnableWindow( FALSE );
	}
}

void TeachOCRFont::OnBnClickedFontteachBtnManualOptionApply()
{
	int nOptionIndex = m_Combo_ManualOption.GetCurSel();
	if( nOptionIndex < 0 ) return;

	CString strBuff;
	CModelInfo::stLabelManualOptionInfo& stLabelOptionInfo = CModelInfo::Instance()->GetLabelManualOptionInfo( (Labe_Reading_Option)nOptionIndex );

	stLabelOptionInfo.nImageTypeIndex			= m_Combo_Option1.GetCurSel();
	stLabelOptionInfo.nSegmentLine_Num			= _ttoi(m_OCRFontLoadData.strSegmentLineNum);
	stLabelOptionInfo.bUseMaxThreshold			= (BOOL)m_Combo_Option2.GetCurSel();
	stLabelOptionInfo.bUseImageFiltering		= (BOOL)m_Combo_Option3.GetCurSel();
	stLabelOptionInfo.bUseeVisionLargeCharsCut	= (BOOL)m_Combo_Option5.GetCurSel();

	GetDlgItem(IDC_FONTTEACH_EDIT_MINWIDTH	)->GetWindowText( strBuff ); stLabelOptionInfo.FontMinSize.cx = _ttoi(strBuff);
	GetDlgItem(IDC_FONTTEACH_EDIT_MINHEIGHT	)->GetWindowText( strBuff ); stLabelOptionInfo.FontMinSize.cy = _ttoi(strBuff);
	GetDlgItem(IDC_FONTTEACH_EDIT_MAXWIDTH	)->GetWindowText( strBuff ); stLabelOptionInfo.FontMaxSize.cx = _ttoi(strBuff);
	GetDlgItem(IDC_FONTTEACH_EDIT_MAXHEIGHT	)->GetWindowText( strBuff ); stLabelOptionInfo.FontMaxSize.cy = _ttoi(strBuff);

	OCRSegmentReading();
}

void TeachOCRFont::OnBnClickedFontteachBtnManualOptionSave()
{
	int nOptionIndex = m_Combo_ManualOption.GetCurSel();

	CString strMessage;
	strMessage.Format(_T("Do you Want Option_%02d Data Save?"), nOptionIndex);
	if( AfxMessageBox(strMessage, MB_YESNO) != IDYES ) return;

	OnBnClickedFontteachBtnManualOptionApply();

	CString strLabelManualInfoFile_Customer = _T("");
#ifndef RELEASE_4G
	strLabelManualInfoFile_Customer = CModelInfo::Instance()->GetRecipePath(CModelInfo::Instance()->GetModelNameLabel()) + _T("4G_MBBLabeManualOption.ini");
#elif RELEASE_6G
	strLabelManualInfoFile_Customer = CModelInfo::Instance()->GetRecipePath(CModelInfo::Instance()->GetModelNameLabel()) + _T("6G_BoxLabeManualOption.ini");
#endif

	CModelInfo::Instance()->SaveLabelManualOptionInfo_Public( strLabelManualInfoFile_Customer ); 
	CModelInfo::Instance()->LoadLabelManualOptionInfo_Public( strLabelManualInfoFile_Customer );

	OnBnClickedCheckManualTeachingMode();
	m_Combo_ManualOption.SetCurSel(nOptionIndex);
	OnCbnSelchangeFontteachComboManualOption();
}

void TeachOCRFont::OnBnClickedFontteachBtnManualOptionDelete()
{
	int nOptionIndex = m_Combo_ManualOption.GetCurSel();
	if( nOptionIndex < 0 ) return;

	CString strMessage;
	strMessage.Format(_T("Do you Want Option_%02d Data Delete?"), nOptionIndex);
	if( AfxMessageBox(strMessage, MB_YESNO) != IDYES ) return;

	CModelInfo::stLabelManualOptionInfo& stLabelOptionInfo = CModelInfo::Instance()->GetLabelManualOptionInfo( (Labe_Reading_Option)nOptionIndex );
	stLabelOptionInfo.Clear();

#ifndef RELEASE_2G
	CString strLabelManualInfoFile_Customer = CModelInfo::Instance()->GetRecipePath(CModelInfo::Instance()->GetModelName()) + _T("1G_AlbagLabeManualOption.ini");
#else
	CString strLabelManualInfoFile_Customer = CModelInfo::Instance()->GetRecipePath(CModelInfo::Instance()->GetModelName()) + _T("2G_BoxLabeManualOption.ini");
#endif

	CModelInfo::Instance()->SaveLabelManualOptionInfo_Public( strLabelManualInfoFile_Customer );
	CModelInfo::Instance()->LoadLabelManualOptionInfo_Public( strLabelManualInfoFile_Customer );

	OnBnClickedCheckManualTeachingMode();
	m_Combo_ManualOption.SetCurSel(nOptionIndex);
	OnCbnSelchangeFontteachComboManualOption();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void TeachOCRFont::OnBnClickedButtonRetryprev()
{
	int nPositionIndex = -1;
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		if( m_SegmentFileData.at(nIndex).strFilePath == m_OCRFontLoadData.strFilePath )
		{
			nPositionIndex = nIndex;
			break;
		}
	}
	if(nPositionIndex == -1) 
	{
		AfxMessageBox(_T("Segment File Data가 변경되었습니다.\nRefresh후 다시 실행하세요."), MB_ICONERROR);
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	int nChangePosition = -1;
	for(int nIndex = nPositionIndex-1; 0 <= nIndex; nIndex--)
	{
		if( m_SegmentFileData.at(nIndex).strSegmentLineNum != m_OCRFontLoadData.strSegmentLineNum )
		{
			nChangePosition = nIndex;
			break;
		}
		else continue;
	}
	if(nChangePosition == -1) 
	{
		AfxMessageBox(_T("Segment File Data가 변경되었습니다.\nRefresh후 다시 실행하세요."), MB_ICONERROR);
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	for(int nIndex = nChangePosition-1; 0 <= nIndex; nIndex--)
	{
		if( m_SegmentFileData.at(nIndex).strSegmentLineNum == m_SegmentFileData.at(nChangePosition).strSegmentLineNum )
			nChangePosition--;
		else break;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	SegmentImageChange(m_SegmentFileData.at(nChangePosition).strFilePath);
}

void TeachOCRFont::OnBnClickedButtonRetrynext()
{
	int nPositionIndex = -1;
	for(int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		if( m_SegmentFileData.at(nIndex).strFilePath == m_OCRFontLoadData.strFilePath )
		{
			nPositionIndex = nIndex;
			break;
		}
	}
	if(nPositionIndex == -1) 
	{
		AfxMessageBox(_T("Segment File Data가 변경되었습니다.\nRefresh후 다시 실행하세요."), MB_ICONERROR);
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	int nChangePosition = -1;
	for(int nIndex = nPositionIndex+1; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		if( m_SegmentFileData.at(nIndex).strSegmentLineNum != m_OCRFontLoadData.strSegmentLineNum )
		{
			nChangePosition = nIndex;
			break;
		}
		else continue;
	}
	if(nChangePosition == -1) 
	{
		AfxMessageBox(_T("Segment File Data가 변경되었습니다.\nRefresh후 다시 실행하세요."), MB_ICONERROR);
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	for(int nIndex = nChangePosition-1; 0 <= nIndex; nIndex--)
	{
		if( m_SegmentFileData.at(nIndex).strSegmentLineNum == m_SegmentFileData.at(nChangePosition).strSegmentLineNum )
			nChangePosition--;
		else break;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	SegmentImageChange(m_SegmentFileData.at(nChangePosition).strFilePath);
}

void TeachOCRFont::SegmentImageChange(CString strImagePath)
{
	if(!GetObject_TeachOCR()->LoadFromFile(strImagePath))
	{
		::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), _T("Cannot open image!"), _T("Error"), MB_OK|MB_ICONSTOP);
		return;
	}

	m_Combo_Option1.SetCurSel(0);

	m_ImageViewForOcr.SetImageObject(GetObject_TeachOCR(), FALSE);	
	m_ImageViewForOcr.GetGraphicObject().Reset();

	m_OCRFontLoadData.clear();
	m_OCRFontReadingData.clear();

	CheckImageNameFormat(strImagePath);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


void TeachOCRFont::OnBnClickedFontteachBtnFontsave()
{	
	USES_CONVERSION;

	if(AfxMessageBox(_T("Teaching Data를 저장하시겠습니까?"), MB_YESNO) != IDYES ) return;
	
	// ----- Time Data -----
	CTime	cTimeData = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%02d%02d%02d%02d%02d%02d"), cTimeData.GetYear(), cTimeData.GetMonth(), cTimeData.GetDay(), cTimeData.GetHour(), cTimeData.GetMinute(), cTimeData.GetSecond());	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ----- Teaching Data Check -----
	for(int nFontNum = 0; nFontNum < (int)m_OCRFontReadingData.vbTeaching.size(); nFontNum++)
	{
		if( m_OCRFontReadingData.vbTeaching.at(nFontNum) ) break;	
		else if( nFontNum == (int)m_OCRFontReadingData.vbTeaching.size()-1)
		{
			AfxMessageBox(_T("폰트 티칭 데이터가 없습니다."), MB_ICONERROR);
			return;
		}
		else continue;
	}

	if( &m_ImageObjectForOcr == NULL || m_ImageObjectForOcr.GetImageBuffer() == NULL )	
	{
		AfxMessageBox(_T("Image Error!!"), MB_ICONERROR);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ----- Font File Copy(Backup) -----
	CStringA strFontFileA;
	strFontFileA = CW2A(strFontPatch);

	EOCR OCRFont;
	OCRFont.Load( T2A(strFontPatch) ); // strFontFileA.GetBuffer(strFontFileA.GetLength()));

	CString strFontCopyPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Font Data Backup");
	if (!IsExistFile((LPCTSTR)strFontCopyPatch)) MakeDirectory( (LPCTSTR)strFontCopyPatch );
	strFontCopyPatch += _T("\\Backup_LabelOCRFont_") + strTime + _T(".OCR");
	OCRFont.Save(T2A(strFontCopyPatch));

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ----- OCR Font Add -----
	EImageBW8 BW8SegmentImg;
	BW8SegmentImg.SetImagePtr(m_ImageObjectForOcr.GetWidth(), m_ImageObjectForOcr.GetHeight(), m_ImageObjectForOcr.GetImageBuffer(), 0);
	
	CVisionSystem::Instance()->SetOCRResult (m_OCRFontReadingData);

	m_LabelData.clear();
	m_LabelData.reLineArea = CRect(0, 0, BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EImageBW8 BW8Buff;
	BW8SegmentImg.SetImagePtr(m_ImageObjectForOcr.GetWidth(), m_ImageObjectForOcr.GetHeight(), m_ImageObjectForOcr.GetImageBuffer(), 0);

	BW8Buff.SetSize(BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());
	EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentImg, &BW8Buff);

	std::vector<READ_DATA_Label> MasterData; MasterData.clear();
	VisionProcess::SegmentReadingOption stReadingOption; stReadingOption.clear();
	SetReadingOption(stReadingOption, TRUE);

	if( !CVisionSystem::Instance()->OCRMeasureInspection(BW8Buff/*BW8SegmentImg*/, m_LabelData, MasterData, strFontPatch, stReadingOption) )
	{
		AfxMessageBox(_T("Add Font Error!!"), MB_ICONERROR);	
		return;
	}

	OCRSegmentReading();
}

void TeachOCRFont::OnBnClickedFontteachBtnViewrefresh()
{
	SegmentFileDataLoad();
}

void TeachOCRFont::OnBnClickedFontteachBtnTeachingDataClear()
{
	OCRSegmentReading();
}

void TeachOCRFont::OnBnClickedFontteachBtnReregisterFont()
{
	TeachOCRReRegistration ReRegistrationDlg;
	ReRegistrationDlg.SetOcrMode(OCR_LABEL);

	if (ReRegistrationDlg.DoModal() != IDOK) return;
}

void TeachOCRFont::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


void TeachOCRFont::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CRect reClient, reGroupBoxArea;
	//GetClientRect(reClient);
	//GetDlgItem(IDC_FONTTEACH_GROUP_SEGMENTINFO)->GetWindowRect(reGroupBoxArea);
	//ScreenToClient(reGroupBoxArea);
	//if( !reGroupBoxArea.PtInRect( point ) ) return;
	CDialogEx::OnLButtonDown(nFlags, point);
}