#pragma once
#include "xFrameGrabber.h"
#include "../SystemConfig.h"

#include "../Include/area_scan_3d_camera/Camera.h"
#include "../Include/area_scan_3d_camera/api_util.h"

#include <opencv2/opencv.hpp>

namespace VisionDevice
{
class CxFrameGrabberMechEye : public CxFrameGrabber
{
private:
	// No copies do not implement
	CxFrameGrabberMechEye(const CxFrameGrabberMechEye &rhs);
	//CxFrameGrabberMechEye &operator=(const CFrameGrabberSentech &rhs);

public:
	CxFrameGrabberMechEye(void);
	virtual ~CxFrameGrabberMechEye(void);

protected:
	mmind::eye::Camera m_Camera;
	std::vector<mmind::eye::CameraInfo> m_DeviceInfoList;
	mmind::eye::ErrorStatus m_Status;
	CString m_strSerialNo[1];

	BOOL m_bOpen[1];
	BOOL m_bLive[1];

	UINT m_nSizeX[1];
	UINT m_nSizeY[1];
public:
	BOOL Open(UINT nIndex, OUT UINT& nViewIndex );
	BOOL IsOpen( UINT nIndex );
	BOOL IsLive( UINT nIndex );
	void Close( UINT nIndex );

	BOOL Live( UINT nIndex );
	BOOL Grab( UINT nIndex );
	BOOL Idle( UINT nIndex );

	virtual void OnGrabFinish( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer );
	inline bool isApprox0(double d) { return std::fabs(d) <= DBL_EPSILON; }

// Live 버튼 클릭시 연속 촬영을 위한 Thread Call-back Function
	static UINT CameraCaptureLive(LPVOID nParam);

protected:
	cv::Mat renderGrayDepthData(const cv::Mat& depth);
};
}
