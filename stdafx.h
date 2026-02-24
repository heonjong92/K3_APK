
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS
 
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#include <afxdisp.h>        // MFC 자동화 클래스입니다.


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원
#include <afxwin.h>

//////////////////////////////////////////////////////////////////////////
// UI
#define DEF_BTN_COLOR_ON			RGB(21, 131, 203),	RGB(21, 131, 203),	RGB(80, 106, 126),	RGB(180, 180, 180),	RGB(255, 255, 255)
#define DEF_BTN_COLOR_OFF			RGB(80, 106, 126),	RGB(80, 106, 126),	RGB(21, 131, 203),	RGB(180, 180, 180),	RGB(255, 255, 255)

#define DEF_BTN_COLOR_SAVE			RGB(97, 135, 169),	RGB(97, 135, 169),	RGB(80, 106, 126),	RGB(180, 180, 180),	RGB(255, 255, 255)
#define DEF_BTN_COLOR_MEASURE		RGB(192, 64, 32),	RGB(192, 64, 32),	RGB(120, 120, 120),	RGB(180, 180, 180),	RGB(255, 255, 255)

#define DEF_BTN_COLOR_TEACING		RGB(200, 100, 0),   RGB(180, 90, 40),   RGB(160, 80, 80),	RGB(180, 180, 180), RGB(255, 255, 255)

#define DEF_BTN_COLOR_AUTORUN		RGB(120, 120, 120),	RGB(120, 120, 120),	RGB(60, 157, 38),	RGB(180, 180, 180),	RGB(255, 255, 255)
#define DEF_BTN_COLOR_STOP			RGB(120, 120, 120),	RGB(120, 120, 120),	RGB(196, 36, 52),	RGB(180, 180, 180),	RGB(255, 255, 255)

#define DEF_EXCEL_PASTEL_RED		RGB(248, 203, 173)
#define DEF_EXCEL_PASTEL_YELLOW		RGB(255, 230, 153)
#define DEF_EXCEL_PASTEL_GREEN		RGB(198, 224, 180)
#define DEF_EXCEL_PASTEL_BLUE		RGB(180, 198, 231)
//////////////////////////////////////////////////////////////////////////
// Program
enum eAccessRight { AccessRightOperator=0, AccessRightEngineer=1, AccessRightProgrammer=2 };
enum eRunStatus { RunStatusAutoRun=0, RunStatusStop };
enum eLogType { LogTypeNormal, LogTypeNotification, LogTypeError };

//////////////////////////////////////////////////////////////////////////
// Camera
enum eCamType { CamTypeAreaScan };
enum eCalibrationDir { CalibrationDirX=0, CalibrationDirY, CalibrationDirMax };
enum eCamChannel { CamChannel8BIT=1, CamChannel16BIT, CamChannel24BIT, CamChannel32BIT };
enum eGrabStatus { GrabStatusIdle=0, GrabStatusGrab, GrabStatusLive };

//////////////////////////////////////////////////////////////////////////
// Light
enum LedCtrlType { LedCtrlTypeArea1=0, LedCtrlTypeArea2, LedCtrlTypeArea3, LedCtrlTypeAreaMax };
enum eUSE_LFINE_LCP100_CTRL_CH_MAX { USE_LFINE_LCP100_CTRL_CH1=0, USE_LFINE_LCP100_CTRL_CH2, 
	USE_LFINE_LCP100_CTRL_CH3, USE_LFINE_LCP100_CTRL_CH4, USE_LFINE_LCP100_CTRL_CH_MAX};

//////////////////////////////////////////////////////////////////////////
// Tab
enum eInspTabIndex { INSP_TAB_IDX_LOG=0, INSP_TAB_IDX_OBJ, INSP_TAB_IDX_ALL };

