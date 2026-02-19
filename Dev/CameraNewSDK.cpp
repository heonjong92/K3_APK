#include "stdafx.h"
#include "CameraNewSDK.h"

// SgGetParamsDefXmlSyn使用范例，内容范文解释。
//Example of using SgGetParamsDefXmlSyn, content template explanation.
#if 0
获取xml字符串	Get XML string
        const char* xmlStr = SgGetParamsDefXmlSyn(m_hCamera);
xml文件格式如下	 The XML file format is as follows
<root verInfo="sG57N_2024_03_13">
    <trigger_group>
        <BatchGrabNumber id="5" min="5" max="65532" desc="抓取线数"></BatchGrabNumber>
        <ProfileExtractNumber id="6" min="1" max="100" desc="轮廓模式采用间隔"></ProfileExtractNumber>
        <Frame id="7" desc="帧率">
        <item desc="100Hz">100</item>
        <item desc="200Hz">200</item>
        <item desc="500Hz">500</item>
        <item desc="1000Hz">1000</item>
        <item desc="1500Hz">1500</item>
        <item desc="2000Hz">2000</item>
        <item desc="2500Hz">2500</item>
        <item desc="3000Hz">3000</item>
        <item desc="4000Hz">4000</item>
        <item desc="5000Hz">5000</item>
        <item desc="6000Hz">6000</item>
        <item desc="8000Hz">8000</item>
        <item desc="10000Hz">10000</item>
        <item desc="12000Hz">12000</item>
        <item desc="15000Hz">15000</item>
        <item desc="20000Hz">20000</item>
        <item desc="24000Hz">24000</item>
        </Frame>
        <MaxFrame id="8" readonly="true" desc="帧率最大值"></MaxFrame>
    </trigger_group>
    <shoot_group>
        <Hdr id="28" desc="感光灵敏度">
        <item desc="高精度1">0</item>
        <item desc="高精度2">1</item>
        <item desc="高精度3">2</item>
        </Hdr>
        <Threshold id="29" min="0" max="255" increment="1" desc="灰度阈值"></Threshold>
        <Expo id="30" desc="曝光">
        <item desc="30us">30</item>
        <item desc="60us">60</item>
        <item desc="120us">120</item>
        <item desc="180us">180</item>
        <item desc="240us">240</item>
        <item desc="320us">320</item>
        <item desc="480us">480</item>
        <item desc="640us">640</item>
        <item desc="960us">960</item>
        </Expo>
    </shoot_group>
