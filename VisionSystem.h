#pragma once

#include "Common/xSingletoneObject.h"
#include "APK.h"

#include <XUtil/xMmTimers.h>
#include <XUtil/xStopWatch.h>
#include <XImage/xImageObject.h>
#include <XGraphic/xDataTypes.h>
#include <XUtil/Comm/xClientSocketTCP.h>
#include <XUtil/Comm/xServerSocketTCP.h>

#include <vector>

#include "VisionInterfaces.h"
#include "VisionDevices.h"

#include "InspectProcess.h"
#include "InspectSaveProcess.h"
#include "InspectSaveMergeProcess.h"
#include "InspectionVision.h"

#include "ModelInfo.h"

#include "SystemConfig.h"

#include <Open_eVision_2_11.h>
#include "DecoderCortex.h"
#include "MariaDbClient.h"

#define WM_LOG_MESSAGE					(WM_USER+10)
#define WM_MODEL_CHANGE_STATUS			(WM_USER+11)
#define WM_UPDATE_MAP_RESET				(WM_USER+12)
#define WM_UPDATE_MAP_SETTING			(WM_USER+13)

#define WM_UPDATE_IMAGE_VIEW			(WM_USER+30)
#define WM_UPDATE_VIEWER_RESOLUTION		(WM_USER+31)

#define DEF_PORT_NONE	10000
#define DEF_FONT_RESULT_SIZE	16
#define DEF_FONT_RESULT_POSI	120

#define DEF_FONT_BASIC_SIZE		11
#define DEF_FONT_BASIC_POSI		100

#define GV8_MIN	0
#define GV8_MAX	255

#define CHIP_GRAB_CNT_MAX 32

using namespace Euresys::Open_eVision_2_11;

class CxGraphicObject;

namespace VisionProcess
{
	class CInspectProcess;
	class CInspectSaveProcess;
	class CInspectSaveMergeProcess;
	class CInspectionVision;
}

class CAPKView;
class CVisionSystem : public CxSingleton<CVisionSystem>, public IDeviceNotify
{
// 내부 기본
public:
	HWND					m_hWndUI;

	CVisionSystem(void);
	virtual~CVisionSystem(void);

	void Initialize( HWND hWndUI );
	void RecipeLoad();
	void JobNumberLoad();
	
	class IVisionStatusChanged
	{
	public:
		virtual void OnVisionStatusChanged() = 0;
		virtual void SendPacket(CString strPacket, int n) = 0;
		virtual BOOL Start(UINT nPort, int n) = 0;
		
	};

protected:
	// Run Status
	CxCriticalSection		m_csRunStatus;
	eRunStatus				m_RunStatus;

	CxImageObject	m_ImageObjectArea[IDX_AREA_MAX];
	CxImageObject	m_ImageObjectArea_3DChip[2];					// 3D Chip Cnt
	CxImageObject	m_ImageObjectAreaChipCnt[CHIP_GRAB_CNT_MAX];	// Chip cnt

	CxImageObject	m_ImageObjectTemp;

	CxGraphicObject*	m_pGOArea[IDX_AREA_MAX];

	eGrabStatus m_GrabStatus[IDX_AREA_MAX];

	BOOL bProgramStartInitialize;
	//////////////////////////////////////////////////////////////////////////
	friend class VisionInterface::FrameGrabber;
	friend class VisionInterface::LightController;
	
	VisionInterface::FrameGrabber			m_FrameGrabberInterface;
	VisionInterface::LightController		m_LightControllerInterface;

	VisionProcess::CInspectProcess*				m_pInspectProcess;				// 검사 Thread
	VisionProcess::CInspectSaveProcess*			m_pInspectSaveProcess;			// 저장 Thread
	VisionProcess::CInspectSaveMergeProcess*	m_pInspectSaveMergeProcess;		// 이미지 병합
	VisionProcess::CInspectionVision*			m_pInspectVision;				// 실질적인 검사

	// Camera
	CFrameGrabberSentechGigE		m_AreaScanCamera;		// Area Scan
	CFrameGrabberMechEyeGigE		m_3DAreaScanCamera;		// Area Scan
	CFrameGrabberPanSightGigE		m_3DLineScanCamera;		// Line Scan

