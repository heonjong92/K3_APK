#include "stdafx.h"
#include "CameraWrapImpl.h"
#include "CameraNewSDK.h"
#include "CameraOldSDK.h"
CameraWrapImpl::CameraWrapImpl()
{
    m_camera = nullptr;
    m_bConnect = false;
    m_bStartGrab = false;
    m_iCameraHandle = SgCreateCamera();
}

CameraWrapImpl::~CameraWrapImpl()
{
	if (m_bConnect)
	{
		CloseCamera();
	}

	if (m_camera != nullptr)
	{
		delete m_camera;
		m_camera = nullptr;
    }
}

bool CameraWrapImpl::LibInit()
{
    return SgLibInit() == SGERR_OK;
}

bool CameraWrapImpl::LibUnInit()
{
    return SgLibUnInit() == SGERR_OK;
}

bool CameraWrapImpl::RegCallback(CBTYPE iCallbackType, void *pCallback, void *pOwner)
{
    return SgRegCallback(m_iCameraHandle,iCallbackType,pCallback,pOwner) == SGERR_OK;
}

const char *CameraWrapImpl::DetectCamera()
{
    return SgDetectNetCameras();
}

bool CameraWrapImpl::Connect2Camera(const char *pLocalIp, const char *pRemoteIp)
{
    SGERROR_CODE ret = SgConnect2CameraSyn(m_iCameraHandle,pLocalIp,pRemoteIp);
    if(ret == SGERR_OK)
    {
        if(m_camera!=nullptr)
        {
            delete m_camera;
            m_camera =nullptr;
        }
        if(IsSupportParamID()) //连接完成后通过该接口判断是新版相机还是旧版相机，实例化相应对象，旧版需要初始化获取相机参数
			//After the connection is completed, use this interface to determine whether it is a new or old version camera, instantiate the corresponding object, and the old version needs to initialize to obtain camera parameters
        {
            m_camera = new CameraNewSDK(m_iCameraHandle);
        }
        else
        {
            m_camera = new CameraOldSDK(m_iCameraHandle);
        }
        m_bConnect = true;
    }
    else
    {
        m_bConnect = false;
        m_bStartGrab = false;
    }

    return ret == SGERR_OK;
}

bool CameraWrapImpl::CloseCamera()
{
    SGERROR_CODE ret =  SgCloseCamera(m_iCameraHandle);
    if(ret == SGERR_OK )
    {
        m_bConnect = false;
        m_bStartGrab = false;

        if (m_camera != nullptr)
        {
            delete m_camera;
            m_camera = nullptr;
        }
    }

    return ret == SGERR_OK;
}

bool CameraWrapImpl::IsSupportParamID()
{
    return SgIsSupportParamId(m_iCameraHandle);
}

bool CameraWrapImpl::GetCameraIsConnect()
{
    return SgIsConnected2Camera(m_iCameraHandle);
}

bool CameraWrapImpl::GetCameraIsGrab()
{
    return m_bStartGrab;
}

USHORT16 CameraWrapImpl::GetMaxFram()
{
    return m_camera ? m_camera->GetMaxFrame() : 0;
}

BYTE8 CameraWrapImpl::GetCaptureMode()
{
    return m_camera ? m_camera->GetCaptureMode() : 0;
}

BYTE8 CameraWrapImpl::GetUserOperatorMode()
{
    return m_camera ? m_camera->GetUserOperatorMode() : 0;
}

BYTE8 CameraWrapImpl::GetDataType()
{
    return m_camera ? m_camera->GetDataType() : 0;
}

BYTE8 CameraWrapImpl::GetTransMode()
{
    return m_camera ? m_camera->GetTransMode() : 0;
}

UINT32 CameraWrapImpl::GetBatchGrabNumber()
{
    return m_camera ? m_camera->GetBatchGrabNumber() : 0;
}

USHORT16 CameraWrapImpl::GetFrame()
{
    return m_camera ? m_camera->GetFrame() : 0;
}

UINT32 CameraWrapImpl::GetExpo()
{
    return m_camera ? m_camera->GetExpo() : 0;
}

int CameraWrapImpl::GetPointCloudCallBackNum()
{
    int iNum = 0;
    if (SgGetPointCloudCallBackNum(m_iCameraHandle, iNum) != SGERR_OK)
        return 0;

    return iNum;
}

const char * CameraWrapImpl::GetEmbedVer()
{
	return m_camera ? m_camera->GetEmbedVer() : "";
}

bool CameraWrapImpl::SetCaptureMode(BYTE8 ucCaptureMode)
{
    return m_camera && m_camera->SetCaptureMode(ucCaptureMode) == SGERR_OK;
}

bool CameraWrapImpl::SetUserOperatorMode(BYTE8 ucUserOperatorMode)
{
    return m_camera && m_camera->SetUserOperatorMode(ucUserOperatorMode) == SGERR_OK;
}

bool CameraWrapImpl::SetDataType(BYTE8 ucDataType)
{
    return m_camera && m_camera->SetDataType(ucDataType) == SGERR_OK;
}

bool CameraWrapImpl::SetTransMode(BYTE8 ucTransMode)
{
    return m_camera && m_camera->SetTransMode(ucTransMode) == SGERR_OK;
}

bool CameraWrapImpl::SetBatchGrabNumber(UINT32 uiBatchGrabNumber)
{
    return m_camera && m_camera->SetBatchGrabNumber(uiBatchGrabNumber) == SGERR_OK;
}

bool CameraWrapImpl::SetFrame(USHORT16 usFrame)
{
    return m_camera && m_camera->SetFrame(usFrame) == SGERR_OK;
}