</root>
        //设计一个结构体，存放解析的信息	Design a structure to store parsed information
        Struct ParaInfo
        {
            string ID;   PARAMID
            string Desc; 节点名称	Node Name
            string Max; spinBox数据最大值	SpinBox data maximum value
            string Min; spinBox数据最小值	SpinBox data minimum value
            string ReadOnly; 数据只读	Data Read Only
            string Increment; spinBox单次调整增加数值	SpinBox single adjustment increases value
            List<ItmeInfo> itmeInfos;   combox的显示文字和对应数据值	Display text and corresponding data values for combo
        }
        Struct ItmeInfo
        {
            string Desc;
            string Value;
        }
        //遍历解析根节点下trigger_group的子节点，伪代码如下
		//Traversing and parsing the child nodes of trigger_group under the root node, the pseudocode is as follows
        List<ParaInfo> paraList 
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(xmlStr);
        XmlElement root = doc.DocumentElement;
        XmlNodeList listNodes = root.FindeNode("trigger_group").ChildNodes;
        foreach (XmlNode node : listNodes)
        {
            ParaInfo paraInfo;
            foreach (XmlAttribute attr : node.Attributes)
            {
                if (attr.Name == "id")
                {
                    paraInfo.ID = node.Attributes["id"].Value;
                }
                else if (attr.Name == "desc")
                {
                    paraInfo.Desc = node.Attributes["desc"].Value;
                }
                else if (attr.Name == "max")
                {
                    paraInfo.Max = node.Attributes["max"].Value;
                }
                else if (attr.Name == "min")
                {
                    paraInfo.Min = node.Attributes["min"].Value;
                }
                else if (attr.Name == "readonly")
                {
                    paraInfo.ReadOnly = node.Attributes["readonly"].Value;
                }
                else if (attr.Name == "increment")
                {
                    paraInfo.Increment = node.Attributes["increment"].Value;
                }
            }
            paraInfo.itmeInfos = new List<ItmeInfo>();
            foreach (XmlNode nod in node.ChildNodes)
            {
                ItmeInfo itmeInfo = new ItmeInfo();
                if (nod.Attributes!=null)
                {
                    foreach (XmlAttribute attr : nod.Attributes)
                    {
                        if (attr.Name == "desc")
                        {
                            itmeInfo.Desc = nod.Attributes["desc"].Value;
                        }
                    }
                }
                itmeInfo.Value = nod.InnerText;
                paraInfo.itmeInfos.Add(itmeInfo);
            }
            paraList.Add(paraInfo);
#endif
//读取多个参数示例，触发模式、操作模式、采集数据类型
//Read multiple parameter examples, including trigger mode, operation mode, and data collection type
//    PARAMID ParamIds[3] = {PID_ucCaptureMode,
//                     PID_ucUserOperatorMode,
//                     PID_ucDataType};
//    CAM_PARAM CamDatas[3]; //初始化获取的相机参数的数量	Number of camera parameters obtained through initialization
//	  int iCount = 0;
//    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ParamIds,3,CamDatas,iCount);
//    if(ret == SGERR_OK)
//    {
//        //解析CamDatas获取相应的参数	Parsing CamData to Obtain Corresponding Parameters
//        for(int i=0;i<3;i++)
//        {
//            if(CamDatas[i]._ucIsSupport==0)
//                return;
//            switch (CamDatas[i]._usParamId) {
//            case PID_ucCaptureMode:
//                m_ucCaptureMode = CamDatas[i]._curValue._ucValue;
//                break;
//            case PID_ucUserOperatorMode:
//                m_ucUserOperatorMode = CamDatas[i]._curValue._ucValue;
//                break;
//            case PID_ucDataType:
//                m_ucDataType = CamDatas[i]._curValue._ucValue;
//                break;
//            }
//        }
//    }
//写入多个参数示例 触发模式、操作模式、采集数据类型
//Write multiple parameter examples such as trigger mode, operation mode, and data collection type
//    CAM_PARAM datas[3];
//    SET_PARAM_RSP msgs[3] ;
//    datas[0]._usParamId = PID_ucCaptureMode;        //写入参数的ID	Write the ID of the parameter


//    datas[0]._curValue._ucValue = ucCaptureMode;    //写入参数的值	Write the value of the parameter
//    datas[0]._ucValueType = VT_BYTE8;               //写入参数的数据类型	The data type for writing parameters
//
//    datas[1]._usParamId = PID_ucUserOperatorMode;        //写入参数的ID	Write the ID of the parameter
//    datas[1]._curValue._ucValue = ucUserOperatorMode;    //写入参数的值	Write the value of the parameter
//    datas[1]._ucValueType = VT_BYTE8;                    //写入参数的数据类型	The data type for writing parameters
//
//    datas[2]._usParamId = PID_ucDataType;        //写入参数的ID	Write the ID of the parameter
//    datas[2]._curValue._ucValue = ucDataType;    //写入参数的值	Write the value of the parameter
//    datas[2]._ucValueType = VT_BYTE8;            //写入参数的数据类型	The data type for writing parameters
//    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,data,3,msg,iCount);
//    if(ret == SGERR_OK)
//    {

//    }
//    else if(ret == SGERR_CAM_RETURN_ERROR)
//    {
        //多个参数设置返回失败时，可以解析SET_PARAM_RSP信息		
        //根据_usParamId判断是哪个参数设置错误，_sResultCode在SGERROR_CODE查看错误码类型
		//When multiple parameter settings fail to return, SET-PARAM-RSP information can be parsed
		//Determine which parameter setting is incorrect based on the _usParamId, and check the error code type in SGERROR-CODE for the _sResultCode
//        std::cout<<"ParamId"<<msgs[0]._usParamId<<"ErrorCode:"<<msgs[0]._sResultCode<<std::endl;
//        std::cout<<"ParamId"<<msgs[1]._usParamId<<"ErrorCode:"<<msgs[1]._sResultCode<<std::endl;
//        std::cout<<"ParamId"<<msgs[2]._usParamId<<"ErrorCode:"<<msgs[2]._sResultCode<<std::endl;
//    }
CameraNewSDK::CameraNewSDK(CAMHANDLE hCamera)
{
    m_hCamera = hCamera;
}

BYTE8 CameraNewSDK::GetCaptureMode()
{
    PARAMID ids[1] = {PID_ucCaptureMode}; 
    CAM_PARAM datas[1];                   
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_ucCaptureMode = datas->_curValue._ucValue;
    }
    return m_ucCaptureMode;
}