	// Light - Inspection
	CLightControllerForAreaScan		m_LightControllerForArea;
	CLightControllerForAreaScan2	m_LightControllerForArea2;
	CLightControllerForAreaScan3	m_LightControllerForArea3;

	// Label
	std::vector<READ_DATA_LABEL>	m_stLabelData;
	CString strLabelData_Manual_Path;
	std::vector<READ_DATA_LABEL>	m_stLabelData_Manual;

	// Tray OCR
	VisionProcess::TrayOCRData		m_stTrayOCRData;
	VisionProcess::ChipOCRData		m_stChipOCRData;

	// Stacker OCR
	CString m_strTraySID;
	CString m_strTrayOcrData;

	// StopWatch - Tacttime
	CxStopWatch stopwatch[IDX_AREA_MAX];

	// Handler
	IVisionStatusChanged*	m_pNotifyVisionStatusChanged;

	// Measure
	BOOL m_bMeasureBusy;

	// Grab
	BOOL m_bAreaGrabFinish[IDX_AREA_MAX];
	BOOL m_bInspectionFinish[IDX_AREA_MAX];

	// Error
	DWORD m_dwErrorCode[IDX_AREA_MAX];

	CString m_strLotID[InspectTypeMax];
	int m_nBoxNo[InspectTypeMax];

	CMariaDbClient m_MariaDB;
//	struct SBubblewrapExportRow
//	{
//		CString bubble_wrap_sisd;
//		double  width_mm = 0.0;
//		double  length_mm = 0.0;
//		CString desciption;
//	};
//	BOOL LoadBubblewrapExport(CMariaDbClient& db, std::vector<SBubblewrapExportRow>& outRows);ㅣ

private:
	CAPKView* m_pMainView;

	void SetRunStatus(eRunStatus status);
	BOOL OnModelChange(int nRecipeNumber, eTeachTabIndex eTabIdx );

// 외부 참조 함수 모음
public:
//	CxMMTimers m_Timer;
//	static BOOL WINAPI _OnTimer(LPVOID lpParam);
//	BOOL OnTimer();

	const eRunStatus GetRunStatus();
	CxImageObject* GetImageObject(eCamType nCamType, int nIdx);
	CxImageObject* GetImageObject_3DChip(int nIdx);
	CxImageObject* GetImageObject_Chip(int nIdx);

	BOOL Run();
	BOOL Stop();

	void ResetAllResult( eCamType tpCam, int nViewIndex );

	void WriteMessage( eLogType type, LPCTSTR lpszFormat, ... );

	void Shutdown();

	// ImageObject 관련
	void SetGraphicObjects( eCamType nCamType, int nIdx, CxGraphicObject* pGO );
	CxGraphicObject* GetGraphicObject(int nIndex) { return m_pGOArea[nIndex]; }

	// Teaching & 검사 관련
	VisionProcess::CInspectionVision* GetInspectVisionModule() { return m_pInspectVision; }

	eGrabStatus GetCameraGrabStatus(CameraType nCamType) { return m_GrabStatus[nCamType]; }

	// 시리얼 장치 접속 해제 이벤트
	virtual void OnDisconnectSerialDevice( SerialDevice device, DWORD dwError) override;

