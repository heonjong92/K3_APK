#pragma once

#include <XGraphic/xDataTypes.h>
#include <vector>

#include <Open_eVision_2_11.h>

#include "IDefectInfo.h"
#include "ModelInfo.h"

#define LABLE_INSPECTION_MAX_IMAGE_TYPE 3 
#define LABLE_INSPECTION_MAX_OPTUION 16

#define SUBMATERIAL_DESICCANT_TEXT_POS 100
#define SUBMATERIAL_HIC_TEXT_POS (SUBMATERIAL_DESICCANT_TEXT_POS * 6)
#define SUBMATERIAL_TEXT_SIZE 20

#define EVISION_ERROR _T("EVision Err: ")

class CxImageObject;
class CxGraphicObject;
class CAPKView;

using namespace Euresys::Open_eVision_2_11;

namespace Euresys
{
	namespace Open_eVision_2_11
	{
		class EMatcher;
		class EasyImage;
		class EWorldShape;
		class EPointGauge;
		class ELineGauge;
		class EException;
		class EOCR;
		class EBarCode;
	}
}

namespace VisionProcess {
	typedef struct _stLabelCodeData
	{
		CRect	reCodeArea;
		CString strCodeText;

		BOOL bReadingRet;
		BOOL bCheckLotID;
		int  nBarcodeCnt;
		int  nQRcodeCnt;

		void clear()
		{
			reCodeArea.SetRectEmpty();
			strCodeText.Empty();

			bReadingRet		= TRUE;
			bCheckLotID	= TRUE;
			nBarcodeCnt		= 0;
			nQRcodeCnt		= 0;
		}
	} LabelCodeData;

	typedef struct _LabelOCRData
	{ 
		BOOL	bInspoResult;

		CString strLineText;
		CRect	reLineArea;				

		CSize	FontMinSize; 
		CSize	FontMaxSize;
		int		nThreshold_Teaching;

		std::vector<CString>	strSegmentText;
		std::vector<CRect>		reSegmentArea;				
		std::vector<BOOL>		bSegmentScoreResult;	
		std::vector<float>		fSegmentScore;				

		void clear()
		{
			bInspoResult = FALSE;

			strLineText.Empty();
			reLineArea.SetRectEmpty();

			FontMinSize = CSize(0, 0);
			FontMaxSize = CSize(0, 0);
			nThreshold_Teaching = 0;

			strSegmentText.			clear();
			reSegmentArea.			clear();
			bSegmentScoreResult.	clear();
			fSegmentScore.			clear();
		}
	}LabelOCRData;

	typedef struct _TrayOCRData
	{
		BOOL	bInspoResult;

		CSize	FontMinSize;
		CSize	FontMaxSize;
		int		nThreshold_Teaching;

		std::vector<CString>	strSegmentText;
		std::vector<CRect>		reSegmentArea;
		std::vector<BOOL>		bSegmentScoreResult;
		std::vector<float>		fSegmentScore;

		void clear()
		{
			bInspoResult = FALSE;

			FontMinSize = CSize(15, 50);
			FontMaxSize = CSize(90, 120);
			nThreshold_Teaching = 0;

			strSegmentText.clear();
			reSegmentArea.clear();
			bSegmentScoreResult.clear();
			fSegmentScore.clear();
		}
	}TrayOCRData;

	typedef struct _ChipOCRData
	{
		BOOL	bInspoResult;

		CSize	FontMinSize;
		CSize	FontMaxSize;
		int		nThreshold_Teaching;

		std::vector<CString>	strSegmentText;
		std::vector<CRect>		reSegmentArea;
		std::vector<BOOL>		bSegmentScoreResult;
		std::vector<float>		fSegmentScore;

		void clear()
		{
			bInspoResult = FALSE;

			FontMinSize = CSize(15, 50);
			FontMaxSize = CSize(90, 120);
			nThreshold_Teaching = 0;

			strSegmentText.clear();
			reSegmentArea.clear();
			bSegmentScoreResult.clear();
			fSegmentScore.clear();
		}
	}ChipOCRData;

	typedef struct _SegmentCuttingData
	{
		BOOL	bContinue;
		int		nSelectPos;
		int		nSpaceChangeCount;
		float	fPrintingRatio;

		void clear()
		{
			bContinue			= FALSE;
			nSelectPos			= 0;
			nSpaceChangeCount	= 0;
			fPrintingRatio		= 0.f;
		}
	}SegmentCuttingData;