BYTE8 CameraNewSDK::GetUserOperatorMode()
{
    PARAMID ids[1] = {PID_ucUserOperatorMode};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_ucUserOperatorMode = datas->_curValue._ucValue;
    }
    return m_ucUserOperatorMode;
}

BYTE8 CameraNewSDK::GetDataType()
{
    PARAMID ids[1] = {PID_ucDataType};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_ucDataType = datas->_curValue._ucValue;
    }
    return m_ucDataType;
}

BYTE8 CameraNewSDK::GetTransMode()
{
    PARAMID ids[1] = {PID_ucTransMode};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_ucTransMode = datas->_curValue._ucValue;
    }
    return m_ucTransMode;
}

UINT32 CameraNewSDK::GetBatchGrabNumber()
{
    PARAMID ids[1] = {PID_uiBatchGrabNumber};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_uiBatchGrabNumber = datas->_curValue._uiValue;
    }
    return m_uiBatchGrabNumber;
}

USHORT16 CameraNewSDK::GetFrame()
{
    PARAMID ids[1] = {PID_usFrame};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_usFrame = datas->_curValue._usValue;
    }
    return m_usFrame;
}

USHORT16 CameraNewSDK::GetMaxFrame()
{
    //获取最大帧率，也可以使用旧版接口 SgGetMaxFrame(m_hCamera,maxFrame);
	//Obtain the maximum frame rate, or use the old interface SgGetMaxFrame (m_hCamera, maxFrame);
    PARAMID ids[1] = {PID_usMaxFrame};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_usMaxFrame = datas->_curValue._usValue;
    }
    return m_usMaxFrame;
}

UINT32 CameraNewSDK::GetExpo()
{
    PARAMID ids[1] = {PID_uiExpo};
    CAM_PARAM datas[1];
    int iGetCount = 1;
    SGERROR_CODE ret = SgGetCamParamsSyn(m_hCamera,ids,1,datas,iGetCount);
    if(ret == SGERR_OK)
    {
        m_uiExpo = datas->_curValue._uiValue;
    }
    return m_uiExpo;
}

const char * CameraNewSDK::GetEmbedVer()
{
	return nullptr;
}