//////////////////////////////////////////////////////////////////////////
// View
enum eViewMode { ViewModeAll=0, ViewModeTrayOcr, ViewMode3DChipCnt, ViewModeChipOcr, ViewModeChip, ViewModeMixing, ViewModeLift, ViewModeStackerOcr,
	ViewModeBanding, ViewModeHIC, ViewModeDesiccantCutting, ViewModeDesiccantMaterial, ViewModeMBB, ViewModeLabel, ViewModeBoxTape, ViewModeBox, ViewModeSealing };

//////////////////////////////////////////////////////////////////////////
// Bypass
enum eBypass { BYPASS_OFF=0, BYPASS_ON, BYPASS_ON_INSP, BYPASS_TYPE_MAX };
static const CString strBypassName[3] = {_T("BYPASS_OFF"), _T("BYPASS_ON"), _T("BYPASS_ON_INSP")};

void EnumerateSerialPorts( CUIntArray& ports );

//////////////////////////////////////////////////////////////////////////
// 변수
static const UINT nIMAGELOG_DRIVE_MAX = 26;
static const UINT nMATCH_MAX = 10;

//////////////////////////////////////////////////////////////////////////

#define WL_ERROR		_T("ERROR")
#define WL_MSG			_T("MSG")
#define WL_DEBUG		_T("DEBUG")
#define WL_SYSTEM		_T("SYSTEM")
#define WL_BTN			_T("BUTTON")
#define WL_TEACHING		_T("TEACHING")

#define WRITE_LOG			GetLogSystem().LogOut

// Reference
enum Object_Color { OBJ_BLACK=0, OBJ_WHITE };
enum OCR_Mode { OCR_TRAY=0, OCR_CHIP, OCR_LABEL }; // 250120 황대은J
enum HICNgKinds { HIC_DISCOLOR_ERROR = 0, HIC_CIRCLE_FINDING_ERROR }; // 250430 황대은J

// 자재 정보
static const UINT nHIC_DESICCANT_MIN_COUNT = 3;
static const UINT nHIC_CIRCLE_MAX = 3;
static const UINT nLABEL_COUNT_MAX = 3;

static const UINT nTRAY_MASKING_MAX = 5;

// Label
enum Labe_Reading_Option { OP_01=0, OP_02, OP_03, OP_04, OP_05, OP_06, OP_07, OP_08, OP_09, OP_10, OP_MAX };
enum eMatchType_LABEL	{ EMATCH_TYPE_LABEL_AREA_EMPTY = 0,
	EMATCH_TYPE_LABEL_AREA_0, EMATCH_TYPE_LABEL_AREA_1, EMATCH_TYPE_LABEL_AREA_2, EMATCH_TYPE_LABEL_AREA_3, EMATCH_TYPE_LABEL_AREA_4,
	EMATCH_TYPE_LABEL_AREA_5, EMATCH_TYPE_LABEL_AREA_6, EMATCH_TYPE_LABEL_AREA_7, EMATCH_TYPE_LABEL_AREA_8, EMATCH_TYPE_LABEL_AREA_9, 
	EMATCH_TYPE_LABEL_MAX  };
enum Masking_Score_Type { MASKING_SCORE_HIGH=0, MASKING_SCORE_MIDDLE, MASKING_SCORE_LOW };

enum eSocketPort { SOCKET_STACKEROCR = 0, SOCKET_TRAYOCR, SOCKET_3DCHIPCNT, SOCKET_CHIPOCR, SOCKET_CHIPCNT, SOCKET_MIXING, SOCKET_LIFT, 
	SOCKET_BANDING, SOCKET_HIC, 
	SOCKET_CUTTING1, SOCKET_CUTTING2, SOCKET_MATERIAL, 
	SOCKET_MBB, SOCKET_MBB_LABEL, 
	SOCKET_BOX, SOCKET_SEALING, 
	SOCKET_BOX_LABEL, SOCKET_TAPE,
	SOCKET_MAX };

enum Auto_Calibration_Mode { ACM_3DCHIPCNT = 0, ACM_CHIP, ACM_LIFT, ACM_DESI_31_R, ACM_DESI_32_L, ACM_MBB, ACM_MAX};

