#include "stdafx.h"
#include "ModelInfo.h"
#include "APK.h"
#include "StringMacro.h"
#include "InspTabObj.h"

#include <XUtil/xUtils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef MAX_PATH
#undef MAX_PATH
#define MAX_PATH	(4096)
#endif

CModelInfo::CModelInfo(void)
{
	m_TrayOcr.Clear();
	m_3DChipCnt.Clear();
	m_ChipOcr.Clear();
	m_Chip.Clear();
	m_Mixing.Clear();
	m_LiftInfo.Clear();
	m_StackerOcr.Clear();
	m_BandingInfo.Clear();
	m_HICInfo.Clear();
	m_DesiccantCuttingInfo.Clear();
	m_DesiccantMaterialInfo.Clear();
	m_DesiccantMaterialTrayInfo.Clear();
	m_MBBInfo.Clear();
	m_BoxInfo.Clear();
	m_SealingInfo.Clear();
	m_TapeInfo.Clear();
	m_LabelInfo.Clear();

	for(int j = 0; j < OP_MAX; j ++)
		m_LabelManualOptionInfo[j].Clear();
}


CModelInfo::~CModelInfo(void)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void CModelInfo::New(LPCTSTR lpszModelName, CString strKind)
{
	if (strKind == TRAYOCR_KIND)
	{
		m_strModelNameTrayOcr = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_TRAY_OCR"), lpszModelName);
		m_TrayOcr.Clear();
	}
	else if (strKind == TOPCHIPCNT_KIND)
	{
		m_strModelName3DChipCnt = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_3D_CHIP_CNT"), lpszModelName);
		m_3DChipCnt.Clear();
	}
	else if (strKind == CHIPOCR_KIND)
	{
		m_strModelNameChipOcr = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIPOCR"), lpszModelName);
		m_ChipOcr.Clear();
	}
	else if (strKind == CHIP_KIND)
	{
		m_strModelNameChip = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIP"), lpszModelName);
		m_Chip.Clear();
	}
	else if (strKind == MIXING_KIND)
	{
		m_strModelNameMixing = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_MIXING"), lpszModelName);
		m_Mixing.Clear();
	}
	else if (strKind == LIFT_KIND)
	{
		m_strModelNameLift = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_LIFT"), lpszModelName);
		m_LiftInfo.Clear();
	}
	else if (strKind == STACKEROCR_KIND)
	{
		m_strModelNameStackerOcr = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_STACKER_OCR"), lpszModelName);
		m_StackerOcr.Clear();
	}
	else if (strKind == BANDING_KIND)
	{
		m_strModelNameBanding = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_BANDING"), lpszModelName);
		m_BandingInfo.Clear();
	}
	else if (strKind == HIC_KIND)
	{
		m_strModelNameHIC = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_HIC"), lpszModelName);
		m_HICInfo.Clear();
	}
	else if (strKind == DESICCANT_KIND)
	{
		m_strModelNameDesiccantCutting = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_CUTTING"), lpszModelName);
		m_DesiccantCuttingInfo.Clear();
	}
	else if (strKind == SUBMATERIAL_KIND)
	{
		m_strModelNameDesiccantMaterial = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL"), lpszModelName);
		m_DesiccantMaterialInfo.Clear();
	}
	else if (strKind == SUBMATERIALTRAY_KIND)
	{
		m_strModelNameDesiccantMaterialTray = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL_TRAY"), lpszModelName);
		m_DesiccantMaterialTrayInfo.Clear();
	}
	else if (strKind == MBB_KIND)
	{
		m_strModelNameMBB = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_MBB"), lpszModelName);
		m_MBBInfo.Clear();
	}
	else if (strKind == BOXQUALITY_KIND)
	{
		m_strModelNameBoxQuality = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_BOX_QUALITY"), lpszModelName);
		m_BoxInfo.Clear();
	}
	else if (strKind == SEALINGQUALITY_KIND)
	{
		m_strModelNameSealingQuality = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_SEALING_QUALITY"), lpszModelName);
		m_SealingInfo.Clear();
	}
	else if (strKind == BOXTAPE_KIND)
	{
		m_strModelNameTape = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_TAPE"), lpszModelName);
		m_TapeInfo.Clear();
	}
	else if (strKind == LABEL_KIND)
	{
		m_strModelNameLabel = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_LABEL"), lpszModelName);
		m_LabelInfo.Clear();
	}
	else
	{
		m_strModelName = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME"), lpszModelName);
	}
}

