#pragma once
#include "Common/xSingletoneObject.h"
#include <XGraphic/xDataTypes.h>
#include <vector>

#define MODELINFO_VERSION		0x0100			// 내용 변경 시 버전 업 필요	0x0101 -> 1.01


class CModelInfo : public CxSingleton<CModelInfo>
{
public:
	struct stTrayOcr		// TrayOcr 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_TrayOcr;
		CString strDescription;
		CString strTrayOcrInfo;
		long lPageCount;
		int nCharWidthMin;
		int nCharWidthMax;
		int nCharHeightMin;
		int nCharHeightMax;
		int nOcrAreaDistX;
		int nOcrAreaDistY;
		BOOL bRemoveBorder;
		BOOL bRemoveNarrowFlat;
		BOOL bCutLargeChar;
		BOOL bRotateImage;

		// Use Adaptive Threshold
		BOOL bUseAdaptiveThreshold;
		int nMethod;
		int nHalfKernelSize;
		int nThresholdOffset;

		CRect rcTabBegin;
		CRect rcInspArea;
		CRect rcLastInspArea; // 레시피로 관리 X

		CRect rcMaskingArea[nTRAY_MASKING_MAX];
		int nMaskingAreaDistX[nTRAY_MASKING_MAX];
		int nMaskingAreaDistY[nTRAY_MASKING_MAX];