bool CameraWrapImpl::SetExpo(UINT32 uiExpo)
{
    return m_camera && m_camera->SetExpo(uiExpo) == SGERR_OK;
}

bool CameraWrapImpl::SetPointCloudCallBackNum(int iNum)
{
    return SgSetPointCloudCallBackNum(m_iCameraHandle,iNum) == SGERR_OK;
}

bool CameraWrapImpl::SetYScaling(float fYScaling)
{
    return m_camera && m_camera->SetYScaling(fYScaling) == SGERR_OK;
}

bool CameraWrapImpl::SetEncFilter(BYTE8 ucEncFilter)
{
    return m_camera && m_camera->SetEncFilter(ucEncFilter) == SGERR_OK;
}

bool CameraWrapImpl::SetEncPulseNumber(UINT32 uiEncPulseNumber)
{
    return m_camera && m_camera->SetEncPulseNumber(uiEncPulseNumber) == SGERR_OK;
}

bool CameraWrapImpl::SetIsLowPrecisionEncoder(BYTE8 isLowPrecisionEncoder)
{
    return m_camera && m_camera->SetIsLowPrecisionEncoder(isLowPrecisionEncoder) == SGERR_OK;
}

bool CameraWrapImpl::GetEncoderCount(UINT32 &uiEncoderCount)
{
    return SgGetEncoderCountSyn(m_iCameraHandle,uiEncoderCount,false) == SGERR_OK;
}

bool CameraWrapImpl::EncoderParamsbySpeed(double dSpeed, int iFrame, double dSinglePulseLength, double dScanLength, int iSubNums, unsigned char &ucEncFilter, unsigned int &uiPulseInterval, unsigned char &ucIsLowPrecisionEncoder, double &dYScale, unsigned int &uiGrabNumber)
{
    return ::GetEncoderParamsbySpeed(dSpeed,iFrame,dSinglePulseLength,dScanLength,iSubNums,ucEncFilter,uiPulseInterval,ucIsLowPrecisionEncoder,dYScale,uiGrabNumber) == SGERR_OK;
}

bool CameraWrapImpl::EncoderParamsbyPulseInterval(unsigned int uiPulseInterval, int iFrame, double dSinglePulseLength, double dScanLength, int iSubNums, unsigned char &ucEncFilter, double &dSpeed, unsigned char &ucIsLowPrecisionEncoder, double &dYScale, unsigned int &uiGrabNumber)
{
    return ::GetEncoderParamsbyPulseInterval(uiPulseInterval,iFrame,dSinglePulseLength,dScanLength,iSubNums,ucEncFilter,dSpeed,ucIsLowPrecisionEncoder,dYScale, uiGrabNumber) == SGERR_OK;
}

bool CameraWrapImpl::EncoderParamsbyYScale(double dYScale, int iFrame, double dSinglePulseLength, double dScanLength, int iSubNums, unsigned char &ucEncFilter, double &dSpeed, unsigned char &ucIsLowPrecisionEncoder, unsigned int &uiPulseInterval, unsigned int &uiGrabNumber)
{
    return ::GetEncoderParamsbyYScale(dYScale,iFrame,dSinglePulseLength,dScanLength,iSubNums,ucEncFilter,dSpeed,ucIsLowPrecisionEncoder,uiPulseInterval,uiGrabNumber) == SGERR_OK;
}

bool CameraWrapImpl::StartCapture()
{
//	SGERROR_CODE ret = SgStartCapture(m_iCameraHandle);
	SGERROR_CODE ret = SgStartCaptureSyn(m_iCameraHandle);
    if(ret==SGERR_OK)
    {
        m_bStartGrab = true;
    }
    return ret == SGERR_OK;
}

bool CameraWrapImpl::StopCapture()
{
//	SGERROR_CODE ret = SgStopCapture(m_iCameraHandle);
	SGERROR_CODE ret = SgStopCaptureSyn(m_iCameraHandle);
    if(ret==SGERR_OK)
    {
        m_bStartGrab = false;
    }
    return ret == SGERR_OK;
}

bool CameraWrapImpl::SendGrabSignalToCamera(bool bGrabEnd)
{
    return SgSendGrabSignalToCameraExSyn(m_iCameraHandle,bGrabEnd) == SGERR_OK;
}


bool CameraWrapImpl::SlantCalibrate(int iRegionNum, float fBegin1, float fEnd1, float fBegin2, float fEnd2, float &fAngle)
{
    //标定需要切换到数据抓取模式
	//Calibration requires switching to data capture mode
    SGERROR_CODE ret = SGERR_FAILED;
    ret = SgSlantCalibrate(m_iCameraHandle,iRegionNum,fBegin1,fEnd1, fBegin2, fEnd2, fAngle);
    return ret == SGERR_OK;
}

bool CameraWrapImpl::GetSlantCalibrationInfo(bool &bUseSlantCal, int &iRegionNum, float &fBegin1, float &fEnd1, float &fBegin2, float &fEnd2)
{
    return SgGetSlantCalibrationInfo(m_iCameraHandle,bUseSlantCal,iRegionNum,fBegin1,fEnd1,fBegin2,fEnd2) == SGERR_OK;
}

bool CameraWrapImpl::SetSlantCalibrationInfo(bool bUseSlantCal)
{
    return SgSetSlantCalibrationInfo(m_iCameraHandle,bUseSlantCal) == SGERR_OK;
}

bool CameraWrapImpl::ReleaseImageBuffer(void *pBuf)
{
    return SgReleaseImageBuffer(m_iCameraHandle,pBuf) == SGERR_OK;
}
