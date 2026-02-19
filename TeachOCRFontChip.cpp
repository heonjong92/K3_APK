// TeachOCRFontChip.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "TeachOCRFontChip.h"
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

IMPLEMENT_DYNAMIC(TeachOCRFontChip, CDialogEx)

using namespace VisionProcess;
using namespace Euresys::Open_eVision_2_11;

TeachOCRFontChip::TeachOCRFontChip(CWnd* pParent /*=NULL*/)
	: CDialogEx(TeachOCRFontChip::IDD, pParent)
{
	m_SegmentFileData.clear();
	m_OCRFontLoadData.clear();
	m_OCRFontReadingData.clear();

	strFontPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\ChipOCRFont.OCR");

	m_clrBorder = RGB(126, 27, 29);
	m_clrBody = RGB(255, 255, 255);
	m_nBorderWidth = 5;

	m_brushBody.CreateSolidBrush(m_clrBody);

	m_strTitle = _T("OCR Font Manager");

	m_pIconImage = GdipLoadImageFromRes(AfxGetResourceHandle(), _T("PNG"), PNG_MENU_ICONS); // PNG_HITS_LOGO
	m_nMenuIconSize = m_pIconImage->GetHeight();

	m_FontSizeViewPosition = CPoint(0, 0);
}

TeachOCRFontChip::~TeachOCRFontChip()
{
	CString strTeachingOCRPatch;
	strTeachingOCRPatch = strFontPatch;
	m_ImageObjectForOcr.Destroy();
}

void TeachOCRFontChip::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_FONTTEACH_BTN_FONTSAVE,			m_btnFontSave);
	DDX_Control(pDX, IDC_FONTTEACH_BTN_VIEWREFRESH,			m_btnViewRefresh);
	DDX_Control(pDX, IDC_FONTTEACH_BTN_REREGISTER_FONT,		m_btnReRegistration);
	DDX_Control(pDX, IDOK,									m_btnClose);
}


BEGIN_MESSAGE_MAP(TeachOCRFontChip, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK,									&TeachOCRFontChip::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_FONTSAVE,			&TeachOCRFontChip::OnBnClickedFontteachBtnFontsave)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_VIEWREFRESH,		&TeachOCRFontChip::OnBnClickedFontteachBtnViewrefresh)
	ON_BN_CLICKED(IDC_FONTTEACH_BTN_REREGISTER_FONT,	&TeachOCRFontChip::OnBnClickedFontteachBtnReregisterFont)

	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL TeachOCRFontChip::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set Button
	m_btnFontSave.SetFontHeight(15);
	m_btnViewRefresh.SetFontHeight(15);
	m_btnDataClear.SetFontHeight(15);
	m_btnReRegistration.SetFontHeight(15);
	m_btnClose.SetFontHeight(20);

	m_btnFontSave.SetColor(DEF_BTN_COLOR_SAVE);
	m_btnReRegistration.SetColor(RGB(51, 0, 153), RGB(102, 51, 204), RGB(153, 102, 255), PDC_LIGHTGRAY, PDC_WHITE);
	m_btnClose.SetColor(DEF_BTN_COLOR_MEASURE);

	m_btnFontSave.SetBold(TRUE);
	m_btnViewRefresh.SetBold(TRUE);
	m_btnDataClear.SetBold(TRUE);
	m_btnReRegistration.SetBold(TRUE);
	m_btnClose.SetBold(TRUE);

#ifdef RELEASE_1G
	CVisionSystem* pVisionSystem = CVisionSystem::Instance();
	CxImageObject* pImgObj = pVisionSystem->GetImageObject(CamTypeAreaScan, IDX_AREA4);

	m_ImageObjectForOcr.Clone(pImgObj);
	BYTE* pDstBuf = (BYTE*)m_ImageObjectForOcr.GetImageBuffer();
	int nWidthBytes = m_ImageObjectForOcr.GetWidthBytes();
	memcpy(pDstBuf, pImgObj->GetImageBuffer(), (size_t)nWidthBytes * m_ImageObjectForOcr.GetHeight());
	OCRSegmentReading();

	//SegmentFileDataLoad();
#endif // RELEASE_1G

	return TRUE;
}