	virtual void OnGrabFinishAreaScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) override;
	virtual void OnGrabFinish3DAreaScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) override;
	virtual void OnGrabFinish3DLineScanCamera(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) override;

	BOOL CreateImageGrabBuffers(eCamType nCamtype, int nWidth, int nHeight, int nViewIndex);
	BOOL CreateImageGrabBuffers_Line(int nWidth, int nHeight, int nViewIndex, BOOL bManual = TRUE);
	BOOL CreateImageGrabBuffers_3DChip(int nWidth, int nHeight);
	BOOL CreateImageGrabBuffers_Chip(int nWidth, int nHeight);
	BOOL ResetImageGrabBuffer_3DChip();
	BOOL ResetImageGrabBuffer_Chip();

	void SetTrayMapGridReset();
	void SetTrayMapGridIndex(int nCol, int nRow);

	VisionInterface::FrameGrabber& GetFrameGrabberInterface() { return m_FrameGrabberInterface; }
	VisionInterface::LightController& GetLightControllerInterface() { return m_LightControllerInterface; }

	BOOL StartInspection( InspectType inspecttype, int nViewIndex, int nGrabCnt, BOOL bManual = FALSE );
	void InspectionFinish( InspectType inspecttype, int nViewIndex );
	void InspectionResult( InspectType inspecttype, int nViewIndex );

	// Read Tray OCR
	BOOL OCRMeasureInspection(CxGraphicObject* pGO, CxImageObject* pImgObj, VisionProcess::TrayOCRData& stTrayData, BOOL bSave = FALSE);

	// Read Chip OCR
	BOOL OCRMeasureInspection(CxGraphicObject* pGO, CxImageObject* pImgObj, VisionProcess::ChipOCRData& stTrayData, BOOL bSave = FALSE);

	// Measure-Label(OCR)
	BOOL OCRMeasureInspection(EImageBW8 SegmentImage, VisionProcess::LabelOCRData& LabelData, std::vector<READ_DATA_LABEL> &MasterData, CString strFontPatch, VisionProcess::SegmentReadingOption stReadingOption);
	void SetOCRResult(VisionProcess::stOCRResult stOCRData) {	m_pInspectVision->m_LabelReadingData = stOCRData;	}

	void SaveTrayOcr();
	void SaveOriginImage( int nViewIndex, InspectType inspecttype, int nGrabCnt );
	void StartSaveImage( CxImageObject *pImgObj, InspectType inspecttype, int nViewIndex, int nGrabCnt );
	void StartSaveImageMerge(BOOL bManual);
	void StartManualLabelInspect(int nViewIndex, InspectType inspecttype);

//	void CameraOnOffArea( BOOL bOn );

	// Tray OCR
	VisionProcess::TrayOCRData& GetTrayOCRData() { return m_stTrayOCRData; }
	VisionProcess::ChipOCRData& GetChipOCRData() { return m_stChipOCRData; }
	void OCRImagePreview(CxImageObject* pImgObj);
	
	// Stacker OCR
	void OCRImagePreview_stacker(CxImageObject* pImgObj);
	CString GetTraySID() const { return m_strTraySID; }
	void SetTraySID(CString strRecipe) { m_strTraySID = strRecipe; }
	CString GetTrayOcrData() const { return m_strTrayOcrData; }
	void SetTrayOcrData(CString strRead) { m_strTrayOcrData = strRead; }
	

	BOOL Result_OK( int nViewIndex, InspectType inspecttype );
	BOOL ResultOKNG_MMI( InspectType inspecttype, BOOL bOK );
	BOOL SetBypassMode( InspectType inspecttype, eBypass eBypassMode );
	void WriteLogforTeaching( InspectType InspType, LPCTSTR lpszFormat, ... );

	DWORD GetErrorCode( int nViewIndex ) const { return m_dwErrorCode[nViewIndex]; }
	void SetErrorCode( int nVIewIndex, DWORD dwErrorCode )	{ m_dwErrorCode[nVIewIndex] |= dwErrorCode; }

	// Label
	std::vector<READ_DATA_LABEL>	GetLabelData()			{ return m_stLabelData;				}
	std::vector<READ_DATA_LABEL>	GetLabelData_Manual()	{ return m_stLabelData_Manual;		}
	CString							GetManualData_Path()	{ return strLabelData_Manual_Path;	}
	void LoadLabelData_Manual( BOOL bReset, CString strPath = _T("") );

	// Log
	BOOL CheckExistDrive(CString strDriveIndex);

	// eVision
	BOOL m_bIsValidEvisionDongle;
	BOOL TestEvisionDongle();
	BOOL GetValidEvisionDongle(){ return m_bIsValidEvisionDongle; }

	//// Code
#ifndef _CodeUsb
	CDecoderCortex m_Decoder;