// Viewer
#ifdef RELEASE_1G
enum eIndex_Area { IDX_AREA1=0, IDX_AREA2, IDX_AREA3, IDX_AREA4, IDX_AREA5, IDX_AREA6, IDX_AREA7, IDX_AREA_MAX };
enum InspectType { InspectTypeTrayOcr=0, InspectType3DChipCnt, InspectTypeChipOcr, InspectTypeChip, InspectTypeMixing, InspectTypeLiftFront, InspectTypeLiftRear, InspectTypeMax };
enum eTeachTabIndex	{ TEACH_TAB_IDX_TRAYOCR=0, TEACH_TAB_IDX_3DCHIPCNT, TEACH_TAB_IDX_CHIPOCR, TEACH_TAB_IDX_CHIP, TEACH_TAB_IDX_MIXING, TEACH_TAB_IDX_LIFTINFO, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeLine=0, CameraType3DArea, CameraTypeArea, CameraTypeLine2, CameraTypeLine3, CameraTypeLine4, CameraTypeMax };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = { 3240, 1280, 1280, 4000, 16, 16, 16 };
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = { 16, 1024, 2048, 3000, 3240, 1600, 1600 };

#elif RELEASE_SG
enum eIndex_Area { IDX_AREA1 = 0, IDX_AREA_MAX };
enum InspectType { InspectTypeStackerOcr = 0, InspectTypeMax };
enum eTeachTabIndex { TEACH_TAB_IDX_STACKER_OCR = 0, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeLine = 0, CameraTypeMax };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = { 5000 };
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = { 1440 };

#elif RELEASE_2G
enum eIndex_Area { IDX_AREA1=0, IDX_AREA2, IDX_AREA_MAX };
enum InspectType { InspectTypeBanding=0, InspectTypeHIC, InspectTypeMax };
enum eTeachTabIndex	{ TEACH_TAB_IDX_BANDING=0, TEACH_TAB_IDX_HIC, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeArea=0, CameraTypeArea2, CameraTypeMax };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = {816, 1280};
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = {624, 1024};

#elif RELEASE_3G
enum eIndex_Area { IDX_AREA1=0, IDX_AREA2, IDX_AREA3, IDX_AREA4, IDX_AREA5, IDX_AREA_MAX };
enum InspectType { InspectTypePositionLeft=0, InspectTypePositionRight, InspectTypeDesiccantLeft, InspectTypeDesiccantRight, InspectTypeMaterial, InspectTypeMaterialTray, InspectTypeMax };
enum eTeachTabIndex	{ TEACH_TAB_IDX_DESICCANT_CUTTING=0, TEACH_TAB_IDX_DESICCANT_MATERIAL, TEACH_TAB_IDX_DESICCANT_MATERIAL_TRAY /*부자재 검사와 같은 탭, 뷰어 사용*/, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeArea=0, CameraTypeArea2, CameraTypeArea3, CameraTypeMax };
enum eMatchType_HIC	{ EMATCH_TYPE_HIC=0, EMATCH_TYPE_HIC_DESICCANT };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = { 1280, 1280, 1280, 1280, 1280 };
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = { 1024, 1024, 1024, 1024, 1024 };

#elif RELEASE_4G
enum eIndex_Area { IDX_AREA1=0, IDX_AREA2, IDX_AREA_MAX };
enum InspectType { InspectTypeMBB=0, InspectTypeLabel, InspectTypeMax };
enum eTeachTabIndex	{ TEACH_TAB_IDX_MBB=0, TEACH_TAB_IDX_LABEL, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeArea=0, CameraTypeArea2, CameraTypeMax };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = {2048, 4000};
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = {1536, 3000};