		void Clear()
		{
			nUseBypass_TrayOcr = BYPASS_OFF;
			strDescription = _T("None");
			strTrayOcrInfo = _T("None");
			lPageCount = 10;

			nCharWidthMin = 15;
			nCharWidthMax = 100;
			nCharHeightMin = 15;
			nCharHeightMax = 90;
			nOcrAreaDistX = 0;
			nOcrAreaDistY = 0;

			bRotateImage = FALSE;
			bRemoveBorder = FALSE;
			bRemoveNarrowFlat = FALSE;
			bCutLargeChar = FALSE;
			bUseAdaptiveThreshold = FALSE;

			rcInspArea.SetRectEmpty();
			rcTabBegin.SetRectEmpty();
			rcLastInspArea.SetRectEmpty();
			nMethod = 0; // 0: mean, 1: median, 2: middle
			nHalfKernelSize = 25;
			nThresholdOffset = -2;

			for (int i = 0; i < nTRAY_MASKING_MAX; i++)
			{
				rcMaskingArea[i].SetRectEmpty();
				nMaskingAreaDistX[i] = 0;
				nMaskingAreaDistY[i] = 0;
			}
		}
	};

	struct st3DChipCnt		// 3DChipCnt 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_3DChipCnt;
		CString strDescription;

		int nSelectUnit;	// Measure Unit Select [0:Px / 1:mm]
		int nImageCut;
		int nImageCut2;
		int nShiftX;
		BOOL bEqualize;

		int nMinThreshold;
		int nMaxThreshold;

		CRect rcChipPosFirst; // First: Tray의 Left Top 칩

		// [2] : Px, mm
		int nRowPitchPx;
		int nColumnPitchPx;

		double dRowPitch;
		double dColumnPitch;

		int nXCount;
		int nYCount;

		void Clear()
		{
			nUseBypass_3DChipCnt = BYPASS_OFF;
			strDescription = _T("None");

			nSelectUnit = 0;
			nImageCut = 0;
			nImageCut2 = 0;
			nShiftX = 0;
			bEqualize = FALSE;

			rcChipPosFirst.SetRectEmpty();
			nMinThreshold = 1;
			nMaxThreshold = 245;

			nRowPitchPx = 1;
			nColumnPitchPx = 1;

			dRowPitch = 1;
			dColumnPitch = 1;

			nXCount = 8;
			nYCount = 15;
		}
	};

	struct stChip		// Chip에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Chip;
		CString strDescription;

		int nSelectUnit;	// Measure Unit Select [0:Px / 1:mm]

		int nValueCh1;
		int nValueCh2;
		int nValueCh3;

		int	nMatrix_X;						// Tray Matrix X
		int	nMatrix_Y;						// Tray Matrix Y
		int	nChipFovCnt_X;					// One Shot Insp Fov X
		int	nChipFovCnt_Y;					// One Shot Insp Fov Y

		float fChipUnitPitch_X_MM;			// Spacing Between Chip and Chip X (MM)
		float fChipUnitPitch_Y_MM;			// Spacing Between Chip and Chip Y (MM)
		int nChipUnitPitch_X_PX;			// Spacing Between Chip and Chip X (PX)
		int nChipUnitPitch_Y_PX;			// Spacing Between Chip and Chip Y (PX)

		CRect rcInspArea;
		CRect rcMatchArea;
		CPoint ptMatch;

		int nAvgGv;

		float fRatio;

		int nShiftX_PX;
		int nShiftY_PX;
		float fShiftX_MM;
		float fShiftY_MM;

		void Clear()
		{
			nUseBypass_Chip = BYPASS_OFF;
			strDescription = _T("None");

			nValueCh1 = 0;
			nValueCh2 = 0;
			nValueCh3 = 0;

			nSelectUnit = 0;

			nMatrix_X					=	1;
			nMatrix_Y					=	1;
			nChipFovCnt_X				=	1;
			nChipFovCnt_Y				=	1;
			fChipUnitPitch_X_MM			=	1.000f;
			fChipUnitPitch_Y_MM			=	1.000f;

			nChipUnitPitch_X_PX = 1;
			nChipUnitPitch_Y_PX = 1;

			rcInspArea.SetRectEmpty();
			rcMatchArea.SetRectEmpty();
#ifdef RELEASE_1G
			rcInspArea.right = nAREA_CAM_SIZE_X[IDX_AREA4];
			rcInspArea.bottom = nAREA_CAM_SIZE_Y[IDX_AREA4];

			rcMatchArea.right = nAREA_CAM_SIZE_X[IDX_AREA4];
			rcMatchArea.bottom = nAREA_CAM_SIZE_Y[IDX_AREA4];
#endif
			ptMatch = (0, 0);

			nAvgGv = 0;

			fRatio = 0.f;
			nShiftX_PX = 0;
			nShiftY_PX = 0;
			fShiftX_MM = 0.f;
			fShiftY_MM = 0.f;
		}
	};

	struct stChipOcr		// ChipOcr 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_ChipOcr;
		CString strDescription;
		CString strChipOcrInfo;

		int nValueCh1;
		int nValueCh2;
		int nValueCh3;

		int nFontColor;		// 0 : WhiteOnBlack, 1 : BlackOnWhite
		int nCharWidthMin;
		int nCharWidthMax;
		int nCharHeightMin;
		int nCharHeightMax;

		//int nOcrAreaWidth;		
		//int nOcrAreaHeight;
		int nOcrAreaDistanceX;
		int nOcrAreaDistanceY;

		BOOL bRemoveBorder;
		BOOL bRemoveNarrowFlat;
		BOOL bCutLargeChar;

		CRect rcInspArea;
		CRect rcInspInside;
		CRect rcOcrROI;

		int nThreshold;

		void Clear()
		{
			nUseBypass_ChipOcr = BYPASS_OFF;
			strDescription = _T("None");
			strChipOcrInfo = _T("");

			nValueCh1 = 0;
			nValueCh2 = 0;
			nValueCh3 = 0;

			nFontColor = OBJ_BLACK;
			nCharWidthMin = 15;
			nCharWidthMax = 90;
			nCharHeightMin = 15;
			nCharHeightMax = 110;

			//nOcrAreaWidth = 0;
			//nOcrAreaHeight = 0;
			nOcrAreaDistanceX = 0;
			nOcrAreaDistanceY = 0;

			rcInspArea.SetRectEmpty();
			rcInspInside.SetRectEmpty();
			rcOcrROI.SetRectEmpty();

			nThreshold = 128;
		}
	};

	struct stMixing		// Mixing 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Mixing;
		CString strDescription;

		long lPageCount;
		float fMatchingScore;
		int nTrayPixelValue;

		CPoint ptOffset[nMATCH_MAX];
		BOOL bIsPlainPattern;

		int nMixingZoneWidth;
		int nTrayCountZoneWidth;

		void Clear()
		{
			nUseBypass_Mixing = BYPASS_OFF;
			strDescription = _T("None");
			bIsPlainPattern = FALSE;

			lPageCount = 10;
			fMatchingScore = 30.f;
			nTrayPixelValue = 50;

			nMixingZoneWidth = 300;
			nTrayCountZoneWidth = 300;

			for (int j = 0; j < nMATCH_MAX; j++)
				ptOffset[j] = CPoint(0, 0);
		}
	};

	struct stLiftInfo		// LiftInfo에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Lift;
		CString strDescription;

		int nSelectUnit;	// Measure Unit Select [0:Px / 1:mm]

		long lPageCount;
		UINT8 nTrayPixelThreshold;
		double dbLiftHeight_MM;
		double dbBreakWidth_MM;
		double dbBreakHeight_MM;
		int nLiftHeight_PX;
		int nBreakWidth_PX;
		int nBreakHeight_PX;
		
		int nLeftOffset_Front;

		int nLeftOffset;
		int nRightOffset;
		int nTopOffset;
		int nBottomOffset;

		int nWidthFirst;
		int nWidthSecond;
		int nWidthThird;
		int nWidthFourth;

		int nRearWidthFirst;
		int nRearWidthSecond;
		int nRearWidthThird;
		int nRearWidthFourth;

		int nFrontNonInspStart;
		int nFrontNonInspWidth;

		void Clear()
		{
			nUseBypass_Lift = BYPASS_OFF;
			strDescription = _T("None");

			nSelectUnit = 0;
			lPageCount = 10;
			nTrayPixelThreshold = 200;
			dbLiftHeight_MM = 1.0;
			dbBreakWidth_MM = 10.0;
			dbBreakHeight_MM = 10.0;
			nLiftHeight_PX = 1;
			nBreakWidth_PX = 1;
			nBreakHeight_PX = 1;
			nLeftOffset_Front = 0;
			nLeftOffset = 0;
			nRightOffset = 0;
			nTopOffset = 0;
			nBottomOffset = 0;

			nWidthFirst = 0;
			nWidthSecond = 0;
			nWidthThird = 0;
			nWidthFourth = 0;

			nRearWidthFirst = 0;
			nRearWidthSecond = 0;
			nRearWidthThird = 0;
			nRearWidthFourth = 0;

			nFrontNonInspStart = 0;
			nFrontNonInspWidth = 0;
		}
	};

	struct stStackerOcr		// TrayOcr 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		CString strDescription;
		CString strOcrInfo;
		long lPageCount;
		int nCharWidthMin;
		int nCharWidthMax;
		int nCharHeightMin;
		int nCharHeightMax;
		int nOcrAreaDistX;
		int nOcrAreaDistY;
		BOOL bRemoveBorder;
		BOOL bRemoveNarrowFlat;
		BOOL bCutLargeChar;
		BOOL bRotateImage;

		// Use Adaptive Threshold
		BOOL bUseAdaptiveThreshold;
		int nMethod;
		int nHalfKernelSize;
		int nThresholdOffset;

		CRect rcLastInspArea; // 레시피로 관리 X

		int nDepthOffset;

		CRect rcMaskingArea[nTRAY_MASKING_MAX];
		int nMaskingAreaDistX[nTRAY_MASKING_MAX];
		int nMaskingAreaDistY[nTRAY_MASKING_MAX];

		void Clear()
		{
			strDescription = _T("None");
			strOcrInfo = _T("None");
			lPageCount = 4000;

			nOcrAreaDistX;
			nOcrAreaDistY;
			nCharWidthMin = 15;
			nCharWidthMax = 100;
			nCharHeightMin = 15;
			nCharHeightMax = 90;
			nOcrAreaDistX = 0;
			nOcrAreaDistY = 0;

			bRotateImage = FALSE;
			bRemoveBorder = FALSE;
			bRemoveNarrowFlat = FALSE;
			bCutLargeChar = FALSE;
			bUseAdaptiveThreshold = FALSE;

			rcLastInspArea.SetRectEmpty();
			nMethod = 0; // 0: mean, 1: median, 2: middle
			nHalfKernelSize = 25;
			nThresholdOffset = -2;

			nDepthOffset = 0;

			for (int i = 0; i < nTRAY_MASKING_MAX; i++)
			{
				rcMaskingArea[i].SetRectEmpty();
				nMaskingAreaDistX[i] = 0;
				nMaskingAreaDistY[i] = 0;
			}
		}
	};

	struct stBandingInfo		// Banding 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Banding;
		CString strDescription;

		int nValueCh1;

		CRect rcInspArea;

		int nThreshold;

		int nBandWidthSpec;
		int nBandWidthRange;

		void Clear()
		{
			nUseBypass_Banding = BYPASS_OFF;
			strDescription = _T("None");

			nValueCh1 = 0;

			rcInspArea.SetRectEmpty();
			rcInspArea.right = nAREA_CAM_SIZE_X[IDX_AREA1];
			rcInspArea.bottom = nAREA_CAM_SIZE_Y[IDX_AREA1];

			nThreshold = 20;

			nBandWidthSpec = 200;
			nBandWidthRange = 5;
		}
	};

	struct stHICInfo		// HIC 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_HIC;
		CString strDescription;

		int nValueCh1;

		int nColorRedLuminance;
		int nColorGreenLuminance;
		int nColorBlueLuminance;

		int nCircleOuterDiameter60;
		int nCircleOuterDiameter10;
		int nCircleOuterDiameter5;

		int nCircleInnerDiameter60;
		int nCircleInnerDiameter10;
		int nCircleInnerDiameter5;

		int nDisColor;
		int nPixelAvgValue;

			// Teaching
		CPoint	ptOffset_HIC;
		float fRatio;

		void Clear()
		{
			nUseBypass_HIC = BYPASS_OFF;
			strDescription = _T("None");

			nValueCh1 = 0;

			nColorRedLuminance = 50;
			nColorGreenLuminance = 50;
			nColorBlueLuminance = 50;

			nCircleOuterDiameter60 = 190;
			nCircleOuterDiameter10 = 230;
			nCircleOuterDiameter5 = 230;

			nCircleInnerDiameter60 = 70;
			nCircleInnerDiameter10 = 110;
			nCircleInnerDiameter5 = 110;

			nDisColor = 0;
			nPixelAvgValue = 0;

			ptOffset_HIC = CPoint(0, 0);
			fRatio = 0.7f;
		}
	};

	struct stDesiccantCuttingInfo		// Desiccant Cutting 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Desiccant_PositionLeft;
		int	nUseBypass_Desiccant_PositionRight;
		int	nUseBypass_Desiccant_CuttingLeft;
		int	nUseBypass_Desiccant_CuttingRight;
		CString strDescription;

		// Desiccant Cutting Line
		int nDesiccantCuttingPixelValue; // Desiccant Position과 다름
		double dSealingLength;
		double dColorLineLengthDifference;

		int nValueCh1;	// left top
		int nValueCh2;	// left btm
		int nValueCh3;	// right top
		int nValueCh4;	// right btm

		CRect rcLeftInspArea;
		CRect rcRightInspArea;
		
		// Desiccant Position
		int		nDesiccantPixelValue;
		int		nROIHeight;
		int		nROIWidth;
		int		nColorLineThreshold;
		double	dDesiccantWidth;
		double	dDesiccantHeight;
		double	dPosColorLineLengthDiff;
		//float	fROIGain;

		int nRoiOffsetLeft;
		int nRoiOffsetRight;
		int nOpenBoxWidth;
		int nOpenBoxHeight;

		CRect rcPositionLeftInspArea;
		CRect rcPositionRightInspArea;

		void Clear()
		{
			nUseBypass_Desiccant_PositionLeft = BYPASS_OFF;
			nUseBypass_Desiccant_PositionRight = BYPASS_OFF;
			nUseBypass_Desiccant_CuttingLeft = BYPASS_OFF;
			nUseBypass_Desiccant_CuttingRight = BYPASS_OFF;
			strDescription = _T("None");

			// Desiccant Cutting Line
			nDesiccantCuttingPixelValue = 254;
			dSealingLength = 50.0;
			dColorLineLengthDifference = 10.0;
			nColorLineThreshold = 120;

			nValueCh1 = 0;
			nValueCh2 = 0;
			nValueCh3 = 0;
			nValueCh4 = 0;

			rcLeftInspArea.SetRectEmpty();
			rcRightInspArea.SetRectEmpty();
			
			// Desiccant Position
			nDesiccantPixelValue = 250;
			nROIWidth = 200;
			nROIHeight = 200;
			dDesiccantWidth	 = 0.0;
			dDesiccantHeight = 0.0;
			//fROIGain = 1.5f; 
			dPosColorLineLengthDiff = 2.0;
			rcPositionLeftInspArea.SetRectEmpty();
			rcPositionRightInspArea.SetRectEmpty();

			nRoiOffsetLeft = 0;
			nRoiOffsetRight = 0;
			nOpenBoxWidth = 0;
			nOpenBoxHeight = 0;
		}
	};

	struct stDesiccantMaterialInfo		// Desiccant Material에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Material;
		CString strDescription;
		int nValueCh1;
		int nValueCh2;

		int nDesiccantQuantity;

		// Teaching
		//CPoint	ptOffset_HICExistShift;
		//CPoint	ptOffset_DesiccantExistShift;
		int nInspectionZoneCol;
		int nInspectionZoneMargin;
		int nSubMaterialPixelVal;
		double dbOutTolerance;
		//double dbPatternAngle;

		int nRatio1;
		int nRatio2;
		int nRatio3;

		CPoint ptHicShift;
		CPoint ptDesiccant1Shift;
		CPoint ptDesiccant2Shift;
		CPoint ptPatternShiftTolerance;

		float fMatchScoreHIC;
		float fMatchScoreDesiccant;

		void Clear()
		{
			nUseBypass_Material = BYPASS_OFF;
			strDescription = _T("None");
			nValueCh1 = 0;
			nValueCh2 = 0;
			nDesiccantQuantity = 1;

			//ptOffset_HICExistShift = CPoint(0, 0);
			//ptOffset_DesiccantExistShift = CPoint(0, 0);

			ptHicShift = CPoint(0, 0);
			ptDesiccant1Shift = CPoint(0, 0);
			ptDesiccant2Shift = CPoint(0, 0);
			ptPatternShiftTolerance = CPoint(0, 0);

			nInspectionZoneCol = 2;
			nInspectionZoneMargin = 5;
			nSubMaterialPixelVal = 250;
			dbOutTolerance = 0.;
			//dbPatternAngle = 0.;
			nRatio1 = 33;
			nRatio2 = 34;
			nRatio3 = 33;

			fMatchScoreHIC = 80.f;
			fMatchScoreDesiccant = 70.f;
		}
	};

	struct stDesiccantMaterialTrayInfo		// Desiccant Material Tray에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		//int	nUseBypass_Material;
		CString strDescription;

		int nTrayQuantityMin;
		int nTrayQuantityMax;

		// Teaching
		CRect	rcTrayRoiMin;
		CRect	rcTrayRoiMax;

		void Clear()
		{
			//nUseBypass_Material = BYPASS_OFF;
			strDescription = _T("None");

			nTrayQuantityMin = 2;
			nTrayQuantityMax = 10;

			rcTrayRoiMin.SetRectEmpty();
			rcTrayRoiMax.SetRectEmpty();
		}
	};

	struct stMBBInfo		// MBB 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_MBB;
		CString strDescription;
		int nValueCh1;

		CRect rcInspArea;

		float fTolerance;
		float fLength;

		int nWidth;
		int nHeight;
		int nRange;

		int nSelectUnit;	// Measure Unit Select [0:Px / 1:mm]
		int nWidth_PX;
		int nHeight_PX;
		int nRange_PX;

		// Teaching
		CPoint	ptOffset_MBB;
		float fRatio;

		void Clear()
		{
			nUseBypass_MBB = BYPASS_OFF;
			strDescription = _T("None");
			nValueCh1 = 0;

			rcInspArea.SetRectEmpty();
			rcInspArea.right = nAREA_CAM_SIZE_X[IDX_AREA1];
			rcInspArea.bottom = nAREA_CAM_SIZE_Y[IDX_AREA1];

			fTolerance = 200;
			fLength = 200;

			nWidth = 30;
			nHeight = 20;
			nRange = 2;

			nSelectUnit = 0;
			nWidth = 20;
			nHeight = 10;
			nRange = 5;

			ptOffset_MBB = CPoint(0, 0);
			fRatio = 0.7f;
		}
	};

	struct stBoxInfo		// Box 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Box;
		CString strDescription;
		int nValueCh1;
		
		float fOkScore;

		int nShiftX;
		int nShiftY;

		CPoint ptMatchCenter[nMATCH_MAX];

		void Clear()
		{
			nUseBypass_Box = BYPASS_OFF;
			strDescription = _T("None");
			nValueCh1 = 0;

			fOkScore = 80.0f;

			for (int i = 0; i < nMATCH_MAX; i++)
				ptMatchCenter[i] = CPoint(0, 0);

			nShiftX = 10;
			nShiftY = 10;
		}
	};

	struct stSealingInfo		// Sealing 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Sealing;
		CString strDescription;
		int nValueCh1;

		int nSelectUnit; // 0 : Tight, 1 : Loose
		int nCount;
		CRect rcInspArea;

		int nInspSpec;

		void Clear()
		{
			nUseBypass_Sealing = BYPASS_OFF;
			strDescription = _T("None");
			nValueCh1 = 0;

			nSelectUnit = 0;
			nCount = 100;

			rcInspArea.SetRectEmpty();
#ifdef RELEASE_5G
			rcInspArea.right = nAREA_CAM_SIZE_X[IDX_AREA2];
			rcInspArea.bottom = nAREA_CAM_SIZE_Y[IDX_AREA2];
#endif
			nInspSpec = 50000;
		}
	};

	struct stTapeInfo		// Tape 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Tape;
		CString strDescription;
		int nValueCh1;

		int nRedThreshold;
		int nGreenThreshold;
		int nBlueThreshold;

		int nTapeWidth;
		int nTapeHeight;

		void Clear()
		{
			nUseBypass_Tape = BYPASS_OFF;
			strDescription = _T("None");
			nValueCh1 = 0;

			nRedThreshold = 120;
			nGreenThreshold = 120;
			nBlueThreshold = 120;

			nTapeWidth = 50;
			nTapeHeight = 30;
		}
	};

	struct stLabelInfo		// Label 에 대한 정보 입니다. 변화 없이 공통으로 쓰일 가능성이 있습니다.
	{
		int	nUseBypass_Label;
		CString strDescription;
		int nValueCh1;

		float	fLabelEdgeOffset;
		float	fLabelAngleSpec;

		int		nLabelMaskingCount;
		int		nLabelMaskingScoreType[EMATCH_TYPE_LABEL_MAX];

		CPoint	ptOffset_Lable[EMATCH_TYPE_LABEL_MAX];
		CPoint	ptOffset_Align;

		BOOL	bLabelManualInsp;		// Label Manual Inspection (No Save)


	// Insp Area
		typedef std::vector<CRect>::iterator InspVecIter;
		typedef std::vector<CRect> InspVec;
		typedef struct _InspectArea
		{
			InspVec Areas;
			InspVec AlignedAreas;
		} InspectArea;
		InspectArea stInspectArea;

		void Clear()
		{
			strDescription = _T("None");
			nUseBypass_Label = BYPASS_OFF;
			nValueCh1 = 0;

			fLabelEdgeOffset = 1.0f;
			nLabelMaskingCount = 0;
			fLabelAngleSpec = 5.f;

			for(int j = 0; j < EMATCH_TYPE_LABEL_MAX; j++)
			{
				nLabelMaskingScoreType[j] = MASKING_SCORE_HIGH;
				ptOffset_Lable[j] = CPoint(0, 0);
			}

			ptOffset_Align = CPoint(0, 0);
			
			bLabelManualInsp = FALSE;
		}

		// Label SimilarFont Data
		std::vector<CString> vstrSimilarFont[2];
		void SimilarFontClear()
		{
			vstrSimilarFont[0].clear();
			vstrSimilarFont[1].clear();
		}
	};

	struct stLabelManualOptionInfo
	{
		int		nSegmentLine_Num;			// Line Number
		int		nImageTypeIndex;			// Image Type
		BOOL	bUseMaxThreshold;			// Threshold Check
		BOOL	bUseImageFiltering;			// Image Filtering Check
		BOOL	bUseeVisionLargeCharsCut;	// eVision Large Chars Cut Check

		CSize	FontMinSize; 
		CSize	FontMaxSize;

		void Clear()
		{
			nSegmentLine_Num			= -1;
			nImageTypeIndex				= -1;
			bUseMaxThreshold			= FALSE;
			bUseImageFiltering			= FALSE;
			bUseeVisionLargeCharsCut	= FALSE;

			FontMinSize = CSize(0, 0);
			FontMaxSize = CSize(0, 0);
		}
	};
