#include "stdafx.h"
#include "InspectionVision.h"

#include <XImage/xImageObject.h>
#include <XGraphic/xGraphicObject.h>

#include "ModelInfo.h"
#include <XUtil/xUtils.h>
#include <XUtil/xStopWatch.h>

#include <algorithm>
#include <vector>
#include "APKView.h"
#include "VisionSystem.h"
#include "StringMacro.h"
#include "LanguageInfo.h"

#include "UIExt/ResourceManager.h"

#include "DecoderCortex.h"

//////////////////////////////////////////////////////////////////////////
#include "TeachTab1G3DChipCnt.h"
#include "TeachTab1GMixing.h"
#include "TeachTab3GDesiccantCutting.h"
#include "TeachTab3GDesiccantMaterial.h"
#include "InspTabObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionProcess;
using namespace Euresys::Open_eVision_2_11;

CInspectionVision::CInspectionVision(void)
{
	try
	{
		for( int i=0; i<IDX_AREA_MAX; ++i )
		{
			for ( int j=0; j<nMATCH_MAX; ++j )
			{
				m_pMatch[i][j] = new EMatcher;
			}
		}

		for( int j=0; j<EMATCH_TYPE_LABEL_MAX; ++j )
		{
			m_pLabelMatch[j] = new EMatcher;
		}

		m_pMatchChip = new EMatcher;
	}
	catch (EException& e)
	{
		e.what();
		for( int i=0; i<IDX_AREA_MAX; ++i )
		{
			for ( int j=0; j<nMATCH_MAX; ++j )
			{
				m_pMatch[i][j] = NULL;
			}
		}

		for( int j=0; j<EMATCH_TYPE_LABEL_MAX; ++j )
		{
			m_pLabelMatch[j] = NULL;
		}

		m_pMatchChip = NULL;
	}

	for( int i=0; i<IDX_AREA_MAX; ++i )
	{
		// OCR Registration 할 때 Calibration값 못받아서 생성자에 코드 추가, 250205 황대은J
		CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
		m_fCalX[i] = CalibrationConfig.fCaliMeasure[i][CalibrationDirX];
		m_fCalY[i] = CalibrationConfig.fCaliMeasure[i][CalibrationDirY];
#ifdef RELEASE_1G
		if (i >= IDX_AREA3)	// 3D Area 카메라 때문에 VIEWER2 이후로  VIEWER 번호랑 Camera 번호 Calibration값 mapping 하나씩 어긋남, 250115 황대은J
		{
			m_fCalX[i] = CalibrationConfig.fCaliMeasure[i-1][CalibrationDirX];
			m_fCalY[i] = CalibrationConfig.fCaliMeasure[i-1][CalibrationDirY];
		}
#endif
	}

	m_rcLabelArea.SetRectEmpty();
	m_rcTray.SetRectEmpty();
	m_nExistingChips = 0;

	m_rcLabelArea.SetRectEmpty();
	m_bLabelAlignTeaching = FALSE;
	m_ptAlign.SetPoint(0, 0);

	m_strMergePath = _T("");
}

CInspectionVision::~CInspectionVision(void)
{
	try
	{
		for( int i=0; i<IDX_AREA_MAX; ++i )
		{

			for ( int j=0; j<nMATCH_MAX; ++j )
			{
				if (m_pMatch[i][j])
					delete m_pMatch[i][j];

				m_pMatch[i][j] = NULL;
			}
		}

		for( int j=0; j<EMATCH_TYPE_LABEL_MAX; ++j )
		{
			if (m_pLabelMatch[j])
				delete m_pLabelMatch[j];

			m_pLabelMatch[j] = NULL;
		}

		if (m_pMatchChip)
			delete m_pMatchChip;

		m_pMatchChip = NULL;
	}
	catch (EException& e)
	{
		e.what();
		for( int i=0; i<IDX_AREA_MAX; ++i )
		{
			for ( int j=0; j<nMATCH_MAX; ++j )
			{
				m_pMatch[i][j];
			}
		}

		for( int j=0; j<EMATCH_TYPE_LABEL_MAX; ++j )
		{
			m_pLabelMatch[j];
		}

		m_pMatchChip;
	}
}

BOOL CInspectionVision::SetDisplayBypass( CxGraphicObject* pGO, CxImageObject* pImgObj )
{
	COLORTEXT clrText;
	clrText.CreateObject( PDC_GREEN, pImgObj->GetWidth()-DEF_FONT_RESULT_POSI, pImgObj->GetHeight()-DEF_FONT_RESULT_POSI, DEF_FONT_RESULT_SIZE, FALSE, CxGOText::TextAlignmentLeft );
	clrText.SetText( _T("Bypass") );
	pGO->AddDrawText( clrText );

	return TRUE;
}

BOOL CInspectionVision::_Inspection( CxGraphicObject* pGO, InspectType inspecttype, int nViewIndex, int nGrabCnt, BOOL bManual )
{
	CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("_Inspection[%d] - Start"), nViewIndex+1);

	if( !pGO ) 
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("_Inspection[%d] - pGO(NULL) NG"), nViewIndex+1);
		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );

		return FALSE; 
	}

	if( nViewIndex < 0 || nViewIndex >= IDX_AREA_MAX )
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("_Inspection - Invalid nViewIndex(%d)"), nViewIndex );
		return FALSE;
	}

	if( !bManual && CVisionSystem::Instance()->GetRunStatus() == RunStatusStop )
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("_Inspection[%d] - Stop state"), nViewIndex+1 );
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// Auto Calibration
	for (int i = 0; i < ACM_MAX; ++i)
	{
		if (CVisionSystem::Instance()->m_bAcm[i])
		{
			BOOL bAcmResult = FALSE;
			bAcmResult = _AutoCalibration(pGO, inspecttype, nViewIndex);

			if (CVisionSystem::Instance()->GetRunStatus() == RunStatusStop)
				CVisionSystem::Instance()->m_bAcm[i] = FALSE;

			return bAcmResult;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	CxImageObject *pSrcImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex );
#ifdef RELEASE_1G
	if( (!bManual) && (inspecttype == InspectTypeChip) )
		pSrcImgObj = CVisionSystem::Instance()->GetImageObject_Chip(nGrabCnt);
#endif

	if( pSrcImgObj == NULL || pSrcImgObj->GetImageBuffer() == NULL )	
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("_Inspection[%d] - pSrcImgObj(NULL) NG"), nViewIndex+1 );
		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );

		return FALSE;
	}

	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
	{
		COLORTEXT clrText;
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("EvisionDongle이 존재하지 않습니다."));
		else												clrText.SetText(_T("EvisionDongle does not exist."));
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, pSrcImgObj->GetHeight()/2, DEF_FONT_BASIC_SIZE, FALSE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
	m_fCalX[nViewIndex] = CalibrationConfig.fCaliMeasure[nViewIndex][CalibrationDirX];
	m_fCalY[nViewIndex] = CalibrationConfig.fCaliMeasure[nViewIndex][CalibrationDirY];
#ifdef RELEASE_1G
	if (nViewIndex >= IDX_AREA3)	// 3D Area 카메라 때문에 VIEWER2 이후로  VIEWER 번호랑 Camera 번호 Calibration값 mapping 하나씩 어긋남, 250115 황대은J
	{
		m_fCalX[nViewIndex] = CalibrationConfig.fCaliMeasure[nViewIndex-1][CalibrationDirX];
		m_fCalY[nViewIndex] = CalibrationConfig.fCaliMeasure[nViewIndex-1][CalibrationDirY];
	}
#endif

	if( (m_fCalX[nViewIndex]<=0.f) || (m_fCalY[nViewIndex]<=0.f) ) 
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("_Inspection[%d] - Calibration(NULL) NG"), nViewIndex+1 );
		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );

		return FALSE; 
	}

	// ========== Start Inspection ==========
	BOOL bResult = FALSE;
 	switch( inspecttype )
 	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:			bResult = _Insp_TrayOcr(pGO, pSrcImgObj, nViewIndex, inspecttype);					break;
	case InspectType3DChipCnt:			bResult = _Insp_3DChipCnt(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
	case InspectTypeChipOcr:			bResult = _Insp_ChipOcr(pGO, pSrcImgObj, nViewIndex, inspecttype);					break;
	case InspectTypeChip:				bResult = _Insp_Chip(pGO, pSrcImgObj, nViewIndex, inspecttype, nGrabCnt);			break;
	case InspectTypeMixing:				bResult = _Insp_Mixing(pGO, pSrcImgObj, nViewIndex, inspecttype, bManual);			break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:			bResult = _Insp_Lift(pGO, pSrcImgObj, nViewIndex, inspecttype);						break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:			bResult = _Insp_StackerOcr(pGO, pSrcImgObj, nViewIndex, inspecttype, bManual);		break;

#elif RELEASE_2G
	case InspectTypeBanding:			bResult = _Insp_Banding(pGO, pSrcImgObj, nViewIndex, inspecttype);					break;
	case InspectTypeHIC:				bResult = _Insp_HIC(pGO, pSrcImgObj, nViewIndex, inspecttype);						break;

#elif RELEASE_3G
	case InspectTypePositionLeft:		bResult = _Insp_Desiccant_Position(pGO, pSrcImgObj, nViewIndex, inspecttype);		break;
	case InspectTypePositionRight:		bResult = _Insp_Desiccant_Position(pGO, pSrcImgObj, nViewIndex, inspecttype);		break;
	case InspectTypeDesiccantLeft:		bResult = _Insp_Desiccant(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
	case InspectTypeDesiccantRight:		bResult = _Insp_Desiccant(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
	case InspectTypeMaterial:			bResult = _Insp_SubMaterial(pGO, pSrcImgObj, nViewIndex, inspecttype, bManual);		break;

#elif RELEASE_4G
	case InspectTypeMBB:				bResult = _Insp_MBB(pGO, pSrcImgObj, nViewIndex, inspecttype);						break;
	case InspectTypeLabel:				bResult = _Insp_Label(pGO, pSrcImgObj, nViewIndex, inspecttype);					break;

#elif RELEASE_5G
	case InspectTypeBox:				bResult = _Insp_BoxQuality(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
	case InspectTypeSealing:			bResult = _Insp_SealingQuality(pGO, pSrcImgObj, nViewIndex, inspecttype);			break;

#elif RELEASE_6G
	case InspectTypeLabel:				bResult = _Insp_Label(pGO, pSrcImgObj, nViewIndex, inspecttype);					break;
	case InspectTypeTape:				bResult = _Insp_Tape(pGO, pSrcImgObj, nViewIndex, inspecttype);						break;

#endif
		default:
			CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("_Inspection[%d] - Unsupported InspectType(%d)"), nViewIndex+1, (int)inspecttype );
			CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
			return FALSE;
 	}

	// ========== Check Bypass ==========
	CModelInfo::stTrayOcr& stTrayOcr										= CModelInfo::Instance()->GetTrayOcr();
	CModelInfo::st3DChipCnt& st3DChipCnt									= CModelInfo::Instance()->Get3DChipCnt();
	CModelInfo::stChipOcr& stChipOcr										= CModelInfo::Instance()->GetChipOcr();
	CModelInfo::stChip& stChip												= CModelInfo::Instance()->GetChip();
	CModelInfo::stMixing& stMixing											= CModelInfo::Instance()->GetMixing();
	CModelInfo::stLiftInfo& stLiftInfo										= CModelInfo::Instance()->GetLiftInfo();
	CModelInfo::stBandingInfo& stBandingInfo								= CModelInfo::Instance()->GetBandingInfo();
	CModelInfo::stHICInfo& stHICInfo										= CModelInfo::Instance()->GetHICInfo();
	CModelInfo::stDesiccantCuttingInfo& stDesiccantCuttingInfo				= CModelInfo::Instance()->GetDesiccantCuttingInfo();
	CModelInfo::stDesiccantMaterialInfo& stDesiccantMaterialInfo			= CModelInfo::Instance()->GetDesiccantMaterialInfo();
	CModelInfo::stMBBInfo& stMBBInfo										= CModelInfo::Instance()->GetMBBInfo();
	CModelInfo::stBoxInfo& stBoxInfo										= CModelInfo::Instance()->GetBoxInfo();
	CModelInfo::stSealingInfo& stSealingInfo								= CModelInfo::Instance()->GetSealingInfo();
	CModelInfo::stTapeInfo& stTapeInfo										= CModelInfo::Instance()->GetTapeInfo();
	CModelInfo::stLabelInfo& stLabelInfo									= CModelInfo::Instance()->GetLabelInfo();

	eBypass eBypassCheck = BYPASS_OFF;
	switch( inspecttype )
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:			eBypassCheck = (eBypass)stTrayOcr.nUseBypass_TrayOcr;									break;
	case InspectType3DChipCnt:			eBypassCheck = (eBypass)st3DChipCnt.nUseBypass_3DChipCnt;								break;
	case InspectTypeChipOcr:			eBypassCheck = (eBypass)stChipOcr.nUseBypass_ChipOcr;									break;
	case InspectTypeChip:				eBypassCheck = (eBypass)stChip.nUseBypass_Chip;											break;
	case InspectTypeMixing:				eBypassCheck = (eBypass)stMixing.nUseBypass_Mixing; 									break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:			eBypassCheck = (eBypass)stLiftInfo.nUseBypass_Lift;										break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:			eBypassCheck = (eBypass)BYPASS_ON;														break;

#elif RELEASE_2G
	case InspectTypeBanding:			eBypassCheck = (eBypass)stBandingInfo.nUseBypass_Banding;								break;
	case InspectTypeHIC:				eBypassCheck = (eBypass)stHICInfo.nUseBypass_HIC;										break;

#elif RELEASE_3G
	case InspectTypePositionLeft:		eBypassCheck = (eBypass)stDesiccantCuttingInfo.nUseBypass_Desiccant_PositionLeft;		break;
	case InspectTypePositionRight:		eBypassCheck = (eBypass)stDesiccantCuttingInfo.nUseBypass_Desiccant_PositionRight;		break;
	case InspectTypeDesiccantLeft:		eBypassCheck = (eBypass)stDesiccantCuttingInfo.nUseBypass_Desiccant_CuttingLeft;		break;
	case InspectTypeDesiccantRight:		eBypassCheck = (eBypass)stDesiccantCuttingInfo.nUseBypass_Desiccant_CuttingRight;		break;
	case InspectTypeMaterial:			eBypassCheck = (eBypass)stDesiccantMaterialInfo.nUseBypass_Material;					break;

#elif RELEASE_4G
	case InspectTypeMBB:				eBypassCheck = (eBypass)stMBBInfo.nUseBypass_MBB;										break;
	case InspectTypeLabel:				eBypassCheck = (eBypass)stLabelInfo.nUseBypass_Label;									break;

#elif RELEASE_5G
	case InspectTypeBox:				eBypassCheck = (eBypass)stBoxInfo.nUseBypass_Box;										break;
	case InspectTypeSealing:			eBypassCheck = (eBypass)stSealingInfo.nUseBypass_Sealing;								break;

#elif RELEASE_6G
	case InspectTypeLabel:				eBypassCheck = (eBypass)stLabelInfo.nUseBypass_Label;									break;
	case InspectTypeTape:				eBypassCheck = (eBypass)stTapeInfo.nUseBypass_Tape;										break;

#endif
	default:
		break;
	}

	if		(eBypassCheck == BYPASS_ON		)	CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("_Inspection[%d] - End[Bypass]"		), nViewIndex+1 );
	else if (eBypassCheck == BYPASS_ON_INSP	)	CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("_Inspection[%d] - End[Bypass-%s]"	), nViewIndex+1, bResult?_T("OK"):_T("NG") );
	else										CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("_Inspection[%d] - End[%s]"			), nViewIndex+1, bResult?_T("OK"):_T("NG") );

	return bResult;
}

BOOL CInspectionVision::_AutoCalibration(CxGraphicObject* pGO, InspectType inspecttype, int nViewIndex)
{
	CxImageObject *pSrcImgObj = CVisionSystem::Instance()->GetImageObject(CamTypeAreaScan, nViewIndex);

	if (pSrcImgObj == NULL || pSrcImgObj->GetImageBuffer() == NULL)
	{
		CVisionSystem::Instance()->WriteMessage(LogTypeError, _T("_Inspection[%d] - pSrcImgObj(NULL) NG"), nViewIndex + 1);
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);

		return FALSE;
	}

	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
	{
		COLORTEXT clrText;
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("EvisionDongle이 존재하지 않습니다."));
		else												clrText.SetText(_T("EvisionDongle does not exist."));
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, pSrcImgObj->GetHeight() / 2, DEF_FONT_BASIC_SIZE, FALSE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	BOOL bResult = TRUE;

	// ========== Start Auto Calibration ==========
#if defined(RELEASE_1G) || defined(RELEASE_3G) || defined(RELEASE_4G)
	switch (inspecttype)
	{
	#ifdef RELEASE_1G
		case InspectType3DChipCnt:		bResult = _Auto_3DChipCnt(pGO, pSrcImgObj, nViewIndex, inspecttype);		break;
		case InspectTypeChipOcr:		bResult = _Auto_Chip(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
		case InspectTypeLiftFront:
		case InspectTypeLiftRear:		bResult = _Auto_Lift(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
	
	#elif RELEASE_3G
		case InspectTypePositionLeft:	bResult = _Auto_Desiccant(pGO, pSrcImgObj, nViewIndex, inspecttype);		break;
		case InspectTypePositionRight:	bResult = _Auto_Desiccant(pGO, pSrcImgObj, nViewIndex, inspecttype);		break;
	
	#elif RELEASE_4G
		case InspectTypeMBB:			bResult = _Auto_MBB(pGO, pSrcImgObj, nViewIndex, inspecttype);				break;
	
	#endif
		default:
			break;
	}
#endif
	if (!bResult)
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
	}

	return bResult;
}

BOOL CInspectionVision::_Auto_3DChipCnt(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - %s"), __FUNCTIONW__);

	COLORTEXT clrText;
	CModelInfo::st3DChipCnt& st3DChipCnt = CModelInfo::Instance()->Get3DChipCnt();

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	
	WRITE_LOG(WL_MSG, _T("Out - %s"), __FUNCTIONW__);

	return bResult;
}

BOOL CInspectionVision::_Auto_Chip(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - %s"), __FUNCTIONW__);

	CModelInfo::stChipOcr& stChip = CModelInfo::Instance()->GetChipOcr();

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	CRect rcSrc, rcDst;
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = pSrcImgObj->GetWidth();
	rcSrc.bottom = pSrcImgObj->GetHeight();
	rcDst.SetRectEmpty();
	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	bResult = _Auto_BuildUsingEasyObject(pGO, pSrcImgObj, rcSrc, rcDst, OBJ_WHITE, FALSE, stChip.nThreshold, TRUE );

	if (bResult)
	{
		clrText.SetText(_T("Chip Width : %d px"), rcDst.Width());
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("Chip Height : %d px"), rcDst.Height());
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 6, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		float fCalX, fCalY;
		fCalX = 25.5f * 1000.f / rcDst.Width();
		fCalY = 25.5f * 1000.f / rcDst.Height();

		if (fCalX < 0.f || fCalY < 0.f)
		{
			clrText.SetText(_T("Calibration NG"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
		CalibrationConfig.fCaliMeasure[2][CalibrationDirX] = fCalX;
		CalibrationConfig.fCaliMeasure[2][CalibrationDirY] = fCalY;

		CSystemConfig::Instance()->SetCalibrationConfig(CalibrationConfig, 2);
		CSystemConfig::Instance()->Load();

		CVisionSystem::Instance()->PostUIMsgUpdateViewerResolution();

		clrText.SetText(_T("Chip Calibration X : %.4f ㎛"), fCalX);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 8, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("Chip Calibration Y : %.4f ㎛"), fCalY);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 10, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);
	}

	// 결과 Graphic
	clrText.SetText(bResult ? _T("Chip Calibration : OK") : _T("Chip Calibration : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - %s"), __FUNCTIONW__);

	return bResult;
}

BOOL CInspectionVision::_Auto_Lift(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - %s"), __FUNCTIONW__);

	CModelInfo::stLiftInfo& stLift = CModelInfo::Instance()->GetLiftInfo();

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	CRect rcSrc, rcDst, rcLift;
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = pSrcImgObj->GetWidth();
	rcSrc.bottom = pSrcImgObj->GetHeight();
	rcDst.SetRectEmpty();
	rcLift.SetRectEmpty();
	
	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	bResult = _Auto_BuildUsingEasyObject_Lift(pGO, pSrcImgObj, rcSrc, rcDst, OBJ_WHITE, FALSE, stLift.nTrayPixelThreshold, TRUE);

	// 각 Edge
	if(bResult)
		bResult = _Auto_FindLift(pGO, pSrcImgObj, rcDst, rcLift);

	if (bResult)
	{
		float fCalX, fCalY;
		fCalX = 315.f * 1000.f / rcLift.Width();
		fCalY = 12.5f * 1000.f / rcLift.Height();

		if (fCalX < 0.f || fCalY < 0.f)
		{
			clrText.SetText(_T("Calibration NG"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
		CalibrationConfig.fCaliMeasure[nViewIndex-1][CalibrationDirX] = fCalX;
		CalibrationConfig.fCaliMeasure[nViewIndex-1][CalibrationDirY] = fCalY;

		CSystemConfig::Instance()->SetCalibrationConfig(CalibrationConfig, nViewIndex-1);

		CSystemConfig::Instance()->Load();

		CVisionSystem::Instance()->PostUIMsgUpdateViewerResolution();

		clrText.SetText(_T("Lift Calibration X : %.4f ㎛"), fCalX);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 8, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("Lift Calibration Y : %.4f ㎛"), fCalY);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 10, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);
	}

	// 결과 Graphic
	clrText.SetText(bResult ? _T("Lift Calibration : OK") : _T("Lift Calibration : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - %s"), __FUNCTIONW__);

	return bResult;
}

BOOL CInspectionVision::_Auto_Desiccant(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - %s"), __FUNCTIONW__);

	CModelInfo::stDesiccantCuttingInfo& stDesiccant = CModelInfo::Instance()->GetDesiccantCuttingInfo();

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	CRect rcSrc, rcDst;
	if(nViewIndex == 0)			rcSrc = stDesiccant.rcPositionLeftInspArea;
	else						rcSrc = stDesiccant.rcPositionRightInspArea;
	rcSrc.NormalizeRect();
	rcDst.SetRectEmpty();
	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	bResult = _Auto_BuildUsingEasyObject(pGO, pSrcImgObj, rcSrc, rcDst, OBJ_WHITE, FALSE, stDesiccant.nColorLineThreshold, TRUE);

	if (rcDst.Width() < 600)
		bResult = FALSE;

	if (rcDst.Height() < 120)
		bResult = FALSE;

	if (bResult)
	{
		clrText.SetText(_T("Rect Width : %d px"), rcDst.Width());
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 3, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("Rect Height : %d px"), rcDst.Height());
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		float fCalX, fCalY;
		fCalX = 80.5f * 1000.f / rcDst.Width();
		fCalY = 20.5f * 1000.f / rcDst.Height();

		if (fCalX < 0.f || fCalY < 0.f)
		{
			clrText.SetText(_T("Calibration NG"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 5, DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
		CalibrationConfig.fCaliMeasure[nViewIndex][CalibrationDirX] = fCalX;
		CalibrationConfig.fCaliMeasure[nViewIndex][CalibrationDirY] = fCalY;
		CalibrationConfig.fCaliMeasure[nViewIndex+2][CalibrationDirX] = fCalX;
		CalibrationConfig.fCaliMeasure[nViewIndex+2][CalibrationDirY] = fCalY;

		CSystemConfig::Instance()->SetCalibrationConfig(CalibrationConfig, nViewIndex);
		CSystemConfig::Instance()->SetCalibrationConfig(CalibrationConfig, nViewIndex+2);
		CSystemConfig::Instance()->Load();

		CVisionSystem::Instance()->PostUIMsgUpdateViewerResolution();

		clrText.SetText(_T("Desiccant Calibration X : %.4f ㎛"), fCalX);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 5, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("Desiccant Calibration Y : %.4f ㎛"), fCalY);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 6, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);
	}

	// 결과 Graphic
	clrText.SetText(bResult ? _T("Desiccant Calibration : OK") : _T("Desiccant Calibration : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - %s"), __FUNCTIONW__);

	return bResult;
}

BOOL CInspectionVision::_Auto_MBB(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - %s"), __FUNCTIONW__);

	CModelInfo::stMBBInfo& stMBBInfo = CModelInfo::Instance()->GetMBBInfo();

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	//////////////////////////////////////////////////////////////////////////
	// 검사
	CRect rcFindRect;
	rcFindRect.SetRectEmpty();

	BOOL bResult = FALSE;
	bResult = _Auto_FindMBB(pGO, pSrcImgObj, nViewIndex, rcFindRect);

	if (bResult)
	{
		float fCalX, fCalY;
		fCalX = 315.f * 1000.f / rcFindRect.Width();
		fCalY = 136.f * 1000.f / rcFindRect.Height();

		if (fCalX < 0.f || fCalY < 0.f)
		{
			clrText.SetText(_T("Calibration NG"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
		CalibrationConfig.fCaliMeasure[0][CalibrationDirX] = fCalX;
		CalibrationConfig.fCaliMeasure[0][CalibrationDirY] = fCalY;

		CSystemConfig::Instance()->SetCalibrationConfig(CalibrationConfig, 0);
		CSystemConfig::Instance()->Load();

		CVisionSystem::Instance()->PostUIMsgUpdateViewerResolution();

		clrText.SetText(_T("MBB Calibration X : %.4f ㎛"), fCalX);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 5, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("MBB Calibration Y : %.4f ㎛"), fCalY);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 6, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);
	}

	// 결과 Graphic
	clrText.SetText(bResult ? _T("MBB Calibration : OK") : _T("MBB Calibration : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - %s"), __FUNCTIONW__);

	return bResult;
}

BOOL CInspectionVision::_Auto_FindLift(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, CRect rcRect, CRect& rcDst)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	COLORTEXT clrText;
	COLORBOX clrBox;
	COLORCROSS clrCross;
	COLORARROW clrArrow;

	CRect rcNormalRect;
	rcNormalRect = rcRect;

	float fTolerance = 150;
	float fLength = 30;

	float fAngle = 0.f;
	double dResultAngle[4];

	DPOINT dptLine = { 0.f, 0.f };
	DPOINT dptResult[4];
	BOOL bResult[4];
	BOOL bSizeResultX = FALSE;
	BOOL bSizeResultY = FALSE;

	// init
	for (int i = 0; i < 4; ++i)
	{
		bResult[i] = FALSE;
		dResultAngle[i] = 0.f;
		dptResult[i] = { 0.f, 0.f };
	}

	try
	{
		CxImageObject CopyImgObj;
		CopyImgObj.Clone(pSrcImgObj);

		EImageBW8 BW8OrgImage, BW8ImgDst;
		BW8OrgImage.SetImagePtr(CopyImgObj.GetWidth(), CopyImgObj.GetHeight(), CopyImgObj.GetImageBuffer(), CopyImgObj.GetWidthBytes() * 8);

		EROIBW8 BW8ROIImage;
		BW8ROIImage.Attach(&BW8OrgImage);
		BW8ROIImage.SetPlacement(rcNormalRect.left, rcNormalRect.top, rcNormalRect.Width(), rcNormalRect.Height());

		BW8ImgDst.SetSize(rcNormalRect.Width(), rcNormalRect.Height());

		// Image Processing
		EasyImage::Threshold(&BW8ROIImage, &BW8ImgDst, EThresholdMode_MinResidue);
		//		USES_CONVERSION;
		//		CString strImgName = _T("D:\\eTabROI.bmp");
		//		BW8ImgDst.Save(T2A((LPTSTR)(LPCTSTR)strImgName));

		DRECT drcArea;
		for (int i = 0; i < 4; ++i)
		{
			// Position
			switch (i)
			{
			case 0:
				dptLine = { static_cast<double>(BW8ImgDst.GetWidth() / 2), 0 };
				drcArea = { dptLine.x - fLength / 2, dptLine.y + fTolerance, dptLine.x + fLength / 2, dptLine.y - fTolerance };
				fAngle = 0.f;
				break;
			case 1:
				dptLine = { static_cast<double>(BW8ImgDst.GetWidth()), static_cast<double>(dptResult[0].y - rcNormalRect.top + 55) };
				drcArea = { dptLine.x - fTolerance, dptLine.y - fLength / 2, dptLine.x + fTolerance, dptLine.y + fLength / 2 };
				fAngle = 90.f;
				break;
			case 2:
				dptLine = { static_cast<double>(BW8ImgDst.GetWidth() / 2), static_cast<double>(BW8ImgDst.GetHeight()) };
				drcArea = { dptLine.x - fLength / 2, dptLine.y + fTolerance, dptLine.x + fLength / 2, dptLine.y - fTolerance };
				fAngle = 180.f;
				break;
			case 3:
				dptLine = { 0, static_cast<double>(dptResult[0].y - rcNormalRect.top + 55) };
				drcArea = { dptLine.x - fTolerance, dptLine.y - fLength / 2, dptLine.x + fTolerance, dptLine.y + fLength / 2 };
				fAngle = 270.f;
				break;
			}

			// Find Line
			bResult[i] = LineGauge2(BW8ImgDst, ETransitionType_Bw, ETransitionChoice_NthFromBegin, dptLine, fTolerance, fLength, fAngle,
				20, 1, 10, 0, 1, 1.5f, 1.f, FALSE, FALSE, TRUE, OUT dptResult[i], OUT dResultAngle[i]);

			// Area Offset
			drcArea.left += rcNormalRect.left;
			drcArea.right += rcNormalRect.left;
			drcArea.top += rcNormalRect.top;
			drcArea.bottom += rcNormalRect.top;
			clrBox.CreateObject(bResult[i] ? PDC_BLUE : PDC_RED, drcArea, PS_DASHDOT, 1);
			pGO->AddDrawBox(clrBox);

			if (!bResult[i])
				return FALSE;

			// Point Offset
			dptResult[i].x += rcNormalRect.left;
			dptResult[i].y += rcNormalRect.top;
			clrCross.CreateObject(PDC_BLUE, dptResult[i], 10, PS_SOLID, 3);
			pGO->AddDrawCross(clrCross);
		}

		// 길이 체크
		int nX, nY;
		nX = GetRoundShort(dptResult[1].x - dptResult[3].x);
		nY = GetRoundShort(dptResult[2].y - dptResult[0].y);

		// 결과
		clrText.SetText(_T("Lift Width : %d px"), nX);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("Lift Height : %d px"), nY);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 6, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrArrow.CreateObject(PDC_BLUE, dptResult[0], dptResult[2], CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		clrArrow.CreateObject(PDC_BLUE, dptResult[1], dptResult[3], CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		CopyImgObj.Destroy();

		rcDst.left		= GetRoundShort(dptResult[3].x);
		rcDst.top		= GetRoundShort(dptResult[0].y);
		rcDst.right		= GetRoundShort(dptResult[1].x);
		rcDst.bottom	= GetRoundShort(dptResult[2].y);
		rcDst.NormalizeRect();
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return TRUE;
}

BOOL CInspectionVision::_Auto_FindMBB(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, CRect& rcFindRect)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stMBBInfo& stMBBInfo = CModelInfo::Instance()->GetMBBInfo();

	COLORTEXT clrText;
	COLORBOX clrBox;
	COLORCROSS clrCross;
	COLORARROW clrArrow;

	CRect rcNormalRect;
	rcNormalRect = stMBBInfo.rcInspArea;
	clrBox.CreateObject(PDC_ORANGE, rcNormalRect, PS_SOLID, 1);
	pGO->AddDrawBox(clrBox);

	float fTolerance = stMBBInfo.fTolerance / 2;
	float fLength = stMBBInfo.fLength;

	float fAngle = 0.f;
	double dResultAngle[4];

	DPOINT dptLine = { 0.f, 0.f };
	DPOINT dptResult[4];
	BOOL bResult[4];
	BOOL bSizeResultX = FALSE;
	BOOL bSizeResultY = FALSE;

	// init
	for (int i = 0; i < 4; ++i)
	{
		bResult[i] = FALSE;
		dResultAngle[i] = 0.f;
		dptResult[i] = { 0.f, 0.f };
	}

	try
	{
		CxImageObject CopyImgObj;
		CopyImgObj.Clone(pSrcImgObj);

		EImageBW8 BW8OrgImage, BW8ImgDst;
		BW8OrgImage.SetImagePtr(CopyImgObj.GetWidth(), CopyImgObj.GetHeight(), CopyImgObj.GetImageBuffer(), CopyImgObj.GetWidthBytes() * 8);

		EROIBW8 BW8ROIImage;
		BW8ROIImage.Attach(&BW8OrgImage);
		BW8ROIImage.SetPlacement(rcNormalRect.left, rcNormalRect.top, rcNormalRect.Width(), rcNormalRect.Height());

		BW8ImgDst.SetSize(rcNormalRect.Width(), rcNormalRect.Height());

		// Image Processing
		EasyImage::Threshold(&BW8ROIImage, &BW8ImgDst, 50);
//		USES_CONVERSION;
//		CString strImgName = _T("D:\\eTabROI.bmp");
//		BW8ImgDst.Save(T2A((LPTSTR)(LPCTSTR)strImgName));

		DRECT drcArea;
		for (int i = 0; i < 4; ++i)
		{
			// Position
			switch (i)
			{
			case 0:
				dptLine = { static_cast<double>(rcNormalRect.left + fLength), static_cast<double>(fTolerance) }; // 임시
				drcArea = { dptLine.x - fLength / 2, dptLine.y + fTolerance, dptLine.x + fLength / 2, dptLine.y - fTolerance };
				fAngle = 0.f;
				break;
			case 1:
				dptLine = { static_cast<double>(rcNormalRect.Width() - fTolerance), static_cast<double>((rcNormalRect.Height() / 2)) };
				drcArea = { dptLine.x - fTolerance, dptLine.y - fLength / 2, dptLine.x + fTolerance, dptLine.y + fLength / 2 };
				fAngle = 90.f;
				break;
			case 2:
				dptLine = { static_cast<double>(rcNormalRect.left + fLength), static_cast<double>(rcNormalRect.Height() - fTolerance) }; // 임시
				drcArea = { dptLine.x - fLength / 2, dptLine.y + fTolerance, dptLine.x + fLength / 2, dptLine.y - fTolerance };
				fAngle = 180.f;
				break;
			case 3:
				dptLine = { static_cast<double>(fTolerance), static_cast<double>(rcNormalRect.Height() / 2) };
				drcArea = { dptLine.x - fTolerance, dptLine.y - fLength / 2, dptLine.x + fTolerance, dptLine.y + fLength / 2 };
				fAngle = 270.f;
				break;
			}

			// Find Line
			bResult[i] = LineGauge2(BW8ImgDst, ETransitionType_Wb, ETransitionChoice_NthFromBegin, dptLine, fTolerance, fLength, fAngle,
				20, 1, 10, 0, 1, 1.5f, 1.f, FALSE, FALSE, TRUE, OUT dptResult[i], OUT dResultAngle[i]);

			// Area Offset
			drcArea.left += rcNormalRect.left;
			drcArea.right += rcNormalRect.left;
			drcArea.top += rcNormalRect.top;
			drcArea.bottom += rcNormalRect.top;
			clrBox.CreateObject(bResult[i] ? PDC_BLUE : PDC_RED, drcArea, PS_DASHDOT, 1);
			pGO->AddDrawBox(clrBox);

			if (!bResult[i])
				return FALSE;

			// Point Offset
			dptResult[i].x += rcNormalRect.left;
			dptResult[i].y += rcNormalRect.top;
			clrCross.CreateObject(PDC_BLUE, dptResult[i], 10, PS_SOLID, 3);
			pGO->AddDrawCross(clrCross);
		}

		// 길이 체크
		int nX, nY;
		nX = GetRoundShort(dptResult[1].x - dptResult[3].x);
		nY = GetRoundShort(dptResult[2].y - dptResult[0].y);

		// 결과
		clrText.SetText(_T("MBB Width : %d px"), nX);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 8), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("MBB Height : %d px"), nY);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 12), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrArrow.CreateObject(PDC_BLUE, dptResult[0], dptResult[2], CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		clrArrow.CreateObject(PDC_BLUE, dptResult[1], dptResult[3], CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		rcFindRect.left		= GetRoundShort(dptResult[1].x);
		rcFindRect.right	= GetRoundShort(dptResult[3].x);
		rcFindRect.top		= GetRoundShort(dptResult[0].y);
		rcFindRect.bottom	= GetRoundShort(dptResult[2].y);
		rcFindRect.NormalizeRect();

		CopyImgObj.Destroy();
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return TRUE;
}

BOOL CInspectionVision::_Auto_BuildUsingEasyObject(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, CRect& rcBlobRect, BOOL nKindOfObj, BOOL bColor, int nThreshold/*=0*/, BOOL bDraw/*= TRUE*/)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	EImageBW8 ImageX;
	EImageC24 ImageX24;
	if (bColor)
	{
		ImageX24.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
		ImageX.SetSize(&ImageX24);
		EasyColor::GetComponent(&ImageX24, &ImageX, 0);
	}
	else
	{
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
	}

	rcRect.NormalizeRect();

	EROIBW8 ImageROI;
	ImageROI.Attach(&ImageX);
	ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

	EListItem* pObject;
	COLORBOX clrBox;

	int		nOffsetX, nOffsetY, nArea;
	float	fWidth, fHeight, fCenterX, fCenterY;

	nOffsetX = rcRect.left;
	nOffsetY = rcRect.top;
	fWidth = fHeight = fCenterX = fCenterY = 0.0f;
	nArea = 0;

	if (bDraw)
	{
		clrBox.CreateObject(PDC_ORANGE, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);
		pGO->AddDrawBox(clrBox);
	}

	ECodedImage CodedImage;

	if (nKindOfObj == OBJ_BLACK)
	{
		CodedImage.SetWhiteClass(0);
		CodedImage.SetBlackClass(1);
	}
	else if (nKindOfObj == OBJ_WHITE)
	{
		CodedImage.SetWhiteClass(1);
		CodedImage.SetBlackClass(0);
	}

	if (nThreshold == 0) nThreshold = EasyImage::AutoThreshold(&ImageROI, EThresholdMode_MinResidue, 0.5).Value;
	else if (nThreshold == 255) nThreshold = EasyImage::AutoThreshold(&ImageROI, EThresholdMode_MaxEntropy, 0.5).Value;

	CodedImage.SetThreshold(nThreshold);
	CodedImage.BuildObjects(&ImageROI);

	int nObjNum = CodedImage.GetNumObjects();
	if (nObjNum <= 0)
		return FALSE;

	CodedImage.AnalyseObjects(ELegacyFeature_Area, ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight, ELegacyFeature_LimitCenterX, ELegacyFeature_LimitCenterY);
	CodedImage.SelectObjectsUsingFeature(ELegacyFeature_LimitWidth, 0.0f, 5.5f, ESelectOption_RemoveOutOfRange);	// px
	CodedImage.SelectObjectsUsingFeature(ELegacyFeature_LimitHeight, 0.0f, 5.5f, ESelectOption_RemoveOutOfRange); // px
	CodedImage.SetConnexity(EConnexity_Connexity8);

	// 	nObjNum = CodedImage.GetNumSelectedObjects();
	// 	if( nObjNum <= 0)
	// 		return FALSE;

	int nMaxArea = 0;
	pObject = CodedImage.GetFirstObjPtr();
	for (int i = 0; i < nObjNum; ++i)
	{
		CodedImage.GetObjectFeature(ELegacyFeature_LimitWidth, pObject, fWidth);
		CodedImage.GetObjectFeature(ELegacyFeature_LimitHeight, pObject, fHeight);
		CodedImage.GetObjectFeature(ELegacyFeature_LimitCenterX, pObject, fCenterX);
		CodedImage.GetObjectFeature(ELegacyFeature_LimitCenterY, pObject, fCenterY);
		CodedImage.GetObjectFeature(ELegacyFeature_Area, pObject, nArea);

		if (nMaxArea < nArea)
		{
			nMaxArea = nArea;

			rcBlobRect.left = nOffsetX + (int)GetRoundLong(fCenterX - (fWidth / 2.f));
			rcBlobRect.top = nOffsetY + (int)GetRoundLong(fCenterY - (fHeight / 2.f));
			rcBlobRect.right = nOffsetX + (int)GetRoundLong(fCenterX + (fWidth / 2.f));
			rcBlobRect.bottom = nOffsetY + (int)GetRoundLong(fCenterY + (fHeight / 2.f));
			rcBlobRect.NormalizeRect();

		}

		pObject = CodedImage.GetNextObjPtr(pObject);
	}

	if (bDraw)
	{
		clrBox.CreateObject(PDC_GREEN, rcBlobRect, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);
	}

	return TRUE;
}

BOOL CInspectionVision::_Auto_BuildUsingEasyObject_Lift(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, CRect& rcBlobRect, BOOL nKindOfObj, BOOL bColor, int nThreshold/*=0*/, BOOL bDraw/*= TRUE*/)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	EImageBW8 ImageX;
	EImageC24 ImageX24;
	if (bColor)
	{
		ImageX24.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
		ImageX.SetSize(&ImageX24);
		EasyColor::GetComponent(&ImageX24, &ImageX, 0);
	}
	else
	{
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
	}

	rcRect.NormalizeRect();

	EROIBW8 ImageROI;
	ImageROI.Attach(&ImageX);
	ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

	EListItem* pObject;
	COLORBOX clrBox;

	int		nOffsetX, nOffsetY, nArea;
	float	fWidth, fHeight, fCenterX, fCenterY;

	nOffsetX = rcRect.left;
	nOffsetY = rcRect.top;
	fWidth = fHeight = fCenterX = fCenterY = 0.0f;
	nArea = 0;

	if (bDraw)
	{
		clrBox.CreateObject(PDC_ORANGE, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);
		pGO->AddDrawBox(clrBox);
	}

	ECodedImage CodedImage;

	if (nKindOfObj == OBJ_BLACK)
	{
		CodedImage.SetWhiteClass(0);
		CodedImage.SetBlackClass(1);
	}
	else if (nKindOfObj == OBJ_WHITE)
	{
		CodedImage.SetWhiteClass(1);
		CodedImage.SetBlackClass(0);
	}

	if (nThreshold == 0) nThreshold = EasyImage::AutoThreshold(&ImageROI, EThresholdMode_MinResidue, 0.5).Value;
	else if (nThreshold == 255) nThreshold = EasyImage::AutoThreshold(&ImageROI, EThresholdMode_MaxEntropy, 0.5).Value;

	CodedImage.SetThreshold(nThreshold);
	CodedImage.BuildObjects(&ImageROI);

	int nObjNum = CodedImage.GetNumObjects();
	if (nObjNum <= 0)
		return FALSE;

	CodedImage.AnalyseObjects(ELegacyFeature_Area, ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight, ELegacyFeature_LimitCenterX, ELegacyFeature_LimitCenterY);
	CodedImage.SelectObjectsUsingFeature(ELegacyFeature_LimitWidth, 0.0f, 5.5f, ESelectOption_RemoveOutOfRange);	// px
	CodedImage.SelectObjectsUsingFeature(ELegacyFeature_LimitHeight, 0.0f, 5.5f, ESelectOption_RemoveOutOfRange); // px
	CodedImage.SetConnexity(EConnexity_Connexity8);

	// 	nObjNum = CodedImage.GetNumSelectedObjects();
	// 	if( nObjNum <= 0)
	// 		return FALSE;

	int nMaxArea = 0;
	pObject = CodedImage.GetFirstObjPtr();
	for (int i = 0; i < nObjNum; ++i)
	{
		CodedImage.GetObjectFeature(ELegacyFeature_LimitWidth, pObject, fWidth);
		CodedImage.GetObjectFeature(ELegacyFeature_LimitHeight, pObject, fHeight);
		CodedImage.GetObjectFeature(ELegacyFeature_LimitCenterX, pObject, fCenterX);
		CodedImage.GetObjectFeature(ELegacyFeature_LimitCenterY, pObject, fCenterY);
		CodedImage.GetObjectFeature(ELegacyFeature_Area, pObject, nArea);

		if (nMaxArea < nArea)
		{
			nMaxArea = nArea;

			rcBlobRect.left = nOffsetX + (int)GetRoundLong(fCenterX - (fWidth / 2.f));
			rcBlobRect.top = nOffsetY + (int)GetRoundLong(fCenterY - (fHeight / 2.f));
			rcBlobRect.right = nOffsetX + (int)GetRoundLong(fCenterX + (fWidth / 2.f));
			rcBlobRect.bottom = nOffsetY + (int)GetRoundLong(fCenterY + (fHeight / 2.f));
			rcBlobRect.NormalizeRect();

		}

		pObject = CodedImage.GetNextObjPtr(pObject);
	}

	if (bDraw)
	{
		rcBlobRect.InflateRect(10, 10, 10, 10);

		clrBox.CreateObject(PDC_GREEN, rcBlobRect, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);
	}

	return TRUE;
}

BOOL CInspectionVision::_Insp_TrayOcr(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_TrayOcr()"));

	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();

	COLORTEXT clrText;

	if (stTrayOcr.nUseBypass_TrayOcr != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if (stTrayOcr.nUseBypass_TrayOcr != BYPASS_OFF)
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_TrayOcr() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResultOcr = TRUE; 

	VisionProcess::TrayOCRData& stTrayData = CVisionSystem::Instance()->GetTrayOCRData();

#ifdef RELEASE_1G
	if (!EVisionOCR_TrayOCR(pGO, pSrcImgObj, nViewIndex, stTrayData, stTrayOcr))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TRAYOCR);
		bResultOcr = FALSE;
	}

	clrText.SetText(bResultOcr ? _T("Tray OCR : OK") : _T("Tray OCR : NG"));
	clrText.CreateObject(bResultOcr ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);
#endif

	WRITE_LOG(WL_MSG, _T("Out - _Insp_TrayOcr(%s)"), (bResultOcr?_T("OK"):_T("NG")) );

	return bResultOcr;
}

BOOL CInspectionVision::_Insp_3DChipCnt(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_3DChipCnt()"));

	CModelInfo::st3DChipCnt& st3DChipCnt = CModelInfo::Instance()->Get3DChipCnt();

	if (st3DChipCnt.nUseBypass_3DChipCnt != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if (st3DChipCnt.nUseBypass_3DChipCnt != BYPASS_OFF)
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_3DChipCnt() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;

	double dRowPitch, dColumnPitch;
	if (st3DChipCnt.nSelectUnit == 0) // px
	{
		dRowPitch = st3DChipCnt.nRowPitchPx;
		dColumnPitch = st3DChipCnt.nColumnPitchPx;
	}
	else // mm
	{
		dRowPitch = (st3DChipCnt.dRowPitch * 1000.0) / m_fCalX[nViewIndex];
		dColumnPitch = (st3DChipCnt.dColumnPitch * 1000.0) / m_fCalY[nViewIndex];
	}

	int nTotalChip;
	nTotalChip = st3DChipCnt.nXCount * st3DChipCnt.nYCount;

#ifdef RELEASE_1G
	m_nExistingChips = 0;
	if (!BuildUsingEasyObject_TrayChipCnt(pGO, pSrcImgObj, st3DChipCnt.nMinThreshold, st3DChipCnt.nMaxThreshold, st3DChipCnt.rcChipPosFirst,  
											dRowPitch, dColumnPitch, st3DChipCnt.nXCount, st3DChipCnt.nYCount))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TRAYCHIPCNT);
		bResult = FALSE;
	}

	clrText.SetText(bResult ? _T("Tray Chip Count : OK") : _T("Tray Chip Count : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

#endif

	WRITE_LOG(WL_MSG, _T("Out - _Insp_3DChipCnt(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_ChipOcr(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_ChipOcr()"));

	CModelInfo::stChipOcr& stChipOcr = CModelInfo::Instance()->GetChipOcr();

	if (stChipOcr.nUseBypass_ChipOcr != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if (stChipOcr.nUseBypass_ChipOcr != BYPASS_OFF)
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_ChipOcr() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;

	VisionProcess::ChipOCRData stChipData = CVisionSystem::Instance()->GetChipOCRData();
	if (!EVisionOCR_ChipOCR(pGO, pSrcImgObj, stChipData, stChipOcr.rcInspArea, stChipOcr.strChipOcrInfo, nViewIndex))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_CHIPOCR);
		bResult = FALSE;
	}

	clrText.SetText(bResult ? _T("Chip OCR : OK") : _T("Chip OCR : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_ChipOcr(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Chip(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, int nGrabCnt)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Chip()"));

	CModelInfo::stChip& stChip = CModelInfo::Instance()->GetChip();

	if (stChip.nUseBypass_Chip != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if (stChip.nUseBypass_Chip != BYPASS_OFF)
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Chip() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	bResult = InspChipCount(pGO, pSrcImgObj, nViewIndex, nGrabCnt);

	// 결과 Graphic
	clrText.SetText(bResult ? _T("Chip Count : OK") : _T("Chip Count : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_TrayOcr(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Mixing(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, BOOL bManual)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Mixing()"));

	CModelInfo::stMixing& stMixingModel = CModelInfo::Instance()->GetMixing();

	if (stMixingModel.nUseBypass_Mixing != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if (stMixing.nUseBypass_Mixing != BYPASS_OFF)
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Mixing() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	// 검사
	BOOL bResult = FALSE;
#ifdef RELEASE_1G
	bResult = MatchModel_Mixing(pGO, pSrcImgObj, IDX_AREA5, stMixingModel, bManual);

	if (!bResult) CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_MIXING);
#endif

	// 결과 Graphic
	clrText.SetText(bResult ? _T("Mixing Search : OK") : _T("Mixing Search : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI*2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_Mixing(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Lift(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Lift()"));

	CModelInfo::stLiftInfo& stLiftInfo = CModelInfo::Instance()->GetLiftInfo();

	if (stLiftInfo.nUseBypass_Lift != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if (stLiftInfo.nUseBypass_Lift != BYPASS_OFF)
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText; 

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Lift() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;

#ifdef RELEASE_1G
	if (!BuildUsingEasyObject_Lift(pGO, pSrcImgObj, nViewIndex, stLiftInfo))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_LIFT);
		bResult = FALSE;
	}
#endif

	// 결과 Graphic
	clrText.SetText(bResult ? _T("Lift Inspect : OK") : _T("Lift Inspect : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_Lift(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}
BOOL CInspectionVision::StackerOcr_ImageCut(CxImageObject* pSrcImgObj)
{
	EImageBW8 eImageX;
	float fAvg = 0.f;
	eImageX.SetImagePtr(pSrcImgObj->GetWidth(), pSrcImgObj->GetHeight(), pSrcImgObj->GetImageBuffer(), pSrcImgObj->GetWidthBytes() * 8);
	EasyImage::PixelAverage(&eImageX, fAvg);

	if (fAvg < 1)
		return FALSE;

	CxImageObject pClone;
	pClone.Clone(pSrcImgObj);

	DPOINT dptCenter, dptResult;
	dptCenter.x = pClone.GetWidth() / 2;
	dptCenter.y = 640; // 고정일 듯
	dptResult.x = 0.f;
	dptResult.y = 0.f;

	BOOL bPoint = FALSE;
	bPoint = PointGauge(&pClone, ETransitionType_Bw, ETransitionChoice_NthFromEnd, dptCenter, 2500.f, 0.f, 20, 150, 10, dptResult);

	if (!bPoint)
	{
		dptCenter.y = 800; // 고정일 듯
		bPoint = PointGauge(&pClone, ETransitionType_Bw, ETransitionChoice_NthFromEnd, dptCenter, 2500.f, 180.f, 20, 150, 10, dptResult);
		if (!bPoint)
		{
			pClone.Destroy();
			return FALSE;
		}
	}

	pSrcImgObj->Destroy();

	int nEndX = GetRoundShort(dptResult.x);
	int nStartX = nEndX - CSystemConfig::Instance()->GetImageCut();

	if (dptCenter.y == 800) // rotate
	{
		nStartX = GetRoundShort(dptResult.x);
		nEndX = nStartX + CSystemConfig::Instance()->GetImageCut();
	}

	if (nStartX < 0)
		nStartX = 0;
	if (nEndX > pClone.GetWidth())
		nEndX = pClone.GetWidth();

	int nFinalWidth = nEndX - nStartX;   // 잘라낼 폭

	pSrcImgObj->Create(nFinalWidth, pClone.GetHeight(), 8, 1);

	BYTE* pSrc = (BYTE*)pClone.GetImageBuffer();
	BYTE* pDst = (BYTE*)pSrcImgObj->GetImageBuffer();

	int nSrcPitch = pClone.GetWidthBytes();
	int nDstPitch = pSrcImgObj->GetWidthBytes();

	for (int y = 0; y < pClone.GetHeight(); y++)
	{
		BYTE* pSrcLine = pSrc + nSrcPitch * y + (nStartX);
		BYTE* pDstLine = pDst + nDstPitch * y;

		memcpy(pDstLine, pSrcLine, (size_t)(nFinalWidth));
	}
	return TRUE;
}

BOOL CInspectionVision::_Insp_StackerOcr(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, BOOL bManual)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_TrayOcr()"));
	
	if( pSrcImgObj->GetWidth() != CSystemConfig::Instance()->GetImageCut() )
		StackerOcr_ImageCut(pSrcImgObj);

	CxImageObject pClone;
	pClone.Clone(pSrcImgObj);

	VisionProcess::TrayOCRData& stTrayData = CVisionSystem::Instance()->GetTrayOCRData();
	CModelInfo::stStackerOcr& stStacker = CModelInfo::Instance()->GetStackerOcr();

	BOOL bOcrResult = FALSE;

	if (bManual)
	{
		if(!EVisionOCR_StackerOCR(pGO, pSrcImgObj, nViewIndex, stTrayData, stStacker))
		{
			CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
			return bOcrResult;
		}
		else
		{
			bOcrResult = TRUE;
			//CString strRecipeName = CModelInfo::Instance()->GetModelNameStackerOcr();
			//CVisionSystem::Instance()->SetTraySID(strRecipeName);
			return bOcrResult;
		}
	}

	CString strRecipeRootPath = CModelInfo::GetRecipeRootPath() + _T("SG_StackerOcr\\");
	CString strFilter = strRecipeRootPath + _T("*.*");

	CFileFind ff;
	BOOL bFind = ff.FindFile(strFilter);
	CVisionSystem::Instance()->SetTraySID(_T(""));

	BOOL bRotate = FALSE;
	// Recipe 순회
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (ff.IsDots())
			continue;
		if (!ff.IsDirectory())
			continue;
		CString strRecipeName = ff.GetFileName();
		CString strStackerOcrFile = strRecipeRootPath + strRecipeName + _T("\\SG_StackerOcr.ini");
		
		if (IsExistFile((LPCTSTR)strStackerOcrFile))
		{
			CModelInfo::Instance()->Load(strRecipeName, STACKEROCR_KIND);
			CModelInfo::stStackerOcr& stStacker = CModelInfo::Instance()->GetStackerOcr();
			if (stStacker.nDepthOffset != 0)
				continue;

			Load(strRecipeName, STACKEROCR_KIND);

			if (stStacker.bRotateImage)
			{
				EImageBW8 eImageX, eImageDst;
				eImageX.SetImagePtr(pSrcImgObj->GetWidth(), pSrcImgObj->GetHeight(), pSrcImgObj->GetImageBuffer(), pSrcImgObj->GetWidthBytes() * 8);
				eImageDst.SetSize(eImageX.GetWidth(), eImageX.GetHeight());

				EasyImage::ScaleRotate(&eImageX,
					(float)eImageX.GetWidth() / 2, (float)eImageX.GetHeight() / 2,
					(float)eImageX.GetWidth() / 2, (float)eImageX.GetHeight() / 2,
					1.00f, 1.00f, 180.00f, &eImageDst, 0);

				EasyImage::Oper(EArithmeticLogicOperation_Copy, &eImageDst, &eImageX);
				bRotate = TRUE;
			}

			if (EVisionOCR_StackerOCR(pGO, pSrcImgObj, nViewIndex, stTrayData, stStacker)) // OCR 결과 OK이면 break
			{
				CVisionSystem::Instance()->SetTraySID(strRecipeName);
				bOcrResult = TRUE;
				break;
			}
			else
			{
				if (bRotate) // NG시, 원복
				{
					pSrcImgObj->Clone(&pClone);
//					pSrcImgObj->SaveToFile(_T("D:\\reset.bmp"));
				}

				pGO->Reset();
			}
		}
	}
	if (!bOcrResult)
	{
		bFind = ff.FindFile(strFilter);
		CVisionSystem::Instance()->SetTraySID(_T(""));
		while (bFind)
		{
			bFind = ff.FindNextFile();
			if (ff.IsDots())
				continue;
			if (!ff.IsDirectory())
				continue;
			CString strRecipeName = ff.GetFileName();
			CString strStackerOcrFile = strRecipeRootPath + strRecipeName + _T("\\SG_StackerOcr.ini");

			if (IsExistFile((LPCTSTR)strStackerOcrFile))
			{
				CModelInfo::Instance()->Load(strRecipeName, STACKEROCR_KIND);
				CModelInfo::stStackerOcr& stStacker = CModelInfo::Instance()->GetStackerOcr();
				if (stStacker.nDepthOffset == 0)
					continue;

				Load(strRecipeName, STACKEROCR_KIND);

				CVisionSystem::Instance()->SetDepthOffset_Size(pSrcImgObj->GetWidth(), pSrcImgObj->GetHeight());

				if (pSrcImgObj->GetWidth() != CSystemConfig::Instance()->GetImageCut())
					StackerOcr_ImageCut(pSrcImgObj);

				if (EVisionOCR_StackerOCR(pGO, pSrcImgObj, nViewIndex, stTrayData, stStacker)) // OCR 결과 OK이면 break
				{
					CVisionSystem::Instance()->SetTraySID(strRecipeName);
					bOcrResult = TRUE;
					break;
				}
				else
				{
					pGO->Reset();
				}
			}
		}
	}
	ff.Close();
	pClone.Destroy();

	WRITE_LOG(WL_MSG, _T("Out - _Insp_TrayOcr(%s)"), (bOcrResult ? _T("OK") : _T("NG")));

	if (!bOcrResult)
	{
		CVisionSystem::Instance()->SetDepthOffset(0);
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bOcrResult;
	}

	return bOcrResult;
}

BOOL CInspectionVision::_Insp_Banding(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Banding()"));

	CModelInfo::stBandingInfo& stBandingInfo = CModelInfo::Instance()->GetBandingInfo();

	if (stBandingInfo.nUseBypass_Banding != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
	//	if( stBandingInfo.nUseBypass_Banding != BYPASS_OFF )
	//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;
	COLORLINE clrLine;
	COLORARROW clrArrow;
	COLORBOX clrBox;

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Banding() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	CxImageObject pImgObj;
	pImgObj.Clone(pSrcImgObj);

	EImageBW8 eImageX;
	eImageX.SetImagePtr(pImgObj.GetWidth(), pImgObj.GetHeight(), pImgObj.GetImageBuffer(), pImgObj.GetWidthBytes() * 8);

	//	EasyImage::CloseBox(&eImageX, &eImageX, 10, 1);

		//////////////////////////////////////////////////////////////////////////
		// 검사
	BOOL bResult = TRUE;
	CString strText = _T("");
	CRect rcArea, rcBox;

	rcArea = stBandingInfo.rcInspArea;
	rcArea.NormalizeRect();

	double dAngleResult;
	DPOINT dptResultLeft, dptResultRight;
	DPOINT dptCenter, dptResult, dptStart, dptEnd;
	float fLength, fTolerance;

	for (int i = 0; i < 2; ++i)
	{
		// init
		dAngleResult = 0.f;
		dptResult.x = dptStart.x = dptEnd.x = 0;
		dptResult.y = dptStart.y = dptEnd.y = 0;
		fLength = GetRoundFloat(rcArea.Height());
		fTolerance = rcArea.Width() / 3 + .5f;

		// Insp Center Point
		if (i == 0)		dptCenter.x = rcArea.left + fTolerance; // Left
		else			dptCenter.x = rcArea.right - fTolerance; // Right

		dptCenter.y = rcArea.CenterPoint().y;

		// Insp Area
		rcBox.SetRectEmpty();
		rcBox.left = GetRoundShort(dptCenter.x - fTolerance);
		rcBox.right = GetRoundShort(dptCenter.x + fTolerance);
		rcBox.top = rcArea.top;
		rcBox.bottom = rcArea.bottom;
		rcBox.NormalizeRect();

		clrBox.CreateObject(PDC_ORANGE, rcBox, PS_DASHDOTDOT, 3);
		pGO->AddDrawBox(clrBox);

		// Insp & Result
		if (!LineGauge3(&pImgObj, (i == 0) ? ETransitionType_Wb : ETransitionType_Bw, ETransitionChoice_LargestAmplitude, dptCenter, fTolerance, fLength, 270.f,
			stBandingInfo.nThreshold, 100, 10, 0, 1, 1.5f, 1.f, FALSE, FALSE, TRUE, OUT dptResult, OUT dAngleResult, OUT dptStart, OUT dptEnd))
		{
			clrBox.CreateObject(PDC_RED, rcBox, PS_SOLID, 3);
			pGO->AddDrawBox(clrBox);

			if (CLanguageInfo::Instance()->m_nLangType == 0)			clrText.SetText(_T("밴딩 라인 감지 실패"));
			else														clrText.SetText(_T("Banding Line Detection Failure"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 2), DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		// Result Angle
		if (dAngleResult > 180.f)			dAngleResult -= 270;
		else if (dAngleResult < 180.f)		dAngleResult -= 90;

		if (abs(dAngleResult) > 5.f)
		{
			CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_BANDING);
			bResult = FALSE;

			clrLine.CreateObject(PDC_RED, dptStart, dptEnd, PS_SOLID, 5);
			pGO->AddDrawLine(clrLine);

			if (CLanguageInfo::Instance()->m_nLangType == 0)		clrText.SetText(_T("밴딩 각도 : NG [%2.lf˚ > 10˚]"), abs(dAngleResult));
			else													clrText.SetText(_T("Banding Angle  :`NG [%2.lf˚ > 10˚]"), abs(dAngleResult));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 3), DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			break;
		}
		else
		{
			clrLine.CreateObject(PDC_GREEN, dptStart, dptEnd, PS_SOLID, 4);
			pGO->AddDrawLine(clrLine);

			if (i == 0)					dptResultLeft = dptResult;
			else if (i == 1) 			dptResultRight = dptResult;
		}
	}

	clrArrow.CreateObject(PDC_GREEN, dptResultLeft, dptResultRight, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 15);
	pGO->AddDrawArrow(clrArrow);

	// Dst X
	double dX = dptResultRight.x - dptResultLeft.x;
	double dY = dptResultRight.y - dptResultLeft.y;
	double dDst = sqrt(dX * dX + dY * dY);

	// Spec
	int nSpec = stBandingInfo.nBandWidthSpec;
	int nRange = stBandingInfo.nBandWidthRange;

	if (dDst <= nSpec - nRange || dDst >= nSpec + nRange)
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_BANDING);
		bResult = FALSE;

		if (CLanguageInfo::Instance()->m_nLangType == 0)		clrText.SetText(_T("밴드 폭 : NG [%.lf] px"), dDst);
		else													clrText.SetText(_T("Banding Dst : NG [%.lf] px"), dDst);
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 4), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);
	}
	else
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("밴드 폭 : OK [%.lf]"), dDst);
		else												clrText.SetText(_T("Banding Dst : OK [%.lf]"), dDst);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 4), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);
	}
	
	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? strText += _T("밴드 찾기 : OK") : strText += _T("밴드 찾기 : NG"));
	else												clrText.SetText(bResult ? strText += _T("Banding Search : OK") : strText += _T("Banding Search : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	pImgObj.Destroy();

	WRITE_LOG(WL_MSG, _T("Out - _Insp_Banding(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_HIC( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_HIC()"));

	CModelInfo::stHICInfo& stHICInfo = CModelInfo::Instance()->GetHICInfo();

	if( stHICInfo.nUseBypass_HIC != BYPASS_OFF )
		SetDisplayBypass( pGO, pSrcImgObj );
//	if( stHICInfo.nUseBypass_HIC != BYPASS_OFF )
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;
	COLORBOX clrBox;

	//////////////////////////////////////////////////////////////////////////
	// 이미지 밝기 검사
	BOOL bError = FALSE;
	if (!ColorTipLuminanceCheck(pGO, pSrcImgObj))
		bError = TRUE;

	if(bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_HIC() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		CVisionSystem::Instance()->m_nHICNgKind = HIC_CIRCLE_FINDING_ERROR;

		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("색상 블록 밝기 : NG"));
		else												clrText.SetText(_T("Color Tip Luminance : NG"));
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		return !bError;
	}

	CRect rcRect = CRect(355, 280, 1120, 785);
	clrBox.CreateObject(PDC_ORANGE, rcRect);
	pGO->AddDrawBox(clrBox);

	//////////////////////////////////////////////////////////////////////////
	// HIC 종류 검사
	BOOL bKind = TRUE;
	if (!MatchModel_HICType(pGO, pSrcImgObj, nViewIndex, rcRect))
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_HIC() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		CVisionSystem::Instance()->m_nHICNgKind = HIC_CIRCLE_FINDING_ERROR;

		return !bKind;
	}

	//////////////////////////////////////////////////////////////////////////
	// HIC 검사
	BOOL bResult = TRUE;
	if (!BuildUsingEasyObject_HICDisColor(pGO, pSrcImgObj, rcRect, OBJ_BLACK, TRUE, 0))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_HIC);
		CVisionSystem::Instance()->m_nHICNgKind = HIC_CIRCLE_FINDING_ERROR;
		bResult = FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("HIC 찾기 : OK") : _T("HIC 찾기 : NG"));
	else												clrText.SetText(bResult ? _T("HIC Search : OK") : _T("HIC Search : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 4), DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_HIC - Search(%s)"), (bResult ? _T("OK") : _T("NG")));

	if (!bResult)
		return FALSE;

	if (!DisColorCheck(pGO, pSrcImgObj))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_HIC);
		CVisionSystem::Instance()->m_nHICNgKind = HIC_DISCOLOR_ERROR;
		bResult = FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("HIC 변색 : OK") : _T("HIC 변색 : NG"));
	else												clrText.SetText(bResult ? _T("HIC DisColorCheck : OK") : _T("HIC DisColorCheck : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 6), DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_HIC - DisColor(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Desiccant_Position(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype)
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Desiccant_Position()"));

	CModelInfo::stDesiccantCuttingInfo& stDesiccant = CModelInfo::Instance()->GetDesiccantCuttingInfo();

	if ((nViewIndex == IDX_AREA1) ? stDesiccant.nUseBypass_Desiccant_PositionLeft != BYPASS_OFF : stDesiccant.nUseBypass_Desiccant_PositionRight != BYPASS_OFF)
		SetDisplayBypass(pGO, pSrcImgObj);
//	if ((nViewIndex == IDX_AREA1) ? stDesiccant.nUseBypass_Desiccant_PositionLeft != BYPASS_OFF : stDesiccant.nUseBypass_Desiccant_PositionRight != BYPASS_OFF)
//		return FALSE;

	CxImageObject pGainImgObj;
	pGainImgObj.Clone(pSrcImgObj);

	COLORTEXT clrText;

	BOOL bError = FALSE;
	if (bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Desiccant_Position() - Error"));

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
#ifdef RELEASE_3G

	// 초기화, Preview
	BOOL bPreview = FALSE;
	if (nViewIndex == IDX_AREA1)
	{
		CVisionSystem::Instance()->m_fDesiPosDistLeft = 0.f;
		bPreview = CTeachTab3GDesiccantCutting::m_bPreviewLeft;
	}
	else
	{
		CVisionSystem::Instance()->m_fDesiPosDistRight = 0.f;
		bPreview = CTeachTab3GDesiccantCutting::m_bPreviewRight;
	}

	if (!BuildUsingEasyObject_DesiccantPosition(pGO, &pGainImgObj, nViewIndex, stDesiccant, bPreview))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, (inspecttype == InspectTypePositionLeft) ? ISRESULT_ERROR_DESICCANT1 : ISRESULT_ERROR_DESICCANT2);
		bResult &= FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("방습제 위치 : OK") : _T("방습제 위치: NG"));
	else												clrText.SetText(bResult ? _T("Desiccant Position : OK") : _T("Desiccant Position : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);
#endif

	pGainImgObj.Destroy();

	WRITE_LOG(WL_MSG, _T("Out - _Insp_Desiccant_Position(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Desiccant( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Desiccant()"));

	CModelInfo::stDesiccantCuttingInfo& stDesiccant = CModelInfo::Instance()->GetDesiccantCuttingInfo();

	BOOL bResult = TRUE;
#ifdef RELEASE_3G
	if( (nViewIndex == IDX_AREA3) ? stDesiccant.nUseBypass_Desiccant_CuttingLeft != BYPASS_OFF : stDesiccant.nUseBypass_Desiccant_CuttingRight != BYPASS_OFF )
		SetDisplayBypass( pGO, pSrcImgObj );
//	if( (nViewIndex == IDX_AREA1) ? stDesiccant.nUseBypass_Desiccant_CuttingLeft != BYPASS_OFF : stDesiccant.nUseBypass_Desiccant_CuttingRight != BYPASS_OFF )
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if(bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Desiccant() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		return !bError;
	}

	int nColorLineLengthDifference	= static_cast<int>(stDesiccant.dColorLineLengthDifference * 1000.0 / m_fCalY[nViewIndex]);
	int nSealingLength				= static_cast<int>(stDesiccant.dSealingLength * 1000.0 / m_fCalY[nViewIndex]);

	//////////////////////////////////////////////////////////////////////////
	// 검사
	if (inspecttype == InspectTypeDesiccantLeft)
	{
		if (!BuildUsingEasyObject_DesiccantCutting(pGO, pSrcImgObj, nViewIndex, stDesiccant.rcLeftInspArea, stDesiccant.nDesiccantCuttingPixelValue, nSealingLength, nColorLineLengthDifference))
		{
			CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_DESICCANT1);
			bResult = FALSE;
		}
	}
	else
	{
		if (!BuildUsingEasyObject_DesiccantCutting(pGO, pSrcImgObj, nViewIndex, stDesiccant.rcRightInspArea, stDesiccant.nDesiccantCuttingPixelValue, nSealingLength, nColorLineLengthDifference))
		{
			CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_DESICCANT2);
			bResult = FALSE;
		}
	}
	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("방습제 절단 : OK") : _T("Desiccant 절단 : NG"));
	else												clrText.SetText(bResult ? _T("Desiccant Cutting : OK") : _T("Desiccant Cutting : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);
#endif

	WRITE_LOG(WL_MSG, _T("Out - _Insp_Desiccant(%s)"), (bResult ? _T("OK") : _T("NG")));
	return bResult;
}

BOOL CInspectionVision::_Insp_SubMaterial( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, BOOL bManual )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_SubMaterial()"));

	CModelInfo::stDesiccantMaterialInfo& stDesiccant = CModelInfo::Instance()->GetDesiccantMaterialInfo();
	CModelInfo::stDesiccantMaterialTrayInfo& stDesiccantTray = CModelInfo::Instance()->GetDesiccantMaterialTrayInfo();

	if( stDesiccant.nUseBypass_Material != BYPASS_OFF )
		SetDisplayBypass( pGO, pSrcImgObj );
//	if( stDesiccant.nUseBypass_Material == BYPASS_ON )
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if(bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_SubMaterial() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		return !bError;
	}

	BOOL bTrayResult = TRUE;
	BOOL bHICRet = TRUE;
	BOOL bDesiccantRet = TRUE;
	BOOL bSubMaterialRet = TRUE;

#ifdef RELEASE_3G

	if (!CalculateTrayRegion(pGO, stDesiccantTray, m_rcTray, bManual))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_MATERIAL);
		bTrayResult = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	int nDesiccantQuantity = stDesiccant.nDesiccantQuantity;
	int nTrayOutTolerance = static_cast<int>((stDesiccant.dbOutTolerance * 1000.0) / m_fCalY[nViewIndex]);
	std::vector<CPoint*> vecPt;

	switch (nDesiccantQuantity)
	{
	case 1:
		vecPt.push_back(&stDesiccant.ptDesiccant1Shift);
		break;
	case 2:
		vecPt.push_back(&stDesiccant.ptDesiccant1Shift);
		vecPt.push_back(&stDesiccant.ptDesiccant2Shift);
		break;
	default:
		break;
	}

	std::sort(vecPt.begin(), vecPt.end(), CompareX);


	if (!MatchModel_Desiccant(pGO, pSrcImgObj, nViewIndex, EMATCH_TYPE_HIC_DESICCANT, nDesiccantQuantity, stDesiccant.ptPatternShiftTolerance, vecPt, stDesiccant.fMatchScoreDesiccant))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_MATERIAL);
		bDesiccantRet = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	if( !MatchModel_HIC( pGO, pSrcImgObj, nViewIndex, EMATCH_TYPE_HIC, FALSE, stDesiccant.ptPatternShiftTolerance, stDesiccant.ptHicShift, stDesiccant.fMatchScoreHIC))
	{
		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_MATERIAL);
		bHICRet = FALSE;
	}

	if (!BuildUsingEasyObject_SubMaterial(pGO, pSrcImgObj, m_rcTray, stDesiccant.nInspectionZoneCol, stDesiccant.nSubMaterialPixelVal,
											stDesiccant.nRatio1, stDesiccant.nRatio2, stDesiccant.nRatio3, stDesiccant.nInspectionZoneMargin, nTrayOutTolerance))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_MATERIAL);
		bSubMaterialRet = FALSE;
	}

#endif

	WRITE_LOG(WL_MSG, _T("Out - _Insp_SubMaterial - Desi(%s) HIC(%s) Sub(%s)"), (bDesiccantRet ? _T("OK") : _T("NG")), (bHICRet ? _T("OK") : _T("NG")), (bSubMaterialRet ? _T("OK") : _T("NG")));

	return bDesiccantRet && bHICRet && bSubMaterialRet;
}

BOOL CInspectionVision::_Insp_MBB( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_MBB()"));

	CModelInfo::stMBBInfo& stMBBInfo = CModelInfo::Instance()->GetMBBInfo();

	if( stMBBInfo.nUseBypass_MBB != BYPASS_OFF )
		SetDisplayBypass( pGO, pSrcImgObj );
//	if( stMBBInfo.nUseBypass_MBB != BYPASS_OFF )
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if(bError) 
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_MBB() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = FALSE;
	bResult = FindMBB(pGO, pSrcImgObj, nViewIndex);

	if (bResult)
		bResult = MatchModel_MBB(pGO, pSrcImgObj, nViewIndex);

	if (!bResult)
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_MBB);

	// 결과 Graphic
	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("MBB 찾기 : OK") : _T("MBB 찾기 : NG"));
	else												clrText.SetText(bResult ? _T("MBB Search : OK") : _T("MBB Search : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_MBB(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_BoxQuality( CxGraphicObject* pGO, CxImageObject* pImgObj, int nViewIndex, InspectType inspecttype )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_BoxQuality()"));

	CModelInfo::stBoxInfo& stBoxInfo = CModelInfo::Instance()->GetBoxInfo();

	if( stBoxInfo.nUseBypass_Box != BYPASS_OFF )
		SetDisplayBypass( pGO, pImgObj );
//	if( stBoxInfo.nUseBypass_Box != BYPASS_OFF )
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if(bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_BoxQuality() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	if (!MatchModel_BoxQuality(pGO, pImgObj, nViewIndex)) //  , stBoxInfo.fOkScore / 100))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_NG);
		bResult = FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("박스 인쇄 품질 : OK") : _T("박스 인쇄 품질 : NG"));
	else												clrText.SetText(bResult ? _T("Box Quality : OK") : _T("Box Quality : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_BoxQuality(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_SealingQuality( CxGraphicObject* pGO, CxImageObject* pImgObj, int nViewIndex, InspectType inspecttype )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_SealingQuality()"));

	CModelInfo::stSealingInfo& stSealingInfo = CModelInfo::Instance()->GetSealingInfo();

	if( stSealingInfo.nUseBypass_Sealing != BYPASS_OFF )
		SetDisplayBypass( pGO, pImgObj );
	//	if( stSealingInfo.nUseBypass_Sealing != BYPASS_OFF )
	//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if(bError)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_SealingQuality() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;
	if (!CheckHistogramData(pGO, pImgObj, nViewIndex))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_SEALING_QUALITY);
		bResult = FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("실링 품질 : OK") : _T("실링 품질 : NG"));
	else												clrText.SetText(bResult ? _T("Sealing Quality : OK") : _T("Sealing Quality : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_SealingQuality(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Tape( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype )
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Tape()"));

	CModelInfo::stTapeInfo& stTapeInfo = CModelInfo::Instance()->GetTapeInfo();

	if( stTapeInfo.nUseBypass_Tape != BYPASS_OFF )
		SetDisplayBypass( pGO, pSrcImgObj );
//	if( stTapeInfo.nUseBypass_Tape != BYPASS_OFF )
//		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	COLORTEXT clrText;

	BOOL bError = FALSE;
	if(bError) 
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Tape() - Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_NG );
		return !bError;
	}

	//////////////////////////////////////////////////////////////////////////
	// 검사
	BOOL bResult = TRUE;

	int nTapeWidth = stTapeInfo.nTapeWidth;
	int nTapeHeight = stTapeInfo.nTapeHeight;
	
	CRect rcBlobRect;
	rcBlobRect.SetRectEmpty();
	if (!BuildUsingEasyObject_BoxTape(pGO, pSrcImgObj, rcBlobRect, stTapeInfo.nRedThreshold, stTapeInfo.nGreenThreshold, stTapeInfo.nBlueThreshold, nTapeWidth, nTapeHeight))
	{
		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TAPE);
		bResult = FALSE;
	}
	else
	{
		rcBlobRect.top = 635;
		rcBlobRect.bottom = rcBlobRect.top + 100;
		rcBlobRect.NormalizeRect();

		// 하단면 부착 검사.
		if (!BuildUsingEasyObject_BoxTapeBtm(pGO, pSrcImgObj, rcBlobRect))
		{
			CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TAPE);
			bResult = FALSE;
		}
	}

	// 결과 Graphic
	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(bResult ? _T("테이프 : OK") : _T("테이프 : NG"));
	else												clrText.SetText(bResult ? _T("Tape : OK") : _T("Tape : NG"));
	clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_MSG, _T("Out - _Insp_Tape(%s)"), (bResult ? _T("OK") : _T("NG")));

	return bResult;
}

BOOL CInspectionVision::_Insp_Label( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype ) 
{
	WRITE_LOG(WL_MSG, _T("In - _Insp_Label()"));

	CModelInfo::stLabelInfo& stLabelInfo = CModelInfo::Instance()->GetLabelInfo();

	if( stLabelInfo.nUseBypass_Label != BYPASS_OFF )
		SetDisplayBypass( pGO, pSrcImgObj );
	if( stLabelInfo.nUseBypass_Label == BYPASS_ON )
		return FALSE;

	COLORTEXT clrText;
	//////////////////////////////////////////////////////////////////////////
	// ----- Label Search Inspection -----
	CRect rcLabelRect;
	m_rcLabelArea.SetRectEmpty();
	if( !LabelInspect( pGO, *pSrcImgObj, nViewIndex, rcLabelRect ) )
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - LabelInspect Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_LABEL_SHIFT);
		return FALSE;
	}
	m_rcLabelArea = rcLabelRect;

	m_ptAlign.SetPoint(0, 0);
	if (!MatchModel_ForLabelAlign(pGO, pSrcImgObj, nViewIndex))
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - MatchModel_ForLabelAlign Error"));

		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("기준 마크를 찾지 못했습니다."));
		else												clrText.SetText(_T("Fiducial Mark Not Found."));
		clrText.CreateObject(PDC_RED, 50, 100, 45, TRUE);
		pGO->AddDrawText(clrText);

		CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_LABEL_SHIFT);
		return FALSE;
	}

	if (CVisionSystem::Instance()->m_bLabelROITeaching)
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - m_bLabelROITeaching() Error"));

		CVisionSystem::Instance()->m_bLabelROITeaching = FALSE;
		return TRUE;
	}

	CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("__________Insp_Label_Masking_Start"));

	// ----- Masking Inspection -----
	CxImageObject pMaskingImgObj;
	pMaskingImgObj.Clone( pSrcImgObj );

	BOOL bInspRet = TRUE;
	for( int nMaskingNum = 0; nMaskingNum < stLabelInfo.nLabelMaskingCount; nMaskingNum++ )
	{
		WRITE_LOG( WL_MSG, _T("Inspection Masking Parameter Num : %d"), stLabelInfo.nLabelMaskingCount );

		if( MatchModel_ForLabel( pGO, &pMaskingImgObj, nViewIndex, nMaskingNum ) )
			MaskingArea( pGO, pMaskingImgObj, m_rcMaskArea_ForLabel[nMaskingNum] );	
		else bInspRet = FALSE;
	}

	if( !bInspRet ) // Masking Result
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - Masking Result Error"));
		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_LABEL_FAULTY);
#ifdef _DEBUG
		pMaskingImgObj.SaveToFile( _T("D:\\LABEL_MASKING.bmp") );
#endif
		pMaskingImgObj.Destroy();

		return FALSE;
	}

	CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("__________Insp_Label_Masking_End"));

	// ----- Inspection Option Check -----
	BOOL bSegmentImageSave = TRUE;
	if( CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun ) bSegmentImageSave = FALSE;

	BOOL bManualInsp_UseZPLData = FALSE;
	if( CVisionSystem::Instance()->GetRunStatus() == RunStatusStop && stLabelInfo.bLabelManualInsp ) bManualInsp_UseZPLData = TRUE;

	// ----- Inspection Data Check -----
	std::vector<READ_DATA_Label> LabelReciveData;
	LabelReciveData.clear();		
	if(CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun)	LabelReciveData = CVisionSystem::Instance()->GetLabelData();
	else if(bManualInsp_UseZPLData)										LabelReciveData = CVisionSystem::Instance()->GetLabelData_Manual();

	if( CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun || bManualInsp_UseZPLData ) // Use ZPL Data
	{
		BOOL bDataCheck;
		if(!LabelReciveData.empty())	bDataCheck = CreateLabelData(LabelReciveData);
		else							bDataCheck = FALSE;

		if(bDataCheck)
		{
			clrText.CreateObject( PDC_BLUE, 700, 50, 40, TRUE );
			clrText.SetText		( _T("ZPL Data : OK") );
			pGO->AddDrawText	( clrText );
		}
		else
		{
			clrText.CreateObject( PDC_LIGHTRED, 700, 50, 40, TRUE );
			clrText.SetText		( _T("ZPL Data : Empty") );
			pGO->AddDrawText	( clrText );

			if ( CVisionSystem::Instance()->GetRunStatus() == RunStatusAutoRun )
			{
				WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - ISRESULT_ERROR_LABEL_FAULTY Error"));

				CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_LABEL_FAULTY);
			}
			pMaskingImgObj.Destroy();
			return FALSE;
		}
	}
	else // No Data_Manual Inspection
	{
		COLORTEXT clrText;
		clrText.CreateObject( PDC_BLUE, 700, 50, 40, TRUE );
		clrText.SetText		( _T("ZPL Data : OK(None)") );
		pGO->AddDrawText	( clrText );
	}

	// ----- BarCode Inspection -----
	BOOL bCodeReadingRet = FALSE, bCheckLotID = FALSE;
	if( !ReadCode( pGO, pMaskingImgObj, LabelReciveData, bCodeReadingRet, bCheckLotID, inspecttype ) )
	{
		if(!bCodeReadingRet) CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_BARCODE_EMPTY);
		if(!bCheckLotID)	 CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_LOTID_MISMATCH);

		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - ReadCode() code[%s] ID[%s]"), (bCodeReadingRet ? _T("OK") : _T("NG")), (bCheckLotID ? _T("OK") : _T("NG")));

#ifdef _DEBUG
		pMaskingImgObj.SaveToFile( _T("D:\\LABEL_BARCODE.bmp") );
#endif	
		pMaskingImgObj.Destroy();

		return FALSE;
	}

#ifdef _DEBUG
	pMaskingImgObj.SaveToFile(_T("D:\\LABEL_BARCODE_OK.bmp"));
#endif	

	if( FALSE ) // stLabelInfo.bLabelFontInspPass )   // OCR Pass 기능 추가 시.
	{
		clrText.CreateObject( PDC_BLUE, 700, 300, 35, TRUE );		
		clrText.SetText		( _T("Insp OCR Pass : ON") );
		pGO->AddDrawText	( clrText );

		pMaskingImgObj.Destroy();
		return TRUE;
	}

	// ----- OCR Inspection -----
	if( !InspeOCR( pGO, pMaskingImgObj, rcLabelRect, LabelReciveData, bSegmentImageSave ) )
	{
		WRITE_LOG(WL_MSG, _T("Ing - _Insp_Label() - InspeOCR() Error"));

		CVisionSystem::Instance()->SetErrorCode( nViewIndex, ISRESULT_ERROR_OCR_NG);
#ifdef _DEBUG
		//pMaskingImgObj.SaveToFile( _T("D:\\LABEL_OCR.bmp") );
#endif	
		pMaskingImgObj.Destroy();

		return FALSE;
	}

	pMaskingImgObj.Destroy();
	
	WRITE_LOG(WL_MSG, _T("Out - _Insp_Label()"));

	return TRUE;
}


BOOL CInspectionVision::LabelInspect( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, int nViewIndex, CRect& rcLabelRect )
{
	COLORTEXT	clrText;
	COLORDCROSS clrDCross;
	COLORLINE	clrLine;
	COLORBOX	clrBox;

	CRect reNormalArea, reLabelArea;
	reNormalArea.left	= 0;
	reNormalArea.top	= 0;
	reNormalArea.right	= pMaskingImgObj.GetWidth();
	reNormalArea.bottom = pMaskingImgObj.GetHeight();

	//////////////////////////////////////////////////////////////////////////
	BOOL	bResult	= TRUE;
	CString strErrorCode;

	int nLabel_AreaThreshold	= 200; // 140;
	int nLabel_AreaOpenValue	=  10;
	int nLabel_LineThreshold	=  20;
	int nLabel_EdgeThreshold	= 200;

	float fEdgeMargin = CModelInfo::Instance()->GetLabelInfo().fLabelEdgeOffset;
	int nEdgeMargin_X = (int)( fEdgeMargin / ( CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[nViewIndex][CalibrationDirX] /1000.f) );
	int nEdgeMargin_Y = (int)( fEdgeMargin / ( CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[nViewIndex][CalibrationDirY] /1000.f) );

	try
	{
		CxImageObject LabelAreaImgObj;
		LabelAreaImgObj.Clone( &pMaskingImgObj );

		EImageBW8 EBW8ImgLabelArea;
		EBW8ImgLabelArea.SetImagePtr(LabelAreaImgObj.GetWidth(), LabelAreaImgObj.GetHeight(), (BYTE*)LabelAreaImgObj.GetImageBuffer(), LabelAreaImgObj.GetWidthBytes()*8);

		//////////////////////////////////////////////////////////////////////////
		EasyImage::OpenBox(&EBW8ImgLabelArea, &EBW8ImgLabelArea, nLabel_AreaOpenValue);

		if (CLanguageInfo::Instance()->m_nLangType == 0)		strErrorCode = _T("라벨 영역 찾기(1) : NG");
		else													strErrorCode = _T("Find Label Area(1) : NG");
		if( !BuildUsingEasyObject( pGO, &LabelAreaImgObj, reNormalArea, reLabelArea, OBJ_WHITE, FALSE, nLabel_AreaThreshold ) ) throw strErrorCode;

		// 240401 HJ : Angle 보정을 해도 정확히 0도가 아닐 때가 있어서 Overkill 발생 함
		reLabelArea.DeflateRect( 3, 3, 3, 3 );

		//////////////////////////////////////////////////////////////////////////
		DPOINT	dptCenter, dptResult;
		double	dLabelAngle;
		dptCenter.x = dptCenter.y = dptResult.x = dptResult.y = dLabelAngle = 0;

#ifdef RELEASE_6G
		dptCenter.x = reLabelArea.CenterPoint().x;
		dptCenter.y = reLabelArea.bottom;
		
		LineGauge( &pMaskingImgObj, ETransitionType_Bw, ETransitionChoice_NthFromEnd, dptCenter, 60, (float)reLabelArea.Width() * 0.7f,
			180.0f, nLabel_LineThreshold, 1, 10, 0, 1, 1.5f, 1.f, FALSE, FALSE, FALSE, OUT dptResult, OUT dLabelAngle );

		if ( abs(dLabelAngle) > 100 ) // 231228 HJ : 각도 버그 있어서 필터링
			dLabelAngle = abs(dLabelAngle) - (double)180;

#elif RELEASE_4G
		dptCenter.x = reLabelArea.CenterPoint().x;
		dptCenter.y = reLabelArea.top;

		LineGauge( &pMaskingImgObj, ETransitionType_Bw, ETransitionChoice_NthFromBegin, dptCenter, 60.f, (float)reLabelArea.Width() * 0.7f,
			0.0f, nLabel_LineThreshold, 1, 10, 0, 1, 1.5f, 1.f, FALSE, FALSE, FALSE, OUT dptResult, OUT dLabelAngle );
#endif

		//////////////////////////////////////////////////////////////////////////
		float fAngleSpec = CModelInfo::Instance()->GetLabelInfo().fLabelAngleSpec;
		if( fAngleSpec > abs(dLabelAngle) )
		{
			if( abs(dLabelAngle) > (double) 0.05f )
			{
				pGO->Reset();

				EImageBW8 EBW8Image_Rotate, EBW8Image_Buff;
				EBW8Image_Rotate.SetImagePtr(pMaskingImgObj.GetWidth(), pMaskingImgObj.GetHeight(), (BYTE*)pMaskingImgObj.GetImageBuffer(), pMaskingImgObj.GetWidthBytes()*8);
				EBW8Image_Buff.SetSize(EBW8Image_Rotate.GetWidth(), EBW8Image_Rotate.GetHeight());
				EasyImage::Oper(EArithmeticLogicOperation_Copy, &EBW8Image_Rotate, &EBW8Image_Buff);

				float fCenterX = (float)pMaskingImgObj.GetWidth()  / 2.0f;
				float fCenterY = (float)pMaskingImgObj.GetHeight() / 2.0f;
				EasyImage::ScaleRotate( &EBW8Image_Buff, fCenterX, fCenterY, fCenterX, fCenterY, 1.000f, 1.000f, (float)dLabelAngle, &EBW8Image_Rotate, 8 );


				//////////////////////////////////////////////////////////////////////////
				LabelAreaImgObj.Clone( &pMaskingImgObj );
#ifdef RELEASE_6G
				for(int nWidthIndex = 1155; nWidthIndex < 1300; nWidthIndex++)
					for(int nHeightIndex = 570; nHeightIndex < 720; nHeightIndex++)
						EBW8ImgLabelArea.SetPixel( 0, nWidthIndex, nHeightIndex );
#elif RELEASE_4G
#endif
				EasyImage::OpenBox(&EBW8ImgLabelArea, &EBW8ImgLabelArea, nLabel_AreaOpenValue);

				if (CLanguageInfo::Instance()->m_nLangType == 0)		strErrorCode = _T("라벨 영역 찾기(2) : NG");
				else													strErrorCode = _T("Find Label Area(2) : NG");
				if( !BuildUsingEasyObject( pGO, &LabelAreaImgObj, reNormalArea, reLabelArea, OBJ_WHITE, FALSE, nLabel_AreaThreshold ) ) throw strErrorCode;
			}
			else
			{
				DPOINT dpResult_Left;
				dpResult_Left.x = dptCenter.x - (double)reLabelArea.Width() * 0.35f;
				dpResult_Left.y = tan(dLabelAngle)*(dpResult_Left.x - dptResult.x) + dptResult.y;

				DPOINT dpResult_Right;
				dpResult_Right.x = dptCenter.x + (double)reLabelArea.Width() * 0.35f;
				dpResult_Right.y = tan(dLabelAngle)*(dpResult_Right.x - dptResult.x) + dptResult.y;

				if( !(170.f < dLabelAngle || dLabelAngle < 190.f) )
				{
					clrText.SetText( _T("%.2f"), dLabelAngle );
					clrText.CreateObject( PDC_YELLOW, (int)dptResult.x, (int)dptResult.y, 15 );
					pGO->AddDrawText( clrText );

					clrDCross.CreateObject(PDC_RED, dpResult_Left,	10, PS_DASH, 2);
					pGO->AddDrawDCross( clrDCross );

					clrDCross.CreateObject(PDC_GREEN, dptResult,	10, PS_DASH, 2);
					pGO->AddDrawDCross( clrDCross );

					clrDCross.CreateObject(PDC_BLUE, dpResult_Right, 10, PS_DASH, 2);
					pGO->AddDrawDCross( clrDCross ); 

					clrLine.CreateObject(PDC_RED, dpResult_Left, dpResult_Right, PS_DASH, 2);
					pGO->AddDrawLine( clrLine );
				}
			}
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)		strErrorCode.Format(_T("라벨 각도 NG (3): Spec[%.1f], Result[%.1f]"), fAngleSpec, dLabelAngle);
			else													strErrorCode.Format(_T("Label Angle NG (3): Spec[%.1f], Result[%.1f]"), fAngleSpec, dLabelAngle);
			throw strErrorCode;
		}

		//////////////////////////////////////////////////////////////////////////
		// ----- Inspection Label Edge -----
		LabelAreaImgObj.Clone( &pMaskingImgObj );
		CRect reAreaBuff = reLabelArea;
		reAreaBuff.DeflateRect( nEdgeMargin_X, nEdgeMargin_Y, nEdgeMargin_X, nEdgeMargin_Y );
		reAreaBuff.NormalizeRect();
		MaskingArea( pGO, LabelAreaImgObj, reAreaBuff );

		if( reLabelArea.left < 0 || reLabelArea.top < 0 || EBW8ImgLabelArea.GetWidth() < reLabelArea.right || EBW8ImgLabelArea.GetHeight() < reLabelArea.bottom )
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("마스킹 영역 에러"));
			else												clrText.SetText( _T("Masking Area Error") );
			clrText.CreateObject( PDC_YELLOW, (int)reLabelArea.CenterPoint().x, (int)reLabelArea.CenterPoint().y, 15, TRUE, CxGOText::TextAlignmentCenter );
			pGO->AddDrawText( clrText );
			return FALSE;
		}
		else
		{
			LONG lHeight = reLabelArea.bottom - reLabelArea.top;
//#ifdef RELEASE_4G
//			if ( lHeight > 1290 ) // Label 용지는 항상 같을테니?
//				reLabelArea.top = reLabelArea.bottom - 1280;
//#else
//			if ( lHeight > 1300 ) // 230703 HJ : Al Label Size가 달라짐.. WD 변경 된 것 같은데...
//				reLabelArea.top = reLabelArea.bottom - 1290;
//#endif
		}

		//////////////////////////////////////////////////////////////////////////
		// 230703 HJ : Masking 수정 및 분리
		// 좌측 구석
		for(int nWidthIndex = reLabelArea.left; nWidthIndex < reLabelArea.left+80; nWidthIndex++)
		{
			for(int nHeightIndex = reLabelArea.top; nHeightIndex < reLabelArea.bottom; nHeightIndex++)
			{
				if( EBW8ImgLabelArea.GetPixel( nWidthIndex, nHeightIndex ).Value == 255 ) break;
				else EBW8ImgLabelArea.SetPixel( 255, nWidthIndex, nHeightIndex );
			}

			for(int nHeightIndex = reLabelArea.bottom - 1; nHeightIndex >= reLabelArea.top; nHeightIndex--)
			{
				if( EBW8ImgLabelArea.GetPixel( nWidthIndex, nHeightIndex ).Value == 255 ) break;
				else EBW8ImgLabelArea.SetPixel( 255, nWidthIndex, nHeightIndex );
			}
		}

		// 우측 구석
		for(int nWidthIndex = reLabelArea.right-80; nWidthIndex < reLabelArea.right; nWidthIndex++)
		{
			for(int nHeightIndex = reLabelArea.top; nHeightIndex < reLabelArea.bottom; nHeightIndex++)
			{
				if( EBW8ImgLabelArea.GetPixel( nWidthIndex, nHeightIndex ).Value == 255 ) break;
				else EBW8ImgLabelArea.SetPixel( 255, nWidthIndex, nHeightIndex );
			}

			for(int nHeightIndex = reLabelArea.bottom - 1; nHeightIndex >= reLabelArea.top; nHeightIndex--)
			{
				if( EBW8ImgLabelArea.GetPixel( nWidthIndex, nHeightIndex ).Value == 255 ) break;
				else EBW8ImgLabelArea.SetPixel( 255, nWidthIndex, nHeightIndex );
			}
		}

		// 위 아래 면
		int nCnt = 0;
		for( int nHeightIndex = reLabelArea.top; nHeightIndex < reLabelArea.bottom; nHeightIndex++, nCnt++ )
		{
			for( int nWidthIndex = reLabelArea.left; nWidthIndex < reLabelArea.right; nWidthIndex++ )
			{
				EBW8ImgLabelArea.SetPixel( 255, nWidthIndex, nHeightIndex );
			}
			if ( nCnt == 5 )
				nHeightIndex = reLabelArea.bottom-5;
		}

		// 좌우 면
		nCnt = 0;
		for( int nWidthIndex = reLabelArea.left; nWidthIndex < reLabelArea.right; nWidthIndex++, nCnt++ )
		{
			for( int nHeightIndex = reLabelArea.top; nHeightIndex < reLabelArea.bottom; nHeightIndex++ )
			{
				EBW8ImgLabelArea.SetPixel( 255, nWidthIndex, nHeightIndex );
			}
			if ( nCnt == 5 )
				nWidthIndex = reLabelArea.right-5;
		}
		//////////////////////////////////////////////////////////////////////////

		CRect reBuff = reLabelArea;
		reBuff.DeflateRect( 10,10,10,10 );
		clrBox.CreateObject( PDC_LIGHTGREEN, reBuff, PS_SOLID, 1 );
		pGO->AddDrawBox( clrBox );

		// TEST 영상에는 속지가 있어서.
	//	strErrorCode = _T("Label Print Edge Line : NG");
	//	if( !BuildUsingEasyObject_ForLabelEdge( pGO, &LabelAreaImgObj, reBuff, OBJ_BLACK, nLabel_EdgeThreshold ) ) 
	//	{
	//		clrBox.CreateObject( PDC_LIGHTRED, reAreaBuff, PS_DASH, 3 );
	//		pGO->AddDrawBox( clrBox );
	//
	//		throw strErrorCode;
	//	}

	}
	catch ( CString strErrorCode )
	{
		clrText.SetText		( strErrorCode );
		clrText.CreateObject( PDC_LIGHTRED, 50, 50, 45, TRUE );
		pGO->AddDrawText	( clrText );

		bResult = FALSE;
	}


	///////////////////////////////////////////////////////////////////////// /
	if(bResult)
	{
		nEdgeMargin_X--; nEdgeMargin_Y--;
		reLabelArea.DeflateRect( nEdgeMargin_X, nEdgeMargin_Y, nEdgeMargin_X, nEdgeMargin_Y );
		rcLabelRect = reLabelArea;

		clrBox.CreateObject( PDC_LIGHTGREEN, rcLabelRect, PS_DASH, 3 );
		pGO->AddDrawBox( clrBox );

		clrText.SetText( _T("Label Search : OK") );
		clrText.CreateObject( PDC_BLUE, 50, 50, 45, TRUE );
		pGO->AddDrawText( clrText );
	}

	return bResult;
}

BOOL CInspectionVision::Save( LPCTSTR lpszRecipeName, eTeachTabIndex TabIndex )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	CString strSavePatch;

	BOOL	bLabelSave;

	switch (TabIndex)
	{
#ifdef RELEASE_1G
	case TEACH_TAB_IDX_TRAYOCR:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_TrayOcr\\") + lpszRecipeName + _T("\\");
		break;
	case TEACH_TAB_IDX_3DCHIPCNT:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_3DChipCnt\\") + lpszRecipeName + _T("\\");
		break;
	case TEACH_TAB_IDX_CHIPOCR:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_ChipOcr\\") + lpszRecipeName + _T("\\");
		break;
	case TEACH_TAB_IDX_CHIP:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_Chip\\") + lpszRecipeName + _T("\\");
		break;
	case TEACH_TAB_IDX_MIXING:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_Mixing\\") + lpszRecipeName + _T("\\");
		break;
	case TEACH_TAB_IDX_LIFTINFO:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("1G_Lift\\") + lpszRecipeName + _T("\\");
		break;
		
#elif RELEASE_SG
	case TEACH_TAB_IDX_STACKER_OCR:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("SG_StackerOcr\\") + lpszRecipeName + _T("\\");
		break;

#elif RELEASE_2G
	case TEACH_TAB_IDX_BANDING:
		bLabelSave	= FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("2G_Banding\\") + lpszRecipeName + _T("\\");
		break;
	case TEACH_TAB_IDX_HIC:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("2G_HIC\\") + lpszRecipeName + _T("\\");
		break;


#elif RELEASE_3G
	case TEACH_TAB_IDX_DESICCANT_MATERIAL:
		bLabelSave	= FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("3G_Desiccant_Material\\") + lpszRecipeName + _T("\\");
		break;
	
	case TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("3G_Desiccant_Material_Tray\\") + lpszRecipeName + _T("\\");
		break;

#elif RELEASE_4G
	case TEACH_TAB_IDX_MBB:
		bLabelSave	= FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("4G_MBB\\") + lpszRecipeName + _T("\\");
		break;

	case TEACH_TAB_IDX_LABEL:
		bLabelSave	= TRUE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("Label\\") + lpszRecipeName + _T("\\");
		strSavePatch += _T("LabelInfo\\");
		break;

#elif RELEASE_5G
	case TEACH_TAB_IDX_BOX:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("5G_Box\\") + lpszRecipeName + _T("\\");
		break;

	case TEACH_TAB_IDX_SEALING:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("5G_Sealing\\") + lpszRecipeName + _T("\\");
		break;

#elif RELEASE_6G
	case TEACH_TAB_IDX_LABEL:
		bLabelSave = TRUE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("Label\\") + lpszRecipeName + _T("\\");
		strSavePatch += _T("LabelInfo\\");
		break;

	case TEACH_TAB_IDX_TAPE:
		bLabelSave = FALSE;
		strSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("6G_Tape\\") + lpszRecipeName + _T("\\");
		break;

#endif

	default:
		return FALSE;
		break;
	}

	MakeDirectory( (LPCTSTR)strSavePatch );

	USES_CONVERSION;
	CString strMatchFileName, strMatchBmpFileName;
 	if(bLabelSave)
 	{
		for( int nMaskingIndex = 0; nMaskingIndex < EMATCH_TYPE_LABEL_MAX; nMaskingIndex++ )
		{
			strMatchFileName.	Format(_T("%sLabel_Mask%d.mch"), strSavePatch, nMaskingIndex);
			strMatchBmpFileName.Format(_T("%sLabel_Mask%d.bmp"), strSavePatch, nMaskingIndex);

			try
			{
				if (!m_pLabelMatch[nMaskingIndex])
					break;

				if (m_pLabelMatch[nMaskingIndex]->GetPatternLearnt())
				{
					m_pLabelMatch[nMaskingIndex]->Save(T2A((LPTSTR)(LPCTSTR)strMatchFileName));

					EImageBW8 pImg;
					m_pLabelMatch[nMaskingIndex]->CopyLearntPattern(pImg);

					pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
				}
			}
			catch (EException& e)
			{
				std::string error = e.what();
				error += "\n";
				OutputDebugStringA( error.c_str() );
				break;
			}
		}

		if (m_bLabelAlignTeaching)
		{
			strMatchFileName.Format(_T("%sAlign_%d.mch"), strSavePatch, 0);
			strMatchBmpFileName.Format(_T("%sAlign_%d.bmp"), strSavePatch, 0);

			int nViewIndex = IDX_AREA1;
#ifdef RELEASE_4G
			nViewIndex = IDX_AREA2;
#endif
			try
			{
				if (!m_pMatch[nViewIndex][0])
					return FALSE;

				if (m_pMatch[nViewIndex][0]->GetPatternLearnt())
				{
					m_pMatch[nViewIndex][0]->Save(T2A((LPTSTR)(LPCTSTR)strMatchFileName));

					EImageBW8 pImg;
					m_pMatch[nViewIndex][0]->CopyLearntPattern(pImg);

					pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
				}
			}
			catch (EException& e)
			{
				std::string error = e.what();
				error += "\n";
				OutputDebugStringA(error.c_str());
				return FALSE;
			}

			m_bLabelAlignTeaching = FALSE;
		}

 		return TRUE;
 	}
 	else
	{
		for( int nViewIndex = IDX_AREA1; nViewIndex < IDX_AREA_MAX; nViewIndex++ )
		{
			for (int nMatchIndex = 0; nMatchIndex < nMATCH_MAX; nMatchIndex++)
			{
				CString strInspName;
				switch (nViewIndex)
				{
#ifdef RELEASE_1G
				case IDX_AREA1:		strInspName = _T("Tray");			break; // Tray Tab
				case IDX_AREA2:		strInspName = _T("None");			break;
				case IDX_AREA3:		strInspName = _T("None");			break;
				case IDX_AREA4:		strInspName = _T("ChipOcr");		break; // ChipOcr
				case IDX_AREA5:		strInspName = _T("Mixing");			break; // Mixing
				case IDX_AREA6:		strInspName = _T("None");			break;
				case IDX_AREA7:		strInspName = _T("None");			break;

#elif RELEASE_SG
				case IDX_AREA1:		strInspName = _T("Stacker");		break; // Stacker TrayOcr

#elif RELEASE_2G									
				case IDX_AREA1:		strInspName = _T("Banding");		break; // Banding
				case IDX_AREA2:		strInspName = _T("HIC");			break; // HIC

#elif RELEASE_3G
				case IDX_AREA1:		strInspName = _T("None");			break; // Position1
				case IDX_AREA2:		strInspName = _T("None");			break; // Position2
				case IDX_AREA3:		strInspName = _T("None");			break; // Desiccant1
				case IDX_AREA4:		strInspName = _T("None");			break; // Desiccant2
				case IDX_AREA5:		strInspName = _T("Material");		break; // Material

#elif RELEASE_4G									
				case IDX_AREA1:		strInspName = _T("MBB");			break; // MBB

#elif RELEASE_5G									
				case IDX_AREA1:		strInspName = _T("Box");			break; // Box Quality
				case IDX_AREA2:		strInspName = _T("None");			break; // Sealing Quality

#elif RELEASE_6G
				case IDX_AREA2:		strInspName = _T("Tape");			break; // Tape

#endif
				}

#ifndef RELEASE_3G
				// 다른 검사 .mch 및 .bmp 파일 저장되어 추가, 250218 황대은J
				if (strInspName == _T("None") || strSavePatch.Find(strInspName) == -1) continue;

				strMatchFileName.Format(_T("%s%s_%d.mch"), strSavePatch, strInspName, nMatchIndex);
				strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strSavePatch, strInspName, nMatchIndex);

				try
				{
					if (!m_pMatch[nViewIndex][nMatchIndex])
						break;

					if (m_pMatch[nViewIndex][nMatchIndex]->GetPatternLearnt())
					{
						m_pMatch[nViewIndex][nMatchIndex]->Save(T2A((LPTSTR)(LPCTSTR)strMatchFileName));

#ifdef RELEASE_2G
						if (nViewIndex == IDX_AREA2)
						{
							EImageC24 pImg;
							m_pMatch[nViewIndex][nMatchIndex]->CopyLearntPattern(pImg);
							pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
						}
						else
						{
							EImageBW8 pImg;
							m_pMatch[nViewIndex][nMatchIndex]->CopyLearntPattern(pImg);
							pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
						}
#else
						EImageBW8 pImg;
						m_pMatch[nViewIndex][nMatchIndex]->CopyLearntPattern(pImg);

						USES_CONVERSION;
						pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
#endif
					}
					else
					{
						if (IsExistFile((LPCTSTR)strMatchBmpFileName))
							::DeleteFile(strMatchBmpFileName);
						if (IsExistFile((LPCTSTR)strMatchFileName))
							::DeleteFile(strMatchFileName);
					}
				}
				catch (EException& e)
				{
					std::string error = e.what();
					error += "\n";
					OutputDebugStringA(error.c_str());
					//break; // break하면 모델 등록 중 이전에 등록하지 않은 모델이 있으면 등록 안되는 문제 발생
				}

#elif RELEASE_3G
				if (TabIndex != TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY)
				{
					// 다른 검사 .mch 및 .bmp 파일 저장되어 추가, 250218 황대은J
					if (strInspName == _T("None") || strSavePatch.Find(strInspName) == -1) continue;

					strMatchFileName.Format(_T("%s%s_%d.mch"), strSavePatch, strInspName, nMatchIndex);
					strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strSavePatch, strInspName, nMatchIndex);

					try
					{
						if (!m_pMatch[nViewIndex][nMatchIndex])
							break;

						if (m_pMatch[nViewIndex][nMatchIndex]->GetPatternLearnt())
						{
							m_pMatch[nViewIndex][nMatchIndex]->Save(T2A((LPTSTR)(LPCTSTR)strMatchFileName));

							EImageBW8 pImg;
							m_pMatch[nViewIndex][nMatchIndex]->CopyLearntPattern(pImg);

							USES_CONVERSION;
							pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
						}
						else
						{
							if (IsExistFile((LPCTSTR)strMatchBmpFileName))
								::DeleteFile(strMatchBmpFileName);
							if (IsExistFile((LPCTSTR)strMatchFileName))
								::DeleteFile(strMatchFileName);
						}
					}
					catch (EException& e)
					{
						std::string error = e.what();
						error += "\n";
						OutputDebugStringA(error.c_str());
					}
				}

#endif
			}

#ifdef RELEASE_1G
			if( (TabIndex == TEACH_TAB_IDX_CHIP) && (nViewIndex == IDX_AREA4) )
			{
				CString strInspName = _T("Chip");

				if (strInspName == _T("None") || strSavePatch.Find(strInspName) == -1)
					return FALSE;

				strMatchFileName.Format(_T("%s%s.mch"), strSavePatch, strInspName);
				strMatchBmpFileName.Format(_T("%s%s.bmp"), strSavePatch, strInspName);

				try
				{
					if (!m_pMatchChip)
						return FALSE;

					if (m_pMatchChip->GetPatternLearnt())
					{
						m_pMatchChip->Save(T2A((LPTSTR)(LPCTSTR)strMatchFileName));

						EImageBW8 pImg;
						m_pMatchChip->CopyLearntPattern(pImg);

						USES_CONVERSION;
						pImg.Save(T2A((LPTSTR)(LPCTSTR)strMatchBmpFileName));
					}
					else
					{
						if (IsExistFile((LPCTSTR)strMatchBmpFileName))
							::DeleteFile(strMatchBmpFileName);
						if (IsExistFile((LPCTSTR)strMatchFileName))
							::DeleteFile(strMatchFileName);
					}
				}
				catch (EException& e)
				{
					std::string error = e.what();
					error += "\n";
					OutputDebugStringA(error.c_str());
				}
			}

#endif	// RELEASE_1G
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CInspectionVision::Load_Label(LPCTSTR lpszRecipeName)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CString strLoadPatch, strMatchFileName, strMatchBmpFileName;
	USES_CONVERSION;

#if defined(RELEASE_4G) || defined(RELEASE_6G)
	strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("Label\\") + lpszRecipeName;
	strLoadPatch += +_T("\\LabelInfo\\");

	for (int nMaskingIndex = 0; nMaskingIndex < EMATCH_TYPE_LABEL_MAX; nMaskingIndex++)
	{
		strMatchFileName.Format(_T("%sLabel_Mask%d.mch"), strLoadPatch, nMaskingIndex);
		strMatchBmpFileName.Format(_T("%sLabel_Mask%d.bmp"), strLoadPatch, nMaskingIndex);

		try
		{
			if (IsExistFile((LPCTSTR)strMatchFileName))
			{
				if (!m_pLabelMatch[nMaskingIndex])
					break;

				m_pLabelMatch[nMaskingIndex]->Load(T2A((LPTSTR)(LPCTSTR)strMatchFileName));
			}
		}
		catch (EException& e)
		{
			e.what();
			break;
		}
	}
	
	// Label Align
	strMatchFileName.Format(_T("%sAlign_%d.mch"), strLoadPatch, 0);
	strMatchBmpFileName.Format(_T("%sAlign_%d.bmp"), strLoadPatch, 0);

	int nViewIndex = IDX_AREA1;
#ifdef RELEASE_4G
	nViewIndex = IDX_AREA2;
#endif
	try
	{
		if (IsExistFile((LPCTSTR)strMatchFileName))
		{
			if (!m_pMatch[nViewIndex][0])
				return FALSE;

			m_pMatch[nViewIndex][0]->Load(T2A((LPTSTR)(LPCTSTR)strMatchFileName));
		}
	}
	catch (EException& e)
	{
		e.what();
		return FALSE;
	}
#endif

	return TRUE;
}

BOOL CInspectionVision::Load( LPCTSTR lpszRecipeName, CString strKind )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	CString strLoadPatch, strMatchFileName, strMatchBmpFileName;
	USES_CONVERSION;

	try
	{
		strLoadPatch = CModelInfo::Instance()->GetRecipeRootPath();

		CString strInspName = _T("None");
		int nViewIndex = IDX_AREA_MAX;

		if (strKind == TRAYOCR_KIND)
		{
			strLoadPatch += _T("1G_TrayOcr\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\Tray");
			nViewIndex = IDX_AREA1;
		}

#ifdef RELEASE_1G
		else if (strKind == CHIPOCR_KIND)
		{
			strLoadPatch += _T("1G_ChipOcr\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\ChipOcr");
			nViewIndex = IDX_AREA4;
		}
		else if (strKind == CHIP_KIND)
		{
			strLoadPatch += _T("1G_Chip\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\Chip");
			nViewIndex = IDX_AREA4;
		}
		else if (strKind == MIXING_KIND)
		{
			strLoadPatch += _T("1G_Mixing\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\Mixing");
			nViewIndex = IDX_AREA5;
		}
		
#elif RELEASE_SG
		else if (strKind == STACKEROCR_KIND)
		{
			strLoadPatch += _T("SG_StackerOcr\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\Stacker");
			nViewIndex = IDX_AREA1;
		}

#elif RELEASE_2G
		else if (strKind == HIC_KIND)
		{
			strLoadPatch += _T("2G_HIC\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\HIC");
			nViewIndex = IDX_AREA2;
		}

#elif RELEASE_3G
		else if (strKind == SUBMATERIAL_KIND)
		{
			strLoadPatch += _T("3G_Desiccant_Material\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\Material");
			nViewIndex = IDX_AREA5;
		}

#elif RELEASE_4G
		else if (strKind == MBB_KIND)
		{
			strLoadPatch += _T("4G_MBB\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\MBB");
			nViewIndex = IDX_AREA1;
		}

#elif RELEASE_5G
		else if (strKind == BOXQUALITY_KIND)
		{
			strLoadPatch += _T("5G_Box\\");
			strLoadPatch += lpszRecipeName;
			strInspName = _T("\\Box");
			nViewIndex = IDX_AREA1;
		}

#endif
		if(strInspName == _T("None")) return FALSE;

		if (strKind == CHIP_KIND)
		{
			strMatchFileName.Format(_T("%s%s.mch"), strLoadPatch, strInspName);
			strMatchBmpFileName.Format(_T("%s%s.bmp"), strLoadPatch, strInspName);

			if (IsExistFile((LPCTSTR)strMatchFileName))
			{
				if (!m_pMatchChip)
					return FALSE;

				m_pMatchChip->Load(T2A((LPTSTR)(LPCTSTR)strMatchFileName));
			}
			else
			{
				delete m_pMatchChip;

				m_pMatchChip = NULL;
				m_pMatchChip = new EMatcher;
			}
		}
		else
		{
			for (int nMatchIndex = 0; nMatchIndex < nMATCH_MAX; nMatchIndex++)
			{
				strMatchFileName.Format(_T("%s%s_%d.mch"), strLoadPatch, strInspName, nMatchIndex);
				strMatchBmpFileName.Format(_T("%s%s_%d.bmp"), strLoadPatch, strInspName, nMatchIndex);

				if (IsExistFile((LPCTSTR)strMatchFileName))
				{
					if (!m_pMatch[nViewIndex][nMatchIndex])
						break;

					m_pMatch[nViewIndex][nMatchIndex]->Load(T2A((LPTSTR)(LPCTSTR)strMatchFileName));
				}
				else
				{
					delete m_pMatch[nViewIndex][nMatchIndex];

					m_pMatch[nViewIndex][nMatchIndex] = NULL;
					m_pMatch[nViewIndex][nMatchIndex] = new EMatcher;
				}
			}
		}
	}
	catch (EException& e)
	{
		e.what();
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::SaveImage( CxImageObject *pImgObj, InspectType inspecttype, int nViewIndex, int nGrabCnt )
{
	///////////////////////////////////////////////////////////////////////////////////
	// ----- Error Check -----
	if( pImgObj == NULL || pImgObj->GetImageBuffer() == NULL )	
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("SaveImage[%d] - pSrcImgObj(NULL) NG"), nViewIndex + 1 );
		return FALSE;
	}

	int nWidth	= pImgObj->GetWidth();
	int nHeight = pImgObj->GetHeight();

	if (nWidth <= 0 || nHeight <= 0)
	{
		CVisionSystem::Instance()->WriteMessage( LogTypeError, _T("SaveImage[%d] - nWidth nHeight NG"), nViewIndex + 1 );
		return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// ----- Set File Path -----
	CString strDriver = CSystemConfig::Instance()->GetSaveLogPath();

	CTime tmCurrent = CTime::GetCurrentTime();
	CString strImgSaveDay, strImgSaveTime;
	strImgSaveDay.	Format( _T("%04d%02d%02d"), tmCurrent.GetYear(), tmCurrent.GetMonth(),	tmCurrent.GetDay()		);
	strImgSaveTime.	Format( _T("%02d%02d%02d"), tmCurrent.GetHour(), tmCurrent.GetMinute(), tmCurrent.GetSecond()	);

	// ----- Set LotID BoxNo -----
	CString strLotID = CVisionSystem::Instance()->GetLotID(inspecttype);
	if (strLotID.IsEmpty()) strLotID = _T("LOT ID Empty");

	short sBoxNo = CVisionSystem::Instance()->GetBoxNo(inspecttype);

	CString strImgBasePath;
	strImgBasePath.	Format( _T("%s:\\HITS\\ImageLog\\%s\\%s\\%03d"), strDriver, strImgSaveDay, strLotID, sBoxNo );

	if( RESULT_OK( CVisionSystem::Instance()->GetErrorCode(nViewIndex) ) )	strImgBasePath += _T("\\OK");
	else																	strImgBasePath += _T("\\NG");

	MakeDirectory( strImgBasePath );

	CString strInspKind;
	switch (nViewIndex)
	{
#ifdef RELEASE_1G
	case IDX_AREA1: 	strInspKind += _T("_TrayOcr");			break;
	case IDX_AREA2:
	case IDX_AREA3:		strInspKind += _T("_3DChipCnt");		break;
	case IDX_AREA4:		strInspKind += _T("_Chip");				break;
	case IDX_AREA5: 	strInspKind += _T("_Mixing");			break;
	case IDX_AREA6:		strInspKind += _T("_Lift_F");			break;
	case IDX_AREA7:		strInspKind += _T("_Lift_R");			break;

#elif RELEASE_SG
	case IDX_AREA1: 	strInspKind += _T("_StackerOcr");		break;

#elif RELEASE_2G
	case IDX_AREA1:		strInspKind += _T("_Banding");			break;
	case IDX_AREA2:		strInspKind += _T("_HIC");				break;

#elif RELEASE_3G
	case IDX_AREA1:		strInspKind += _T("_Position1");		break;
	case IDX_AREA2:		strInspKind += _T("_Position2");		break;
	case IDX_AREA3:		strInspKind += _T("_Desiccant1");		break;
	case IDX_AREA4:		strInspKind += _T("_Desiccant2");		break;
	case IDX_AREA5:		strInspKind += _T("_Material");			break;

#elif RELEASE_4G
	case IDX_AREA1:		strInspKind += _T("_MBB");				break;
	case IDX_AREA2:		strInspKind += _T("_MBBLabel");			break;

#elif RELEASE_5G
	case IDX_AREA1:		strInspKind += _T("_BoxQuality");		break;
	case IDX_AREA2:		strInspKind += _T("_SealingQuality");	break;

#elif RELEASE_6G
	case IDX_AREA1:		strInspKind += _T("_BoxLabel");			break;
	case IDX_AREA2:		strInspKind += _T("_Tape");				break;

#endif
	}

	// ----- Set Recipe -----
	CString strSubName = _T("");
	CString strRecipeName = CModelInfo::Instance()->GetModelName();
	if (strRecipeName.IsEmpty()) strRecipeName = _T("Recipe Empty");

	switch (inspecttype)
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:			strRecipeName = CModelInfo::Instance()->GetModelNameTrayOcr();					break;
	case InspectType3DChipCnt:			strRecipeName = CModelInfo::Instance()->GetModelName3DChipCnt();				break;
	case InspectTypeChipOcr:			strRecipeName = CModelInfo::Instance()->GetModelNameChipOcr();					break;
	case InspectTypeChip:				strSubName.Format(_T("(%d)"), nGrabCnt);
										strRecipeName = CModelInfo::Instance()->GetModelNameChip();						break;
	case InspectTypeMixing:				strRecipeName = CModelInfo::Instance()->GetModelNameMixing();					break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:			strRecipeName = CModelInfo::Instance()->GetModelNameLift();						break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:			if (RESULT_OK(CVisionSystem::Instance()->GetErrorCode(nViewIndex))) 	strRecipeName = CModelInfo::Instance()->GetModelNameStackerOcr();
										else																	strRecipeName = _T("Recipe Empty");
										strSubName.Format(_T("(%d)"), CVisionSystem::Instance()->m_nStackerZAxis);		break;

#elif RELEASE_2G
	case InspectTypeBanding:
		switch (nGrabCnt)
		{
		case 0:							strSubName.Format(_T("(1st_L)"));												break;
		case 1:							strSubName.Format(_T("(1st_R)"));												break;
		case 2:							strSubName.Format(_T("(1st_C)"));												break;
		case 3:							strSubName.Format(_T("(2nd_L)"));												break;
		case 4:							strSubName.Format(_T("(2nd_R)"));												break;
		case 5:							strSubName.Format(_T("(2nd_C)"));												break;
		case 6:							strSubName.Format(_T("(3rd_L)"));												break;
		case 7:							strSubName.Format(_T("(3rd_R)"));												break;
		case 8:							strSubName.Format(_T("(3rd_C)"));												break;
		case 9:							strSubName.Format(_T("(4th_L)"));												break;
		case 10:						strSubName.Format(_T("(4th_R)"));												break;
		case 11:						strSubName.Format(_T("(4th_C)"));												break;
		default:						strSubName.Format(_T("(%d)"), nGrabCnt);										break;
		}
										strRecipeName = CModelInfo::Instance()->GetModelNameBanding();					break;
	case InspectTypeHIC:				strRecipeName = CModelInfo::Instance()->GetModelNameHIC();						break;

#elif RELEASE_3G
	case InspectTypePositionLeft:
	case InspectTypePositionRight:
	case InspectTypeDesiccantLeft:
	case InspectTypeDesiccantRight:		strRecipeName = CModelInfo::Instance()->GetModelNameDesiccantCutting();			break;
	case InspectTypeMaterial:			strRecipeName = CModelInfo::Instance()->GetModelNameDesiccantMaterial();
										strRecipeName += _T(" + ");
										strRecipeName +=  CModelInfo::Instance()->GetModelNameDesiccantMaterialTray();	break;

#elif RELEASE_4G
	case InspectTypeMBB:				strRecipeName = CModelInfo::Instance()->GetModelNameMBB();						break;
	case InspectTypeLabel:				strSubName.Format( _T("(%d)"), nGrabCnt);
										strRecipeName = CModelInfo::Instance()->GetModelNameLabel();					break;

#elif RELEASE_5G
	case InspectTypeBox:				strRecipeName = CModelInfo::Instance()->GetModelNameBoxQuality();				break;
	case InspectTypeSealing:			strRecipeName = CModelInfo::Instance()->GetModelNameSealingQuality();			break;

#elif RELEASE_6G
	case InspectTypeLabel:				strSubName.Format( _T("(%d)"), nGrabCnt);
										strRecipeName = CModelInfo::Instance()->GetModelNameLabel();					break;
	case InspectTypeTape:				strRecipeName = CModelInfo::Instance()->GetModelNameTape();						break;

#endif
	default:
		break;
	}

	CString strImgPathName;
	strImgPathName.Format( _T("%s\\%s%s_%s%s"), strImgBasePath, strImgSaveTime, strInspKind, strSubName, strRecipeName );

	///////////////////////////////////////////////////////////////////////////////////
	CString strPathBuff;

	// ----- Image Save -----
	strPathBuff = strImgPathName + _T(".bmp");
	pImgObj->SaveToFile( strPathBuff );

#ifdef RELEASE_1G
	if (inspecttype == InspectTypeTrayOcr)
	{
		CSystemConfig::Instance()->SetTrayOcrImageFile(strPathBuff);
		return TRUE;
	}
#endif

	// ---- 3D Data -----
#ifdef RELEASE_SG
	CString strPathBin;
	strPathBin = strImgPathName + _T(".bin");
	CVisionSystem::Instance()->SaveDepthToBin(strPathBin);

#endif
	
	// ----- Merge Image Save -----
	CString strSendPath, strMergePath, strMergePathName;
	strSendPath = CSystemConfig::Instance()->GetSendPath();

#ifdef RELEASE_1G
	if (inspecttype == InspectType3DChipCnt)		strMergePath.Format(_T("%s\\%s\\%s\\Bundle\\"), strSendPath, strImgSaveDay, strLotID);
	else											strMergePath.Format(_T("%s\\%s\\%s\\%03d\\"), strSendPath, strImgSaveDay, strLotID, sBoxNo);
	MakeDirectory(strMergePath);

#else
	strMergePath.Format(_T("%s\\%s\\%s\\%03d\\"), strSendPath, strImgSaveDay, strLotID, sBoxNo);
	MakeDirectory( strMergePath );

#endif


#ifdef RELEASE_1G
	if (inspecttype == InspectType3DChipCnt)		strMergePathName.Format(_T("%s\\%s%s(%03d)_%s%s"), strMergePath, strImgSaveTime, strInspKind, sBoxNo, strSubName, strRecipeName);
	else											strMergePathName.Format(_T("%s\\%s%s_%s%s"), strMergePath, strImgSaveTime, strInspKind, strSubName, strRecipeName);

#else
	strMergePathName.Format(_T("%s\\%s%s_%s%s"), strMergePath, strImgSaveTime, strInspKind, strSubName, strRecipeName);

#endif

	eBypass ebypass;
	switch (inspecttype)
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:		ebypass = (eBypass)CModelInfo::Instance()->GetTrayOcr().nUseBypass_TrayOcr;			break;
	case InspectType3DChipCnt:		ebypass = (eBypass)CModelInfo::Instance()->Get3DChipCnt().nUseBypass_3DChipCnt;		break;
	case InspectTypeChipOcr:		ebypass = (eBypass)CModelInfo::Instance()->GetChipOcr().nUseBypass_ChipOcr;			break;
	case InspectTypeChip:			ebypass = (eBypass)CModelInfo::Instance()->GetChip().nUseBypass_Chip;				break;
	case InspectTypeMixing:			ebypass = (eBypass)CModelInfo::Instance()->GetMixing().nUseBypass_Mixing;			break;
	case InspectTypeLiftFront:
	case InspectTypeLiftRear:		ebypass = (eBypass)CModelInfo::Instance()->GetLiftInfo().nUseBypass_Lift;			break;

#elif RELEASE_SG
	case InspectTypeStackerOcr:		ebypass = eBypass::BYPASS_OFF;														break;

#elif RELEASE_2G
	case InspectTypeBanding:		ebypass = (eBypass)CModelInfo::Instance()->GetBandingInfo().nUseBypass_Banding;		break;
	case InspectTypeHIC:			ebypass = (eBypass)CModelInfo::Instance()->GetHICInfo().nUseBypass_HIC;				break;

#elif RELEASE_3G
	case InspectTypePositionLeft:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionLeft;		break;
	case InspectTypePositionRight:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_PositionRight;	break;
	case InspectTypeDesiccantLeft:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingLeft;		break;
	case InspectTypeDesiccantRight:	ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantCuttingInfo().nUseBypass_Desiccant_CuttingRight;		break;
	case InspectTypeMaterial:		ebypass = (eBypass)CModelInfo::Instance()->GetDesiccantMaterialInfo().nUseBypass_Material;					break;

#elif RELEASE_4G
	case InspectTypeMBB:			ebypass = (eBypass)CModelInfo::Instance()->GetMBBInfo().nUseBypass_MBB;				break;
	case InspectTypeLabel:			ebypass = (eBypass)CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label;			break;

#elif RELEASE_5G
	case InspectTypeBox:			ebypass = (eBypass)CModelInfo::Instance()->GetBoxInfo().nUseBypass_Box;				break;
	case InspectTypeSealing:		ebypass = (eBypass)CModelInfo::Instance()->GetSealingInfo().nUseBypass_Sealing;		break;

#elif RELEASE_6G
	case InspectTypeLabel:			ebypass = (eBypass)CModelInfo::Instance()->GetLabelInfo().nUseBypass_Label;			break;
	case InspectTypeTape:			ebypass = (eBypass)CModelInfo::Instance()->GetTapeInfo().nUseBypass_Tape;			break;

#endif
	default:
		break;
	}

	if (CSystemConfig::Instance()->GetOriginImgSend())
	{
		if (RESULT_OK(CVisionSystem::Instance()->GetErrorCode(nViewIndex)) || ebypass != BYPASS_OFF)
		{
#ifdef RELEASE_1G
			if (inspecttype == InspectTypeChip)
			{
				CString strFile;
				strFile.Format(_T("%s\\%s%s_Merge_%s.jpg"), strMergePath, strImgSaveTime, strInspKind, strRecipeName);

				m_strMergePath = strFile;
				return TRUE;
			}
#endif
			USES_CONVERSION;

			strPathBuff = strMergePathName + _T(".jpg");

			if ((int)pImgObj->GetChannel() == 3)
			{
				EImageC24 EC24Dst;
				EC24Dst.SetImagePtr(nWidth, nHeight, (BYTE*)pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
				EC24Dst.SaveJpeg(T2A((LPTSTR)(LPCTSTR)(strPathBuff)), 90);
			}
			else
			{
				EImageBW8 EBW8Src;
				EBW8Src.SetImagePtr(nWidth, nHeight, (BYTE*)pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

				EImageC24 EC24Conv;
				EC24Conv.SetSize(nWidth, nHeight);

				EasyImage::Convert(&EBW8Src, &EC24Conv);
				EC24Conv.SaveJpeg(T2A((LPTSTR)(LPCTSTR)(strPathBuff)), 90);
			}
		}
	}
	CVisionSystem::Instance()->WriteMessage( LogTypeNormal, _T("SaveImage[%d] - End[%s]"), nViewIndex + 1, strImgPathName );

	return TRUE;
}

BOOL CInspectionVision::MergeImage(CString strPath, BOOL bXMerge)
{
	WRITE_LOG(WL_DEBUG, _T("In MergeImage()"));

	USES_CONVERSION;
	CModelInfo::stChip& stChip = CModelInfo::Instance()->GetChip();

	int nGrabCntX = stChip.nMatrix_X / stChip.nChipFovCnt_X;
	int nGrabCntY = stChip.nMatrix_Y / stChip.nChipFovCnt_Y;
	int nTotalCount = nGrabCntX * nGrabCntY;

	int nImgW = 4000;
	int nImgH = 3000;

	int nTotalW = nImgW * nGrabCntX;
	int nTotalH = nImgH * nGrabCntY;

	WRITE_LOG(WL_DEBUG, _T("MergeImage() - 결과 이미지 크기: %d x %d\n"), nTotalW, nTotalH);

	// 결과 이미지 생성
	CxImageObject pImgObj_Dst;
	pImgObj_Dst.Create(nTotalW, nTotalH, 8, 1);

	BYTE* pDstBuf = (BYTE*)pImgObj_Dst.GetImageBuffer();
	if (!pDstBuf)
	{
		WRITE_LOG(WL_DEBUG, _T("MergeImage() - 버퍼 생성 실패"));
		return FALSE;
	}

	for (int y = 0; y < nGrabCntY; ++y)            // y: 0 = top row, Y-1 = bottom row (matches your tables)
	{
		for (int x = 0; x < nGrabCntX; ++x)
		{
			int nFactor = (nGrabCntX - 1 - x);      // block number from right to left
			int nSrcIndex;

			if ((nFactor % 2) == 0)
			{
				// 이 열은 위->아래로 "내림" (예: 11,10,9,8)
				nSrcIndex = nFactor * nGrabCntY + (nGrabCntY - 1 - y);
			}
			else
			{
				// 이 열은 위->아래로 "오름"  (예: 4,5,6,7)
				nSrcIndex = nFactor * nGrabCntY + y;
			}

			// 범위 안전성 검사
			if (nSrcIndex < 0 || nSrcIndex >= nGrabCntX * nGrabCntY)
				continue;

			CxImageObject* pSrcObj = CVisionSystem::Instance()->GetImageObject_Chip(nSrcIndex);
			if (!pSrcObj) continue;
			BYTE* pSrcBuf = (BYTE*)pSrcObj->GetImageBuffer();
			if (!pSrcBuf) continue;

			// 붙일 위치: (x, y) 은 화면에서 왼쪽(x) / 위에서부터(y)
			int nDstX = x * nImgW;
			int nDstY = y * nImgH;

			for (int yy = 0; yy < nImgH; ++yy)
			{
				BYTE* pSrcLine = pSrcBuf + yy * nImgW;
				BYTE* pDstLine = pDstBuf + ((nDstY + yy) * nTotalW) + nDstX;
				memcpy(pDstLine, pSrcLine, nImgW);
			}
		}
	}

	WRITE_LOG(WL_DEBUG, _T("MergeImage() - Merge 완료"));

	// 8bit Mat
	cv::Mat MatGray(nTotalH, nTotalW, CV_8UC1);
	memcpy(MatGray.data, pImgObj_Dst.GetImageBuffer(), nTotalW * nTotalH);

	// 24bit Mat
	cv::Mat Mat;
	cv::cvtColor(MatGray, Mat, cv::COLOR_GRAY2BGR);

	// 격자
	cv::Scalar lineColor(255, 255, 255);
	int nLingThickness = 20;

	int totalW = nImgW * nGrabCntX;
	int totalH = nImgH * nGrabCntY;

	for (int i = 1; i < nGrabCntX; ++i)
	{
		int x = i * nImgW;
		cv::line(Mat, cv::Point(x, 0), cv::Point(x, nTotalH - 1), lineColor, nLingThickness);
	}

	for (int j = 1; j < nGrabCntY; ++j)
	{
		int y = j * nImgH;
		cv::line(Mat, cv::Point(0, y), cv::Point(nTotalW - 1, y), lineColor, nLingThickness);
	}
	// 격자 완료

	// Rect
	cv::Scalar RectColor(0, 165, 255);   // 오렌지
	nLingThickness = 20;
	int nOffset = 100;

	float fCalibrationX = CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[2][CalibrationDirX];
	float fCalibrationY = CSystemConfig::Instance()->GetCalibrationConfig().fCaliMeasure[2][CalibrationDirY];

	CRect rcRect;
	if (stChip.nSelectUnit == 0)
	{
		rcRect.right = stChip.rcInspArea.right + nOffset;
		rcRect.bottom = stChip.rcInspArea.bottom + nOffset;
		rcRect.left = rcRect.right - (stChip.nChipFovCnt_X * stChip.rcInspArea.Width()) - ((stChip.nChipUnitPitch_X_PX - stChip.rcInspArea.Width()) * (stChip.nChipFovCnt_X - 1)) - (nOffset*2);
		rcRect.top = rcRect.bottom - (stChip.nChipFovCnt_Y * stChip.rcInspArea.Height()) - ((stChip.nChipUnitPitch_Y_PX - stChip.rcInspArea.Height()) * (stChip.nChipFovCnt_Y - 1)) - (nOffset*2);
	}
	else
	{
		rcRect.right = GetRoundShort(stChip.rcInspArea.right) + nOffset;
		rcRect.bottom = GetRoundShort(stChip.rcInspArea.bottom) + nOffset;
		rcRect.left = GetRoundShort(rcRect.right - (stChip.nChipFovCnt_X * stChip.rcInspArea.Width()) - (((stChip.fChipUnitPitch_X_MM * 1000.f / fCalibrationX) - stChip.rcInspArea.Width()) * (stChip.nChipFovCnt_X - 1))) - (nOffset*2);
		rcRect.top = GetRoundShort(rcRect.bottom - (stChip.nChipFovCnt_Y * stChip.rcInspArea.Height()) - (((stChip.fChipUnitPitch_Y_MM * 1000.f / fCalibrationY) - stChip.rcInspArea.Height()) * (stChip.nChipFovCnt_Y - 1))) - (nOffset*2);
	}

	for (int y = 0; y < nGrabCntY; ++y)
	{
		for (int x = 0; x < nGrabCntX; ++x)
		{
			int nBaseX = x * nImgW;
			int nBaseY = y * nImgH;

			cv::Rect rc( nBaseX + rcRect.left,
						nBaseY + rcRect.top,
						rcRect.Width(), rcRect.Height());

			cv::rectangle(Mat, rc, RectColor, nLingThickness);
		}
	}
	// Rect 완료

	//strPath = _T("\\\\192.168.11.99\\VisionImageLog\\ChipMergeTest.jpg");
//	strPath = _T("D:\\ChipMergeTest.jpg");
	WRITE_LOG(WL_DEBUG, _T("MergeImage() - Save Start"));
	cv::String str = CT2A(strPath);
	std::vector<int> vcParams;
	vcParams.push_back(cv::IMWRITE_JPEG_QUALITY);
	vcParams.push_back(90);

	cv::imwrite(str, Mat, vcParams);

	WRITE_LOG(WL_DEBUG, _T("MergeImage() - Save end"));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
static const int MIN_REDUCE_AREA	= 64;
BOOL CInspectionVision::SetLearnModel( CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker, InspectType inspecttype, UINT nIdx, UINT nType, BOOL bBlob, BOOL bColor )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	CRect rtNormalRect;

	if( pImgObj == NULL ) return 0;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	CRect rcTrack(rcTracker);
	CRect rcImgBody( 0, 0, pImgObj->GetWidth()-1, pImgObj->GetHeight()-1 );
	rcTrack.IntersectRect( rcTrack, rcImgBody );

	try
	{
		CRect rcAlign = rcTrack;
		if ( rcAlign.IsRectEmpty())
			return FALSE;

		EImageC24 ImageC24;
		EImageBW8 ImageX;
		if (bColor)		ImageC24.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
		else			ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		EMatcher* pMatcher = m_pMatch[nIdx][nType];

		rtNormalRect.left	= rcTrack.left;
		rtNormalRect.right	= rcTrack.right;
		rtNormalRect.top	= rcTrack.top <= nPageSizeY ? rcTrack.top : rcTrack.top;
		rtNormalRect.bottom	= rcTrack.bottom <= nPageSizeY ? rcTrack.bottom : rcTrack.bottom;

		EROIC24 ImageROI24;
		EROIBW8 ImageROI;
		if (bColor)
		{
			ImageROI24.Detach();
			ImageROI24.Attach(&ImageC24);
			ImageROI24.SetPlacement(rtNormalRect.left, rtNormalRect.top, rtNormalRect.Width(), rtNormalRect.Height());

			pMatcher->SetMinReducedArea(MIN_REDUCE_AREA);
			pMatcher->LearnPattern(&ImageROI24);
		}
		else
		{
			ImageROI.Detach();
			ImageROI.Attach( &ImageX );
			ImageROI.SetPlacement( rtNormalRect.left, rtNormalRect.top, rtNormalRect.Width(), rtNormalRect.Height() );

			pMatcher->SetMinReducedArea(MIN_REDUCE_AREA);
			pMatcher->LearnPattern(&ImageROI);
		}

		COLORBOX clrBox;
		clrBox.CreateObject( PDC_GREEN, rtNormalRect, PS_SOLID, 3 );
		pGO->AddDrawBox( clrBox );

		return TRUE;
	}

	catch (EException& e)
	{
		std::string error = e.what();
		error += "\n";
		OutputDebugStringA( error.c_str() );
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::SetLearnModel_ForLabel( CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker, UINT nIdx, UINT nType )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	CRect rcBlobRect;
	CRect rcNormalRect;
	CRect rtNormalRect;

	if( pImgObj == NULL ) return 0;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	CRect rcTrack(rcTracker);
	CRect rcImgBody( 0, 0, pImgObj->GetWidth()-1, pImgObj->GetHeight()-1 );
	rcTrack.IntersectRect( rcTrack, rcImgBody );

	EImageBW8 ImageX;
	ImageX.SetImagePtr( nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes()*8);

	if( &ImageX == NULL )
		return FALSE;

	try
	{
		CRect rcAlign = rcTrack;
		if ( rcAlign.IsRectEmpty())
			return FALSE;

		EImageBW8 ImageX;
		ImageX.SetImagePtr( nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes()*8);

		EMatcher* pMatcher = m_pLabelMatch[nType];

		rtNormalRect.left	= rcTrack.left;
		rtNormalRect.right	= rcTrack.right;
		rtNormalRect.top	= rcTrack.top <= nPageSizeY ? rcTrack.top : rcTrack.top;
		rtNormalRect.bottom	= rcTrack.bottom <= nPageSizeY ? rcTrack.bottom : rcTrack.bottom;

		EROIBW8 ImageROI;
		ImageROI.Detach();
		ImageROI.Attach( &ImageX );
		ImageROI.SetPlacement( rtNormalRect.left, rtNormalRect.top, rtNormalRect.Width(), rtNormalRect.Height() );

		pMatcher->SetMinReducedArea(MIN_REDUCE_AREA);
		pMatcher->LearnPattern(&ImageROI);

		COLORBOX clrBox;
		clrBox.CreateObject( PDC_GREEN, rtNormalRect, PS_SOLID, 3 );
		pGO->AddDrawBox( clrBox );

		return TRUE;
	}

	catch (EException& e)
	{
		std::string error = e.what();
		error += "\n";
		OutputDebugStringA( error.c_str() );
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::SetLearnModel_ForBox(CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker, UINT nIdx, UINT nType)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CRect rtNormalRect;

	if (pImgObj == NULL) return 0;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	CRect rcTrack(rcTracker);
	CRect rcImgBody(0, 0, pImgObj->GetWidth() - 1, pImgObj->GetHeight() - 1);
	rcTrack.IntersectRect(rcTrack, rcImgBody);

	EImageBW8 ImageX_Origin;
	ImageX_Origin.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
	
	if (&ImageX_Origin == NULL)
		return FALSE;

	try
	{
		CRect rcAlign = rcTrack;
		if (rcAlign.IsRectEmpty())
			return FALSE;

		EMatcher* pMatcher = m_pMatch[nIdx][nType];

		rtNormalRect.left = rcTrack.left;
		rtNormalRect.right = rcTrack.right;
		rtNormalRect.top = rcTrack.top <= nPageSizeY ? rcTrack.top : rcTrack.top;
		rtNormalRect.bottom = rcTrack.bottom <= nPageSizeY ? rcTrack.bottom : rcTrack.bottom;

		EROIBW8 ImageROI;
		ImageROI.Detach();
		ImageROI.Attach(&ImageX_Origin);
		ImageROI.SetPlacement(rtNormalRect.left, rtNormalRect.top, rtNormalRect.Width(), rtNormalRect.Height());

		pMatcher->SetMinReducedArea(MIN_REDUCE_AREA);
		pMatcher->LearnPattern(&ImageROI);

		COLORBOX clrBox;
		clrBox.CreateObject(PDC_GREEN, rtNormalRect, PS_SOLID, 3);
		pGO->AddDrawBox(clrBox);

		return TRUE;
	}

	catch (EException& e)
	{
		std::string error = e.what();
		error += "\n";
		OutputDebugStringA(error.c_str());
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::SetLearnModel_ForChip(CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CRect rtNormalRect;

	if (pImgObj == NULL) return 0;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	CRect rcTrack(rcTracker);
	CRect rcImgBody(0, 0, pImgObj->GetWidth() - 1, pImgObj->GetHeight() - 1);
	rcTrack.IntersectRect(rcTrack, rcImgBody);

	EImageBW8 ImageX;
	ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

	if (&ImageX == NULL)
		return FALSE;

	try
	{
		CRect rcAlign = rcTrack;
		if (rcAlign.IsRectEmpty())
			return FALSE;

		EImageBW8 ImageX;
		ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		EMatcher* pMatcher = m_pMatchChip;

		rtNormalRect.left = rcTrack.left;
		rtNormalRect.right = rcTrack.right;
		rtNormalRect.top = rcTrack.top <= nPageSizeY ? rcTrack.top : rcTrack.top;
		rtNormalRect.bottom = rcTrack.bottom <= nPageSizeY ? rcTrack.bottom : rcTrack.bottom;

		EROIBW8 ImageROI;
		ImageROI.Detach();
		ImageROI.Attach(&ImageX);
		ImageROI.SetPlacement(rtNormalRect.left, rtNormalRect.top, rtNormalRect.Width(), rtNormalRect.Height());

		pMatcher->SetMinReducedArea(MIN_REDUCE_AREA);
		pMatcher->LearnPattern(&ImageROI);

		COLORBOX clrBox;
		clrBox.CreateObject(PDC_GREEN, rtNormalRect, PS_SOLID, 3);
		pGO->AddDrawBox(clrBox);

		return TRUE;
	}

	catch (EException& e)
	{
		std::string error = e.what();
		error += "\n";
		OutputDebugStringA(error.c_str());
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::EVisionOCR_TrayOCR(CxGraphicObject* pGO, CxImageObject* pImgObj, const int nViewIndex, VisionProcess::TrayOCRData& stTrayData, CModelInfo::stTrayOcr& stTray, BOOL bSave)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;
		
	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORARROW clrArrow;

	CString strOcrInfo					= stTray.strTrayOcrInfo;
	const CRect rcInspArea				= stTray.rcInspArea;
	CRect rcTabBegin					= stTray.rcTabBegin;
	const int nOcrAreaDistX				= stTray.nOcrAreaDistX;
	const int nOcrAreaDistY				= stTray.nOcrAreaDistY;
	const int nCharWidthMin				= stTray.nCharWidthMin;
	const int nCharWidthMax				= stTray.nCharWidthMax;
	const int nCharHeightMin			= stTray.nCharHeightMin;
	const int nCharHeightMax			= stTray.nCharHeightMax;
	const BOOL bRemoveBorder			= stTray.bRemoveBorder;
	const BOOL bRemoveNarrowFlat		= stTray.bRemoveNarrowFlat;
	const BOOL bCutLargeChar			= stTray.bCutLargeChar;
	CRect& rcLastInspArea				= stTray.rcLastInspArea;
	const BOOL bUseAdaptiveThreshold	= stTray.bUseAdaptiveThreshold;
	const int nMethod					= stTray.nMethod;
	const int nHalfKernelSize			= stTray.nHalfKernelSize;
	const int nThresholdOffset			= stTray.nThresholdOffset;

	CRect rcNonROI[nTRAY_MASKING_MAX], rcNonInsp[nTRAY_MASKING_MAX];
	int nMaskingAreaDistX[nTRAY_MASKING_MAX], nMaskingAreaDistY[nTRAY_MASKING_MAX];
	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		rcNonROI[i] = stTray.rcMaskingArea[i];
		nMaskingAreaDistX[i] = stTray.nMaskingAreaDistX[i];
		nMaskingAreaDistY[i] = stTray.nMaskingAreaDistY[i];
	}

	strOcrInfo.Replace(_T(" "), NULL);

	try 
	{
		USES_CONVERSION;

		if (!MatchModel_Tab(pGO, pImgObj, nViewIndex, rcTabBegin))	return FALSE;

		CxImageObject pImgObj2;
		pImgObj2.Clone(pImgObj);

		// ocr
		CRect rcOcrArea(0, 0, 0, 0);
		rcOcrArea.left = rcTabBegin.left + nOcrAreaDistX;
		rcOcrArea.top = rcTabBegin.top + nOcrAreaDistY;
		rcOcrArea.right = rcOcrArea.left + rcInspArea.Width();
		rcOcrArea.bottom = rcOcrArea.top + rcInspArea.Height();

		if (rcOcrArea.left < 0 ||
			rcOcrArea.top < 0 ||
			rcOcrArea.right > pImgObj2.GetWidth() ||
			rcOcrArea.bottom > pImgObj2.GetHeight())
		{
			WRITE_LOG(WL_ERROR, _T("ROI Out of Range"));
			clrText.SetText(_T("ROI Out of Range"));
			clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
			pGO->AddDrawText(clrText);
			return FALSE;
		}

		rcLastInspArea = rcOcrArea;

		EImageBW8 ImageX, Image_Thres;
		ImageX.SetImagePtr(pImgObj2.GetWidth(), pImgObj2.GetHeight(), pImgObj2.GetImageBuffer(), pImgObj2.GetWidthBytes() * 8);
		Image_Thres.SetSize(pImgObj2.GetWidth(), pImgObj2.GetHeight());
		EasyImage::Oper(EArithmeticLogicOperation_Copy, &ImageX, &Image_Thres);

		clrArrow.CreateObject(PDC_BLUE, rcTabBegin.TopLeft(), rcOcrArea.TopLeft(), CxGOArrow::ArrowDirectionEnd, CxGOArrow::ArrowHeadTypeLine, 15);
		pGO->AddDrawArrow(clrArrow);

		EROIBW8 eTabROI, eTabROIThreshold;
		eTabROI.Attach(&ImageX);
		eTabROI.SetPlacement(rcOcrArea.left, rcOcrArea.top, rcOcrArea.Width(), rcOcrArea.Height());

		eTabROIThreshold.Attach(&Image_Thres);
		eTabROIThreshold.SetPlacement(rcOcrArea.left, rcOcrArea.top, rcOcrArea.Width(), rcOcrArea.Height());

		clrBox.CreateObject(PDC_ORANGE, rcOcrArea, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

//		CString strImgName = _T("D:\\eTabROI.bmp");
//		eTabROI.Save(T2A((LPTSTR)(LPCTSTR)strImgName));

		CString strOcrFont = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TrayInputFont.OCR");
		EOCR eOCR;
		eOCR.Load(T2A(strOcrFont));
		CString strResult;
		if (bUseAdaptiveThreshold)
		{
			EasyImage::AdaptiveThreshold(&eTabROI, &eTabROIThreshold, (EAdaptiveThresholdMethod)nMethod, nHalfKernelSize, nThresholdOffset);

			// masking
			float fAvgGv = 0.f;
			EasyImage::PixelAverage(&eTabROI, fAvgGv);
			for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
			{
				rcNonInsp[i].left = nMaskingAreaDistX[i];
				rcNonInsp[i].top = nMaskingAreaDistY[i];
				rcNonInsp[i].right = rcNonInsp[i].left + rcNonROI[i].Width();
				rcNonInsp[i].bottom = rcNonInsp[i].top + rcNonROI[i].Height();

				rcNonInsp[i].NormalizeRect();
				MaskingArea_Black(eTabROIThreshold, rcNonInsp[i], 0);
			}

//			strImgName = _T("D:\\eTabROIThreshold.bmp");
//			eTabROIThreshold.Save(T2A((LPTSTR)(LPCTSTR)strImgName));

			strResult = ReadOCR_Tray(eTabROIThreshold, pGO, eOCR, stTrayData, strOcrInfo, nCharWidthMin, nCharWidthMax, nCharHeightMin, nCharHeightMax,
				bRemoveBorder, bRemoveNarrowFlat, bCutLargeChar);
		}
		else
		{
			// masking
			float fAvgGv = 0.f;
			EasyImage::PixelAverage(&eTabROI, fAvgGv);
			for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
			{
				rcNonInsp[i].left = nMaskingAreaDistX[i];
				rcNonInsp[i].top = nMaskingAreaDistY[i];
				rcNonInsp[i].right = rcNonInsp[i].left + rcNonROI[i].Width();
				rcNonInsp[i].bottom = rcNonInsp[i].top + rcNonROI[i].Height();

				rcNonInsp[i].NormalizeRect();
				MaskingArea_Black(eTabROI, rcNonInsp[i], GetRoundShort(fAvgGv-10));
			}
			strResult = ReadOCR_Tray(eTabROI, pGO, eOCR, stTrayData, strOcrInfo, nCharWidthMin, nCharWidthMax, nCharHeightMin, nCharHeightMax,
				bRemoveBorder, bRemoveNarrowFlat, bCutLargeChar);
		}
		
		if (bSave)
		{
			if (bUseAdaptiveThreshold)	EVisionOCRAddFont(eTabROIThreshold, &eOCR, strOcrFont);
			else						EVisionOCRAddFont(eTabROI,			&eOCR, strOcrFont);
		}

		BOOL bResult = FALSE;
		if (strResult == strOcrInfo)
			bResult = TRUE;
		else
		{
			strResult.MakeUpper();
			strOcrInfo.MakeUpper();

			if (strResult == strOcrInfo)
				bResult = TRUE;
		}

		pImgObj2.Destroy();
		return bResult;
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}
	catch (...)
	{
		WRITE_LOG(WL_ERROR, _T("Unknown Exception"));
		clrText.SetText(_T("Unknown Exception"));
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}

}

BOOL CInspectionVision::EVisionOCR_ChipOCR(CxGraphicObject* pGO, CxImageObject* pImgObj, VisionProcess::ChipOCRData& stChipData,
											CRect& rcChipROI, CString strChipOcrInfo, int nViewIndex, BOOL bSave)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	COLORBOX clrBox;
	COLORTEXT clrText;

	try
	{
		CRect rcChipInside;
		rcChipInside.SetRectEmpty();

		CModelInfo::stChipOcr stChipOcr = CModelInfo::Instance()->GetChipOcr();

		EImageBW8 ImageX;
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		clrBox.CreateObject(PDC_ORANGE, rcChipROI, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

		EROIBW8 eChipROI;
		eChipROI.Attach(&ImageX);
		eChipROI.SetPlacement(rcChipROI.left, rcChipROI.top, rcChipROI.Width(), rcChipROI.Height());

		if(!MatchModel_ChipInside(pGO, eChipROI, nViewIndex, rcChipInside))		return FALSE;

		CPoint ptChipInside(rcChipInside.left, rcChipInside.top);
		EROIBW8 eOcrROI;
		SetOcrROI(ImageX, eOcrROI, pGO, ptChipInside, stChipOcr.nOcrAreaDistanceX, stChipOcr.nOcrAreaDistanceY, stChipOcr.rcOcrROI);

		USES_CONVERSION;
		CString strOcrFont = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\ChipOCRFont.OCR");
		EOCR eOCR;
		eOCR.Load(T2A(strOcrFont));

		CString strResult = ReadOCR_Chip(eOcrROI, pGO, eOCR, stChipData, 
			stChipOcr.nCharWidthMin, stChipOcr.nCharWidthMax, stChipOcr.nCharHeightMin, stChipOcr.nCharHeightMax, stChipOcr.nThreshold,
			stChipOcr.nFontColor, stChipOcr.bRemoveBorder, stChipOcr.bRemoveNarrowFlat, stChipOcr.bCutLargeChar);

		if (bSave)	EVisionOCRAddFont(eOcrROI, &eOCR, strOcrFont);

		BOOL bResult = FALSE;
		if (strChipOcrInfo == strResult)
			bResult = TRUE;
		else
		{
			strChipOcrInfo.MakeUpper();
			strResult.MakeUpper();
			if (strChipOcrInfo == strResult)
				bResult = TRUE;
			else
			{
				if (strChipOcrInfo.GetLength() == strResult.GetLength())
				{
					// ----- Change Similar Characters -----
					CheckSimilarFont(strChipOcrInfo, strResult);
					if (strChipOcrInfo == strResult)
						bResult = TRUE;
				}
			}
		}
		
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("OCR 결과 : %s"), strResult);
		else												clrText.SetText(_T("OCR Result : %s"), strResult);
		clrText.CreateObject(bResult?PDC_GREEN:PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, 50, TRUE);
		pGO->AddDrawText(clrText);
			
		return bResult;
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}
}

BOOL CInspectionVision::EVisionOCR_StackerOCR(CxGraphicObject* pGO, CxImageObject* pImgObj, int nViewIndex, VisionProcess::TrayOCRData& stTrayData, 
												CModelInfo::stStackerOcr& stStacker, BOOL bSave)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	COLORBOX clrBox;
	COLORTEXT clrText;

	CString strOcrInfo					= stStacker.strOcrInfo;
	CRect& rcLastInspArea				= stStacker.rcLastInspArea;
	const int nOcrAreaDistX				= stStacker.nOcrAreaDistX;
	const int nOcrAreaDistY				= stStacker.nOcrAreaDistY;
	const int nCharWidthMin				= stStacker.nCharWidthMin;
	const int nCharWidthMax				= stStacker.nCharWidthMax;
	const int nCharHeightMin			= stStacker.nCharHeightMin;
	const int nCharHeightMax			= stStacker.nCharHeightMax;
	const BOOL bRemoveBorder			= stStacker.bRemoveBorder;
	const BOOL bRemoveNarrowFlat		= stStacker.bRemoveNarrowFlat;
	const BOOL bCutLargeChar			= stStacker.bCutLargeChar;
	const BOOL bUseAdaptiveThreshold	= stStacker.bUseAdaptiveThreshold;
	const int nMethod					= stStacker.nMethod;
	const int nHalfKernelSize			= stStacker.nHalfKernelSize;
	const int nThresholdOffset			= stStacker.nThresholdOffset;

	CRect rcNonROI[nTRAY_MASKING_MAX], rcNonInsp[nTRAY_MASKING_MAX];
	int nMaskingAreaDistX[nTRAY_MASKING_MAX], nMaskingAreaDistY[nTRAY_MASKING_MAX];
	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		rcNonROI[i] = stStacker.rcMaskingArea[i];
		nMaskingAreaDistX[i] = stStacker.nMaskingAreaDistX[i];
		nMaskingAreaDistY[i] = stStacker.nMaskingAreaDistY[i];
	}

	strOcrInfo.Replace(_T(" "), NULL);

	try
	{
		USES_CONVERSION;

		double dResultAngle;
		DPOINT dptGaugeCenter, dptResult;
		dptGaugeCenter.x = pImgObj->GetWidth()/2;
		dptGaugeCenter.y = pImgObj->GetHeight()/2;

		int nOffset = 1;
		float fAngle = 0.f;
		CRect rcOcrArea(0, 0, 0, 0);
		if (stStacker.bRotateImage)
		{
			LineGauge(pImgObj, ETransitionType::ETransitionType_Bw, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, 500.f, 2000.f,
				0, 20, 1, 10, 0, 1, 3, 20.f, FALSE, FALSE, TRUE, dptResult, dResultAngle);

			nOffset = -1;
			fAngle = 180.f;
		}
		else
		{
			LineGauge(pImgObj, ETransitionType::ETransitionType_Wb, ETransitionChoice::ETransitionChoice_NthFromEnd, dptGaugeCenter, 500.f, 2000.f,
				0, 20, 1, 10, 0, 1, 3, 20.f, FALSE, FALSE, TRUE, dptResult, dResultAngle);
		}

		if (dptResult.x < 1)
			return FALSE;
		if (dptResult.y < 1)
			return FALSE;

		CxImageObject pImgObj2;
		pImgObj2.Clone(pImgObj);

		DPOINT dptCenter, dptResultPoint;
		dptCenter.x = pImgObj2.GetWidth() / 2;
		dptCenter.y = stStacker.bRotateImage ? 800 : 640; // 고정일 듯
		dptResultPoint.x = 0.f;
		dptResultPoint.y = 0.f;
		
		// ocr
		BOOL bPoint = FALSE;
		if (stStacker.bRotateImage)
		{
			bPoint = PointGauge(&pImgObj2, ETransitionType_Bw, ETransitionChoice_NthFromEnd, dptCenter, 2500.f, fAngle, 20, 150, 10, dptResultPoint);

			int nStartX = GetRoundShort(dptResultPoint.x);
			int nEndX = nStartX + CSystemConfig::Instance()->GetImageCut();

			rcOcrArea.left = bPoint ? nStartX : 0;
			rcOcrArea.top = GetRoundShort(dptResult.y + 30);
			rcOcrArea.right = bPoint ? nEndX : pImgObj2.GetWidth();
			rcOcrArea.bottom = rcOcrArea.top + 180;
		}
		else
		{
			bPoint = PointGauge(&pImgObj2, ETransitionType_Bw, ETransitionChoice_NthFromEnd, dptCenter, 2500.f, fAngle, 20, 150, 10, dptResultPoint);
			
			int nEndX = GetRoundShort(dptResultPoint.x);
			int nStartX = nEndX - CSystemConfig::Instance()->GetImageCut();

			rcOcrArea.left = bPoint ? nStartX : 0;
			rcOcrArea.top = GetRoundShort(dptResult.y - (190 * nOffset));
			rcOcrArea.right = bPoint ? nEndX : pImgObj2.GetWidth();
			rcOcrArea.bottom = rcOcrArea.top + (180 * nOffset);
		}

		if (rcOcrArea.left < 0)							rcOcrArea.left = 0;
		if (rcOcrArea.right > pImgObj->GetWidth())		rcOcrArea.right = pImgObj->GetWidth()-1;

		rcOcrArea.NormalizeRect();
		rcLastInspArea = rcOcrArea;

		EImageBW8 ImageX, Image_Thres;
		ImageX.SetImagePtr(pImgObj2.GetWidth(), pImgObj2.GetHeight(), pImgObj2.GetImageBuffer(), pImgObj2.GetWidthBytes() * 8);
		Image_Thres.SetSize(pImgObj2.GetWidth(), pImgObj2.GetHeight());
		EasyImage::Oper(EArithmeticLogicOperation_Copy, &ImageX, &Image_Thres);

		EROIBW8 eTabROI, eTabROIThreshold;
		eTabROI.Attach(&ImageX);
		eTabROI.SetPlacement(rcOcrArea.left, rcOcrArea.top, rcOcrArea.Width(), rcOcrArea.Height());

		eTabROIThreshold.Attach(&Image_Thres);
		eTabROIThreshold.SetPlacement(rcOcrArea.left, rcOcrArea.top, rcOcrArea.Width(), rcOcrArea.Height());

		clrBox.CreateObject(PDC_ORANGE, rcOcrArea, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

//		CString strImgName = _T("D:\\eTabROI.bmp");
//		eTabROI.Save(T2A((LPTSTR)(LPCTSTR)strImgName));

		CString strOcrFont = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\TrayInputFont.OCR");
		EOCR eOCR;
		eOCR.Load(T2A(strOcrFont));
		CString strResult;
		if (bUseAdaptiveThreshold)
		{
			EasyImage::AdaptiveThreshold(&eTabROI, &eTabROIThreshold, (EAdaptiveThresholdMethod)nMethod, nHalfKernelSize, nThresholdOffset);

			// masking
			float fAvgGv = 0.f;
			EasyImage::PixelAverage(&eTabROI, fAvgGv);
			for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
			{
				rcNonInsp[i].left = nMaskingAreaDistX[i];
				rcNonInsp[i].top = nMaskingAreaDistY[i];
				rcNonInsp[i].right = rcNonInsp[i].left + rcNonROI[i].Width();
				rcNonInsp[i].bottom = rcNonInsp[i].top + rcNonROI[i].Height();

				rcNonInsp[i].NormalizeRect();
				MaskingArea_Black(eTabROIThreshold, rcNonInsp[i], 0);
			}
			
//			strImgName = _T("D:\\eTabROIThreshold.bmp");
//			eTabROIThreshold.Save(T2A((LPTSTR)(LPCTSTR)strImgName));

			strResult = ReadOCR_Tray(eTabROIThreshold, pGO, eOCR, stTrayData, strOcrInfo, nCharWidthMin, nCharWidthMax, nCharHeightMin, nCharHeightMax,
				bRemoveBorder, bRemoveNarrowFlat, bCutLargeChar);
		}
		else
		{
			// masking
			float fAvgGv = 0.f;
			EasyImage::PixelAverage(&eTabROI, fAvgGv);
			for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
			{
				rcNonInsp[i].left = nMaskingAreaDistX[i];
				rcNonInsp[i].top = nMaskingAreaDistY[i];
				rcNonInsp[i].right = rcNonInsp[i].left + rcNonROI[i].Width();
				rcNonInsp[i].bottom = rcNonInsp[i].top + rcNonROI[i].Height();

				rcNonInsp[i].NormalizeRect();
				MaskingArea_Black(eTabROI, rcNonInsp[i], GetRoundShort(fAvgGv-10));
			}

			strResult = ReadOCR_Tray(eTabROI, pGO, eOCR, stTrayData, strOcrInfo, nCharWidthMin, nCharWidthMax, nCharHeightMin, nCharHeightMax,
				bRemoveBorder, bRemoveNarrowFlat, bCutLargeChar);
		}
		if (bSave)
		{
			if (bUseAdaptiveThreshold)	EVisionOCRAddFont(eTabROIThreshold, &eOCR, strOcrFont);
			else						EVisionOCRAddFont(eTabROI, &eOCR, strOcrFont);
		}

		BOOL bResult = FALSE;
		if (strResult == strOcrInfo)
			bResult = TRUE;
		else
		{
			strResult.MakeUpper();
			strOcrInfo.MakeUpper();

			if (strResult == strOcrInfo)
				bResult = TRUE;
		}

		pImgObj2.Destroy();
		return bResult;
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_LIGHTRED, pImgObj->GetWidth() / 2, pImgObj->GetHeight() / 2, 20, FALSE, CxGOText::TextAlignmentCenter);
		pGO->AddDrawText(clrText);

		return FALSE;
	}
}

bool CInspectionVision::CompareCenterX(const CRect& rect1, const CRect& rect2)
{
	return rect1.CenterPoint().x < rect2.CenterPoint().x;
}

bool CInspectionVision::CompareWidth(const CRect& rect1, const CRect& rect2)
{
	return rect1.Width() < rect2.Width();
}

BOOL CInspectionVision::BuildUsingEasyObject( CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, CRect& rcBlobRect, BOOL nKindOfObj, BOOL bColor, int nThreshold/*=0*/,  BOOL bDraw/*= TRUE*/ )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	EImageBW8 ImageX;
	EImageC24 ImageX24;
	if( bColor )
	{
		ImageX24.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes()*8);
		ImageX.SetSize(&ImageX24);
		EasyColor::GetComponent(&ImageX24, &ImageX, 0);
	}
	else
	{
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes()*8);
	}

	rcRect.NormalizeRect();

	EROIBW8 ImageROI;
	ImageROI.Attach( &ImageX );
	ImageROI.SetPlacement( rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height() );

	EListItem* pObject;
	COLORBOX clrBox;

	int		nOffsetX, nOffsetY, nArea;
	float	fWidth, fHeight, fCenterX, fCenterY;

	nOffsetX = rcRect.left;
	nOffsetY = rcRect.top;
	fWidth = fHeight = fCenterX = fCenterY = 0.0f;
	nArea = 0;

	if(bDraw)
	{
		clrBox.CreateObject( PDC_ORANGE, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom );
		pGO->AddDrawBox( clrBox );
	}

	ECodedImage CodedImage;

	if( nKindOfObj == OBJ_BLACK ) 
	{
		CodedImage.SetWhiteClass( 0 );
		CodedImage.SetBlackClass( 1 );
	}
	else if( nKindOfObj == OBJ_WHITE )
	{
		CodedImage.SetWhiteClass( 1 );
		CodedImage.SetBlackClass( 0 );
	}
	
	if		( nThreshold == 0	) nThreshold = EasyImage::AutoThreshold( &ImageROI, EThresholdMode_MinResidue, 0.5).Value;
	else if ( nThreshold == 255 ) nThreshold = EasyImage::AutoThreshold( &ImageROI, EThresholdMode_MaxEntropy, 0.5).Value;

	CodedImage.SetThreshold( nThreshold );
	CodedImage.BuildObjects( &ImageROI );

	int nObjNum = CodedImage.GetNumObjects();
	if( nObjNum <= 0)
		return FALSE;

	CodedImage.AnalyseObjects( ELegacyFeature_Area, ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight, ELegacyFeature_LimitCenterX, ELegacyFeature_LimitCenterY );
	CodedImage.SelectObjectsUsingFeature( ELegacyFeature_LimitWidth,  0.0f, 5.5f, ESelectOption_RemoveOutOfRange );	// px
 	CodedImage.SelectObjectsUsingFeature( ELegacyFeature_LimitHeight, 0.0f, 5.5f, ESelectOption_RemoveOutOfRange ); // px
	CodedImage.SetConnexity( EConnexity_Connexity8 );

// 	nObjNum = CodedImage.GetNumSelectedObjects();
// 	if( nObjNum <= 0)
// 		return FALSE;

	int nMaxArea = 0;
	pObject = CodedImage.GetFirstObjPtr();
	for( int i = 0; i<nObjNum; ++i )
	{
		CodedImage.GetObjectFeature( ELegacyFeature_LimitWidth,		pObject, fWidth		);
		CodedImage.GetObjectFeature( ELegacyFeature_LimitHeight,	pObject, fHeight	);
		CodedImage.GetObjectFeature( ELegacyFeature_LimitCenterX,	pObject, fCenterX	);
		CodedImage.GetObjectFeature( ELegacyFeature_LimitCenterY,	pObject, fCenterY	);
		CodedImage.GetObjectFeature( ELegacyFeature_Area,			pObject, nArea		);

		if(nMaxArea < nArea)
		{
			nMaxArea = nArea;

			rcBlobRect.left   = nOffsetX + (int)GetRoundLong(fCenterX - (fWidth  / 2.f));
			rcBlobRect.top    = nOffsetY + (int)GetRoundLong(fCenterY - (fHeight / 2.f));
			rcBlobRect.right  = nOffsetX + (int)GetRoundLong(fCenterX + (fWidth  / 2.f));
			rcBlobRect.bottom = nOffsetY + (int)GetRoundLong(fCenterY + (fHeight / 2.f));
			rcBlobRect.NormalizeRect();

		}

		pObject = CodedImage.GetNextObjPtr( pObject );
	}

	if(bDraw)
	{
		rcBlobRect.InflateRect(10, 10, 10, 10);

		clrBox.CreateObject( PDC_GREEN, rcBlobRect, PS_SOLID, 2 );
		pGO->AddDrawBox( clrBox );
	}

	return TRUE;
}

BOOL CInspectionVision::BuildUsingEasyObject_TrayChipCnt(CxGraphicObject* pGO, CxImageObject* pImgObj, 
														int nMinThreshold, int nMaxThreshold, CRect rcChipPos, 
														double dRowPitch, double dColumnPitch, int nXCount, int nYCount)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	int nDrawTextPosY = 100;
	int nDrawTextSize = 25;
	CString strException = _T("");

	COLORBOX clrBox;
	COLORTEXT clrText;

	BOOL bCheck = TRUE;

	CVisionSystem* pVisionSystem = CVisionSystem::Instance();

	/* 기존 가로 방향(카메라와 수평)으로 놓고 찍었던 트레이를 세로 방향으로 놓고 찍는 것으로 변경되어
		ChipWidth ↔ ChipHeight, RowCount ↔ ColumnCount를 상호 변경*/
#ifdef RELEASE_1G
	try 
	{
		EImageBW8 eImageX;
		eImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		constexpr int nTrayAreaThreshold = 200000;

		ECodedImage2 CodedImage;
		EObjectSelection CodedImageObjectSelection;
		EImageEncoder CodedImageEncoder;

		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_MinResidue);
		CodedImageEncoder.Encode(eImageX, CodedImage);

		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjects(CodedImage);
		CodedImageObjectSelection.RemoveUsingUnsignedIntegerFeature(EFeature_Area, nTrayAreaThreshold, ESingleThresholdMode_Less);
		CodedImageObjectSelection.Sort(EFeature_Area, ESortDirection_Descending); // Blob Area 기준 내림차순 정렬

		int nObjCnt = CodedImageObjectSelection.GetElementCount();
		if (nObjCnt == 0)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("트레이를 찾을 수 없습니다.");
			else												strException = _T("Can't find a tray.");

			throw strException;
		}
		
		ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(0);

		// Line Gauge로 이미지 틀어짐 교정
		DPOINT dptGaugeCenter;
		dptGaugeCenter.x = GetRoundLong(eCodedElement.GetLeftLimit());
		dptGaugeCenter.y = eCodedElement.GetBoundingBoxCenterY();

		DPOINT dptResult;
		double dResultAngle = 0.f;
		LineGauge2(eImageX, ETransitionType::ETransitionType_Bw, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, 60.0f, 850.0f,
					-90.0f, 20, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptResult, dResultAngle);

		float fRotateAngle = dResultAngle <= 0.0f ? static_cast<float>(dResultAngle + 90.0f) : static_cast<float>(dResultAngle - 90.0f);

		EImageBW8 eImageX_temp;
		eImageX_temp.SetSize(&eImageX);
		float fPivotX = static_cast<float>(eImageX.GetWidth() / 2);
		float fPivotY = static_cast<float>(eImageX.GetHeight() / 2);

		if (abs(fRotateAngle) > 0.05f)
		{
			EasyImage::ScaleRotate(&eImageX, fPivotX, fPivotY, fPivotX, fPivotY, 1.0f, 1.0f, fRotateAngle, &eImageX_temp);
			EasyImage::Oper(EArithmeticLogicOperation_Copy, &eImageX_temp, &eImageX);
		}

		int nFirstChipLeft, nFirstChipTop;

		nFirstChipLeft = rcChipPos.left;
		nFirstChipTop = rcChipPos.top;

		EROIBW8 ChipROI;
		ChipROI.Attach(&eImageX);
		ChipROI.SetPlacement(nFirstChipLeft, nFirstChipTop, rcChipPos.Width(), rcChipPos.Height()); // 첫 번째 칩 위치에 ROI 설정
	
		double dOffsetXAccumulation = nFirstChipLeft;
		double dOffsetYAccumulation = nFirstChipTop;
		
		// SingleThreshold 사용 시 DepthMap에서 point(pixel) 깨지는 부분의 Gray Value가 0이 되는 문제로 인해 사용 어려움
		CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_GrayscaleDoubleThreshold);
		CodedImageEncoder.GetGrayscaleDoubleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleDoubleThresholdSegmenter().SetWhiteLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleDoubleThresholdSegmenter().SetNeutralLayerEncoded(true);
		CodedImageEncoder.GetGrayscaleDoubleThresholdSegmenter().SetLowThreshold(nMinThreshold);	
		CodedImageEncoder.GetGrayscaleDoubleThresholdSegmenter().SetHighThreshold(nMaxThreshold);	
		CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);

		const int nAreaThreshold = GetRoundShort((rcChipPos.Width() * rcChipPos.Height()) / 3);
		for (int i = 0; i < nYCount; ++i)
		{
			for (int j = 0; j < nXCount; ++j)
			{
				// 이미지 범위 벗어났을 때 예외처리 코드 추가, 250213 황대은J
				if (ChipROI.GetOrgX() < 0 || ChipROI.GetOrgY() < 0 || 
					ChipROI.GetOrgX() + ChipROI.GetWidth() > pImgObj->GetWidth() || 
					ChipROI.GetOrgY() + ChipROI.GetHeight() > pImgObj->GetHeight())
				{
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("검사 범위가 이미지를 초과했습니다. Viewer3");
					else												strException = _T("The Inspect Area Exceeds The Image. Viewer3");
					throw strException;
				}

				CodedImageEncoder.Encode(ChipROI, CodedImage);
				CodedImageObjectSelection.Clear();
				CodedImageObjectSelection.AddObjectsUsingUnsignedIntegerFeature(CodedImage, EFeature_Area, nAreaThreshold, ESingleThresholdMode_GreaterEqual);

				//CRect reChip;
				DRECT drcChip;
				int nObjCnt = CodedImageObjectSelection.GetElementCount();
				if (nObjCnt == 1) 
				{
					drcChip.left = ChipROI.GetOrgX();
					drcChip.top = ChipROI.GetOrgY();
					drcChip.right = ChipROI.GetOrgX() + rcChipPos.Width();
					drcChip.bottom = ChipROI.GetOrgY() + rcChipPos.Height();

					clrBox.CreateObject(PDC_ORANGE, drcChip, PS_DASHDOT, 1);
					pGO->AddDrawBox(clrBox);
				}
				else
				{
					drcChip.left = ChipROI.GetOrgX();
					drcChip.top = ChipROI.GetOrgY();
					drcChip.right = ChipROI.GetOrgX() + rcChipPos.Width();
					drcChip.bottom = ChipROI.GetOrgY() + rcChipPos.Height();

					clrBox.CreateObject(PDC_GREEN, drcChip, PS_SOLID, 3);
					pGO->AddDrawBox(clrBox);

					m_nExistingChips++;
				}
				dOffsetXAccumulation += dColumnPitch;
				ChipROI.SetPlacement(GetRoundShort(dOffsetXAccumulation), GetRoundShort(dOffsetYAccumulation), rcChipPos.Width(), rcChipPos.Height());
			}

			dOffsetXAccumulation = nFirstChipLeft;
			dOffsetYAccumulation += dRowPitch;

			ChipROI.SetPlacement(GetRoundShort(dOffsetXAccumulation), GetRoundShort(dOffsetYAccumulation), rcChipPos.Width(), rcChipPos.Height());
		}

		// 수동 검사
		if (pVisionSystem->GetRunStatus() == RunStatusStop) 
		{
			if (m_nExistingChips == nManualChipNum)		bCheck = TRUE;
			else										bCheck = FALSE;

			COLORTEXT clrText;
			CString strText;

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("검사 수량 / 총 수량 : [%d / %d]"), m_nExistingChips, nManualChipNum);
			else												strText.Format(_T("Inspect QTY / Total QTY : [%d / %d]"), m_nExistingChips, nManualChipNum);

			clrText.SetText(strText);
			nDrawTextPosY += nDrawTextSize * 4;
			clrText.CreateObject(bCheck ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
			pGO->AddDrawText(clrText);
		} 

		// 자동 검사
		else 
		{
			if (m_nExistingChips == pVisionSystem->m_nMMI3DChipCnt)	bCheck = TRUE;
			else													bCheck = FALSE;
			
			COLORTEXT clrText;
			CString strText;

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("검사 수량 / 총 수량 : [%d / %d]"), m_nExistingChips, pVisionSystem->m_nMMI3DChipCnt);
			else												strText.Format(_T("Inspect QTY / Total QTY : [%d / %d]"), m_nExistingChips, pVisionSystem->m_nMMI3DChipCnt);

			clrText.SetText(strText);
			nDrawTextPosY += nDrawTextSize * 4;
			clrText.CreateObject(bCheck ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
			pGO->AddDrawText(clrText);

			if (bCheck)		CVisionSystem::Instance()->m_b3DChipCntResult = TRUE;
			else			CVisionSystem::Instance()->m_b3DChipCntResult = FALSE;
		}
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25*4), DEF_FONT_BASIC_SIZE, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}
#endif
	return bCheck;
}

BOOL CInspectionVision::BuildUsingEasyObject_Lift(CxGraphicObject* pGO, CxImageObject* pImgObj, const int nViewIndex, const CModelInfo::stLiftInfo& stLift)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	const long lnPageCount = stLift.lPageCount;
	const UINT8 nTrayPixelThreshold = stLift.nTrayPixelThreshold;
	int nLiftHeight, nBreakWidth, nBreakHeight;
	if (stLift.nSelectUnit == 0) // px
	{
		nLiftHeight = stLift.nLiftHeight_PX;
		nBreakWidth = stLift.nBreakWidth_PX;
		nBreakHeight = stLift.nBreakHeight_PX;
	}
	else
	{
		nLiftHeight = GetRoundShort(stLift.dbLiftHeight_MM * 1000.0 / m_fCalY[nViewIndex]);
		nBreakWidth = GetRoundShort(stLift.dbBreakWidth_MM * 1000.0 / m_fCalX[nViewIndex]);
		nBreakHeight = GetRoundShort(stLift.dbBreakHeight_MM * 1000.0 / m_fCalY[nViewIndex]);
	}

	int nFirstWidth;
	int nSecondWidth;
	int nThirdWidth;
	int nFourthWidth;

	if (nViewIndex == 5) // IDX_AREA6)
	{
		nFirstWidth = stLift.nWidthFirst;
		nSecondWidth = stLift.nWidthSecond;
		nThirdWidth = stLift.nWidthThird;
		nFourthWidth = stLift.nWidthFourth;
	}
	else if (nViewIndex == 6) // IDX_AREA7)
	{
		nFirstWidth = stLift.nRearWidthFirst;
		nSecondWidth = stLift.nRearWidthSecond;
		nThirdWidth = stLift.nRearWidthThird;
		nFourthWidth = stLift.nRearWidthFourth;
	}

	const int nLOffset_Front = stLift.nLeftOffset_Front;
	const int nLOffset = stLift.nLeftOffset;
	const int nROffset = stLift.nRightOffset;
	const int nTOffset = stLift.nTopOffset;
	const int nBOffset = stLift.nBottomOffset;

	COLORBOX clrBox;
	COLORTEXT clrText;

	BOOL bCheck = TRUE;
	
	CVisionSystem* pVisionSystem = CVisionSystem::Instance();

	try
	{
		EImageBW8 ImageX;
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		ECodedImage2 CodedImage;
		EImageEncoder CodedImageEncoder;
		EObjectSelection CodedImageObjectSelection;

		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_Absolute);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nTrayPixelThreshold);

		CodedImageEncoder.Encode(ImageX, CodedImage);
		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjectsUsingFloatFeature(CodedImage, EFeature_BoundingBoxWidth, (float)(lnPageCount / 2), ESingleThresholdMode_GreaterEqual);

		int nObjCnt = CodedImageObjectSelection.GetElementCount();

		if (!nObjCnt)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이를 찾을 수 없습니다."));
			else 												clrText.SetText(_T("Tray Not Found"));
			clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		CodedImageObjectSelection.Sort(EFeature_Area, ESortDirection_Descending);

		ECodedElement &eTraySide = CodedImageObjectSelection.GetElement(0);
		CRect rcTraySide;
		if(nViewIndex == 5)		rcTraySide.left	  = eTraySide.GetLeftLimit()   + nLOffset_Front;
		else					rcTraySide.left	  = eTraySide.GetLeftLimit()   + nLOffset;
		rcTraySide.right  = eTraySide.GetRightLimit()  - nROffset;
		rcTraySide.top	  = eTraySide.GetTopLimit()    + nTOffset;
		rcTraySide.bottom = eTraySide.GetBottomLimit() - nBOffset;

		EROIBW8 TraySideROI;
		TraySideROI.Attach(&ImageX);
		TraySideROI.SetPlacement(rcTraySide.left, rcTraySide.top, rcTraySide.Width(), rcTraySide.Height());

		const int nMidOffset = nFirstWidth + nSecondWidth;
		EROIBW8 TraySideMidROI;
		TraySideMidROI.Attach(&ImageX);
		TraySideMidROI.SetPlacement(rcTraySide.left + nFirstWidth + nSecondWidth, rcTraySide.top, nThirdWidth, rcTraySide.Height());

		float fAverage;
		EasyImage::PixelAverage(&TraySideMidROI, fAverage);
		UINT nThreshold = static_cast<int>(fAverage - 45);
		
		EROIBW8 TrayNumROI;
		TrayNumROI.Attach(&ImageX);
		TrayNumROI.SetPlacement(rcTraySide.left + nFirstWidth, rcTraySide.top, nSecondWidth, rcTraySide.Height());

		UINT nTrayCount = CalcNumTray(TrayNumROI, pGO, nThreshold);
		if (!nTrayCount)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이를 찾을 수 없습니다."));
			else 												clrText.SetText(_T("Tray Not Found"));
			clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
			pGO->AddDrawText(clrText);

			CString strText = _T("Lift : NG");

			if (CLanguageInfo::Instance()->m_nLangType == 0)
				strText = _T("사면 들뜸 : NG");

			bCheck = FALSE;
			clrText.SetText(strText);
			clrText.CreateObject(bCheck ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);

			return bCheck;
		}

		BOOL bTrayCount = TRUE;

		/* 추후 Tray 개수 검사 할 때 사용 */
		if (pVisionSystem->GetRunStatus() == RunStatusAutoRun)
		{
			if (nTrayCount != pVisionSystem->m_nMMITrayQuantity_Lift)
				bTrayCount = FALSE;

			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량 : V[%d] / H[%d]"), nTrayCount, pVisionSystem->m_nMMITrayQuantity_Lift);
			else 												clrText.SetText(_T("Tray QTY : V[%d] / H[%d]"), nTrayCount, pVisionSystem->m_nMMITrayQuantity_Lift);
			
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량 : V[%d]"), nTrayCount);
			else 												clrText.SetText(_T("Tray QTY : V[%d]"), nTrayCount);
		}
		clrText.CreateObject(bTrayCount ? PDC_GREEN : PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
		pGO->AddDrawText(clrText);

		if (!bTrayCount)
			return FALSE;

		// 검사
		UINT nTrayStatus = StatusOK;

		// 깨짐 검사 ROI 설정
		const int nOffset4 = nFirstWidth + nSecondWidth;

		// | 0 | 2 | 4 |
		// | 1 | 3 | 5 | 
		std::array<EROIBW8, 6> BreakROI;
		for (int i = 0; i < BreakROI.size(); ++i)
		{
			BreakROI[i].Attach(&ImageX);
		}

		BreakROI[0].SetPlacement(rcTraySide.left, rcTraySide.top, nFirstWidth, rcTraySide.Height() / 2);
		BreakROI[1].SetPlacement(rcTraySide.left, rcTraySide.top + rcTraySide.Height() / 2, nFirstWidth, rcTraySide.Height() / 2);
		BreakROI[2].SetPlacement(rcTraySide.left + nOffset4, rcTraySide.top, nThirdWidth, rcTraySide.Height() / 2);
		BreakROI[3].SetPlacement(rcTraySide.left + nOffset4, rcTraySide.top + rcTraySide.Height() / 2,  nThirdWidth, rcTraySide.Height() / 2);
		BreakROI[4].SetPlacement(rcTraySide.right - nFourthWidth, rcTraySide.top, nFourthWidth, rcTraySide.Height() / 2);
		BreakROI[5].SetPlacement(rcTraySide.right - nFourthWidth, rcTraySide.top + rcTraySide.Height() / 2, nFourthWidth, rcTraySide.Height() / 2);

		// 검사영역 그리기
		std::array<CRect, 6> rcBreakAllRect;
		for (int i = 0; i < rcBreakAllRect.size(); ++i)
		{
			rcBreakAllRect[i].SetRect(BreakROI[i].GetOrgX(), BreakROI[i].GetOrgY(), BreakROI[i].GetOrgX() + BreakROI[i].GetWidth(), BreakROI[i].GetOrgY() + BreakROI[i].GetHeight());

			clrBox.CreateObject(PDC_ORANGE, rcBreakAllRect[i], PS_SOLID);
			pGO->AddDrawBox(clrBox);
		}

		// 검사
		for (int i = 0; i < BreakROI.size(); ++i)
		{
			UINT nResultTemp = StatusOK;

			if ((nResultTemp = DetectBreakAndLIft(BreakROI[i], pGO, stLift.nSelectUnit, nViewIndex, nBreakWidth, nBreakHeight, nLiftHeight, i)) != StatusOK)
			{
				clrBox.CreateObject(PDC_RED, rcBreakAllRect[i], PS_SOLID, 3);
				pGO->AddDrawBox(clrBox);
			}

			nTrayStatus |= nResultTemp;
		}

		// 결과 표시
		CString strText;
		switch (nTrayStatus)
		{
		case StatusLift:		(CLanguageInfo::Instance()->m_nLangType == 0) ? strText = _T("NG : 들뜸") : strText = _T("NG : Lift");						bCheck = FALSE;		break;
		case StatusBreak:		(CLanguageInfo::Instance()->m_nLangType == 0) ? strText = _T("NG : 깨짐") : strText = _T("NG : Break");						bCheck = FALSE;		break;
		case StatusAll:			(CLanguageInfo::Instance()->m_nLangType == 0) ? strText = _T("NG : 들뜸&깨짐") : strText = _T("NG : Lift and Break");		bCheck = FALSE;		break;
//		default:					strText = _T("OK");							bCheck = TRUE;			break;
		}

		clrText.SetText(strText);
		clrText.CreateObject(bCheck ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 6, 70, TRUE);
		pGO->AddDrawText(clrText);
	}
	
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return bCheck;
}

BOOL CInspectionVision::BuildUsingEasyObject_SubMaterial(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect& rcTray, int nInspZoneCol, int nSubMaterialPixelVal,
														int nRatio1, int nRatio2, int nRatio3, int nMargin, int nTrayOutTolerance)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	COLORBOX clrBox;

	clrBox.CreateObject(PDC_LIGHTGREEN, rcTray, PS_DASH, 2);
	pGO->AddDrawBox(clrBox);

	EImageBW8 eImageX;
	eImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

	int nOffsetX = rcTray.left;
	int nOffsetY = rcTray.top - nMargin;
	int nInspZoneWidth, nInspZoneHeight;
	nInspZoneHeight = rcTray.Height() + nMargin * 2;

	std::vector<EROIBW8> vecEROI;
	vecEROI.clear();

	std::vector<int> vecRatio;
	vecRatio.clear();
	vecRatio.push_back(nRatio1);
	vecRatio.push_back(nRatio2);
	vecRatio.push_back(nRatio3);

	for (int i = 0; i < nInspZoneCol; ++i)
	{
		nInspZoneWidth = GetRoundShort(rcTray.Width() * (static_cast<double>(vecRatio[i]) / 100));
		EROIBW8 eImageROI;
		eImageROI.Attach(&eImageX);
		eImageROI.SetPlacement(nOffsetX, nOffsetY, nInspZoneWidth, nInspZoneHeight);

		vecEROI.push_back(eImageROI);
		nOffsetX += nInspZoneWidth;
	}
	nOffsetY += nInspZoneHeight;
	nOffsetX = rcTray.left;
	
	BOOL bResult = FindSubMaterialObj(pGO, vecEROI, nSubMaterialPixelVal, nTrayOutTolerance);

	return bResult;
}

BOOL CInspectionVision::BuildUsingEasyObject_HICDisColor(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, BOOL nKindOfObj, BOOL bColor, int nThreshold/*=0*/, BOOL bDraw/*= TRUE*/)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stHICInfo& stHICInfo = CModelInfo::Instance()->GetHICInfo();
	int nMin = __min(stHICInfo.nCircleOuterDiameter60, stHICInfo.nCircleOuterDiameter10);
	nMin = __min(nMin, stHICInfo.nCircleOuterDiameter5);

	int nMax = __max(stHICInfo.nCircleOuterDiameter60, stHICInfo.nCircleOuterDiameter10);
	nMax = __max(nMax, stHICInfo.nCircleOuterDiameter5);

	nMin -= 30;
	nMax += 30;

	COLORBOX clrBox;
	COLORPOINT clrPoint;
	COLORTEXT clrText;

	CRect rcRoi;
	rcRoi = rcRect;

	try
	{
		EImageBW8 ImageX;
		EImageC24 ImageX24;
		if (bColor)
		{
			ImageX24.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
			ImageX.SetSize(&ImageX24);
			EasyColor::GetComponent(&ImageX24, &ImageX, 0);
		}
		else
		{
			ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
		}

		EROIBW8 ImageROI;
		ImageROI.Attach(&ImageX);
		ImageROI.SetPlacement(rcRoi.left, rcRoi.top, rcRoi.Width(), rcRoi.Height());

		EListItem* pObject;
		CRect rcBlobRect;

		int		nOffsetX, nOffsetY, nArea;
		float	fWidth, fHeight, fCenterX, fCenterY;

		nOffsetX = 0;
		nOffsetY = 0;
		fWidth = fHeight = fCenterX = fCenterY = 0.0f;
		nArea = 0;

		ECodedImage CodedImage;

		if (nKindOfObj == OBJ_BLACK)
		{
			CodedImage.SetWhiteClass(0);
			CodedImage.SetBlackClass(1);
		}
		else if (nKindOfObj == OBJ_WHITE)
		{
			CodedImage.SetWhiteClass(1);
			CodedImage.SetBlackClass(0);
		}

		if (nThreshold == 0) nThreshold = EasyImage::AutoThreshold(&ImageROI, EThresholdMode_MinResidue, 0.5).Value;
		else if (nThreshold == 255) nThreshold = EasyImage::AutoThreshold(&ImageROI, EThresholdMode_MaxEntropy, 0.5).Value;

		CodedImage.SetThreshold(nThreshold);
		CodedImage.BuildObjects(&ImageROI);

		int nObjNum = CodedImage.GetNumObjects();
		if (nObjNum <= 0)
			return FALSE;

		CodedImage.AnalyseObjects(ELegacyFeature_Area, ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight, ELegacyFeature_LimitCenterX, ELegacyFeature_LimitCenterY);
		CodedImage.SelectObjectsUsingFeature(ELegacyFeature_LimitWidth, static_cast<float>(nMin), static_cast<float>(nMax), ESelectOption_RemoveOutOfRange);
		CodedImage.SelectObjectsUsingFeature(ELegacyFeature_LimitHeight, static_cast<float>(nMin), static_cast<float>(nMax), ESelectOption_RemoveOutOfRange);
		CodedImage.SetConnexity(EConnexity_Connexity8);

		nObjNum = CodedImage.GetNumSelectedObjects();
		// HIC 원형 3개
		if (nObjNum != 3)	return FALSE;

		pObject = CodedImage.GetFirstObjPtr();
		for (int i = 0; i < nObjNum; ++i)
		{
			CodedImage.GetObjectFeature(ELegacyFeature_LimitWidth, pObject, fWidth);
			CodedImage.GetObjectFeature(ELegacyFeature_LimitHeight, pObject, fHeight);
			CodedImage.GetObjectFeature(ELegacyFeature_LimitCenterX, pObject, fCenterX);
			CodedImage.GetObjectFeature(ELegacyFeature_LimitCenterY, pObject, fCenterY);
			CodedImage.GetObjectFeature(ELegacyFeature_Area, pObject, nArea);

			rcBlobRect.left = nOffsetX + (int)GetRoundLong(fCenterX - (fWidth / 2.f));
			rcBlobRect.top = nOffsetY + (int)GetRoundLong(fCenterY - (fHeight / 2.f));
			rcBlobRect.right = nOffsetX + (int)GetRoundLong(fCenterX + (fWidth / 2.f));
			rcBlobRect.bottom = nOffsetY + (int)GetRoundLong(fCenterY + (fHeight / 2.f));
			rcBlobRect.NormalizeRect();
			
			m_rcHicCenter[i].left		= rcRoi.left	+ rcBlobRect.left;
			m_rcHicCenter[i].top		= rcRoi.top		+ rcBlobRect.top;
			m_rcHicCenter[i].right		= rcRoi.left	+ rcBlobRect.right;
			m_rcHicCenter[i].bottom		= rcRoi.top		+ rcBlobRect.bottom;

			pObject = CodedImage.GetNextObjPtr(pObject);

			if (bDraw)
			{
				clrBox.CreateObject(PDC_GREEN, m_rcHicCenter[i], PS_SOLID, 2);
				pGO->AddDrawBox(clrBox);
			}
		}

		std::sort(std::begin(m_rcHicCenter), std::end(m_rcHicCenter), CompareCenterX); // CompareWidth);
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return TRUE;
}

BOOL CInspectionVision::BuildUsingEasyObject_ForLabelEdge( CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, BOOL nKindOfObj, int nThreshold )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	COLORBOX clrBox;

	EImageBW8 ImageX;
	ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes()*8);

	rcRect.NormalizeRect();

	EROIBW8 ImageROI;
	ImageROI.Attach( &ImageX );
	ImageROI.SetPlacement( rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height() );

	int nOffsetX, nOffsetY, nArea;
	float fWidth, fHeight, fCenterX, fCenterY;

	nOffsetX = rcRect.left;
	nOffsetY = rcRect.top;
	nArea	 = 0;
	fWidth = fHeight = fCenterX = fCenterY = 0.0f;

	ECodedImage CodedImage;
	EListItem* pObject;

	if( nKindOfObj == OBJ_BLACK ) 
	{
		CodedImage.SetWhiteClass( 0 );
		CodedImage.SetBlackClass( 1 );
	}
	else if( nKindOfObj == OBJ_WHITE )
	{
		CodedImage.SetWhiteClass( 1 );
		CodedImage.SetBlackClass( 0 );
	}

	if( nThreshold == 0 ) nThreshold = EasyImage::AutoThreshold( &ImageROI, EThresholdMode_MinResidue, 0.5).Value;
	CodedImage.SetThreshold( nThreshold );
	CodedImage.BuildObjects( &ImageROI	);

	int nObjNum;
	nObjNum = CodedImage.GetNumObjects();
	if( nObjNum <= 0) return TRUE;

	CodedImage.AnalyseObjects( ELegacyFeature_Area, ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight, ELegacyFeature_LimitCenterX, ELegacyFeature_LimitCenterY	);
	CodedImage.SelectObjectsUsingFeature( ELegacyFeature_Area, 0, 5, ESelectOption_RemoveRange );
	CodedImage.SetConnexity( EConnexity_Connexity8 );

	nObjNum = CodedImage.GetNumSelectedObjects();
	if( nObjNum <= 0 ) return TRUE;

	pObject = CodedImage.GetFirstObjPtr();
	for( int i=0; i<nObjNum; ++i )
	{
		CodedImage.GetObjectFeature( ELegacyFeature_LimitWidth,			pObject, fWidth		);
		CodedImage.GetObjectFeature( ELegacyFeature_LimitHeight,		pObject, fHeight	);
		CodedImage.GetObjectFeature( ELegacyFeature_LimitCenterX,		pObject, fCenterX	);
		CodedImage.GetObjectFeature( ELegacyFeature_LimitCenterY,		pObject, fCenterY	);
		CodedImage.GetObjectFeature( ELegacyFeature_Area,				pObject ,nArea		);

		CRect reArea;
		reArea.left   = nOffsetX + (int)GetRoundLong(fCenterX - (fWidth  / 2.f));
		reArea.top    = nOffsetY + (int)GetRoundLong(fCenterY - (fHeight / 2.f));
		reArea.right  = nOffsetX + (int)GetRoundLong(fCenterX + (fWidth  / 2.f));
		reArea.bottom = nOffsetY + (int)GetRoundLong(fCenterY + (fHeight / 2.f));
		reArea.NormalizeRect();

		clrBox.CreateObject( PDC_LIGHTRED, reArea, PS_SOLID, 2 );
		pGO->AddDrawBox( clrBox );

		pObject = CodedImage.GetNextObjPtr( pObject );
	}

	return FALSE;
}

BOOL CInspectionVision::BuildUsingEasyObject_DesiccantCutting(CxGraphicObject *pGO, CxImageObject* pImgObj, int nViewIndex, CRect rcInspArea, int nDesiccantPixelValue, 
																int nSealingLength, int nColorLineLengthDifference)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CString strException = _T("");
	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORARROW clrArrow;
	constexpr int nAreaThreshold = 10000;

	try
	{
		EImageBW8 eImageX;
		eImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		EROIBW8 eLightROI;
		eLightROI.Attach(&eImageX);
		eLightROI.SetPlacement(rcInspArea.left, rcInspArea.top, rcInspArea.Width(), rcInspArea.Height());

		clrBox.CreateObject(PDC_ORANGE, rcInspArea, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

		ECodedImage2 CodedImage;
		EObjectSelection CodedImageObjectSelection;
		EImageEncoder CodedImageEncoder;

		// Blob으로 Desiccant 찾기
		CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_GrayscaleSingleThreshold);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_Absolute);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nDesiccantPixelValue);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(true);
		CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);
		CodedImageEncoder.Encode(eLightROI, CodedImage);
		
		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjectsUsingUnsignedIntegerFeature(CodedImage, EFeature_Area, nAreaThreshold, ESingleThresholdMode_GreaterEqual);
		CodedImageObjectSelection.Sort(EFeature_Area, ESortDirection_Descending);
		
		int nNumCnt = CodedImageObjectSelection.GetElementCount();

		if (nNumCnt==0)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("방습제 찾지 못함");
			else												strException = _T("No Desiccant");

			throw strException;
		}

		CRect rcRect, rcArea;
		constexpr int nOffset = 5;
		for (int i = 0; i < nNumCnt; i++)
		{
			ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(i);
			rcRect.left = eCodedElement.GetLeftLimit();
			rcRect.top = eCodedElement.GetTopLimit();
			rcRect.right = eCodedElement.GetRightLimit();
			rcRect.bottom = eCodedElement.GetBottomLimit();
			rcRect.NormalizeRect();

			if (i == 0) rcArea = rcRect;
			else
			{
				if (rcRect.left < rcArea.left) rcArea.left = rcRect.left;
				if (rcRect.top < rcArea.top) rcArea.top = rcRect.top;
				if (rcRect.right > rcArea.right) rcArea.right = rcRect.right;
				if (rcRect.bottom > rcArea.bottom) rcArea.bottom = rcRect.bottom;

				rcArea.NormalizeRect();
			}
		}
		
		// Desiccant ROI
		rcArea.left += eLightROI.GetOrgX() + nOffset;
		rcArea.top += eLightROI.GetOrgY();
		rcArea.right += eLightROI.GetOrgX() - nOffset;
		rcArea.bottom += eLightROI.GetOrgY();
		rcArea.NormalizeRect();

		clrBox.CreateObject(PDC_GREEN, rcArea, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);
		
		EROIBW8 eDesiccantROI;
		eDesiccantROI.Attach(&eImageX);
		eDesiccantROI.SetPlacement(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height());

		constexpr int nLineThreshold = 20;

		// Left Sealing Line Gauge
		DPOINT dptGaugeCenter;
		dptGaugeCenter.x	= static_cast<double>(eDesiccantROI.GetOrgX() + (eDesiccantROI.GetWidth() / 4));	// Desiccant 왼쪽 1/4 지점
		dptGaugeCenter.y	= static_cast<double>(eDesiccantROI.GetOrgY() + eDesiccantROI.GetHeight()/ 2);		// Desiccant 세로 중앙
		float fTolerance	= static_cast<float>(abs(dptGaugeCenter.y - eDesiccantROI.GetOrgY()));				// Desiccant 중앙부터 Desiccant Top까지 Line Gauge Tolerance 설정
		float fLength		= static_cast<float>(eDesiccantROI.GetWidth() / 4);

		DPOINT dptLeftSealingTop;
		double dLeftSealingTopResultAngle;
		if(!LineGauge2(eImageX, ETransitionType::ETransitionType_Bw, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, fTolerance, fLength,
						0, nLineThreshold, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptLeftSealingTop, dLeftSealingTopResultAngle))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("좌측 실링 윗 부분 에러"));
			else												strException.Format(_T("Left Sealing Top Error"));
			throw strException;
		}
		
		float fTolerance2 = static_cast<float>(abs(rcInspArea.bottom - eDesiccantROI.GetOrgY()) / 2);
		DPOINT dptLeftSealingBottom;
		double dLeftSealingBottomResultAngle;
		if (!LineGauge2(eImageX, ETransitionType::ETransitionType_Wb, ETransitionChoice::ETransitionChoice_NthFromEnd, dptGaugeCenter, fTolerance2, fLength,
						0, nLineThreshold, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptLeftSealingBottom, dLeftSealingBottomResultAngle))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("좌측 실링 밑 부분 에러"));
			else												strException.Format(_T("Left Sealing Bottom Error"));
			throw strException;
		}

		// Right Sealing Line Gauge
		dptGaugeCenter.x = static_cast<double>(eDesiccantROI.GetOrgX() + (eDesiccantROI.GetWidth() / 4) * 3);
		dptGaugeCenter.y = static_cast<double>(eDesiccantROI.GetOrgY() + eDesiccantROI.GetHeight() / 2);

		DPOINT dptRightSealingTop;
		double dRightSealingTopResultAngle;
		if (!LineGauge2(eImageX, ETransitionType::ETransitionType_Bw, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, fTolerance, fLength,
						0, nLineThreshold, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptRightSealingTop, dRightSealingTopResultAngle))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("우측 실링 윗 부분 에러"));
			else												strException.Format(_T("Right Sealing Top Error"));
			throw strException;
		}

		DPOINT dptRightSealingBottom;
		double dRightSealingBottomResultAngle;
		if (!LineGauge2(eImageX, ETransitionType::ETransitionType_Wb, ETransitionChoice::ETransitionChoice_NthFromEnd, dptGaugeCenter, fTolerance2, fLength,
						0, nLineThreshold, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptRightSealingBottom, dRightSealingBottomResultAngle))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("우측 실링 밑 부분 에러"));
			else												strException.Format(_T("Right Sealing Bottom Error"));
			throw strException;
		}

		// Left Color Line Gague
		dptGaugeCenter.x = dptLeftSealingTop.x;
		dptGaugeCenter.y = dptLeftSealingTop.y;

		DPOINT dptLeftColorLine;
		double dLeftColorLineAngle;
		if(!LineGauge2(eImageX, ETransitionType::ETransitionType_Wb, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, fTolerance, fLength,
						0, nLineThreshold, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptLeftColorLine, dLeftColorLineAngle))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("좌측 컬러 라인 에러"));
			else												strException.Format(_T("Left Color Error"));
			throw strException;
		}

		// Right Color Line Gauge
		dptGaugeCenter.x = dptRightSealingTop.x;
		dptGaugeCenter.y = dptRightSealingTop.y;

		DPOINT dptRightColorLine;
		double dRightColorLineAngle;
		if (!LineGauge2(eImageX, ETransitionType::ETransitionType_Wb, ETransitionChoice::ETransitionChoice_NthFromBegin, dptGaugeCenter, fTolerance, fLength,
						0, nLineThreshold, 1, 5, 1, 1, 1.5f, 10.0f, FALSE, FALSE, TRUE, dptRightColorLine, dRightColorLineAngle))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("우측 컬러 라인 에러"));
			else												strException.Format(_T("Right Color Error"));
			throw strException;
		}
		// Sealing 길이 검사 & Graphic Display
		int nLeftSealingLength = static_cast<int>(dptLeftSealingBottom.y - dptLeftSealingTop.y);
		BOOL bCheckLeftSealingLength = nLeftSealingLength >= nSealingLength ? TRUE : FALSE;
		dptLeftSealingBottom.x = dptLeftSealingTop.x;
		clrArrow.CreateObject(bCheckLeftSealingLength ? PDC_GREEN : PDC_RED, dptLeftSealingTop, dptLeftSealingBottom, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 7);
		pGO->AddDrawArrow(clrArrow);

		clrText.SetText(_T("%.1lf mm"), (nLeftSealingLength * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(bCheckLeftSealingLength ? PDC_GREEN : PDC_RED, GetRoundShort(dptLeftSealingBottom.x), GetRoundShort(dptLeftSealingBottom.y), 25, TRUE);
		pGO->AddDrawText(clrText);

		int nRightSealingLength = static_cast<int>(dptRightSealingBottom.y - dptRightSealingTop.y);
		BOOL bCheckRightSealingLength = nRightSealingLength >= nSealingLength ? TRUE : FALSE;
		dptRightSealingBottom.x = dptRightSealingTop.x;
		clrArrow.CreateObject(bCheckRightSealingLength ? PDC_GREEN : PDC_RED, dptRightSealingTop, dptRightSealingBottom, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 7);
		pGO->AddDrawArrow(clrArrow);

		clrText.SetText(_T("%.1lf mm"), (nRightSealingLength * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(bCheckRightSealingLength ? PDC_GREEN : PDC_RED, GetRoundShort(dptRightSealingTop.x), GetRoundShort(dptRightSealingBottom.y), 25, TRUE);
		pGO->AddDrawText(clrText);

		// ColorLine 길이 검사 & Graphic Display
		int nLeftColorLineLength = static_cast<int>(dptLeftSealingTop.y - dptLeftColorLine.y);
		int nRightColorLineLength = static_cast<int>(dptRightSealingTop.y - dptRightColorLine.y);
		BOOL bCheckColorLength = abs(nLeftColorLineLength - nRightColorLineLength) > nColorLineLengthDifference ? FALSE : TRUE;

		clrText.SetText(_T("%.1lf mm"), (nLeftColorLineLength * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(PDC_GREEN, GetRoundShort(dptLeftColorLine.x), GetRoundShort(dptLeftColorLine.y), 25, TRUE);
		pGO->AddDrawText(clrText);

		clrText.SetText(_T("%.1lf mm"), (nRightColorLineLength * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(PDC_GREEN, GetRoundShort(dptRightColorLine.x), GetRoundShort(dptRightColorLine.y), 25, TRUE);
		pGO->AddDrawText(clrText);

		dptLeftSealingTop.x -= 30;
		dptLeftColorLine.x = dptLeftSealingTop.x;
		clrArrow.CreateObject(bCheckColorLength ? PDC_GREEN : PDC_RED, dptLeftColorLine, dptLeftSealingTop, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 7);
		pGO->AddDrawArrow(clrArrow);

		dptRightSealingTop.x += 30;
		dptRightColorLine.x = dptRightSealingTop.x;
		clrArrow.CreateObject(bCheckColorLength ? PDC_GREEN : PDC_RED, dptRightColorLine, dptRightSealingTop, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 7);
		pGO->AddDrawArrow(clrArrow);

		// 결과 Display
		int nTextX = DEF_FONT_BASIC_POSI;
		int nTextY = DEF_FONT_BASIC_POSI * 2;
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("좌측 실링 길이 : %.1lf mm"), (nLeftSealingLength * m_fCalY[nViewIndex]) / 1000.0);
		else												clrText.SetText(_T("Left Sealing Length : %.1lf mm"), (nLeftSealingLength * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(bCheckLeftSealingLength ? PDC_GREEN : PDC_RED, nTextX, nTextY, 25, TRUE);
		pGO->AddDrawText(clrText);

		nTextY += DEF_FONT_BASIC_POSI;
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("우측 실링 길이 : %.1lf mm"), (nRightSealingLength * m_fCalY[nViewIndex]) / 1000.0);
		else												clrText.SetText(_T("Right Sealing Length : %.1lf mm"), (nRightSealingLength * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(bCheckRightSealingLength ? PDC_GREEN : PDC_RED, nTextX, nTextY, 25, TRUE);
		pGO->AddDrawText(clrText);

		nTextY += DEF_FONT_BASIC_POSI;
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("컬러 라인 좌우 길이 차이 : %.1lf mm"), (abs(nLeftColorLineLength - nRightColorLineLength) * m_fCalY[nViewIndex]) / 1000.0);
		else												clrText.SetText(_T("Color Line Left And Right Length Diff : %.1lf mm"), (abs(nLeftColorLineLength - nRightColorLineLength) * m_fCalY[nViewIndex]) / 1000.0);
		clrText.CreateObject(bCheckColorLength ? PDC_GREEN : PDC_RED, nTextX, nTextY, 25, TRUE);
		pGO->AddDrawText(clrText);

		return bCheckLeftSealingLength && bCheckRightSealingLength && bCheckColorLength;
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString& e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, 30, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}
	return TRUE;
}

BOOL CInspectionVision::BuildUsingEasyObject_DesiccantPosition(CxGraphicObject *pGO, CxImageObject* pImgObj, int nViewIndex, CModelInfo::stDesiccantCuttingInfo& stDesiccant, BOOL m_bPreview)
{
#ifdef RELEASE_3G
	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORARROW clrArrow;
	COLORLINE clrLine;
	COLORCROSS clrCross;

	CString strException				= _T("");
	CString strFormat					= _T("");
	const CRect rcInspArea				= nViewIndex == IDX_AREA1 ? stDesiccant.rcPositionLeftInspArea : stDesiccant.rcPositionRightInspArea;
	const int nDesiccantThreshold		= stDesiccant.nDesiccantPixelValue;
	const int nDesiccantWidth			= static_cast<int>((stDesiccant.dDesiccantWidth * 1000.0) / m_fCalX[nViewIndex]);
	const int nDesiccantHeight			= static_cast<int>((stDesiccant.dDesiccantHeight * 1000.0) / m_fCalY[nViewIndex]);
	const int nROIWidth					= stDesiccant.nROIWidth;
	const int nROIHeight				= stDesiccant.nROIHeight;
	const int nColorLineThreshold		= stDesiccant.nColorLineThreshold;
	const int nOffsetLeft				= stDesiccant.nRoiOffsetLeft;
	const int nOffsetRight				= stDesiccant.nRoiOffsetRight;
	const int nOpenWidth				= stDesiccant.nOpenBoxWidth;
	const int nOpenHeight				= stDesiccant.nOpenBoxHeight;
	const double dColorLineLengthDiff	= stDesiccant.dPosColorLineLengthDiff;

	try
	{
		if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
			return FALSE;

		DPOINT dptResult;
		BOOL bCheck = FALSE;
		double dDistanceY;
		double dDistanceLeftY, dDistanceRightY;

		EImageBW8 eImageX;
		eImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		EROIBW8 eDesiccantROI;
		eDesiccantROI.Attach(&eImageX);
		eDesiccantROI.SetPlacement(rcInspArea.left, rcInspArea.top, rcInspArea.Width(), rcInspArea.Height());

		ECodedImage2 CodedImage;
		EImageEncoder CodedImageEncoder;
		EObjectSelection CodedImageObjectSelection;

		// Desiccant 찾기
		CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_GrayscaleSingleThreshold);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nDesiccantThreshold);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);
		CodedImageEncoder.Encode(eDesiccantROI, CodedImage);

		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjects(CodedImage);
		CodedImageObjectSelection.Sort(EFeature_Area, ESortDirection_Descending);

		ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(0);
		int nElementWidth = static_cast<int>(eCodedElement.GetBoundingBoxWidth());
		int nElementHeight = static_cast<int>(eCodedElement.GetBoundingBoxHeight());

		CRect rcDesiccant;
		rcDesiccant.left = eCodedElement.GetLeftLimit() + eDesiccantROI.GetOrgX();
		rcDesiccant.top = eCodedElement.GetTopLimit() + eDesiccantROI.GetOrgY();
		rcDesiccant.right = eCodedElement.GetRightLimit() + eDesiccantROI.GetOrgX();
		rcDesiccant.bottom = eCodedElement.GetBottomLimit() + eDesiccantROI.GetOrgY();;

		BOOL bDesiccant = nElementWidth >= nDesiccantWidth && nElementHeight >= nDesiccantHeight ? TRUE : FALSE;
		clrBox.CreateObject(bDesiccant ? PDC_GREEN : PDC_RED, rcDesiccant, PS_DASH, 2);
		pGO->AddDrawBox(clrBox);

		if (!bDesiccant)
		{
			switch (nViewIndex)
			{
			case InspectTypePositionLeft:
				CVisionSystem::Instance()->m_bNgRetryLeft = FALSE;
				break;
			case InspectTypePositionRight:
				CVisionSystem::Instance()->m_bNgRetryRight = FALSE;
				break;
			}
			
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("검사 에러 : 방습제 크기");
			else												strException = _T("Inspection Error : Desiccant Size");
			throw strException;
		}

		const int nKnifePos		= nViewIndex == IDX_AREA1 ? 190 : 185; // 칼날위치 Left 검사일 때 190, Right 검사일 때 185
		const int nROITop		= nKnifePos + 5; // ROI Top 칼날보다 아래로
		const int nROIBottom	= nROITop + nROIHeight;

		DPOINT dptLeftClrLine; // Left ColorLine ROI(Inspection Box)
		DPOINT dptRightClrLine; // Left ColorLine ROI
		DPOINT dptROICenter;
		int nOrgXLeft, nOrgXRight;

		for (int i = 0; i < 2; ++i)
		{
			CRect rcInspROI;
			switch (i)
			{
			case 0: // left
				rcInspROI.left		= rcDesiccant.left + nOffsetLeft;
				rcInspROI.top		= nROITop;
				rcInspROI.right		= rcInspROI.left + nROIWidth;
				rcInspROI.bottom	= nROIBottom;
				break;
			case 1: // right
				rcInspROI.left		= rcDesiccant.right - nOffsetRight - nROIWidth;
				rcInspROI.top		= nROITop;
				rcInspROI.right		= rcInspROI.left + nROIWidth;
				rcInspROI.bottom	= nROIBottom;
				break;
			}
			EROIBW8 eImageROI;
			eImageROI.Attach(&eImageX);
			eImageROI.SetPlacement(rcInspROI.left, rcInspROI.top, rcInspROI.Width(), rcInspROI.Height());

			clrBox.CreateObject(PDC_ORANGE, rcInspROI, PS_SOLID, 2);
			pGO->AddDrawBox(clrBox);

			EImageBW8 eImageROI_copy;
			eImageROI_copy.SetSize(&eImageROI);
			EasyImage::Oper(EArithmeticLogicOperation_Copy, &eImageROI, &eImageROI_copy);
			if (!m_bPreview)
			{
				//EasyImage::GainOffset(&eImageROI_copy, &eImageROI_copy, fROIGain, 0.0f);
				EasyImage::OpenBox(&eImageROI_copy, &eImageROI_copy, nOpenWidth, nOpenHeight);
			}
			// Desiccant Color Line 찾기
			CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_GrayscaleSingleThreshold);
			CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_Absolute);
			CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nColorLineThreshold);
			CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(false);
			CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(true);
			CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);
			CodedImageEncoder.Encode(eImageROI_copy, CodedImage);

			CodedImageObjectSelection.Clear();
			CodedImageObjectSelection.AddObjectsUsingFloatFeature(CodedImage, EFeature_BoundingBoxWidth, float(rcInspROI.Width() * 0.9f), ESingleThresholdMode_GreaterEqual);
			CodedImageObjectSelection.Sort(EFeature_BoundingBoxHeight, ESortDirection_Descending);

			int nCount = CodedImageObjectSelection.GetElementCount();
			if (!nCount)  // Color Line 못 찾음
			{
				switch (nViewIndex)
				{
				case InspectTypePositionLeft:
					CVisionSystem::Instance()->m_bNgRetryLeft = TRUE;
					break;
				case InspectTypePositionRight:
					CVisionSystem::Instance()->m_bNgRetryRight = TRUE;
					break;
				}

				switch (i)
				{
				case 0: // left
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("검사 에러 : 좌측 색상 라인 없음");
					else												strException = _T("Inspection Error : No Desiccant Color Line (Left)");
					break;
				case 1: // right
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("검사 에러 : 우측 색상 라인 없음");
					else												strException = _T("Inspection Error : No Desiccant Color Line (Right)");
					break;
				}
				throw strException;
			}
			constexpr int nClrLineHeight = 15;

			CRect rcColorLine;
			rcColorLine.SetRectEmpty();
			if (CodedImageObjectSelection.GetElement(0).GetBoundingBoxHeight() >= nClrLineHeight)
			{
				rcColorLine.left = CodedImageObjectSelection.GetElement(0).GetLeftLimit();
				rcColorLine.top = CodedImageObjectSelection.GetElement(0).GetTopLimit();
				rcColorLine.right = CodedImageObjectSelection.GetElement(0).GetRightLimit();
				rcColorLine.bottom = CodedImageObjectSelection.GetElement(0).GetBottomLimit();
			}

			// 디버깅용
			//CPoint pt1(eImageROI.GetOrgX() + rcColorLine.left, eImageROI.GetOrgY() + rcColorLine.CenterPoint().y);
			//CPoint pt2(eImageROI.GetOrgX() + rcColorLine.right, eImageROI.GetOrgY() + rcColorLine.CenterPoint().y);

			//clrArrow.CreateObject(PDC_BLUE, pt1, pt2, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 7);
			//pGO->AddDrawArrow(clrArrow);

			dptResult.x = eImageROI.GetOrgX() + static_cast<double>(rcColorLine.CenterPoint().x);
			dptResult.y = eImageROI.GetOrgY() + static_cast<double>(rcColorLine.bottom);

			if (rcColorLine.Width() <= static_cast<int>(rcInspROI.Width() * 0.95))
			{
				switch (nViewIndex)
				{
				case InspectTypePositionLeft:
					CVisionSystem::Instance()->m_bNgRetryLeft = TRUE;
					break;
				case InspectTypePositionRight:
					CVisionSystem::Instance()->m_bNgRetryRight = TRUE;
					break;
				}
				if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("색상 라인이 감지되지 않거나 너무 짧습니다");
				else												strException = _T("Color Line doesn't detected or too short");
				throw strException;
			}

			clrCross.CreateObject(PDC_GREEN, dptResult, 5, PS_SOLID, 3);
			pGO->AddDrawCross(clrCross);

			int nDistanceY = static_cast<int>(dptResult.y) - nKnifePos;
			double dDistanceY = nDistanceY * (m_fCalX[nViewIndex] / 1000);
			
			switch (i)
			{
			case 0:
				dDistanceLeftY = dDistanceY;
				dptLeftClrLine = dptResult;
				nOrgXLeft = eImageROI.GetOrgX();
				break;
			case 1:
				dDistanceRightY = dDistanceY;
				dptRightClrLine = dptResult;
				nOrgXRight = eImageROI.GetOrgX();
				break;
			}
			dptROICenter.x = rcColorLine.CenterPoint().x + eImageROI.GetOrgX();
			dptROICenter.y = static_cast<double>(nKnifePos);
			clrArrow.CreateObject(PDC_GREEN, dptROICenter, dptResult, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 5, PS_SOLID);
			pGO->AddDrawArrow(clrArrow);

			strFormat.Format(_T("%.3lfmm"), dDistanceY);
			clrText.SetText(strFormat);
			clrText.CreateObject(PDC_GREEN, GetRoundShort(dptResult.x), GetRoundShort(dptResult.y), 15, TRUE);
			pGO->AddDrawText(clrText);
		}
		clrLine.CreateObject(PDC_BLUE, dptLeftClrLine, dptRightClrLine, PS_DASH, 2);
		pGO->AddDrawLine(clrLine);

		DPOINT dptCenter, dptKnife;
		dptCenter.x = (dptLeftClrLine.x + dptRightClrLine.x) / 2.0;
		dptCenter.y = (dptLeftClrLine.y + dptRightClrLine.y) / 2.0;

		if (abs(dDistanceLeftY - dDistanceRightY) > dColorLineLengthDiff)
		{
			switch (nViewIndex)
			{
			case InspectTypePositionLeft:				CVisionSystem::Instance()->m_bNgRetryLeft = TRUE;				break;
			case InspectTypePositionRight:				CVisionSystem::Instance()->m_bNgRetryRight = TRUE;				break;
			}

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("좌우 길이 차이 > %.1lfmm"), dColorLineLengthDiff);
			else												strException.Format(_T("Left Right Length Diff > %.1lfmm"), dColorLineLengthDiff);
			throw strException;
		}

		dptKnife.x = (dptLeftClrLine.x  + dptRightClrLine.x) / 2.0;
		dptKnife.y = static_cast<double>(nKnifePos);

		dDistanceY = (dDistanceLeftY + dDistanceRightY) / 2.0;

		clrArrow.CreateObject(PDC_GREEN, dptCenter, dptKnife, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		strFormat.Format(_T("%.3lfmm"), dDistanceY);
		clrText.SetText(strFormat);
		clrText.CreateObject(PDC_GREEN, GetRoundShort(dptKnife.x), GetRoundShort(dptKnife.y), 20, TRUE);
		pGO->AddDrawText(clrText);

		if (nViewIndex == IDX_AREA1) 	CVisionSystem::Instance()->m_fDesiPosDistLeft = static_cast<float>(dDistanceY);
		else							CVisionSystem::Instance()->m_fDesiPosDistRight = static_cast<float>(dDistanceY);

		return TRUE;
	}

	catch (EException& e)
	{
		return DisplayEException(pGO, e, 30);
	}
	catch (CString& e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, 30, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CInspectionVision::BuildUsingEasyObject_BoxTape(CxGraphicObject* pGO, CxImageObject *pImgObj, CRect& rcBlobRect, int nRedThreshold, int nGreenThrehsold, int nBlueThreshold, int nTapeWidth, int nTapeHeight)
{
	BOOL bCheck = FALSE;
	CString strException = _T("");

	COLORBOX clrBox;
	COLORARROW clrArrow;
	COLORTEXT clrText;

	try
	{
		if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
			return FALSE;

		CRect rcRect;
		rcRect.left = 0;
		rcRect.right = pImgObj->GetWidth();
		rcRect.top = 0;
		rcRect.bottom = pImgObj->GetHeight();
		rcRect.NormalizeRect();

		clrBox.CreateObject(PDC_ORANGE, rcRect, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

		EImageC24 ImageX;
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		constexpr int nAreaSelection = 10000;
		ECodedImage2 CodedImage;
		EImageEncoder CodedImageEncoder;
		EObjectSelection CodedImageObjectSelection;

		CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_ColorSingleThreshold);
		CodedImageEncoder.GetColorSingleThresholdSegmenter().SetThreshold(EC24(nRedThreshold, nGreenThrehsold, nBlueThreshold));
		CodedImageEncoder.GetColorSingleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.GetColorSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
		CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);
		CodedImageEncoder.Encode(ImageX, CodedImage);
	
		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjectsUsingUnsignedIntegerFeature(CodedImage, EFeature_Area, nAreaSelection, ESingleThresholdMode_GreaterEqual);

		int nObjCnt = CodedImageObjectSelection.GetElementCount();
		if (!nObjCnt)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("테이프 없음");
			else												strException = _T("No Box Tape");
			throw strException;
		}

		ECodedElement& eCodedObj = CodedImageObjectSelection.GetElement(0);
		int nCenterX	= static_cast<int>(eCodedObj.GetBoundingBoxCenterX());
		int nCenterY	= static_cast<int>(eCodedObj.GetBoundingBoxCenterY());
		int nWidth		= static_cast<int>(eCodedObj.GetBoundingBoxWidth());
		int nHeight		= static_cast<int>(eCodedObj.GetBoundingBoxHeight());

		int nTapeLeft	= nCenterX - nWidth	 / 2;
		int nTapeRight	= nCenterX + nWidth	 / 2;
		int nTapeTop	= nCenterY - nHeight / 2;
		int nTapeBottom	= nCenterY + nHeight / 2;
		
		BOOL bWidthOK	= nWidth  > nTapeWidth  ? TRUE : FALSE;
		BOOL bHeightOK	= nHeight > nTapeHeight ? TRUE : FALSE;
		bCheck = bWidthOK && bHeightOK;

		clrArrow.CreateObject(bWidthOK ? PDC_GREEN : PDC_LIGHTRED, CPoint(nTapeLeft, nCenterY), CPoint(nTapeRight, nCenterY), CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 10);
		pGO->AddDrawArrow(clrArrow);

		clrArrow.CreateObject(bHeightOK ? PDC_GREEN : PDC_LIGHTRED, CPoint(nCenterX, nTapeTop), CPoint(nCenterX, nTapeBottom), CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 10);
		pGO->AddDrawArrow(clrArrow);

		clrBox.CreateObject(bCheck ? PDC_GREEN : PDC_LIGHTRED, nTapeLeft, nTapeTop, nTapeRight, nTapeBottom, PS_SOLID, 3);
		pGO->AddDrawBox(clrBox);

		CString strSize;
		strSize.Format(_T("Width: %dpx"), GetRoundShort(eCodedObj.GetBoundingBoxWidth()));
		clrText.SetText(strSize);
		clrText.CreateObject(bWidthOK ? PDC_GREEN : PDC_LIGHTRED, CPoint(nTapeRight + 30, nCenterY - 30), 20, TRUE);
		pGO->AddDrawText(clrText);

		strSize.Format(_T("Height: %dpx"), GetRoundShort(eCodedObj.GetBoundingBoxHeight()));
		clrText.SetText(strSize);
		clrText.CreateObject(bHeightOK ? PDC_GREEN : PDC_LIGHTRED, CPoint(nTapeRight + 30, nCenterY + 30), 20, TRUE);
		pGO->AddDrawText(clrText);

		rcBlobRect = CRect(nTapeLeft, nTapeTop, nTapeRight, nTapeBottom);
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString& e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, 40, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	return bCheck;
}

BOOL CInspectionVision::BuildUsingEasyObject_BoxTapeBtm(CxGraphicObject* pGO, CxImageObject *pImgObj, CRect rcRect)
{
	BOOL bCheck = TRUE;
	CString strException = _T("Bottom Side Tape OK");
	if (CLanguageInfo::Instance()->m_nLangType == 0)
		strException = _T("아랫면 테이프 검사 OK");

	COLORBOX clrBox;
	COLORARROW clrArrow;
	COLORTEXT clrText;

	try
	{
		if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
			return FALSE;

		CRect rcRoi;
		rcRoi = rcRect;
		rcRoi.NormalizeRect();

		clrBox.CreateObject(PDC_ORANGE, rcRoi, PS_SOLID, 1);
		pGO->AddDrawBox(clrBox);

		EImageC24 ImageX;
		ImageX.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
		
		EROIC24 EC24Roi;
		EC24Roi.Attach(&ImageX);
		EC24Roi.SetPlacement(rcRoi.left, rcRoi.top, rcRoi.Width(), rcRoi.Height());

		constexpr int nAreaSelection = 1000;
		ECodedImage2 CodedImage;
		EImageEncoder CodedImageEncoder;
		EObjectSelection CodedImageObjectSelection;

		CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_ColorSingleThreshold);
		CodedImageEncoder.GetColorSingleThresholdSegmenter().SetThreshold(EC24(160, 160, 160));
		CodedImageEncoder.GetColorSingleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.GetColorSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
		CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);
		CodedImageEncoder.Encode(EC24Roi, CodedImage);

		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjects(CodedImage);
		CodedImageObjectSelection.RemoveUsingUnsignedIntegerFeature(EFeature_Area, nAreaSelection, ESingleThresholdMode_Less);
		CodedImageObjectSelection.Sort(EFeature_Area, ESortDirection_Descending);

		int nObjCnt = CodedImageObjectSelection.GetElementCount();
		if (nObjCnt == 0)
			throw strException;

		ECodedElement& eCodedObj = CodedImageObjectSelection.GetElement(0);
		int nWidth = static_cast<int>(eCodedObj.GetBoundingBoxWidth());
		int nHeight = static_cast<int>(eCodedObj.GetBoundingBoxHeight());

		// Width, Height가 ROI 절반 이상이면 NG
		BOOL bWidthNG	= nWidth	> GetRoundShort(rcRoi.Width()/2)	? FALSE : TRUE;
		BOOL bHeightNG	= nHeight	> GetRoundShort(rcRoi.Height()/2)	? FALSE : TRUE;
		bCheck = bWidthNG && bHeightNG;

		CRect rcBlob;
		rcBlob.left = EC24Roi.GetOrgX() + eCodedObj.GetLeftLimit();
		rcBlob.top = EC24Roi.GetOrgY() + eCodedObj.GetTopLimit();
		rcBlob.right = EC24Roi.GetOrgX() + eCodedObj.GetRightLimit();
		rcBlob.bottom = EC24Roi.GetOrgY() + eCodedObj.GetBottomLimit();
		rcBlob.NormalizeRect();

		if (bCheck)
			throw strException;
		else
		{
			clrBox.CreateObject(PDC_LIGHTRED, rcBlob, PS_SOLID, 3);
			pGO->AddDrawBox(clrBox);

			CString strSize;
			strSize.Format(_T("Width: %dpx"), GetRoundShort(eCodedObj.GetBoundingBoxWidth()));
			clrText.SetText(strSize);
			clrText.CreateObject(PDC_LIGHTRED, CPoint(rcBlob.right + 30, rcBlob.CenterPoint().y - 30), 20, TRUE);
			pGO->AddDrawText(clrText);

			strSize.Format(_T("Height: %dpx"), GetRoundShort(eCodedObj.GetBoundingBoxHeight()));
			clrText.SetText(strSize);
			clrText.CreateObject(PDC_LIGHTRED, CPoint(rcBlob.right + 30, rcBlob.CenterPoint().y + 30), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("박스 아랫면 테이프 부착 : NG"));
			else												clrText.SetText(_T("Bottom Side Tape NG"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, DEF_FONT_BASIC_SIZE);
			pGO->AddDrawText(clrText);
		}
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString& e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI*2, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		return TRUE;
	}

	return bCheck;
}

BOOL CInspectionVision::MatchModel_Tab(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, CRect& rcTabBegin)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;
#ifdef RELEASE_1G
	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();
#elif RELEASE_SG
	CModelInfo::stStackerOcr& stStackerOcr = CModelInfo::Instance()->GetStackerOcr();
#endif
	COLORBOX clrBox;
	COLORTEXT clrText;

	int nDrawTextPosY = 100;
	int nDrawTextSize = 20;
	CString strException = _T("");

	if (pImgObj == NULL)
		return FALSE;

	const int nPageSizeX = pImgObj->GetWidth();
	const int nPageSizeY = pImgObj->GetHeight();

	EImageBW8 ImageX;
	ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

	if (&ImageX == NULL) return FALSE;

	CRect rcRect;
	rcRect.left = 0;
	rcRect.right = nPageSizeX;
	rcRect.top = 0;
	rcRect.bottom = nPageSizeY;
	rcRect.NormalizeRect();

	EROIBW8 ImageROI;
	ImageROI.Attach(&ImageX);
	ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

	clrBox.CreateObject(PDC_ORANGE, rcRect, PS_SOLID, 1);
	pGO->AddDrawBox(clrBox);

	EMatcher* pMatch = m_pMatch[nViewIndex][0];
	if (!pMatch) return FALSE;

	if (!pMatch->GetPatternLearnt())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Tab 찾기 : NG [등록 모델 없음]"));
		else												clrText.SetText(_T("Tab Search : NG [No Teaching]"));
		clrText.CreateObject(PDC_LIGHTRED, nPageSizeX / 2, nPageSizeY - 200, nDrawTextSize, FALSE, CxGOText::TextAlignmentCenter);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	pMatch->SetContrastMode(EMatchContrastMode_Normal);
	pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
	pMatch->SetInterpolate(TRUE);
	pMatch->SetMinScore(0.5f);
	pMatch->SetMinAngle((-10.f)); //(-10.0f);						
	pMatch->SetMaxAngle((10.f));// ( ( 1.f) * fRange	); //( 10.0f);
	pMatch->SetMinScale(0.9f);
	pMatch->SetMaxScale(1.1f);
	pMatch->SetMaxPositions(1);
	pMatch->SetMaxInitialPositions(0);

	pMatch->Match(&ImageROI);

	BOOL bSearchRet = TRUE;

	int nCount = pMatch->GetNumPositions();

	if (!nCount)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("모델 찾기 실패 : [Tray Tab]");
		else												strException = _T("No Matching Pattern : [Tray Tab]");
		throw strException;
	}

	EMatchPosition& eMatchPos = pMatch->GetPosition(0);
	CRect rePatternArea;
	rePatternArea.left = (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f));
	rePatternArea.top = (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f));
	rePatternArea.right = (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f));
	rePatternArea.bottom = (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f));
	rePatternArea.NormalizeRect();

	rcTabBegin = rePatternArea;
	
	clrText.SetText(_T("[Score : %.2f]"), eMatchPos.Score * 100.f);
	clrText.CreateObject(PDC_GREEN, rePatternArea.left + 3, rePatternArea.top + 10, 25, TRUE);
	pGO->AddDrawText(clrText);

	clrBox.CreateObject(PDC_GREEN, rePatternArea, PS_SOLID, 3);
	pGO->AddDrawBox(clrBox);

	if (!bSearchRet) return FALSE;

	return TRUE;
}

BOOL CInspectionVision::MatchModel_Chip(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, CRect rcRoi)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stChip& stChip = CModelInfo::Instance()->GetChip();

	COLORREF clrColor;
	COLORBOX clrBox;
	COLORCROSS clrCross;
	COLORTEXT clrText;

	int nDrawTextPosY = 400;
	int nDrawTextSize = 70;
	CString strException = _T("");

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	try
	{
		EImageBW8 ImageX;
		ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		if (&ImageX == NULL) return FALSE;

		CRect rcRect = rcRoi;
		rcRect.NormalizeRect();

		EROIBW8 ImageROI;
		ImageROI.Attach(&ImageX);
		ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

		EMatcher* pMatch = m_pMatchChip;
		if (!pMatch) return FALSE;

		if (!pMatch->GetPatternLearnt())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Chip 찾기 : NG [등록 모델 없음]"));
			else 												clrText.SetText(_T("Chip Match Search : NG [No Teaching]"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		pMatch->SetContrastMode(EMatchContrastMode_Normal);
		pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
		pMatch->SetInterpolate(TRUE);
		pMatch->SetMinScore(0.5f);
		pMatch->SetMinAngle((-10.f)); //(-10.0f);						
		pMatch->SetMaxAngle((10.f));// ( ( 1.f) * fRange	); //( 10.0f);
		pMatch->SetMinScale(0.9f);
		pMatch->SetMaxScale(1.1f);
		pMatch->SetMaxPositions(1);
		pMatch->SetMaxInitialPositions(0);

		pMatch->Match(&ImageROI);

		BOOL bSearchRet = TRUE;

		int nCount = pMatch->GetNumPositions();
		if (nCount == 0)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("모델 찾기 실패 : [Chip Inside]");
			else												strException = _T("None Matching ChipCnt : [Chip Inside]");
			throw strException;
		}

		//////////////////////////////////////////////////////////////////////////
		float fScore = stChip.fRatio;

		EMatchPosition& eMatchPos = pMatch->GetPosition(0);
		CRect rePatternArea;
		rePatternArea.left = (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f)) + rcRect.left;
		rePatternArea.top = (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f)) + rcRect.top;
		rePatternArea.right = (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f)) + rcRect.left;
		rePatternArea.bottom = (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f)) + rcRect.top;
		rePatternArea.NormalizeRect();

		if (eMatchPos.Score * 100.f > fScore)	clrColor = PDC_GREEN;
		else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

		clrText.SetText(_T("[Score : %.2f]"), eMatchPos.Score * 100.f);
		clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top, 25, TRUE);
		pGO->AddDrawText(clrText);

		if (!bSearchRet) return FALSE;

		CPoint ptBase, ptInsp;
		ptBase.x = rcRect.left + stChip.ptMatch.x;
		ptBase.y = rcRect.top + stChip.ptMatch.y;
		ptInsp.x = rePatternArea.CenterPoint().x;
		ptInsp.y = rePatternArea.CenterPoint().y;

		clrCross.CreateObject(PDC_ORANGE, ptBase, 10, PS_SOLID, 3);
		pGO->AddDrawCross(clrCross);

		clrCross.CreateObject(PDC_GREEN, ptInsp, 10, PS_SOLID, 3);
		pGO->AddDrawCross(clrCross);

		if (stChip.nSelectUnit == 0) // px
		{
			int nXSift = ptInsp.x - ptBase.x;
			int nYSift = ptInsp.y - ptBase.y;
			
			if (abs(nXSift) < stChip.nShiftX_PX) 	clrColor = PDC_GREEN;
			else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

			clrText.SetText(_T("[Shift X : %d]"), nXSift);
			clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top + 50, 25, TRUE);
			pGO->AddDrawText(clrText);

			if (abs(nYSift) < stChip.nShiftY_PX) 	clrColor = PDC_GREEN;
			else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

			clrText.SetText(_T("[Shift Y : %d]"), nYSift);
			clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top + 100, 25, TRUE);
			pGO->AddDrawText(clrText);

			clrBox.CreateObject(bSearchRet ? PDC_GREEN : PDC_LIGHTRED, rePatternArea, PS_SOLID, 3);
			pGO->AddDrawBox(clrBox);
		}
		else
		{
			float fXSift = (float)(ptInsp.x - ptBase.x) * 1000.f / m_fCalX[nViewIndex];
			float fYSift = (float)(ptInsp.y - ptBase.y) * 1000.f / m_fCalX[nViewIndex];

			if (abs(fXSift) < stChip.fShiftX_MM) 	clrColor = PDC_GREEN;
			else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

			clrText.SetText(_T("[Shift X : %.4f]"), fXSift);
			clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top + 50, 25, TRUE);
			pGO->AddDrawText(clrText);

			if (abs(fYSift) < stChip.fShiftY_MM) 	clrColor = PDC_GREEN;
			else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

			clrText.SetText(_T("[Shift Y : %.4f]"), fYSift);
			clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top + 100, 25, TRUE);
			pGO->AddDrawText(clrText);

			clrBox.CreateObject(bSearchRet ? PDC_GREEN : PDC_LIGHTRED, rePatternArea, PS_SOLID, 3);
			pGO->AddDrawBox(clrBox);
		}

		if (!bSearchRet) return FALSE;
	}
	catch (EException& e)
	{
		e.what();
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Chip 찾기 에러"));
		else												clrText.SetText(_T("Find Chip Error"));
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 70, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::MatchModel_ChipInside(CxGraphicObject* pGO, EROIBW8& eImgROI, int nViewIndex, CRect& rcChipInside)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stChipOcr& stChipOcr = CModelInfo::Instance()->GetChipOcr();

	COLORBOX clrBox;
	COLORTEXT clrText;

	int nDrawTextPosY = 100;
	int nDrawTextSize = 20;
	CString strException = _T("");

	int nPageSizeX = eImgROI.GetWidth();
	int nPageSizeY = eImgROI.GetHeight();

	CRect rcRect;
	rcRect.left = 0;
	rcRect.right = nPageSizeX;
	rcRect.top = 0;
	rcRect.bottom = nPageSizeY;
	rcRect.NormalizeRect();

	EMatcher* pMatch = m_pMatch[nViewIndex][0];
	if (!pMatch) return FALSE;

	if (!pMatch->GetPatternLearnt())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Chip 내부 모델 찾기 : NG [등록 모델 없음]"));
		else												clrText.SetText(_T("Chip Inside Align Search : NG [No Teaching]"));
		clrText.CreateObject(PDC_LIGHTRED, nPageSizeX / 2, nPageSizeY - 200, nDrawTextSize, FALSE, CxGOText::TextAlignmentCenter);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	pMatch->SetContrastMode(EMatchContrastMode_Normal);
	pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
	pMatch->SetInterpolate(TRUE);
	pMatch->SetMinScore(0.5f);
	pMatch->SetMinAngle((-10.f)); //(-10.0f);						
	pMatch->SetMaxAngle((10.f));// ( ( 1.f) * fRange	); //( 10.0f);
	pMatch->SetMinScale(0.9f);
	pMatch->SetMaxScale(1.1f);
	pMatch->SetMaxPositions(1);
	pMatch->SetMaxInitialPositions(0);

	pMatch->Match(&eImgROI);

	BOOL bSearchRet = TRUE;

	int nCount = pMatch->GetNumPositions();

	if (!nCount)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("모델 찾기 실패 : [Chip Inside Pattern]");
		else												strException = _T("No Matching Pattern : [Chip Inside Pattern]");
		throw strException;
	}

	EMatchPosition& eMatchPos = pMatch->GetPosition(0);
	CRect rePatternArea;
	rePatternArea.left		=	eImgROI.GetOrgX() + (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f));
	rePatternArea.top		=	eImgROI.GetOrgY() + (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f));
	rePatternArea.right		=	eImgROI.GetOrgX() + (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth()  / 2.f));
	rePatternArea.bottom	=	eImgROI.GetOrgY() + (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f));
	rePatternArea.NormalizeRect();

	rcChipInside = rePatternArea;

	clrText.SetText(_T("[Score : %.2f]"), eMatchPos.Score * 100.f);
	clrText.CreateObject(PDC_GREEN, rePatternArea.left + 3, rePatternArea.top + 10, 25, TRUE);
	pGO->AddDrawText(clrText);

	clrBox.CreateObject(PDC_GREEN, rePatternArea, PS_SOLID, 3);
	pGO->AddDrawBox(clrBox);

	if (!bSearchRet) return FALSE;

	return TRUE;

}

BOOL CInspectionVision::MatchModel_Mixing(CxGraphicObject* pGO, CxImageObject *pImgObj, const int nViewIndex, CModelInfo::stMixing& stMixingModel, BOOL bManual)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CString strException = _T("");
	const int nMixingZoneWidth = stMixingModel.nMixingZoneWidth;
	const int nTrayCountZoneWidth = stMixingModel.nTrayCountZoneWidth;
	const int nTrayPixelValue = stMixingModel.nTrayPixelValue;
	const float fMatchingScore = stMixingModel.fMatchingScore;
	const BOOL bIsPlainPattern = stMixingModel.bIsPlainPattern;

	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORDCROSS clrDCross;

	int nDrawTextPosY = 100;
	int nDrawTextSize = 35;
	nDrawTextPosY += nDrawTextSize * 5;

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	BOOL bResult = TRUE;
	BOOL bArrMixing[nMATCH_MAX];
	std::fill(bArrMixing, bArrMixing + nMATCH_MAX, TRUE);

	try
	{
		EImageBW8 eImageX;
		eImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		if (&eImageX == NULL) return FALSE;

		// ROI Tray로 설정하기 위해 추가, 250325 황대은J
		ECodedImage2 CodedImage;
		EImageEncoder CodedImageEncoder;
		EObjectSelection CodedImageObjectSelection;

		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_Absolute);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nTrayPixelValue);
		CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Four);

		CodedImageEncoder.Encode(eImageX, CodedImage);
		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjects(CodedImage);
		CodedImageObjectSelection.Sort(EFeature_Area, ESortDirection_Descending);

		int nElementCount = CodedImageObjectSelection.GetElementCount();

		CRect rcTray;
		rcTray.left = CodedImageObjectSelection.GetElement(0).GetLeftLimit();
		rcTray.top = CodedImageObjectSelection.GetElement(0).GetTopLimit();
		rcTray.right = CodedImageObjectSelection.GetElement(0).GetRightLimit();
		rcTray.bottom = CodedImageObjectSelection.GetElement(0).GetBottomLimit();

		for (int i = 1; i < nElementCount; ++i)
		{
			ECodedElement& eCodedElement2 = CodedImageObjectSelection.GetElement(i);
			if ((int)(eCodedElement2.GetBoundingBoxWidth()) >= rcTray.Width() * 0.8)
			{
				rcTray.top = min((int)rcTray.top, eCodedElement2.GetTopLimit());
				rcTray.left = min((int)rcTray.left, eCodedElement2.GetLeftLimit());
				rcTray.right = max((int)rcTray.right, eCodedElement2.GetRightLimit());
				rcTray.bottom = max((int)rcTray.bottom, eCodedElement2.GetBottomLimit());
			}
		}

		constexpr int nOffset = 20;

		rcTray.left += nOffset;
		rcTray.top += nOffset;
		rcTray.right -= nOffset;
		rcTray.bottom -= nOffset / 2;

		EROIBW8 eImageROI;
		eImageROI.Attach(&eImageX);
		eImageROI.SetPlacement(rcTray.left, rcTray.top, rcTray.Width(), rcTray.Height());

		const int nMixingZoneLeft = rcTray.left + nMixingZoneWidth;
		CRect rcTrayCount;
		rcTrayCount.SetRect(nMixingZoneLeft, rcTray.top, nMixingZoneLeft + nTrayCountZoneWidth, rcTray.top + rcTray.Height());
		if (rcTrayCount.right >= pImgObj->GetWidth())
		{
			rcTrayCount.right = pImgObj->GetWidth() - 2;

		}
		if (!RectInImage(rcTrayCount, pImgObj))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("검사 영역이 이미지 밖에 있습니다."));
			else												strException.Format(_T("The inspection area is outside the image."));
			nDrawTextPosY += nDrawTextSize * 5;
			throw strException;
		}
		clrBox.CreateObject(PDC_ORANGE, rcTrayCount, PS_DASH, 1);
		pGO->AddDrawBox(clrBox);

		EROIBW8 eTrayCountROI;
		eTrayCountROI.Attach(&eImageX);
		eTrayCountROI.SetPlacement(rcTrayCount.left, rcTrayCount.top, rcTrayCount.Width(), rcTrayCount.Height());

		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(false);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(true);
		CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(1);

		CodedImageEncoder.Encode(eTrayCountROI, CodedImage);
		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjectsUsingFloatFeature(CodedImage, EFeature_BoundingBoxWidth, float(eTrayCountROI.GetWidth() * 0.5), float(eTrayCountROI.GetWidth() * 1.1), EDoubleThresholdMode_Inside);

		nElementCount = CodedImageObjectSelection.GetElementCount();
		int nTrayQuantitiy = nElementCount;

		for (int i = 0; i < nTrayQuantitiy; ++i)
		{
			ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(i);
			int nX = static_cast<int>(eCodedElement.GetBoundingBoxCenterX()) + eTrayCountROI.GetOrgX();
			int nY = static_cast<int>(eCodedElement.GetBoundingBoxCenterY()) + eTrayCountROI.GetOrgY();

			clrDCross.CreateObject(PDC_BLUE, nX, nY, 15, PS_SOLID, 2);
			pGO->AddDrawDCross(clrDCross);
		}

		nDrawTextPosY += nDrawTextSize * 7;

		BOOL bTrayQuantity;
		if (bManual)
		{
			bTrayQuantity = (nTrayQuantitiy == nManualMixingNum) ? TRUE : FALSE;
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량 : V[%d] / Manual[%d]"), nTrayQuantitiy, nManualMixingNum);
			else												clrText.SetText(_T("Tray QTY : V[%d] / Manual[%d]"), nTrayQuantitiy, nManualMixingNum);
		}
		else
		{
			const int nMMITrayQuantity = CVisionSystem::Instance()->m_nMMITrayQuantity_Mixing;
//			const int nMMITrayQuantity = nTrayQuantitiy;
			bTrayQuantity = (nTrayQuantitiy == nMMITrayQuantity) ? TRUE : FALSE;
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량 : V[%d] / H[%d]"), nTrayQuantitiy, nMMITrayQuantity);
			else												clrText.SetText(_T("Tray QTY : V[%d] / H[%d]"), nTrayQuantitiy, nMMITrayQuantity);
		}
		clrText.CreateObject(bTrayQuantity ? PDC_GREEN : PDC_RED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize * 2, TRUE);
		pGO->AddDrawText(clrText);
		if (!bTrayQuantity) return FALSE;
		// ========================================================================================================

		CRect rcMixing;
		rcMixing.SetRect(eImageROI.GetOrgX(), eImageROI.GetOrgY(), eImageROI.GetOrgX() + nMixingZoneWidth, eImageROI.GetOrgY() + eImageROI.GetHeight());
		if (!RectInImage(rcMixing, pImgObj))
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("검사 영역이 이미지 밖에 있습니다."));
			else												strException.Format(_T("The inspection area is outside the image."));
			nDrawTextPosY += nDrawTextSize * 5;
			throw strException;
		}
		clrBox.CreateObject(PDC_ORANGE, rcMixing, PS_DASH, 1);
		pGO->AddDrawBox(clrBox);

		// 260210 HJ : 위치 변경 Plain이면 검사를 안해야지..
		if (bIsPlainPattern)
			return TRUE;

		EROIBW8 eMixingROI;
		eMixingROI.Attach(&eImageX);
		eMixingROI.SetPlacement(rcMixing.left, rcMixing.top, rcMixing.Width(), rcMixing.Height());

		EMatcher* pMatchFirst = m_pMatch[nViewIndex][0];
		pMatchFirst->SetContrastMode(EMatchContrastMode_Normal);
		pMatchFirst->SetCorrelationMode(ECorrelationMode_Normalized);
		pMatchFirst->SetInterpolate(TRUE);
		pMatchFirst->SetMinScore(0.7f);
		pMatchFirst->SetMinAngle((-10.f)); //(-10.0f);						
		pMatchFirst->SetMaxAngle((10.f));// ( ( 1.f) * fRange	); //( 10.0f);
		pMatchFirst->SetMinScale(0.8f);
		pMatchFirst->SetMaxScale(1.2f);
		pMatchFirst->SetMaxPositions(nTrayQuantitiy);
		pMatchFirst->SetMaxInitialPositions(0);

		pMatchFirst->Match(&eMixingROI);

		int nMixingPos = pMatchFirst->GetMaxPositions();

		if (nMixingPos != nTrayQuantitiy)
		{
			strException.Format(_T("Mixing NG"));
			nDrawTextPosY += nDrawTextSize * 5;
			throw strException;
		}

		for (int i = 0; i < nMixingPos; ++i)
		{
			CRect rcMixing;
			EMatchPosition& eMixPos = pMatchFirst->GetPosition(i);
			rcMixing.left = eMixingROI.GetOrgX() + (int)GetRoundLong(eMixPos.CenterX - ((float)pMatchFirst->GetPatternWidth() / 2.f));
			rcMixing.top = eMixingROI.GetOrgY() + (int)GetRoundLong(eMixPos.CenterY - ((float)pMatchFirst->GetPatternHeight() / 2.f));
			rcMixing.right = eMixingROI.GetOrgX() + (int)GetRoundLong(eMixPos.CenterX + ((float)pMatchFirst->GetPatternWidth() / 2.f));
			rcMixing.bottom = eMixingROI.GetOrgY() + (int)GetRoundLong(eMixPos.CenterY + ((float)pMatchFirst->GetPatternHeight() / 2.f));

			EROIBW8 ePatternROI;
			ePatternROI.Attach(&eImageX);
			ePatternROI.SetPlacement(rcMixing.left, rcMixing.top, rcMixing.Width(), rcMixing.Height());

			for (int j = 1; j < nMATCH_MAX; ++j)
			{
				EMatcher* pMatch = m_pMatch[nViewIndex][j];

				if (!pMatch->GetPatternLearnt())
					continue;

				pMatch->SetContrastMode(EMatchContrastMode_Normal);
				pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
				pMatch->SetInterpolate(TRUE);
				pMatch->SetMinScore(0.6f);
				pMatch->SetMinAngle((-10.f)); //(-10.0f);						
				pMatch->SetMaxAngle((10.f));// ( ( 1.f) * fRange	); //( 10.0f);
				pMatch->SetMinScale(0.8f);
				pMatch->SetMaxScale(1.2f);
				pMatch->SetMaxPositions(1);
				pMatch->SetMaxInitialPositions(0);

				pMatch->Match(&ePatternROI);

				int nPos = pMatch->GetNumPositions();

				if (!nPos)
				{
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("일치하는 패턴 없음 - [일치 유형: %d]"), j);
					else												strException.Format(_T("No Matched Pattern - [Match Type : %d]"), j);
					clrBox.CreateObject(PDC_RED, rcMixing, PS_DASH);
					pGO->AddDrawBox(clrBox);
					nDrawTextPosY += nDrawTextSize * 5;
					throw strException;
				}

				EMatchPosition& eMatchPos = pMatch->GetPosition(0);

				CRect rcPattern;
				rcPattern.left = ePatternROI.GetOrgX() + (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f));
				rcPattern.top = ePatternROI.GetOrgY() + (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f));
				rcPattern.right = ePatternROI.GetOrgX() + (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f));
				rcPattern.bottom = ePatternROI.GetOrgY() + (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f));

				if (eMatchPos.Score * 100.0f < fMatchingScore)
				{
					bResult = FALSE;
					bArrMixing[i] = FALSE;
					clrBox.CreateObject(PDC_RED, rcPattern, PS_DASH);
					pGO->AddDrawBox(clrBox);

					clrText.SetText(_T("[%.2f]"), eMatchPos.Score * 100.0);
					clrText.CreateObject(PDC_RED, rcPattern.CenterPoint().x, rcPattern.top, 20, TRUE, CxGOText::TextAlignmentCenter);
					pGO->AddDrawText(clrText);
				}
				else
				{
					clrBox.CreateObject(PDC_GREEN, rcPattern, PS_DASH);
					pGO->AddDrawBox(clrBox);

					clrText.SetText(_T("[%.2f]"), eMatchPos.Score * 100.0);
					clrText.CreateObject(PDC_GREEN, rcPattern.CenterPoint().x, rcPattern.top, 20, TRUE, CxGOText::TextAlignmentCenter);
					pGO->AddDrawText(clrText);
				}
			}

			clrBox.CreateObject(bArrMixing[i] ? PDC_GREEN : PDC_RED, rcMixing, PS_SOLID);
			pGO->AddDrawBox(clrBox);
		}

		return bResult;
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString& e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize * 2, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::MatchModel_HIC(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, int nType, BOOL bColor, CPoint ptPatternShift, CPoint ptHIC, float fMatchScore)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	COLORBOX  clrBox;
	COLORTEXT clrText;
	COLORDCROSS clrDCross;

	int nDrawTextPosY = SUBMATERIAL_HIC_TEXT_POS;
	int nDrawTextSize = SUBMATERIAL_TEXT_SIZE;

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	try
	{
		EImageBW8 ImageX;
		EImageC24 ImageX24;
		if (bColor)
		{
			ImageX24.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
			ImageX.SetSize(&ImageX24);
			EasyColor::GetComponent(&ImageX24, &ImageX, 0);
		}
		else
		{
			ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);
		}

		if (&ImageX == NULL)
			return FALSE;

		EROIBW8 eImageROI;
		eImageROI.Attach(&ImageX);
		eImageROI.SetPlacement(m_rcTray.left, m_rcTray.top, m_rcTray.Width(), m_rcTray.Height());

		EMatcher* pMatch = m_pMatch[nViewIndex][nType];
		if (!pMatch) return FALSE;

		if (!pMatch->GetPatternLearnt())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("HIC 찾기 : NG [등록 모델 없음]"));
			else												clrText.SetText(_T("HIC Search : NG [No Teaching]"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		pMatch->SetContrastMode(EMatchContrastMode_Normal);
		pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
		pMatch->SetInterpolate(TRUE);
		pMatch->SetMinScore(0.6f); // 0.5f
		pMatch->SetMinAngle(-20.f); // -10.0f
		pMatch->SetMaxAngle(20.f); // 10.0f
		pMatch->SetMinScale(0.8f);
		pMatch->SetMaxScale(1.2f);
		pMatch->SetMaxPositions(1); // nHIC_CIRCLE_MAX);
		pMatch->SetMaxInitialPositions(0);
		pMatch->Match(&eImageROI);

		//////////////////////////////////////////////////////////////////////////
		BOOL bHICRet = TRUE;
		int nCount = pMatch->GetNumPositions();
		if (nCount != 1) bHICRet = FALSE;

		int nMatchCnt = 0;
		EMatchPosition& eMatchPos = pMatch->GetPosition(nMatchCnt);

		m_dptHICCenter[nViewIndex][nMatchCnt].x = eMatchPos.CenterX;
		m_dptHICCenter[nViewIndex][nMatchCnt].y = eMatchPos.CenterY;
		m_fHICAngle[nViewIndex][nMatchCnt] = eMatchPos.Angle;
		m_fHICScale[nViewIndex][nMatchCnt] = eMatchPos.Scale;
		m_fHICWidth[nViewIndex][nMatchCnt] = pMatch->GetPatternWidth() * m_fHICScale[nViewIndex][nMatchCnt];
		m_fHICHeight[nViewIndex][nMatchCnt] = pMatch->GetPatternHeight() * m_fHICScale[nViewIndex][nMatchCnt];

		CRect rcMatchArea;
		rcMatchArea.left	= (int)GetRoundLong(eMatchPos.CenterX - ((float)m_fHICWidth[nViewIndex][nMatchCnt]	/ 2.f))	+ eImageROI.GetOrgX();
		rcMatchArea.top		= (int)GetRoundLong(eMatchPos.CenterY - ((float)m_fHICHeight[nViewIndex][nMatchCnt] / 2.f)) + eImageROI.GetOrgY();
		rcMatchArea.right	= (int)GetRoundLong(eMatchPos.CenterX + ((float)m_fHICWidth[nViewIndex][nMatchCnt]	/ 2.f))	+ eImageROI.GetOrgX();
		rcMatchArea.bottom	= (int)GetRoundLong(eMatchPos.CenterY + ((float)m_fHICHeight[nViewIndex][nMatchCnt] / 2.f)) + eImageROI.GetOrgY();
		rcMatchArea.NormalizeRect();

		int nMinX = ptHIC.x - ptPatternShift.x;
		int nMaxX = ptHIC.x + ptPatternShift.x;
		int nMinY = ptHIC.y - ptPatternShift.y;
		int nMaxY = ptHIC.y + ptPatternShift.y;

		BOOL bShiftX = nMinX <= rcMatchArea.CenterPoint().x && rcMatchArea.CenterPoint().x <= nMaxX ? TRUE : FALSE;
		BOOL bShiftY = nMinY <= rcMatchArea.CenterPoint().y && rcMatchArea.CenterPoint().y <= nMaxY ? TRUE : FALSE;

		clrBox.CreateObject(PDC_BLUE, rcMatchArea, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

		BOOL bMatchScore = eMatchPos.Score * 100.f >= fMatchScore ? TRUE : FALSE;
		clrText.SetText(_T("[Score : %3.1f]"), eMatchPos.Score * 100.f);
		clrText.CreateObject(bMatchScore ? PDC_BLUE : PDC_RED, rcMatchArea.left + 3, rcMatchArea.top + 10, 15, TRUE);
		pGO->AddDrawText(clrText);

		if (!bHICRet || !bMatchScore)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("HIC 찾기 : NG"));
			else												clrText.SetText(_T("HIC Search : NG"));
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("HIC 찾기 : OK"));
			else												clrText.SetText(_T("HIC Search : OK"));
		}
		clrText.CreateObject(bHICRet && bMatchScore ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
		pGO->AddDrawText(clrText);

		nDrawTextPosY += nDrawTextSize * 2;
		clrText.SetText(_T("HIC Shift (X, Y) : (%d, %d)"), rcMatchArea.CenterPoint().x, rcMatchArea.CenterPoint().y);
		clrText.CreateObject(bShiftX && bShiftY ? PDC_GREEN : PDC_RED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
		pGO->AddDrawText(clrText);

		clrDCross.CreateObject(PDC_BLUE, rcMatchArea.CenterPoint(), 10, PS_SOLID, 2);
		pGO->AddDrawDCross(clrDCross);

		float fPatternAngle = eMatchPos.Angle;

		clrBox.CreateObject(PDC_BLUE, rcMatchArea, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);

		return bHICRet && bShiftX && bShiftY && bMatchScore;
	}
	catch (EException& e)
	{
		e.what();
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("HIC를 찾을 수 없습니다."));
		else												clrText.SetText(_T("Can't find a HIC"));
		clrText.CreateObject(PDC_LIGHTRED, 700, SUBMATERIAL_HIC_TEXT_POS, 25, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}
}

BOOL CInspectionVision::MatchModel_Desiccant(CxGraphicObject* pGO, CxImageObject *pImgObj, const int nViewIndex, const int nType, const int nDesiccantQuantity,
												CPoint ptPatternShift, std::vector<CPoint*>& vecPt, float fMatchScore)
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	CModelInfo::stDesiccantMaterialInfo& stDesiccant = CModelInfo::Instance()->GetDesiccantMaterialInfo();

	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORDCROSS clrDCross;

	int nDrawTextPosY = SUBMATERIAL_DESICCANT_TEXT_POS;
	const int nDrawTextSize = SUBMATERIAL_TEXT_SIZE;

	if( pImgObj == NULL )
		return FALSE;

	const int nPageSizeX = pImgObj->GetWidth();
	const int nPageSizeY = pImgObj->GetHeight();

	auto pImgObjClone = make_unique<CxImageObject>();
	pImgObjClone->Clone(pImgObj);

	try
	{
		EImageBW8 ImageX;
		ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObjClone->GetImageBuffer(), pImgObjClone->GetWidthBytes() * 8);

		if (&ImageX == NULL) return FALSE;

		EROIBW8 ImageROI;
		ImageROI.Attach(&ImageX);
		ImageROI.SetPlacement(m_rcTray.left, m_rcTray.top, m_rcTray.Width(), m_rcTray.Height());

		EMatcher* pMatch = m_pMatch[nViewIndex][nType];
		if (!pMatch) return FALSE;

		if (!pMatch->GetPatternLearnt())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Desiccant 찾기 : NG [등록 모델 없음]"));
			else												clrText.SetText(_T("Desiccant Search : NG [No Teaching]"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		std::vector<EMatchPosition> vecMatch;
		for (int i = 0; i < nDesiccantQuantity; ++i)
		{
			pMatch->SetContrastMode(EMatchContrastMode_Normal);
			pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
			pMatch->SetInterpolate(TRUE);
			pMatch->SetMinScore(0.5f);
			pMatch->SetMinAngle(-10.f); //(-10.0f);						
			pMatch->SetMaxAngle(10.f);// ( ( 1.f) * fRange	); //( 10.0f);
			pMatch->SetMinScale(0.7f);
			pMatch->SetMaxScale(1.3f);
			pMatch->SetMaxPositions(1);
			pMatch->SetMaxInitialPositions(0);

			pMatch->Match(&ImageROI);
			
			EMatchPosition& eMatchPos = pMatch->GetPosition(0);
			vecMatch.push_back(eMatchPos);

			CRect rcMatchRect;
			rcMatchRect.left = (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f)) + ImageROI.GetOrgX();
			rcMatchRect.top = (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f)) + ImageROI.GetOrgY();
			rcMatchRect.right = (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f)) + ImageROI.GetOrgX();
			rcMatchRect.bottom = (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f)) + ImageROI.GetOrgY();
			rcMatchRect.NormalizeRect();

			EROIBW8 eMatchROI;
			eMatchROI.Attach(&ImageX);
			eMatchROI.SetPlacement(rcMatchRect.left, rcMatchRect.top, rcMatchRect.Width(), rcMatchRect.Height());

			EasyImage::GainOffset(&eMatchROI, &eMatchROI, 1.00f, -255.00f);
		}

		sort(vecMatch.begin(), vecMatch.end(), CompareMatchX);
		BOOL bSearchRet = TRUE;
		BOOL bShift = TRUE;
		
		int nCount = static_cast<int>(vecMatch.size());
		int nDesiccantOK = 0;
		CRect rcMatchArea;
		float fMaxAngle = 0.f;//fAverageAngle
		//std::array<BOOL, nDesiccantQuantity> vecBoolMatchScore;
		std::vector<BOOL> vecBoolMatchScore(nDesiccantQuantity, TRUE);

		for (int nMatchCnt = 0; nMatchCnt < nCount; nMatchCnt++)
		{
			EMatchPosition& eMatchPos = vecMatch[nMatchCnt];
			rcMatchArea.left	= (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f))	+ ImageROI.GetOrgX();
			rcMatchArea.top		= (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f))	+ ImageROI.GetOrgY();
			rcMatchArea.right	= (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f))	+ ImageROI.GetOrgX();
			rcMatchArea.bottom	= (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f))	+ ImageROI.GetOrgY();
			rcMatchArea.NormalizeRect();
	
			ScaleRectAroundCenter(rcMatchArea, eMatchPos.Scale);

			vecBoolMatchScore[nMatchCnt] = eMatchPos.Score * 100.f >= fMatchScore ? TRUE : FALSE;
			if (vecBoolMatchScore[nMatchCnt])
				nDesiccantOK++;
			clrText.SetText(_T("[Score : %3.1f]"), eMatchPos.Score * 100.f);
			clrText.CreateObject(vecBoolMatchScore[nMatchCnt] ? PDC_BLUE : PDC_RED, rcMatchArea.left + 3, rcMatchArea.top + 10, 15, TRUE);
			pGO->AddDrawText(clrText);

			int nMinX = vecPt[nMatchCnt]->x - ptPatternShift.x;
			int nMaxX = vecPt[nMatchCnt]->x + ptPatternShift.x;
			int nMinY = vecPt[nMatchCnt]->y - ptPatternShift.y;
			int nMaxY = vecPt[nMatchCnt]->y + ptPatternShift.y;

			BOOL bShiftX = nMinX <= rcMatchArea.CenterPoint().x && rcMatchArea.CenterPoint().x <= nMaxX ? TRUE : FALSE;
			BOOL bShiftY = nMinY <= rcMatchArea.CenterPoint().y && rcMatchArea.CenterPoint().y <= nMaxY ? TRUE : FALSE;

			if (!bShiftX || !bShiftY)
				bShift = FALSE;

			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Desiccant 위치 %d (X, Y) : (%d, %d)"), nMatchCnt + 1, rcMatchArea.CenterPoint().x, rcMatchArea.CenterPoint().y);
			else												clrText.SetText(_T("Desiccant Position %d (X, Y) : (%d, %d)"), nMatchCnt + 1, rcMatchArea.CenterPoint().x, rcMatchArea.CenterPoint().y);
			clrText.CreateObject(bShiftX && bShiftY ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
			pGO->AddDrawText(clrText);

			clrDCross.CreateObject(PDC_BLUE, rcMatchArea.CenterPoint(), 10, PS_SOLID, 2);
			pGO->AddDrawDCross(clrDCross);

			clrBox.CreateObject(PDC_BLUE, rcMatchArea, PS_SOLID, 2);
			pGO->AddDrawBox(clrBox);

			nDrawTextPosY += nDrawTextSize * 2;
		}

		if (nCount != stDesiccant.nDesiccantQuantity)
			bSearchRet = FALSE;

		BOOL bAllResult = bSearchRet;
		for (int i = 0; i < vecBoolMatchScore.size(); ++i)
		{
			bAllResult = bAllResult && vecBoolMatchScore[i];
		}
		if (bAllResult)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Desiccant 찾기 : OK"));
			else												clrText.SetText(_T("Desiccant Search : OK"));
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Desiccant 찾기 : NG [Total : %d][OK : %d]"), stDesiccant.nDesiccantQuantity, nDesiccantOK);
			else												clrText.SetText(_T("Desiccant Search : NG [Total : %d][OK : %d]"), stDesiccant.nDesiccantQuantity, nDesiccantOK);
		}
		clrText.CreateObject(bAllResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize, TRUE);
		nDrawTextPosY += nDrawTextSize * 2;
		pGO->AddDrawText(clrText);

		return bShift && bAllResult;
	}
	catch (EException& e)
	{
		e.what();
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("방습제를 찾을 수 없습니다."));
		else												clrText.SetText(_T("Can't find a Desiccant"));
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, 25, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::MatchModel_HICType(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, CRect rcRoi)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stHICInfo& stHIC = CModelInfo::Instance()->GetHICInfo();

	COLORREF clrColor;
	COLORBOX clrBox;
	COLORCROSS clrCross;
	COLORTEXT clrText;

	CString strException = _T("");
	int nDrawTextPosY = 100;
	int nDrawTextSize = 25;

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	try
	{
		EImageC24 ImageC24;
		ImageC24.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		if (&ImageC24 == NULL) return FALSE;

		CRect rcRect = rcRoi;
		rcRect.NormalizeRect();

		EROIC24 ImageROI;
		ImageROI.Attach(&ImageC24);
		ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

		EMatcher* pMatch = m_pMatch[nViewIndex][0];
		if (!pMatch) return FALSE;

		if (!pMatch->GetPatternLearnt())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("HIC 찾기 : NG [등록 모델 없음]");
			else												strException = _T("HIC Match Search : NG [No Teaching]");

			throw strException;
		}

		//////////////////////////////////////////////////////////////////////////
		pMatch->SetContrastMode(EMatchContrastMode_Normal);
		pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
		pMatch->SetInterpolate(TRUE);
		pMatch->SetMinScore(0.5f);
		pMatch->SetMinAngle((-5.f));
		pMatch->SetMaxAngle((5.f));
		pMatch->SetMinScale(0.95f);
		pMatch->SetMaxScale(1.05f);
		pMatch->SetMaxPositions(1);
		pMatch->SetMaxInitialPositions(0);

		pMatch->Match(&ImageROI);

		BOOL bSearchRet = TRUE;

		int nCount = pMatch->GetNumPositions();
		if (nCount == 0)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("HIC 등록 모델 찾을 수 없음");
			else 												strException = _T("None Matching HIC");
			throw strException;
		}

		//////////////////////////////////////////////////////////////////////////
		float fScore = stHIC.fRatio;

		EMatchPosition& eMatchPos = pMatch->GetPosition(0);
		CRect rePatternArea;
		rePatternArea.left = (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f)) + rcRect.left;
		rePatternArea.top = (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f)) + rcRect.top;
		rePatternArea.right = (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f)) + rcRect.left;
		rePatternArea.bottom = (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f)) + rcRect.top;
		rePatternArea.NormalizeRect();

		if (eMatchPos.Score * 100.f > fScore)	clrColor = PDC_GREEN;
		else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

		if (bSearchRet)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("HIC 종류 검사 : OK"));
			else 												clrText.SetText(_T("HIC Type Check : OK"));
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("HIC 종류 검사 : NG"));
			else 												clrText.SetText(_T("HIC Type Check : NG"));
		}
		clrText.CreateObject(clrColor, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 2), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		if (!bSearchRet)
			return FALSE;

		clrText.SetText(_T("[Score : %.2f]"), eMatchPos.Score * 100.f);
		clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top, 20, TRUE);
		pGO->AddDrawText(clrText);

		clrBox.CreateObject(bSearchRet ? PDC_GREEN : PDC_LIGHTRED, rePatternArea, PS_SOLID, 3);
		pGO->AddDrawBox(clrBox);
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 2), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::MatchModel_MBB(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stMBBInfo& stMBB = CModelInfo::Instance()->GetMBBInfo();

	COLORREF clrColor;
	COLORBOX clrBox;
	COLORCROSS clrCross;
	COLORTEXT clrText;

	CString strException = _T("");
	int nDrawTextPosY = 100;
	int nDrawTextSize = 25;

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	try
	{
		EImageBW8 ImageX;
		ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		if (&ImageX == NULL) return FALSE;

		CRect rcRect = stMBB.rcInspArea;
		rcRect.NormalizeRect();

		EROIBW8 ImageROI;
		ImageROI.Attach(&ImageX);
		ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

		EMatcher* pMatch = m_pMatch[nViewIndex][0];
		if (!pMatch) return FALSE;

		if (!pMatch->GetPatternLearnt())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("MBB 찾기 : NG [등록 모델 없음]"));
			else												clrText.SetText(_T("MBB Match Search : NG [No Teaching]"));
			clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, nDrawTextPosY, nDrawTextSize);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		pMatch->SetContrastMode(EMatchContrastMode_Normal);
		pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
		pMatch->SetInterpolate(TRUE);
		pMatch->SetMinScore(0.5f);
		pMatch->SetMinAngle((-5.f));
		pMatch->SetMaxAngle((5.f));
		pMatch->SetMinScale(0.9f);
		pMatch->SetMaxScale(1.1f);
		pMatch->SetMaxPositions(1);
		pMatch->SetMaxInitialPositions(0);

		pMatch->Match(&ImageROI);

		BOOL bSearchRet = TRUE;

		int nCount = pMatch->GetNumPositions();
		if (nCount == 0)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("MBB 등록 모델 찾을 수 없음");
			else 												strException = _T("None Matching MBB");
			throw strException;
		}

		//////////////////////////////////////////////////////////////////////////
		float fScore = stMBB.fRatio;

		EMatchPosition& eMatchPos = pMatch->GetPosition(0);
		CRect rePatternArea;
		rePatternArea.left = (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f)) + rcRect.left;
		rePatternArea.top = (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f)) + rcRect.top;
		rePatternArea.right = (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f)) + rcRect.left;
		rePatternArea.bottom = (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f)) + rcRect.top;
		rePatternArea.NormalizeRect();

		if (eMatchPos.Score * 100.f > fScore)	clrColor = PDC_GREEN;
		else									clrColor = PDC_LIGHTRED, bSearchRet = FALSE;

		if (bSearchRet)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("MBB 정방향 검사 : OK"));
			else 												clrText.SetText(_T("MBB Orientation Check : OK"));
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("MBB 정방향 검사 : NG"));
			else 												clrText.SetText(_T("MBB Orientation Check : NG"));
		}
		clrText.CreateObject(clrColor, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 16), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		if (!bSearchRet)
			return FALSE;

		clrText.SetText(_T("[Score : %.2f]"), eMatchPos.Score * 100.f);
		clrText.CreateObject(clrColor, rePatternArea.left + 3, rePatternArea.top, 25, TRUE);
		pGO->AddDrawText(clrText);

		clrBox.CreateObject(bSearchRet ? PDC_GREEN : PDC_LIGHTRED, rePatternArea, PS_SOLID, 3);
		pGO->AddDrawBox(clrBox);
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 2, 40, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	return TRUE;
}

BOOL CInspectionVision::MatchModel_ForLabel(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, int nType)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stLabelInfo& stLabelInfo = CModelInfo::Instance()->GetLabelInfo();

	COLORBOX clrBox;
	COLORTEXT clrText;

	CString strType, strText;
	strType.Format(_T("Masking_%d"), nType + 1);

	int nTextPosY = 125 + (60 * (nType+1));

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	EImageBW8 ImageX;
	ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_1[Label][Num %d]"), nType);

	if (&ImageX == NULL)
		return FALSE;

	EMatcher* pMatch = m_pLabelMatch[nType];
	// 	if (!pMatch)
	// 		return FALSE;

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_2"));
	if (!pMatch->GetPatternLearnt())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("%s 찾기 : NG [등록 모델 없음]"), strType);
		else												clrText.SetText(_T("%s Search : NG [No Teaching]"), strType);
		clrText.CreateObject(PDC_LIGHTRED, 50, nTextPosY, 35, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	float fParaScore = 0.50f;

	pMatch->SetContrastMode(EMatchContrastMode_Normal);
	pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
	pMatch->SetInterpolate(TRUE);
	pMatch->SetMinScore(0.50f); // 최소 Parameter가 0.50f
	switch (stLabelInfo.nLabelMaskingScoreType[nType])
	{
	case MASKING_SCORE_HIGH:		fParaScore = 0.90f;		break;
	case MASKING_SCORE_MIDDLE:		fParaScore = 0.70f;		break;
	case MASKING_SCORE_LOW:			fParaScore = 0.50f;		break;
		default:
			break;
	}
	pMatch->SetMinAngle(-1.0f);
	pMatch->SetMaxAngle(1.0f);
	pMatch->SetMinScale(0.99f);
	pMatch->SetMaxScale(1.01f);
	pMatch->SetMaxPositions(5);
	pMatch->SetMaxInitialPositions(0);
	pMatch->Match(&ImageX);

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_3"));

	int nCount = pMatch->GetNumPositions();
	if (nCount < 1)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("%s 찾기 : NG [수량 %d<1]"), strType, nCount);
		else												clrText.SetText(_T("%s Search : NG [Count %d<1]"), strType, nCount);
		clrText.CreateObject(PDC_LIGHTRED, 50, nTextPosY, 35, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_4"));

	int nNgCnt = 0;
	float fScore = 0.f, fCenterX = 0.f, fCenterY = 0.f;
	BOOL bRatioOK = FALSE;

	for (int nMatchCnt = 0; nMatchCnt < nCount; nMatchCnt++)
	{
		EMatchPosition& eMatchPos = pMatch->GetPosition(nMatchCnt);

		strText.Format(_T("__________Insp_Label_4-%d [%.2f]"), nMatchCnt, eMatchPos.Score);
		CVisionSystem::Instance()->WriteMessage(LogTypeNormal, strText);

		if (bRatioOK == FALSE && eMatchPos.Score < fParaScore)
		{
			clrText.SetText(_T("%s Score : NG [<%.2f], [%d:%.2f]"), strType, fParaScore, nMatchCnt, eMatchPos.Score);
			clrText.CreateObject(PDC_LIGHTRED, 50, nTextPosY + (20 * (nMatchCnt + 1)), 35, TRUE);
			pGO->AddDrawText(clrText);

			nNgCnt++;
		}
		else
		{
			if (fScore < eMatchPos.Score)
			{
				fScore = eMatchPos.Score;
				fCenterX = eMatchPos.CenterX;
				fCenterY = eMatchPos.CenterY;

				if (fScore > fParaScore)
					bRatioOK = TRUE;
			}
		}

		if (nNgCnt == nCount)
		{
			return FALSE;
		}
	}

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_5"));

	CRect rePatternArea;
	rePatternArea.left = (int)GetRoundLong(fCenterX - ((float)pMatch->GetPatternWidth() / 2.f));
	rePatternArea.top = (int)GetRoundLong(fCenterY - ((float)pMatch->GetPatternHeight() / 2.f));
	rePatternArea.right = (int)GetRoundLong(fCenterX + ((float)pMatch->GetPatternWidth() / 2.f));
	rePatternArea.bottom = (int)GetRoundLong(fCenterY + ((float)pMatch->GetPatternHeight() / 2.f));
	rePatternArea.NormalizeRect();

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_6"));

	if (rePatternArea.left < 0 || rePatternArea.top < 0 || rePatternArea.right > ImageX.GetWidth() || rePatternArea.bottom > ImageX.GetHeight()
		|| rePatternArea.left > rePatternArea.right || rePatternArea.top > rePatternArea.bottom)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("%s 영역 : 위치 NG"), strType);
		else												clrText.SetText(_T("%s Area : Position NG"), strType);
		clrText.CreateObject(PDC_LIGHTRED, 50, nTextPosY, 35, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_7"));

	m_rcMaskArea_ForLabel[nType] = rePatternArea;

	clrBox.CreateObject(PDC_GREEN, rePatternArea);
	pGO->AddDrawBox(clrBox);

	clrText.SetText(_T("[ %3.1f ]"), fScore*100.f);
	clrText.CreateObject(PDC_GREEN, rePatternArea.CenterPoint(), DEF_FONT_BASIC_SIZE);
	pGO->AddDrawText(clrText);

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("%s 찾기 : OK"), strType);
	else												clrText.SetText(_T("%s Search : OK"), strType);
	clrText.CreateObject(PDC_BLUE, 50, nTextPosY, 35, TRUE);
	pGO->AddDrawText(clrText);

	CVisionSystem::Instance()->WriteMessage(LogTypeNormal, _T("__________Insp_Label_8"));

	return TRUE;
}

BOOL CInspectionVision::MatchModel_BoxQuality(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex) // , float fOkScore)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stBoxInfo& stBoxInfo = CModelInfo::Instance()->GetBoxInfo();

	COLORTEXT clrText;
	COLORBOX clrBox;
	COLORCROSS clrCross;

	CString strException = _T("");
	BOOL bResult = TRUE;
	BOOL bSearchRet[nMATCH_MAX];

	try
	{	
		EImageBW8 ImageX_Origin;
		ImageX_Origin.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		clrBox.CreateObject(PDC_ORANGE, CRect(0, 0, ImageX_Origin.GetWidth() - 1, ImageX_Origin.GetHeight() - 1), 0, 2);
		pGO->AddDrawBox(clrBox);

		for (int i = 0; i < nMATCH_MAX; ++i)
		{
			bSearchRet[i] = TRUE;
			EMatcher* pMatch = m_pMatch[nViewIndex][i];

			if (!pMatch) return FALSE;

			if (!pMatch->GetPatternLearnt()) continue;

			pMatch->SetContrastMode(EMatchContrastMode_Normal);
			pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
			pMatch->SetInterpolate(TRUE);
			pMatch->SetMinScore(0.4f);
			pMatch->SetMaxPositions(1);
			pMatch->SetMaxAngle(5.00f);
			pMatch->SetMinAngle(-5.00f);
			pMatch->SetMaxScale(1.10f);
			pMatch->SetMinScale(0.90f);

			pMatch->Match(&ImageX_Origin);
			
			int nNumMatch = pMatch->GetNumPositions();

			if (!nNumMatch)
			{
				if (CLanguageInfo::Instance()->m_nLangType == 0)	strException.Format(_T("모델 찾기 실패 : 모델_%d"), i);
				else												strException.Format(_T("No Matching Pattern : Model_%d"), i);
				throw strException;
			}

			EMatchPosition& eMatchPos = pMatch->GetPosition(0);
			float fAngle = eMatchPos.Angle;
			float fScale = eMatchPos.Scale;

			CRect rePatternArea;
			rePatternArea.left		= (int)GetRoundLong(eMatchPos.CenterX - ((float)pMatch->GetPatternWidth() / 2.f));
			rePatternArea.top		= (int)GetRoundLong(eMatchPos.CenterY - ((float)pMatch->GetPatternHeight() / 2.f));
			rePatternArea.right		= (int)GetRoundLong(eMatchPos.CenterX + ((float)pMatch->GetPatternWidth() / 2.f));
			rePatternArea.bottom	= (int)GetRoundLong(eMatchPos.CenterY + ((float)pMatch->GetPatternHeight() / 2.f));
			rePatternArea.NormalizeRect();

			clrText.SetText(_T("[Score : %3.1f]"), eMatchPos.Score * 100.f);

			// Matching Score와 비교하여 OK or NG 판정
			if (eMatchPos.Score >= stBoxInfo.fOkScore/100.f)
			{
				clrText.CreateObject(PDC_GREEN, rePatternArea.left + 3, rePatternArea.top + 10, 25, TRUE);
				pGO->AddDrawText(clrText);
				clrBox.CreateObject(PDC_GREEN, rePatternArea, 0, 2);
				pGO->AddDrawBox(clrBox);
				
				bSearchRet[i] = TRUE;
			}
			else
			{
				clrText.CreateObject(PDC_RED, rePatternArea.left + 3, rePatternArea.top + 10, 25, TRUE);
				pGO->AddDrawText(clrText);
				clrBox.CreateObject(PDC_RED, rePatternArea, 0, 2);
				pGO->AddDrawBox(clrBox);
				
				bSearchRet[i] = FALSE;
			}

			// Shift
			BOOL bShiftX, bShiftY;
			bShiftX = bShiftY = FALSE;
			if (bSearchRet[i])
			{
				int nDisX = stBoxInfo.ptMatchCenter[i].x - rePatternArea.CenterPoint().x;
				int nDisY = stBoxInfo.ptMatchCenter[i].y - rePatternArea.CenterPoint().y;

				if (stBoxInfo.nShiftX < abs(nDisX))
					bShiftX = TRUE;
				if (stBoxInfo.nShiftY < abs(nDisY))
					bShiftY = TRUE;

				clrCross.CreateObject(PDC_ORANGE, stBoxInfo.ptMatchCenter[i], 10, PS_SOLID, 1);
				pGO->AddDrawCross(clrCross);

				clrCross.CreateObject( (bShiftX||bShiftY) ? PDC_LIGHTRED : PDC_GREEN, rePatternArea.CenterPoint(), 10, PS_SOLID, 1);
				pGO->AddDrawCross(clrCross);

				clrText.SetText(_T("[Shift X : %d]"), nDisX);
				clrText.CreateObject(bShiftX ? PDC_LIGHTRED : PDC_GREEN, rePatternArea.left + 3, rePatternArea.top + 45, 25, TRUE);
				pGO->AddDrawText(clrText);

				clrText.SetText(_T("[Shift Y : %d]"), nDisY);
				clrText.CreateObject(bShiftY ? PDC_LIGHTRED : PDC_GREEN, rePatternArea.left + 3, rePatternArea.top + 80, 25, TRUE);
				pGO->AddDrawText(clrText);

				if (bShiftX || bShiftY)
					bSearchRet[i] = FALSE;
			}
		}


		for (int i = 0; i < nMATCH_MAX; ++i)
			bResult &= bSearchRet[i];
	}

	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	catch (CString e)
	{
		clrText.SetText(e);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 4, 40, TRUE);
		pGO->AddDrawText(clrText);

		return FALSE;
	}

	return bResult;
}

BOOL CInspectionVision::MatchModel_ForLabelAlign(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stLabelInfo& stLabelInfo= CModelInfo::Instance()->GetLabelInfo();

	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORCROSS clrCross;

	int nTextPosY = 125;

	if (pImgObj == NULL)
		return FALSE;

	int nPageSizeX = pImgObj->GetWidth();
	int nPageSizeY = pImgObj->GetHeight();

	try
	{
		EImageBW8 ImageX;
		ImageX.SetImagePtr(nPageSizeX, nPageSizeY, pImgObj->GetImageBuffer(), pImgObj->GetWidthBytes() * 8);

		if (&ImageX == NULL) return FALSE;

		CRect rcRect;
		rcRect.left = 0;
		rcRect.right = nPageSizeX;
		rcRect.top = 0;
		rcRect.bottom = nPageSizeY;
		rcRect.NormalizeRect();

		EROIBW8 ImageROI;
		ImageROI.Attach(&ImageX);
		ImageROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

		clrBox.CreateObject(PDC_ORANGE, rcRect, PS_SOLID, 1);
		pGO->AddDrawBox(clrBox);

		EMatcher* pMatch = m_pMatch[nViewIndex][0];
		if (!pMatch) return FALSE;

		if (!pMatch->GetPatternLearnt())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Label 기준점 찾기 : NG [등록 모델 없음]"));
			else												clrText.SetText(_T("Label Align Search : NG [No Teaching]"));
			clrText.CreateObject(PDC_LIGHTRED, 50, nTextPosY, 35);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		pMatch->SetContrastMode(EMatchContrastMode_Normal);
		pMatch->SetCorrelationMode(ECorrelationMode_Normalized);
		pMatch->SetInterpolate(TRUE);
		pMatch->SetMinScore(0.6f);
		pMatch->SetMinAngle((5.f*-1));
		pMatch->SetMaxAngle((5.f));
		pMatch->SetMinScale(0.95f);
		pMatch->SetMaxScale(1.05f);
		pMatch->SetMaxPositions(3);
		pMatch->SetMaxInitialPositions(0);

		pMatch->Match(&ImageROI);

		BOOL bSearchRet = TRUE;

		int nCount = pMatch->GetNumPositions();
		if (nCount == 0)
			return FALSE;

		//////////////////////////////////////////////////////////////////////////
		int nNgCnt = 0;
		float fScore = 0.f, fCenterX = 0.f, fCenterY = 0.f;
		BOOL bRatioOK = FALSE;

		for (int nMatchCnt = 0; nMatchCnt < nCount; nMatchCnt++)
		{
			EMatchPosition& eMatchPos = pMatch->GetPosition(nMatchCnt);

			if (bRatioOK == FALSE && eMatchPos.Score < 0.6f)
			{
				nNgCnt++;
			}
			else
			{
				if (fScore < eMatchPos.Score)
				{
					fScore = eMatchPos.Score;
					fCenterX = eMatchPos.CenterX;
					fCenterY = eMatchPos.CenterY;

					if (fScore > 0.6f)
						bRatioOK = TRUE;
				}
			}

			if (nNgCnt == nCount)
			{
				return FALSE;
			}
		}

		CRect rePatternArea;
		rePatternArea.left = (int)GetRoundLong(fCenterX - ((float)pMatch->GetPatternWidth() / 2.f));
		rePatternArea.top = (int)GetRoundLong(fCenterY - ((float)pMatch->GetPatternHeight() / 2.f));
		rePatternArea.right = (int)GetRoundLong(fCenterX + ((float)pMatch->GetPatternWidth() / 2.f));
		rePatternArea.bottom = (int)GetRoundLong(fCenterY + ((float)pMatch->GetPatternHeight() / 2.f));
		rePatternArea.NormalizeRect();

		if (rePatternArea.left < 0 || rePatternArea.top < 0 || rePatternArea.right > ImageX.GetWidth() || rePatternArea.bottom > ImageX.GetHeight()
			|| rePatternArea.left > rePatternArea.right || rePatternArea.top > rePatternArea.bottom)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Align : 위치 NG"));
			else												clrText.SetText(_T("Align : Position NG"));
			clrText.CreateObject(PDC_LIGHTRED, 50, nTextPosY, 35, TRUE);
			pGO->AddDrawText(clrText);

			return FALSE;
		}

		clrBox.CreateObject(PDC_GREEN, rePatternArea);
		pGO->AddDrawBox(clrBox);

		clrText.SetText(_T("[ %3.1f ]"), fScore*100.f);
		clrText.CreateObject(PDC_GREEN, rePatternArea.CenterPoint(), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Align 찾기 : OK"));
		else												clrText.SetText(_T("Align Search : OK"));
		clrText.CreateObject(PDC_BLUE, 50, nTextPosY, 35, TRUE);
		pGO->AddDrawText(clrText);

		clrCross.CreateObject(PDC_GREEN, rePatternArea.CenterPoint(), 10, PS_SOLID, 3);
		pGO->AddDrawCross(clrCross);

		m_ptAlign.SetPoint(rePatternArea.CenterPoint().x, rePatternArea.CenterPoint().y);
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return TRUE;
}

BOOL CInspectionVision::LineGauge( CxImageObject* pImgObj, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice, DPOINT& dptCenter,
								  float fTolerance, float fLength, float fAngle, int nThreshold, int nThickness, int nMinAmplitude, int nMinArea,
								  int nNumFilteringPasses, float fFilteringThreshold, float fSamplingStep,
								  BOOL bHVConstraint,	BOOL bKnownAngle, BOOL bSampleResult,
								  OUT DPOINT& dptResult, OUT double& dResultAngle )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	BOOL bRetVal = FALSE;

	dptResult.x = 0.f;
	dptResult.y = 0.f;
	dResultAngle = 0.f;

	BYTE* pBuffer = (BYTE*)pImgObj->GetImageBuffer();

	if( pBuffer == NULL ) return FALSE;

	int nWidth		= pImgObj->GetWidth();
	int nHeight		= pImgObj->GetHeight();
	int nWidthBytes = pImgObj->GetWidthBytes();

	EImageBW8 eImg;
	eImg.SetImagePtr( nWidth, nHeight, pBuffer, nWidthBytes*8);

	EWorldShape EWorldShape1;
	ELineGauge eLineGauge;

	eLineGauge.Attach(&EWorldShape1);

	eLineGauge.SetAngle(fAngle);
	eLineGauge.SetCenterXY((float)dptCenter.x, (float)dptCenter.y);
	eLineGauge.SetTolerance( fTolerance );
	eLineGauge.SetLength( fLength );
	eLineGauge.SetMinAmplitude(nMinAmplitude); // 5
	eLineGauge.SetMinArea(nMinArea); //0
	eLineGauge.SetThreshold(nThreshold); //5
	eLineGauge.SetThickness(nThickness); //1
	eLineGauge.SetSmoothing(1); //1
	eLineGauge.SetTransitionChoice(eTransitionChoice);
	eLineGauge.SetTransitionType(eTransitionType);
	eLineGauge.SetNumFilteringPasses(nNumFilteringPasses); //1
	eLineGauge.SetFilteringThreshold(fFilteringThreshold); //1.5
	eLineGauge.SetSamplingStep(fSamplingStep); //1
	eLineGauge.SetKnownAngle(bKnownAngle); //TRUE
	eLineGauge.SetHVConstraint(bHVConstraint); //FALSE

	eLineGauge.Measure( &eImg );

	if (bSampleResult)
	{
		int nSampleCnt = eLineGauge.GetNumSamples();
		int nValidSampleCnt = eLineGauge.GetNumValidSamples();

		if( nSampleCnt == 0 || nValidSampleCnt == 0 )
			return FALSE;

		if( nValidSampleCnt < ((double)50/nSampleCnt)*100 )
			return FALSE;
	}

	dptResult.x		= eLineGauge.GetMeasuredLine().GetX();
	dptResult.y		= eLineGauge.GetMeasuredLine().GetY();
	dResultAngle	= eLineGauge.GetMeasuredLine().GetAngle();

	if( (dptResult.x<=0.f) || (dptResult.y<=0.f) )
		bRetVal = FALSE;			
	else
		bRetVal = TRUE;

	return bRetVal;
}

BOOL CInspectionVision::LineGauge2( EImageBW8 pImageX, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice, DPOINT& dptCenter,
									float fTolerance, float fLength, float fAngle, int nThreshold, int nThickness, int nMinAmplitude, int nMinArea,
									int nNumFilteringPasses, float fFilteringThreshold, float fSamplingStep,
									BOOL bHVConstraint, BOOL bKnownAngle, BOOL bSampleResult,
									OUT DPOINT& dptResult, OUT double& dResultAngle)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	BOOL bRetVal = FALSE;

	dptResult.x = 0.f;
	dptResult.y = 0.f;
	dResultAngle = 0.f;

	EImageBW8 eImg;
	eImg.SetSize(pImageX.GetWidth(), pImageX.GetHeight());

	EasyImage::Oper(EArithmeticLogicOperation_Copy, &pImageX, &eImg);

	EWorldShape EWorldShape1;
	ELineGauge eLineGauge;

	eLineGauge.Attach(&EWorldShape1);

	eLineGauge.SetAngle(fAngle);
	eLineGauge.SetCenterXY((float)dptCenter.x, (float)dptCenter.y);
	eLineGauge.SetTolerance(fTolerance);
	eLineGauge.SetLength(fLength);
	eLineGauge.SetMinAmplitude(nMinAmplitude); // 5
	eLineGauge.SetMinArea(nMinArea); //0
	eLineGauge.SetThreshold(nThreshold); //5
	eLineGauge.SetThickness(nThickness); //1
	eLineGauge.SetSmoothing(1); //1
	eLineGauge.SetTransitionChoice(eTransitionChoice);
	eLineGauge.SetTransitionType(eTransitionType);
	eLineGauge.SetNumFilteringPasses(nNumFilteringPasses); //1
	eLineGauge.SetFilteringThreshold(fFilteringThreshold); //1.5
	eLineGauge.SetSamplingStep(fSamplingStep); //1
	eLineGauge.SetKnownAngle(bKnownAngle); //TRUE
	eLineGauge.SetHVConstraint(bHVConstraint); //FALSE

	eLineGauge.Measure(&eImg);

	if (bSampleResult)
	{
		int nSampleCnt = eLineGauge.GetNumSamples();
		int nValidSampleCnt = eLineGauge.GetNumValidSamples();

		if (nSampleCnt == 0 || nValidSampleCnt == 0)
			return FALSE;

		//if (nValidSampleCnt < ((double)50 / nSampleCnt) * 100)
		if (nValidSampleCnt < nSampleCnt * 0.5)
			return FALSE;
	}

	dptResult.x = eLineGauge.GetMeasuredLine().GetX();
	dptResult.y = eLineGauge.GetMeasuredLine().GetY();
	dResultAngle = eLineGauge.GetMeasuredLine().GetAngle();

	if ((dptResult.x <= 0.f) || (dptResult.y <= 0.f))
		bRetVal = FALSE;
	else
		bRetVal = TRUE;

	return bRetVal;
}

BOOL CInspectionVision::LineGauge3(	CxImageObject* pImgObj, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice, DPOINT& dptCenter,
									float fTolerance, float fLength, float fAngle, int nThreshold, int nThickness, int nMinAmplitude, int nMinArea,
									int nNumFilteringPasses, float fFilteringThreshold, float fSamplingStep,
									BOOL bHVConstraint, BOOL bKnownAngle, BOOL bSampleResult,
									OUT DPOINT& dptResult, OUT double& dResultAngle, OUT DPOINT& dptSampleStart, OUT DPOINT& dptSampleEnd)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	BOOL bRetVal = FALSE;

	dptResult.x = 0.f;
	dptResult.y = 0.f;
	dResultAngle = 0.f;

	BYTE* pBuffer = (BYTE*)pImgObj->GetImageBuffer();

	if (pBuffer == NULL) return FALSE;

	int nWidth = pImgObj->GetWidth();
	int nHeight = pImgObj->GetHeight();
	int nWidthBytes = pImgObj->GetWidthBytes();

	EImageBW8 eImg;
	eImg.SetImagePtr(nWidth, nHeight, pBuffer, nWidthBytes * 8);

	EWorldShape EWorldShape1;
	ELineGauge eLineGauge;

	eLineGauge.Attach(&EWorldShape1);

	eLineGauge.SetAngle(fAngle);
	eLineGauge.SetCenterXY((float)dptCenter.x, (float)dptCenter.y);
	eLineGauge.SetTolerance(fTolerance);
	eLineGauge.SetLength(fLength);
	eLineGauge.SetMinAmplitude(nMinAmplitude); // 5
	eLineGauge.SetMinArea(nMinArea); //0
	eLineGauge.SetThreshold(nThreshold); //5
	eLineGauge.SetThickness(nThickness); //1
	eLineGauge.SetSmoothing(1); //1
	eLineGauge.SetTransitionChoice(eTransitionChoice);
	eLineGauge.SetTransitionType(eTransitionType);
	eLineGauge.SetNumFilteringPasses(nNumFilteringPasses); //1
	eLineGauge.SetFilteringThreshold(fFilteringThreshold); //1.5
	eLineGauge.SetSamplingStep(fSamplingStep); //1
	eLineGauge.SetKnownAngle(bKnownAngle); //TRUE
	eLineGauge.SetHVConstraint(bHVConstraint); //FALSE

	eLineGauge.Measure(&eImg);

	if (bSampleResult)
	{
		int nSampleCnt = eLineGauge.GetNumSamples();
		int nValidSampleCnt = eLineGauge.GetNumValidSamples();

		if (nSampleCnt == 0 || nValidSampleCnt == 0)
			return FALSE;

		if ((double)nValidSampleCnt / nSampleCnt < 0.5)
			return FALSE;
	}

	dptResult.x = eLineGauge.GetMeasuredLine().GetX();
	dptResult.y = eLineGauge.GetMeasuredLine().GetY();
	dResultAngle = eLineGauge.GetMeasuredLine().GetAngle();

	if ((dptResult.x <= 0.f) || (dptResult.y <= 0.f))
		bRetVal = FALSE;
	else
		bRetVal = TRUE;

	if (bRetVal)
	{
		EPoint eptStart, eptEnd;
		eptStart = eLineGauge.GetMeasuredLine().GetPoint(-1);
		eptEnd = eLineGauge.GetMeasuredLine().GetPoint(1);

		dptSampleStart.x = eptStart.GetX();
		dptSampleStart.y = eptStart.GetY();
		dptSampleEnd.x = eptEnd.GetX();
		dptSampleEnd.y = eptEnd.GetY();
	}

	return bRetVal;
}

BOOL CInspectionVision::PointGauge(CxImageObject* pImgObj, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice,
	DPOINT& dptCenter, float fTolerance, float fAngle, int nThresholdGV, int nThickness, int nMinAmplitude, OUT DPOINT& dptResult)
{
	BOOL bRetVal = FALSE;

	dptResult.x = 0.f;
	dptResult.y = 0.f;

	BYTE* pBuffer = (BYTE*)pImgObj->GetImageBuffer();
	if (pBuffer == NULL) return FALSE;

	EImageBW8 eImg;
	eImg.SetImagePtr(pImgObj->GetWidth(), pImgObj->GetHeight(), pBuffer);

	EWorldShape EWorldShape1;
	EPointGauge ePointGauge;

	ePointGauge.Attach(&EWorldShape1);
	ePointGauge.SetAngle(fAngle);
	ePointGauge.SetTolerance(fTolerance);
	ePointGauge.SetCenterXY((float)dptCenter.x, (float)dptCenter.y);
	ePointGauge.SetMinAmplitude(nMinAmplitude);
	ePointGauge.SetMinArea(0);
	ePointGauge.SetThreshold(nThresholdGV);
	ePointGauge.SetThickness(nThickness);
	ePointGauge.SetSmoothing(0);
	ePointGauge.SetTransitionChoice(eTransitionChoice);
	ePointGauge.SetTransitionType(eTransitionType);

	ePointGauge.Measure(&eImg);

	int nNumberOfPoints = (int)ePointGauge.GetNumMeasuredPoints();
	if (nNumberOfPoints <= 0) return FALSE;

	if (ePointGauge.GetValid())
	{
		dptResult.x = ePointGauge.GetMeasuredPoint().GetX();
		dptResult.y = ePointGauge.GetMeasuredPoint().GetY();
		bRetVal = TRUE;
	}

	return bRetVal;
}

BOOL CInspectionVision::MaskingArea_Black(EROIBW8 eRoiImage, CRect rcRect, int nGv )
{
	if( rcRect.left   < 0						) rcRect.left	= 0;
	if( rcRect.top    < 0						) rcRect.top	= 0;
	if( rcRect.right  > eRoiImage.GetWidth()	) rcRect.right	= eRoiImage.GetWidth();
	if( rcRect.bottom > eRoiImage.GetHeight()	) rcRect.bottom = eRoiImage.GetHeight();

	rcRect.NormalizeRect();

	int nWidth = eRoiImage.GetWidth();
	int nHeight = eRoiImage.GetHeight();

	for (INT32 x = rcRect.left; x < rcRect.right; x++)
		for (INT32 y = rcRect.top; y < rcRect.bottom; y++)
			eRoiImage.SetPixel(nGv, x, y);

	return TRUE;
}

BOOL CInspectionVision::MaskingArea( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, CRect rcRect )
{
	if( rcRect.left   < 0							) rcRect.left	= 0;
	if( rcRect.top    < 0							) rcRect.top	= 0;
	if( rcRect.right  > pMaskingImgObj.GetWidth()	) rcRect.right	= pMaskingImgObj.GetWidth();
	if( rcRect.bottom > pMaskingImgObj.GetHeight()	) rcRect.bottom = pMaskingImgObj.GetHeight();

	int nWidth = pMaskingImgObj.GetWidth();
	int nHeight = pMaskingImgObj.GetHeight();
	int nWBytes = pMaskingImgObj.GetWidthBytes();

	BYTE* pImgBuffer = (BYTE*)pMaskingImgObj.GetImageBuffer();

	for(int nY = rcRect.top; nY < rcRect.bottom; ++nY)
		for(int nX = rcRect.left; nX < rcRect.right; ++nX)
			pImgBuffer[nY*(nWBytes) + nX] = 255;

	return TRUE;
}

BOOL CInspectionVision::MaskingArea( EImageBW8& pSrc, EImageBW8& pMaskingImgObj, CRect rcRect )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	pMaskingImgObj.SetSize(pSrc.GetWidth(), pSrc.GetHeight());
	EasyImage::Oper(EArithmeticLogicOperation_Copy, &pSrc, &pMaskingImgObj);

	if(rcRect.left   < 0) rcRect.left = 0;
	if(rcRect.top    < 0) rcRect.top  = 0;
	if(rcRect.right  > pMaskingImgObj.GetWidth() ) rcRect.right  = pMaskingImgObj.GetWidth( );
	if(rcRect.bottom > pMaskingImgObj.GetHeight()) rcRect.bottom = pMaskingImgObj.GetHeight();
	rcRect.NormalizeRect();

	for(INT32 x = rcRect.left ; x < rcRect.right ; x++)
		for(INT32 y = rcRect.top; y < rcRect.bottom; y++)
			pMaskingImgObj.SetPixel(255, x, y);	

	return TRUE;
}

BOOL CInspectionVision::CreateLabelData( std::vector<READ_DATA_Label> &LabelData )
{
	if( LabelData.empty() ) return FALSE;

	std::vector<READ_DATA_Label> DataBuff;
	DataBuff.clear();

	// ----- No Segment Data Clear -----
	for( int nIndex = 0; nIndex < (int)LabelData.size(); nIndex++ )
	{
		if((LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::TEXT_SEGMENT			||
			LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_SEGMENT		||
			LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_TEXT_SEGMENT)&&
			LabelData.at(nIndex).strString != _T("")											)
			DataBuff.push_back(LabelData.at(nIndex));
	}

	// 240401 HJ : Ocr Line 걸쳐져서 구성된 Label 추가 됨에 따른 병합 기능 추가
	int nHeightMerge = 5 ;//+ CModelInfo::Instance()->GetLabelInfo().nLabelOcrHeightMerge[CVisionSystem::Instance()->GetLabelNo()];

	// ----- Create Data -----
	int nCheckCodeCount = (int)DataBuff.size();
	std::vector<int> CheckNumber;
	CheckNumber.clear();
	for (int nIndex = 0; nIndex < nCheckCodeCount; nIndex++) CheckNumber.push_back(nIndex);

	std::vector<int> CodePosBuff;
	for( int nIndex = 0; nIndex < nCheckCodeCount; nIndex++ )
	{
		if(CheckNumber.at(nIndex) == -1) continue;

		CodePosBuff.clear();
		int nPos_1 = DataBuff.at(nIndex).cPosition.y;
		for( int nCheck = 0; nCheck < (int)DataBuff.size(); nCheck++ )
		{				
			int nPos_2 = DataBuff.at(nCheck).cPosition.y;
			//			if( nPos_2 - 5 < nPos_1 && nPos_1 < nPos_2 + 5 )
			if( nPos_2 - nHeightMerge < nPos_1 && nPos_1 < nPos_2 + nHeightMerge )
				CodePosBuff.push_back(nCheck);
		}

		if( 1 < (int)CodePosBuff.size() )
		{		
			READ_DATA_Label CreateCodeData;
			CreateCodeData.clear();

			CreateCodeData.DataType = READ_DATA_Label::TEXT_SEGMENT;
			for( int nCheck = 0; nCheck < (int)CodePosBuff.size(); nCheck++ )
			{
				CheckNumber.at(CodePosBuff.at(nCheck)) = -1;

				CreateCodeData.strCode		+= DataBuff.at(CodePosBuff.at(nCheck)).strCode		+ _T(" ");
				CreateCodeData.strString	+= DataBuff.at(CodePosBuff.at(nCheck)).strString	+ _T(" ");
			}
			CreateCodeData.cPosition	= CPoint(-1, DataBuff.at(CodePosBuff.at(0)).cPosition.y);
			CreateCodeData.cSize		= CSize (-1, -1);

			DataBuff.push_back(CreateCodeData);
		}
	}

	CheckNumber.clear();
	CodePosBuff.clear();

	if(DataBuff.empty()) return FALSE;

	LabelData.clear();
	LabelData = DataBuff;
	DataBuff.clear();

	return TRUE;
}

BOOL CInspectionVision::InspeOCR( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, CRect rcLabelRect, std::vector<READ_DATA_Label> LabelData, BOOL bSegmentSave )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	COLORTEXT	clrText;
	COLORBOX	clrBox;

	// ----- Search Segment Area -----
	std::vector<CRect> reSegmentArea;
	reSegmentArea.clear();

	// ----- Font File Check -----
	CString strFontPatch;
	strFontPatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\LabelOCRFont.OCR");
	if (!IsExistFile((LPCTSTR)strFontPatch))
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("폰트 파일이 없습니다."));
		else												clrText.SetText(_T("Font file does not exist."));
		clrText.CreateObject(PDC_LIGHTRED, 700, 300, 35, TRUE);
		pGO->AddDrawText(clrText);

		WRITE_LOG(WL_ERROR, _T("In InspeOCR() - 폰트 파일이 없습니다."));
		return FALSE;
	}

	CModelInfo::stLabelInfo& stLabelInfo = CModelInfo::Instance()->GetLabelInfo();
	MakeMarkAlignedInspectArea(stLabelInfo.stInspectArea, m_ptAlign, 0.f); // Label 회전 이후 -> 각도 0?

	CModelInfo::stLabelInfo::InspVecIter iterBegin = stLabelInfo.stInspectArea.AlignedAreas.begin();
	CModelInfo::stLabelInfo::InspVecIter iterEnd = stLabelInfo.stInspectArea.AlignedAreas.end();

	int nIndex = 0;
	for (CModelInfo::stLabelInfo::InspVecIter iter = iterBegin; iter != iterEnd; ++iter, ++nIndex)
	{
		CRect rcArea = *iter;
		clrBox.CreateObject( PDC_ORANGE, rcArea.left, rcArea.top, rcArea.right, rcArea.bottom);
		clrText.CreateObject(PDC_ORANGE, rcArea.left, rcArea.top, 10);
		clrText.SetText(_T("%d"), nIndex);
		pGO->AddDrawBox(clrBox);
		pGO->AddDrawText(clrText);

		reSegmentArea.push_back(rcArea);		
	}

	// ----- Segment Reading -----
	if (!ReadingSegment(pGO, pMaskingImgObj, reSegmentArea, LabelData, strFontPatch, bSegmentSave))
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("라벨 OCR 정보 불일치 : NG"));
		else												clrText.SetText(_T("Label OCR Data Mismatch : NG"));
		clrText.CreateObject(PDC_LIGHTRED, 700, 300, 35, TRUE);
		pGO->AddDrawText(clrText);

		reSegmentArea.clear();
		return FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Label OCR 정보 일치 : OK"));
	else												clrText.SetText(_T("Label OCR Data Match : OK"));
	clrText.CreateObject(PDC_BLUE, 700, 300, 35, TRUE);
	pGO->AddDrawText(clrText);

	reSegmentArea.clear();

	return TRUE;
}

BOOL CInspectionVision::ReadingSegment( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, std::vector<CRect> reSegmentArea, std::vector<READ_DATA_Label> LabelData, CString strFontPatch, BOOL bSegmentSave )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	BOOL bResult = TRUE;
	m_LabelOCRData.clear();

	// ----- Check Segment Data -----
	std::vector<READ_DATA_Label> MasterData;
	MasterData.clear();
	for( int nIndex = 0; nIndex < (int)LabelData.size(); nIndex++ )
	{
		if((LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::TEXT_SEGMENT				||
			LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_TEXT_SEGMENT		)&&
			LabelData.at(nIndex).strString != _T("")												)
			MasterData.push_back(LabelData.at(nIndex));	
	}
	if( !LabelData.empty() && MasterData.empty() ) return FALSE;


	// ----- Save Segment -----
	USES_CONVERSION;
	CString strSegmentSavePath = _T("D:\\Label_Segment");
	if( IsExistFile( (LPCTSTR)strSegmentSavePath) ) DeleteDirectory( (LPCTSTR)strSegmentSavePath );

	// ----- Image Format Conversion & Copy -----
	EImageBW8 BW8LabelImg;
	BW8LabelImg.SetImagePtr( pMaskingImgObj.GetWidth(), pMaskingImgObj.GetHeight(), pMaskingImgObj.GetImageBuffer(), pMaskingImgObj.GetWidthBytes()*8 );

	EROIBW8 BW8ROISegmentImg;
	BW8ROISegmentImg.Attach(&BW8LabelImg);

	// ----- Algorithm Start -----
	ECodedImage eCodedImage_Segment;
	EListItem*  pObject_Segment;

	eCodedImage_Segment.SetWhiteClass( 0 );
	eCodedImage_Segment.SetBlackClass( 1 );	
	eCodedImage_Segment.SetConnexity( EConnexity_Connexity8 );

	VisionProcess::LabelOCRData OCRDataBuff; 

	for( int nSegmentLineNum = 0; nSegmentLineNum < (int)reSegmentArea.size(); nSegmentLineNum++ )
	{	
//		if( CVisionSystem::Instance()->GetLabelNo() == 0 && nSegmentLineNum == 4 ) continue;

		CRect reLineAreaBuff = reSegmentArea.at(nSegmentLineNum);
		reLineAreaBuff.DeflateRect(5,0,5,0); // Edge Pass Area Set

		OCRDataBuff.clear();
		OCRDataBuff.reLineArea = reLineAreaBuff;

		// ----- Set ROI Area -----
		int nLineOffset_Height = (int)( (float)reLineAreaBuff.Height() * 0.25f );
		reLineAreaBuff.InflateRect(0, nLineOffset_Height, 0, nLineOffset_Height);
		BW8ROISegmentImg.SetPlacement( reLineAreaBuff.left, reLineAreaBuff.top, reLineAreaBuff.Width(), reLineAreaBuff.Height());

		// ----- Image Data Type Conversion (EROIBW8 -> EImageBW8) -----
		EImageBW8 BW8SegmentOrgBuff; 
		BW8SegmentOrgBuff.SetSize(BW8ROISegmentImg.GetWidth(), BW8ROISegmentImg.GetHeight());
		EasyImage::Oper(EArithmeticLogicOperation_Copy, &BW8ROISegmentImg, &BW8SegmentOrgBuff);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CRect reMaskingArea = CRect(0,0,BW8SegmentOrgBuff.GetWidth(),0);
		nLineOffset_Height = (int)( (float)reLineAreaBuff.Height() * 0.1f );

		reMaskingArea.bottom = nLineOffset_Height - 1;
		MaskingArea( BW8SegmentOrgBuff, BW8SegmentOrgBuff, reMaskingArea );

		reMaskingArea.top = BW8SegmentOrgBuff.GetHeight() - (nLineOffset_Height - 1);
		reMaskingArea.bottom = BW8SegmentOrgBuff.GetHeight();
		MaskingArea( BW8SegmentOrgBuff, BW8SegmentOrgBuff, reMaskingArea );

		// ----- Erase Around Segment -----
		float fMinPosition_Y	= (float)BW8ROISegmentImg.GetHeight() * 0.15f;// 0.25f;
		float fMaxPosition_Y	= (float)BW8ROISegmentImg.GetHeight() * 0.80f;// 0.75f;
		float fMinHeight		= (float)reSegmentArea.at(nSegmentLineNum).Height() * 0.06f; // 0.10f;

		int nArea;
		float fWidth, fHeight, fCenterX, fCenterY;

		int nThreshold = EasyImage::AutoThreshold( &BW8ROISegmentImg, EThresholdMode_MinResidue ).Value;
		if(nThreshold - 30 > 0) nThreshold -= 30;

		eCodedImage_Segment.SetThreshold( nThreshold		);		
		eCodedImage_Segment.BuildObjects( &BW8SegmentOrgBuff ); //BW8ROISegmentImg

		int nObjectCount = eCodedImage_Segment.GetNumSelectedObjects();
		if( nObjectCount <= 0 ) continue;//return FALSE; // ----- Segment Data Check (Font로 인식 못한 경우 Line Pass // Line 인식은 했지만 실제로 폰트는 없는 경우)-----

		eCodedImage_Segment.AnalyseObjects( ELegacyFeature_Area, ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight, ELegacyFeature_LimitCenterX, ELegacyFeature_LimitCenterY );
		pObject_Segment = eCodedImage_Segment.GetFirstObjPtr();

		for(int nObjectIndex = 0; nObjectIndex < nObjectCount; nObjectIndex++)
		{		
			eCodedImage_Segment.GetObjectFeature( ELegacyFeature_LimitWidth,		pObject_Segment, fWidth		);
			eCodedImage_Segment.GetObjectFeature( ELegacyFeature_LimitHeight,		pObject_Segment, fHeight	);
			eCodedImage_Segment.GetObjectFeature( ELegacyFeature_LimitCenterX,		pObject_Segment, fCenterX	);
			eCodedImage_Segment.GetObjectFeature( ELegacyFeature_LimitCenterY,		pObject_Segment, fCenterY	);
			eCodedImage_Segment.GetObjectFeature( ELegacyFeature_Area,				pObject_Segment, nArea		);

			CRect reBlobArea;
			reBlobArea.left   = (int)GetRoundLong( fCenterX - (fWidth  / 2.f) );
			reBlobArea.top    = (int)GetRoundLong( fCenterY - (fHeight / 2.f) );
			reBlobArea.right  = (int)GetRoundLong( fCenterX + (fWidth  / 2.f) );
			reBlobArea.bottom = (int)GetRoundLong( fCenterY + (fHeight / 2.f) );

			reBlobArea.InflateRect(1, 2, 1, 2);
			reBlobArea.NormalizeRect();

			if( !(fMinPosition_Y < fCenterY && fCenterY < fMaxPosition_Y) || fHeight < fMinHeight || nArea < 20 )
				MaskingArea( BW8SegmentOrgBuff, BW8SegmentOrgBuff, reBlobArea );
			else
			{
				if( (int)OCRDataBuff.reSegmentArea.size() == 0 ) OCRDataBuff.reSegmentArea.push_back(reBlobArea);
				else
				{
					CRect rePrevArea = OCRDataBuff.reSegmentArea.at( (int)OCRDataBuff.reSegmentArea.size()-1 );

					if( rePrevArea.CenterPoint().x - 3 <= reBlobArea.CenterPoint().x && reBlobArea.CenterPoint().x <= rePrevArea.CenterPoint().x + 3 )
					{
						if( reBlobArea.left		< rePrevArea.left	) rePrevArea.left	= reBlobArea.left;
						if( reBlobArea.top		< rePrevArea.top	) rePrevArea.top	= reBlobArea.top;
						if( reBlobArea.right	> rePrevArea.right	) rePrevArea.right	= reBlobArea.right;
						if( reBlobArea.bottom	> rePrevArea.bottom ) rePrevArea.bottom = reBlobArea.bottom;

						OCRDataBuff.reSegmentArea.at( (int)OCRDataBuff.reSegmentArea.size()-1 ) = rePrevArea;
					}
					else OCRDataBuff.reSegmentArea.push_back(reBlobArea);
				}
			}

			pObject_Segment = eCodedImage_Segment.GetNextObjPtr( pObject_Segment );	
		}

		// ----- Segment Data Check (Font로 인식 못한 경우 Line Pass)-----
		if ( (int)OCRDataBuff.reSegmentArea.size() <= 0 ) continue;

		// ----- Segment Data Classification -----
		float fAverage_Width, fAverage_Height, fDispersion_Width, fDispersion_Height, fVariance_Width, fVariance_Height;
		fAverage_Width = fAverage_Height = fDispersion_Width = fDispersion_Height = fVariance_Width = fVariance_Height = 0.f;

		for( int nDataIndex = 0; nDataIndex < (int)OCRDataBuff.reSegmentArea.size(); nDataIndex++ )
		{
			fAverage_Width	+= OCRDataBuff.reSegmentArea.at(nDataIndex).Width();
			fAverage_Height += OCRDataBuff.reSegmentArea.at(nDataIndex).Height();
		}

		fAverage_Width	/= (float)OCRDataBuff.reSegmentArea.size();
		fAverage_Height	/= (float)OCRDataBuff.reSegmentArea.size();

		for( int nDataIndex = 0; nDataIndex < (int)OCRDataBuff.reSegmentArea.size(); nDataIndex++ )
		{
			fDispersion_Width  += pow( OCRDataBuff.reSegmentArea.at(nDataIndex).Width()  - fAverage_Width,  2 );
			fDispersion_Height += pow( OCRDataBuff.reSegmentArea.at(nDataIndex).Height() - fAverage_Height, 2 );
		}

		fDispersion_Width  /= (float)OCRDataBuff.reSegmentArea.size();
		fDispersion_Height /= (float)OCRDataBuff.reSegmentArea.size(); 

		fVariance_Width  = sqrt(fDispersion_Width );                          
		fVariance_Height = sqrt(fDispersion_Height);  

		float fVarianceRatio_Width = 1.5f, fVarianceRatio_Height = 1.5f;

		CSize FontMinSize = CSize( (int)( fAverage_Width  - (fVariance_Width  * fVarianceRatio_Width ) ), (int)( fAverage_Height - (fVariance_Height * fVarianceRatio_Height) ) );
		CSize FontMaxSize = CSize( (int)( fAverage_Width  + (fVariance_Width  * fVarianceRatio_Width ) ), (int)( fAverage_Height + (fVariance_Height * fVarianceRatio_Height) ) );

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		EImageBW8 BW8SegmentCutBuff, BW8SegmentCenterCutBuff;
		BW8SegmentCutBuff.SetSize(BW8SegmentOrgBuff.GetWidth(), BW8SegmentOrgBuff.GetHeight());
		EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentOrgBuff, &BW8SegmentCutBuff );

		BW8SegmentCenterCutBuff.SetSize(BW8SegmentOrgBuff.GetWidth(), BW8SegmentOrgBuff.GetHeight());
		EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentOrgBuff, &BW8SegmentCenterCutBuff );

		EROIBW8 EBW8ROICutBuff, EBW8ROICenterCutBuff;  
		EBW8ROICutBuff.Attach(&BW8SegmentCutBuff);
		EBW8ROICenterCutBuff.Attach(&BW8SegmentCenterCutBuff);

		BOOL bUseCuttingInsp = FALSE;
		for( int nDataIndex = 0; nDataIndex < (int)OCRDataBuff.reSegmentArea.size(); nDataIndex++ )
		{
			CRect reAreaBuff = OCRDataBuff.reSegmentArea.at(nDataIndex);

			int  nFontType_Width  = 0, nFontType_Height = 0;

			// ----- Check Width -----		
			if( 1.f < fVariance_Width && (reAreaBuff.Width() < FontMinSize.cx || FontMaxSize.cx < reAreaBuff.Width()) )
			{
				if		( reAreaBuff.Width() < FontMinSize.cx )	nFontType_Width =  1;
				else if ( FontMaxSize.cx < reAreaBuff.Width() )	nFontType_Width =  2;
				else											nFontType_Width = -1;
			}

			// ----- Check Height -----
			if( 1.f < fVariance_Height && (reAreaBuff.Height() < FontMinSize.cy || FontMaxSize.cy < reAreaBuff.Height()) )
			{
				if		( reAreaBuff.Height() < FontMinSize.cy )	nFontType_Height =  1;
				else if ( FontMaxSize.cy < reAreaBuff.Height() )	nFontType_Height =  2;
				else												nFontType_Height = -1;
			}

			BOOL bSegmentCutting = FALSE;
			if		( nFontType_Width == 0 && nFontType_Height == 0 ) { }
			else if ( nFontType_Width == 0 && nFontType_Height == 1 ) { }
			else if ( nFontType_Width == 1 && nFontType_Height == 0 ) { }
			else if ( nFontType_Width == 1 && nFontType_Height == 1 ) { }
			else if ( nFontType_Width == 0 && nFontType_Height == 2 ) { }
			else if ( nFontType_Width == 1 && nFontType_Height == 2 ) { }
			else if ( nFontType_Width == 2 && nFontType_Height == 0 ) { if( (int)((float)FontMaxSize.cx * 1.1f) < reAreaBuff.Width()) bSegmentCutting = TRUE; }
			else if ( nFontType_Width == 2 && nFontType_Height == 1 ) { if( (int)((float)FontMaxSize.cx * 1.1f) < reAreaBuff.Width()) bSegmentCutting = TRUE; }	
			else if ( nFontType_Width == 2 && nFontType_Height == 2 ) { }
			else {}

			if (bSegmentCutting)
			{
				bUseCuttingInsp = TRUE;

				EBW8ROICutBuff.SetPlacement(reAreaBuff.left, reAreaBuff.top, reAreaBuff.Width(), reAreaBuff.Height());
				CuttingSegment(pGO, EBW8ROICutBuff, reAreaBuff, 0);

				EBW8ROICenterCutBuff.SetPlacement(reAreaBuff.left, reAreaBuff.top, reAreaBuff.Width(), reAreaBuff.Height());
				CuttingSegment(pGO, EBW8ROICenterCutBuff, reAreaBuff, 1);
			}
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////
		BOOL bOCRRead = FALSE;
		for( int nImageTypeIndex = 0; nImageTypeIndex < LABLE_INSPECTION_MAX_IMAGE_TYPE; nImageTypeIndex++)
		{
			SegmentReadingOption stReadingSet; stReadingSet.clear();
			stReadingSet.nImageType = nImageTypeIndex;

			////////////////////////////////////////////////////////////////////////////////////////////////////
			EImageBW8 BW8InspImgBuff;
			switch (nImageTypeIndex) // 0:Center Cut, 1:Algorithm Cut, 2:OneThird, 3:TwoThirds, 4:Original
			{
			case 0:
				if(!bUseCuttingInsp) continue;
				BW8InspImgBuff.SetSize(BW8SegmentCenterCutBuff.GetWidth(), BW8SegmentCenterCutBuff.GetHeight());
				EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentCenterCutBuff, &BW8InspImgBuff );
				break;

			case 1:
				if(!bUseCuttingInsp) continue;
				BW8InspImgBuff.SetSize(BW8SegmentCutBuff.GetWidth(), BW8SegmentCutBuff.GetHeight());
				EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentCutBuff, &BW8InspImgBuff );
				break;

			case 2:
				BW8InspImgBuff.SetSize(BW8SegmentOrgBuff.GetWidth(), BW8SegmentOrgBuff.GetHeight());
				EasyImage::Oper( EArithmeticLogicOperation_Copy, &BW8SegmentOrgBuff, &BW8InspImgBuff );
				break;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////			
			for( int nReadingCase = 0; nReadingCase < OP_MAX; nReadingCase++ )
			{
				CModelInfo::stLabelManualOptionInfo& stLabelOptionInfo = CModelInfo::Instance()->GetLabelManualOptionInfo( (Labe_Reading_Option)nReadingCase );
				if( stLabelOptionInfo.nImageTypeIndex == -1 || stLabelOptionInfo.nSegmentLine_Num == -1 ) continue;
				if( /*nImageTypeIndex != stLabelOptionInfo.nImageTypeIndex ||*/ nSegmentLineNum != stLabelOptionInfo.nSegmentLine_Num ) continue;

				OCRDataBuff.clear();
				OCRDataBuff.reLineArea = reLineAreaBuff;

				stReadingSet.nOptionIndex				= nReadingCase;
				stReadingSet.bUseFontsizeManualSetting	= TRUE;
				stReadingSet.bTeachingMode				= FALSE;

				stReadingSet.bUseMaxThreshold			= stLabelOptionInfo.bUseMaxThreshold; 
				stReadingSet.bUseImageFiltering			= stLabelOptionInfo.bUseImageFiltering; 
				stReadingSet.bUseFontSizeFiltering		= FALSE; 
				stReadingSet.bUseeVisionLargeCharsCut	= stLabelOptionInfo.bUseeVisionLargeCharsCut;

				stReadingSet.FontMinSize = stLabelOptionInfo.FontMinSize; 
				stReadingSet.FontMaxSize = stLabelOptionInfo.FontMaxSize;

				bOCRRead = EVisionOCRRead( BW8InspImgBuff, OCRDataBuff, MasterData, strFontPatch, stReadingSet );
				if( bOCRRead ) break;
			}
			if( bOCRRead ) break;

			////////////////////////////////////////////////////////////////////////////////////////////////////
			for( int nReadingCase = 0; nReadingCase < LABLE_INSPECTION_MAX_OPTUION; nReadingCase++ )
			{
				OCRDataBuff.clear();
				OCRDataBuff.reLineArea = reLineAreaBuff;

				stReadingSet.nOptionIndex				= nReadingCase;
				stReadingSet.bUseFontsizeManualSetting	= FALSE;
				stReadingSet.bTeachingMode				= FALSE;

				switch (nReadingCase)
				{
				case  0: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case  1: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				case  2: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case  3: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				case  4: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case  5: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				case  6: //stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case  7: break;//stReadingSet.bUseMaxThreshold = FALSE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;				
				case  8: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case  9: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				case 10: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case 11: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering = FALSE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				case 12: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case 13: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering = FALSE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				case 14: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut = FALSE; break;
				case 15: stReadingSet.bUseMaxThreshold =  TRUE; stReadingSet.bUseImageFiltering =  TRUE; stReadingSet.bUseFontSizeFiltering =  TRUE; stReadingSet.bUseeVisionLargeCharsCut =  TRUE; break;
				} 

				bOCRRead = EVisionOCRRead( BW8InspImgBuff, OCRDataBuff, MasterData, strFontPatch, stReadingSet );
				if( bOCRRead ) break;
			}
			if( bOCRRead ) break;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		if(!bOCRRead /*|| TRUE*/)
		{
			bResult = FALSE;

			if( !bSegmentSave )	// --- Auto Run Inspection ---
			{
				break;
			}		
			else				// --- Manual Inspection ---
			{
				if( !IsExistFile ( (LPCTSTR)strSegmentSavePath) ) MakeDirectory( (LPCTSTR)strSegmentSavePath );	

				CString strImgName;
				strImgName.Format(_T("\\Segment_%02d_%03d%03d%03d%03d"), nSegmentLineNum, OCRDataBuff.FontMinSize.cx, OCRDataBuff.FontMaxSize.cx, OCRDataBuff.FontMinSize.cy, OCRDataBuff.FontMaxSize.cy );
				strImgName = strSegmentSavePath + strImgName + _T("_Org.bmp");
				BW8SegmentOrgBuff.Save( T2A((LPTSTR)(LPCTSTR)strImgName) );

				if(bUseCuttingInsp)
				{
					strImgName.Format(_T("\\Segment_%02d_%03d%03d%03d%03d"), nSegmentLineNum, OCRDataBuff.FontMinSize.cx, OCRDataBuff.FontMaxSize.cx, OCRDataBuff.FontMinSize.cy, OCRDataBuff.FontMaxSize.cy );
					strImgName = strSegmentSavePath + strImgName + _T("_Cut.bmp");
					BW8SegmentCutBuff.Save( T2A((LPTSTR)(LPCTSTR)strImgName) );

					strImgName.Format(_T("\\Segment_%02d_%03d%03d%03d%03d"), nSegmentLineNum, OCRDataBuff.FontMinSize.cx, OCRDataBuff.FontMaxSize.cx, OCRDataBuff.FontMinSize.cy, OCRDataBuff.FontMaxSize.cy );
					strImgName = strSegmentSavePath + strImgName + _T("_CenterCut.bmp");
					BW8SegmentCenterCutBuff.Save( T2A((LPTSTR)(LPCTSTR)strImgName) );
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	// ----- Result Draw Object -----
	if( (int)m_LabelOCRData.size() <= 0 ) return FALSE;

	COLORBOX  clrBox;
	COLORTEXT clrText;
	COLORREF  clrDraw_color;

	for(int nLineIndex = 0; nLineIndex < (int)m_LabelOCRData.size(); nLineIndex++)
	{
		OCRDataBuff.clear();
		OCRDataBuff = m_LabelOCRData.at(nLineIndex);

		if( !OCRDataBuff.bInspoResult )
		{
			clrBox.CreateObject( PDC_LIGHTRED, OCRDataBuff.reLineArea, PS_DASH, 4 );	
			pGO->AddDrawBox(clrBox);
		}

		for(int nSegmentIndex = 0; nSegmentIndex < (int)OCRDataBuff.reSegmentArea.size(); nSegmentIndex++)
		{	
			CRect reAreaBuff = OCRDataBuff.reSegmentArea.at(nSegmentIndex);

			if( OCRDataBuff.bInspoResult )
			{
				clrDraw_color = PDC_BLUE;

				if( OCRDataBuff.bSegmentScoreResult.at(nSegmentIndex) ) 
					clrText.SetText( _T("%s"), OCRDataBuff.strSegmentText.at(nSegmentIndex) ); // Segment Score > 30
				else 
				{
					clrText.SetText( _T("%s(%.1f)"), OCRDataBuff.strSegmentText.at(nSegmentIndex), OCRDataBuff.fSegmentScore.at(nSegmentIndex) );
					clrDraw_color = PDC_ORANGE;

					////////////////////////////////////////////////////////////////////////////////////////////////////
					// Score 미달 Font Save
					//CRect reFontROI = reAreaBuff;
					//reFontROI.InflateRect(5,5,5,5);
					//reFontROI.NormalizeRect();
					//if(reFontROI.left	<  0						) reFontROI.left	= 0;
					//if(reFontROI.top	<  0						) reFontROI.top		= 0;
					//if(reFontROI.right	>= BW8LabelImg.GetWidth()	) reFontROI.right	= BW8LabelImg.GetWidth() -1;
					//if(reFontROI.bottom >= BW8LabelImg.GetHeight()	) reFontROI.bottom	= BW8LabelImg.GetHeight()-1;

					//EROIBW8 EBW8FontROI;  
					//EBW8FontROI.Attach(&BW8LabelImg);
					//EBW8FontROI.SetPlacement(reFontROI.left, reFontROI.top, reFontROI.Width(), reFontROI.Height());

					//CString strFontSaveName;
					//strFontSaveName.Format( _T("D:\\FontTeaching\\%s_%.2f_%d_%d.bmp"),OCRDataBuff.strSegmentText.at(nSegmentIndex), OCRDataBuff.fSegmentScore.at(nSegmentIndex), nLineIndex, nSegmentIndex );
					//EBW8FontROI.Save(T2A((LPTSTR)(LPCTSTR)strFontSaveName));
					////////////////////////////////////////////////////////////////////////////////////////////////////
				}
			}
			else
			{
				clrDraw_color = PDC_BLUE;
				float fScore = OCRDataBuff.fSegmentScore.at(nSegmentIndex);
				if		( fScore <=  30.f ) clrDraw_color = PDC_LIGHTRED;
				else if ( fScore <=  60.f ) clrDraw_color = PDC_ORANGE;
				else if ( fScore <=  80.f ) clrDraw_color = PDC_GREEN;

				clrText.SetText( _T("%s (%.1f)"), OCRDataBuff.strSegmentText.at(nSegmentIndex), fScore );
			}		

			clrText.CreateObject( clrDraw_color, reAreaBuff.CenterPoint().x, reAreaBuff.CenterPoint().y, 25, TRUE, CxGOText::TextAlignmentCenter );
			pGO->AddDrawText(clrText);

			clrBox.CreateObject( clrDraw_color, reAreaBuff, PS_DASH, 1 );
			pGO->AddDrawBox(clrBox);
		}
	}

	return bResult;
}

BOOL CInspectionVision::CuttingSegment( CxGraphicObject* pGO, EROIBW8& BW8CuttingImg, CRect reSegmentArea, int nCutting )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	CRect reImgArea = CRect(0, 0, BW8CuttingImg.GetWidth(), BW8CuttingImg.GetHeight());

	// 240404 HJ : 1/3cut, 2/3cut 검사 추가로 인해 BOOL -> int로 변경하여 관리
	if( nCutting != 0 )
	{
		// Center Cut
		if ( nCutting == 1 )
		{
			int nPositionX = reImgArea.CenterPoint().x;

			for(int nPositionY = reImgArea.top; nPositionY < reImgArea.bottom; nPositionY++)
				BW8CuttingImg.SetPixel( 255, nPositionX, nPositionY );
		}
		// OneThird Cut
		if ( nCutting == 2 )
		{
			int nPositionX = GetRoundShort(reImgArea.Width() / 3);

			for(int nPositionY = reImgArea.top; nPositionY < reImgArea.bottom; nPositionY++)
				BW8CuttingImg.SetPixel( 255, nPositionX, nPositionY );
		}

		// TwoThirds Cut
		if ( nCutting == 3 )
		{
			int nPositionX = reImgArea.Width() - GetRoundShort(reImgArea.Width() / 3);

			for(int nPositionY = reImgArea.top; nPositionY < reImgArea.bottom; nPositionY++)
				BW8CuttingImg.SetPixel( 255, nPositionX, nPositionY );
		}

		return TRUE;
	}

	EImageBW8 BW8ImgTemp;
	BW8ImgTemp.SetSize( reImgArea.Width(), reImgArea.Height() );
	EasyImage::Threshold( &BW8CuttingImg, &BW8ImgTemp, EThresholdMode_MinResidue);

	float fImgSelectAvg = 0.f;
	EasyImage::PixelAverage( &BW8ImgTemp, fImgSelectAvg );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	EBW8Vector EBW8ProjectionVector;  
	EasyImage::ProjectOnARow( &BW8ImgTemp, &EBW8ProjectionVector );

	int nProjectionData = 0;
	int nProjectionCnt = EBW8ProjectionVector.GetNumElements();
	if( nProjectionCnt < 10 ) return FALSE;	

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<int> ProjectionPosition; ProjectionPosition.clear();

	int nStartPos = (int)((float)reImgArea.Width()*0.1f), nEndPos = (int)((float)reImgArea.Width()*0.9f), nCheckCount = 0;
	for(int nIndex = nStartPos; nIndex <= nEndPos; nIndex++)
	{
		nProjectionData = EBW8ProjectionVector.GetElement(nIndex).Value;
		if( (int)fImgSelectAvg < nProjectionData ) nCheckCount++;

		if( nCheckCount < 2 ) continue;

		if( nIndex + 1 <= nProjectionCnt - 5 )
			if( (int)fImgSelectAvg < nProjectionData ) continue;

		int nFirstPosition		= nIndex - (nCheckCount - 1);
		int nMiddlePosition		= nIndex - (int)( (float)(nCheckCount - 1) / 2.f );
		int nLastPosition		= nIndex;
		int nMaximumPosition	= nIndex, nMaxValue = nProjectionData;
		int nMinimumPosition	= nIndex, nMinValue = nProjectionData;

		for(int nCnt = 1; nCnt < nCheckCount; nCnt++)
		{
			nProjectionData = EBW8ProjectionVector.GetElement(nIndex - nCnt).Value;

			if( nMaxValue < nProjectionData )
			{
				nMaximumPosition = nIndex - nCnt;
				nMaxValue = nProjectionData;
			}
			else if( nMaxValue == nProjectionData )
				nMaximumPosition = nIndex - nCnt;

			if( nMinValue > nProjectionData )
			{
				nMinimumPosition = nIndex - nCnt;
				nMinValue = nProjectionData;
			}
			else if( nMinValue == nProjectionData )
				nMinimumPosition = nIndex - nCnt;
		}

		ProjectionPosition.push_back( nFirstPosition   );
		ProjectionPosition.push_back( nMiddlePosition  );
		ProjectionPosition.push_back( nLastPosition	   );
		ProjectionPosition.push_back( nMaximumPosition );
		ProjectionPosition.push_back( nMinimumPosition );

		nCheckCount = 0;	
	}
	sort(ProjectionPosition.begin(), ProjectionPosition.end());
	ProjectionPosition.erase( unique(ProjectionPosition.begin(), ProjectionPosition.end()), ProjectionPosition.end() );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<SegmentCuttingData> CuttingData; CuttingData.clear();
	SegmentCuttingData DataBuff;

	for(int nIndex = 0; nIndex < (int)ProjectionPosition.size(); nIndex++)
	{
		DataBuff.clear();

		int nPosition_X = ProjectionPosition.at(nIndex);
		BOOL bSpaceChange = FALSE;

		for(int nPosition_Y = reImgArea.top; nPosition_Y < reImgArea.bottom; nPosition_Y++)
		{
			int nPixelValue = BW8CuttingImg.GetPixel( nPosition_X, nPosition_Y ).Value;

			BOOL bSpaceChangeBuf = bSpaceChange;
			if( nPixelValue < 180 ) 
			{
				bSpaceChange = TRUE;
				DataBuff.fPrintingRatio++;
			}
			else bSpaceChange = FALSE;

			if( bSpaceChangeBuf != bSpaceChange ) DataBuff.nSpaceChangeCount ++;
		}
		DataBuff.fPrintingRatio = (DataBuff.fPrintingRatio / (float)reImgArea.Height()) * 100.f;

		//////////////////////////////////////////////////	
		BOOL bPrintingCheck = TRUE;
		if( DataBuff.nSpaceChangeCount <= 2 )
		{
			if( !( 5.f < DataBuff.fPrintingRatio && DataBuff.fPrintingRatio < 30.f) ) bPrintingCheck = FALSE;
		}
		else
		{
			if( !(30.f < DataBuff.fPrintingRatio && DataBuff.fPrintingRatio < 55.f) ) bPrintingCheck = FALSE;
		}

		BOOL bSpeceCheck = TRUE;
		if( 4 < DataBuff.nSpaceChangeCount ) bSpeceCheck = FALSE;

		//////////////////////////////////////////////////
		if( !bPrintingCheck || !bSpeceCheck ) continue;

		DataBuff.nSelectPos = nPosition_X;
		if( 0 < (int)CuttingData.size() )
			if( DataBuff.nSelectPos == CuttingData.at((int)CuttingData.size() - 1).nSelectPos + 1 )		
				CuttingData.at((int)CuttingData.size() - 1).bContinue = DataBuff.bContinue = TRUE;

		CuttingData.push_back(DataBuff);	
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	BOOL bFindPos = TRUE;
	int  nPositionX = 0;

	if (	 (int)CuttingData.size() <= 0 ) nPositionX = reImgArea.CenterPoint().x,		bFindPos = FALSE;		
	else if( (int)CuttingData.size() == 1 )	nPositionX = CuttingData.at(0).nSelectPos,	bFindPos = TRUE;
	else
	{
		for(int nIndex = 0; nIndex < (int)CuttingData.size(); nIndex++)
		{
			if( !CuttingData.at(nIndex).bContinue )
			{
				bFindPos = FALSE;	
				break;
			}		
		}

		//////////////////////////////////////////////////
		int nMinPos = 0; float fMinValue = 0.f, fCalcVlaue= 0.f; BOOL bContinueCheck = FALSE;
		for(int nIndex = 0; nIndex < (int)CuttingData.size(); nIndex++)
		{
			if( bFindPos || (!bFindPos && (int)CuttingData.size() <= 3) )	fCalcVlaue = CuttingData.at(nIndex).fPrintingRatio / (float)CuttingData.at(nIndex).nSpaceChangeCount;
			else															fCalcVlaue = (float)abs( CuttingData.at(nIndex).nSelectPos - (int)((float)reImgArea.CenterPoint().x / 2.f) );

			if( CuttingData.at(nIndex).bContinue ) bContinueCheck = TRUE;

			if( nIndex == 0 )
			{
				fMinValue = fCalcVlaue;
				nMinPos	  = nIndex;
				continue;
			}

			if( fCalcVlaue < fMinValue )
			{
				fMinValue = fCalcVlaue;
				nMinPos	  = nIndex;
			}
		}

		//////////////////////////////////////////////////
		if( !bFindPos && bContinueCheck && !CuttingData.at(nMinPos).bContinue )
		{
			fMinValue = CuttingData.at(nMinPos).fPrintingRatio / (float)CuttingData.at(nMinPos).nSpaceChangeCount;

			for(int nIndex = 0; nIndex < (int)CuttingData.size(); nIndex++)
			{
				if(!CuttingData.at(nIndex).bContinue) continue;
				if( CuttingData.at(nMinPos).nSpaceChangeCount < CuttingData.at(nIndex).nSpaceChangeCount ) continue;

				fCalcVlaue = CuttingData.at(nIndex).fPrintingRatio / (float)CuttingData.at(nIndex).nSpaceChangeCount;
				if( fCalcVlaue < fMinValue )
				{
					fMinValue = fCalcVlaue;
					nMinPos	  = nIndex;
				}
			}
		}

		nPositionX = CuttingData.at(nMinPos).nSelectPos;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int nPositionY = reImgArea.top; nPositionY < reImgArea.bottom; nPositionY++)
		BW8CuttingImg.SetPixel( 255, nPositionX, nPositionY );

	ProjectionPosition.clear();
	CuttingData.clear();
	DataBuff.clear();

	return TRUE;
}

BOOL CInspectionVision::EVisionOCRRead( EImageBW8 SegmentImage, LabelOCRData& LabelData, std::vector<READ_DATA_Label> &MasterData, CString strFontPatch, SegmentReadingOption stReadingOption )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	USES_CONVERSION;

	// ----- Algorithm Start -----
	int nThreshold = 0;
	if( stReadingOption.bUseMaxThreshold )		nThreshold = EasyImage::AutoThreshold(&SegmentImage, EThresholdMode_MaxEntropy).Value;
	else										nThreshold = EasyImage::AutoThreshold(&SegmentImage, EThresholdMode_MinResidue).Value;

	if(nThreshold ==   0) nThreshold = 1;
	if(nThreshold == 255) nThreshold = 254;
	LabelData.nThreshold_Teaching = nThreshold;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	if( stReadingOption.bUseImageFiltering )
	{
		EasyImage::Threshold(&SegmentImage, &SegmentImage, nThreshold);
		EasyImage::DilateBox(&SegmentImage, &SegmentImage, 0, 2);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	if( stReadingOption.bUseFontsizeManualSetting )
	{
		LabelData.FontMinSize = stReadingOption.FontMinSize;
		LabelData.FontMaxSize = stReadingOption.FontMaxSize;
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		ECodedImage eCodedImage_Font;
		EListItem*  pObject_Font;
//		CString str = _T("D:\\test.bmp");
//		SegmentImage.Save(T2A((LPTSTR)(LPCTSTR)str));
		eCodedImage_Font.SetWhiteClass	( 0 );
		eCodedImage_Font.SetBlackClass	( 1 );	
		eCodedImage_Font.SetConnexity	( EConnexity_Connexity8 );
		eCodedImage_Font.SetThreshold	( nThreshold	 );		
		eCodedImage_Font.BuildObjects	( &SegmentImage	 );

		int nObjectCount = eCodedImage_Font.GetNumSelectedObjects();
		if( nObjectCount <= 0 ) return FALSE;

		eCodedImage_Font.AnalyseObjects ( ELegacyFeature_LimitWidth, ELegacyFeature_LimitHeight );


		////////////////////////////////////////////////////////////////////////////////////////////////////
		float fAverage_Width = 0.f, fAverage_Height = 0.f;
		std::vector<float> vfWidth;		vfWidth.clear();
		std::vector<float> vfHeight;	vfHeight.clear();

		pObject_Font = eCodedImage_Font.GetFirstObjPtr();

		for(int nObjectIndex = 0; nObjectIndex < nObjectCount; nObjectIndex++)
		{		
			float fWidth, fHeight;
			eCodedImage_Font.GetObjectFeature( ELegacyFeature_LimitWidth,		pObject_Font, fWidth	);
			eCodedImage_Font.GetObjectFeature( ELegacyFeature_LimitHeight,		pObject_Font, fHeight	);
			pObject_Font = eCodedImage_Font.GetNextObjPtr( pObject_Font );	

			if( fWidth < 5.f || fHeight < 5.f ) continue;

			vfWidth. push_back(fWidth);
			vfHeight.push_back(fHeight);

			fAverage_Width	+= fWidth;
			fAverage_Height += fHeight;
		}

		if(vfWidth.empty() || vfHeight.empty()) return FALSE;
		if( (int)vfWidth.size() != (int)vfHeight.size() ) return FALSE;


		////////////////////////////////////////////////////////////////////////////////////////////////////
		if( stReadingOption.bUseFontSizeFiltering )
		{
			fAverage_Width	/= (float)vfWidth.size();
			fAverage_Height	/= (float)vfHeight.size();

			float fMin_Width	= fAverage_Width	* 0.5f;
			float fMin_Height	= fAverage_Height	* 0.5f;
			float fMax_Width	= fAverage_Width	* 1.5f;
			float fMax_Height	= fAverage_Height	* 1.5f;

			for(int nIndex = 0; nIndex < (int)vfWidth.size(); nIndex++)
			{
				if( fAverage_Width * 0.5 > vfWidth.at(nIndex) || 
					fAverage_Width * 1.5 < vfWidth.at(nIndex)  )	vfWidth.erase( vfWidth.begin()+nIndex );
			}

			for(int nIndex = 0; nIndex < (int)vfHeight.size(); nIndex++)
			{
				if( fAverage_Height * 0.5 > vfHeight.at(nIndex) || 
					fAverage_Height * 1.5 < vfHeight.at(nIndex)  )	vfHeight.erase( vfHeight.begin()+nIndex );
			}

			if(vfWidth.empty() || vfHeight.empty()) return FALSE;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		LabelData.FontMinSize.cx  = (int)*min_element( vfWidth.begin(),  vfWidth.end()  );
		LabelData.FontMaxSize.cx  = (int)*max_element( vfWidth.begin(),  vfWidth.end()  );
		LabelData.FontMinSize.cy  = (int)*min_element( vfHeight.begin(), vfHeight.end() );
		LabelData.FontMaxSize.cy  = (int)*max_element( vfHeight.begin(), vfHeight.end() );
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ----- OCR Setting -----
	EOCR OcrFont;	
	OcrFont.Load(T2A(strFontPatch));

	OcrFont.SetThreshold		( nThreshold								);
	OcrFont.SetMinCharWidth		( LabelData.FontMinSize.cx					);
	OcrFont.SetMaxCharWidth		( LabelData.FontMaxSize.cx + 1				);
	OcrFont.SetMinCharHeight	( LabelData.FontMinSize.cy					);
	OcrFont.SetMaxCharHeight	( LabelData.FontMaxSize.cy + 1				);
	OcrFont.SetCharSpacing		( 1											);
	OcrFont.SetCutLargeChars	( stReadingOption.bUseeVisionLargeCharsCut	);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	//char szOCRRecoText[512];
	UINT unUsingRecoClasses = EOCRClass_Digit | EOCRClass_UpperCase | EOCRClass_Special | EOCRClass_Extended;

	std::string szOCRRecoText = OcrFont.Recognize(&SegmentImage, 512, unUsingRecoClasses);
	
	int nOCRCount = OcrFont.GetNumChars();
	if( nOCRCount <= 0 ) return FALSE;

	int nMinPosY = 0, nMaxPosY = 0;
	for(int nIndex = 0 ; nIndex < nOCRCount ; nIndex++)
	{
		float fScore = OcrFont.GetConfidenceRatio(nIndex);
		
		CRect reFontArea;
		reFontArea.left		= OcrFont.CharGetOrgX(nIndex);
		reFontArea.top		= OcrFont.CharGetOrgY(nIndex);
		reFontArea.right	= reFontArea.left + OcrFont.CharGetWidth (nIndex);
		reFontArea.bottom	= reFontArea.top  + OcrFont.CharGetHeight(nIndex);
		if(reFontArea.Width() < 5 || reFontArea.Height() < 5) continue;

		reFontArea.OffsetRect(LabelData.reLineArea.left, LabelData.reLineArea.top);

		CString strReading;
		strReading.Format(_T("%c"), szOCRRecoText.at(nIndex)); //szOCRRecoText[nIndex]);

		LabelData.strLineText += strReading;

		LabelData.reSegmentArea.		push_back (reFontArea);
		LabelData.strSegmentText.		push_back (strReading);
		LabelData.bSegmentScoreResult.	push_back (30.f<=fScore?TRUE:FALSE);
		LabelData.fSegmentScore.		push_back (fScore);

		if((int)LabelData.reSegmentArea.size() == 1)
		{
			nMinPosY = reFontArea.top;
			nMaxPosY = reFontArea.bottom;
		}
		else
		{
			if( nMinPosY > reFontArea.top	 ) nMinPosY = reFontArea.top;
			if( nMaxPosY < reFontArea.bottom ) nMaxPosY = reFontArea.bottom;
		}
	}


	nOCRCount = (int)LabelData.strSegmentText.size();
	if( nOCRCount <= 0 ) return FALSE;

	LabelData.reLineArea = CRect(LabelData.reSegmentArea.at(0).left, nMinPosY, LabelData.reSegmentArea.at(nOCRCount-1).right, nMaxPosY);


	// ----- Reading Data Check -----
	if( MasterData.empty() ) LabelData.bInspoResult = TRUE; // --- Manual Inspection ---
	else // ----- Auto Run Inspection -----
	{		
		for(int nCheak = 0; nCheak < (int)MasterData.size(); nCheak++)
		{
			if( MasterData.at(nCheak).bInspCheck ) continue;

			CString	strMaster = MasterData.at(nCheak).strString;	
			if( CheckString( LabelData, strMaster ) )
			{			
				MasterData.at(nCheak).bInspCheck = TRUE;
				break;
			}
		}
	}

	BOOL bReadingResult = LabelData.bInspoResult;		
	if( bReadingResult || (stReadingOption.nImageType == LABLE_INSPECTION_MAX_IMAGE_TYPE-1 && stReadingOption.nOptionIndex == LABLE_INSPECTION_MAX_OPTUION-1) )
		m_LabelOCRData.push_back(LabelData);

	if( stReadingOption.bTeachingMode )
	{
		EOCR *OcrFontBuff = &OcrFont;	

		EROIBW8 eROI;
		eROI.Attach(&SegmentImage);
		eROI.SetPlacement(0, 0, SegmentImage.GetWidth(), SegmentImage.GetHeight());

		if( !EVisionOCRAddFont(eROI, OcrFontBuff, strFontPatch) ) return FALSE;
	}

	return bReadingResult;
}

BOOL CInspectionVision::EVisionOCRAddFont(EROIBW8 SegmentImage, EOCR *OCRFont, CString strFontPatch, BOOL bUseAdaptiveThreshold /*= FALSE*/)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	USES_CONVERSION;

	// ----- Time Data -----
	CTime	cTimeData = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format(_T("%02d%02d%02d%02d%02d%02d"), cTimeData.GetYear(), cTimeData.GetMonth(), cTimeData.GetDay(), cTimeData.GetHour(), cTimeData.GetMinute(), cTimeData.GetSecond());

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UINT unUsingRecoClasses = EOCRClass_Digit | EOCRClass_UpperCase | EOCRClass_Special | EOCRClass_Extended;
	std::string szOCRRecoText = OCRFont->Recognize(&SegmentImage, 512, unUsingRecoClasses);

	///// Data Check /////
	CString szOCRRecoCString(szOCRRecoText.c_str()); // string -> CString
	int nReadingCount = (int)m_LabelReadingData.vreReadingArea.size();
	if (m_LabelReadingData.strReadingResult != szOCRRecoCString)
	{
		CString strError = _T("");
		strError.Format(_T("Ocr Reading Data Error !! \r\n - Viewer[%s] // Size[%d]\r\n - Add[%s] // Size[%d]"),
			m_LabelReadingData.strReadingResult, m_LabelReadingData.strReadingResult.GetLength(),
			szOCRRecoCString, szOCRRecoCString.GetLength() );

		AfxMessageBox(strError, MB_ICONINFORMATION);
		return FALSE;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EROIBW8 eEBW8Image_ROI;
	eEBW8Image_ROI.Attach(&SegmentImage);

	CString strSegmentSavePatch;

	// 1G에 Chip OCR 추가되어 조건부 컴파일로 폰트 이미지 저장경로 설정이 아닌 if-else로 수정
	if (strFontPatch.Find(_T("Chip"), 0) != -1)
		strSegmentSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Chip Font Segment");
	else if(strFontPatch.Find(_T("Tray"), 0) != -1)
		strSegmentSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Tray Font Segment");
	else
		strSegmentSavePatch = CModelInfo::Instance()->GetRecipeRootPath() + _T("_Common_Recipe\\Label Font Segment");

	if (!IsExistFile((LPCTSTR)strSegmentSavePatch)) MakeDirectory( (LPCTSTR)strSegmentSavePatch );

	// ----- OCR Font & Image Save -----	
	for(int nFontNum = 0; nFontNum < nReadingCount; nFontNum++)
	{
		if( !m_LabelReadingData.vbTeaching.at(nFontNum) ) continue;

		CString strCharCode		= m_LabelReadingData.vstrTeachingText.at(nFontNum);
		wchar_t OCRCharCode		= m_LabelReadingData.vstrTeachingText.at(nFontNum)[0];
		CRect	reSegmentROI	= m_LabelReadingData.vreReadingArea.at(nFontNum);	
		CPoint	cpSegmentPT		= CPoint(reSegmentROI.CenterPoint().x, reSegmentROI.CenterPoint().y);

		// ----- Add Font -----
		UINT32 nSelectIndex; 
		OCRFont->HitChars((INT32)cpSegmentPT.x, (INT32)cpSegmentPT.y, nSelectIndex, 1.00f, 1.00f, 0.00f, 0.00f);
		OCRFont->LearnPattern(&SegmentImage, nSelectIndex, OCRCharCode, EOCRClass_Digit);

		// ----- Segment Image Save -----
		reSegmentROI.InflateRect(3, 3, 3, 3);
#ifdef RELEASE_1G
		// Chip OCR, Tray OCR 공통, 250120 황대은J
		eEBW8Image_ROI.SetPlacement(reSegmentROI.left - SegmentImage.GetOrgX(), reSegmentROI.top - SegmentImage.GetOrgY(), reSegmentROI.Width(), reSegmentROI.Height());
#elif RELEASE_SG
		eEBW8Image_ROI.SetPlacement(reSegmentROI.left - SegmentImage.GetOrgX(), reSegmentROI.top - SegmentImage.GetOrgY(), reSegmentROI.Width(), reSegmentROI.Height());
#else
		eEBW8Image_ROI.SetPlacement(reSegmentROI.left, reSegmentROI.top, reSegmentROI.Width(), reSegmentROI.Height());
#endif
		int nASCIICode = strCharCode[0];

		// 240404 HJ : Font 폴더 이름 ACSII Code 값만 저장하면 알아보기 어렵다고 변경 요청
		CString strTemp;
		strTemp.Format(_T("%d_"), nASCIICode);

		if ( (nASCIICode > 47 && nASCIICode < 58)		// 0 1 2 3 4 5 6 7 8 9
			|| (nASCIICode > 64 && nASCIICode < 91)		// A B C D ... W X Y Z
			|| (nASCIICode > 96 && nASCIICode < 123) )	// a b c d ... w x y z
		{
			strTemp.Format(_T("%d_%c"), nASCIICode, strCharCode[0]);
		}

		strTemp = strSegmentSavePatch + _T("\\") + strTemp;
		if (!IsExistFile((LPCTSTR)strTemp)) MakeDirectory( (LPCTSTR)strTemp );

		CString strSubData;

		// Segment Char 이미지 저장시 Chip OCR 폰트 색이 Black, White 둘 다 있어서 구별 하기위해 추가, 250120 황대은J
		if (strFontPatch.Find(_T("Chip"), 0) != -1)
		{
			// Font Color Black이면 이미지 이름 끝에 'B' 추가, White이면 'W' 추가
			int nFontColor = CModelInfo::Instance()->GetChipOcr().nFontColor;
			
			if (nFontColor == OBJ_BLACK)
			{
				strSubData.Format(_T("_%d_%03d%03d%03d%03d%c"),
								nFontNum, OCRFont->GetMinCharWidth(), OCRFont->GetMaxCharWidth(), OCRFont->GetMinCharHeight(), OCRFont->GetMaxCharHeight(), 'B');
			}
			else
			{
				strSubData.Format(_T("_%d_%03d%03d%03d%03d%c"),
								nFontNum, OCRFont->GetMinCharWidth(), OCRFont->GetMaxCharWidth(), OCRFont->GetMinCharHeight(), OCRFont->GetMaxCharHeight(), 'W');
			}
		}
		//strSubData.Format(_T("_%d"), nFontNum );
		else
		{
			strSubData.Format(_T("_%d_%03d%03d%03d%03d"),
				nFontNum, OCRFont->GetMinCharWidth(), OCRFont->GetMaxCharWidth(), OCRFont->GetMinCharHeight(), OCRFont->GetMaxCharHeight());
		}

		strTemp += _T("\\") + strTime + strSubData + _T(".bmp");
		eEBW8Image_ROI.Save(T2A(strTemp));
	}

	OCRFont->Save(T2A(strFontPatch));
	return TRUE;
}

BOOL CInspectionVision::CheckString( LabelOCRData &stTargetData, CString strMaster )
{
	BOOL bResult = FALSE;

	CString strMaster_Temp = strMaster;
	CString strTarget_Temp = stTargetData.strLineText;
	std::vector<int> DeletePos; DeletePos.clear();

	try
	{	
		if(stTargetData.strLineText == strMaster) throw TRUE;

		// ----- Remove Space Characters -----
		strMaster_Temp.Replace( _T(" "), NULL );
		strTarget_Temp.Replace( _T(" "), NULL );

		if(strMaster_Temp == strTarget_Temp) throw TRUE;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// ----- Change Capital Letters ----- CString strOrgMasterData = strMaster_Temp;	
		strMaster_Temp.MakeUpper();
		strTarget_Temp.MakeUpper();

		if(strMaster_Temp == strTarget_Temp) throw TRUE;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if( strMaster_Temp.GetLength() == strTarget_Temp.GetLength() )
		{
			// ----- Change Similar Characters -----
			CheckSimilarFont(strMaster_Temp, strTarget_Temp);
			if(strMaster_Temp == strTarget_Temp) throw TRUE;
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			// ----- Remove Special Characters -----	
			ClearSpecialFont(strMaster_Temp, strMaster_Temp, DeletePos);
			ClearSpecialFont(strTarget_Temp, strTarget_Temp, DeletePos);

			if(strMaster_Temp == strTarget_Temp) throw TRUE;

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if( strMaster_Temp.GetLength() == strTarget_Temp.GetLength() )
			{
				// ----- Change Similar Characters -----
				CheckSimilarFont(strMaster_Temp, strTarget_Temp);
			}
			else
			{
				// ----- Large Characters -----
				LargeCharacterFont( stTargetData, strMaster_Temp, strTarget_Temp );
			}

			if(strMaster_Temp == strTarget_Temp) throw TRUE;		

		}
	}
	catch (BOOL bReadingResult)
	{		
		bResult = stTargetData.bInspoResult = bReadingResult;

		CString strMasterReplace = strMaster;
		strMasterReplace.Replace( _T(" "), NULL );
		stTargetData.strLineText = strMasterReplace;

		if( !DeletePos.empty() && strTarget_Temp.GetLength() != (int)stTargetData.strSegmentText.size() )
		{		

		}
		else
		{
			for(int nIndex = 0; nIndex < (int)stTargetData.strSegmentText.size(); nIndex++)
			{
				char chBuff = (char)strMasterReplace.GetAt(nIndex);
				stTargetData.strSegmentText.at(nIndex).SetAt(0, chBuff);
			}
		}
	}

	return bResult;	
}

BOOL CInspectionVision::CheckSimilarFont( CString strMaster, CString &strTarget )
{
	CModelInfo::stLabelInfo& stLabelInfo = CModelInfo::Instance()->GetLabelInfo();

	BOOL bResult;
	try
	{
		if( strMaster.GetLength() != strTarget.GetLength() ) throw FALSE;


		for(int nIndex = 0; nIndex < strMaster.GetLength(); nIndex++)
		{
			char chMasterBuff, chTargetBuff;
			chMasterBuff = (char)strMaster.GetAt(nIndex);
			chTargetBuff = (char)strTarget.GetAt(nIndex);

			if( chMasterBuff == chTargetBuff ) continue;

			for(int nSimilarFontNum = 0; nSimilarFontNum < (int)stLabelInfo.vstrSimilarFont[0].size(); nSimilarFontNum++)
			{
				if( stLabelInfo.vstrSimilarFont[0].at(nSimilarFontNum) == _T("None_Target_0") ) break;
				if( stLabelInfo.vstrSimilarFont[1].at(nSimilarFontNum) == _T("None_Target_1") ) break;

				if( chMasterBuff == stLabelInfo.vstrSimilarFont[0].at(nSimilarFontNum) || chMasterBuff == stLabelInfo.vstrSimilarFont[1].at(nSimilarFontNum) )
				{
					if( chTargetBuff == stLabelInfo.vstrSimilarFont[0].at(nSimilarFontNum) || chTargetBuff == stLabelInfo.vstrSimilarFont[1].at(nSimilarFontNum) ) 
					{	
						strTarget.SetAt(nIndex, chMasterBuff);
						break;
					}
				}
			}
		}


		if(strMaster == strTarget)	bResult = TRUE;
		else						bResult = FALSE;
	}
	catch(BOOL Error)
	{
		if(!Error) bResult = FALSE;
	}	

	return bResult;
}

BOOL CInspectionVision::ClearSpecialFont( CString strSrc, CString &strDst, std::vector<int> &DeletePos )
{
	std::vector<CString> strSpecial;
	strSpecial.clear();

	strSpecial.push_back(_T("'")); // 0
	strSpecial.push_back(_T("*")); // 1
	strSpecial.push_back(_T(".")); // 2
	//strSpecial.push_back(_T(":")); // 3
	strSpecial.push_back(_T("(")); // 4
	strSpecial.push_back(_T(")")); // 5
	strSpecial.push_back(_T("-")); // 6

	DeletePos.clear();
	for(int SpecialCnt = 0; SpecialCnt < (int)strSpecial.size(); SpecialCnt++)
	{
		int nCheckPos;
		while (TRUE)
		{
			nCheckPos = strSrc.Find(strSpecial.at(SpecialCnt));

			if(nCheckPos == -1) break;
			else 
			{
				strSrc.Delete(nCheckPos, 1);
				DeletePos.push_back(nCheckPos);
			}
		}
	}

	strDst = strSrc;
	strSpecial.clear();

	return TRUE;
}

BOOL CInspectionVision::LargeCharacterFont( LabelOCRData &stTargetData, CString strSrc, CString &strDst )
{
	CString strLarge[2];
	for( int nIndex = 0; nIndex < 3; nIndex++)
	{
		CString strDstBuff = strDst;

		int nSizeChange = 0;
		switch (nIndex)
		{
		case 0:
			strLarge[0] = (_T("VV"));
			strLarge[1] = (_T("W"));
			nSizeChange = -1;
			break;

		case 1:
			strLarge[0] = (_T("VA"));
			strLarge[1] = (_T("WI"));
			nSizeChange = 0;
			break;

		case 2:
			strLarge[0] = (_T("W"));
			strLarge[1] = (_T("WI"));
			nSizeChange = 1;
			break;

		default: return FALSE;
		}


		int nFindPos = strDstBuff.Find( strLarge[0] );
		if( nFindPos != -1 )
		{
			strDstBuff.Delete( nFindPos, strLarge[0].GetLength() );
			strDstBuff.Insert( nFindPos, strLarge[1] );

			if( strSrc == strDstBuff ) 
			{
				strDst = strDstBuff;

				////////////////////////////////////////////////////////////////////////////////////////////////////
				if( nSizeChange == -1 )
				{
					if( !(nFindPos + 1 < (int)stTargetData.strSegmentText.size() && nFindPos + 1 < (int)stTargetData.reSegmentArea. size() &&
						nFindPos + 1 < (int)stTargetData.bSegmentScoreResult.size() && nFindPos + 1 < (int)stTargetData.fSegmentScore. size()) ) break;

					stTargetData.strLineText = strDst;
					stTargetData.reSegmentArea.at(nFindPos).right += stTargetData.reSegmentArea.at(nFindPos+1).Width();

					stTargetData.strSegmentText.		erase(stTargetData.strSegmentText.		begin()+nFindPos+1);
					stTargetData.reSegmentArea.			erase(stTargetData.reSegmentArea.		begin()+nFindPos+1);
					stTargetData.bSegmentScoreResult.	erase(stTargetData.bSegmentScoreResult.	begin()+nFindPos+1);
					stTargetData.fSegmentScore.			erase(stTargetData.fSegmentScore.		begin()+nFindPos+1);

					stTargetData.strSegmentText.at(nFindPos+1) = strLarge[1];
				}
				else if(nSizeChange == 1)
				{

				}
				////////////////////////////////////////////////////////////////////////////////////////////////////

				break;
			}
		}	
	}

	return TRUE;
}

BOOL CInspectionVision::ReadCode( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, std::vector<READ_DATA_Label> LabelData, BOOL& bReadingRet, BOOL& bCheckLotID, InspectType inspecttype  )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	COLORBOX  clrBox;
	COLORTEXT clrText;

	CString strException = _T("");

	// Barcode 수량 파악
	int nBarcodeDataCnt = 0;
	int nQRDataCnt = 0;
	for( int nIndex = 0; nIndex < (int)LabelData.size(); nIndex++ )
	{
		if( LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_SEGMENT		||
			LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_TEXT_SEGMENT	)
		{
			CString strQA = _T("QA");

			if ( LabelData.at(nIndex).strString.Find(strQA) > 1 || LabelData.at(nIndex).strString.Find(strQA) == -1 )
				nBarcodeDataCnt++;
			else
				nQRDataCnt++;
		}
	}

	int nBarcodeCnt = 0;
	int nQRcodeCnt  = 0;

	try
	{
		m_LabelCodeData.clear();

#ifndef _CodeUsb
		CDecoderCortex* pDecoder = CDecoderCortex::GetPtr();
		pDecoder->DecodeImage(pMaskingImgObj);

		if (pDecoder->m_nNumDecoded <= 0)
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("바코드 찾기 에러");
			else												strException = _T("BarCode Search Error");
			
			throw strException;
		}
		for (int i = 0; i < pDecoder->m_nNumDecoded; ++i)
		{
			if (i == 20)
				break;
			pDecoder->GetResultData(i);

			CString strData = _T("");
			CString strType = _T("");

			strData = pDecoder->m_strData[i];
			strType = pDecoder->m_strType[i];

			CRect rcRect = pDecoder->m_rcResult[i];

			if (rcRect.Width() < 15 || rcRect.Height() < 15)
				continue;

			if (strType == _T("Code-39") || strType == _T("Code-128") || strType == _T("PDF417") || strType == _T("DataMatrix") )
			{
				nBarcodeCnt++;
			}
			else if (strType == _T("QR-Code") || strType == _T("QR-Micro") || strType == _T("QR-Model1") || strType == _T("QR-2005"))
			{
				nBarcodeCnt++;
				nQRcodeCnt++;
			}

			int nMargin = -5;
			rcRect.DeflateRect(nMargin, nMargin, nMargin, nMargin);
			rcRect.NormalizeRect();

			if (strType == _T("CODE-39") || strType == _T("CODE-128") || strType == _T("PDF417") )
				GetBarcodeArea(pMaskingImgObj, pGO, rcRect);

			MaskingArea(pGO, pMaskingImgObj, rcRect);

			VisionProcess::LabelCodeData DataBuff;
			DataBuff.clear();

			DataBuff.reCodeArea = rcRect;
			DataBuff.strCodeText = strData;
			m_LabelCodeData.push_back(DataBuff);
		}
#else
		if (CLanguageInfo::Instance()->m_nLangType == 0)	strException = _T("바코드 검사 동글 없음");
		else												strException = _T("No Code Dongle");

		throw strException;
#endif
	}
	catch (CString strError)
	{
		clrText.SetText		( strError ); // _T("BarCode Search Error")
		clrText.CreateObject( PDC_LIGHTRED, 700, 120, 35, TRUE );	
		pGO->AddDrawText	( clrText );
		return FALSE;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bReadingRet = bCheckLotID = FALSE;

	VisionProcess::LabelCodeData DataBuff; DataBuff.clear();

	DataBuff.nBarcodeCnt = nBarcodeCnt;
	if( nBarcodeCnt >= 0 )		// --- 1D Code Check ---
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText( _T("바코드 찾기 : %d"), nBarcodeCnt );
		else												clrText.SetText( _T("BarCode Search : %d"), nBarcodeCnt );
		clrText.CreateObject( PDC_BLUE, 700, 120, 35, TRUE );	
		pGO->AddDrawText	( clrText );
	}

	DataBuff.nQRcodeCnt  = nQRDataCnt; // nQRcodeCnt;
	if( nQRDataCnt /*nQRcodeCnt */>= 0 )		// --- 2D Code Check ---
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText( _T("QR코드 찾기 : %d"), nQRDataCnt /*nQRcodeCnt */);
		else												clrText.SetText( _T("QRCode Search : %d"), nQRDataCnt /*nQRcodeCnt */);
		clrText.CreateObject( PDC_BLUE, 700, 180, 35, TRUE );
		pGO->AddDrawText	( clrText );
	}

	CheckCode( pGO, pMaskingImgObj, LabelData, DataBuff, inspecttype );
	bReadingRet	 = DataBuff.bReadingRet;
 	bCheckLotID = DataBuff.bCheckLotID; 

	return bReadingRet && bCheckLotID;
}

void CInspectionVision::GetBarcodeArea( CxImageObject& pMaskingImgObj, CxGraphicObject* pGO, CRect& rcBarcode )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return;

	CxDPoint dptRectCenter;
	dptRectCenter.x =  rcBarcode.left + (rcBarcode.Width()/2);
	dptRectCenter.y =  rcBarcode.top + (rcBarcode.Height()/2);

	CxGOCross  cxCross;
	cxCross.CreateObject( PDC_PURPLE , dptRectCenter, PS_SOLID, 2 );
	pGO->AddDrawCross(cxCross);

	BOOL bSearch = TRUE;	// 바코드 영역의 끝이라고 판단하면 Pixel 탐색 그만 둠
	int nWhitePixelCnt = 0; // 흰색 영역이 연속되는 횟수 // 일정 횟수 넘어가면 바코드 영역이 아님을 뜻 함.		
	int nWhitePixelCntWidthLimit = 40;
	int nWhileCnt = 0; // 반복문 카운트 ( 각 픽셀 값 얻을 때 좌표값의 가중치)

	CRect rcSearchLimit = m_rcLabelArea;

	CRect rcBarCodeArea; // 계산하여 얻은 바코드 영역을 저장할 Rect 공간
	int nBarcodeThreshold = 230; // 바코드의 검은 영역이라고 판단하는 값

	EImageBW8 pImageX;
	pImageX.SetImagePtr( pMaskingImgObj.GetWidth(), pMaskingImgObj.GetHeight(), pMaskingImgObj.GetImageBuffer(), pMaskingImgObj.GetWidthBytes()*8 );

	// Left 좌표 계산
	while (bSearch)
	{
		if ( nBarcodeThreshold < pImageX.GetPixel( GetRoundShort(dptRectCenter.x - nWhileCnt), GetRoundShort(dptRectCenter.y) ).Value )
			nWhitePixelCnt++;
		else
			nWhitePixelCnt = 0;

		if (dptRectCenter.x - nWhileCnt <= rcSearchLimit.left || nWhitePixelCnt > nWhitePixelCntWidthLimit )
		{
			if(nWhitePixelCnt > nWhitePixelCntWidthLimit)
				rcBarCodeArea.left = GetRoundLong(dptRectCenter.x - nWhileCnt + nWhitePixelCntWidthLimit);
			else
				rcBarCodeArea.left = GetRoundLong(dptRectCenter.x - nWhileCnt + nWhitePixelCnt);

			bSearch = FALSE;
			nWhitePixelCnt = 0; 	
			nWhileCnt = 0;
		}
		else
			nWhileCnt++;
	}

	// Right 좌표 계산
	bSearch = TRUE;
	while (bSearch)
	{
		if ( nBarcodeThreshold < pImageX.GetPixel( GetRoundShort(dptRectCenter.x + nWhileCnt), GetRoundShort(dptRectCenter.y)).Value )
			nWhitePixelCnt++;
		else
			nWhitePixelCnt = 0;

		if (dptRectCenter.x + nWhileCnt >= rcSearchLimit.right || nWhitePixelCnt > nWhitePixelCntWidthLimit )
		{
			if(nWhitePixelCnt > nWhitePixelCntWidthLimit)
				rcBarCodeArea.right = GetRoundLong(dptRectCenter.x + nWhileCnt - nWhitePixelCntWidthLimit);
			else
				rcBarCodeArea.right = GetRoundLong(dptRectCenter.x + nWhileCnt - nWhitePixelCnt);

			bSearch = FALSE;
			nWhitePixelCnt = 0; 	
			nWhileCnt = 0;
		}
		else
			nWhileCnt++;
	}

	BlobBarcodeLine( &pImageX, rcBarCodeArea, dptRectCenter);
	nBarcodeThreshold = 200;

	// Top 좌표 계산
	bSearch = TRUE;
	while (bSearch)
	{
		if ( nBarcodeThreshold < pImageX.GetPixel( GetRoundShort(dptRectCenter.x), GetRoundShort(dptRectCenter.y - nWhileCnt) ).Value )
			nWhitePixelCnt++;
		else
			nWhitePixelCnt = 0;

		if (dptRectCenter.y - nWhileCnt <= rcSearchLimit.top || nWhitePixelCnt > 1 )
		{
			if(nWhitePixelCnt > 1)
				rcBarCodeArea.top = GetRoundLong(dptRectCenter.y - nWhileCnt + 1);
			else
				rcBarCodeArea.top = GetRoundLong(dptRectCenter.y - nWhileCnt);

			bSearch = FALSE;
			nWhitePixelCnt = 0; 	
			nWhileCnt = 0;
		}
		else
			nWhileCnt++;
	}

	// Bottom 좌표 계산
	bSearch = TRUE;
	while (bSearch)
	{
		if ( nBarcodeThreshold < pImageX.GetPixel( GetRoundShort(dptRectCenter.x), GetRoundShort(dptRectCenter.y + nWhileCnt) ).Value )
			nWhitePixelCnt++;
		else
			nWhitePixelCnt = 0;

		if (dptRectCenter.y + nWhileCnt >= rcSearchLimit.bottom || nWhitePixelCnt > 1 )
		{
			if(nWhitePixelCnt > 1)
				rcBarCodeArea.bottom = GetRoundLong(dptRectCenter.y + nWhileCnt - 1);
			else
				rcBarCodeArea.bottom = GetRoundLong(dptRectCenter.y + nWhileCnt);

			bSearch = FALSE;
			nWhitePixelCnt = 0; 	
			nWhileCnt = 0;
		}
		else
			nWhileCnt++;
	}

	rcBarcode = rcBarCodeArea;
}

void CInspectionVision::BlobBarcodeLine(EImageBW8* pImageX, CRect rcBarcode, CxDPoint& dptBarcode)
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return;

	EROIBW8 eRoi;
	ECodedImage eCodedImage;
	EListItem* pItem = NULL;

	eCodedImage.SetConnexity(EConnexity_Connexity8);
	eCodedImage.SetThreshold(100);
	eCodedImage.SetWhiteClass(0);
	eCodedImage.SetBlackClass(1);

	CxGOBox	cxBox;
	int nBarcodeWidth = rcBarcode.Width();

	if(rcBarcode.Width() > 700) // 바코드랑 "(" ")" 괄호랑 겹쳐져 있는 곳 있음 이걸 방지 하기 위하여 범위 줄임
		nBarcodeWidth = rcBarcode.Width()/3;

	eRoi.Attach(pImageX);
	eRoi.SetPlacement( GetRoundShort(rcBarcode.left), GetRoundShort(dptBarcode.y), nBarcodeWidth, 1);

	eCodedImage.BuildObjects(&eRoi);

	int nObjNum = eCodedImage.GetNumObjects();

	if (nObjNum > 0)
	{
		eCodedImage.AnalyseObjects(ELegacyFeature_LimitCenterX, ELegacyFeature_LimitWidth);

		pItem = eCodedImage.GetFirstObjPtr();
		float fCX, fW = 0.0f;

		BOOL bSelected = FALSE;
		CRect rcArea;
		eCodedImage.IsObjectSelected( pItem, bSelected );

		if( bSelected )
		{
			eCodedImage.GetObjectFeature( ELegacyFeature_LimitCenterX, pItem, fCX );
			eCodedImage.GetObjectFeature( ELegacyFeature_LimitWidth, pItem, fW );

			dptBarcode.x =  rcBarcode.left+fCX;
		}
	}

	eCodedImage.RemoveAllRuns();
	eCodedImage.RemoveAllObjects();
	eRoi.Detach();
}

BOOL CInspectionVision::CheckCode( CxGraphicObject* pGO, CxImageObject& pImgObj, std::vector<READ_DATA_Label> LabelData, LabelCodeData& CodeData, InspectType inspecttype )
{
	if( CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE )
		return FALSE;

	COLORBOX  clrBox;
	COLORTEXT clrText;

	// ----- Check BarCode Data -----
	std::vector<READ_DATA_Label> MasterCode;
	MasterCode.clear();
	for( int nIndex = 0; nIndex < (int)LabelData.size(); nIndex++ )
	{
		if( LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_SEGMENT		||
			LabelData.at(nIndex).DataType == READ_DATA_Label::SegmentType::BARCODE_TEXT_SEGMENT	)
		{
			CString strBuff = LabelData.at(nIndex).strCode;
			if(strBuff.Find(_T("^B7")) == -1) //PDF417
				MasterCode.push_back(LabelData.at(nIndex));
		}
	}

	// ----- Manual Inspection Check -----
	if( CVisionSystem::Instance()->GetRunStatus() == RunStatusStop && LabelData.empty() ) 
	{
		for(int nIndex = 0; nIndex < (int)m_LabelCodeData.size(); nIndex++)
		{	
			CRect crRect = m_LabelCodeData.at(nIndex).reCodeArea;
			clrBox.CreateObject( PDC_SKYBLUE, crRect, PS_SOLID, 2 );
			pGO->AddDrawBox( clrBox );

			clrText.CreateObject( PDC_SKYBLUE, crRect.CenterPoint().x, crRect.CenterPoint().y, 38, TRUE, CxGOText::TextAlignmentCenter );
			clrText.SetText( m_LabelCodeData.at(nIndex).strCodeText );
			pGO->AddDrawText( clrText );
		}

		return TRUE;
	}

	// ----- Receive Data Check -----
	if( LabelData.empty() || MasterCode.empty() )
	{
		clrText.SetText		( _T("BarCode Data Empty : [Rcv:%d]"), (int)MasterCode.size() );
		clrText.CreateObject( PDC_LIGHTRED, 700, 240, 35, TRUE );		
		pGO->AddDrawText	( clrText );

		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ----- Reading Data Check -----
	CString stLotID = CVisionSystem::Instance()->GetLotID(inspecttype);
	stLotID.Replace( _T(" "), NULL );
	stLotID.MakeUpper();

	if( CodeData.nBarcodeCnt + CodeData.nQRcodeCnt != (int)MasterCode.size() ) CodeData.bReadingRet = FALSE;

	if( CVisionSystem::Instance()->GetRunStatus() == RunStatusStop )
	{
		stLotID = _T("Manual"); //_T("MNYC220QCAE2002");
		CodeData.bCheckLotID = TRUE; //FALSE; 
	}
//	else if( nLabelNum == 0 ) CodeData.bCheckLotID = FALSE; 

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int nIndex = 0; nIndex < (int)m_LabelCodeData.size(); nIndex++)
	{	
		BOOL bCheckMaster = FALSE;

		CString strReadingText, strMasterText;
		for(int nMasterIndex = 0; nMasterIndex < (int)MasterCode.size(); nMasterIndex++)
		{
			if( MasterCode.at(nMasterIndex).bInspCheck ) continue;

			strReadingText	= m_LabelCodeData.at(nIndex).strCodeText;
			strMasterText	= MasterCode.at(nMasterIndex).strString;

			strReadingText.Replace( _T(" "), NULL );
			strReadingText.MakeUpper();

			strMasterText.Replace( _T(" "), NULL );
			strMasterText.MakeUpper();

			if( strMasterText == strReadingText )
			{
				bCheckMaster = MasterCode.at(nMasterIndex).bInspCheck = TRUE;
				break;
			}
			else // Code Data Exception Handling
			{
				// QR Code Add Data Handling
				CString strAdditionalText = _T("QA"); 
				if( strMasterText.Find(strAdditionalText) != -1 && strMasterText.Find(strReadingText) != -1 )
				{
					bCheckMaster = MasterCode.at(nMasterIndex).bInspCheck = TRUE;
					break;
				}
			}

		}

		//////////
		if( !bCheckMaster ) CodeData.bReadingRet = FALSE;
		else if( bCheckMaster && !CodeData.bCheckLotID )
		{
			if( strReadingText == stLotID ) 
			{
				CodeData.bCheckLotID = TRUE;
				stLotID = CVisionSystem::Instance()->GetLotID(inspecttype);
			}
		}

		//////////
		CRect reRect = m_LabelCodeData.at(nIndex).reCodeArea;
		clrBox.CreateObject( bCheckMaster?PDC_BLUE:PDC_LIGHTRED, reRect, PS_SOLID, 2 );
		pGO->AddDrawBox( clrBox );

		clrText.SetText( m_LabelCodeData.at(nIndex).strCodeText );
		clrText.CreateObject( bCheckMaster?PDC_BLUE:PDC_LIGHTRED, reRect.CenterPoint().x, reRect.CenterPoint().y, 38, TRUE, CxGOText::TextAlignmentCenter );
		pGO->AddDrawText( clrText );
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (CodeData.bReadingRet && CodeData.bCheckLotID)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("바코드 정보 일치 : OK(%s)"), stLotID);
		else												clrText.SetText(_T("BarCode Data Match : OK(%s)"), stLotID);
	}
	else if (!CodeData.bCheckLotID)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("바코드 정보 불일치 : NG[%s]"), CVisionSystem::Instance()->GetLotID(inspecttype));
		else												clrText.SetText(_T("BarCode Data Mismatch : NG[%s]"), CVisionSystem::Instance()->GetLotID(inspecttype));
	}
	else
	{
		if (CodeData.nBarcodeCnt + CodeData.nQRcodeCnt == (int)MasterCode.size())
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("바코드 정보 불일치 : NG[%d / %d]"), CodeData.nBarcodeCnt + CodeData.nQRcodeCnt, (int)MasterCode.size());
			else												clrText.SetText(_T("BarCode Data Mismatch : NG[%d / %d]"), CodeData.nBarcodeCnt + CodeData.nQRcodeCnt, (int)MasterCode.size());
		}
		else
		{
			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("바코드 정보 불일치 : NG[%d / %d]"), CodeData.nBarcodeCnt + CodeData.nQRcodeCnt, (int)MasterCode.size());
			else												clrText.SetText(_T("BarCode Count Mismatch : NG[%d / %d]"), CodeData.nBarcodeCnt + CodeData.nQRcodeCnt, (int)MasterCode.size());
		}
	}

	clrText.CreateObject( CodeData.bReadingRet&&CodeData.bCheckLotID?PDC_BLUE:PDC_LIGHTRED, 700, 240, 35, TRUE );		
	pGO->AddDrawText	( clrText );

	MasterCode.clear();
	return CodeData.bReadingRet && CodeData.bCheckLotID;
}

BOOL CInspectionVision::ColorTipLuminanceCheck(CxGraphicObject* pGO, CxImageObject* pSrcImgObj)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stHICInfo& stHICInfo = CModelInfo::Instance()->GetHICInfo();

	COLORTEXT clrText;
	COLORBOX clrBox;

	BOOL bColorTipResult[3] = { TRUE, TRUE, TRUE };
	int nColorPara[3] = { stHICInfo.nColorRedLuminance, stHICInfo.nColorGreenLuminance, stHICInfo.nColorBlueLuminance };
	float fLStar[3] = { 0.f, 0.f, 0.f };

	CRect rcTipArea[3];
	rcTipArea[0] = CRect(230, 260, 280, 430); // Red
	rcTipArea[1] = CRect(230, 450, 280, 620); // Green
	rcTipArea[2] = CRect(230, 645, 280, 815); // Blue

	CxImageObject CopyImgObj;
	CopyImgObj.Clone(pSrcImgObj);

	try
	{
		EImageC24 EC24Image;
		EC24Image.SetImagePtr(CopyImgObj.GetWidth(), CopyImgObj.GetHeight(), CopyImgObj.GetImageBuffer(), CopyImgObj.GetWidthBytes() * 8);

		EImageBW8 EBW8Image;
		EBW8Image.SetSize(&EC24Image);

		EColorLookup EColorLookup; // EColorLookup instance
		EColorLookup.ConvertFromRgb(EColorSystem_Lab);
		EasyColor::GetComponent(&EC24Image, &EBW8Image, 0, &EColorLookup);

		EROIBW8 BW8ImageRoi[3];

		for (int i = 0; i < 3; i++)
		{
			clrBox.CreateObject(PDC_ORANGE, rcTipArea[i], PS_SOLID, 1);
			pGO->AddDrawBox(clrBox);

			BW8ImageRoi[i].Attach(&EBW8Image);
			BW8ImageRoi[i].SetPlacement(rcTipArea[i].left, rcTipArea[i].top, rcTipArea[i].Width(), rcTipArea[i].Height());

			float fAverage = 0.f;
			EasyImage::PixelAverage(&BW8ImageRoi[i], fAverage);

			fLStar[i] = fAverage * 100 / 255;

			if (fLStar[i] > nColorPara[i])
				bColorTipResult[i] = FALSE;

			// 판정
			CString strText;
			switch (i)
			{
			case 0:		strText.Format(_T("R Tip"));	break;
			case 1:		strText.Format(_T("G Tip"));	break;
			case 2:		strText.Format(_T("B Tip"));	break;
			default:
				break;
			}
			clrText.SetText(strText);
			clrText.CreateObject(bColorTipResult[i] ? PDC_GREEN : PDC_LIGHTRED, rcTipArea[i].left - 75, rcTipArea[i].CenterPoint().y - 10, 20, TRUE);
			pGO->AddDrawText(clrText);

			clrText.SetText(_T("L* : %.2f"), fLStar[i]);
			clrText.CreateObject(bColorTipResult[i] ? PDC_GREEN : PDC_LIGHTRED, rcTipArea[i].left - 75, rcTipArea[i].CenterPoint().y + 10, 20, TRUE);
			pGO->AddDrawText(clrText);
		}

		CopyImgObj.Destroy();
	}
	catch (EException& e)
	{
		CopyImgObj.Destroy();

		return DisplayEException(pGO, e);
	}

	return bColorTipResult[0] && bColorTipResult[1] && bColorTipResult[2];
}

BOOL CInspectionVision::DisColorCheck(CxGraphicObject* pGO, CxImageObject* pSrcImgObj)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	enum DisColor {Red = 0, Green, Blue};

	CModelInfo::stHICInfo& stHICInfo = CModelInfo::Instance()->GetHICInfo();
	cv::Mat frame = cv::Mat(static_cast<int>(pSrcImgObj->GetHeight()), static_cast<int>(pSrcImgObj->GetWidth()), CV_8UC3, pSrcImgObj->GetImageBuffer());

	int nDisColor = stHICInfo.nDisColor;

	COLORTEXT clrText;
	COLORELLIPSE clrEllipse;

	BOOL bCircleResult[nHIC_CIRCLE_MAX] = { TRUE, TRUE, TRUE };

	// 원 내부의 B, R 색상 평균 계산
	int nTotalBlue[nHIC_CIRCLE_MAX] = { 0,0,0 };
	int nTotalGreen[nHIC_CIRCLE_MAX] = { 0,0,0 };
	int nTotalRed[nHIC_CIRCLE_MAX] = { 0,0,0 };
	int nAvgBlue[nHIC_CIRCLE_MAX] = { 0,0,0 };
	int nAvgGreen[nHIC_CIRCLE_MAX] = { 0,0,0 };
	int nAvgRed[nHIC_CIRCLE_MAX] = { 0,0,0 };
	int nCount = 0;

	int nRadius[nHIC_CIRCLE_MAX] = { stHICInfo.nCircleInnerDiameter5 / 2, stHICInfo.nCircleInnerDiameter10 / 2, stHICInfo.nCircleInnerDiameter60 / 2 };
	CPoint cpCenter[nHIC_CIRCLE_MAX];
	DRECT drcInner;

	for (int i = 0; i < nHIC_CIRCLE_MAX; i++)
	{
		nCount = 0;
		cpCenter[i] = m_rcHicCenter[i].CenterPoint();

		for (int y = cpCenter[i].y - nRadius[i]; y <= cpCenter[i].y + nRadius[i]; y++)
		{
			for (int x = cpCenter[i].x - nRadius[i]; x <= cpCenter[i].x + nRadius[i]; x++)
			{
				// 원 내부에 해당하는 픽셀만 선택
				if (sqrt((x - cpCenter[i].x) * (x - cpCenter[i].x) + (y - cpCenter[i].y) * (y - cpCenter[i].y)) <= nRadius[i])
				{
					cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
					nTotalBlue[i] += pixel[0]; // B 값 (Blue)
					nTotalGreen[i] += pixel[1];
					nTotalRed[i] += pixel[2]; // R 값 (Red)
					nCount++;
				}
			}
		}

		// 평균 값
		if (nCount > 0)
		{
			nAvgBlue[i] = nTotalBlue[i] / nCount;
			nAvgGreen[i] = nTotalGreen[i] / nCount;
			nAvgRed[i] = nTotalRed[i] / nCount;
		}
	}

	// 판정
	for (int i = 0; i < nHIC_CIRCLE_MAX; i++)
	{
		CString strText;
		switch (nDisColor)
		{
		case Red:
			if (stHICInfo.nPixelAvgValue < nAvgRed[i])
				bCircleResult[i] = FALSE;

			// Color value
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("R 평균 : %d"), nAvgRed[i]);
			else												strText.Format(_T("R Avg : %d"), nAvgRed[i]);
			clrText.SetText(strText);
			clrText.CreateObject(bCircleResult[i] ? PDC_GREEN : PDC_LIGHTRED, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 3), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("G 평균 : %d"), nAvgGreen[i]);
			else												strText.Format(_T("G Avg : %d"), nAvgGreen[i]);
			clrText.SetText(strText);
			clrText.CreateObject(PDC_GREEN, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 5), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("B 평균 : %d"), nAvgBlue[i]);
			else												strText.Format(_T("B Avg : %d"), nAvgBlue[i]);
			clrText.SetText(strText);
			clrText.CreateObject(PDC_GREEN, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 7), 20, TRUE);
			pGO->AddDrawText(clrText);
			break;

		case Green:
			if (stHICInfo.nPixelAvgValue < nAvgGreen[i])
				bCircleResult[i] = FALSE;

			// Color value
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("R 평균 : %d"), nAvgRed[i]);
			else												strText.Format(_T("R Avg : %d"), nAvgRed[i]);
			clrText.SetText(strText);
			clrText.CreateObject(PDC_GREEN, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 3), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("G 평균 : %d"), nAvgGreen[i]);
			else												strText.Format(_T("G Avg : %d"), nAvgGreen[i]);
			clrText.SetText(strText);
			clrText.CreateObject(bCircleResult[i] ? PDC_GREEN : PDC_LIGHTRED, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 5), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("B 평균 : %d"), nAvgBlue[i]);
			else												strText.Format(_T("B Avg : %d"), nAvgBlue[i]);
			clrText.SetText(strText);
			clrText.CreateObject(PDC_GREEN, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 7), 20, TRUE);
			pGO->AddDrawText(clrText);
			break;

		case Blue:
			if (stHICInfo.nPixelAvgValue < nAvgBlue[i])
				bCircleResult[i] = FALSE;

			// Color value
			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("R 평균 : %d"), nAvgRed[i]);
			else												strText.Format(_T("R Avg : %d"), nAvgRed[i]);
			clrText.SetText(strText);
			clrText.CreateObject(PDC_GREEN, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 3), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("G 평균 : %d"), nAvgGreen[i]);
			else												strText.Format(_T("G Avg : %d"), nAvgGreen[i]);
			clrText.SetText(strText);
			clrText.CreateObject(PDC_GREEN, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 5), 20, TRUE);
			pGO->AddDrawText(clrText);

			if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("B 평균 : %d"), nAvgBlue[i]);
			else												strText.Format(_T("B Avg : %d"), nAvgBlue[i]);
			clrText.SetText(strText);
			clrText.CreateObject(bCircleResult[i] ? PDC_GREEN : PDC_LIGHTRED, m_rcHicCenter[i].left, m_rcHicCenter[i].bottom + 10 + (11 * 7), 20, TRUE);
			pGO->AddDrawText(clrText);
			break;

		}

		drcInner.left = m_rcHicCenter[i].CenterPoint().x - nRadius[i];
		drcInner.top = m_rcHicCenter[i].CenterPoint().y - nRadius[i];
		drcInner.right = m_rcHicCenter[i].CenterPoint().x + nRadius[i];
		drcInner.bottom = m_rcHicCenter[i].CenterPoint().y + nRadius[i];

		clrEllipse.CreateObject(bCircleResult[i] ? PDC_GREEN : PDC_LIGHTRED, drcInner, PS_DASH, 3);
		pGO->AddDrawEllipse(clrEllipse);
	}

	return bCircleResult[0] && bCircleResult[1] && bCircleResult[2];
}

BOOL CInspectionVision::FindMBB(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stMBBInfo& stMBBInfo = CModelInfo::Instance()->GetMBBInfo();
	
	COLORTEXT clrText;
	COLORBOX clrBox;
	COLORCROSS clrCross;
	COLORARROW clrArrow;

	CRect rcNormalRect;
	rcNormalRect = stMBBInfo.rcInspArea;
	clrBox.CreateObject(PDC_ORANGE, rcNormalRect, PS_SOLID, 1);
	pGO->AddDrawBox(clrBox);
	
	float fTolerance = stMBBInfo.fTolerance / 2;
	float fLength = stMBBInfo.fLength;

	float fAngle = 0.f;
	double dResultAngle[4];

	DPOINT dptLine = { 0.f, 0.f };
	DPOINT dptResult[4];
	BOOL bResult[4];
	BOOL bSizeResultX = FALSE;
	BOOL bSizeResultY = FALSE;

	// init
	for (int i = 0; i < 4; ++i)
	{
		bResult[i] = FALSE;
		dResultAngle[i] = 0.f;
		dptResult[i] = { 0.f, 0.f };
	}
	
	try
	{
		CxImageObject CopyImgObj;
		CopyImgObj.Clone(pSrcImgObj);

		EImageBW8 BW8OrgImage, BW8ImgGaussian, BW8ImgSobel;
		BW8OrgImage.SetImagePtr(CopyImgObj.GetWidth(), CopyImgObj.GetHeight(), CopyImgObj.GetImageBuffer(), CopyImgObj.GetWidthBytes() * 8);

		EROIBW8 BW8ROIImage;
		BW8ROIImage.Attach(&BW8OrgImage);
		BW8ROIImage.SetPlacement(rcNormalRect.left, rcNormalRect.top, rcNormalRect.Width(), rcNormalRect.Height());

		BW8ImgGaussian.SetSize(rcNormalRect.Width(), rcNormalRect.Height());
		BW8ImgSobel.SetSize(rcNormalRect.Width(), rcNormalRect.Height());

		// Image Processing
		EasyImage::ConvolGaussian(&BW8ROIImage, &BW8ImgGaussian, 1);
		EasyImage::ConvolSobel(&BW8ImgGaussian, &BW8ImgSobel);

		DRECT drcArea;
		for (int i = 0; i < 4; ++i)
		{
			// Position
			switch (i)
			{
			case 0:
				dptLine = { static_cast<double>(rcNormalRect.left + fLength), static_cast<double>(fTolerance) }; // 임시
				drcArea = { dptLine.x - fLength / 2, dptLine.y + fTolerance, dptLine.x + fLength / 2, dptLine.y - fTolerance };
				fAngle = 0.f;
				break;
			case 1:
				dptLine = { static_cast<double>(rcNormalRect.Width() - fTolerance), static_cast<double>((rcNormalRect.Height() / 2)) };
				drcArea = { dptLine.x - fTolerance, dptLine.y - fLength / 2, dptLine.x + fTolerance, dptLine.y + fLength / 2 };
				fAngle = 90.f;
				break;
			case 2:
				dptLine = { static_cast<double>(rcNormalRect.left + fLength), static_cast<double>(rcNormalRect.Height() - fTolerance) }; // 임시
				drcArea = { dptLine.x - fLength / 2, dptLine.y + fTolerance, dptLine.x + fLength / 2, dptLine.y - fTolerance };
				fAngle = 180.f;
				break;
			case 3:
				dptLine = { static_cast<double>(fTolerance), static_cast<double>(rcNormalRect.Height() / 2) };
				drcArea = { dptLine.x - fTolerance, dptLine.y - fLength / 2, dptLine.x + fTolerance, dptLine.y + fLength / 2 };
				fAngle = 270.f;
				break;
			}

			// Find Line
			bResult[i] = LineGauge2(BW8ImgSobel, ETransitionType_Bw, ETransitionChoice_NthFromBegin, dptLine, fTolerance, fLength, fAngle,
				20, 1, 10, 0, 1, 1.5f, 1.f, FALSE, FALSE, TRUE, OUT dptResult[i], OUT dResultAngle[i]);

			// Area Offset
			drcArea.left += rcNormalRect.left;
			drcArea.right += rcNormalRect.left;
			drcArea.top += rcNormalRect.top;
			drcArea.bottom += rcNormalRect.top;
			clrBox.CreateObject(bResult[i] ? PDC_BLUE : PDC_RED, drcArea, PS_DASHDOT, 1);
			pGO->AddDrawBox(clrBox);

			if (!bResult[i])
				return FALSE;

			// Point Offset
			dptResult[i].x += rcNormalRect.left;
			dptResult[i].y += rcNormalRect.top;
			clrCross.CreateObject(PDC_BLUE, dptResult[i], 10, PS_SOLID, 3);
			pGO->AddDrawCross(clrCross);
		}

		// 길이 체크
		int nX, nY;
		double dX, dY;
		if (stMBBInfo.nSelectUnit == 0)
		{
			nX = GetRoundShort(dptResult[1].x - dptResult[3].x);
			nY = GetRoundShort(dptResult[2].y - dptResult[0].y);

			// 길이 판정
			if (nX > (stMBBInfo.nWidth_PX - stMBBInfo.nRange_PX) && nX < (stMBBInfo.nWidth_PX + stMBBInfo.nRange_PX))
				bSizeResultX = TRUE;
			if (nY > (stMBBInfo.nHeight_PX - stMBBInfo.nRange_PX) && nY < (stMBBInfo.nHeight_PX + stMBBInfo.nRange_PX))
				bSizeResultY = TRUE;
		}
		else
		{
			dX = (dptResult[1].x - dptResult[3].x) * m_fCalX[nViewIndex] / 1000; // 단위 mm로 변경
			dY = (dptResult[2].y - dptResult[0].y) * m_fCalY[nViewIndex] / 1000; // 단위 mm로 변경

			// 길이 판정
			if (dX > (stMBBInfo.nWidth - stMBBInfo.nRange) && dX < (stMBBInfo.nWidth + stMBBInfo.nRange))
				bSizeResultX = TRUE;
			if (dY > (stMBBInfo.nHeight - stMBBInfo.nRange) && dY < (stMBBInfo.nHeight + stMBBInfo.nRange))
				bSizeResultY = TRUE;
		}

		// 결과
		if (stMBBInfo.nSelectUnit == 0)			clrText.SetText(_T("MBB Width : %d px"), nX);
		else									clrText.SetText(_T("MBB Width : %.2f mm"), dX);
		clrText.CreateObject(bSizeResultX ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 8), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		if (stMBBInfo.nSelectUnit == 0)			clrText.SetText(_T("MBB Height : %d px"), nY);
		else									clrText.SetText(_T("MBB Height : %.2f mm"), dY);
		clrText.CreateObject(bSizeResultY ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 12), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		clrArrow.CreateObject(bSizeResultY ? PDC_BLUE : PDC_RED, dptResult[0], dptResult[2], CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		clrArrow.CreateObject(bSizeResultX ? PDC_BLUE : PDC_RED, dptResult[1], dptResult[3], CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
		pGO->AddDrawArrow(clrArrow);

		CopyImgObj.Destroy();
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return bSizeResultX && bSizeResultY;
}

BOOL CInspectionVision::CheckHistogramData(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stSealingInfo& stSealing = CModelInfo::Instance()->GetSealingInfo();

	COLORTEXT clrText;
	COLORBOX clrBox;

	CRect rcNormalRect, rcTop, rcBtm;
	rcNormalRect = stSealing.rcInspArea;
	clrBox.CreateObject(PDC_ORANGE, rcNormalRect, PS_SOLID, 1);
	pGO->AddDrawBox(clrBox);

	BOOL bResult = TRUE;

	try
	{
		CxImageObject CopyImgObj;
		CopyImgObj.Clone(pSrcImgObj);

		EImageBW8 BW8OrgImage, BW8ImgSobel;
		BW8OrgImage.SetImagePtr(CopyImgObj.GetWidth(), CopyImgObj.GetHeight(), CopyImgObj.GetImageBuffer(), CopyImgObj.GetWidthBytes() * 8);

		EROIBW8 BW8ROIImage;
		BW8ROIImage.Attach(&BW8OrgImage);
		BW8ROIImage.SetPlacement(rcNormalRect.left, rcNormalRect.top, rcNormalRect.Width(), rcNormalRect.Height());

		BW8ImgSobel.SetSize(rcNormalRect.Width(), rcNormalRect.Height());

		// Image Processing
		EasyImage::ConvolSobel(&BW8ROIImage, &BW8ImgSobel);

		EROIBW8 BW8ROI;
		BW8ROI.Attach(&BW8ImgSobel);
		BW8ROI.SetPlacement(0, 0, BW8ImgSobel.GetWidth(), BW8ImgSobel.GetHeight());

		EBWHistogramVector EBWHistogramVector;
		EasyImage::Histogram(&BW8ROI, &EBWHistogramVector);

		int nCount = EBWHistogramVector.GetElement(255);
		int nBase = stSealing.nInspSpec;

		float fPercent = (float)nCount / (float)nBase * 100.f;
		CString strUnit = _T("");
		
		if (stSealing.nSelectUnit == 0) // Tight
		{
			if (fPercent < stSealing.nCount)
				bResult = FALSE;

			strUnit = _T("Tight");

			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("표면 굴곡 검출량 : [Spec:%d][Result:%.2f％]"), stSealing.nCount, fPercent);
			else												clrText.SetText(_T("Surface Curvature Detection Value : [Spec:%d][Result:%.2f％]"), stSealing.nCount, fPercent);
		}
		else // Loose
		{
			if (fPercent > stSealing.nCount)
				bResult = FALSE;

			strUnit = _T("Loose");

			if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("표면 굴곡 검출량 : [Spec:%d][Result:%.2f％]"), stSealing.nCount, fPercent);
			else												clrText.SetText(_T("Surface Curvature Detection Value : [Spec:%d][Result:%.2f％]"), stSealing.nCount, fPercent);
		}
		clrText.CreateObject(bResult ? PDC_GREEN : PDC_LIGHTRED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI + (25 * 2), DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("실링 선택 조건 : %s"), strUnit);
		else												clrText.SetText(_T("Select Sealing Conditions : %s"), strUnit);
		clrText.CreateObject(PDC_ORANGE, GetRoundShort(BW8OrgImage.GetWidth()/2) - DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_SIZE);
		pGO->AddDrawText(clrText);

		CopyImgObj.Destroy();
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}

	return bResult;
}

BOOL CInspectionVision::OCRImagePreview(CxImageObject* pImgObj)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	if (pImgObj == NULL)
		return FALSE;

	CModelInfo::stTrayOcr& TrayOCR = CModelInfo::Instance()->GetTrayOcr();
	
	const int nOCRAreaDistX = TrayOCR.nOcrAreaDistX;
	const int nOCRAreaDistY = TrayOCR.nOcrAreaDistY;
	const CRect rcInspArea = TrayOCR.rcLastInspArea;

	CRect rcNonROI[nTRAY_MASKING_MAX], rcNonInsp[nTRAY_MASKING_MAX];
	int nMaskingAreaDistX[nTRAY_MASKING_MAX];
	int nMaskingAreaDistY[nTRAY_MASKING_MAX];
	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		rcNonROI[i] = TrayOCR.rcMaskingArea[i];
		nMaskingAreaDistX[i] = TrayOCR.nMaskingAreaDistX[i];
		nMaskingAreaDistY[i] = TrayOCR.nMaskingAreaDistY[i];
	}

	if (rcInspArea.right == 0 || rcInspArea.bottom == 0) // 검사 테스트 누르고 나서 해야 Image Preview 가능
	{
		AfxMessageBox(_T("Click \"Inspection Test\" button first."), MB_ICONWARNING);
		return FALSE;
	}

	CxImageObject pImgObj2;
	pImgObj2.Clone(pImgObj);

	EImageBW8 eImageX;
	eImageX.SetImagePtr(pImgObj2.GetWidth(), pImgObj2.GetHeight(), pImgObj2.GetImageBuffer(), pImgObj2.GetWidthBytes() * 8);

	EROIBW8 eImageROI;
	eImageROI.Attach(&eImageX);
	eImageROI.SetPlacement(rcInspArea.left, rcInspArea.top, rcInspArea.Width(), rcInspArea.Height());

	EImageBW8 eImageThreshold;
	eImageThreshold.SetSize(&eImageROI);
	EasyImage::AdaptiveThreshold(&eImageROI, &eImageThreshold, (EAdaptiveThresholdMethod)TrayOCR.nMethod, TrayOCR.nHalfKernelSize, TrayOCR.nThresholdOffset);

	EROIBW8 eImageMasking;
	eImageMasking.Attach(&eImageThreshold);
	eImageMasking.SetPlacement(0, 0, eImageThreshold.GetWidth(), eImageThreshold.GetHeight());

	// masking
	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		rcNonInsp[i].left = nMaskingAreaDistX[i];
		rcNonInsp[i].top = nMaskingAreaDistY[i];
		rcNonInsp[i].right = rcNonInsp[i].left + rcNonROI[i].Width();
		rcNonInsp[i].bottom = rcNonInsp[i].top + rcNonROI[i].Height();

		rcNonInsp[i].NormalizeRect();
		MaskingArea_Black(eImageMasking, rcNonInsp[i], 0);
	}

	USES_CONVERSION;
	CString strRecipeName = CModelInfo::Instance()->GetModelNameTrayOcr();
	CString strImageFile = _T("OCR Image Preview.bmp");
	CString strOriginImageFile = _T("OriginImage.bmp");
	CString strSavePath = CModelInfo::GetRecipeRootPath() + _T("1G_TrayOCR\\") + strRecipeName + _T("\\") + strImageFile;
	CString strOriginSavePath = CModelInfo::GetRecipeRootPath() + _T("\\1G_TrayOCR\\") + strRecipeName + _T("\\") + strOriginImageFile;

	std::string strPath = CT2A(strSavePath);
	eImageThreshold.Save(strPath);

	cv::Mat cvImage = cv::imread(strPath);
	cv::namedWindow("Adaptive Threshold OCR Preview", cv::WINDOW_NORMAL);
	cv::imshow("Adaptive Threshold OCR Preview", cvImage);
	cv::waitKey();

	pImgObj->SaveToFile(strOriginSavePath);

	pImgObj2.Destroy();
	return TRUE;
}

BOOL CInspectionVision::OCRImagePreview_stacker(CxImageObject* pImgObj)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	if (pImgObj == NULL)
		return FALSE;

	CModelInfo::stStackerOcr& StackerOCR = CModelInfo::Instance()->GetStackerOcr();

	const int nOCRAreaDistX = StackerOCR.nOcrAreaDistX;
	const int nOCRAreaDistY = StackerOCR.nOcrAreaDistY;
	const CRect rcInspArea = StackerOCR.rcLastInspArea;

	CRect rcNonROI[nTRAY_MASKING_MAX], rcNonInsp[nTRAY_MASKING_MAX];
	int nMaskingAreaDistX[nTRAY_MASKING_MAX];
	int nMaskingAreaDistY[nTRAY_MASKING_MAX];
	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		rcNonROI[i] = StackerOCR.rcMaskingArea[i];
		nMaskingAreaDistX[i] = StackerOCR.nMaskingAreaDistX[i];
		nMaskingAreaDistY[i] = StackerOCR.nMaskingAreaDistY[i];
	}

	if (rcInspArea.right == 0 || rcInspArea.bottom == 0) // 검사 테스트 누르고 나서 해야 Image Preview 가능
	{
		AfxMessageBox(_T("Click \"Inspection Test\" button first."), MB_ICONWARNING);
		return FALSE;
	}

	CxImageObject pImgObj2;
	pImgObj2.Clone(pImgObj);

	EImageBW8 eImageX;
	eImageX.SetImagePtr(pImgObj2.GetWidth(), pImgObj2.GetHeight(), pImgObj2.GetImageBuffer(), pImgObj2.GetWidthBytes() * 8);

	EROIBW8 eImageROI;
	eImageROI.Attach(&eImageX);
	eImageROI.SetPlacement(rcInspArea.left, rcInspArea.top, rcInspArea.Width(), rcInspArea.Height());

	EImageBW8 eImageThreshold;
	eImageThreshold.SetSize(&eImageROI);
	EasyImage::AdaptiveThreshold(&eImageROI, &eImageThreshold, (EAdaptiveThresholdMethod)StackerOCR.nMethod, StackerOCR.nHalfKernelSize, StackerOCR.nThresholdOffset);

	EROIBW8 eImageMasking;
	eImageMasking.Attach(&eImageThreshold);
	eImageMasking.SetPlacement(0, 0, eImageThreshold.GetWidth(), eImageThreshold.GetHeight());

	// masking
	for (int i = 0; i < nTRAY_MASKING_MAX; ++i)
	{
		rcNonInsp[i].left = nMaskingAreaDistX[i];
		rcNonInsp[i].top = nMaskingAreaDistY[i];
		rcNonInsp[i].right = rcNonInsp[i].left + rcNonROI[i].Width();
		rcNonInsp[i].bottom = rcNonInsp[i].top + rcNonROI[i].Height();

		rcNonInsp[i].NormalizeRect();
		MaskingArea_Black(eImageMasking, rcNonInsp[i], 0);
	}

	USES_CONVERSION;
	CString strRecipeName = CModelInfo::Instance()->GetLastModelNameStackerOcr();
	CString strImageFile = _T("OCR Image Preview.bmp");
	CString strOriginImageFile = _T("OriginImage.bmp");
	CString strSavePath = CModelInfo::GetRecipeRootPath() + _T("\\SG_StackerOcr\\") + strRecipeName + _T("\\") + strImageFile;
	CString strOriginSavePath = CModelInfo::GetRecipeRootPath() + _T("\\SG_StackerOcr\\") + strRecipeName + _T("\\") + strOriginImageFile;

	std::string strPath = CT2A(strSavePath);
	eImageThreshold.Save(strPath);

	cv::Mat cvImage = cv::imread(strPath);
	cv::namedWindow("Adaptive Threshold OCR Preview", cv::WINDOW_NORMAL);
	cv::imshow("Adaptive Threshold OCR Preview", cvImage);
	cv::waitKey();

	pImgObj->SaveToFile(strOriginSavePath);

	pImgObj2.Destroy();
	return TRUE;
}

// ========================================================↓ private functions (Only used in this class) ========================================================
UINT CInspectionVision::CalcNumTray(EROIBW8& eImageROI, CxGraphicObject* pGO, UINT nThreshold)
{
	COLORBOX clrBox;
	COLORDCROSS clrDCross;

	ECodedImage2 CodedImage;
	EImageEncoder CodedImageEncoder;
	EObjectSelection CodedImageObjectSelection;

	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(true);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(false);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_Absolute);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nThreshold);

	CodedImageEncoder.Encode(eImageROI, CodedImage);

	CodedImageObjectSelection.Clear();
	//CodedImageObjectSelection.AddObjectsUsingUnsignedIntegerFeature(CodedImage, EFeature_Area, 4000, ESingleThresholdMode_GreaterEqual);
	CodedImageObjectSelection.AddObjectsUsingFloatFeature(CodedImage, EFeature_BoundingBoxWidth, float(eImageROI.GetWidth() * 0.5f), float(eImageROI.GetWidth()+10), EDoubleThresholdMode_Inside);
	
	UINT nTrayCount = CodedImageObjectSelection.GetElementCount();

	if (!nTrayCount)	return nTrayCount;

	clrBox.CreateObject(PDC_BLUE, CRect(eImageROI.GetOrgX(), eImageROI.GetOrgY(), eImageROI.GetOrgX() + eImageROI.GetWidth() - 1, eImageROI.GetOrgY() + eImageROI.GetHeight() + 1), PS_SOLID, 2);
	pGO->AddDrawBox(clrBox);

	int nTop;
	int nBottom;
	int nCenterX, nCenterY;

	for (UINT i = 0; i < nTrayCount; ++i)
	{
		ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(i);
		nCenterX	= eImageROI.GetOrgX() + static_cast<int>(eCodedElement.GetBoundingBoxCenterX());
		nCenterY	= eImageROI.GetOrgY() + static_cast<int>(eCodedElement.GetBoundingBoxCenterY());
		nTop		= eImageROI.GetOrgY() + eCodedElement.GetTopLimit();
		nBottom		= eImageROI.GetOrgY() + eCodedElement.GetBottomLimit();

		clrDCross.CreateObject(PDC_BLUE, nCenterX, nCenterY, 10, PS_SOLID, 2);
		pGO->AddDrawDCross(clrDCross);
	}

	return nTrayCount;
}

UINT CInspectionVision::DetectBreakAndLIft(EROIBW8& eImageROI, CxGraphicObject* pGO, int nSelectUnit, const int nViewIndex, int nBreakWidth, int nBreakHeight, int nLiftHeight, int nNum)
{
	ECodedImage2 CodedImage;
	EImageEncoder CodedImageEncoder;
	EObjectSelection CodedImageObjectSelection;

	COLORBOX clrBox;
	COLORTEXT clrText;
	COLORARROW clrArrow;
	UINT nStatus = StatusOK;

	EImageBW8 eImageCopy;
	eImageCopy.SetSize(eImageROI.GetWidth(), eImageROI.GetHeight());
	EasyImage::Oper(EArithmeticLogicOperation_Copy, &eImageROI, &eImageCopy);

	float fAverage, fStdDev;
	EasyImage::PixelAverage(&eImageCopy, fAverage);
	EasyImage::PixelStdDev(&eImageCopy, fStdDev, fAverage);

	CModelInfo::stLiftInfo& stLiftInfo = CModelInfo::Instance()->GetLiftInfo();

	// 비검사 영역 설정 중앙 파인 곳 난반사 있어서.
	if (nViewIndex == 5)
	{
		if (nNum == 2 || nNum == 3)
		{
			for (int i = 0; i < eImageCopy.GetHeight(); ++i)
			{
				for (int j = stLiftInfo.nFrontNonInspStart; j < stLiftInfo.nFrontNonInspStart+stLiftInfo.nFrontNonInspWidth; ++j)
					eImageCopy.SetPixel((int)fAverage, j, i);


				CRect rcNonInsp;
				rcNonInsp.left		= eImageROI.GetOrgX() + stLiftInfo.nFrontNonInspStart;
				rcNonInsp.right		= rcNonInsp.left + stLiftInfo.nFrontNonInspWidth;
				rcNonInsp.top		= eImageROI.GetOrgY();
				rcNonInsp.bottom	= eImageROI.GetOrgY() + eImageROI.GetHeight();

				clrBox.CreateObject(PDC_LIGHTGREEN, rcNonInsp, PS_DOT, 3);
				pGO->AddDrawBox(clrBox);
			}
		}
	}

	float fThresholdOffset = fStdDev >= 14.0f ? 40.0f : 45.0f; // StdDev가 14.0이상이면 Lift나 Break가 있다고 판단
	UINT nThreshold = static_cast<UINT>(fAverage - fThresholdOffset);

	// 테스트용
	//CString strTest;
	//strTest.Format(_T("average: %.2f, stdDev: %.2f, nThreshold: %d"), fAverage, fStdDev, nThreshold);
	//clrText.SetText(strTest);
	//clrText.CreateObject(PDC_GREEN, eImageROI.GetOrgX(), eImageROI.GetOrgY(), 25, TRUE);
	//pGO->AddDrawText(clrText);

	CodedImageEncoder.SetSegmentationMethod(ESegmentationMethod_GrayscaleSingleThreshold);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(true);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(false);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold((UINT)nThreshold);
	CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Four);

	CodedImageEncoder.Encode(eImageCopy, CodedImage);
	CodedImageObjectSelection.Clear();
	CodedImageObjectSelection.AddObjectsUsingUnsignedIntegerFeature(CodedImage, EFeature_Area, 100, ESingleThresholdMode_GreaterEqual);

	int nObjCnt = CodedImageObjectSelection.GetElementCount();
	int nLeft, nTop;
	int nRight, nBottom;
	int nCenterX, nCenterY;
	int nObjWidth, nObjHeight;
	//float fEccentricity;

	if (nObjCnt)
	{
		for (int i = 0; i < nObjCnt; ++i)
		{
			ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(i);
			nObjWidth	= static_cast<int>(eCodedElement.GetBoundingBoxWidth());
			nObjHeight	= static_cast<int>(eCodedElement.GetBoundingBoxHeight());
			//fEccentricity = eCodedElement.GetEccentricity();

			if (nObjWidth < eImageROI.GetWidth() * 0.8 && nObjWidth >= nBreakWidth && nObjHeight >= nBreakHeight /*&& fEccentricity < 0.995f*/)
			{
				nStatus |= StatusBreak;

				nLeft = eImageROI.GetOrgX() + eCodedElement.GetLeftLimit();
				nTop = eImageROI.GetOrgY() + eCodedElement.GetTopLimit();
				nRight = eImageROI.GetOrgX() + eCodedElement.GetRightLimit();;
				nBottom = eImageROI.GetOrgY() + eCodedElement.GetBottomLimit();;

				CRect rcArea(nLeft, nTop, nRight, nBottom);

				clrBox.CreateObject(PDC_LIGHTRED, rcArea, PS_SOLID, 2);
				pGO->AddDrawBox(clrBox);

				CString strText;
				if (nSelectUnit == 0)
				{
					int nBreakWidth = GetRoundShort(eCodedElement.GetBoundingBoxWidth());
					int nBreakHeight = GetRoundShort(eCodedElement.GetBoundingBoxHeight());
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("깨짐 폭: %dpx, 높이: %dpx"), nBreakWidth, nBreakHeight);
					else												strText.Format(_T("Break Width: %dpx, Height: %dpx"), nBreakWidth, nBreakHeight);
				}
				else
				{
					double dbBreakWidth = (eCodedElement.GetBoundingBoxWidth() * m_fCalX[nViewIndex]) / 1000.0;
					double dbBreakHeight = (eCodedElement.GetBoundingBoxHeight() * m_fCalY[nViewIndex]) / 1000.0;

					if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("깨짐 폭: %.4lfmm, 높이: %.2lfmm"), dbBreakWidth, dbBreakHeight);
					else												strText.Format(_T("Break Width: %.4lfmm, Height: %.2lfmm"), dbBreakWidth, dbBreakHeight);
				}
				clrText.SetText(strText);
				clrText.CreateObject(PDC_LIGHTRED, nLeft, nTop - 30, 25, TRUE, CxGOText::TextAlignmentRight);
				pGO->AddDrawText(clrText);
			}

			else if (nObjWidth >= eImageROI.GetWidth() * 0.3 && nObjHeight > nLiftHeight)
			{
				nStatus |= StatusLift;

				nCenterX = eImageROI.GetOrgX() + static_cast<int>(eCodedElement.GetBoundingBoxCenterX());
				nCenterY = eImageROI.GetOrgY() + static_cast<int>(eCodedElement.GetBoundingBoxCenterY());

				nTop = eImageROI.GetOrgY() + eCodedElement.GetTopLimit();
				nBottom = eImageROI.GetOrgY() + eCodedElement.GetBottomLimit();

				clrArrow.CreateObject(PDC_RED, nCenterX, nTop, nCenterX, nBottom, CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeTriangle, 10, PS_SOLID);
				pGO->AddDrawArrow(clrArrow);

				CString strText;
				if (nSelectUnit == 0)
				{
					int nLiftHeight = GetRoundShort(eCodedElement.GetBoundingBoxHeight());
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("들뜸 높이: %dpx"), nLiftHeight);
					else												strText.Format(_T("Lift Height: %dpx"), nLiftHeight);
				}
				else
				{
					double dbLiftHeight_MM = (eCodedElement.GetBoundingBoxHeight() * m_fCalX[nViewIndex]) / 1000.0;
					if (CLanguageInfo::Instance()->m_nLangType == 0)	strText.Format(_T("들뜸 높이: %.4lfmm"), dbLiftHeight_MM);
					else												strText.Format(_T("Lift Height: %.4lfmm"), dbLiftHeight_MM);
				}
				clrText.SetText(strText);
				clrText.CreateObject(PDC_RED, nCenterX, nBottom + 30, 25, TRUE, CxGOText::TextAlignmentCenter);
				pGO->AddDrawText(clrText);
			}
		}
	}

	return nStatus;
}

CString CInspectionVision::ReadOCR_Tray(EROIBW8& eImageROI, CxGraphicObject* pGO, EOCR& eOCR, VisionProcess::TrayOCRData& stTrayData, CString strOcrInfo,
										UINT nMinWidth, UINT nMaxWidth, UINT nMinHeight, UINT nMaxHeight,
										BOOL bRemoveBorder, BOOL bRemoveNarrowOrFlat, BOOL bCutLargeChar, BOOL bIsFontWhite, UINT nCharSpacing, UINT nNoiseArea)
{
	COLORTEXT clrText;
	COLORBOX clrBox;
	CString strOcrResult = NULL;
	UINT nOcrClasses = EOCRClass_AllClasses;
	stTrayData.clear();
	
	USES_CONVERSION;

	eOCR.SetMinCharWidth(nMinWidth);
	eOCR.SetMaxCharWidth(nMaxWidth);
	eOCR.SetMinCharHeight(nMinHeight);
	eOCR.SetMaxCharHeight(nMaxHeight);
	eOCR.SetCharSpacing(nCharSpacing);
	eOCR.SetNoiseArea(nNoiseArea);
	eOCR.SetCutLargeChars(bCutLargeChar);
	eOCR.SetRemoveNarrowOrFlat(bRemoveNarrowOrFlat);
	eOCR.SetRemoveBorder(bRemoveBorder);
	eOCR.SetRelativeSpacing(0.05f);
	eOCR.SetSegmentationMode(ESegmentationMode_RepasteObjects);

	if (bIsFontWhite)
	{
		eOCR.SetTextColor(EOCRColor_WhiteOnBlack);
		eOCR.SetThreshold(EThresholdMode_MinResidue);
	}
	else
	{
		eOCR.SetTextColor(EOCRColor_BlackOnWhite);
		eOCR.SetThreshold(EThresholdMode_MinResidue);
	}

	eOCR.SetShiftXTolerance(0);
	eOCR.SetShiftYTolerance(0);

	strOcrResult = (CString)eOCR.Recognize(&eImageROI, 256, nOcrClasses).c_str();
	strOcrResult.Replace(_T(" "), NULL);
	m_LabelReadingData.strReadingResult = strOcrResult;

	BOOL bOcrCompare = strOcrResult == strOcrInfo ? TRUE : FALSE;
	if (!bOcrCompare)
	{
		strOcrInfo.MakeUpper();
		strOcrResult.MakeUpper();

		if (strOcrInfo == strOcrResult)
			bOcrCompare = TRUE;

		if(!bOcrCompare)
		{
			if (strOcrInfo.GetLength() == strOcrResult.GetLength())
			{
				// ----- Change Similar Characters -----
				CheckSimilarFont(strOcrInfo, strOcrResult);
				if (strOcrInfo == strOcrResult)
					bOcrCompare = TRUE;
			}
		}
	}

	if (strOcrResult.GetLength()) 
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("OCR 결과 : %s"), strOcrResult);
		else												clrText.SetText(_T("OCR Result : %s"), strOcrResult);
		clrText.CreateObject(bOcrCompare ? PDC_GREEN : PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI*4, 70, TRUE);
		pGO->AddDrawText(clrText);
	}
	else
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("Character not recognized."));
		else												clrText.SetText(_T("문자를 인식할 수 없습니다."));
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI*4, 70, TRUE);
		pGO->AddDrawText(clrText);
	}

	int nOCRCount = eOCR.GetNumChars();

	int nMinPosY = 0, nMaxPosY = 0, nMinSize = 0;
	for (int nIndex = 0; nIndex < nOCRCount; nIndex++)
	{
		float fScore = eOCR.GetConfidenceRatio(nIndex);

		CRect reCharArea;
		reCharArea.left = eImageROI.GetOrgX() + eOCR.CharGetOrgX(nIndex - nMinSize);
		reCharArea.top = eImageROI.GetOrgY() + eOCR.CharGetOrgY(nIndex - nMinSize);
		reCharArea.right = reCharArea.left + eOCR.CharGetWidth(nIndex - nMinSize);
		reCharArea.bottom = reCharArea.top + eOCR.CharGetHeight(nIndex - nMinSize);
		
		if (reCharArea.Width() < 5 || reCharArea.Height() < 5)
		{
			nMinSize++;
			continue;
		}

		clrBox.CreateObject(PDC_LIGHTGREEN, reCharArea, PS_SOLID, 1);
		pGO->AddDrawBox(clrBox);

		CString strReading;
		strReading.Format(_T("[%c], %.1f"), strOcrResult[nIndex-nMinSize], fScore); //szOCRRecoText[nIndex]);	

		stTrayData.strSegmentText.push_back((CString)strOcrResult[nIndex-nMinSize]);
		stTrayData.reSegmentArea.push_back(reCharArea);
		stTrayData.bSegmentScoreResult.push_back(fScore >= 40.0f ? TRUE : FALSE);
		stTrayData.fSegmentScore.push_back(fScore);

		clrText.SetText(strReading);
		clrText.CreateObject(PDC_LIGHTGREEN, (reCharArea.left + reCharArea.right) / 2, reCharArea.top-20, 15, TRUE, CxGOText::TextAlignmentCenter);
		pGO->AddDrawText(clrText);
	}
	return strOcrResult;
}

CString CInspectionVision::ReadOCR_Chip(EROIBW8& eImageROI, CxGraphicObject* pGO, EOCR& eOCR, VisionProcess::ChipOCRData& stChipData,
										UINT nMinWidth, UINT nMaxWidth, UINT nMinHeight, UINT nMaxHeight, int nThreshold,
										int nFontColor, BOOL bRemoveBorder, BOOL bRemoveNarrowOrFlat, BOOL bCutLargeChar, UINT nCharSpacing, UINT nNoiseArea)
{
	COLORTEXT clrText;
	COLORBOX clrBox;
	CString strOcrResult = NULL;
	UINT nOcrClasses = EOCRClass_AllClasses;
	stChipData.clear();

	USES_CONVERSION;

	eOCR.SetMinCharWidth(nMinWidth);
	eOCR.SetMaxCharWidth(nMaxWidth);
	eOCR.SetMinCharHeight(nMinHeight);
	eOCR.SetMaxCharHeight(nMaxHeight);
	eOCR.SetCharSpacing(nCharSpacing);
	eOCR.SetNoiseArea(nNoiseArea);
	eOCR.SetCutLargeChars(bCutLargeChar);
	eOCR.SetRemoveNarrowOrFlat(bRemoveNarrowOrFlat);
	eOCR.SetRemoveBorder(bRemoveBorder);
	eOCR.SetRelativeSpacing(0.05f);
	eOCR.SetSegmentationMode(ESegmentationMode_RepasteObjects);

	if (nFontColor == OBJ_BLACK)
	{
		eOCR.SetTextColor(EOCRColor_BlackOnWhite);
		if(nThreshold == 0)		eOCR.SetThreshold(EThresholdMode_MinResidue);
		else					eOCR.SetThreshold(nThreshold);
	}
	else
	{
		eOCR.SetTextColor(EOCRColor_WhiteOnBlack);
		//eOCR.SetThreshold(EThresholdMode_MaxEntropy);
		if (nThreshold == 0)	eOCR.SetThreshold(EThresholdMode_MinResidue);
		else					eOCR.SetThreshold(nThreshold);
	}

	eOCR.SetMatchingMode(EMatchingMode_Rms);
	eOCR.SetShiftingMode(EShiftingMode_Text);
	eOCR.SetShiftXTolerance(0);
	eOCR.SetShiftYTolerance(0);

	strOcrResult = (CString)eOCR.Recognize(&eImageROI, 256, nOcrClasses).c_str();
	m_LabelReadingData.strReadingResult = strOcrResult;

	int nOCRCount = eOCR.GetNumChars();

	int nMinPosY = 0, nMaxPosY = 0;
	for (int nIndex = 0; nIndex < nOCRCount; nIndex++)
	{
		float fScore = eOCR.GetConfidenceRatio(nIndex);

		CRect reCharArea;
		reCharArea.left = eImageROI.GetOrgX() + eOCR.CharGetOrgX(nIndex);
		reCharArea.top = eImageROI.GetOrgY() + eOCR.CharGetOrgY(nIndex);
		reCharArea.right = reCharArea.left + eOCR.CharGetWidth(nIndex);
		reCharArea.bottom = reCharArea.top + eOCR.CharGetHeight(nIndex);
		if (reCharArea.Width() < 5 || reCharArea.Height() < 5) continue;

		clrBox.CreateObject(PDC_LIGHTGREEN, reCharArea, PS_SOLID, 1);
		pGO->AddDrawBox(clrBox);

		CString strReading;
		strReading.Format(_T("[%c]"), strOcrResult[nIndex]);	

		stChipData.strSegmentText.push_back((CString)strOcrResult[nIndex]);
		stChipData.reSegmentArea.push_back(reCharArea);
		stChipData.bSegmentScoreResult.push_back(fScore >= 40.0f ? TRUE : FALSE);
		stChipData.fSegmentScore.push_back(fScore);

		clrText.SetText(strReading);
		clrText.CreateObject(PDC_LIGHTGREEN, (reCharArea.left + reCharArea.right) / 2, reCharArea.top-20, 20, TRUE, CxGOText::TextAlignmentCenter);
		pGO->AddDrawText(clrText);

	}
	return strOcrResult;
}

void CInspectionVision::SetOcrROI(EROIBW8& eImageX, EROIBW8& eOcrROI, CxGraphicObject* pGO, CPoint ptChipInside, int nOcrAreaDistanceX, int nOcrAreaDistanceY, CRect rcOcrROI)
{
	CString strException = _T("");
	CRect rcOCR;
	rcOCR.left = ptChipInside.x - nOcrAreaDistanceX;
	rcOCR.top = ptChipInside.y - nOcrAreaDistanceY;
	rcOCR.right = ptChipInside.x - nOcrAreaDistanceX + rcOcrROI.Width();
	rcOCR.bottom = ptChipInside.y - nOcrAreaDistanceY + rcOcrROI.Height();

	COLORTEXT clrText;
	COLORARROW clrArrow;
	clrArrow.CreateObject(PDC_BLUE, ptChipInside, CPoint(rcOCR.left, ptChipInside.y - nOcrAreaDistanceY), CxGOArrow::ArrowDirectionBoth, CxGOArrow::ArrowHeadTypeLine, 5);
	pGO->AddDrawArrow(clrArrow);

	// 이미지 범위 벗어났을 때 예외처리 코드 추가, 250204 황대은J
	if (rcOCR.left < 0 || rcOCR.top < 0 || rcOCR.right >= eImageX.GetWidth() || rcOCR.bottom >= eImageX.GetHeight())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)
			strException.Format(_T("검사 영역이 이미지 밖에 있습니다. LT: (%d, %d), RB: (%d, %d)"), rcOCR.left, rcOCR.top, rcOCR.right, rcOCR.bottom);
		else 
			strException.Format(_T("The Inspection Area Is Outside The Image. LT: (%d, %d), RB: (%d, %d)"), rcOCR.left, rcOCR.top, rcOCR.right, rcOCR.bottom);

		WRITE_LOG(WL_ERROR, strException);

		clrText.SetText(strException);
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI * 6, DEF_FONT_BASIC_SIZE, TRUE);
		pGO->AddDrawText(clrText);

		return;
	}
	eOcrROI.Attach(&eImageX);
	eOcrROI.SetPlacement(rcOCR.left, rcOCR.top, rcOCR.Width(), rcOCR.Height());

	COLORBOX clrBox;
	clrBox.CreateObject(PDC_ORANGE, rcOCR, PS_SOLID, 2);
	pGO->AddDrawBox(clrBox);
}

BOOL CInspectionVision::InspChipCount(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, int nGrabCnt)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	CModelInfo::stChip& stChip = CModelInfo::Instance()->GetChip();

	COLORBOX  clrBox;
	COLORLINE clrLine;
	COLORTEXT clrText;

	BOOL bResult = TRUE;
	CString str;
	CRect rcRect;
	rcRect.SetRectEmpty();
	
	int nScanColCnt = 0, nScanRowCnt = 0, nInspCol = 0, nInspRow = 0, nResultMatrix_OffsetX = 0, nResultMatrix_OffsetY = 0;
	CVisionSystem::Instance()->GetMetrixInfoCount(nGrabCnt, nScanColCnt, nScanRowCnt, nInspCol, nInspRow, nResultMatrix_OffsetX, nResultMatrix_OffsetY);

	try
	{
		EImageBW8 ImageX;
		ImageX.SetImagePtr(pSrcImgObj->GetWidth(), pSrcImgObj->GetHeight(), pSrcImgObj->GetImageBuffer(), pSrcImgObj->GetWidthBytes() * 8);

		for (int nY = 0; nY < nInspCol; ++nY)
		{
			for (int nX = 0; nX < nInspRow; ++nX)
			{
				BOOL bScoreOK = TRUE;
				BOOL bSiftOK = TRUE;

				if (stChip.nSelectUnit == 0) // px
				{
					rcRect.left = stChip.rcInspArea.left - stChip.nChipUnitPitch_X_PX * nX;
					rcRect.right = stChip.rcInspArea.right - stChip.nChipUnitPitch_X_PX * nX;
					rcRect.top = stChip.rcInspArea.top - stChip.nChipUnitPitch_Y_PX * nY;
					rcRect.bottom = stChip.rcInspArea.bottom - stChip.nChipUnitPitch_Y_PX * nY;
				}
				else
				{
					rcRect.left = stChip.rcInspArea.left - GetRoundLong((stChip.fChipUnitPitch_X_MM * 1000.f / m_fCalX[nViewIndex]) * nX);
					rcRect.right = stChip.rcInspArea.right - GetRoundLong((stChip.fChipUnitPitch_X_MM * 1000.f / m_fCalX[nViewIndex]) * nX);
					rcRect.top = stChip.rcInspArea.top - GetRoundLong((stChip.fChipUnitPitch_Y_MM * 1000.f / m_fCalY[nViewIndex]) * nY);
					rcRect.bottom = stChip.rcInspArea.bottom - GetRoundLong((stChip.fChipUnitPitch_Y_MM * 1000.f / m_fCalY[nViewIndex]) * nY);
				}

				EROIBW8 EBW8InspROI;
				EBW8InspROI.Attach(&ImageX);
				EBW8InspROI.SetPlacement(rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height());

				float fScore = 0.f;
				EasyImage::PixelAverage(&EBW8InspROI, fScore);
				
				// spec 보다 낮으면
				if (fScore < stChip.nAvgGv) 		bScoreOK = FALSE;
				else								CVisionSystem::Instance()->m_nChipInspCnt++;

				clrBox.CreateObject(bScoreOK ? PDC_GREEN : PDC_BLUE, rcRect, PS_DASHDOT, 3);
				pGO->AddDrawBox(clrBox);
				
				if (CLanguageInfo::Instance()->m_nLangType == 0)	str.Format(_T("평균 GV : %.2f"), fScore);
				else												str.Format(_T("Avg GV : %.2f"), fScore);
				clrText.SetText(str);
				clrText.CreateObject(bScoreOK ? PDC_GREEN : PDC_BLUE, rcRect.left + 20, rcRect.top - 30, 50, TRUE);
				pGO->AddDrawText(clrText);

				// Shift 검사.
				if (bScoreOK)
					bSiftOK = MatchModel_Chip(pGO, pSrcImgObj, nViewIndex, rcRect);

				CInspTabObj* pTabObject = CInspTabObj::GetPtr();
				if (nGrabCnt + 1 <= nScanColCnt)
				{
					pTabObject->SetObjectInfoResult(nX + nResultMatrix_OffsetX, nY + nResultMatrix_OffsetY, bScoreOK ? UIExt::CObjectInfoCtrlEx::UnitType_OK : UIExt::CObjectInfoCtrlEx::UnitType_Empty);

					if (!bSiftOK)
					{
						pTabObject->SetObjectInfoResult(nX + nResultMatrix_OffsetX, nY + nResultMatrix_OffsetY, UIExt::CObjectInfoCtrlEx::UnitType_Error);
						CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TRAYCHIP);
						bResult = FALSE;
						CVisionSystem::Instance()->m_nChipInspCnt--;
					}
				}
				else
				{
					//int reverseY = stChip.nMatrix_Y - (nInspCol * (nGrabCnt+1 - nScanColCnt));
					pTabObject->SetObjectInfoResult(nX + nResultMatrix_OffsetX, nY + nResultMatrix_OffsetY, bScoreOK ? UIExt::CObjectInfoCtrlEx::UnitType_OK : UIExt::CObjectInfoCtrlEx::UnitType_Empty);

					if (!bSiftOK)
					{
						pTabObject->SetObjectInfoResult(nX + nResultMatrix_OffsetX, nY + nResultMatrix_OffsetY, UIExt::CObjectInfoCtrlEx::UnitType_Error);
						CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TRAYCHIP);
						bResult = FALSE;
						CVisionSystem::Instance()->m_nChipInspCnt--;
					}
				}
			}
		}

		// 마지막 Grab에서는 수량 체크 확인
		if (CVisionSystem::Instance()->m_bChipCntHandlerStart)
		{
			if ((nGrabCnt + 1) == (nScanColCnt * nScanRowCnt))
			{
				int nTotal = CVisionSystem::Instance()->m_nMMIChipCnt;
				int nInsp = CVisionSystem::Instance()->m_nChipInspCnt;

				if (nTotal == nInsp)
				{
					bResult = TRUE;
					CVisionSystem::Instance()->m_bChipCntResult = TRUE;
				}
				else
				{
					bResult = FALSE;
					CVisionSystem::Instance()->m_bChipCntResult = FALSE;
					CVisionSystem::Instance()->SetErrorCode(nViewIndex, ISRESULT_ERROR_TRAYCHIP);
				}

				if (CLanguageInfo::Instance()->m_nLangType == 0)	str.Format(_T("Chip 수량 : [%d / %d]"), nInsp, nTotal);
				else												str.Format(_T("Chip QTY : [%d / %d]"), nInsp, nTotal);
				clrText.SetText(str);
				clrText.CreateObject(bResult ? PDC_GREEN : PDC_RED, DEF_FONT_BASIC_POSI, DEF_FONT_BASIC_POSI*7, 70, TRUE);
				pGO->AddDrawText(clrText);
			}
		}
	}
	catch (EException& e)
	{
		return DisplayEException(pGO, e);
	}
	return bResult;
}

BOOL CInspectionVision::PreviewImage_ForSelingQuality(CxImageObject* pImgObj, CxImageObject &pDstImgObj)
{
	pDstImgObj.Clone(pImgObj);

	EImageBW8 ImageX;
	ImageX.SetImagePtr(pDstImgObj.GetWidth(), pDstImgObj.GetHeight(), pDstImgObj.GetImageBuffer(), pDstImgObj.GetWidthBytes() * 8);

	EasyImage::ConvolSobel(&ImageX, &ImageX);
//	EasyImage::Threshold(&ImageX, &ImageX, EThresholdMode_MinResidue);
	
	return TRUE;
}

BOOL CInspectionVision::PreviewImage_ForDesiccantPosition(CxImageObject* pImgObj, CxImageObject &pDstImgObj)
{
	pDstImgObj.Clone(pImgObj);

	CModelInfo::stDesiccantCuttingInfo& stDesiccant = CModelInfo::Instance()->GetDesiccantCuttingInfo();

	EImageBW8 ImageX;
	ImageX.SetImagePtr(pDstImgObj.GetWidth(), pDstImgObj.GetHeight(), pDstImgObj.GetImageBuffer(), pDstImgObj.GetWidthBytes() * 8);

	//EasyImage::GainOffset(&ImageX, &ImageX, stDesiccant.fROIGain, 0.f);
	EasyImage::OpenBox(&ImageX, &ImageX, stDesiccant.nOpenBoxWidth, stDesiccant.nOpenBoxHeight); // Erode or Open?

	return TRUE;
}

BOOL CInspectionVision::SetInspPosition(int nViewIndex, CRect& rcTeachedMaskArea, RECT rcTrackRegion, CxGraphicObject* pGO)
{
	CModelInfo::stLabelInfo& stLabelInfo = CModelInfo::Instance()->GetLabelInfo();

	double dAngle = 0.f;
	double dSinT = sin(dAngle);
	double dCosT = cos(dAngle);

	double dQ1, dQ2, dQ3, dQ4;

	rcTeachedMaskArea.left -= m_ptAlign.x;
	rcTeachedMaskArea.right -= m_ptAlign.x;
	rcTeachedMaskArea.top -= m_ptAlign.y;
	rcTeachedMaskArea.bottom -= m_ptAlign.y;

	dQ1 = rcTeachedMaskArea.left * dCosT;
	dQ2 = rcTeachedMaskArea.top * dSinT;
	dQ3 = rcTeachedMaskArea.left * dSinT;
	dQ4 = rcTeachedMaskArea.top * dCosT;

	int nMaskW = rcTeachedMaskArea.Width();
	int nMaskH = rcTeachedMaskArea.Height();

	rcTeachedMaskArea.left = int(dQ1 - dQ2 + .5);
	rcTeachedMaskArea.top = int(dQ3 + dQ4 + .5);

	rcTeachedMaskArea.right = rcTeachedMaskArea.left + nMaskW;
	rcTeachedMaskArea.bottom = rcTeachedMaskArea.top + nMaskH;

	COLORBOX clrBox;
	clrBox.CreateObject(PDC_YELLOW, rcTrackRegion.left, rcTrackRegion.top, rcTrackRegion.right, rcTrackRegion.bottom);
	pGO->AddDrawBox(clrBox);

	return TRUE;
}

void CInspectionVision::MakeMarkAlignedInspectArea(CModelInfo::stLabelInfo::InspectArea& InspectArea, CPoint& ptAlignTopMark, double dAlignAngle)
{
	double dAngle = dAlignAngle; // - dRegisterAlignAngle;
	double dSinT = sin(-dAngle);
	double dCosT = cos(-dAngle);
	double dQ1, dQ2, dQ3, dQ4;

	InspectArea.AlignedAreas.clear();

	CModelInfo::stLabelInfo::InspVecIter iterBegin = InspectArea.Areas.begin();
	CModelInfo::stLabelInfo::InspVecIter iterEnd = InspectArea.Areas.end();
	for (CModelInfo::stLabelInfo::InspVecIter iter = iterBegin; iter != iterEnd; ++iter)
	{
		CRect rcArea = (*iter);
		int nAreaW = rcArea.Width();
		int nAreaH = rcArea.Height();
		dQ1 = rcArea.left * dCosT;
		dQ2 = rcArea.top * dSinT;
		dQ3 = rcArea.left * dSinT;
		dQ4 = rcArea.top * dCosT;

		rcArea.left = int(dQ1 - dQ2 + ptAlignTopMark.x + .5);
		rcArea.top = int(dQ3 + dQ4 + ptAlignTopMark.y + .5);

		rcArea.right = rcArea.left + nAreaW;
		rcArea.bottom = rcArea.top + nAreaH;

		InspectArea.AlignedAreas.push_back(rcArea);
	}
}

BOOL CInspectionVision::CalculateTrayRegion(CxGraphicObject* pGO, const CModelInfo::stDesiccantMaterialTrayInfo& stDesiccantTray, const CRect& rcTray, BOOL bManual)
{
	if (CVisionSystem::Instance()->GetValidEvisionDongle() != TRUE)
		return FALSE;

	COLORBOX clrBox;
	COLORTEXT clrText;
	const int nTrayQuantityMin = stDesiccantTray.nTrayQuantityMin;
	const int nTrayQuantityMax = stDesiccantTray.nTrayQuantityMax;
	const CRect rcTrayRoiMin = stDesiccantTray.rcTrayRoiMin;
	const CRect rcTrayRoiMax = stDesiccantTray.rcTrayRoiMax;

	const int nTrayQuantity = bManual ? CTeachTab3GDesiccantMaterial::m_nManualTray : CVisionSystem::Instance()->m_nMMITrayQuantity_SubMaterial;
	constexpr int nFontSize = 25;
	constexpr int nFontPosY = 700;

	if (nTrayQuantityMin == nTrayQuantityMax)
	{
		m_rcTray = rcTrayRoiMin;

		clrBox.CreateObject(PDC_LIGHTGREEN, m_rcTray, PS_DASH, 1);
		pGO->AddDrawBox(clrBox);
		return TRUE;
	}
	
	if (rcTrayRoiMin.IsRectEmpty() || rcTrayRoiMax.IsRectEmpty())
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 영역을 설정하세요."));
		else												clrText.SetText(_T("Set The Tray Area."));
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, nFontPosY, nFontSize, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}

	if (nTrayQuantity < nTrayQuantityMin)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량이 최소 수량 보다 적습니다."));
		else												clrText.SetText(_T("The Tray Quantity Is Less Than The Minimum Quantity."));
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, nFontPosY, nFontSize, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}

	if (nTrayQuantity > nTrayQuantityMax)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량이 최대 수량 보다 많습니다."));
		else												clrText.SetText(_T("Tray Quantity Is Greater Than Maximum."));
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, nFontPosY, nFontSize, TRUE);
		pGO->AddDrawText(clrText);
		return FALSE;
	}

	if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("트레이 수량 : %d"), nTrayQuantity);
	else												clrText.SetText(_T("Tray QTY : %d"), nTrayQuantity);
	clrText.CreateObject(PDC_GREEN, DEF_FONT_BASIC_POSI, nFontPosY, nFontSize, TRUE);
	pGO->AddDrawText(clrText);

	m_rcTray.left	= rcTrayRoiMin.left		- (abs(rcTrayRoiMin.left	- rcTrayRoiMax.left)	/ (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);
	m_rcTray.top	= rcTrayRoiMin.top		- (abs(rcTrayRoiMin.top		- rcTrayRoiMax.top)		/ (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);
	m_rcTray.right  = rcTrayRoiMin.right	+ (abs(rcTrayRoiMin.right	- rcTrayRoiMax.right)	/ (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);
	m_rcTray.bottom = rcTrayRoiMin.bottom	+ (abs(rcTrayRoiMin.bottom	- rcTrayRoiMax.bottom)	/ (nTrayQuantityMax - nTrayQuantityMin))	* (nTrayQuantity - nTrayQuantityMin);

	clrBox.CreateObject(PDC_LIGHTGREEN, m_rcTray, PS_DASH, 1);
	pGO->AddDrawBox(clrBox);

	return TRUE;
}

bool CInspectionVision::CompareX(const CPoint* pt1, const CPoint* pt2)
{
	return pt1->x < pt2->x;
}

bool CInspectionVision::CompareMatchX(const EMatchPosition eMatch1, const EMatchPosition eMatch2)
{
	return eMatch1.CenterX < eMatch2.CenterX;
}

BOOL CInspectionVision::FindSubMaterialObj(CxGraphicObject* pGO, std::vector<EROIBW8>& vecEROI, int nSubMaterialPixelVal, int nTrayOutTolerance)
{
	COLORBOX clrBox;
	COLORTEXT clrText;
	BOOL bResult = TRUE;
	
	ECodedImage2 CodedImage;
	EObjectSelection CodedImageObjectSelection;
	EImageEncoder CodedImageEncoder;

	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetMode(EGrayscaleSingleThreshold_Absolute);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetAbsoluteThreshold(nSubMaterialPixelVal);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetWhiteLayerEncoded(true);
	CodedImageEncoder.GetGrayscaleSingleThresholdSegmenter().SetBlackLayerEncoded(false);
	CodedImageEncoder.SetEncodingConnexity(EEncodingConnexity_Eight);

	for (int i = 0; i < vecEROI.size(); ++i)
	{
		BOOL bCurrent = TRUE;
		CodedImageEncoder.Encode(vecEROI[i], CodedImage);
		CodedImageObjectSelection.Clear();
		CodedImageObjectSelection.AddObjectsUsingFloatFeature(CodedImage, EFeature_BoundingBoxWidth, 50.f, ESingleThresholdMode_GreaterEqual);
		
		CRect rcROI;
		rcROI.left = vecEROI[i].GetOrgX();
		rcROI.top = vecEROI[i].GetOrgY();
		rcROI.right = vecEROI[i].GetOrgX() + vecEROI[i].GetWidth();
		rcROI.bottom = vecEROI[i].GetOrgY() + vecEROI[i].GetHeight();

		clrBox.CreateObject(PDC_ORANGE, rcROI, PS_DASH);
		pGO->AddDrawBox(clrBox);

		int nElemCnt = CodedImageObjectSelection.GetElementCount();

		if (!nElemCnt) continue;

		CRect rcElement;
		rcElement.left		= CodedImageObjectSelection.GetElement(0).GetLeftLimit()	+ vecEROI[i].GetOrgX();
		rcElement.top		= CodedImageObjectSelection.GetElement(0).GetTopLimit()		+ vecEROI[i].GetOrgY();
		rcElement.right		= CodedImageObjectSelection.GetElement(0).GetRightLimit()	+ vecEROI[i].GetOrgX();
		rcElement.bottom	= CodedImageObjectSelection.GetElement(0).GetBottomLimit()	+ vecEROI[i].GetOrgY();

		for (int j = 1; j < nElemCnt; ++j)
		{
			ECodedElement& eCodedElement = CodedImageObjectSelection.GetElement(j);

			rcElement.left		= min((int)rcElement.left, eCodedElement.GetLeftLimit()		+ vecEROI[i].GetOrgX());
			rcElement.top		= min((int)rcElement.top, eCodedElement.GetTopLimit()		+ vecEROI[i].GetOrgY());
			rcElement.right		= max((int)rcElement.right, eCodedElement.GetRightLimit()	+ vecEROI[i].GetOrgX());
			rcElement.bottom	= max((int)rcElement.bottom, eCodedElement.GetBottomLimit() + vecEROI[i].GetOrgY());
		}

		// 상하좌우 중 한 곳이라도 영역 벗어나면 NG
		if (rcElement.left <= vecEROI[i].GetOrgX()								|| rcElement.top <= m_rcTray.top - nTrayOutTolerance || 
			rcElement.right >= vecEROI[i].GetOrgX() + vecEROI[i].GetWidth() - 1	|| rcElement.bottom >= m_rcTray.bottom + nTrayOutTolerance)
		{
			bResult = FALSE;
			bCurrent = FALSE;
		} 
		clrBox.CreateObject(bCurrent ? PDC_GREEN : PDC_RED, rcElement, PS_SOLID, 2);
		pGO->AddDrawBox(clrBox);
	}

	if (!bResult)
	{
		if (CLanguageInfo::Instance()->m_nLangType == 0)	clrText.SetText(_T("NG : 부자재 정위치 이탈"));
		else												clrText.SetText(_T("NG : SubMaterial Misalignment"));
		clrText.CreateObject(PDC_RED, DEF_FONT_BASIC_POSI, 800, 25, TRUE);
		pGO->AddDrawText(clrText);
	}

	return bResult;
}

BOOL CInspectionVision::RectInImage(CRect& rcRect, const CxImageObject* pImgObj)
{
	const int nWidth = pImgObj->GetWidth();
	const int nHeight = pImgObj->GetHeight();

	rcRect.NormalizeRect();

	if (rcRect.left < 0 || rcRect.top < 0 || rcRect.right > nWidth || rcRect.bottom > nHeight)
		return FALSE;

	else
		return TRUE;
}

inline BOOL CInspectionVision::DisplayEException(CxGraphicObject* pGO, EException& e, const int nFontSize, const int nTextPosX, const int nTextPosY) const
{
	COLORTEXT clrText;

	std::string error = e.what();
	error += "\n";
	OutputDebugStringA(error.c_str());
	USES_CONVERSION;
	CString strError = EVISION_ERROR;
	strError += (CString)error.c_str();
	clrText.SetText(strError);
	clrText.CreateObject(PDC_RED, nTextPosX,  nTextPosY, nFontSize, TRUE);
	pGO->AddDrawText(clrText);

	WRITE_LOG(WL_ERROR, strError);

	return FALSE;
}

void CInspectionVision::ScaleRectAroundCenter(CRect& rect, float fScale)
{
	int centerX = rect.CenterPoint().x;
	int centerY = rect.CenterPoint().y;

	int width = rect.Width();
	int height = rect.Height();

	int newWidth = static_cast<int>(width * fScale);
	int newHeight = static_cast<int>(height * fScale);

	rect.left = centerX - newWidth / 2;
	rect.right = centerX + newWidth / 2;
	rect.top = centerY - newHeight / 2;
	rect.bottom = centerY + newHeight / 2;
}