#elif RELEASE_5G
enum eIndex_Area { IDX_AREA1=0, IDX_AREA2, IDX_AREA_MAX };
enum InspectType { InspectTypeBox=0, InspectTypeSealing, InspectTypeMax };
enum eTeachTabIndex	{ TEACH_TAB_IDX_BOX=0, TEACH_TAB_IDX_SEALING, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeArea=0, CameraTypeArea2, CameraTypeMax };
//enum eMatchType_BoxQuality { EMATCH_TYPE_CAUTION = 0, EMATCH_TYPE_MOISTURE, EMATCH_TYPE_FRAGILE, EMATCH_TYPE_UMBRELLA, EMATCH_TYPE_RECYCLABLE };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = {2448, 2048};
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = {2048, 1536};

#elif RELEASE_6G
enum eIndex_Area { IDX_AREA1=0, IDX_AREA2, IDX_AREA_MAX };
enum InspectType { InspectTypeLabel=0, InspectTypeTape, InspectTypeMax };
enum eTeachTabIndex	{ TEACH_TAB_IDX_LABEL=0, TEACH_TAB_IDX_TAPE, TEACH_TAB_IDX_ALL };
enum CameraType { CameraTypeArea=0, CameraTypeArea2, CameraTypeMax };
static const UINT nAREA_CAM_SIZE_X[IDX_AREA_MAX] = {4000, 1280};
static const UINT nAREA_CAM_SIZE_Y[IDX_AREA_MAX] = {3000, 1024};

#endif

typedef struct _READ_DATA_Label
{
	CString		strCode;
	enum SegmentType { OTHER_INFO=0, TEXT_SEGMENT, BARCODE_SEGMENT, BARCODE_TEXT_SEGMENT  };
	SegmentType DataType;

	CString		strString;
	CPoint		cPosition;
	CSize		cSize;

	BOOL		bInspCheck;

	void clear()
	{
		strCode		= _T("");
		DataType	= OTHER_INFO;

		strString	= _T("");
		cPosition	= CPoint(0, 0);
		cSize		= CSize (0, 0);

		bInspCheck	= FALSE;
	}
}READ_DATA_Label;

// Basic Defect Codes ----------------------------------------------
#define ISRESULT_ERROR						(0x80000000)
#define ISRESULT_ERROR_NG					(0x80000001)
// 1G
#define ISRESULT_ERROR_TRAYOCR				(0x80000004)
#define ISRESULT_ERROR_TRAYCHIPCNT			(0x80000008)
#define ISRESULT_ERROR_TRAYCHIP				(0x80000010)
#define ISRESULT_ERROR_MIXING				(0x80000020)
#define ISRESULT_ERROR_LIFT					(0x80000040)
#define ISRESULT_ERROR_CHIPOCR				(0x80000080)
// 2G
#define ISRESULT_ERROR_BANDING				(0x80000100)
#define ISRESULT_ERROR_HIC					(0x80000200)
// 3G
#define ISRESULT_ERROR_DESICCANT1			(0x80000400)
#define ISRESULT_ERROR_DESICCANT2			(0x80000800)
#define ISRESULT_ERROR_MATERIAL				(0x80001000)
// 4G
#define ISRESULT_ERROR_MBB					(0x80002000)
#define ISRESULT_ERROR_RESERVE13			(0x80004000)
// 5G
#define ISRESULT_ERROR_BOX_QUALITY			(0x80008000)
#define ISRESULT_ERROR_SEALING_QUALITY 		(0x80010000)
// 6G
#define ISRESULT_ERROR_TAPE					(0x80020000)
#define ISRESULT_ERROR_RESERVE22			(0x80040000)
// Label
#define ISRESULT_ERROR_LABEL_SHIFT			(0x80080000)					   
#define ISRESULT_ERROR_LABEL_FAULTY			(0x80100000)
#define ISRESULT_ERROR_BARCODE_EMPTY		(0x80200000)
#define ISRESULT_ERROR_LOTID_MISMATCH		(0x80400000)
#define ISRESULT_ERROR_OCR_NG				(0x80800000)

#define RESULT_OK( dwErrCode ) ( ((dwErrCode & ISRESULT_ERROR) == 0) ? TRUE : FALSE )
#define RESULT_NG( dwErrCode ) ( !RESULT_OK(dwErrCode) )

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