public:
	CModelInfo(void);
	~CModelInfo(void);

private:
	DWORD			m_dwModelInfoVersion;
	CString			m_strModelName;
	CString			m_strModelNameTrayOcr;
	CString			m_strModelName3DChipCnt;
	CString			m_strModelNameChipOcr;
	CString			m_strModelNameChip;
	CString			m_strModelNameMixing;
	CString			m_strModelNameLift;
	CString			m_strModelNameStackerOcr;
	CString			m_strModelNameBanding;
	CString			m_strModelNameHIC;
	CString			m_strModelNameDesiccantCutting;
	CString			m_strModelNameDesiccantMaterial;
	CString			m_strModelNameDesiccantMaterialTray;
	CString			m_strModelNameMBB;
	CString			m_strModelNameBoxQuality;
	CString			m_strModelNameSealingQuality;
	CString			m_strModelNameTape;
	CString			m_strModelNameLabel;

	stTrayOcr			m_TrayOcr;
	st3DChipCnt			m_3DChipCnt;
	stChipOcr			m_ChipOcr;
	stChip				m_Chip;
	stMixing			m_Mixing;
	stLiftInfo			m_LiftInfo;
	stStackerOcr		m_StackerOcr;
	stBandingInfo		m_BandingInfo;
	stHICInfo			m_HICInfo;
	stDesiccantCuttingInfo		m_DesiccantCuttingInfo;
	stDesiccantMaterialInfo		m_DesiccantMaterialInfo;
	stDesiccantMaterialTrayInfo	m_DesiccantMaterialTrayInfo; // 부자재 검사에서 Tray Edge 검사용
	stMBBInfo			m_MBBInfo;
	stBoxInfo			m_BoxInfo;
	stSealingInfo		m_SealingInfo;
	stTapeInfo			m_TapeInfo;
	stLabelInfo			m_LabelInfo;
	stLabelManualOptionInfo	m_LabelManualOptionInfo[OP_MAX];