SGERROR_CODE CameraNewSDK::SetCaptureMode(BYTE8 ucCaptureMode)
{
    CAM_PARAM data;
    SET_PARAM_RSP msg ;
    data._usParamId = PID_ucCaptureMode;
    data._curValue._ucValue = ucCaptureMode;
    data._ucValueType = VT_BYTE8;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_ucCaptureMode = ucCaptureMode;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        //多个参数设置返回失败时，可以解析SET_PARAM_RSP信息
        //根据_usParamId判断是哪个参数设置错误，_sResultCode在SGERROR_CODE查看错误码类型
		//When multiple parameter settings fail to return, SET-PARAM-RSP information can be parsed
		//Determine which parameter setting is incorrect based on the _usParamId, and check the error code type in SGERROR-CODE for the _sResultCode
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetUserOperatorMode(BYTE8 ucUserOperatorMode)
{
    CAM_PARAM data;
    data._usParamId = PID_ucUserOperatorMode;
    data._curValue._ucValue = ucUserOperatorMode;
    data._ucValueType = VT_BYTE8;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_ucUserOperatorMode = ucUserOperatorMode;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetDataType(BYTE8 ucDataType)
{
    CAM_PARAM data;
    data._usParamId = PID_ucDataType;
    data._curValue._ucValue = ucDataType;
    data._ucValueType = VT_BYTE8;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_ucDataType = ucDataType;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetTransMode(BYTE8 ucTransMode)
{
    CAM_PARAM data;
    data._usParamId = PID_ucTransMode;
    data._curValue._ucValue = ucTransMode;
    data._ucValueType = VT_BYTE8;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_ucTransMode = ucTransMode;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetBatchGrabNumber(UINT32 uiBatchGrabNumber)
{
    CAM_PARAM data;
    data._usParamId = PID_uiBatchGrabNumber;
    data._curValue._uiValue = uiBatchGrabNumber;
    data._ucValueType = VT_INT32;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_uiBatchGrabNumber = uiBatchGrabNumber;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetFrame(USHORT16 usFrame)
{
    //帧率设置 不可以超过最大帧率，设置曝光会影响帧率
	//The frame rate setting cannot exceed the maximum frame rate, as setting the exposure will affect the frame rate
    CAM_PARAM data;
    data._usParamId = PID_usFrame;
    data._curValue._usValue = usFrame;
    data._ucValueType = VT_SHORT16;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_usFrame = usFrame;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetExpo(UINT32 uiExpo)
{
    //曝光设置 新款相机只能设定固定的几个档位值，需要从xml文件中解析
    //参见文件顶部 SgGetParamsDefXmlSyn 的使用范例
	//The new camera can only set fixed gear values for exposure settings, which need to be parsed from an XML file
	//See the example of using SgGetParamsDefXmlSyn at the top of the file
    CAM_PARAM data;
    data._usParamId = PID_uiExpo;
    data._curValue._uiValue = uiExpo;
    data._ucValueType = VT_INT32;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    if(ret == SGERR_OK)
    {
        m_uiExpo = uiExpo;
    }
    else if(ret == SGERR_CAM_RETURN_ERROR)
    {
        std::cout<<"ParamId"<<msg._usParamId<<"ErrorCode:"<<msg._sResultCode<<std::endl;
    }
    return ret;
}

SGERROR_CODE CameraNewSDK::SetYScaling(float fYScaling)
{
    CAM_PARAM data;
    data._usParamId = PID_fYScaling;
    data._curValue._ufValue = fYScaling;
    data._ucValueType = VT_FLOAT;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    return ret;
}

SGERROR_CODE CameraNewSDK::SetEncFilter(BYTE8 ucEncFilter)
{
    CAM_PARAM data;
    data._usParamId = PID_ucEncFilter;
    data._curValue._ucValue = ucEncFilter;
    data._ucValueType = VT_BYTE8;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    return ret;
}

SGERROR_CODE CameraNewSDK::SetEncPulseNumber(UINT32 uiEncPulseNumber)
{
    CAM_PARAM data;
    data._usParamId = PID_uiEncPulseNumber;
    data._curValue._uiValue = uiEncPulseNumber;
    data._ucValueType = VT_INT32;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    return ret;
}

SGERROR_CODE CameraNewSDK::SetIsLowPrecisionEncoder(BYTE8 isLowPrecisionEncoder)
{
    CAM_PARAM data;
    data._usParamId = PID_ucIsLowPrecisionEncoder;
    data._curValue._ucValue = isLowPrecisionEncoder;
    data._ucValueType = VT_BYTE8;
    SET_PARAM_RSP msg;
    int dataCount = 1;
    int FailedCount = 1;
    SGERROR_CODE ret = SgSetCamParamsSyn(m_hCamera,&data,dataCount,&msg,FailedCount);
    return ret;
}
