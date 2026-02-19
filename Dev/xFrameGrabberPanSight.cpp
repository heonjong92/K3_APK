#include "stdafx.h"

#include "xFrameGrabberPanSight.h"
#include "../SystemConfig.h"
#include "../ModelInfo.h"

#include <opencv2/opencv.hpp>

using namespace VisionDevice;

CxFrameGrabberPanSight::CxFrameGrabberPanSight()
{
	for (int i = 0; i < CameraTypeMax; ++i) 
	{
		m_bOpen[i] = FALSE;
		m_bLive[i] = FALSE;

		m_nSizeX[i] = 0;
		m_nSizeY[i] = 0;

		m_pCameraWrapImpl[i] = new CameraWrapImpl();
	}
}

CxFrameGrabberPanSight::~CxFrameGrabberPanSight()
{
	for (int i = 0; i < CameraTypeMax; ++i)
	{
		Close(i);
		delete m_pCameraWrapImpl[i];
	}
}

BOOL CxFrameGrabberPanSight::FindCameraIndex(std::vector<string> vec, UINT nIndex, OUT UINT &nViewIndex)
{
	CSystemConfig::CameraSerialNo& SerialNoConfig = CSystemConfig::Instance()->GetCameraSerialNo();
	std::vector<CString> vec2;

	for (int i = 0; i < vec.size(); ++i)
		vec2.push_back((CString)vec[i].c_str());

#if defined(RELEASE_1G) || defined(RELEASE_SG)
	int serialIndex = -1;
	switch (nIndex)
	{
	case CameraTypeLine:		serialIndex = 0; 		break;
	
#ifdef RELEASE_1G
	case CameraTypeLine2:		serialIndex = 3; 		break;
	case CameraTypeLine3:		serialIndex = 4; 		break;
	case CameraTypeLine4:		serialIndex = 5; 		break;
#endif
	default:
		return FALSE;
	}

	for (int i = 0; i < vec2.size(); ++i)
	{
		if (SerialNoConfig.strSerialNo[serialIndex] == vec2[i])
		{
			nViewIndex = serialIndex;
			return TRUE;
		}
	}
#endif

	return FALSE;
}