void TeachOCRFontChip::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	using namespace Gdiplus;

	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcSplit2;
	GetDlgItem(IDC_SPLIT2)->GetWindowRect(rcSplit2);
	ScreenToClient(rcSplit2);

	int nTopOffset = 36;
	int nBorderWidth = m_nBorderWidth;

	CRect rcTitle = rcClient;
	rcTitle.bottom = rcTitle.top + nTopOffset;

	dc.FillSolidRect(rcClient.left, rcSplit2.top - rcClient.top, rcClient.Width(), rcClient.bottom, m_clrBody);

	dc.FillSolidRect(rcTitle, m_clrBorder);
	dc.FillSolidRect(rcClient.left, rcClient.bottom - nBorderWidth, rcClient.Width(), nBorderWidth, m_clrBorder);
	dc.FillSolidRect(rcClient.left, rcTitle.bottom, nBorderWidth, rcClient.bottom - rcTitle.bottom, m_clrBorder);
	dc.FillSolidRect(rcClient.right - nBorderWidth, rcTitle.bottom, nBorderWidth, rcClient.bottom - rcTitle.bottom, m_clrBorder);

	const int nLeftOffsetX = 8;

	Graphics g(dc.GetSafeHdc());

	int nIW = m_nMenuIconSize; // m_pIconImage->GetWidth();
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

	g.DrawImage(m_pIconImage, Rect(nIX, nIY, nIW, nIH), m_nMenuIconSize * 1, 0, nIW, nIH, UnitPixel, &ImgAttr);

	Color colorText(180, 0, 0, 0);
	SolidBrush brushText(colorText);
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetFormatFlags(StringFormatFlagsLineLimit | StringFormatFlagsNoWrap);
	stringFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	stringFormat.SetAlignment(StringAlignmentNear);
	SolidBrush brushTitle(Color(200, 255, 255, 255));
	Gdiplus::FontFamily* pFontFamily = UIExt::CResourceManager::SystemFontFamily();
	Gdiplus::Font fontTitle(pFontFamily, 15, FontStyleBold, UnitPixel);
	BSTR bstrTitle = m_strTitle.AllocSysString();
	g.DrawString(bstrTitle, -1, &fontTitle, RectF((float)rcTitle.left + nIX + nIW + 3, (float)rcTitle.top, (float)rcTitle.Width() - 8, (float)rcTitle.Height()), &stringFormat, &brushTitle);
	SysFreeString(bstrTitle);

	m_ImageViewForOcr.ImageUpdate();
}

int TeachOCRFontChip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwLineBtnTypes = MBT_ZOOM_INOUT | MBT_MEASURE | MBT_LOAD | MBT_SAVE;
	REGISTER_CALLBACK regCB;
	memset(&regCB, 0, sizeof(REGISTER_CALLBACK));
	regCB.fnOnDrawExt = ImageViewDrawExt_OCR;
	regCB.fnOnEvent = ImageViewEvent_OCR;
	regCB.fnOnFireMouseEvent = ImageViewMouseEvent_OCR;
	regCB.lpUsrData[0] = regCB.lpUsrData[1] = regCB.lpUsrData[2] = regCB.lpUsrData[3] = regCB.lpUsrData[4] = this;

	DWORD dwTitleBgColor = UIExt::CResourceManager::Instance()->GetViewerTitleBackgroundColor();
	DWORD dwTitleFgColor = UIExt::CResourceManager::Instance()->GetViewerTitleForegroundColor();
	DWORD dwStatusBgColor = UIExt::CResourceManager::Instance()->GetViewerStatusBackgroundColor();
	DWORD dwStatusFgColor = UIExt::CResourceManager::Instance()->GetViewerStatusForegroundColor();
	DWORD dwBodyColor = UIExt::CResourceManager::Instance()->GetViewerBodyColor();
	DWORD dwBtnColor = UIExt::CResourceManager::Instance()->GetViewerActiveButtonColor();

	m_ImageViewForOcr.Create(&m_ImageViewManagerForOcr, this);
	m_ImageViewForOcr.ShowWindow(SW_SHOW);
	m_ImageViewForOcr.ShowTitleWindow(TRUE);
	m_ImageViewForOcr.SetAnimateWindow(FALSE);
	m_ImageViewForOcr.SetMiniButtonTypeAll(dwLineBtnTypes);
	m_ImageViewForOcr.SetImageObject(&m_ImageObjectForOcr, FALSE);
	m_ImageViewForOcr.SetRegisterCallBack(0, &regCB);
	m_ImageViewForOcr.SetTitleColor(dwTitleBgColor, dwTitleFgColor, dwTitleFgColor);
	m_ImageViewForOcr.SetStatusColor(dwStatusBgColor, dwStatusFgColor);
	m_ImageViewForOcr.SetButtonColor(RGB(255, 255, 255), dwBtnColor, (DWORD)-1, (DWORD)-1);
	m_ImageViewForOcr.SetBodyColor(dwBodyColor);
	m_ImageViewForOcr.SetTitle(_T("View : Chip OCR Font"), FALSE);
	m_ImageViewForOcr.SetRealSizePerPixel(18.0001f);
	m_ImageViewForOcr.ShowScrollBar(TRUE);

	LockWindowUpdate();
	HDWP hDWP = BeginDeferWindowPos(0);
	CRect reViewSize;
	GetClientRect(reViewSize);
	reViewSize.DeflateRect(10, 41, 10, reViewSize.Height() / 3);
	hDWP = DeferWindowPos(hDWP, m_ImageViewForOcr.GetSafeHwnd(), NULL, reViewSize.left, reViewSize.top, reViewSize.Width(), reViewSize.Height(), 0);

	EndDeferWindowPos(hDWP);
	UnlockWindowUpdate();

	return 0;
}