	typedef struct _SegmentReadingOption
	{
		BOOL	bUseFontsizeManualSetting;
		CSize	FontMinSize; 
		CSize	FontMaxSize;

		int		nOptionIndex;				// LABLE_INSPECTION_MAX_OPTUION
		int		nImageType;					// 0:Center Cut, 1:Algorithm Cut, 2:OneThird Cut, 3:TwoThirds Cut, 4:Original
		BOOL	bUseMaxThreshold;			// TRUE:Max Threshold, FALSE:Min Threshold
		BOOL	bUseImageFiltering;			// TRUE:Use Filtering, FALSE:UnUse Filtering
		BOOL	bUseFontSizeFiltering;		// TRUE:Use Filtering, FALSE:UnUse Filtering
		BOOL	bUseeVisionLargeCharsCut;	// TRUE:Use Large Chars Cut, FALSE:UnUse Large Chars Cut

		BOOL	bTeachingMode;

		void clear()
		{
			bUseFontsizeManualSetting	= FALSE;
			FontMinSize					= CSize(0, 0);
			FontMaxSize					= CSize(0, 0);

			nOptionIndex				= 0;
			nImageType					= 0;
			bUseMaxThreshold			= FALSE;
			bUseImageFiltering			= FALSE;
			bUseFontSizeFiltering		= FALSE;
			bUseeVisionLargeCharsCut	= FALSE;

			bTeachingMode				= FALSE;
		}
	}SegmentReadingOption;

	typedef struct _stOCRResult
	{
		CString					strReadingResult;
		std::vector<CString>	vstrReadingText;
		std::vector<CRect>		vreReadingArea;

		// Font Teaching
		std::vector<BOOL>		vbTeaching;
		std::vector<CString>	vstrTeachingText;

		void clear()
		{
			strReadingResult.Empty();
			vstrReadingText.clear();
			vreReadingArea.clear();

			vbTeaching.clear();
			vstrTeachingText.clear();
		}

	}stOCRResult;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
class CInspectionVision
{
protected:
	// 3D Chip Count
	UINT m_nExistingChips;

	// Chip Count
	Euresys::Open_eVision_2_11::EMatcher* m_pMatchChip;

	// Tray Lift
	enum TrayStatus {StatusOK = 0b00, StatusLift = 0b01, StatusBreak = 0b10, StatusAll = 0b11 };

	// HIC
	CRect m_rcHicCenter[nHIC_CIRCLE_MAX];

	// Desiccant
	Euresys::Open_eVision_2_11::EMatcher* m_pMatch[IDX_AREA_MAX][nMATCH_MAX]; // Box Quality 검사에서도 사용
	DPOINT m_dptHICCenter[IDX_AREA_MAX][nHIC_CIRCLE_MAX];
	float m_fHICAngle[IDX_AREA_MAX][nHIC_CIRCLE_MAX];		
	float m_fHICScale[IDX_AREA_MAX][nHIC_CIRCLE_MAX];	
	float m_fHICWidth[IDX_AREA_MAX][nHIC_CIRCLE_MAX];
	float m_fHICHeight[IDX_AREA_MAX][nHIC_CIRCLE_MAX];

	// Sub-Material Tray
	CRect m_rcTray;

	// Label
	Euresys::Open_eVision_2_11::EMatcher* m_pLabelMatch[EMATCH_TYPE_LABEL_MAX]; // [1/2][10]
	CRect	m_rcMaskArea_ForLabel[EMATCH_TYPE_LABEL_MAX];

	std::vector<VisionProcess::LabelCodeData>	m_LabelCodeData;
	std::vector<VisionProcess::LabelOCRData>	m_LabelOCRData;

	float m_fCalX[IDX_AREA_MAX];
	float m_fCalY[IDX_AREA_MAX];

public:
	CRect m_rcLabelArea;				// Label Edge Rect
	stOCRResult m_LabelReadingData;		// OcrReadingData
	BOOL m_bLabelAlignTeaching;			// Align Teaching Mode
	CPoint m_ptAlign;					// ROI 기준 Align 좌표