void CModelInfo::LoadTrayOcr(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strBuff;
	CString strItem = _T("100");

	m_TrayOcr.Clear();

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("USE_TRAYOCR_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nUseBypass_TrayOcr = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.strDescription = szTemp;

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("TRAY_OCR_INFO"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.strTrayOcrInfo = szTemp;

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("OCR_AREA_DISTANCE_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nOcrAreaDistX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("OCR_AREA_DISTANCE_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nOcrAreaDistY = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_WIDTH_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nCharWidthMin = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_WIDTH_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nCharWidthMax = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_HEIGHT_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nCharHeightMin = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_HEIGHT_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nCharHeightMax = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("PAGE_COUNT"), _T("2048"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.lPageCount = (long)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.rcInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.rcInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.rcInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.rcInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("REMOVE_BORDER"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.bRemoveBorder = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("REMOVE_NARROW_OR_FLAT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.bRemoveNarrowFlat = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("CUT_LARGE_CHAR"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.bCutLargeChar = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("USE_ADAPTIVE_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.bUseAdaptiveThreshold = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("METHOD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nMethod = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("HALF_KERNEL_SIZE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nHalfKernelSize = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("THRESHOLD_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.nThresholdOffset = (int)_tcstod(szTemp, NULL);	

	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("ROTATE_IMAGE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TrayOcr.bRotateImage = (int)_tcstod(szTemp, NULL);

	for (int i = 0; i < nTRAY_MASKING_MAX; i++)
	{
		strBuff.Format(_T("MASKING_%d_"), i);

		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_TrayOcr.rcMaskingArea[i].left = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_TrayOcr.rcMaskingArea[i].top = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_TrayOcr.rcMaskingArea[i].right = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_TrayOcr.rcMaskingArea[i].bottom = (int)_tcstod(szTemp, NULL);

		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_TrayOcr.nMaskingAreaDistX[i] = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_TrayOcr.nMaskingAreaDistY[i] = (int)_tcstod(szTemp, NULL);
	}
}

void CModelInfo::Load3DChipCnt(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_3DChipCnt.Clear();

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("USE_3DCHIPCNT_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nUseBypass_3DChipCnt = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.strDescription = szTemp;

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("IMAGE_CUT_1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nImageCut = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("IMAGE_CUT_2"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nImageCut2 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("IMAGE_SHIFTX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nShiftX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("EQUALIZE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.bEqualize = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("MIN_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nMinThreshold = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("MAX_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nMaxThreshold = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.rcChipPosFirst.left = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.rcChipPosFirst.top = (int)_tcstod(szTemp, NULL);
	
	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.rcChipPosFirst.right = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.rcChipPosFirst.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("ROW_PITCH_PX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nRowPitchPx = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("ROW_PITCH"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.dRowPitch = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("COLUMN_PITCH_PX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nColumnPitchPx = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("COLUMN_PITCH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.dColumnPitch = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("X_COUNT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nXCount = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("Y_COUNT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nYCount = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(TOPCHIPCNT_APPNAME, _T("SELECT_UNIT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_3DChipCnt.nSelectUnit = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadChipOcr(LPCTSTR lpszModelInfoFile)
{
#ifdef RELEASE_1G
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strItem = _T("100");

	m_ChipOcr.Clear();

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("USE_CHIPOCR_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nUseBypass_ChipOcr = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.strDescription = szTemp;

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("CHIP_OCR_INFO"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.strChipOcrInfo = szTemp;

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nValueCh1 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("LIGHT_CH2"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nValueCh2 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("LIGHT_CH3"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nValueCh3 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("OCR_AREA_DISTANCE_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nOcrAreaDistanceX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("OCR_AREA_DISTANCE_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nOcrAreaDistanceY = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("BLACK_ON_WHITE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nFontColor = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_WIDTH_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nCharWidthMin = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_WIDTH_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nCharWidthMax = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_HEIGHT_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nCharHeightMin = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_HEIGHT_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nCharHeightMax = (int)_tcstod(szTemp, NULL);

	// Chip ¿µ¿ª
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcInspArea.bottom = (int)_tcstod(szTemp, NULL);

	// OCR ROI
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcOcrROI.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcOcrROI.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcOcrROI.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.rcOcrROI.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("REMOVE_BORDER"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.bRemoveBorder = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("REMOVE_NARROW_OR_FLAT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.bRemoveNarrowFlat = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("CUT_LARGE_CHAR"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.bCutLargeChar = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIPOCR_APPNAME, _T("THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_ChipOcr.nThreshold = (int)_tcstod(szTemp, NULL);
#endif
}

void CModelInfo::LoadChip( LPCTSTR lpszModelInfoFile )
{
#ifdef RELEASE_1G
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strItem;

	m_Chip.Clear();
	
	::GetPrivateProfileString( CHIP_APPNAME, _T("USE_CHIP_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.nUseBypass_Chip = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString(CHIP_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.strDescription = szTemp;

	::GetPrivateProfileString( CHIP_APPNAME, _T("LIGHT_CH1"), _T("1"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.nValueCh1 = (int)_tcstod( szTemp, NULL );
	::GetPrivateProfileString(CHIP_APPNAME, _T("LIGHT_CH2"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nValueCh2 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("LIGHT_CH3"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nValueCh3 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString( CHIP_APPNAME, _T("TRAY_MATRIX_X"), _T("1"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.nMatrix_X = (int)_tcstod( szTemp, NULL );
	::GetPrivateProfileString( CHIP_APPNAME, _T("TRAY_MATRIX_Y"), _T("1"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.nMatrix_Y = (int)_tcstod( szTemp, NULL );
	::GetPrivateProfileString( CHIP_APPNAME, _T("CHIP_FOV_CNT_X"), _T("1"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.nChipFovCnt_X = (int)_tcstod( szTemp, NULL );
	::GetPrivateProfileString( CHIP_APPNAME, _T("CHIP_FOV_CNT_Y"), _T("1"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.nChipFovCnt_Y = (int)_tcstod( szTemp, NULL );
	::GetPrivateProfileString( CHIP_APPNAME, _T("CHIP_UNIT_PITCH_X_MM"), _T("1.000f"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.fChipUnitPitch_X_MM = (float)_tcstod( szTemp, NULL );
	::GetPrivateProfileString( CHIP_APPNAME, _T("CHIP_UNIT_PITCH_Y_MM"), _T("1.000f"), szTemp, MAX_PATH, strModelInfoFile );
	m_Chip.fChipUnitPitch_Y_MM = (float)_tcstod( szTemp, NULL );
	::GetPrivateProfileString(CHIP_APPNAME, _T("CHIP_UNIT_PITCH_X_PX"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nChipUnitPitch_X_PX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("CHIP_UNIT_PITCH_Y_PX"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nChipUnitPitch_Y_PX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcInspArea.left = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_X[IDX_AREA4]);
	::GetPrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcInspArea.right = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcInspArea.top = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_Y[IDX_AREA4]);
	::GetPrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcMatchArea.left = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_X[IDX_AREA4]);
	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcMatchArea.right = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcMatchArea.top = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_Y[IDX_AREA4]);
	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.rcMatchArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_CENTER_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.ptMatch.x = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_CENTER_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.ptMatch.y = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("CHIP_AVG_GV"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nAvgGv = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("MATCH_RATIO"), _T("1.000f"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.fRatio = (float)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("SHIFT_X_PX"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nShiftX_PX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("SHIFT_Y_PX"), _T("1"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nShiftY_PX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("SHIFT_X_MM"), _T("1.000f"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.fShiftX_MM = (float)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(CHIP_APPNAME, _T("SHIFT_Y_MM"), _T("1.000f"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.fShiftY_MM = (float)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(CHIP_APPNAME, _T("SELECT_UNIT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Chip.nSelectUnit = (int)_tcstod(szTemp, NULL);
#endif
}

void CModelInfo::LoadMixing(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strBuff;

	m_Mixing.Clear();

	::GetPrivateProfileString(MIXING_APPNAME, _T("USE_MIXING_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.nUseBypass_Mixing = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MIXING_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.strDescription = szTemp;

	::GetPrivateProfileString(MIXING_APPNAME, _T("PAGE_COUNT"), _T("3000"), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.lPageCount = (long)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MIXING_APPNAME, _T("MATCHING_SCORE"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.fMatchingScore = (float)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MIXING_APPNAME, _T("TRAY_PIXEL_VALUE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.nTrayPixelValue = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MIXING_APPNAME, _T("PLAIN_PATTERN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.bIsPlainPattern = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MIXING_APPNAME, _T("MIXING_ZONE_WIDTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.nMixingZoneWidth = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MIXING_APPNAME, _T("TRAY_COUNT_ZONE_WIDTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_Mixing.nTrayCountZoneWidth = (int)_tcstod(szTemp, NULL);

	for (int j = 0; j < nMATCH_MAX; j++)
	{
		strBuff.Format(_T("MATCH_%d_"), j);

		::GetPrivateProfileString(MIXING_APPNAME, strBuff + _T("OFFSET_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_Mixing.ptOffset[j].x = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(MIXING_APPNAME, strBuff + _T("OFFSET_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_Mixing.ptOffset[j].y = (int)_tcstod(szTemp, NULL);
	}
}

void CModelInfo::LoadStackerOcr(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strBuff;
	CString strItem = _T("100");

	m_StackerOcr.Clear();

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.strDescription = szTemp;

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("STACKER_OCR_INFO"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.strOcrInfo = szTemp;

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("OCR_AREA_DISTANCE_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nOcrAreaDistX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("OCR_AREA_DISTANCE_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nOcrAreaDistY = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_WIDTH_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nCharWidthMin = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_WIDTH_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nCharWidthMax = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_HEIGHT_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nCharHeightMin = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_HEIGHT_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nCharHeightMax = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("PAGE_COUNT"), _T("5000"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.lPageCount = (long)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("ROTATE_IMAGE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.bRotateImage = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("REMOVE_BORDER"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.bRemoveBorder = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("REMOVE_NARROW_OR_FLAT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.bRemoveNarrowFlat = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("CUT_LARGE_CHAR"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.bCutLargeChar = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("USE_ADAPTIVE_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.bUseAdaptiveThreshold = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("METHOD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nMethod = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("HALF_KERNEL_SIZE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nHalfKernelSize = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("THRESHOLD_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nThresholdOffset = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("DEPTH_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_StackerOcr.nDepthOffset = (int)_tcstod(szTemp, NULL);

	for (int i = 0; i < nTRAY_MASKING_MAX; i++)
	{
		strBuff.Format(_T("MASKING_%d_"), i);

		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_StackerOcr.rcMaskingArea[i].left = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_StackerOcr.rcMaskingArea[i].top = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_StackerOcr.rcMaskingArea[i].right = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_StackerOcr.rcMaskingArea[i].bottom = (int)_tcstod(szTemp, NULL);

		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_StackerOcr.nMaskingAreaDistX[i] = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_StackerOcr.nMaskingAreaDistY[i] = (int)_tcstod(szTemp, NULL);
	}
}


void CModelInfo::LoadLiftInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_LiftInfo.Clear();

	::GetPrivateProfileString(LIFT_APPNAME, _T("USE_LIFT_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nUseBypass_Lift = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.strDescription = szTemp;

	::GetPrivateProfileString(LIFT_APPNAME, _T("PAGE_COUNT"), _T("4400"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.lPageCount = (long)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("TRAY_PIXEL_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nTrayPixelThreshold = (UINT8)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("LIFT_HEIGHT_MM"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.dbLiftHeight_MM = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("BREAK_WIDTH_MM"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.dbBreakWidth_MM = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("BREAK_HEIGHT_MM"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.dbBreakHeight_MM = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("LIFT_HEIGHT_PX"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nLiftHeight_PX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("BREAK_WIDTH_PX"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nBreakWidth_PX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("BREAK_HEIGHT_PX"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nBreakHeight_PX = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("L_OFFSET_FRONT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nLeftOffset_Front = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("L_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nLeftOffset = (int)_tcstod(szTemp, NULL);	
	
	::GetPrivateProfileString(LIFT_APPNAME, _T("R_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nRightOffset = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("T_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nTopOffset = (int)_tcstod(szTemp, NULL);	
	
	::GetPrivateProfileString(LIFT_APPNAME, _T("B_OFFSET"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nBottomOffset = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("WIDTH_FIRST"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nWidthFirst = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("WIDTH_SECOND"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nWidthSecond = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("WIDTH_THIRD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nWidthThird = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("WIDTH_FOURTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nWidthFourth = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_FIRST"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nRearWidthFirst = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_SECOND"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nRearWidthSecond = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_THIRD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nRearWidthThird = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_FOURTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nRearWidthFourth = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("SELECT_UNIT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nSelectUnit = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("FRONT_NON_INSP_START"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nFrontNonInspStart = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(LIFT_APPNAME, _T("FRONT_NON_INSP_WIDTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LiftInfo.nFrontNonInspWidth = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadBandingInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];	
	CString strItem;

	m_BandingInfo.Clear();

	::GetPrivateProfileString( BANDING_APPNAME, _T("USE_BANDING_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_BandingInfo.nUseBypass_Banding = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString(BANDING_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.strDescription = szTemp;

	::GetPrivateProfileString(BANDING_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);

	// Center
	::GetPrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.rcInspArea.left = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_X[IDX_AREA1]);
	::GetPrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.rcInspArea.right = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.rcInspArea.top = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_Y[IDX_AREA1]);
	::GetPrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.rcInspArea.bottom = (int)_tcstod(szTemp, NULL);
	
	//
	::GetPrivateProfileString(BANDING_APPNAME, _T("THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.nThreshold = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BANDING_APPNAME, _T("SPEC"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.nBandWidthSpec = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BANDING_APPNAME, _T("RANGE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BandingInfo.nBandWidthRange = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadHICInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_HICInfo.Clear();

	::GetPrivateProfileString(HIC_APPNAME, _T("USE_HIC_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nUseBypass_HIC = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(HIC_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.strDescription = szTemp;

	::GetPrivateProfileString(HIC_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);
	
	::GetPrivateProfileString(HIC_APPNAME, _T("COLOR_RED_LUMINANCE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nColorRedLuminance = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("COLOR_GREEN_LUMINANCE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nColorGreenLuminance = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("COLOR_BLUE_LUMINANCE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nColorBlueLuminance = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(HIC_APPNAME, _T("CIRCLE_OUTER_DIAMETER_60"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nCircleOuterDiameter60 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("CIRCLE_OUTER_DIAMETER_10"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nCircleOuterDiameter10 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("CIRCLE_OUTER_DIAMETER_5"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nCircleOuterDiameter5 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(HIC_APPNAME, _T("CIRCLE_INNER_DIAMETER_60"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nCircleInnerDiameter60 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("CIRCLE_INNER_DIAMETER_10"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nCircleInnerDiameter10 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("CIRCLE_INNER_DIAMETER_5"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nCircleInnerDiameter5 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(HIC_APPNAME, _T("DIS_COLOR"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nDisColor = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("AVG_VALUE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.nPixelAvgValue = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(HIC_APPNAME, _T("HIC_OFFSET_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.ptOffset_HIC.x = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(HIC_APPNAME, _T("HIC_OFFSET_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.ptOffset_HIC.y = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(HIC_APPNAME, _T("RATIO"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_HICInfo.fRatio = (float)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadDesiccantCuttingInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_DesiccantCuttingInfo.Clear();

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("USE_DESICCANT_POSTION_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("USE_DESICCANT_POSTION2_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString( DESICCANT_APPNAME, _T("USE_DESICCANT_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft = (int)_tcstod( szTemp, NULL );
	::GetPrivateProfileString( DESICCANT_APPNAME, _T("USE_DESICCANT2_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.strDescription = szTemp;

	// Desiccant Cutting Line
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_CUTTING_PIXEL_VALUE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nDesiccantCuttingPixelValue = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("SEALING_LENGTH"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.dSealingLength = _tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("COLOR_LINE_LENGTH_DIFFERENCE"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.dColorLineLengthDifference = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcLeftInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcLeftInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcLeftInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcLeftInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcRightInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcRightInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcRightInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcRightInspArea.bottom = (int)_tcstod(szTemp, NULL);
	
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH2"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nValueCh2 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH3"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nValueCh3 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH4"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nValueCh4 = (int)_tcstod(szTemp, NULL);

	// Desiccant Position
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_PIXEL_VALUE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nDesiccantPixelValue = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_WIDTH"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.dDesiccantWidth = _tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_HEIGHT"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.dDesiccantHeight = _tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_COLOR_LINE_LENGTH_DIFFERENCE"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.dPosColorLineLengthDiff = _tcstod(szTemp, NULL);

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("ROI_HEIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nROIHeight = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("ROI_WIDTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nROIWidth = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("COLOR_LINE_VALUE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nColorLineThreshold = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionLeftInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionLeftInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionLeftInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionLeftInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionRightInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionRightInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionRightInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.rcPositionRightInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(DESICCANT_APPNAME, _T("ROI_OFFSET_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nRoiOffsetLeft = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("ROI_OFFSET_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nRoiOffsetRight = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("OPEN_BOX_WIDTH"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nOpenBoxWidth = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(DESICCANT_APPNAME, _T("OPEN_BOX_HEIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantCuttingInfo.nOpenBoxHeight = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadDesiccantMaterialInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_DesiccantMaterialInfo.Clear();

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("USE_MATERIAL_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nUseBypass_Material = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.strDescription = szTemp;

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("LIGHT_CH2"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nValueCh2 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("INSPECTION_ZONE_COL"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nInspectionZoneCol = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("INSPECTION_ZONE_MARGIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nInspectionZoneMargin = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT_QUANTITY"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nDesiccantQuantity = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("HIC_SHIFT_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptHicShift.x = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("HIC_SHIFT_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptHicShift.y = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT1_SHIFT_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptDesiccant1Shift.x = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT1_SHIFT_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptDesiccant1Shift.y = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT2_SHIFT_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptDesiccant2Shift.x = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT2_SHIFT_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptDesiccant2Shift.y = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("PATTERN_SHIFT_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptPatternShiftTolerance.x = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("PATTERN_SHIFT_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.ptPatternShiftTolerance.y = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("MATCH_SCORE_HIC"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.fMatchScoreHIC = (float)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("MATCH_SCORE_DESICCANT"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.fMatchScoreDesiccant = (float)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("1ST_RATIO"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nRatio1 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("2ND_RATIO"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nRatio2 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIAL_APPNAME, _T("3RD_RATIO"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.nRatio3 = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadDesiccantMaterialTrayInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_DesiccantMaterialTrayInfo.Clear();

	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialInfo.strDescription = szTemp;

	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("TRAY_AMOUNT_MIN"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.nTrayQuantityMin = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("TRAY_AMOUNT_MAX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.nTrayQuantityMax = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMin.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMin.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMin.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMin.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMax.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMax.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_RIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMax.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_BOTTOM"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_DesiccantMaterialTrayInfo.rcTrayRoiMax.bottom = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadMBBInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strItem;

	m_MBBInfo.Clear();

	::GetPrivateProfileString( MBB_APPNAME, _T("USE_MBB_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_MBBInfo.nUseBypass_MBB = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString( MBB_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.strDescription = szTemp;
	::GetPrivateProfileString(MBB_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MBB_APPNAME, _T("INSP_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.rcInspArea.left = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_X[IDX_AREA1]);
	::GetPrivateProfileString(MBB_APPNAME, _T("INSP_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.rcInspArea.right = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MBB_APPNAME, _T("INSP_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.rcInspArea.top = (int)_tcstod(szTemp, NULL);

	strItem.Format(_T("%d"), nAREA_CAM_SIZE_Y[IDX_AREA1]);
	::GetPrivateProfileString(MBB_APPNAME, _T("INSP_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.rcInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MBB_APPNAME, _T("TOLERANCE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.fTolerance = (float)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(MBB_APPNAME, _T("LENGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.fLength = (float)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MBB_APPNAME, _T("WIDTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nWidth = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(MBB_APPNAME, _T("WIDTH_PX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nWidth_PX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(MBB_APPNAME, _T("HEIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nHeight = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(MBB_APPNAME, _T("HEIGHT_PX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nHeight_PX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(MBB_APPNAME, _T("RANGE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nRange = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(MBB_APPNAME, _T("RANGE_PX"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nRange_PX = (int)_tcstod(szTemp, NULL);

	CString strBuff = _T("");
	for (int i = 0; i < nMATCH_MAX; i++)
	{
		strBuff.Format(_T("MATCH_%d_"), i);

		::GetPrivateProfileString(MBB_APPNAME, strBuff + _T("CENTER_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_MBBInfo.ptOffset_MBB[i].x = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(MBB_APPNAME, strBuff + _T("CENTER_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_MBBInfo.ptOffset_MBB[i].y = (int)_tcstod(szTemp, NULL);
	}

	::GetPrivateProfileString(MBB_APPNAME, _T("RATIO"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.fRatio = (float)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(MBB_APPNAME, _T("SELECT_UNIT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_MBBInfo.nSelectUnit = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadBoxInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_BoxInfo.Clear();

	::GetPrivateProfileString( BOXQUALITY_APPNAME, _T("USE_BOX_QUALITY_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_BoxInfo.nUseBypass_Box = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString(BOXQUALITY_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_BoxInfo.strDescription = szTemp;
	::GetPrivateProfileString(BOXQUALITY_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BoxInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BOXQUALITY_APPNAME, _T("OK_SCORE"), _T("0.0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BoxInfo.fOkScore = (float)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(BOXQUALITY_APPNAME, _T("SHIFT_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BoxInfo.nShiftX = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BOXQUALITY_APPNAME, _T("SHIFT_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_BoxInfo.nShiftY = (int)_tcstod(szTemp, NULL);

	CString strBuff = _T("");
	for (int i = 0; i < nMATCH_MAX; i++)
	{
		strBuff.Format(_T("MATCH_%d_"), i);

		::GetPrivateProfileString(BOXQUALITY_APPNAME, strBuff + _T("CENTER_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_BoxInfo.ptMatchCenter[i].x = (int)_tcstod(szTemp, NULL);
		::GetPrivateProfileString(BOXQUALITY_APPNAME, strBuff + _T("CENTER_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		m_BoxInfo.ptMatchCenter[i].y = (int)_tcstod(szTemp, NULL);
	}
}

void CModelInfo::LoadSealingInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strItem;

	m_SealingInfo.Clear();

	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("USE_SEALING_QUALITY_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.nUseBypass_Sealing = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.strDescription = szTemp;
	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);
	
	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("COUNT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_SealingInfo.nCount = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SEALINGQUALITY_APPNAME, _T("INSP_SPEC"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_SealingInfo.nInspSpec = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_LEFT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.rcInspArea.left = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_RIGHT"), strItem, szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.rcInspArea.right = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_TOP"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.rcInspArea.top = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_BTM"), strItem, szTemp, MAX_PATH, strModelInfoFile );
	m_SealingInfo.rcInspArea.bottom = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(SEALINGQUALITY_APPNAME, _T("SELECT_UNIT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_SealingInfo.nSelectUnit = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadTapeInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_TapeInfo.Clear();

	::GetPrivateProfileString( BOXTAPE_APPNAME, _T("USE_BOX_TAPE_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_TapeInfo.nUseBypass_Tape = (int)_tcstod( szTemp, NULL );

	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.strDescription = szTemp;
	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("RED_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.nRedThreshold = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("GREEN_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.nGreenThreshold = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("BLUE_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.nBlueThreshold = (int)_tcstod(szTemp,NULL);

	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("TAPE_WIDTH"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.nTapeWidth = (int)_tcstod(szTemp, NULL);
	::GetPrivateProfileString(BOXTAPE_APPNAME, _T("TAPE_HEIGHT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_TapeInfo.nTapeHeight = (int)_tcstod(szTemp, NULL);
}

void CModelInfo::LoadLabelInfo( LPCTSTR lpszModelInfoFile, LPCTSTR lpszCustomerModelInfoFile )
{
	CString strModelInfoFile = lpszCustomerModelInfoFile;
	TCHAR szTemp[MAX_PATH];

	m_LabelInfo.Clear();

	CString strBuff;

	::GetPrivateProfileString(LABEL_APPNAME, _T("DESCRIPTION"), _T(""), szTemp, MAX_PATH, strModelInfoFile);
	m_LabelInfo.strDescription = szTemp;
	::GetPrivateProfileString(LABEL_APPNAME, _T("LIGHT_CH1"), _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	m_LabelInfo.nValueCh1 = (int)_tcstod(szTemp, NULL);

	::GetPrivateProfileString( LABEL_APPNAME, _T("EDGE_OFFSET"), _T("1.5"), szTemp, MAX_PATH, strModelInfoFile );
	m_LabelInfo.fLabelEdgeOffset = (float)_tcstod( szTemp, NULL );

	::GetPrivateProfileString( LABEL_APPNAME, _T("ANGLE_SPEC"), _T("5.0"), szTemp, MAX_PATH, strModelInfoFile );
	m_LabelInfo.fLabelAngleSpec = (float)_tcstod( szTemp, NULL );

	::GetPrivateProfileString( LABEL_APPNAME, _T("MASKING_COUNT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_LabelInfo.nLabelMaskingCount = (int)_tcstod( szTemp, NULL );

	for(int j = 0; j < EMATCH_TYPE_LABEL_MAX - 1; j ++)
	{
		strBuff.Format(_T("MASKING_%d_"), j);

		::GetPrivateProfileString( LABEL_APPNAME, strBuff + _T("OFFSET_X"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelInfo.ptOffset_Lable[j].x = (int)_tcstod( szTemp, NULL );
		::GetPrivateProfileString( LABEL_APPNAME, strBuff + _T("OFFSET_Y"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelInfo.ptOffset_Lable[j].y = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( LABEL_APPNAME, strBuff + _T("SCORE"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelInfo.nLabelMaskingScoreType[j] = (int)_tcstod( szTemp, NULL );
	}

	//Insp' Area
	int nIndex = 0;
	CRect rcArea;
	CString strKeyName = _T("");

	strKeyName.Format(_T("INSP_AREA"));
	::GetPrivateProfileString(LABEL_APPNAME, strKeyName, _T("0"), szTemp, MAX_PATH, strModelInfoFile);
	nIndex = (int)_tcstod(szTemp, NULL);

	m_LabelInfo.stInspectArea.Areas.clear();
	m_LabelInfo.stInspectArea.AlignedAreas.clear();

	for (int n = 0; n < nIndex; n++)
	{
		strKeyName.Format(_T("INSP_AREA_%02d_LEFT"), n);
		::GetPrivateProfileString(LABEL_APPNAME, strKeyName, _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		rcArea.left = (int)_tcstod(szTemp, NULL);

		strKeyName.Format(_T("INSP_AREA_%02d_RIGHT"), n);
		::GetPrivateProfileString(LABEL_APPNAME, strKeyName, _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		rcArea.right = (int)_tcstod(szTemp, NULL);

		strKeyName.Format(_T("INSP_AREA_%02d_TOP"), n);
		::GetPrivateProfileString(LABEL_APPNAME, strKeyName, _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		rcArea.top = (int)_tcstod(szTemp, NULL);

		strKeyName.Format(_T("INSP_AREA_%02d_BOTTOM"), n);
		::GetPrivateProfileString(LABEL_APPNAME, strKeyName, _T("0"), szTemp, MAX_PATH, strModelInfoFile);
		rcArea.bottom = (int)_tcstod(szTemp, NULL);

		m_LabelInfo.stInspectArea.Areas.push_back(rcArea);
		m_LabelInfo.stInspectArea.AlignedAreas.push_back(rcArea);
	}

	//////////////////////////////////////////////////////////////////////////
	strModelInfoFile = lpszModelInfoFile;

	::GetPrivateProfileString( LABEL_APPNAME, _T("USE_LABEL_BYPASS"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
	m_LabelInfo.nUseBypass_Label = (int)_tcstod( szTemp, NULL );
}

void CModelInfo::LoadLabelManualOptionInfo( LPCTSTR lpszCustomerModelInfoFile )
{
	CString strModelInfoFile = lpszCustomerModelInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strItem;

	for(int j = 0; j < OP_MAX; j ++)
		m_LabelManualOptionInfo[j].Clear();

	CString strTemp;

	CString strLabelName;
	strLabelName.Format(_T("LABEL"));

	for(int j = 0; j < OP_MAX; j++)
	{
		CString strBuff;
		strBuff.Format(_T("OPTION%02d_"), j);

		::GetPrivateProfileString( strLabelName, strBuff + _T("SEGMENT_LINE_NUM"), _T("-1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].nSegmentLine_Num = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("IMAGE_TYPE"), _T("-1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].nImageTypeIndex = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("USE_MAX_THRESHOLD"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].bUseMaxThreshold = (BOOL)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("USE_IMAGE_FILTERING"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].bUseImageFiltering = (BOOL)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("USE_eVISION_LARGECHARSCUT"), _T("0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].bUseeVisionLargeCharsCut = (BOOL)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("FONT_MIN_WIDTH"), _T("-1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].FontMinSize.cx = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("FONT_MIN_HEIGHT"), _T("-1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].FontMinSize.cy = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("FONT_MAX_WIDTH"), _T("-1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].FontMaxSize.cx = (int)_tcstod( szTemp, NULL );

		::GetPrivateProfileString( strLabelName, strBuff + _T("FONT_MAX_HEIGHT"), _T("-1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelManualOptionInfo[j].FontMaxSize.cy = (int)_tcstod( szTemp, NULL );
	}
}

void CModelInfo::LoadSimilarFontInfo()
{
	CString strModelInfoFile;
	strModelInfoFile = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\SimilarFont.ini");

	TCHAR szTemp[MAX_PATH];

	m_LabelInfo.SimilarFontClear();

	CString strFontNum;
	for( int nIndex = 0; nIndex < 50; nIndex++)
	{
		strFontNum.Format(_T("FONT_%02d"), nIndex);

		::GetPrivateProfileString( strFontNum, _T("TARGET_0"), _T("None_Target_0"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelInfo.vstrSimilarFont[0].push_back( szTemp );

		::GetPrivateProfileString( strFontNum, _T("TARGET_1"), _T("None_Target_1"), szTemp, MAX_PATH, strModelInfoFile );
		m_LabelInfo.vstrSimilarFont[1].push_back( szTemp );
	}
}

BOOL CModelInfo::Load(LPCTSTR lpszModelName, CString strKind)
{
	CString strRecipePath;
	strRecipePath = GetRecipeRootPath() + strKind + _T("\\") + lpszModelName + _T("\\");

	CString strModelInfoFile = strRecipePath + _T("ModelInfo.ini");
	if(!IsExistFile((LPCTSTR)strModelInfoFile)) return FALSE;

#ifdef RELEASE_1G
	if (strKind == TRAYOCR_KIND)
	{
		CString strTrayOcrFile = GetRecipeRootPath() + _T("1G_TrayOcr\\") + lpszModelName + _T("\\1G_TrayOcr.ini");
		if (IsExistFile((LPCTSTR)strTrayOcrFile))	LoadTrayOcr(strTrayOcrFile);
	}
	else if (strKind == TOPCHIPCNT_KIND)
	{
		CString str3DChipCntFile = GetRecipeRootPath() + _T("1G_3DChipCnt\\") + lpszModelName + _T("\\1G_3DChipCnt.ini");
		if (IsExistFile((LPCTSTR)str3DChipCntFile))	Load3DChipCnt(str3DChipCntFile);
	}
	else if (strKind == CHIPOCR_KIND)
	{
		CString strChipOcrFile = GetRecipeRootPath() + _T("1G_ChipOcr\\") + lpszModelName + _T("\\1G_ChipOcr.ini");
		if (IsExistFile((LPCTSTR)strChipOcrFile))	LoadChipOcr(strChipOcrFile);
	}
	else if (strKind == CHIP_KIND)
	{
		CString strChipFile = GetRecipeRootPath() + _T("1G_Chip\\") + lpszModelName + _T("\\1G_Chip.ini");
		if (IsExistFile((LPCTSTR)strChipFile))	LoadChip(strChipFile);

		// ----- Object Map Update -----
//		CInspTabObj ModeTabObj;
//		ModeTabObj.SetObjectTabMatrix();
	}
	else if (strKind == MIXING_KIND)
	{
		CString strMixingFile = GetRecipeRootPath() + _T("1G_Mixing\\") + lpszModelName + _T("\\1G_Mixing.ini");
		if (IsExistFile((LPCTSTR)strMixingFile))	LoadMixing(strMixingFile);
	}
	else if (strKind == LIFT_KIND)
	{
		CString strLiftInfoFile = GetRecipeRootPath() + _T("1G_Lift\\") + lpszModelName + _T("\\1G_Lift.ini");
		if (IsExistFile((LPCTSTR)strLiftInfoFile))	LoadLiftInfo(strLiftInfoFile);
	}
	
#elif RELEASE_SG
	else if (strKind == STACKEROCR_KIND)
	{
		CString strStackerOcrFile = GetRecipeRootPath() + _T("SG_StackerOcr\\") + lpszModelName + _T("\\SG_StackerOcr.ini");
		if (IsExistFile((LPCTSTR)strStackerOcrFile))	LoadStackerOcr(strStackerOcrFile);
	}

#elif RELEASE_2G
	if (strKind == BANDING_KIND)
	{
		CString strBandingInfoFile = GetRecipeRootPath() + _T("2G_Banding\\") + lpszModelName + _T("\\2G_Banding.ini");
		if (IsExistFile((LPCTSTR)strBandingInfoFile))	LoadBandingInfo(strBandingInfoFile);
	}
	else if (strKind == HIC_KIND)
	{
		CString strHICInfoFile = GetRecipeRootPath() + _T("2G_HIC\\") + lpszModelName + _T("\\2G_HIC.ini");
		if (IsExistFile((LPCTSTR)strHICInfoFile))	LoadHICInfo(strHICInfoFile);
	}

#elif RELEASE_3G
	if (strKind == DESICCANT_KIND)
	{
		CString strCuttingInfoFile = GetRecipeRootPath() + _T("3G_Desiccant_Cutting\\") + lpszModelName + _T("\\3G_Desiccant_Cutting.ini");
		if (IsExistFile((LPCTSTR)strCuttingInfoFile))	LoadDesiccantCuttingInfo(strCuttingInfoFile);
}
	else if (strKind == SUBMATERIAL_KIND)
	{
		CString strMaterialInfoFile = GetRecipeRootPath() + _T("3G_Desiccant_Material\\") + lpszModelName + _T("\\3G_Desiccant_Material.ini");
		if (IsExistFile((LPCTSTR)strMaterialInfoFile))	LoadDesiccantMaterialInfo(strMaterialInfoFile);
	}
	else if (strKind == SUBMATERIALTRAY_KIND)
	{
		CString strMaterialTrayInfoFile = GetRecipeRootPath() + _T("3G_Desiccant_Material_Tray\\") + lpszModelName + _T("\\3G_Desiccant_Material_Tray.ini");
		if (IsExistFile((LPCTSTR)strMaterialTrayInfoFile))	LoadDesiccantMaterialTrayInfo(strMaterialTrayInfoFile);
	}

#elif RELEASE_4G
	if (strKind == MBB_KIND)
	{
		CString strMBBInfoFile = GetRecipeRootPath() + _T("4G_MBB\\") + lpszModelName + _T("\\4G_MBB.ini");
		if (IsExistFile((LPCTSTR)strMBBInfoFile))		LoadMBBInfo(strMBBInfoFile);
	}
	else if (strKind == LABEL_KIND)
	{
		CString strLabelFile = GetRecipeRootPath() + _T("Label\\") + lpszModelName + _T("\\LabelInfo.ini");
		CString strLabelManualInfoFile_Customer = GetRecipeRootPath() + _T("Label\\") + lpszModelName + _T("\\LabelManualOption.ini");

		if( IsExistFile( (LPCTSTR)strLabelFile ))		LoadLabelInfo( strLabelFile, strLabelManualInfoFile_Customer );
		if( IsExistFile( (LPCTSTR)strLabelFile ))		LoadLabelManualOptionInfo( strLabelManualInfoFile_Customer );
	}

#elif RELEASE_5G
	if( strKind == BOXQUALITY_KIND )
	{
		CString strBoxInfoFile = GetRecipeRootPath() + _T("5G_Box\\") + lpszModelName + _T("\\5G_Box.ini");

		if( IsExistFile( (LPCTSTR)strBoxInfoFile) )			LoadBoxInfo( strBoxInfoFile );
	}
	else if( strKind == SEALINGQUALITY_KIND )
	{
		CString strSealingInfoFile = GetRecipeRootPath() + _T("5G_Sealing\\") + lpszModelName + _T("\\5G_Sealing.ini");

		if( IsExistFile( (LPCTSTR)strSealingInfoFile) )			LoadSealingInfo( strSealingInfoFile );
	}

#elif RELEASE_6G
	if (strKind == LABEL_KIND)
	{
		CString strLabelFile = GetRecipeRootPath() + _T("Label\\") + lpszModelName + _T("\\LabelInfo.ini");
		CString strLabelManualInfoFile_Customer = GetRecipeRootPath() + _T("Label\\") + lpszModelName + _T("\\LabelManualOption.ini");

		if (IsExistFile((LPCTSTR)strLabelFile))			LoadLabelInfo(strLabelFile, strLabelManualInfoFile_Customer);
		if (IsExistFile((LPCTSTR)strLabelFile))			LoadLabelManualOptionInfo(strLabelManualInfoFile_Customer);
	}
	else if (strKind == BOXTAPE_KIND)
	{
		CString strTapeInfoFile = GetRecipeRootPath() + _T("6G_Tape\\") + lpszModelName + _T("\\6G_Tape.ini");
		if (IsExistFile((LPCTSTR)strTapeInfoFile))		LoadTapeInfo(strTapeInfoFile);
	}
	
#endif

	if (strKind == TRAYOCR_KIND)
	{
		m_strModelNameTrayOcr = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_TRAY_OCR"), lpszModelName);
	}
	else if (strKind == TOPCHIPCNT_KIND)
	{
		m_strModelName3DChipCnt = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_3D_CHIP_CNT"), lpszModelName);
	}
	else if (strKind == CHIPOCR_KIND)
	{
		m_strModelNameChipOcr = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIPOCR"), lpszModelName);
	}
	else if (strKind == CHIP_KIND)
	{
		m_strModelNameChip = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIP"), lpszModelName);
	}
	else if (strKind == MIXING_KIND)
	{
		m_strModelNameMixing = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_MIXING"), lpszModelName);
	}
	else if (strKind == LIFT_KIND)
	{
		m_strModelNameLift = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_LIFT"), lpszModelName);
	}
	else if (strKind == STACKEROCR_KIND)
	{
		m_strModelNameStackerOcr = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_STACKER_OCR"), lpszModelName);
	}
	else if (strKind == BANDING_KIND)
	{
		m_strModelNameBanding = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_BANDING"), lpszModelName);
	}
	else if (strKind == HIC_KIND)
	{
		m_strModelNameHIC = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_HIC"), lpszModelName);
	}
	else if (strKind == DESICCANT_KIND)
	{
		m_strModelNameDesiccantCutting = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_CUTTING"), lpszModelName);
	}
	else if (strKind == SUBMATERIAL_KIND)
	{
		m_strModelNameDesiccantMaterial = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL"), lpszModelName);
	}
	else if (strKind == SUBMATERIALTRAY_KIND)
	{
		m_strModelNameDesiccantMaterialTray = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL_TRAY"), lpszModelName);
	}
	else if (strKind == MBB_KIND)
	{
		m_strModelNameMBB = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_MBB"), lpszModelName);
	}
	else if (strKind == BOXQUALITY_KIND)
	{
		m_strModelNameBoxQuality = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_BOX_QUALITY"), lpszModelName);
	}
	else if (strKind == SEALINGQUALITY_KIND)
	{
		m_strModelNameSealingQuality = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_SEALING_QUALITY"), lpszModelName);
	}
	else if (strKind == BOXTAPE_KIND)
	{
		m_strModelNameTape = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_TAPE"), lpszModelName);
	}
	else if (strKind == LABEL_KIND)
	{
		m_strModelNameLabel = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_LABEL"), lpszModelName);
	}
	else
	{
		m_strModelName = lpszModelName;
		AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME"), lpszModelName);
	}

	return TRUE;
}

void CModelInfo::SaveModelInfo( LPCTSTR lpszModelInfoFile )
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	CTime tmNow = CTime::GetCurrentTime();
	strTemp.Format( _T("%4d/%02d/%02d-%02d/%02d/%02d"), tmNow.GetYear(),tmNow.GetMonth(), tmNow.GetDay(), tmNow.GetHour(),tmNow.GetMinute(), tmNow.GetSecond() );

	::WritePrivateProfileString( _T("MODEL_INFO"), _T("SAVE_TIME"), strTemp, strModelInfoFile );
}

void CModelInfo::SaveTrayOcr(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp, strBuff;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.nUseBypass_TrayOcr);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("USE_TRAYOCR_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_TrayOcr.strDescription);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_TrayOcr.strTrayOcrInfo);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("TRAY_OCR_INFO"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.nOcrAreaDistX);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("OCR_AREA_DISTANCE_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.nOcrAreaDistY);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("OCR_AREA_DISTANCE_Y"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.nCharWidthMin);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_WIDTH_MIN"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.nCharWidthMax);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_WIDTH_MAX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.nCharHeightMin);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_HEIGHT_MIN"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.nCharHeightMax);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("CHAR_HEIGHT_MAX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.lPageCount);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("PAGE_COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.rcInspArea.left);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.rcInspArea.right);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.rcInspArea.top);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.rcInspArea.bottom);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("INSP_AREA_BTM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.bRemoveBorder);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("REMOVE_BORDER"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.bRemoveNarrowFlat);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("REMOVE_NARROW_OR_FLAT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.bCutLargeChar);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("CUT_LARGE_CHAR"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.bUseAdaptiveThreshold);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("USE_ADAPTIVE_THRESHOLD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.nMethod);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("METHOD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.nHalfKernelSize);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("HALF_KERNEL_SIZE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TrayOcr.nThresholdOffset);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("THRESHOLD_OFFSET"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TrayOcr.bRotateImage);
	::WritePrivateProfileString(TRAYOCR_APPNAME, _T("ROTATE_IMAGE"), strTemp, strModelInfoFile);

	for (int i = 0; i < nTRAY_MASKING_MAX; i++)
	{
		strBuff.Format(_T("MASKING_%d_"), i);

		strTemp.Format(_T("%d"), m_TrayOcr.rcMaskingArea[i].left);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_LEFT"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_TrayOcr.rcMaskingArea[i].top);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_TOP"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_TrayOcr.rcMaskingArea[i].right);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_RIGHT"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_TrayOcr.rcMaskingArea[i].bottom);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_BOTTOM"), strTemp, strModelInfoFile);

		strTemp.Format(_T("%d"), m_TrayOcr.nMaskingAreaDistX[i]);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_X"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_TrayOcr.nMaskingAreaDistY[i]);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_Y"), strTemp, strModelInfoFile);
	}
}

void CModelInfo::Save3DChipCnt(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nUseBypass_3DChipCnt);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("USE_3DCHIPCNT_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_3DChipCnt.strDescription);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nImageCut);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("IMAGE_CUT_1"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nImageCut2);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("IMAGE_CUT_2"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nShiftX);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("IMAGE_SHIFTX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.bEqualize);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("EQUALIZE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nMinThreshold);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("MIN_THRESHOLD"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nMaxThreshold);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("MAX_THRESHOLD"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.rcChipPosFirst.left);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_LEFT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.rcChipPosFirst.top);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_TOP"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.rcChipPosFirst.right);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_RIGHT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.rcChipPosFirst.bottom);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("CHIP_POS_FIRST_BOTTOM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.3lf"), m_3DChipCnt.dRowPitch);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("ROW_PITCH"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nRowPitchPx);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("ROW_PITCH_PX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.3lf"), m_3DChipCnt.dColumnPitch);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("COLUMN_PITCH"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nColumnPitchPx);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("COLUMN_PITCH_PX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nXCount);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("X_COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nYCount);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("Y_COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_3DChipCnt.nSelectUnit);
	::WritePrivateProfileString(TOPCHIPCNT_APPNAME, _T("SELECT_UNIT"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveChipOcr(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_ChipOcr.nUseBypass_ChipOcr);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("USE_CHIPOCR_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_ChipOcr.strDescription);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%s"), m_ChipOcr.strChipOcrInfo);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("CHIP_OCR_INFO"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_ChipOcr.nValueCh1);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nValueCh2);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("LIGHT_CH2"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nValueCh3);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("LIGHT_CH3"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_ChipOcr.nOcrAreaDistanceX);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("OCR_AREA_DISTANCE_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nOcrAreaDistanceY);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("OCR_AREA_DISTANCE_Y"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_ChipOcr.nFontColor);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("BLACK_ON_WHITE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nCharWidthMin);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_WIDTH_MIN"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nCharWidthMax);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_WIDTH_MAX"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nCharHeightMin);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_HEIGHT_MIN"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.nCharHeightMax);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("CHAR_HEIGHT_MAX"), strTemp, strModelInfoFile);

	// Tray Outline Model
	strTemp.Format(_T("%d"), m_ChipOcr.rcInspArea.left);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.rcInspArea.right);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.rcInspArea.top);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.rcInspArea.bottom);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("INSP_AREA_BTM"), strTemp, strModelInfoFile);

	// OCR ROI
	strTemp.Format(_T("%d"), m_ChipOcr.rcOcrROI.left);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.rcOcrROI.right);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.rcOcrROI.top);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.rcOcrROI.bottom);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("OCR_ROI_BTM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_ChipOcr.bRemoveBorder);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("REMOVE_BORDER"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.bRemoveNarrowFlat);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("REMOVE_NARROW_OR_FLAT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_ChipOcr.bCutLargeChar);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("CUT_LARGE_CHAR"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_ChipOcr.nThreshold);
	::WritePrivateProfileString(CHIPOCR_APPNAME, _T("THRESHOLD"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveChip(LPCTSTR lpszModelInfoFile)
{
#ifdef RELEASE_1G
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format( _T("%d"), m_Chip.nUseBypass_Chip );
	::WritePrivateProfileString( CHIP_APPNAME, _T("USE_CHIP_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format(_T("%s"), m_Chip.strDescription);
	::WritePrivateProfileString(CHIP_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format( _T("%d"), m_Chip.nValueCh1 );
	::WritePrivateProfileString( CHIP_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile );
	strTemp.Format(_T("%d"), m_Chip.nValueCh2);
	::WritePrivateProfileString(CHIP_APPNAME, _T("LIGHT_CH2"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.nValueCh3);
	::WritePrivateProfileString(CHIP_APPNAME, _T("LIGHT_CH3"), strTemp, strModelInfoFile);
	
	strTemp.Format( _T("%d"), m_Chip.nMatrix_X );
	::WritePrivateProfileString( CHIP_APPNAME, _T("TRAY_MATRIX_X"), strTemp, strModelInfoFile );
	strTemp.Format( _T("%d"), m_Chip.nMatrix_Y );
	::WritePrivateProfileString( CHIP_APPNAME, _T("TRAY_MATRIX_Y"), strTemp, strModelInfoFile );
	strTemp.Format( _T("%d"), m_Chip.nChipFovCnt_X );
	::WritePrivateProfileString( CHIP_APPNAME, _T("CHIP_FOV_CNT_X"), strTemp, strModelInfoFile );
	strTemp.Format( _T("%d"), m_Chip.nChipFovCnt_Y );
	::WritePrivateProfileString( CHIP_APPNAME, _T("CHIP_FOV_CNT_Y"), strTemp, strModelInfoFile );
	strTemp.Format( _T("%.3f"), m_Chip.fChipUnitPitch_X_MM);
	::WritePrivateProfileString( CHIP_APPNAME, _T("CHIP_UNIT_PITCH_X_MM"), strTemp, strModelInfoFile );
	strTemp.Format( _T("%.3f"), m_Chip.fChipUnitPitch_Y_MM);
	::WritePrivateProfileString( CHIP_APPNAME, _T("CHIP_UNIT_PITCH_Y_MM"), strTemp, strModelInfoFile );
	strTemp.Format(_T("%d"), m_Chip.nChipUnitPitch_X_PX);
	::WritePrivateProfileString(CHIP_APPNAME, _T("CHIP_UNIT_PITCH_X_PX"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.nChipUnitPitch_Y_PX);
	::WritePrivateProfileString(CHIP_APPNAME, _T("CHIP_UNIT_PITCH_Y_PX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Chip.rcInspArea.left);
	::WritePrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.rcInspArea.right);
	::WritePrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.rcInspArea.top);
	::WritePrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.rcInspArea.bottom);
	::WritePrivateProfileString(CHIP_APPNAME, _T("INSP_AREA_BTM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Chip.rcMatchArea.left);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.rcMatchArea.right);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.rcMatchArea.top);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.rcMatchArea.bottom);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_AREA_BTM"), strTemp, strModelInfoFile);
	
	strTemp.Format(_T("%d"), m_Chip.ptMatch.x);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_CENTER_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.ptMatch.y);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_CENTER_Y"), strTemp, strModelInfoFile);
		
	strTemp.Format(_T("%d"), m_Chip.nAvgGv);
	::WritePrivateProfileString(CHIP_APPNAME, _T("CHIP_AVG_GV"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.2f"), m_Chip.fRatio);
	::WritePrivateProfileString(CHIP_APPNAME, _T("MATCH_RATIO"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.nShiftX_PX);
	::WritePrivateProfileString(CHIP_APPNAME, _T("SHIFT_X_PX"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_Chip.nShiftY_PX);
	::WritePrivateProfileString(CHIP_APPNAME, _T("SHIFT_Y_PX"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.4f"), m_Chip.fShiftX_MM);
	::WritePrivateProfileString(CHIP_APPNAME, _T("SHIFT_X_MM"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.4f"), m_Chip.fShiftY_MM);
	::WritePrivateProfileString(CHIP_APPNAME, _T("SHIFT_Y_MM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Chip.nSelectUnit);
	::WritePrivateProfileString(CHIP_APPNAME, _T("SELECT_UNIT"), strTemp, strModelInfoFile);
#endif
}

void CModelInfo::SaveMixing(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp, strBuff;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_Mixing.nUseBypass_Mixing);
	::WritePrivateProfileString(MIXING_APPNAME, _T("USE_MIXING_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_Mixing.strDescription);
	::WritePrivateProfileString(MIXING_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Mixing.lPageCount);
	::WritePrivateProfileString(MIXING_APPNAME, _T("PAGE_COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Mixing.nTrayPixelValue);
	::WritePrivateProfileString(MIXING_APPNAME, _T("TRAY_PIXEL_VALUE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.2f"), m_Mixing.fMatchingScore);
	::WritePrivateProfileString(MIXING_APPNAME, _T("MATCHING_SCORE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Mixing.bIsPlainPattern);
	::WritePrivateProfileString(MIXING_APPNAME, _T("PLAIN_PATTERN"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Mixing.nMixingZoneWidth);
	::WritePrivateProfileString(MIXING_APPNAME, _T("MIXING_ZONE_WIDTH"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_Mixing.nTrayCountZoneWidth);
	::WritePrivateProfileString(MIXING_APPNAME, _T("TRAY_COUNT_ZONE_WIDTH"), strTemp, strModelInfoFile);

	for (int j = 0; j < nMATCH_MAX; j++)
	{
		strBuff.Format(_T("MATCH_%d_"), j);

		strTemp.Format(_T("%d"), m_Mixing.ptOffset[j].x);
		::WritePrivateProfileString(MIXING_APPNAME, strBuff + _T("OFFSET_X"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_Mixing.ptOffset[j].y);
		::WritePrivateProfileString(MIXING_APPNAME, strBuff + _T("OFFSET_Y"), strTemp, strModelInfoFile);
	}
}

void CModelInfo::SaveLiftInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nUseBypass_Lift);
	::WritePrivateProfileString(LIFT_APPNAME, _T("USE_LIFT_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_LiftInfo.strDescription);
	::WritePrivateProfileString(LIFT_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.lPageCount);
	::WritePrivateProfileString(LIFT_APPNAME, _T("PAGE_COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%u"), m_LiftInfo.nTrayPixelThreshold);
	::WritePrivateProfileString(LIFT_APPNAME, _T("TRAY_PIXEL_THRESHOLD"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.4lf"), m_LiftInfo.dbLiftHeight_MM);
	::WritePrivateProfileString(LIFT_APPNAME, _T("LIFT_HEIGHT_MM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.4lf"), m_LiftInfo.dbBreakWidth_MM);
	::WritePrivateProfileString(LIFT_APPNAME, _T("BREAK_WIDTH_MM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.4lf"), m_LiftInfo.dbBreakHeight_MM);
	::WritePrivateProfileString(LIFT_APPNAME, _T("BREAK_HEIGHT_MM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nLiftHeight_PX);
	::WritePrivateProfileString(LIFT_APPNAME, _T("LIFT_HEIGHT_PX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nBreakWidth_PX);
	::WritePrivateProfileString(LIFT_APPNAME, _T("BREAK_WIDTH_PX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nBreakHeight_PX);
	::WritePrivateProfileString(LIFT_APPNAME, _T("BREAK_HEIGHT_PX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nLeftOffset_Front);
	::WritePrivateProfileString(LIFT_APPNAME, _T("L_OFFSET_FRONT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nLeftOffset);
	::WritePrivateProfileString(LIFT_APPNAME, _T("L_OFFSET"), strTemp, strModelInfoFile);
	
	strTemp.Format(_T("%d"), m_LiftInfo.nRightOffset);
	::WritePrivateProfileString(LIFT_APPNAME, _T("R_OFFSET"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nTopOffset);
	::WritePrivateProfileString(LIFT_APPNAME, _T("T_OFFSET"), strTemp, strModelInfoFile);
	
	strTemp.Format(_T("%d"), m_LiftInfo.nBottomOffset);
	::WritePrivateProfileString(LIFT_APPNAME, _T("B_OFFSET"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nWidthFirst);
	::WritePrivateProfileString(LIFT_APPNAME, _T("WIDTH_FIRST"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nWidthSecond);
	::WritePrivateProfileString(LIFT_APPNAME, _T("WIDTH_SECOND"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nWidthThird);
	::WritePrivateProfileString(LIFT_APPNAME, _T("WIDTH_THIRD"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nWidthFourth);
	::WritePrivateProfileString(LIFT_APPNAME, _T("WIDTH_FOURTH"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nRearWidthFirst);
	::WritePrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_FIRST"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nRearWidthSecond);
	::WritePrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_SECOND"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nRearWidthThird);
	::WritePrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_THIRD"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nRearWidthFourth);
	::WritePrivateProfileString(LIFT_APPNAME, _T("REAR_WIDTH_FOURTH"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nSelectUnit);
	::WritePrivateProfileString(LIFT_APPNAME, _T("SELECT_UNIT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nFrontNonInspStart);
	::WritePrivateProfileString(LIFT_APPNAME, _T("FRONT_NON_INSP_START"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_LiftInfo.nFrontNonInspWidth);
	::WritePrivateProfileString(LIFT_APPNAME, _T("FRONT_NON_INSP_WIDTH"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveStackerOcr(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp, strBuff;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%s"), m_StackerOcr.strDescription);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_StackerOcr.strOcrInfo);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("STACKER_OCR_INFO"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nOcrAreaDistX);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("OCR_AREA_DISTANCE_X"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nOcrAreaDistY);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("OCR_AREA_DISTANCE_Y"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nCharWidthMin);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_WIDTH_MIN"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nCharWidthMax);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_WIDTH_MAX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nCharHeightMin);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_HEIGHT_MIN"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nCharHeightMax);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("CHAR_HEIGHT_MAX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.lPageCount);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("PAGE_COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.bRotateImage);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("ROTATE_IMAGE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_StackerOcr.bRemoveBorder);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("REMOVE_BORDER"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_StackerOcr.bRemoveNarrowFlat);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("REMOVE_NARROW_OR_FLAT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_StackerOcr.bCutLargeChar);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("CUT_LARGE_CHAR"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.bUseAdaptiveThreshold);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("USE_ADAPTIVE_THRESHOLD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_StackerOcr.nMethod);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("METHOD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_StackerOcr.nHalfKernelSize);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("HALF_KERNEL_SIZE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_StackerOcr.nThresholdOffset);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("THRESHOLD_OFFSET"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_StackerOcr.nDepthOffset);
	::WritePrivateProfileString(STACKEROCR_APPNAME, _T("DEPTH_OFFSET"), strTemp, strModelInfoFile);

	for (int i = 0; i < nTRAY_MASKING_MAX; i++)
	{
		strBuff.Format(_T("MASKING_%d_"), i);

		strTemp.Format(_T("%d"), m_StackerOcr.rcMaskingArea[i].left);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_LEFT"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_StackerOcr.rcMaskingArea[i].top);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_TOP"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_StackerOcr.rcMaskingArea[i].right);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_RIGHT"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_StackerOcr.rcMaskingArea[i].bottom);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("AREA_BOTTOM"), strTemp, strModelInfoFile);

		strTemp.Format(_T("%d"), m_StackerOcr.nMaskingAreaDistX[i]);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_X"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_StackerOcr.nMaskingAreaDistY[i]);
		::WritePrivateProfileString(TRAYOCR_APPNAME, strBuff + _T("DIS_Y"), strTemp, strModelInfoFile);
	}
}

void CModelInfo::SaveBandingInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format( _T("%d"), m_BandingInfo.nUseBypass_Banding );
	::WritePrivateProfileString( BANDING_APPNAME, _T("USE_BANDING_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format(_T("%s"), m_BandingInfo.strDescription);
	::WritePrivateProfileString(BANDING_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_BandingInfo.nValueCh1);
	::WritePrivateProfileString(BANDING_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	// Center
	strTemp.Format(_T("%d"), m_BandingInfo.rcInspArea.left);
	::WritePrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BandingInfo.rcInspArea.right);
	::WritePrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BandingInfo.rcInspArea.top);
	::WritePrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BandingInfo.rcInspArea.bottom);
	::WritePrivateProfileString(BANDING_APPNAME, _T("INSP_AREA_BTM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_BandingInfo.nThreshold);
	::WritePrivateProfileString(BANDING_APPNAME, _T("THRESHOLD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BandingInfo.nBandWidthSpec);
	::WritePrivateProfileString(BANDING_APPNAME, _T("SPEC"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BandingInfo.nBandWidthRange);
	::WritePrivateProfileString(BANDING_APPNAME, _T("RANGE"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveHICInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.nUseBypass_HIC);
	::WritePrivateProfileString(HIC_APPNAME, _T("USE_HIC_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%s"), m_HICInfo.strDescription);
	::WritePrivateProfileString(HIC_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.nValueCh1);
	::WritePrivateProfileString(HIC_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.nColorRedLuminance);
	::WritePrivateProfileString(HIC_APPNAME, _T("COLOR_RED_LUMINANCE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nColorGreenLuminance);
	::WritePrivateProfileString(HIC_APPNAME, _T("COLOR_GREEN_LUMINANCE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nColorBlueLuminance);
	::WritePrivateProfileString(HIC_APPNAME, _T("COLOR_BLUE_LUMINANCE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.nCircleOuterDiameter60);
	::WritePrivateProfileString(HIC_APPNAME, _T("CIRCLE_OUTER_DIAMETER_60"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nCircleOuterDiameter10);
	::WritePrivateProfileString(HIC_APPNAME, _T("CIRCLE_OUTER_DIAMETER_10"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nCircleOuterDiameter5);
	::WritePrivateProfileString(HIC_APPNAME, _T("CIRCLE_OUTER_DIAMETER_5"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.nCircleInnerDiameter60);
	::WritePrivateProfileString(HIC_APPNAME, _T("CIRCLE_INNER_DIAMETER_60"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nCircleInnerDiameter10);
	::WritePrivateProfileString(HIC_APPNAME, _T("CIRCLE_INNER_DIAMETER_10"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nCircleInnerDiameter5);
	::WritePrivateProfileString(HIC_APPNAME, _T("CIRCLE_INNER_DIAMETER_5"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.nDisColor);
	::WritePrivateProfileString(HIC_APPNAME, _T("DIS_COLOR"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.nPixelAvgValue);
	::WritePrivateProfileString(HIC_APPNAME, _T("AVG_VALUE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_HICInfo.ptOffset_HIC.x);
	::WritePrivateProfileString(HIC_APPNAME, _T("HIC_OFFSET_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_HICInfo.ptOffset_HIC.y);
	::WritePrivateProfileString(HIC_APPNAME, _T("HIC_OFFSET_Y"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.2f"), m_HICInfo.fRatio);
	::WritePrivateProfileString(HIC_APPNAME, _T("RATIO"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveDesiccantCuttingInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("USE_POSITION_BYPASS"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("USE_POSITION2_BYPASS"), strTemp, strModelInfoFile);

	strTemp.Format( _T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft);
	::WritePrivateProfileString( DESICCANT_APPNAME, _T("USE_DESICCANT_BYPASS"), strTemp, strModelInfoFile );
	strTemp.Format( _T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight);
	::WritePrivateProfileString( DESICCANT_APPNAME, _T("USE_DESICCANT2_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format(_T("%s"), m_DesiccantCuttingInfo.strDescription);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	
	// Desiccant Cutting Line
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nDesiccantCuttingPixelValue);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_CUTTING_PIXEL_VALUE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.1lf"), m_DesiccantCuttingInfo.dSealingLength);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("SEALING_LENGTH"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.1lf"), m_DesiccantCuttingInfo.dColorLineLengthDifference);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("COLOR_LINE_LENGTH_DIFFERENCE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcLeftInspArea.left);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcLeftInspArea.top);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcLeftInspArea.right);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcLeftInspArea.bottom);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LEFT_INSP_BOTTOM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcRightInspArea.left);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcRightInspArea.top);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcRightInspArea.right);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcRightInspArea.bottom);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("RIGHT_INSP_BOTTOM"), strTemp, strModelInfoFile);


	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nValueCh1);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nValueCh2);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH2"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nValueCh3);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH3"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nValueCh4);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("LIGHT_CH4"), strTemp, strModelInfoFile);

	// Desiccant Position
	strTemp.Format(_T("%d"),	m_DesiccantCuttingInfo.nDesiccantPixelValue);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_PIXEL_VALUE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.3lf"), m_DesiccantCuttingInfo.dDesiccantWidth);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_WIDTH"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.3lf"), m_DesiccantCuttingInfo.dDesiccantHeight);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("DESICCANT_HEIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.3lf"), m_DesiccantCuttingInfo.dPosColorLineLengthDiff);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_COLOR_LINE_LENGTH_DIFFERENCE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nROIWidth);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("ROI_WIDTH"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nROIHeight);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("ROI_HEIGHT"), strTemp, strModelInfoFile);	
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nColorLineThreshold);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("COLOR_LINE_VALUE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionLeftInspArea.left);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionLeftInspArea.top);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionLeftInspArea.right);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionLeftInspArea.bottom);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_LEFT_INSP_BOTTOM"), strTemp, strModelInfoFile);	
	
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionRightInspArea.left);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionRightInspArea.top);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionRightInspArea.right);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.rcPositionRightInspArea.bottom);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("POSITION_RIGHT_INSP_BOTTOM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nRoiOffsetLeft);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("ROI_OFFSET_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nRoiOffsetRight);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("ROI_OFFSET_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nOpenBoxWidth);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("OPEN_BOX_WIDTH"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantCuttingInfo.nOpenBoxHeight);
	::WritePrivateProfileString(DESICCANT_APPNAME, _T("OPEN_BOX_HEIGHT"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveDesiccantMaterialInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nUseBypass_Material);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("USE_MATERIAL_BYPASS"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%s"), m_DesiccantMaterialInfo.strDescription);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nValueCh1);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nValueCh2);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("LIGHT_CH2"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nInspectionZoneCol);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("INSPECTION_ZONE_COL"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nInspectionZoneMargin);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("INSPECTION_ZONE_MARGIN"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nDesiccantQuantity);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT_QUANTITY"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptHicShift.x);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("HIC_SHIFT_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptHicShift.y);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("HIC_SHIFT_Y"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptDesiccant1Shift.x);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT1_SHIFT_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptDesiccant1Shift.y);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT1_SHIFT_Y"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptDesiccant2Shift.x);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT2_SHIFT_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptDesiccant2Shift.y);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("DESICCANT2_SHIFT_Y"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptPatternShiftTolerance.x);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("PATTERN_SHIFT_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.ptPatternShiftTolerance.y);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("PATTERN_SHIFT_Y"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.1lf"), m_DesiccantMaterialInfo.fMatchScoreHIC);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("MATCH_SCORE_HIC"), strTemp, strModelInfoFile);	
	strTemp.Format(_T("%.1lf"), m_DesiccantMaterialInfo.fMatchScoreDesiccant);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("MATCH_SCORE_DESICCANT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nRatio1);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("1ST_RATIO"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nRatio2);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("2ND_RATIO"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialInfo.nRatio3);
	::WritePrivateProfileString(SUBMATERIAL_APPNAME, _T("3RD_RATIO"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveDesiccantMaterialTrayInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format(_T("%s"), m_DesiccantMaterialTrayInfo.strDescription);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.nTrayQuantityMin);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("TRAY_AMOUNT_MIN"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.nTrayQuantityMax);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("TRAY_AMOUNT_MAX"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMin.left);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMin.top);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMin.right);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMin.bottom);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MIN_BOTTOM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMax.left);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMax.top);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMax.right);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_DesiccantMaterialTrayInfo.rcTrayRoiMax.bottom);
	::WritePrivateProfileString(SUBMATERIALTRAY_APPNAME, _T("INSP_AREA_MAX_BOTTOM"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveMBBInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp, strBuff;

	::DeleteFile(strModelInfoFile);

	strTemp.Format( _T("%d"), m_MBBInfo.nUseBypass_MBB );
	::WritePrivateProfileString( MBB_APPNAME, _T("USE_MBB_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format( _T("%s"), m_MBBInfo.strDescription );
	::WritePrivateProfileString(MBB_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.nValueCh1);
	::WritePrivateProfileString(MBB_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_MBBInfo.rcInspArea.left);
	::WritePrivateProfileString(MBB_APPNAME, _T("INSP_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.rcInspArea.right);
	::WritePrivateProfileString(MBB_APPNAME, _T("INSP_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.rcInspArea.top);
	::WritePrivateProfileString(MBB_APPNAME, _T("INSP_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.rcInspArea.bottom);
	::WritePrivateProfileString(MBB_APPNAME, _T("INSP_AREA_BTM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.2f"), m_MBBInfo.fTolerance);
	::WritePrivateProfileString(MBB_APPNAME, _T("TOLERANCE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%.2f"), m_MBBInfo.fLength);
	::WritePrivateProfileString(MBB_APPNAME, _T("LENGHT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_MBBInfo.nWidth);
	::WritePrivateProfileString(MBB_APPNAME, _T("WIDTH"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.nWidth_PX);
	::WritePrivateProfileString(MBB_APPNAME, _T("WIDTH_PX"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.nHeight);
	::WritePrivateProfileString(MBB_APPNAME, _T("HEIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.nHeight_PX);
	::WritePrivateProfileString(MBB_APPNAME, _T("HEIGHT_PX"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.nRange);
	::WritePrivateProfileString(MBB_APPNAME, _T("RANGE"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_MBBInfo.nRange_PX);
	::WritePrivateProfileString(MBB_APPNAME, _T("RANGE_PX"), strTemp, strModelInfoFile);

	for (int i = 0; i < nMATCH_MAX; i++)
	{
		strBuff.Format(_T("MATCH_%d_"), i);

		strTemp.Format(_T("%d"), m_MBBInfo.ptOffset_MBB[i].x);
		::WritePrivateProfileString(MBB_APPNAME, strBuff + _T("CENTER_X"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_MBBInfo.ptOffset_MBB[i].y);
		::WritePrivateProfileString(MBB_APPNAME, strBuff + _T("CENTER_Y"), strTemp, strModelInfoFile);
	}

	strTemp.Format(_T("%.2f"), m_MBBInfo.fRatio);
	::WritePrivateProfileString(MBB_APPNAME, _T("RATIO"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_MBBInfo.nSelectUnit);
	::WritePrivateProfileString(MBB_APPNAME, _T("SELECT_UNIT"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveBoxInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp, strBuff;

	::DeleteFile(strModelInfoFile);

	strTemp.Format( _T("%d"), m_BoxInfo.nUseBypass_Box );
	::WritePrivateProfileString( BOXQUALITY_APPNAME, _T("USE_BOX_QUALITY_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format(_T("%s"), m_BoxInfo.strDescription);
	::WritePrivateProfileString(BOXQUALITY_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BoxInfo.nValueCh1);
	::WritePrivateProfileString(BOXQUALITY_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%.1f"), m_BoxInfo.fOkScore);
	::WritePrivateProfileString(BOXQUALITY_APPNAME, _T("OK_SCORE"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_BoxInfo.nShiftX);
	::WritePrivateProfileString(BOXQUALITY_APPNAME, _T("SHIFT_X"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_BoxInfo.nShiftY);
	::WritePrivateProfileString(BOXQUALITY_APPNAME, _T("SHIFT_Y"), strTemp, strModelInfoFile);

	for(int i = 0; i < nMATCH_MAX; i++)
	{
		strBuff.Format(_T("MATCH_%d_"), i);

		strTemp.Format(_T("%d"), m_BoxInfo.ptMatchCenter[i].x);
		::WritePrivateProfileString(BOXQUALITY_APPNAME, strBuff + _T("CENTER_X"), strTemp, strModelInfoFile);
		strTemp.Format(_T("%d"), m_BoxInfo.ptMatchCenter[i].y);
		::WritePrivateProfileString(BOXQUALITY_APPNAME, strBuff + _T("CENTER_Y"), strTemp, strModelInfoFile);
	}
}

void CModelInfo::SaveSealingInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format( _T("%d"), m_SealingInfo.nUseBypass_Sealing );
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("USE_SEALING_QUALITY_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format(_T("%s"), m_SealingInfo.strDescription);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_SealingInfo.nValueCh1);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_SealingInfo.nCount);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("COUNT"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_SealingInfo.nInspSpec);
	::WritePrivateProfileString(SEALINGQUALITY_APPNAME, _T("INSP_SPEC"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_SealingInfo.rcInspArea.left);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_LEFT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_SealingInfo.rcInspArea.right);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_RIGHT"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_SealingInfo.rcInspArea.top);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_TOP"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_SealingInfo.rcInspArea.bottom);
	::WritePrivateProfileString( SEALINGQUALITY_APPNAME, _T("INSP_AREA_BTM"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_SealingInfo.nSelectUnit);
	::WritePrivateProfileString(SEALINGQUALITY_APPNAME, _T("SELECT_UNIT"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveTapeInfo(LPCTSTR lpszModelInfoFile)
{
	CString strModelInfoFile = lpszModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	strTemp.Format( _T("%d"), m_TapeInfo.nUseBypass_Tape );
	::WritePrivateProfileString( BOXTAPE_APPNAME, _T("USE_BOX_TAPE_BYPASS"), strTemp, strModelInfoFile );

	strTemp.Format(_T("%s"), m_TapeInfo.strDescription);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TapeInfo.nValueCh1);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	strTemp.Format(_T("%d"), m_TapeInfo.nRedThreshold);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("RED_THRESHOLD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TapeInfo.nGreenThreshold);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("GREEN_THRESHOLD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TapeInfo.nBlueThreshold);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("BLUE_THRESHOLD"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TapeInfo.nTapeWidth);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("TAPE_WIDTH"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_TapeInfo.nTapeHeight);
	::WritePrivateProfileString(BOXTAPE_APPNAME, _T("TAPE_HEIGHT"), strTemp, strModelInfoFile);
}

void CModelInfo::SaveLabelInfo( LPCTSTR lpszModelInfoFile, LPCTSTR lpszCustomerModelInfoFile )
{
	CString strModelInfoFile = lpszCustomerModelInfoFile;
	CString strTemp;

	::DeleteFile(strModelInfoFile);

	//////////////////////////////////////////////////////////////////////////
	CString strBuff;
	strTemp.Format(_T("%s"), m_LabelInfo.strDescription);
	::WritePrivateProfileString(LABEL_APPNAME, _T("DESCRIPTION"), strTemp, strModelInfoFile);
	strTemp.Format(_T("%d"), m_LabelInfo.nValueCh1);
	::WritePrivateProfileString(LABEL_APPNAME, _T("LIGHT_CH1"), strTemp, strModelInfoFile);

	strTemp.Format( _T("%.1f"), m_LabelInfo.fLabelEdgeOffset );
	::WritePrivateProfileString( LABEL_APPNAME, _T("EDGE_OFFSET"), strTemp, strModelInfoFile );

	strTemp.Format( _T("%.1f"), m_LabelInfo.fLabelAngleSpec );
	::WritePrivateProfileString( LABEL_APPNAME, _T("ANGLE_SPEC"), strTemp, strModelInfoFile );

	strTemp.Format( _T("%d"), m_LabelInfo.nLabelMaskingCount );
	::WritePrivateProfileString( LABEL_APPNAME, _T("MASKING_COUNT"), strTemp, strModelInfoFile );

	for(int j = 0; j < EMATCH_TYPE_LABEL_MAX - 1; j ++)
	{
		strBuff.Format(_T("MASKING_%d_"), j);

		strTemp.Format( _T("%d"), m_LabelInfo.ptOffset_Lable[j].x );
		::WritePrivateProfileString( LABEL_APPNAME, strBuff + _T("OFFSET_X"), strTemp, strModelInfoFile );
		strTemp.Format( _T("%d"), m_LabelInfo.ptOffset_Lable[j].y );
		::WritePrivateProfileString( LABEL_APPNAME, strBuff + _T("OFFSET_Y"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelInfo.nLabelMaskingScoreType[j] );
		::WritePrivateProfileString( LABEL_APPNAME, strBuff + _T("SCORE"), strTemp, strModelInfoFile );
	}

	//Insp' Area
	CModelInfo::stLabelInfo::InspVecIter iterBegin = m_LabelInfo.stInspectArea.Areas.begin();
	CModelInfo::stLabelInfo::InspVecIter iterEnd = m_LabelInfo.stInspectArea.Areas.end();

	int nIndex = 0;
	CString strKeyName = _T("");

	strKeyName.Format(_T("INSP_AREA"));
	strTemp.Format(_T("%d"), m_LabelInfo.stInspectArea.Areas.size());
	::WritePrivateProfileString(LABEL_APPNAME, strKeyName, strTemp, strModelInfoFile);

	if (m_LabelInfo.stInspectArea.Areas.size() == 0) return;

	for (CModelInfo::stLabelInfo::InspVecIter iter = iterBegin; iter != iterEnd; ++iter, ++nIndex)
	{
		CRect& rcArea = (*iter);

		strKeyName.Format(_T("INSP_AREA_%02d_LEFT"), nIndex);
		strTemp.Format(_T("%d"), rcArea.left);
		::WritePrivateProfileString(LABEL_APPNAME, strKeyName, strTemp, strModelInfoFile);
		strKeyName.Format(_T("INSP_AREA_%02d_RIGHT"), nIndex);
		strTemp.Format(_T("%d"), rcArea.right);
		::WritePrivateProfileString(LABEL_APPNAME, strKeyName, strTemp, strModelInfoFile);
		strKeyName.Format(_T("INSP_AREA_%02d_TOP"), nIndex);
		strTemp.Format(_T("%d"), rcArea.top);
		::WritePrivateProfileString(LABEL_APPNAME, strKeyName, strTemp, strModelInfoFile);
		strKeyName.Format(_T("INSP_AREA_%02d_BOTTOM"), nIndex);
		strTemp.Format(_T("%d"), rcArea.bottom);
		::WritePrivateProfileString(LABEL_APPNAME, strKeyName, strTemp, strModelInfoFile);
	}

	//////////////////////////////////////////////////////////////////////////
	strModelInfoFile = lpszModelInfoFile;

	strTemp.Format( _T("%d"), m_LabelInfo.nUseBypass_Label );
	::WritePrivateProfileString( LABEL_APPNAME, _T("USE_LABEL_BYPASS"), strTemp, strModelInfoFile );
}

void CModelInfo::SaveLabelManualOptionInfo( LPCTSTR lpszCustomerModelInfoFile )
{
	CString strModelInfoFile = lpszCustomerModelInfoFile;
	::DeleteFile(strModelInfoFile);

	CString strTemp, strLabelName;
	strLabelName.Format(_T("LABEL"));

	for(int j = 0; j < OP_MAX; j++)
	{
		CString strBuff;
		strBuff.Format(_T("OPTION%02d_"), j);

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].nSegmentLine_Num );
		::WritePrivateProfileString( strLabelName, strBuff + _T("SEGMENT_LINE_NUM"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].nImageTypeIndex );
		::WritePrivateProfileString( strLabelName, strBuff + _T("IMAGE_TYPE"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].bUseMaxThreshold );
		::WritePrivateProfileString( strLabelName, strBuff + _T("USE_MAX_THRESHOLD"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].bUseImageFiltering );
		::WritePrivateProfileString( strLabelName, strBuff + _T("USE_IMAGE_FILTERING"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].bUseeVisionLargeCharsCut );
		::WritePrivateProfileString( strLabelName, strBuff + _T("USE_eVISION_LARGECHARSCUT"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].FontMinSize.cx );
		::WritePrivateProfileString( strLabelName, strBuff + _T("FONT_MIN_WIDTH"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].FontMinSize.cy );
		::WritePrivateProfileString( strLabelName, strBuff + _T("FONT_MIN_HEIGHT"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].FontMaxSize.cx );
		::WritePrivateProfileString( strLabelName, strBuff + _T("FONT_MAX_WIDTH"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%d"), m_LabelManualOptionInfo[j].FontMaxSize.cy );
		::WritePrivateProfileString( strLabelName, strBuff + _T("FONT_MAX_HEIGHT"), strTemp, strModelInfoFile );
	}
}

void CModelInfo::SaveSimilarFontInfo()
{
	CString strModelInfoFile;
	strModelInfoFile = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\SimilarFont.ini");

	CString strTemp;
	::DeleteFile(strModelInfoFile);

	CString strFontNum;
	for( int nIndex = 0; nIndex < 50; nIndex++)
	{
		strFontNum.Format(_T("FONT_%02d"), nIndex);

		strTemp.Format( _T("%s"), m_LabelInfo.vstrSimilarFont[0].at(nIndex) );
		::WritePrivateProfileString( strFontNum, _T("TARGET_0"), strTemp, strModelInfoFile );

		strTemp.Format( _T("%s"), m_LabelInfo.vstrSimilarFont[1].at(nIndex) );
		::WritePrivateProfileString( strFontNum, _T("TARGET_1"), strTemp, strModelInfoFile );
	}
}

BOOL CModelInfo::Save( eTeachTabIndex TabIndex )
{
	CString strRecipePath						= GetRecipePath(m_strModelName);
	CString strModelInfoFile					= strRecipePath + _T("ModelInfo.ini");

	// 1G
	CString strTrayOcrFile						= GetRecipeRootPath() + _T("1G_TrayOcr\\") + m_strModelNameTrayOcr + _T("\\");
	CString str3DChipCntFile					= GetRecipeRootPath() + _T("1G_3DChipCnt\\") + m_strModelName3DChipCnt + _T("\\");
	CString strChipOcrFile						= GetRecipeRootPath() + _T("1G_ChipOcr\\") + m_strModelNameChipOcr + _T("\\");
	CString strChipFile							= GetRecipeRootPath() + _T("1G_Chip\\") + m_strModelNameChip + _T("\\");
	CString strMixingFile						= GetRecipeRootPath() + _T("1G_Mixing\\") + m_strModelNameMixing + _T("\\");
	CString strLiftInfoFile						= GetRecipeRootPath() + _T("1G_Lift\\") + m_strModelNameLift + _T("\\");

	// SG
	CString strStackerOcrFile					= GetRecipeRootPath() + _T("SG_StackerOcr\\") + m_strModelNameStackerOcr + _T("\\");

	// 2G
	CString strBandingInfoFile					= GetRecipeRootPath() + _T("2G_Banding\\") + m_strModelNameBanding + _T("\\");
	CString strHICInfoFile						= GetRecipeRootPath() + _T("2G_HIC\\") + m_strModelNameHIC + _T("\\");

	// 3G
	CString strDesiccantCuttingInfoFile			= GetRecipeRootPath() + _T("3G_Desiccant_Cutting\\") + m_strModelNameDesiccantCutting + _T("\\");
	CString strDesiccantMaterialInfoFile		= GetRecipeRootPath() + _T("3G_Desiccant_Material\\") + m_strModelNameDesiccantMaterial + _T("\\");
	CString strDesiccantMaterialTrayInfoFile	= GetRecipeRootPath() + _T("3G_Desiccant_Material_Tray\\") + m_strModelNameDesiccantMaterialTray + _T("\\");

	// 4G
	CString strMBBInfoFile						= GetRecipeRootPath() + _T("4G_MBB\\") + m_strModelNameMBB + _T("\\");
	CString strLabelInfoFile					= GetRecipeRootPath() + _T("Label\\") + m_strModelNameLabel + _T("\\");
	CString strLabelInfoFile_Customer			= GetRecipeRootPath() + _T("Label\\") + m_strModelNameLabel + _T("\\");

	// 5G
	CString strBoxInfoFile						= GetRecipeRootPath() + _T("5G_Box\\") + m_strModelNameBoxQuality + _T("\\");
	CString strSealingInfoFile					= GetRecipeRootPath() + _T("5G_Sealing\\") + m_strModelNameSealingQuality + _T("\\");

	// 6G
	CString strTapeInfoFile						= GetRecipeRootPath() + _T("6G_Tape\\") + m_strModelNameTape + _T("\\");

	// ModelInfo
	CString strTrayOcrModelInfo					= strTrayOcrFile;
	CString str3DChipCntModelInfo				= str3DChipCntFile;
	CString strChipOcrModelInfo					= strChipOcrFile;
	CString strChipModelInfo					= strChipFile;
	CString strMixingModelInfo					= strMixingFile;
	CString strLiftModelInfo					= strLiftInfoFile;
	CString strStackerOcrModelInfo				= strStackerOcrFile;
	CString strBandingModelInfo					= strBandingInfoFile;
	CString strHICModelInfo						= strHICInfoFile;
	CString strDesiccantCuttingModelInfo		= strDesiccantCuttingInfoFile;
	CString strDesiccantMaterialModelInfo		= strDesiccantMaterialInfoFile;
	CString strDesiccantMaterialTrayModelInfo	= strDesiccantMaterialTrayInfoFile;
	CString strMBBModelInfo						= strMBBInfoFile;
	CString strBoxModelInfo						= strBoxInfoFile;
	CString strSealingModelInfo					= strSealingInfoFile;
	CString strTapeModelInfo					= strTapeInfoFile;
	CString strLabelModelInfo					= strLabelInfoFile;

	// recipe
	switch( TabIndex )
	{
#ifdef RELEASE_1G
	case TEACH_TAB_IDX_TRAYOCR:
		MakeDirectory((LPCTSTR)strTrayOcrFile);
		strTrayOcrFile += _T("1G_TrayOcr.ini");				SaveTrayOcr(strTrayOcrFile);
		strTrayOcrModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strTrayOcrModelInfo);
		break;

	case TEACH_TAB_IDX_3DCHIPCNT:
		MakeDirectory((LPCTSTR)str3DChipCntFile);
		str3DChipCntFile += _T("1G_3DChipCnt.ini");			Save3DChipCnt(str3DChipCntFile);
		str3DChipCntModelInfo += _T("ModelInfo.ini");		SaveModelInfo(str3DChipCntModelInfo);
		break;

	case TEACH_TAB_IDX_CHIPOCR:
		MakeDirectory((LPCTSTR)strChipOcrFile);
		strChipOcrFile += _T("1G_ChipOcr.ini");				SaveChipOcr(strChipOcrFile);
		strChipOcrModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strChipOcrModelInfo);
		break;

	case TEACH_TAB_IDX_CHIP:
		MakeDirectory((LPCTSTR)strChipFile);
		strChipFile += _T("1G_Chip.ini");					SaveChip(strChipFile);
		strChipModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strChipModelInfo);
		break;

	case TEACH_TAB_IDX_MIXING:
		MakeDirectory((LPCTSTR)strMixingFile);
		strMixingFile += _T("1G_Mixing.ini");				SaveMixing(strMixingFile);
		strMixingModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strMixingModelInfo);
		break;

	case TEACH_TAB_IDX_LIFTINFO:
		MakeDirectory((LPCTSTR)strLiftInfoFile);
		strLiftInfoFile += _T("1G_Lift.ini");				SaveLiftInfo(strLiftInfoFile);
		strLiftModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strLiftModelInfo);
		break;

	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strTrayOcrFile);
		strTrayOcrFile += _T("1G_TrayOcr.ini");				SaveTrayOcr(strTrayOcrFile);
		strTrayOcrModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strTrayOcrModelInfo);

		MakeDirectory((LPCTSTR)str3DChipCntFile);
		str3DChipCntFile += _T("1G_3DChipCnt.ini");			Save3DChipCnt(str3DChipCntFile);
		str3DChipCntModelInfo += _T("ModelInfo.ini");		SaveModelInfo(str3DChipCntModelInfo);
		
		MakeDirectory((LPCTSTR)strChipOcrFile);
		strChipOcrFile += _T("1G_ChipOcr.ini");				SaveChipOcr(strChipOcrFile);
		strChipOcrModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strChipOcrModelInfo);

		MakeDirectory((LPCTSTR)strChipFile);
		strChipFile += _T("1G_Chip.ini");					SaveChip(strChipFile);
		strChipModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strChipModelInfo);

		MakeDirectory((LPCTSTR)strMixingFile);
		strMixingFile += _T("1G_Mixing.ini");				SaveMixing(strMixingFile);
		strMixingModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strMixingModelInfo);

		MakeDirectory((LPCTSTR)strLiftInfoFile);
		strLiftInfoFile += _T("1G_Lift.ini");				SaveLiftInfo(strLiftInfoFile);
		strLiftModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strLiftModelInfo);
		break;

#elif RELEASE_SG
	case TEACH_TAB_IDX_STACKER_OCR:
	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strStackerOcrFile);
		strStackerOcrFile += _T("SG_StackerOcr.ini");		SaveStackerOcr(strStackerOcrFile);
		strStackerOcrModelInfo += _T("ModelInfo.ini");		SaveModelInfo(strStackerOcrModelInfo);
		break;

#elif RELEASE_2G
	case TEACH_TAB_IDX_BANDING:
		MakeDirectory((LPCTSTR)strBandingInfoFile);
		strBandingInfoFile += _T("2G_Banding.ini");			SaveBandingInfo(strBandingInfoFile);
		strBandingModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strBandingModelInfo);
		break;
	case TEACH_TAB_IDX_HIC:
		MakeDirectory((LPCTSTR)strHICInfoFile);
		strHICInfoFile += _T("2G_HIC.ini");					SaveHICInfo(strHICInfoFile);
		strHICModelInfo += _T("ModelInfo.ini");				SaveModelInfo(strHICModelInfo);
		break;

	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strBandingInfoFile);
		strBandingInfoFile += _T("2G_Banding.ini");			SaveBandingInfo(strBandingInfoFile);
		strBandingModelInfo += _T("ModelInfo.ini");			SaveModelInfo(strBandingModelInfo);

		MakeDirectory((LPCTSTR)strHICInfoFile);
		strHICInfoFile += _T("2G_HIC.ini");					SaveHICInfo(strHICInfoFile);
		strHICModelInfo += _T("ModelInfo.ini");				SaveModelInfo(strHICModelInfo);
		break;

#elif RELEASE_3G
	case TEACH_TAB_IDX_DESICCANT_CUTTING:
		MakeDirectory((LPCTSTR)strDesiccantCuttingInfoFile);
		strDesiccantCuttingInfoFile += _T("3G_Desiccant_Cutting.ini");				SaveDesiccantCuttingInfo(strDesiccantCuttingInfoFile);
		strDesiccantCuttingModelInfo += _T("ModelInfo.ini");						SaveModelInfo(strDesiccantCuttingModelInfo);
		break;

	case TEACH_TAB_IDX_DESICCANT_MATERIAL:
		MakeDirectory((LPCTSTR)strDesiccantMaterialInfoFile);
		strDesiccantMaterialInfoFile += _T("3G_Desiccant_Material.ini");			SaveDesiccantMaterialInfo(strDesiccantMaterialInfoFile);
		strDesiccantMaterialModelInfo += _T("ModelInfo.ini");						SaveModelInfo(strDesiccantMaterialModelInfo);

	case TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY:
		MakeDirectory((LPCTSTR)strDesiccantMaterialTrayInfoFile);
		strDesiccantMaterialTrayInfoFile += _T("3G_Desiccant_Material_Tray.ini");	SaveDesiccantMaterialTrayInfo(strDesiccantMaterialTrayInfoFile);
		strDesiccantMaterialTrayModelInfo	+= _T("ModelInfo.ini");					SaveModelInfo(strDesiccantMaterialTrayModelInfo);
		break;

	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strDesiccantCuttingInfoFile);
		strDesiccantCuttingInfoFile += _T("3G_Desiccant_Cutting.ini");				SaveDesiccantCuttingInfo(strDesiccantCuttingInfoFile);
		strDesiccantCuttingModelInfo += _T("ModelInfo.ini");						SaveModelInfo(strDesiccantCuttingModelInfo);

		MakeDirectory((LPCTSTR)strDesiccantMaterialInfoFile);
		strDesiccantMaterialInfoFile += _T("3G_Desiccant_Material.ini");			SaveDesiccantMaterialInfo(strDesiccantMaterialInfoFile);
		strDesiccantMaterialModelInfo += _T("ModelInfo.ini");						SaveModelInfo(strDesiccantMaterialModelInfo);

		MakeDirectory((LPCTSTR)strDesiccantMaterialTrayInfoFile);
		strDesiccantMaterialTrayInfoFile += _T("3G_Desiccant_Material_Tray.ini");	SaveDesiccantMaterialTrayInfo(strDesiccantMaterialTrayInfoFile);
		strDesiccantMaterialTrayModelInfo += _T("ModelInfo.ini");					SaveModelInfo(strDesiccantMaterialTrayModelInfo);
		break;

#elif RELEASE_4G
	case TEACH_TAB_IDX_MBB:
		MakeDirectory((LPCTSTR)strMBBInfoFile);
		strMBBInfoFile	+= _T("4G_MBB.ini");										SaveMBBInfo(strMBBInfoFile);
		strMBBModelInfo += _T("ModelInfo.ini");										SaveModelInfo(strMBBModelInfo);
		break;
		
	case TEACH_TAB_IDX_LABEL:
		MakeDirectory((LPCTSTR)strLabelInfoFile);
		strLabelInfoFile				+= _T("LabelInfo.ini");		
		strLabelInfoFile_Customer		+= _T("LabelManualOption.ini");				SaveLabelInfo(strLabelInfoFile, strLabelInfoFile_Customer);
		strLabelModelInfo				+= _T("ModelInfo.ini");						SaveModelInfo(strLabelModelInfo);
		break;

	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strMBBInfoFile);
		strMBBInfoFile	+= _T("4G_MBB.ini");										SaveMBBInfo(strMBBInfoFile);
		strMBBModelInfo += _T("ModelInfo.ini");										SaveModelInfo(strMBBModelInfo);

		MakeDirectory((LPCTSTR)strLabelInfoFile);
		strLabelInfoFile			+= _T("LabelInfo.ini");
		strLabelInfoFile_Customer	+= _T("LabelManualOption.ini");					SaveLabelInfo(strLabelInfoFile, strLabelInfoFile_Customer);
		strLabelModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strLabelModelInfo);
		break;

#elif RELEASE_5G
	case TEACH_TAB_IDX_BOX:
		MakeDirectory((LPCTSTR)strBoxInfoFile);
		strBoxInfoFile				+= _T("5G_Box.ini");							SaveBoxInfo(strBoxInfoFile);
		strBoxModelInfo				+= _T("ModelInfo.ini");							SaveModelInfo(strBoxModelInfo);
		break;

	case TEACH_TAB_IDX_SEALING:
		MakeDirectory((LPCTSTR)strSealingInfoFile);
		strSealingInfoFile			+= _T("5G_Sealing.ini");						SaveSealingInfo(strSealingInfoFile);
		strSealingModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strSealingModelInfo);
		break;

	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strBoxInfoFile);
		strBoxInfoFile				+= _T("5G_Box.ini");							SaveBoxInfo(strBoxInfoFile);
		strBoxModelInfo				+= _T("ModelInfo.ini");							SaveModelInfo(strBoxModelInfo);

		MakeDirectory((LPCTSTR)strSealingInfoFile);
		strSealingInfoFile			+= _T("5G_Sealing.ini");						SaveSealingInfo(strSealingInfoFile);
		strSealingModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strSealingModelInfo);
		break;

#elif RELEASE_6G
	case TEACH_TAB_IDX_LABEL:
		MakeDirectory((LPCTSTR)strLabelInfoFile);
		strLabelInfoFile			+= _T("LabelInfo.ini");
		strLabelInfoFile_Customer	+= _T("LabelManualOption.ini");					SaveLabelInfo(strLabelInfoFile, strLabelInfoFile_Customer);
		strLabelModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strLabelModelInfo);
		break;

	case TEACH_TAB_IDX_TAPE:
		MakeDirectory((LPCTSTR)strTapeInfoFile);
		strTapeInfoFile				+= _T("6G_Tape.ini");							SaveTapeInfo(strTapeInfoFile);
		strTapeModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strTapeModelInfo);
		break;

	case TEACH_TAB_IDX_ALL:
		MakeDirectory((LPCTSTR)strLabelInfoFile);
		strLabelInfoFile			+= _T("LabelInfo.ini");
		strLabelInfoFile_Customer	+= _T("LabelManualOption.ini");					SaveLabelInfo(strLabelInfoFile, strLabelInfoFile_Customer);
		strLabelModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strLabelModelInfo);

		MakeDirectory((LPCTSTR)strTapeInfoFile);
		strTapeInfoFile				+= _T("6G_Tape.ini");							SaveTapeInfo(strTapeInfoFile);
		strTapeModelInfo			+= _T("ModelInfo.ini");							SaveModelInfo(strTapeModelInfo);
		break;
#endif

	default:
		break;
	}

	return TRUE;
}

BOOL CModelInfo::Save_JobNumber(eTeachTabIndex TabIndex)
{
	CString strJobNumber = _T("");

	switch (TabIndex)
	{
#ifdef RELEASE_1G
	case TEACH_TAB_IDX_TRAYOCR:						strJobNumber = GetRecipeRootPath() + _T("1G_TrayOcr\\");						break;
	case TEACH_TAB_IDX_3DCHIPCNT:					strJobNumber = GetRecipeRootPath() + _T("1G_3DChipCnt\\");						break;
	case TEACH_TAB_IDX_CHIPOCR:						strJobNumber = GetRecipeRootPath() + _T("1G_ChipOcr\\");						break;
	case TEACH_TAB_IDX_CHIP:						strJobNumber = GetRecipeRootPath() + _T("1G_Chip\\");							break;
	case TEACH_TAB_IDX_MIXING:						strJobNumber = GetRecipeRootPath() + _T("1G_Mixing\\");							break;
	case TEACH_TAB_IDX_LIFTINFO:					strJobNumber = GetRecipeRootPath() + _T("1G_Lift\\");							break;

#elif RELEASE_SG
	case TEACH_TAB_IDX_STACKER_OCR:					strJobNumber = GetRecipeRootPath() + _T("SG_StackerOcr\\");						break;

#elif RELEASE_2G
	case TEACH_TAB_IDX_BANDING:						strJobNumber = GetRecipeRootPath() + _T("2G_Banding\\");						break;
	case TEACH_TAB_IDX_HIC:							strJobNumber = GetRecipeRootPath() + _T("2G_HIC\\");							break;

#elif RELEASE_3G
	case TEACH_TAB_IDX_DESICCANT_CUTTING:			strJobNumber = GetRecipeRootPath() + _T("3G_Desiccant_Cutting\\");				break;
	case TEACH_TAB_IDX_DESICCANT_MATERIAL:			strJobNumber = GetRecipeRootPath() + _T("3G_Desiccant_Material\\");				break;
	case TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY: 	strJobNumber = GetRecipeRootPath() + _T("3G_Desiccant_Material_Tray\\");		break;

#elif RELEASE_4G
	case TEACH_TAB_IDX_MBB:							strJobNumber = GetRecipeRootPath() + _T("4G_MBB\\");							break;
	case TEACH_TAB_IDX_LABEL:						strJobNumber = GetRecipeRootPath() + _T("Label\\");								break;

#elif RELEASE_5G
	case TEACH_TAB_IDX_BOX:							strJobNumber = GetRecipeRootPath() + _T("5G_Box\\");							break;
	case TEACH_TAB_IDX_SEALING:						strJobNumber = GetRecipeRootPath() + _T("5G_Sealing\\");						break;

#elif RELEASE_6G
	case TEACH_TAB_IDX_LABEL:						strJobNumber = GetRecipeRootPath() + _T("Label\\");								break;
	case TEACH_TAB_IDX_TAPE:						strJobNumber = GetRecipeRootPath() + _T("6G_Tape\\");							break;
#endif

	default:
		break;
	}

	if (strJobNumber != _T(""))
	{
		CString strName = strJobNumber + _T("_Make_JobNumber.bat");
		ShellExecute(NULL, _T("open"), strName, NULL, NULL, SW_SHOW);
	}

	return TRUE;
}
void CModelInfo::Delete( LPCTSTR lpszModelName )
{
	CString strRecipePath = GetRecipePath(m_strModelName);

	DeleteDirectory( (LPCTSTR)strRecipePath );
	SetEmpty();
}

void CModelInfo::SetEmpty()
{
	m_strModelName = _T("");

	m_strModelNameTrayOcr = _T("");
	m_strModelName3DChipCnt = _T("");
	m_strModelNameChipOcr = _T("");
	m_strModelNameChip = _T("");
	m_strModelNameMixing = _T("");
	m_strModelNameLift = _T("");
	m_strModelNameStackerOcr = _T("");
	m_strModelNameBanding = _T("");
	m_strModelNameHIC = _T("");
	m_strModelNameMBB = _T("");
	m_strModelNameDesiccantCutting = _T("");
	m_strModelNameDesiccantMaterial = _T("");
	m_strModelNameDesiccantMaterialTray = _T("");
	m_strModelNameBoxQuality = _T("");
	m_strModelNameSealingQuality = _T("");
	m_strModelNameTape = _T("");
	m_strModelNameLabel = _T("");

	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME"), m_strModelName);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_TRAY_OCR"), m_strModelNameTrayOcr);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_3D_CHIP_CNT"), m_strModelName3DChipCnt);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIPOCR"), m_strModelNameChipOcr);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIP"), m_strModelNameChip);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_MIXING"), m_strModelNameMixing);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_LIFT"), m_strModelNameLift);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_STACKER_OCR"), m_strModelNameStackerOcr);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_BANDING"), m_strModelNameBanding);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_HIC"), m_strModelNameHIC);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_CUTTING"), m_strModelNameDesiccantCutting);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL"), m_strModelNameDesiccantMaterial);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL_TRAY"), m_strModelNameDesiccantMaterialTray);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_MBB"), m_strModelNameMBB);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_BOX_QUALITY"), m_strModelNameBoxQuality);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_SEALING_QUALITY"), m_strModelNameSealingQuality);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_TAPE"), m_strModelNameTape);
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME_LABEL"), m_strModelNameLabel);

	m_TrayOcr.Clear();
	m_3DChipCnt.Clear();
	m_ChipOcr.Clear();
	m_Chip.Clear();
	m_Mixing.Clear();
	m_LiftInfo.Clear();
	m_StackerOcr.Clear();
	m_BandingInfo.Clear();
	m_HICInfo.Clear();
	m_DesiccantCuttingInfo.Clear();
	m_DesiccantMaterialInfo.Clear();
	m_MBBInfo.Clear();
	m_BoxInfo.Clear();
	m_SealingInfo.Clear();
	m_TapeInfo.Clear();
	m_LabelInfo.Clear();

	for (int j = 0; j < OP_MAX; j++)
		m_LabelManualOptionInfo[j].Clear();
}

BOOL CModelInfo::IsValid()
{
	return m_strModelName.IsEmpty() ? FALSE : TRUE;
}

CString CModelInfo::GetRecipePath(LPCTSTR lpszModelName)
{
	CString strRecipePath;
	strRecipePath.Format(_T("%s%s\\"), (LPCTSTR)GetRecipeRootPath(), lpszModelName);
	return strRecipePath;
}

CString CModelInfo::GetRecipeRootPath()
{
	CString strRecipeRoot;
	strRecipeRoot = GetExecuteDirectory() + _T("\\Data\\Recipe\\");
	return strRecipeRoot;
}

CString CModelInfo::GetLastModelName()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameTrayOcr()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_TRAY_OCR"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelName3DChipCnt()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_3D_CHIP_CNT"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameChipOcr()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIPOCR"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameChip()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_CHIP"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameMixing()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_MIXING"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameLift()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_LIFT"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameStackerOcr()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_STACKER_OCR"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameBanding()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_BANDING"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameHIC()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_HIC"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameDesiccantCutting()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_CUTTING"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameDesiccantMaterial()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameDesiccantMaterialTray()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_DESICCANT_MATERIAL_TRAY"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameMBB()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_MBB"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameLabel()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_LABEL"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameTape()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_TAPE"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameBoxQuality()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_BOX_QUALITY"), _T(""));
	return strModelName;
}

CString CModelInfo::GetLastModelNameSealingQuality()
{
	CString strModelName;
	strModelName = AfxGetApp()->GetProfileString(_T("RECIPE"), _T("MODEL_NAME_SEALING_QUALITY"), _T(""));
	return strModelName;
}

BOOL CModelInfo::IsExistModelName( LPCTSTR lpszModelName )
{
	CString strRootPath = GetRecipeRootPath();
	TRACE(_T("%s%s%s"), strRootPath, lpszModelName, +_T("\\ModelInfo.ini"));
	if(IsExistFile((LPCTSTR)(strRootPath +lpszModelName +_T("\\ModelInfo.ini"))))
		return TRUE;
	return FALSE;
}

BOOL CModelInfo::SaveBypassMode( InspectType inspecttype /*=InspectTypeMax*/)
{
	CString strRecipePath = GetRecipePath(m_strModelName);
	MakeDirectory((LPCTSTR)strRecipePath);
	strRecipePath += _T("ModelInfo.ini");

	// 1G
	CString strTrayOcr						= GetRecipeRootPath() + _T("1G_TrayOcr\\") + m_strModelNameTrayOcr + _T("\\1G_TrayOcr.ini");
	CString str3DChipCntFile				= GetRecipeRootPath() + _T("1G_3DChipCnt\\") + m_strModelName3DChipCnt + _T("\\1G_3DChipCnt.ini");
	CString strChipOcrFile					= GetRecipeRootPath() + _T("1G_ChipOcr\\") + m_strModelNameChipOcr + _T("\\1G_ChipOcr.ini");
	CString strChipFile						= GetRecipeRootPath() + _T("1G_Chip\\") + m_strModelNameChip + _T("\\1G_Chip.ini");
	CString strMixingFile					= GetRecipeRootPath() + _T("1G_Mixing\\") + m_strModelNameMixing + _T("\\1G_Mixing.ini");
	CString strLiftInfoFile					= GetRecipeRootPath() + _T("1G_Lift\\") + m_strModelNameLift + _T("\\1G_Lift.ini");

	// 2G
	CString strBandingInfoFile				= GetRecipeRootPath() + _T("2G_Banding\\") + m_strModelNameBanding + _T("\\2G_Banding.ini");
	CString strHICInfoFile					= GetRecipeRootPath() + _T("2G_HIC\\") + m_strModelNameHIC + _T("\\2G_HIC.ini" );

	// 3G
	CString strDesiccantCuttingInfoFile		= GetRecipeRootPath() + _T("3G_Desiccant_Cutting\\") + m_strModelNameDesiccantCutting + _T("\\3G_Desiccant_Cutting.ini");
	CString strDesiccantMaterialInfoFile	= GetRecipeRootPath() + _T("3G_Desiccant_Material\\") + m_strModelNameDesiccantMaterial + _T("\\3G_Desiccant_Material.ini");

	// 4G
	CString strMBBInfoFile					= GetRecipeRootPath() + _T("4G_MBB\\") + m_strModelNameMBB + _T("\\4G_MBB.ini" );

	// 5G
	CString strBoxInfoFile					= GetRecipeRootPath() + _T("5G_Box\\") + m_strModelNameBoxQuality + _T("\\5G_Box.ini" );
	CString strSealingInfoFile				= GetRecipeRootPath() + _T("5G_Sealing\\") + m_strModelNameSealingQuality + _T("\\5G_Sealing.ini" );

	// 6G
	CString strTapeInfoFile					= GetRecipeRootPath() + _T("6G_Tape\\") + m_strModelNameTape + _T("\\6G_Tape.ini" );

	// Label
	CString strLabelInfoFile				= GetRecipeRootPath() + _T("Label\\") + m_strModelNameLabel + _T("\\LabelInfo.ini");

	/////
	CString strTemp_1, strTemp_2, strData;
	switch( inspecttype )
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:
		strTemp_1 = TRAYOCR_APPNAME;		strTemp_2 = _T("USE_TRAYOCR_BYPASS");						strData.Format(_T("%d"), m_TrayOcr.nUseBypass_TrayOcr);
		break;
	case InspectType3DChipCnt:
		strTemp_1 = TOPCHIPCNT_APPNAME;		strTemp_2 = _T("USE_3DCHIPCNT_BYPASS");						strData.Format(_T("%d"), m_3DChipCnt.nUseBypass_3DChipCnt);
		break;
	case InspectTypeChipOcr:
		strTemp_1 = CHIPOCR_APPNAME;		strTemp_2 = _T("USE_CHIPOCR_BYPASS");						strData.Format(_T("%d"), m_ChipOcr.nUseBypass_ChipOcr);
		break;
	case InspectTypeChip:
		strTemp_1 = CHIP_APPNAME;			strTemp_2 = _T("USE_CHIP_BYPASS");							strData.Format(_T("%d"), m_Chip.nUseBypass_Chip);
		break;
	case InspectTypeMixing:
		strTemp_1 = MIXING_APPNAME;			strTemp_2 = _T("USE_MIXING_BYPASS");						strData.Format(_T("%d"), m_Mixing.nUseBypass_Mixing);
		break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:
		strTemp_1 = LIFT_APPNAME;			strTemp_2 = _T("USE_LIFT_BYPASS");							strData.Format(_T("%d"), m_LiftInfo.nUseBypass_Lift);
		break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:
		strTemp_1 = _T("");					strTemp_2 = _T("");											strData = _T("");
		break;
		
#elif RELEASE_2G
	case InspectTypeBanding:
		strTemp_1 = BANDING_APPNAME;		strTemp_2 = _T("USE_BANDING_BYPASS");						strData.Format( _T("%d"), m_BandingInfo.nUseBypass_Banding );
		break;
	case InspectTypeHIC:
		strTemp_1 = HIC_APPNAME;			strTemp_2 = _T("USE_HIC_BYPASS");							strData.Format( _T("%d"), m_HICInfo.nUseBypass_HIC );
		break;

#elif RELEASE_3G
	case InspectTypePositionLeft:
		strTemp_1 = DESICCANT_APPNAME;		strTemp_2 = _T("USE_DESICCANT_POSTION_LEFT_BYPASS");		strData.Format(_T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft);
		break;
	case InspectTypePositionRight:
		strTemp_1 = DESICCANT_APPNAME;		strTemp_2 = _T("USE_DESICCANT_POSTION_RIGHT_BYPASS");		strData.Format(_T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight);
		break;
	case InspectTypeDesiccantLeft:
		strTemp_1 = DESICCANT_APPNAME;		strTemp_2 = _T("USE_DESICCANT_LEFT_BYPASS");				strData.Format( _T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft);
		break;
	case InspectTypeDesiccantRight:
		strTemp_1 = DESICCANT_APPNAME;		strTemp_2 = _T("USE_DESICCANT_RIGHT_BYPASS");				strData.Format( _T("%d"), m_DesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight);
		break;
	case InspectTypeMaterial:
		strTemp_1 = SUBMATERIAL_APPNAME;	strTemp_2 = _T("USE_MATERIAL_BYPASS");						strData.Format( _T("%d"), m_DesiccantMaterialInfo.nUseBypass_Material );
		break;

#elif RELEASE_4G
	case InspectTypeMBB:
		strTemp_1 = MBB_APPNAME;			strTemp_2 = _T("USE_MBB_BYPASS");							strData.Format( _T("%d"), m_MBBInfo.nUseBypass_MBB );
		break;
	case InspectTypeLabel:
		strTemp_1 = LABEL_APPNAME;			strTemp_2 = _T("USE_LABEL_BYPASS");							strData.Format( _T("%d"), m_LabelInfo.nUseBypass_Label );
		break;

#elif RELEASE_5G
	case InspectTypeBox:
		strTemp_1 = BOXQUALITY_APPNAME;		strTemp_2 = _T("USE_BOX_QUALITY_BYPASS");					strData.Format( _T("%d"), m_BoxInfo.nUseBypass_Box );
		break;
	case InspectTypeSealing:
		strTemp_1 = SEALINGQUALITY_APPNAME;	strTemp_2 = _T("USE_SEALING_QUALITY_BYPASS");				strData.Format( _T("%d"), m_SealingInfo.nUseBypass_Sealing );
		break;

#elif RELEASE_6G
	case InspectTypeLabel:
		strTemp_1 = LABEL_APPNAME;			strTemp_2 = _T("USE_LABEL_BYPASS");							strData.Format( _T("%d"), m_LabelInfo.nUseBypass_Label );
		break;
	case InspectTypeTape:
		strTemp_1 = BOXTAPE_APPNAME;		strTemp_2 = _T("USE_BOX_TAPE_BYPASS");						strData.Format( _T("%d"), m_TapeInfo.nUseBypass_Tape );
		break;

#endif
	default:
		break;
	}


	CString strModelInfoFile;
	switch( inspecttype )
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:			strModelInfoFile = strTrayOcr;						break;
	case InspectType3DChipCnt:			strModelInfoFile = str3DChipCntFile; 				break;
	case InspectTypeChipOcr:			strModelInfoFile = strChipOcrFile;					break;
	case InspectTypeChip:				strModelInfoFile = strChipFile;						break;
	case InspectTypeMixing:				strModelInfoFile = strMixingFile;					break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:			strModelInfoFile = strLiftInfoFile;					break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:			strModelInfoFile = _T("");							break;

#elif RELEASE_2G
	case InspectTypeBanding:			strModelInfoFile = strBandingInfoFile;				break;
	case InspectTypeHIC:				strModelInfoFile = strHICInfoFile;					break;

#elif RELEASE_3G
	case InspectTypePositionLeft:
	case InspectTypePositionRight:
	case InspectTypeDesiccantLeft:
	case InspectTypeDesiccantRight:		strModelInfoFile = strDesiccantCuttingInfoFile;		break;
	case InspectTypeMaterial:			strModelInfoFile = strDesiccantMaterialInfoFile;	break;

#elif RELEASE_4G
	case InspectTypeMBB:				strModelInfoFile = strMBBInfoFile;					break;
	case InspectTypeLabel:				strModelInfoFile = strLabelInfoFile;				break;

#elif RELEASE_5G
	case InspectTypeBox:				strModelInfoFile = strBoxInfoFile;					break;
	case InspectTypeSealing:			strModelInfoFile = strSealingInfoFile;				break;

#elif RELEASE_6G
	case InspectTypeLabel:				strModelInfoFile = strLabelInfoFile;				break;
	case InspectTypeTape:				strModelInfoFile = strTapeInfoFile;					break;

#endif
	default:
		break;
	}

	::WritePrivateProfileString(strTemp_1, strTemp_2, strData, strModelInfoFile );

	return TRUE;
}

CString CModelInfo::GetTrayOcrInfo(LPCTSTR lpszModelName)
{
	CString strModelInfoFile = lpszModelName;
	CString strReturn = _T("None Data");
	TCHAR szTemp[MAX_PATH];

#ifdef RELEASE_1G
	::GetPrivateProfileString(TRAYOCR_APPNAME, _T("TRAY_OCR_INFO"), _T(""), szTemp, MAX_PATH, strModelInfoFile);

#else
	::GetPrivateProfileString(STACKEROCR_APPNAME, _T("STACKER_OCR_INFO"), _T(""), szTemp, MAX_PATH, strModelInfoFile);

#endif
	strReturn = szTemp;

	return strReturn;
}