HBRUSH TeachOCRFontChip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch ((short)pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC:
	case IDC_FONTTEACH_GROUP_SEGMENTINFO:
		pDC->SetBkMode(TRANSPARENT);
		return m_brushBody;
		break;
	}

	return hbr;
}

void APIENTRY TeachOCRFontChip::ImageViewDrawExt_OCR(IxDeviceContext* pIDC, CDC* pDC, UINT nIndexData, LPVOID lpUsrData)
{
	TeachOCRFontChip* pOCRFontDlg = (TeachOCRFontChip*)lpUsrData;
	return;
}

BOOL APIENTRY TeachOCRFontChip::ImageViewEvent_OCR(ImageViewEvent::Event evt, UINT nIndexData, LPVOID lpUsrData)
{
	if (evt == ImageViewEvent::ButtonEventLoadClick)
	{
		TeachOCRFontChip* pOCRFontDlg = (TeachOCRFontChip*)lpUsrData;

		CString strFilter;
		strFilter = _T("Bitmap Files(*.bmp)|*.bmp|All Files(*.*)|*.*|");
		CFileDialog FileDialog(TRUE, _T("*.bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, AfxGetMainWnd());

		if (FileDialog.DoModal() == IDOK)
		{
			CxImageObject* pImageObject = pOCRFontDlg->GetObject_TeachOCR();

			if (!pImageObject->LoadFromFile(FileDialog.GetPathName()))
			{
				::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), _T("Cannot open image!"), _T("Error"), MB_OK | MB_ICONSTOP);
				return TRUE;
			}

			pOCRFontDlg->m_ImageViewForOcr.SetImageObject(pImageObject, FALSE);
			pOCRFontDlg->m_ImageViewForOcr.GetGraphicObject().Reset();
			pOCRFontDlg->m_ImageViewForOcr.ImageUpdate();

			pOCRFontDlg->m_OCRFontLoadData.clear();
			pOCRFontDlg->m_OCRFontReadingData.clear();
		}

		pOCRFontDlg->OCRSegmentReading();
		return TRUE;
	}

	return FALSE;
}

BOOL APIENTRY TeachOCRFontChip::ImageViewMouseEvent_OCR(DWORD dwMsg, CPoint& point, UINT nIndexData, LPVOID lpUsrData)
{
	TeachOCRFontChip* pOCRFontDlg = (TeachOCRFontChip*)lpUsrData;
	CPoint ptImagePoint = pOCRFontDlg->m_ImageViewForOcr.GetIDeviceContext()->MousePosToImagePos(point.x, point.y);

	if (dwMsg == WM_LBUTTONDBLCLK)
	{
		for (int nAreaNum = 0; nAreaNum < (int)pOCRFontDlg->m_OCRFontReadingData.vreReadingArea.size(); nAreaNum++)
		{
			if (pOCRFontDlg->m_OCRFontReadingData.vreReadingArea.at(nAreaNum).PtInRect(ptImagePoint))
			{
				pOCRFontDlg->AddFontData(nAreaNum, ptImagePoint);
				break;
			}
		}
	}

	return FALSE;
}