	// Image Merge
	CString m_strMergePath;				// Image Merge Path

public:
	CInspectionVision(void);
	~CInspectionVision(void);

	BOOL Save( LPCTSTR lpszRecipeName, eTeachTabIndex TabIndex );
	BOOL Load_Label( LPCTSTR lpszRecipeName );
	BOOL Load( LPCTSTR lpszRecipeName, CString strKind );
	BOOL SaveImage( CxImageObject* pImgObj, InspectType inspecttype, int nViewIndex, int nGrabCnt );
	BOOL SetDisplayBypass( CxGraphicObject* pGO, CxImageObject* pImgObj );
	BOOL MergeImage( CString strPath, BOOL bXMerge);

	// 공통 사용 함수
	BOOL RectInImage(CRect& rcRect, const CxImageObject* pImgObj);

	//////////////////////////////////////////////////////////////////////////
	// Auto Calibration
	BOOL _AutoCalibration(CxGraphicObject* pGO, InspectType inspecttype, int nViewIndex);
	BOOL _Auto_3DChipCnt(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Auto_Chip(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Auto_Lift(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Auto_Desiccant(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Auto_MBB(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);

	BOOL _Auto_FindLift(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, CRect rcRect, CRect& rcDst);
	BOOL _Auto_FindMBB(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, CRect& rcFindRect);
	BOOL _Auto_BuildUsingEasyObject(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, CRect& rcBlobRect, BOOL nKindOfObj, BOOL bColor, int nThreshold = 0, BOOL bDraw = TRUE);
	BOOL _Auto_BuildUsingEasyObject_Lift(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, CRect& rcBlobRect, BOOL nKindOfObj, BOOL bColor, int nThreshold = 0, BOOL bDraw = TRUE);
	//////////////////////////////////////////////////////////////////////////
	// inspection
	BOOL _Inspection( CxGraphicObject* pGO, InspectType inspecttype, int nViewIndex, int nGrabCnt, BOOL bManual=FALSE );

	// 1G
	BOOL _Insp_TrayOcr(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Insp_3DChipCnt(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Insp_ChipOcr(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	BOOL _Insp_Chip(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, int nGrabCnt);
	BOOL _Insp_Mixing(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, BOOL bManaul = FALSE);
	BOOL _Insp_Lift(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype);
	
	// SG
	BOOL _Insp_StackerOcr(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, BOOL bManual = FALSE);
	BOOL StackerOcr_ImageCut(CxImageObject* pSrcImgObj);

	// 2G
	BOOL _Insp_Banding( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL _Insp_HIC( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL ColorTipLuminanceCheck( CxGraphicObject* pGO, CxImageObject* pSrcImgObj);
	BOOL DisColorCheck( CxGraphicObject* pGO, CxImageObject* pSrcImgObj);

	// 3G
	BOOL _Insp_Desiccant_Position( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL _Insp_Desiccant( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL _Insp_SubMaterial( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype, BOOL bManual = FALSE);

	// 4G
	BOOL _Insp_MBB( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL FindMBB(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex);

	// 5G
	BOOL _Insp_BoxQuality( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL _Insp_SealingQuality( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL CheckHistogramData(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex);

	// 6G
	BOOL _Insp_Tape( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );

	// Label
	BOOL _Insp_Label( CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, InspectType inspecttype );
	BOOL LabelInspect( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, int nViewIndex, CRect& rcLabelRect );
	BOOL ReadCode( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, std::vector<READ_DATA_Label> LabelData, BOOL& bReadingRet, BOOL& bCheckLotID, InspectType inspecttype );
	BOOL InspeOCR( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, CRect rcLabelRect, std::vector<READ_DATA_Label> LabelData, BOOL bSegmentSave );

	BOOL CheckCode( CxGraphicObject* pGO, CxImageObject& pImgObj, std::vector<READ_DATA_Label> LabelData, LabelCodeData& CodeData, InspectType inspecttype );
	BOOL CheckString( LabelOCRData &stTargetData, CString strMaster );
	BOOL CheckSimilarFont( CString strMaster, CString& strTarget );
	BOOL ClearSpecialFont( CString strSrc, CString &strDst, std::vector<int> &DeletePos );
	BOOL LargeCharacterFont( LabelOCRData &stTargetData, CString strSrc, CString &strDst );
	void GetBarcodeArea( CxImageObject& pMaskingImgObj, CxGraphicObject* pGO, CRect& rcBarcode );
	void BlobBarcodeLine( EImageBW8* pImageX, CRect rcBarcode, CxDPoint& dptBarcode );

	BOOL ReadingSegment( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, std::vector<CRect> reSegmentArea, std::vector<READ_DATA_Label> LabelData, CString strFontPatch, BOOL bSegmentSave );
	BOOL CuttingSegment( CxGraphicObject* pGO, EROIBW8& BW8CuttingImg, CRect reSegmentArea, int nCutting );
	BOOL EVisionOCRRead( EImageBW8 SegmentImage, LabelOCRData& LabelData, std::vector<READ_DATA_Label> &MasterData, CString strFontPatch, SegmentReadingOption stReadingOption );
	BOOL EVisionOCRAddFont(EROIBW8 SegmentImage, EOCR *OCRFont, CString strFontPatch, BOOL bUseAdaptiveThreshold = FALSE);

	BOOL CreateLabelData( std::vector<READ_DATA_Label> &LabelData);	

	BOOL MaskingArea_Black(EROIBW8 eRoiImage, CRect rcRect, int nGv );
	BOOL MaskingArea( CxGraphicObject* pGO, CxImageObject& pMaskingImgObj, CRect rcRect );
	BOOL MaskingArea( EImageBW8& pSrc, EImageBW8& pMaskingImgObj, CRect rcRect );

	//////////////////////////////////////////////////////////////////////////
	BOOL LineGauge(	CxImageObject* pImgObj, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice,
		DPOINT& dptCenter, float fTolerance, float fLength, float fAngle, 
		int nThreshold, int nThickness, int nMinAmplitude, int nMinArea, int nNumFilteringPasses, float fFilteringThreshold, float fSamplingStep,
		BOOL bHVConstraint,	BOOL bKnownAngle, BOOL bSampleResult, 
		OUT DPOINT& dptResult, OUT double& dResultAngle );

	BOOL LineGauge2( EImageBW8 pImageX, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice,
		DPOINT& dptCenter, float fTolerance, float fLength, float fAngle,
		int nThreshold, int nThickness, int nMinAmplitude, int nMinArea, int nNumFilteringPasses, float fFilteringThreshold, float fSamplingStep,
		BOOL bHVConstraint, BOOL bKnownAngle, BOOL bSampleResult,
		OUT DPOINT& dptResult, OUT double& dResultAngle);

	BOOL LineGauge3(CxImageObject* pImgObj, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice,
		DPOINT& dptCenter, float fTolerance, float fLength, float fAngle,
		int nThreshold, int nThickness, int nMinAmplitude, int nMinArea, int nNumFilteringPasses, float fFilteringThreshold, float fSamplingStep,
		BOOL bHVConstraint, BOOL bKnownAngle, BOOL bSampleResult,
		OUT DPOINT& dptResult, OUT double& dResultAngle, OUT DPOINT& dptSampleStart, OUT DPOINT& dptSampleEnd);

	BOOL PointGauge(CxImageObject* pImgObj, ETransitionType eTransitionType, ETransitionChoice eTransitionChoice,
		DPOINT& dptCenter, float fTolerance, float fAngle, int nThresholdGV, int nThickness, int nMinAmplitude,	OUT DPOINT& dptResult);

	BOOL EVisionOCR_TrayOCR(CxGraphicObject* pGO, CxImageObject* pImgObj, const int nViewIndex, VisionProcess::TrayOCRData& stTrayData, CModelInfo::stTrayOcr& stTray, BOOL bSave = FALSE);

	BOOL EVisionOCR_ChipOCR(CxGraphicObject* pGO, CxImageObject* pImgObj, VisionProcess::ChipOCRData& stChipData, 
							CRect& rcChipROI, CString strChipOcrInfo, int nViewIndex, BOOL bSave = FALSE);

	BOOL EVisionOCR_StackerOCR(CxGraphicObject* pGO, CxImageObject* pImgObj, int nViewIndex, VisionProcess::TrayOCRData& stTrayData, CModelInfo::stStackerOcr& stStacker, BOOL bSave = FALSE);

	BOOL MatchModel_Tab( CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, CRect& rcTabBegin );
	BOOL MatchModel_Chip(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, CRect rcRoi);
	BOOL MatchModel_ChipInside(CxGraphicObject* pGO, EROIBW8& eImgROI, int nViewIndex, CRect& rcChipInside);
	BOOL MatchModel_Mixing(CxGraphicObject* pGO, CxImageObject *pImgObj, const int nViewIndex, CModelInfo::stMixing& stMixingModel, const BOOL bManual = FALSE);
	BOOL MatchModel_HICType(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, CRect rcRect);
	BOOL MatchModel_HIC(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, int nType, BOOL bColor, CPoint ptPatternShift, CPoint ptHIC, const float fMatchScore );
	BOOL MatchModel_Desiccant( CxGraphicObject* pGO, CxImageObject *pImgObj, const int nViewIndex, const int nType, const int nDesiccantQuantity, CPoint ptPatternShift, std::vector<CPoint*>& vecPt, const float fMatchScore);
	BOOL MatchModel_MBB(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex);
	BOOL MatchModel_BoxQuality(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex); // , float fOkSCore);
	BOOL MatchModel_ForLabel(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex, int nType);
	BOOL MatchModel_ForLabelAlign(CxGraphicObject* pGO, CxImageObject *pImgObj, int nViewIndex);
	
	BOOL SetLearnModel( CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker, InspectType inspecttype, UINT nIdx, UINT nType, BOOL bBlob, BOOL bColor );
	BOOL SetLearnModel_ForLabel( CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker, UINT nIdx, UINT nType );
	BOOL SetLearnModel_ForBox(CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker, UINT nIdx, UINT nType);
	BOOL SetLearnModel_ForChip(CxGraphicObject* pGO, CxImageObject* pImgObj, RECT rcTracker);

	BOOL BuildUsingEasyObject( CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, CRect& rcBlobRect, BOOL nKindOfObj, BOOL bColor, int nThreshold = 0, BOOL bDraw = TRUE );
	BOOL BuildUsingEasyObject_TrayChipCnt(CxGraphicObject* pGO, CxImageObject* pImgObj, int nMinThreshold, int nMaxThreshold, CRect rcChipPos,
											double dRowPitch, double dColumnPitch, int nXCount, int nYCount);
	BOOL BuildUsingEasyObject_Lift(CxGraphicObject* pGO, CxImageObject* pImgObj, const int nViewIndex, const CModelInfo::stLiftInfo& stLift);
	BOOL BuildUsingEasyObject_SubMaterial(CxGraphicObject* pGO, CxImageObject* pImgObj, CRect& rcTray, int nInspZoneCol, int nRatio1, int nRatio2, int nRatio3, int nSubMaterialPixelVal, int nMargin, int nTrayOutTolerance);
	BOOL BuildUsingEasyObject_HICDisColor( CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, BOOL nKindOfObj, BOOL bColor, int nThreshold=0, BOOL bDraw = TRUE );
	BOOL BuildUsingEasyObject_ForLabelEdge( CxGraphicObject* pGO, CxImageObject* pImgObj, CRect rcRect, BOOL nKindOfObj, int nThreshold=0 );
	BOOL BuildUsingEasyObject_DesiccantCutting(CxGraphicObject *pGO, CxImageObject* pImgObj, int nViewIndex, CRect rcInspArea, int nDesiccantPixelValue, int nSealingLength, int nColorLineLengthDifference );
	BOOL BuildUsingEasyObject_DesiccantPosition(CxGraphicObject *pGO, CxImageObject* pImgObj, const int nViewIndex, CModelInfo::stDesiccantCuttingInfo& stDesiccant, BOOL m_bPreview);
	BOOL BuildUsingEasyObject_BoxTape(CxGraphicObject *pGO, CxImageObject* pImgObj, CRect& rcBlobRect, int nRedThreshold, int nGreenThreshold, int nBlueThreshold, int nTapeWidth, int nTapeHeight);
	BOOL BuildUsingEasyObject_BoxTapeBtm(CxGraphicObject *pGO, CxImageObject* pImgObj, CRect rcRect);

	//////////////////////////////////////////////////////////////////////////
	Euresys::Open_eVision_2_11::EMatcher* GetMatchData( int nViewIndex,  int nMatchIndex ) { return m_pMatch[nViewIndex][nMatchIndex]; }
	Euresys::Open_eVision_2_11::EMatcher* GetLabelMatchData(int nMatchIndex) { return m_pLabelMatch[nMatchIndex]; }
	Euresys::Open_eVision_2_11::EMatcher* GetMatchChipData() { return m_pMatchChip; }

	BOOL OCRImagePreview(CxImageObject* pImgObj);
	BOOL OCRImagePreview_stacker(CxImageObject* pImgObj);
	static bool CompareCenterX(const CRect& rect1, const CRect& rect2);
	static bool CompareWidth(const CRect& rect1, const CRect& rect2);

	BOOL PreviewImage_ForSelingQuality(CxImageObject* pImgObj, CxImageObject &pDstImgObj);
	BOOL PreviewImage_ForDesiccantPosition(CxImageObject* pImgObj, CxImageObject &pDstImgObj);

	// Label
	BOOL SetInspPosition(int nViewIndex, CRect& rcTeachedMaskArea, RECT rcTrackRegion, CxGraphicObject* pGO);
	void MakeMarkAlignedInspectArea(CModelInfo::stLabelInfo::InspectArea& InspectArea, CPoint& ptAlignTopMark, double dAlignAngle);

private:
	// 1G Tray Lift
	UINT CalcNumTray(EROIBW8& eImageROI, CxGraphicObject* pGO, UINT nThreshold); // For 1G Tray Lift
	UINT DetectBreakAndLIft(EROIBW8& eImageROI, CxGraphicObject* pGO, int nSelectUnit, const int nViewIndex, int nBreakWidth, int nBreakHeight, int nLiftHeight, int nNum);
	//////////////////////////////////////////////////////////////////////////

	// 1G Tray OCR
	CString ReadOCR_Tray(EROIBW8& eImageROI, CxGraphicObject* pGO, EOCR& eOCR, VisionProcess::TrayOCRData& stTrayData, CString strOcrInfo,
					UINT nMinWidth, UINT nMaxWidth, UINT nMinHeight, UINT nMaxHeight,
					BOOL bRemoveBorder = FALSE, BOOL bRemoveNarrowOrFlat = FALSE, BOOL bCutLargeChar = TRUE, BOOL bIsFontWhite = TRUE, UINT nCharSpacing = 1, UINT nNoiseArea = 50);

	// 1G Chip OCR
	CString ReadOCR_Chip(EROIBW8& eImageROI, CxGraphicObject* pGO, EOCR& eOCR, VisionProcess::ChipOCRData& stChipData,
						UINT nMinWidth, UINT nMaxWidth, UINT nMinHeight, UINT nMaxHeight, int nThreshold,
						int nFontColor = OBJ_BLACK, BOOL bRemoveBorder = FALSE, BOOL bRemoveNarrowOrFlat = FALSE, BOOL bCutLargeChar = TRUE, UINT nCharSpacing = 1, UINT nNoiseArea = 20);

	// 3G Desiccant Material
	BOOL FindSubMaterialObj(CxGraphicObject* pGO, std::vector<EROIBW8>& vecEROI, int nSubMaterialPixelVal, int nTrayOutTolerance);
	static bool CompareX(const CPoint* pt1, const CPoint* pt2);
	static bool CompareMatchX(const EMatchPosition eMatch1, const EMatchPosition eMatch2);

	BOOL CalculateTrayRegion(CxGraphicObject* pGO, const CModelInfo::stDesiccantMaterialTrayInfo& stDesiccantTray, const CRect& rcTray, BOOL bManual);

	void SetOcrROI(EROIBW8& ImageX, EROIBW8& eOcrROI, CxGraphicObject* pGO, CPoint ptHole, int nOcrAreaDistanceX, int nOcrAreaDistanceY, CRect rcOcrROI);
	BOOL InspChipCount(CxGraphicObject* pGO, CxImageObject* pSrcImgObj, int nViewIndex, int nGrabCnt);

	void ScaleRectAroundCenter(CRect& rect, float fScale);

	BOOL DisplayEException(CxGraphicObject* pGO, EException& e, const int nFontSize = 50, const int nTextPosX = 100, const int nTextPosY = 400) const;
};
}
