#include "stdafx.h"

#include "xFrameGrabberMechEye.h"
#include "../VisionSystem.h"

#include <XUtil/xUtils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace VisionDevice;

CxFrameGrabberMechEye::CxFrameGrabberMechEye(void) 
{
	for( int i=0; i<1; ++i )
	{
		m_bOpen[i] = FALSE;
		m_bLive[i] = FALSE;

		m_nSizeX[i] = 0;
		m_nSizeY[i] = 0;
		m_strSerialNo[i] = _T("");
	}
}

CxFrameGrabberMechEye::~CxFrameGrabberMechEye(void)
{	
	for( int i=0; i<1; ++i )
		Close(i);

}

BOOL CxFrameGrabberMechEye::Open(UINT nIndex, OUT UINT& nViewIndex )
{
	BOOL bReveal = FALSE;
#if defined(RELEASE_1G)
	m_DeviceInfoList = mmind::eye::Camera::discoverCameras();

	if (m_DeviceInfoList.empty()) {
		return FALSE;
	}

	CSystemConfig::CameraSerialNo& SerialNoConfig = CSystemConfig::Instance()->GetCameraSerialNo();
	m_strSerialNo[0] = SerialNoConfig.strSerialNo[1];
	std::string str(CT2A(m_strSerialNo[0].GetString()));


	if (m_DeviceInfoList[0].ipAddress == str)
	{
		m_Status = m_Camera.connect(m_DeviceInfoList[0]);
		if (!m_Status.isOK()) {
			return FALSE;
		}
		else
		{
			bReveal = TRUE;
			nViewIndex = IDX_AREA2;
		}
	}

	m_bOpen[nIndex] = bReveal;

#endif
	return bReveal;
}

void CxFrameGrabberMechEye::Close( UINT nIndex )
{
	if( nIndex >= 1 || nIndex < 0 )
		return;

	if( !m_bOpen[nIndex])
		return;

	m_Camera.disconnect();

	m_bOpen[nIndex] = FALSE;
}

BOOL CxFrameGrabberMechEye::Grab( UINT nIndex )
{
#if defined(RELEASE_1G)
	if( nIndex >= 1 || nIndex < 0 )
		return FALSE;

	// 2D 캡쳐
//	mmind::eye::Frame2D frame2D;
//	m_Camera.capture2D(frame2D);
//	mmind::eye::GrayScale2DImage grayImage = frame2D.getGrayScaleImage();
//
//	OnGrabFinish( grayImage.width(), grayImage.height(), IDX_AREA2, grayImage.data() );

	// 3D 캡쳐
	mmind::eye::Frame3D frame3D;
	m_Camera.capture3D(frame3D);
	
	// DepthMap 획득, 8Bit 변환
	mmind::eye::DepthMap depthMap = frame3D.getDepthMap();

//	cv::Mat depth32F = cv::Mat(depthMap.height(), depthMap.width(), CV_32FC1, depthMap.data());
	if (depthMap.height() > std::numeric_limits<int>::max() || depthMap.width() > std::numeric_limits<int>::max()) {
		throw std::overflow_error("Image dimensions exceed int limits");
	}
	cv::Mat depth32F = cv::Mat(static_cast<int>(depthMap.height()), static_cast<int>(depthMap.width()), CV_32FC1, depthMap.data());
	cv::Mat renderedDepth = renderGrayDepthData(depth32F);

	cv::Mat rotateImage(renderedDepth.cols, renderedDepth.rows, CV_8UC1);
	cv::rotate(renderedDepth, rotateImage, 1);

	// OnGrabfinish
	OnGrabFinish(rotateImage.cols, rotateImage.rows, IDX_AREA2, rotateImage.data );
#endif
	return TRUE;
}

BOOL CxFrameGrabberMechEye::Live( UINT nIndex )
{
	if (nIndex >= 1 || nIndex < 0)
		return FALSE;

	if (m_bLive[nIndex])
		return TRUE;

	m_bLive[nIndex] = TRUE;

	AfxBeginThread(CxFrameGrabberMechEye::CameraCaptureLive, (LPVOID)this);

	return TRUE;
}