public:
	void New( LPCTSTR lpszModelName, CString strKind );
	BOOL Load( LPCTSTR lpszModelName, CString strKind = _T("") );
	void Delete( LPCTSTR lpszModelName );

	BOOL Save( eTeachTabIndex TabIndex = TEACH_TAB_IDX_ALL );
	BOOL SaveBypassMode( InspectType inspecttype = InspectTypeMax );
	CString GetTrayOcrInfo(LPCTSTR lpszModelName);
	void SetEmpty();
	BOOL IsValid();

	stTrayOcr& GetTrayOcr() { return m_TrayOcr; }
	st3DChipCnt& Get3DChipCnt() { return m_3DChipCnt; }
	stChipOcr& GetChipOcr() { return m_ChipOcr; }
	stChip& GetChip() { return m_Chip; }
	stMixing& GetMixing() { return m_Mixing; }
	stLiftInfo& GetLiftInfo() { return m_LiftInfo; }
	stStackerOcr& GetStackerOcr() { return m_StackerOcr; }
	stBandingInfo& GetBandingInfo() { return m_BandingInfo; }
	stHICInfo& GetHICInfo() { return m_HICInfo; }
	stDesiccantCuttingInfo& GetDesiccantCuttingInfo() { return m_DesiccantCuttingInfo; }
	stDesiccantMaterialInfo& GetDesiccantMaterialInfo() { return m_DesiccantMaterialInfo; }
	stDesiccantMaterialTrayInfo& GetDesiccantMaterialTrayInfo() { return m_DesiccantMaterialTrayInfo;  }
	stMBBInfo& GetMBBInfo() { return m_MBBInfo; }
	stBoxInfo& GetBoxInfo() { return m_BoxInfo; }
	stSealingInfo& GetSealingInfo() { return m_SealingInfo; }
	stTapeInfo& GetTapeInfo() { return m_TapeInfo; }
	stLabelInfo& GetLabelInfo() { return m_LabelInfo; }
	stLabelManualOptionInfo& GetLabelManualOptionInfo(Labe_Reading_Option OptionNo)	{ return m_LabelManualOptionInfo[OptionNo];}

	static CString GetRecipePath( LPCTSTR lpszModelName );
	static CString GetRecipeRootPath();

	static CString GetLastModelName();
	static CString GetLastModelNameTrayOcr();
	static CString GetLastModelName3DChipCnt();
	static CString GetLastModelNameChipOcr();
	static CString GetLastModelNameChip();
	static CString GetLastModelNameMixing();
	static CString GetLastModelNameLift();
	static CString GetLastModelNameStackerOcr();
	static CString GetLastModelNameBanding();
	static CString GetLastModelNameHIC();
	static CString GetLastModelNameDesiccantCutting();
	static CString GetLastModelNameDesiccantMaterial();
	static CString GetLastModelNameDesiccantMaterialTray();
	static CString GetLastModelNameMBB();
	static CString GetLastModelNameBoxQuality();
	static CString GetLastModelNameSealingQuality();
	static CString GetLastModelNameTape();
	static CString GetLastModelNameLabel();
	static BOOL IsExistModelName( LPCTSTR lpszModelName );

	LPCTSTR GetModelName() const { return m_strModelName; }
	LPCTSTR GetModelNameTrayOcr() const { return m_strModelNameTrayOcr; }
	LPCTSTR GetModelName3DChipCnt() const { return m_strModelName3DChipCnt; }
	LPCTSTR GetModelNameChipOcr() const { return m_strModelNameChipOcr; }
	LPCTSTR GetModelNameChip() const { return m_strModelNameChip; }
	LPCTSTR GetModelNameMixing() const { return m_strModelNameMixing; }
	LPCTSTR GetModelNameLift() const { return m_strModelNameLift; }
	LPCTSTR GetModelNameStackerOcr() const { return m_strModelNameStackerOcr; }
	LPCTSTR GetModelNameBanding() const { return m_strModelNameBanding; }
	LPCTSTR GetModelNameHIC() const { return m_strModelNameHIC; }
	LPCTSTR GetModelNameDesiccantCutting() const { return m_strModelNameDesiccantCutting; }
	LPCTSTR GetModelNameDesiccantMaterial() const { return m_strModelNameDesiccantMaterial; }
	LPCTSTR GetModelNameDesiccantMaterialTray() const { return m_strModelNameDesiccantMaterialTray; }
	LPCTSTR GetModelNameMBB() const { return m_strModelNameMBB; }
	LPCTSTR GetModelNameBoxQuality() const { return m_strModelNameBoxQuality; }
	LPCTSTR GetModelNameSealingQuality() const { return m_strModelNameSealingQuality; }
	LPCTSTR GetModelNameTape() const { return m_strModelNameTape; }
	LPCTSTR GetModelNameLabel() const { return m_strModelNameLabel; }

	void Rename(LPCTSTR lpszModelName) { m_strModelName = lpszModelName; }
	void RenameTrayOcr(LPCTSTR lpszModelName) { m_strModelNameTrayOcr = lpszModelName; }
	void Rename3DChipCnt(LPCTSTR lpszModelName) { m_strModelName3DChipCnt = lpszModelName; }
	void RenameChipOcr(LPCTSTR lpszModelName) { m_strModelNameChipOcr = lpszModelName; }
	void RenameChip(LPCTSTR lpszModelName) { m_strModelNameChip = lpszModelName; }
	void RenameMixing(LPCTSTR lpszModelName) { m_strModelNameMixing = lpszModelName; }
	void RenameLift(LPCTSTR lpszModelName) { m_strModelNameLift = lpszModelName; }
	void RenameStackerOcr(LPCTSTR lpszModelName) { m_strModelNameStackerOcr = lpszModelName; }
	void RenameBanding(LPCTSTR lpszModelName) { m_strModelNameBanding = lpszModelName; }
	void RenameHIC(LPCTSTR lpszModelName) { m_strModelNameHIC = lpszModelName; }
	void RenameDesiccantCutting(LPCTSTR lpszModelName) { m_strModelNameDesiccantCutting = lpszModelName; }
	void RenameDesiccantMaterial(LPCTSTR lpszModelName) { m_strModelNameDesiccantMaterial = lpszModelName; }
	void RenameDesiccantMaterialTray(LPCTSTR lpszModelName) { m_strModelNameDesiccantMaterialTray = lpszModelName; }
	void RenameMBB(LPCTSTR lpszModelName) { m_strModelNameMBB = lpszModelName; }
	void RenameBoxQuality(LPCTSTR lpszModelName) { m_strModelNameBoxQuality = lpszModelName; }
	void RenameSealingQuality(LPCTSTR lpszModelName) { m_strModelNameSealingQuality = lpszModelName; }
	void RenameTape(LPCTSTR lpszModelName) { m_strModelNameTape = lpszModelName; }
	void RenameLabel(LPCTSTR lpszModelName) { m_strModelNameLabel = lpszModelName; }
	