BOOL CxFrameGrabberPanSight::Open(UINT nIndex, OUT UINT& nViewIndex)
{
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	string IP = CameraWrapImpl::DetectCamera();

	if (IP == "")
		return FALSE;

	std::vector<string> vecLocalIPs, vecCameraIPs;

	CommonMethods::AnalysisIpInfos(IP.c_str(), vecLocalIPs, vecCameraIPs);
	try
	{
		if ( vecLocalIPs.empty() || vecCameraIPs.empty() )
			return FALSE;

		if (FindCameraIndex(vecCameraIPs, nIndex, nViewIndex))
		{
			CSystemConfig::CameraSerialNo& SerialNoConfig = CSystemConfig::Instance()->GetCameraSerialNo();

			SetCameraIndex(nViewIndex, nIndex);

			string cameraIP(CT2A(SerialNoConfig.strSerialNo[nViewIndex].GetString()));

			for (int i = 0; i < vecLocalIPs.size(); ++i) 
			{
				if (m_pCameraWrapImpl[nViewIndex]->Connect2Camera(vecLocalIPs[i].c_str(), cameraIP.c_str()))
					break;
				if (i == vecLocalIPs.size() - 1)
					return FALSE;
			}

			switch (nViewIndex)
			{
			case CameraTypeLine:
				m_pCameraWrapImpl[nViewIndex]->RegCallback(CBTYPE_CAP_DEPTH, (void *)DepthCallBack, this);
				break;
				
	#ifdef RELEASE_1G
			case CameraTypeLine2:
				m_pCameraWrapImpl[nViewIndex]->RegCallback(CBTYPE_CAP_DEPTH, (void *)DepthCallBack2, this);
				break;
			case CameraTypeLine3:
				m_pCameraWrapImpl[nViewIndex]->RegCallback(CBTYPE_CAP_DEPTH, (void *)DepthCallBack3, this);
				break;
			case CameraTypeLine4:
				m_pCameraWrapImpl[nViewIndex]->RegCallback(CBTYPE_CAP_DEPTH, (void *)DepthCallBack4, this);
				break;
	#endif
			}
		}
		else
			return FALSE;

		if (!SetPageCount(nViewIndex))
			return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
#endif

	return TRUE;
}

BOOL CxFrameGrabberPanSight::SetPageCount(UINT nViewIndex)
{
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	int nPageCount = 1;
	switch (nViewIndex)
	{
#ifdef RELEASE_SG
	case CameraTypeLine:	nPageCount = CModelInfo::Instance()->GetStackerOcr().lPageCount;	break;
#elif RELEASE_1G
	case CameraTypeLine:	nPageCount = CModelInfo::Instance()->GetTrayOcr().lPageCount;		break;
	case CameraTypeLine2:	nPageCount = CModelInfo::Instance()->GetMixing().lPageCount;		break;
	case CameraTypeLine3:
	case CameraTypeLine4:	nPageCount = CModelInfo::Instance()->GetLiftInfo().lPageCount;		break;
#endif // RELEASE_SG
	default:																					break;
	}

	if (nViewIndex >= CameraTypeMax || nViewIndex < 0)
		return FALSE;

#ifndef DEBUG
	if (m_pCameraWrapImpl[nViewIndex]->GetCameraIsConnect()) // 카메라 연결 안 된 상태에서 Change2DDepthMode() 실행시 프로그램 종료
	{
		if (!Change2DepthMode(nPageCount, nViewIndex))
			return FALSE;

		m_bOpen[nViewIndex] = TRUE;
	}
#endif

#endif // RELEASE_SG || RELEASE_1G
	return TRUE;
}

BOOL CxFrameGrabberPanSight::IsOpen(UINT nIndex)
{
	if (nIndex >= CameraTypeMax || nIndex < 0)
		return FALSE;

	return m_bOpen[nIndex];
}

BOOL CxFrameGrabberPanSight::Live(UINT nIndex)
{
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	try 
	{
		if (nIndex >= CameraTypeMax || nIndex < 0)
			return FALSE;

		if (m_bLive[nIndex])
			return TRUE;

		m_bLive[nIndex] = TRUE;

		switch (nIndex)
		{
		case CameraTypeLine:
			AfxBeginThread(CameraCaptureLive, this);
			break;
#ifdef RELEASE_1G
		case CameraTypeLine2:
			AfxBeginThread(CameraCaptureLive2, this);
			break;
		case CameraTypeLine3:
			AfxBeginThread(CameraCaptureLive3, this);
			break;
		case CameraTypeLine4:
			AfxBeginThread(CameraCaptureLive4, this);
			break;
#endif
		}
	} 
	catch (...)
	{
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CxFrameGrabberPanSight::IsLive(UINT nIndex)
{
	if (nIndex >= CameraTypeMax || nIndex < 0)
		return FALSE;

	return m_bLive[nIndex];
}


BOOL CxFrameGrabberPanSight::Grab(UINT nIndex)
{
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	try 
	{
		int nPageCount = 1;
		switch (nIndex)
		{
		case CameraTypeLine:
#ifdef RELEASE_SG
								nPageCount = CModelInfo::Instance()->GetStackerOcr().lPageCount;	break;
#elif RELEASE_1G
								nPageCount = CModelInfo::Instance()->GetTrayOcr().lPageCount;		break;
		case CameraTypeLine2:	nPageCount = CModelInfo::Instance()->GetMixing().lPageCount;		break;
		case CameraTypeLine3:
		case CameraTypeLine4:	nPageCount = CModelInfo::Instance()->GetLiftInfo().lPageCount;		break;
#endif
		default:
			break;
		}

		if (nIndex >= CameraTypeMax || nIndex < 0)
			return FALSE;

//		if (!Change2DepthMode(nPageCount, nIndex))
//			return FALSE;

		m_pCameraWrapImpl[nIndex]->SetPointCloudCallBackNum(nPageCount);
		m_pCameraWrapImpl[nIndex]->StartCapture();
		
		if (m_pCameraWrapImpl[nIndex]->SendGrabSignalToCamera())
		{
//			Sleep(2000);
//			m_pCameraWrapImpl[nIndex]->StopCapture();
		}
		else
			return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CxFrameGrabberPanSight::Idle(UINT nIndex)
{
	if (nIndex >= CameraTypeMax || nIndex < 0)
		return FALSE;

	if (!m_bLive[nIndex])
		return TRUE;

	BOOL result = m_pCameraWrapImpl[nIndex]->StopCapture();

	if (result)	
		m_bLive[nIndex] = FALSE;
	else					
		return FALSE;
		
	return TRUE;
}

void CxFrameGrabberPanSight::Close(UINT nIndex)
{
	if (!m_bOpen[nIndex])
		return;

	m_pCameraWrapImpl[nIndex]->CloseCamera();
}

void CxFrameGrabberPanSight::OnGrabFinish(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer) 
{
	TRACE(_T("CFrameGrabberPanSight::OnGrabFinish - Warning! Index: %dx%d, Channel=%d, Depth=%d, %p\n"), nWidth, nHeight, nChannel, pBuffer);
}

/*
	====================================================================================================
	↓ Line Scan Camera Live Thread Functions (static)
	====================================================================================================
*/

UINT CxFrameGrabberPanSight::CameraCaptureLive(LPVOID nParam) 
{
#if defined(RELEASE_1G)
	CxFrameGrabberPanSight* pPanSight = static_cast<CxFrameGrabberPanSight*>(nParam);

	int nPageCount = 1;
	nPageCount = CModelInfo::Instance()->GetTrayOcr().lPageCount;
	
	if (!pPanSight->Change2DepthMode(nPageCount, CameraTypeLine))
		return 0;

	while (pPanSight->IsLive(CameraTypeLine)) 
	{
		pPanSight->m_pCameraWrapImpl[CameraTypeLine]->SetPointCloudCallBackNum(nPageCount);
		pPanSight->m_pCameraWrapImpl[CameraTypeLine]->StartCapture();
		pPanSight->m_pCameraWrapImpl[CameraTypeLine]->SendGrabSignalToCamera();

//		Sleep(2000);
	}

	pPanSight->m_pCameraWrapImpl[CameraTypeLine]->StopCapture();
#endif
	return 0;
}

UINT CxFrameGrabberPanSight::CameraCaptureLive2(LPVOID nParam)
{
#if defined(RELEASE_1G)
	CxFrameGrabberPanSight* pPanSight = static_cast<CxFrameGrabberPanSight*>(nParam);

	int nPageCount = 1;
	nPageCount = CModelInfo::Instance()->GetMixing().lPageCount;

	if (!pPanSight->Change2DepthMode(nPageCount, CameraTypeLine2))
		return 0;

	while (pPanSight->IsLive(CameraTypeLine2)) 
	{
		pPanSight->m_pCameraWrapImpl[CameraTypeLine2]->SetPointCloudCallBackNum(nPageCount);
		pPanSight->m_pCameraWrapImpl[CameraTypeLine2]->StartCapture();
		pPanSight->m_pCameraWrapImpl[CameraTypeLine2]->SendGrabSignalToCamera();

//		Sleep(2000);
	}

	pPanSight->m_pCameraWrapImpl[CameraTypeLine2]->StopCapture();
#endif
	return 0;
}

UINT CxFrameGrabberPanSight::CameraCaptureLive3(LPVOID nParam)
{
#if defined(RELEASE_1G)
	CxFrameGrabberPanSight* pPanSight = static_cast<CxFrameGrabberPanSight*>(nParam);

	int nPageCount = 1;
	nPageCount = CModelInfo::Instance()->GetLiftInfo().lPageCount;

	if (!pPanSight->Change2DepthMode(nPageCount, CameraTypeLine3))
		return 0;

	while (pPanSight->IsLive(CameraTypeLine3)) 
	{
		pPanSight->m_pCameraWrapImpl[CameraTypeLine3]->SetPointCloudCallBackNum(nPageCount);
		pPanSight->m_pCameraWrapImpl[CameraTypeLine3]->StartCapture();
		pPanSight->m_pCameraWrapImpl[CameraTypeLine3]->SendGrabSignalToCamera();

//		Sleep(2000);
	}

	pPanSight->m_pCameraWrapImpl[CameraTypeLine3]->StopCapture();
#endif
	return 0;
}

UINT CxFrameGrabberPanSight::CameraCaptureLive4(LPVOID nParam)
{
#if defined(RELEASE_1G)
	CxFrameGrabberPanSight* pPanSight = static_cast<CxFrameGrabberPanSight*>(nParam);

	int nPageCount = 1;
	nPageCount = CModelInfo::Instance()->GetLiftInfo().lPageCount;

	if (!pPanSight->Change2DepthMode(nPageCount, CameraTypeLine4))
		return 0;

	while (pPanSight->IsLive(CameraTypeLine4)) 
	{
		pPanSight->m_pCameraWrapImpl[CameraTypeLine4]->SetPointCloudCallBackNum(nPageCount);
		pPanSight->m_pCameraWrapImpl[CameraTypeLine4]->StartCapture();
		pPanSight->m_pCameraWrapImpl[CameraTypeLine4]->SendGrabSignalToCamera();

//		Sleep(2000);
	}

	pPanSight->m_pCameraWrapImpl[CameraTypeLine4]->StopCapture();
#endif
	return 0;
}

/*
	==================================================================================
	↓ private functions
	==================================================================================
*/

BOOL CxFrameGrabberPanSight::Change2DepthMode(int iGrabLineNum, UINT nIndex) 
{
	if (!m_pCameraWrapImpl[nIndex]->SetUserOperatorMode(DATAMODE_GRAB))
		return FALSE;
	if (!m_pCameraWrapImpl[nIndex]->SetDataType(DATATYPE_PROFILE_GRAY))
		return FALSE;
	if (!m_pCameraWrapImpl[nIndex]->SetTransMode(TRANSMODE_BATCH_SW))
		return FALSE;
	if (!m_pCameraWrapImpl[nIndex]->SetBatchGrabNumber(iGrabLineNum))
		return FALSE;

	return TRUE;
}

UINT8 CxFrameGrabberPanSight::Rescale(float value, float max, float min)
{
	if (value < min)
		return 0;
	else if (value > max)
		return 255;
	else if (abs(max - min) < 0.00001)
		return 0;

	return (unsigned char)(((value - min) / (max - min)) * 255);
}

void CxFrameGrabberPanSight::DepthData2Gray(float *pDepth, UINT8* pGray, int iDataSize, int nIndex)
{
	float zMax = 0; //pDepth[0];
	float zMin = 255;// pDepth[0];
	
	CSystemConfig::Cam3DScaleValue& Cam3DScaleValue = CSystemConfig::Instance()->GetCam3DScaleValue();
	float fLow, fHigh;
	fLow = Cam3DScaleValue.fScaleLowValues[nIndex];
	fHigh = Cam3DScaleValue.fScaleHighValues[nIndex];

	for (int j = 0; j < iDataSize; j++)
	{
		if (pDepth[j] > 0)
		{
			if ((pDepth[j] < fLow) || (pDepth[j] > fHigh))
				pDepth[j] = 0;

			if(( pDepth[j] < zMin) && (pDepth[j] > fLow) )
				zMin = pDepth[j];
			if ((pDepth[j] > zMax) && (pDepth[j] < fHigh) )
				zMax = pDepth[j];
		}
	}
	for (int i = 0; i < iDataSize; i++)
		pGray[i] = Rescale(pDepth[i], zMax, zMin);
}

void CxFrameGrabberPanSight::OnDepth(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam)
{
#if defined(RELEASE_1G) || defined(RELEASE_SG)
	m_pCameraWrapImpl[CameraTypeLine]->StopCapture();

	SGPOINT* pPnt = (SGPOINT*)pBuf;
	int h = pParam->_iCapturedProfileLineNum; // 캡처된 라인의 수
	int w = pParam->_iPointNumPerLine;        // 각 라인당 포인트의 수

	std::vector<float> depthData(h * w);
	std::vector<unsigned char> grayData(h * w);
	m_depthData.resize(h * w);

	// 깊이 데이터를 추출하여 depthData에 저장
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			depthData[i * w + j] = pPnt[i * w + j]._z;
	
	// 변환용
	m_depthData = depthData;
	m_nDepthWidth = w;
	m_nDepthHeight = h;

	// 깊이 데이터를 그레이스케일로 변환
	DepthData2Gray(depthData.data(), grayData.data(), h * w, 0);

	cv::Mat grayImage(h, w, CV_8UC1, grayData.data());
	cv::Mat rotateImage(h, w, CV_8UC1);
#endif

#ifdef RELEASE_1G
	cv::rotate(grayImage, rotateImage, cv::ROTATE_180);
	CModelInfo::stTrayOcr& stTrayOcr = CModelInfo::Instance()->GetTrayOcr();

//	if (stTrayOcr.bRotateImage)		cv::rotate(grayImage, rotateImage, cv::ROTATE_180);
	//cv::flip(rotateImage, rotateImage, 1); // flipCode > 0 → y-axis flip
	if (stTrayOcr.bRotateImage)
		OnGrabFinish(rotateImage.cols, rotateImage.rows, CameraTypeLine, rotateImage.data);
	else
		OnGrabFinish(grayImage.cols, grayImage.rows, CameraTypeLine, grayImage.data);


#elif RELEASE_SG
	//cv::flip(rotateImage, rotateImage, 1); // flipCode > 0 → y-axis flip
	cv::rotate(grayImage, rotateImage, cv::ROTATE_90_CLOCKWISE);
	OnGrabFinish(rotateImage.cols, rotateImage.rows, CameraTypeLine, rotateImage.data);

#endif
	
//	int nPageCount = 0;
//	while (nPageCount <= CVisionSystem::Instance()->GetMax3DPageCnt(IDX_AREA1))
//	{
//		float fTotalData[51840]; //nAREA_CAM_SIZE_X[IDX_AREA1]];
//		for (int i = 0; i < h; i++)
//			for (int j = 0; j < w; j++)
//				fTotalData[i * w + j] = pPnt[i * w + j]._z;
//
//		OnGrabFinish(w, h, CameraTypeLine, fTotalData);
//		nPageCount++;
//	}
}

void CxFrameGrabberPanSight::OnDepth2(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam)
{
#if defined(RELEASE_1G)
	m_pCameraWrapImpl[CameraTypeLine2]->StopCapture();
	
	SGPOINT* pPnt = (SGPOINT*)pBuf;
	int h = pParam->_iCapturedProfileLineNum; // 캡처된 라인의 수
	int w = pParam->_iPointNumPerLine;        // 각 라인당 포인트의 수

	std::vector<float> depthData(h * w);
	std::vector<unsigned char> grayData(h * w);

	// 깊이 데이터를 추출하여 depthData에 저장
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			depthData[i * w + j] = pPnt[i * w + j]._z;

	// 깊이 데이터를 그레이스케일로 변환
	DepthData2Gray(depthData.data(), grayData.data(), h * w, 1);

	cv::Mat grayImage(h, w, CV_8UC1, grayData.data());
	cv::Mat rotateImage(w, h, CV_8UC1);
	//cv::rotate(grayImage, rotateImage, 2);
	cv::rotate(grayImage, rotateImage, cv::ROTATE_90_COUNTERCLOCKWISE);

	OnGrabFinish(rotateImage.cols, rotateImage.rows, CameraTypeLine2 + 1, rotateImage.data);
#endif
}

void CxFrameGrabberPanSight::OnDepth3(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam)
{
#if defined(RELEASE_1G)
	m_pCameraWrapImpl[CameraTypeLine3]->StopCapture();

	SGPOINT* pPnt = (SGPOINT*)pBuf;
	int h = pParam->_iCapturedProfileLineNum; // 캡처된 라인의 수
	int w = pParam->_iPointNumPerLine;        // 각 라인당 포인트의 수

	std::vector<float> depthData(h * w);
	std::vector<unsigned char> grayData(h * w);

	// 깊이 데이터를 추출하여 depthData에 저장
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			depthData[i * w + j] = pPnt[i * w + j]._z;

	// 깊이 데이터를 그레이스케일로 변환
	DepthData2Gray(depthData.data(), grayData.data(), h * w, 2);

	cv::Mat grayImage(h, w, CV_8UC1, grayData.data());
	cv::Mat rotateImage(w, h, CV_8UC1);
	//cv::rotate(grayImage, rotateImage, 2);
	cv::rotate(grayImage, rotateImage, cv::ROTATE_90_COUNTERCLOCKWISE);

	OnGrabFinish(rotateImage.cols, rotateImage.rows, CameraTypeLine3 + 1, rotateImage.data);
#endif
}

void CxFrameGrabberPanSight::OnDepth4(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam)
{
#if defined(RELEASE_1G)
	m_pCameraWrapImpl[CameraTypeLine4]->StopCapture();
	
	SGPOINT* pPnt = (SGPOINT*)pBuf;
	int h = pParam->_iCapturedProfileLineNum; // 캡처된 라인의 수
	int w = pParam->_iPointNumPerLine;        // 각 라인당 포인트의 수

	std::vector<float> depthData(h * w);
	std::vector<unsigned char> grayData(h * w);

	// 깊이 데이터를 추출하여 depthData에 저장
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			depthData[i * w + j] = pPnt[i * w + j]._z;

	// 깊이 데이터를 그레이스케일로 변환
	DepthData2Gray(depthData.data(), grayData.data(), h * w, 3);

	cv::Mat grayImage(h, w, CV_8UC1, grayData.data());
	cv::Mat rotateImage(w, h, CV_8UC1);
	//cv::rotate(grayImage, rotateImage, 2);
	cv::rotate(grayImage, rotateImage, cv::ROTATE_90_COUNTERCLOCKWISE);

	OnGrabFinish(rotateImage.cols, rotateImage.rows, CameraTypeLine4 + 1, rotateImage.data);
#endif
}

/*static*/ void CxFrameGrabberPanSight::DepthCallBack(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner)
{
	if (pOwner != NULL)
	{
		CxFrameGrabberPanSight* pThis = static_cast<CxFrameGrabberPanSight*>(pOwner);
		pThis->OnDepth((float*)pBuf, (char*)pGrayBuf, pParam);
	}
}

/*static*/ void CxFrameGrabberPanSight::DepthCallBack2(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner)
{
	if (pOwner != NULL)
	{
		CxFrameGrabberPanSight* pThis = static_cast<CxFrameGrabberPanSight*>(pOwner);
		pThis->OnDepth2((float*)pBuf, (char*)pGrayBuf, pParam);
	}
}

/*static*/ void CxFrameGrabberPanSight::DepthCallBack3(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner)
{
	if (pOwner != NULL)
	{
		CxFrameGrabberPanSight* pThis = static_cast<CxFrameGrabberPanSight*>(pOwner);
		pThis->OnDepth3((float*)pBuf, (char*)pGrayBuf, pParam);
	}
}

/*static*/ void CxFrameGrabberPanSight::DepthCallBack4(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner)
{
	if (pOwner != NULL)
	{
		CxFrameGrabberPanSight* pThis = static_cast<CxFrameGrabberPanSight*>(pOwner);
		pThis->OnDepth4((float*)pBuf, (char*)pGrayBuf, pParam);
	}
}