cv::Mat CxFrameGrabberMechEye::renderGrayDepthData(const cv::Mat& depth)
{

	// 입력된 depth 이미지가 비어 있으면 빈 행렬 반환
	if (depth.empty())
		return cv::Mat();

	// 유효한 depth 값을 가진 픽셀을 위한 마스크 생성
	cv::Mat mask = cv::Mat(depth == depth);  // NaN 필터링

	// 최소, 최대 깊이 값을 계산
	double minDepthValue, maxDepthValue;
	cv::minMaxLoc(depth, &minDepthValue, &maxDepthValue, nullptr, nullptr, mask);

	CSystemConfig::Cam3DScaleValue& Cam3DScaleValue = CSystemConfig::Instance()->GetCam3DScaleValue();
	minDepthValue = Cam3DScaleValue.fScaleAreaLowValues; // 360.f;
	maxDepthValue = Cam3DScaleValue.fScaleAreaHighValues; // 400.f;

	// 깊이 데이터를 8비트로 변환
	cv::Mat depth8U;
	if (isApprox0(maxDepthValue - minDepthValue)) {
		// 깊이 값이 거의 일정할 때 8비트로 변환 (스케일링 없이)
		depth.convertTo(depth8U, CV_8UC1);
	}
	else {
		// 깊이 값을 0~255 범위로 스케일링하여 8비트로 변환
//			depth.convertTo(depth8U, CV_8UC1, (255.0 / (maxDepthValue - minDepthValue)),
//				-minDepthValue * (255.0 / (maxDepthValue - minDepthValue)));

			// 색상 반전, 멀리 있는게 어둡게
		depth.convertTo(depth8U, CV_8UC1, -(255.0 / (maxDepthValue - minDepthValue)),
			(maxDepthValue * 255.0 / (maxDepthValue - minDepthValue)));
	}

	// 변환된 8비트 깊이 이미지가 비어 있으면 빈 행렬 반환
	if (depth8U.empty())
		return cv::Mat();

	// 유효하지 않은 깊이 값(0인 부분)은 검정색으로 처리
	depth8U.forEach<uchar>([&](auto& val, const int* pos) {
		if (!depth.ptr<float>(pos[0])[pos[1]]) {
			val = 0;  // 깊이 값이 0이면 해당 픽셀을 검정색(0)으로 설정
		}
	});

	// 결과로 8비트 그레이스케일 깊이 이미지 반환
	return depth8U;
}

BOOL CxFrameGrabberMechEye::Idle( UINT nIndex )
{
	if( nIndex >= 1 || nIndex < 0 )
		return FALSE;

	if (!m_bLive[nIndex])
		return TRUE;

	m_bLive[nIndex] = FALSE;

	return TRUE;
}

BOOL CxFrameGrabberMechEye::IsOpen( UINT nIndex )
{
	if( nIndex >= CameraTypeMax || nIndex < 0 )
		return FALSE;

	return m_bOpen[nIndex]; 
}

BOOL CxFrameGrabberMechEye::IsLive(UINT nIndex)
{
	if (nIndex >= CameraTypeMax || nIndex < 0)
		return FALSE;

	return m_bLive[nIndex];
}

void CxFrameGrabberMechEye::OnGrabFinish( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer )
{
	TRACE( _T("CxFrameGrabberMechEye::OnGrabFinish - Warning! Index: %dx%d, Channel=%d, Depth=%d, %p\n"), nWidth, nHeight, nChannel, pBuffer );
}

UINT CxFrameGrabberMechEye::CameraCaptureLive( LPVOID nParam ) 
{
#if defined(RELEASE_1G)
	CxFrameGrabberMechEye* mechEye =  static_cast<CxFrameGrabberMechEye*>(nParam);

	while (mechEye->IsLive(0))
	{
		mmind::eye::Frame3D frame3D;
		mechEye->m_Camera.capture3D(frame3D);

		// DepthMap 획득, 8Bit 변환
		mmind::eye::DepthMap depthMap = frame3D.getDepthMap();

		//	cv::Mat depth32F = cv::Mat(depthMap.height(), depthMap.width(), CV_32FC1, depthMap.data());
		if (depthMap.height() > std::numeric_limits<int>::max() || depthMap.width() > std::numeric_limits<int>::max()) {
			throw std::overflow_error("Image dimensions exceed int limits");
		}
		cv::Mat depth32F = cv::Mat(static_cast<int>(depthMap.height()), static_cast<int>(depthMap.width()), CV_32FC1, depthMap.data());
		cv::Mat renderedDepth = mechEye->renderGrayDepthData(depth32F);

		cv::Mat rotateImage(renderedDepth.cols, renderedDepth.rows, CV_8UC1);
		cv::rotate(renderedDepth, rotateImage, 1);

		// OnGrabfinish
		/*if(//좌측면 검사)*/
		mechEye->OnGrabFinish(rotateImage.cols, rotateImage.rows, IDX_AREA2, rotateImage.data);
		/*else//우측면 검사*/
		// mechEye->OnGrabFinish(renderedDepth.cols, renderedDepth.rows, IDX_AREA3, renderedDepth.data);

		// 추후 트리거로 변경해야 함
		Sleep(500);
	}
#endif
	return 0;
}