void TeachOCRFontChip::AddFontData(int nSelectIndex, CPoint ptSelectPos)
{
	TeachAddFont AddFontDlg;

	AddFontDlg.nSegmentIndex = nSelectIndex;
	AddFontDlg.strReadingText = m_OCRFontReadingData.vstrReadingText.at(nSelectIndex);
	AddFontDlg.m_TeachingText = m_OCRFontReadingData.vstrTeachingText.at(nSelectIndex);

	if (AddFontDlg.DoModal() != IDOK) return;

	CString strTeachingData = AddFontDlg.m_TeachingText;
	if (!strTeachingData.IsEmpty())
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


void TeachOCRFontChip::OCRSegmentReading(BOOL bSave /*=FALSE*/)
{
	USES_CONVERSION;
	if (&m_ImageObjectForOcr == NULL || m_ImageObjectForOcr.GetImageBuffer() == NULL) return;

	// ----- OCR Reading -----
	EImageBW8 BW8SegmentImg, BW8Buff;
	BW8SegmentImg.SetImagePtr(m_ImageObjectForOcr.GetWidth(), m_ImageObjectForOcr.GetHeight(), m_ImageObjectForOcr.GetImageBuffer(), 0);

	BW8Buff.SetSize(BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());
	EasyImage::Oper(EArithmeticLogicOperation_Copy, &BW8SegmentImg, &BW8Buff);

	m_ChipData.clear();
	m_ChipData = CVisionSystem::Instance()->GetChipOCRData();
	std::vector<READ_DATA_Label> MasterData;
	MasterData.clear();

	CxGraphicObject& pGO = m_ImageViewForOcr.GetGraphicObject();
	pGO.Reset();
	if (!CVisionSystem::Instance()->OCRMeasureInspection(&pGO, &m_ImageObjectForOcr, m_ChipData, bSave))
	{
		AfxMessageBox(_T("Segment Reading Error!!"), MB_ICONERROR);

		WRITE_LOG(WL_ERROR, _T("No Font FIle in ReadOcr()"));
		return;
	}

	// ----- Read Data Setting -----
	m_OCRFontReadingData.clear();
	//m_OCRFontReadingData.strReadingResult = m_TrayData.strLineText;
	m_OCRFontReadingData.vstrReadingText = m_ChipData.strSegmentText;
	m_OCRFontReadingData.vreReadingArea = m_ChipData.reSegmentArea;
	for (int nIndex = 0; nIndex < (int)m_ChipData.reSegmentArea.size(); nIndex++)
	{
		m_OCRFontReadingData.vbTeaching.push_back(FALSE);
		m_OCRFontReadingData.vstrTeachingText.push_back(_T(""));
	}

	if (!bSave)
		m_ImageViewForOcr.ImageUpdate();
	//OCRSegmentBtnCheck();
	//ViewGraphicObjectUpdate();
}

void TeachOCRFontChip::OCRSegmentBtnCheck()
{
	USES_CONVERSION;

	CString strSegmentNum;
	GetDlgItem(IDC_FONTTEACH_EDIT_POSNUM)->GetWindowText(strSegmentNum);
	int nSegmentNum = _ttoi(strSegmentNum);

	BOOL bPrevBtnActivate = FALSE, bNextBtmActivate = FALSE;
	for (int nIndex = 0; nIndex < (int)m_SegmentFileData.size(); nIndex++)
	{
		int nLoadDataNum = _ttoi(m_SegmentFileData.at(nIndex).strSegmentLineNum);

		if (!bPrevBtnActivate)
			if (nSegmentNum > nLoadDataNum) bPrevBtnActivate = TRUE;

		if (!bNextBtmActivate)
			if (nSegmentNum < nLoadDataNum) bNextBtmActivate = TRUE;

		if (bPrevBtnActivate && bNextBtmActivate) break;
	}

	GetDlgItem(IDC_BUTTON_RETRYPREV)->EnableWindow(bPrevBtnActivate);
	GetDlgItem(IDC_BUTTON_RETRYNEXT)->EnableWindow(bNextBtmActivate);
}

void TeachOCRFontChip::ViewGraphicObjectUpdate()
{
	m_ImageViewForOcr.GetGraphicObject().Reset();

	COLORBOX	clrBox;
	COLORTEXT	clrText;
	COLORREF	clrColor;

	for (int nSegmentNum = 0; nSegmentNum < (int)m_OCRFontReadingData.vreReadingArea.size(); nSegmentNum++)
	{
		CRect reBuff = m_OCRFontReadingData.vreReadingArea.at(nSegmentNum);

		if (m_OCRFontReadingData.vbTeaching.at(nSegmentNum))
		{
			clrColor = PDC_YELLOW;

			clrText.SetText(m_OCRFontReadingData.vstrTeachingText.at(nSegmentNum));
			clrText.CreateObject(PDC_RED, reBuff.left, reBuff.top - 5, 20, TRUE, CxGOText::TextAlignment::TextAlignmentRight);
			m_ImageViewForOcr.GetGraphicObject().AddDrawText(clrText);
		}
		else clrColor = PDC_LIGHTGREEN;

		clrBox.CreateObject(clrColor, reBuff, PS_SOLID, 2);
		m_ImageViewForOcr.GetGraphicObject().AddDrawBox(clrBox);

		clrText.SetText(_T("%d"), nSegmentNum);
		clrText.CreateObject(clrColor, reBuff.CenterPoint().x, reBuff.CenterPoint().y, 20, TRUE, CxGOText::TextAlignment::TextAlignmentCenter);
		m_ImageViewForOcr.GetGraphicObject().AddDrawText(clrText);

		clrText.SetText(m_OCRFontReadingData.vstrReadingText.at(nSegmentNum));
		clrText.CreateObject(PDC_BLUE, reBuff.right, reBuff.top - 5, 20, TRUE, CxGOText::TextAlignment::TextAlignmentLeft);
		m_ImageViewForOcr.GetGraphicObject().AddDrawText(clrText);
	}

	clrText.SetText(m_OCRFontReadingData.strReadingResult);
	clrText.CreateObject(PDC_GREEN, 10, 20, 20, FALSE);
	m_ImageViewForOcr.GetGraphicObject().AddDrawText(clrText, 0);

	if (TRUE) // Font Size Area View
	{
		CString strBuff;
		CSize MinSize, MaxSize;

		m_FontSizeViewPosition.x = 100;
		m_FontSizeViewPosition.y = (int)((float)m_ImageObjectForOcr.GetHeight() / 2.f);

		CRect reMinArea, reMaxArea;
		reMinArea.SetRectEmpty();	reMinArea.OffsetRect(m_FontSizeViewPosition.x, m_FontSizeViewPosition.y);
		reMaxArea.SetRectEmpty();  	reMaxArea.OffsetRect(m_FontSizeViewPosition.x, m_FontSizeViewPosition.y);
		reMinArea.DeflateRect(-(int)((float)MinSize.cx / 2.f), -(int)((float)MinSize.cy / 2.f), -(int)((float)MinSize.cx / 2.f), -(int)((float)MinSize.cy / 2.f));
		reMaxArea.DeflateRect(-(int)((float)MaxSize.cx / 2.f), -(int)((float)MaxSize.cy / 2.f), -(int)((float)MaxSize.cx / 2.f), -(int)((float)MaxSize.cy / 2.f));

		clrBox.CreateObject(PDC_ORANGERED, reMinArea, PS_SOLID, 2);
		m_ImageViewForOcr.GetGraphicObject().AddDrawBox(clrBox);

		clrBox.CreateObject(PDC_ORANGE, reMaxArea, PS_SOLID, 2);
		m_ImageViewForOcr.GetGraphicObject().AddDrawBox(clrBox);
	}


	m_ImageViewForOcr.ImageUpdate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void TeachOCRFontChip::OnBnClickedFontteachBtnFontsave()
{
	USES_CONVERSION;

	if (AfxMessageBox(_T("Teaching Data를 저장하시겠습니까?"), MB_YESNO) != IDYES) return;

	// ----- Time Data -----
	CTime	cTimeData = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%02d%02d%02d%02d%02d%02d"), cTimeData.GetYear(), cTimeData.GetMonth(), cTimeData.GetDay(), cTimeData.GetHour(), cTimeData.GetMinute(), cTimeData.GetSecond());

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_OCRFontReadingData.vbTeaching.empty())
	{
		AfxMessageBox(_T("폰트 티칭 데이터가 없습니다."), MB_ICONERROR);
		return;
	}


	// ----- Teaching Data Check -----
	for (int nFontNum = 0; nFontNum < (int)m_OCRFontReadingData.vbTeaching.size(); nFontNum++)
	{
		if (m_OCRFontReadingData.vbTeaching.at(nFontNum)) break;
		else if (nFontNum == (int)m_OCRFontReadingData.vbTeaching.size() - 1)
		{
			AfxMessageBox(_T("폰트 티칭 데이터가 없습니다."), MB_ICONERROR);
			return;
		}
		else continue;
	}

	if (&m_ImageObjectForOcr == NULL || m_ImageObjectForOcr.GetImageBuffer() == NULL)
	{
		AfxMessageBox(_T("Image Error!!"), MB_ICONERROR);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ----- Font File Copy(Backup) -----
	CStringA strFontFileA;
	strFontFileA = CW2A(strFontPatch);

	EOCR OCRFont;
	OCRFont.Load(T2A(strFontPatch)); // strFontFileA.GetBuffer(strFontFileA.GetLength()));

	CString strFontCopyPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Font Data Backup");
	if (!IsExistFile((LPCTSTR)strFontCopyPatch)) MakeDirectory((LPCTSTR)strFontCopyPatch);
	strFontCopyPatch += _T("\\Backup_ChipOCRFOnt_") + strTime + _T(".OCR");
	OCRFont.Save(T2A(strFontCopyPatch));

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ----- OCR Font Add -----
	EImageBW8 BW8SegmentImg;
	BW8SegmentImg.SetImagePtr(m_ImageObjectForOcr.GetWidth(), m_ImageObjectForOcr.GetHeight(), m_ImageObjectForOcr.GetImageBuffer(), 0);

	CVisionSystem::Instance()->SetOCRResult(m_OCRFontReadingData);

	m_ChipData.clear();
	//m_TrayData.reLineArea = CRect(0, 0, BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EImageBW8 BW8Buff;
	BW8SegmentImg.SetImagePtr(m_ImageObjectForOcr.GetWidth(), m_ImageObjectForOcr.GetHeight(), m_ImageObjectForOcr.GetImageBuffer(), 0);

	BW8Buff.SetSize(BW8SegmentImg.GetWidth(), BW8SegmentImg.GetHeight());
	EasyImage::Oper(EArithmeticLogicOperation_Copy, &BW8SegmentImg, &BW8Buff);

	std::vector<READ_DATA_Label> MasterData; MasterData.clear();
	VisionProcess::SegmentReadingOption stReadingOption; stReadingOption.clear();
	//SetReadingOption(stReadingOption, TRUE);

	//if (!CVisionSystem::Instance()->OCRMeasureInspection(BW8Buff/*BW8SegmentImg*/, m_TrayData, MasterData, strFontPatch, stReadingOption))
	//{
	//	AfxMessageBox(_T("Add Font Error!!"), MB_ICONERROR);
	//	return;
	//}

	OCRSegmentReading(TRUE);
}

void TeachOCRFontChip::OnBnClickedFontteachBtnViewrefresh()
{
	OCRSegmentReading();
	m_ImageViewForOcr.ImageUpdate();
}

void TeachOCRFontChip::OnBnClickedFontteachBtnReregisterFont()
{
	TeachOCRReRegistration reRegistrationDlg;
	reRegistrationDlg.SetOcrMode(OCR_CHIP);

	if (reRegistrationDlg.DoModal() != IDOK) return;
}

void TeachOCRFontChip::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void TeachOCRFontChip::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CRect reClient, reGroupBoxArea;
	//GetClientRect(reClient);
	//GetDlgItem(IDC_FONTTEACH_GROUP_SEGMENTINFO)->GetWindowRect(reGroupBoxArea);
	//ScreenToClient(reGroupBoxArea);
	//if( !reGroupBoxArea.PtInRect( point ) ) return;
	CDialogEx::OnLButtonDown(nFlags, point);
}