#endif

	// ----- Tray Map -----
	void GetMetrixInfoCount( int nGrabIndex, int &nScanColCnt, int &nScanRowCnt, int &nInspCol, int &nInspRow, int &nResultMatrix_OffsetX, int &nResultMatrix_OffsetY );
	void GetMetrixInfoGrab( int &nGrabIndex, int &nImgCol, int &nImgRow, int &OffsetX, int &OffsetY, int nCol, int nRow );
	void SetChipImage(int nGrabIndex);

	// StopWatch
	void BeginStopWatch( int nViewIndex ) { stopwatch[nViewIndex].Begin(); }
	double GetStopWatch( int nViewIndex ) { return stopwatch[nViewIndex].GetElapsedTime() / 1000.0f; }

	// Lot, Box 정보
	CString GetLotID( InspectType inspecttype )						{ return m_strLotID[inspecttype]; }
	void SetLotID( InspectType inspecttype, CString strLotID )		{ m_strLotID[inspecttype] = strLotID; }

	short GetBoxNo( InspectType inspecttype )						{ return m_nBoxNo[inspecttype]; }
	void SetBoxNo( InspectType inspecttype, int sBoxNo )			{ m_nBoxNo[inspecttype] = sBoxNo; }

	int m_nImageW[IDX_AREA_MAX];
	int m_nImageH[IDX_AREA_MAX];
	int m_nImageWBytes[IDX_AREA_MAX];
	BYTE* m_pImageBuffer[IDX_AREA_MAX];
	
	// ACM
	BOOL m_bAcm[ACM_MAX];

	// Tray ocr ISS
	BOOL m_bISSOn;

	// Stacker Ocr Z축 100mm = 1, 120mm = 2
	int m_nStackerZAxis;

	// 3D Chip
	int m_n3DAreaShot;
	int m_nMMI3DChipCnt;			// 상위에서 받은 3D Chip 수량
	BOOL m_b3DChipCntResult;		// 검사 후 DEE에 보낼 최종 결과 값

	// Chip Count
	BOOL m_bChipCnt;				// ocr : FASLE,  cnt : TRUE
	BOOL m_bChipCntHandlerStart;	// 상위에서 받은 신호로 움직일 때만 수량 결과 그래픽 그리기 위해
	int m_nChipGrabCnt;				// Grab Count 체크
	int m_nMMIChipCnt;				// 상위에서 받은 Chip 수량
	int m_nChipInspCnt;				// 검사 한 Chip 수량
	BOOL m_bChipCntResult;			// 검사 후 DEE에 보낼 최종 결과 값

	// Tray Mixing				
	int m_nMMITrayQuantity_Mixing;	// 상위에서 받은 Tray 개수

	// Tray Lift
	int m_nMMITrayQuantity_Lift;	// 상위에서 받은 Tray 개수

	// Banding
	int m_nBandingDir;				// Banding Dir ( 0, 1 : 1차 단'장변, 2, 3 : 2차 단'장변 )
	int m_nBandingPos;				// Banding Pos ( 0 : Left, 1 : Right, 2 : Center )

	// HIC
	int m_nHICNgKind;				// 0 : HIC Disclor NG, 1 : HIC Circle finding error

	// Desiccant
	BOOL m_bDesiccantInspLeft;		// Position : FASLE,  Cutting : TRUE
	BOOL m_bDesiccantInspRight;		// Position : FASLE,  Cutting : TRUE
	BOOL m_bNgRetryLeft;			// Position 검사, TRUE : Retry, FALSE : Stop
	BOOL m_bNgRetryRight;			// Position 검사, TRUE : Retry, FALSE : Stop
	float m_fDesiPosDistLeft;		// Desiccant Position Distance 결과 보고 값
	float m_fDesiPosDistRight;		// Desiccant Position Distance 결과 보고 값
	int m_nMMITrayQuantity_SubMaterial;

	// Label
	BOOL m_bLabelROITeaching;		// Label ROI Teaching 확인