private:
	void SaveModelInfo( LPCTSTR lpszModelInfoFile );

	void SaveTrayOcr(LPCTSTR lpszModelInfoFile);
	void Save3DChipCnt(LPCTSTR lpszModelInfoFile);
	void SaveChipOcr(LPCTSTR lpszModelInfoFile);
	void SaveChip( LPCTSTR lpszModelInfoFile );
	void SaveMixing(LPCTSTR lpszModelInfoFile);
	void SaveLiftInfo(LPCTSTR lpszModelInfoFile);
	void SaveStackerOcr(LPCTSTR lpszModelInfoFile);
	void SaveBandingInfo(LPCTSTR lpszModelInfoFile);
	void SaveHICInfo(LPCTSTR lpszModelInfoFile);
	void SaveDesiccantCuttingInfo( LPCTSTR lpszModelInfoFile );
	void SaveDesiccantMaterialInfo( LPCTSTR lpszModelInfoFile );
	void SaveDesiccantMaterialTrayInfo( LPCTSTR lpszModelInfoFile );
	void SaveMBBInfo( LPCTSTR lpszModelInfoFile );
	void SaveBoxInfo( LPCTSTR lpszModelInfoFile );
	void SaveSealingInfo( LPCTSTR lpszModelInfoFile );
	void SaveTapeInfo( LPCTSTR lpszModelInfoFile );
	void SaveLabelInfo( LPCTSTR lpszModelInfoFile, LPCTSTR lpszCustomerModelInfoFile );
	void SaveLabelManualOptionInfo( LPCTSTR lpszCustomerModelInfoFile );

	void LoadTrayOcr(LPCTSTR lpszModelInfoFile);
	void Load3DChipCnt(LPCTSTR lpszModelInfoFile);
	void LoadChipOcr(LPCTSTR lpszModelInfoFile);
	void LoadChip( LPCTSTR lpszModelInfoFile );
	void LoadMixing(LPCTSTR lpszModelInfoFile);
	void LoadLiftInfo(LPCTSTR lpszModelInfoFile);
	void LoadStackerOcr(LPCTSTR lpszModelInfoFile);
	void LoadBandingInfo(LPCTSTR lpszModelInfoFile);
	void LoadHICInfo(LPCTSTR lpszModelInfoFile);
	void LoadDesiccantCuttingInfo(LPCTSTR lpszModelInfoFile);
	void LoadDesiccantMaterialInfo(LPCTSTR lpszModelInfoFile);
	void LoadDesiccantMaterialTrayInfo(LPCTSTR lpszModelInfoFile);
	void LoadMBBInfo( LPCTSTR lpszModelInfoFile );
	void LoadBoxInfo( LPCTSTR lpszModelInfoFile );
	void LoadSealingInfo( LPCTSTR lpszModelInfoFile );
	void LoadTapeInfo( LPCTSTR lpszModelInfoFile );
	void LoadLabelInfo( LPCTSTR lpszModelInfoFile, LPCTSTR lpszCustomerModelInfoFile );
	void LoadLabelManualOptionInfo( LPCTSTR lpszCustomerModelInfoFile );

public:
	void SaveSimilarFontInfo();
	void LoadSimilarFontInfo();

	void LoadLabelManualOptionInfo_Public( LPCTSTR lpszCustomerModelInfoFile ) { LoadLabelManualOptionInfo(lpszCustomerModelInfoFile); }
	void SaveLabelManualOptionInfo_Public( LPCTSTR lpszCustomerModelInfoFile ) { SaveLabelManualOptionInfo(lpszCustomerModelInfoFile); }
};


