#include "stdafx.h"
#include "CameraOldSDK.h"
CameraOldSDK::CameraOldSDK(CAMHANDLE hCamera)
{
    m_hCamera = hCamera;
    if(SGERR_OK!=InitCameraParam())
        return;
}

SGERROR_CODE CameraOldSDK::InitCameraParam()
{
    SGERROR_CODE ret = SGERR_OK;
    ret=SgGetRoiAndMediaSyn(m_hCamera,m_curRoi);
    if(ret != SGERR_OK)
        return ret;
    ret=SgGetCamModeSyn(m_hCamera,m_curMode);
    if(ret != SGERR_OK)
        return ret;
    ret=SgGetCamConfigSyn(m_hCamera,m_curCfg);
    if(ret != SGERR_OK)
        return ret;
    ret=SgGetCamProductInfosSyn(m_hCamera,m_curProctInfo);
    if(ret != SGERR_OK)
        return ret;
    return SGERR_OK;
}

USHORT16 CameraOldSDK::GetMaxFrame()
{
    UINT32 maxFrame;
    SgGetMaxFrame(m_hCamera,maxFrame);
	if (m_curCfg._usFrame > maxFrame)
	{
		m_curCfg._usFrame = maxFrame;
	}
    return maxFrame;
}

SGERROR_CODE CameraOldSDK::SetCaptureMode(BYTE8 ucCaptureMode)
{
    SGEXPORT_MODE mode = m_curMode;
    mode._ucCaptureMode = ucCaptureMode;
    SGERROR_CODE ret = SgSetCamModeSyn(m_hCamera,mode);
    if(ret == SGERR_OK)
    {
        m_curMode = mode;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetUserOperatorMode(BYTE8 ucUserOperatorMode)
{
    SGEXPORT_MODE mode = m_curMode;
    mode._ucUserOperatorMode = ucUserOperatorMode;
    SGERROR_CODE ret = SgSetCamModeSyn(m_hCamera,mode);
    if(ret == SGERR_OK)
    {
        m_curMode = mode;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetDataType(BYTE8 ucDataType)
{
    SGEXPORT_MODE mode = m_curMode;
    mode._ucDataType = ucDataType;
    SGERROR_CODE ret = SgSetCamModeSyn(m_hCamera,mode);
    if(ret == SGERR_OK)
    {
        m_curMode = mode;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetTransMode(BYTE8 ucTransMode)
{
    SGEXPORT_MODE mode = m_curMode;
    mode._ucTransMode = ucTransMode;
    SGERROR_CODE ret = SgSetCamModeSyn(m_hCamera,mode);
    if(ret == SGERR_OK)
    {
        m_curMode = mode;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetBatchGrabNumber(UINT32 uiBatchGrabNumber)
{
    SGEXPORT_MODE mode = m_curMode;
    mode._uiGrabNumber = uiBatchGrabNumber;
    SGERROR_CODE ret = SgSetCamModeSyn(m_hCamera,mode);
    if(ret == SGERR_OK)
    {
        m_curMode = mode;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetFrame(USHORT16 usFrame)
{
    SGEXPORT_CAMCONFIG Cfg = m_curCfg;
    Cfg._usFrame = usFrame;
    SGERROR_CODE ret = SgSetCamConfig(m_hCamera,Cfg);
    if(ret == SGERR_OK)
    {
        m_curCfg = Cfg;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetExpo(UINT32 uiExpo)
{
    SGEXPORT_CAMCONFIG Cfg = m_curCfg;
    Cfg._uiExpo = uiExpo;
    SGERROR_CODE ret = SgSetCamConfig(m_hCamera,Cfg);
    if(ret == SGERR_OK)
    {
        m_curCfg = Cfg;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetYScaling(float fYScaling)
{
    SGEXPORT_CAMCONFIG Cfg = m_curCfg;
    Cfg._fYScaling = fYScaling;
    SGERROR_CODE ret = SgSetCamConfig(m_hCamera,Cfg);
    if(ret == SGERR_OK)
    {
        m_curCfg = Cfg;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetEncFilter(BYTE8 ucEncFilter)
{
    SGEXPORT_CAMCONFIG Cfg = m_curCfg;
    Cfg._encParam._ucEncFilter = ucEncFilter;
    SGERROR_CODE ret = SgSetCamConfig(m_hCamera,Cfg);
    if(ret == SGERR_OK)
    {
        m_curCfg = Cfg;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetEncPulseNumber(UINT32 uiEncPulseNumber)
{
    SGEXPORT_CAMCONFIG Cfg = m_curCfg;
    Cfg._encParam._uiPulseNumber = uiEncPulseNumber;
    SGERROR_CODE ret = SgSetCamConfig(m_hCamera,Cfg);
    if(ret == SGERR_OK)
    {
        m_curCfg = Cfg;
    }
    return ret;
}

SGERROR_CODE CameraOldSDK::SetIsLowPrecisionEncoder(BYTE8 isLowPrecisionEncoder)
{
    SGEXPORT_CAMCONFIG Cfg = m_curCfg;
    Cfg._encParam._isLowPrecisionEncoder = isLowPrecisionEncoder;
    SGERROR_CODE ret = SgSetCamConfig(m_hCamera,Cfg);
    if(ret == SGERR_OK)
    {
        m_curCfg = Cfg;
    }
    return ret;
}