//	// Material Info
//	//			0 | 1	| 2 	| 3		| 4		| 5
//	// 1st : None | L+R | L+C+R	|		|		|
//	// 2nd : None | C 	| T+C	| T+C+B	|		|
//	// 3rd : None | L+R | C		|		|		|
//	// 4th : None | T	| T+C	| T+B	| C		| C+B
//	int m_nBandingCode;
//	
//	// Desiccant Insert
//	//		0 | 1					| 2 					| 3						| 4
//	//   None |	Before 1st Banding  | Before 2nd Banding	| Before 3nd Banding	| Before 4th Banding
//	int m_nDesiccantSeqCode;
//
//	// Desiccant Position
//	//		0 |	1		| 2 		| 3			| 4
//	//   None |	Left	| Center	| Right 	| L+R
//	int m_nDesiccantPosCode;
//
//	// HIC Position
//	//		0 |	1		| 2 		| 3			| 4			| 5			| 6			| 7			| 8			| 9
//	//   None |	LeftTop	| LeftCen	| LeftBtm 	| CenterTop	| CenterCen	| CenterBtm	| RightTop	| RightCen	| RightBtm
//	int m_nHICPosCode;
	//////////////////////////////////////////////////////////////////////////

	// 기타 시스템
	int TimeDelay( DWORD dwTimeout );

	//// MMI 통신 관련
	void OnPacketReceived(ClientThreadContext* pContext, CxIoBuffer* pPacketBuffer);
	void ProcessBuffer(ServerThreadContext* pContext, char* pReceiveBuf, int nRecvBytes);

	void SetSystemTime(CString strTimeSink);

	void CameraExposureChange(CameraType camType, double dExposure);
	void CameraOnOff(InspectType inspecttype, BOOL bOn);
	void LightOnOff(InspectType inspecttype, BOOL bOn, int nGrabCnt = 0);
	
	void DongleConnet();
	void CameraConnect();
	void LightCtrlConnect();
	void ServerOpen();
	void ServerClose();
	void MariaDBOpen();

	BOOL CheckCamera(CameraType  nCamType);
	BOOL CheckLedCtrl(LedCtrlType nCtrlType);
	BOOL CheckSystemCondition();

	BOOL ShowErrorDlg(CString strTitle, CString strMessage, CString strDetail);

	void SetPreviewGainOffset(int nViewIndex, BOOL bSet);

	// 3D Camera
	void SetDepthOffset(int nBase = 0);
	void SetDepthOffset_Size(int nWidth, int nHeight);
	UINT8 Rescale(float value, float max, float min);
	BOOL SaveDepthToBin(const CString& path);
	BOOL LoadDepthFromBin(const CString& path);

protected:
	// UI Update 관련 메시지 함수
	void PostUIMsgModelChangeStatus( BOOL bIsFinish, int nRecipeNumber, BOOL bModelChangeOK );
	void PostUIMsgUpdateImageView( BOOL bZoomFit, eCamType nCamtype, int nIndex=0 );

public:
	void PostUIMsgUpdateViewerResolution();
	

private:
	void DES201S(const CString& strReceiveBuf, const CString& strVisionCode);
	void DES201(const CString& strReceiveBuf, const CString& strVisionCode);
	void DES202(const CString& strReceiveBuf, const CString& strVisionCode);
	void DES203(const CString& strReceiveBuf, const CString& strVisionCode);
	void DES204(const CString& strReceiveBuf, const CString& strVisionCode);
	void DES205(const CString& strReceiveBuf, const CString& strVisionCode);
	void DES207(const CString& strReceiveBuf, const CString& strVisionCode);

	void VIS201S(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void VIS201(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void VIS202(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void VIS203(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void VIS204(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void VIS205(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void VIS207(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);

	void DEE201S(const CString& strReceiveBuf, const CString& strVisionCode);
	void DEE201(const CString& strReceiveBuf, const CString& strVisionCode);
	void DEE202(const CString& strReceiveBuf, const CString& strVisionCode);
	void DEE203(const CString& strReceiveBuf, const CString& strVisionCode);
	void DEE204(const CString& strReceiveBuf, const CString& strVisionCode);
	void DEE205(const CString& strReceiveBuf, const CString& strVisionCode);
	void DEE207(const CString& strReceiveBuf, const CString& strVisionCode);

	void ACM201(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void ACM203(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);
	void ACM204(const CString& strReceiveBuf, const CString& strVisionCode, VisionInterface::FrameGrabber& FrameGrabber);

	void RemoveBoxData(const CString& strReceiveBuf, int nIndex);
	void ReworkBoxData(const CString& strReceiveBuf, int nIndex);
	BOOL DeleteFolderRecursive(LPCTSTR folderPath);
	
public:
	void TestMerge();
};