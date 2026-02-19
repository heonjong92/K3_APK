#pragma once
#include "../stdafx.h"
#include "xFrameGrabber.h"
#include "../SystemConfig.h"

#include <string>
#include <atomic>

#include <CameraWrapImpl.h>
#include <CommonMethods.h>

struct SGPOINT
{
	float _y;
	float _x;
	float _z;
};

namespace VisionDevice
{
	class CxFrameGrabberPanSight : public VisionDevice::CxFrameGrabber
	{
	private:
		// No copies do not implement
		CxFrameGrabberPanSight(const CxFrameGrabberPanSight &rhs);
		//CxFrameGrabberPanSight &operator=(const CxFrameGrabberPanSight &rhs);

	public:
		CxFrameGrabberPanSight(void);
		virtual ~CxFrameGrabberPanSight(void);

	protected:
		BOOL m_bLive[CameraTypeMax];
		BOOL m_bOpen[CameraTypeMax];

		UINT m_nSizeX[CameraTypeMax];
		UINT m_nSizeY[CameraTypeMax];

		int m_nCameraIndex[CameraTypeMax];

		BOOL FindCameraIndex(std::vector<string> vec, UINT nIndex, OUT UINT &nViewIndex);
		void SetCameraIndex(UINT nIndex, UINT nViewIndex) { m_nCameraIndex[nViewIndex] = nIndex; }
		int GetCameraIndex(UINT nIndex) const { return m_nCameraIndex[nIndex]; }

		CameraWrapImpl* m_pCameraWrapImpl[CameraTypeMax];

//		std::atomic_int m_nHaveGrabLineNum;

	public:
		BOOL Open(UINT nIndex, OUT UINT& nViewIndex);
		BOOL IsOpen(UINT nIndex);

		void Close(UINT nIndex);

		BOOL Grab(UINT nIndex);
		BOOL Live(UINT nIndex);
		BOOL Idle(UINT nIndex);
		BOOL IsLive(UINT nIndex);

		BOOL SetPageCount(UINT nViewIndex);

		virtual void OnGrabFinish(UINT nWidth, UINT nHeight, int nChannel, void* pBuffer);

		// Line Scan Camera Live Thread Functions
		static UINT CameraCaptureLive(LPVOID nParam);
		static UINT CameraCaptureLive2(LPVOID nParam);
		static UINT CameraCaptureLive3(LPVOID nParam);
		static UINT CameraCaptureLive4(LPVOID nParam);

		// Callback Functions
		static void DepthCallBack(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner);
		static void DepthCallBack2(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner);
		static void DepthCallBack3(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner);
		static void DepthCallBack4(void *pBuf, void *pGrayBuf, SG_DEPTHDATA_PARAM *pParam, void *pOwner);

		vector<float> m_depthData;
		int m_nDepthWidth;
		int m_nDepthHeight;

		const vector<float>& GetDepthData() const { return m_depthData; }

	private:
		BOOL Change2DepthMode(int nGrabLineNum, UINT nIndex);
		UINT8 Rescale(float value, float max, float min);
		void DepthData2Gray(float *pDepth, UINT8* pGray, int iDataSize, int nIndex);

		void OnDepth(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam);
		void OnDepth2(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam);
		void OnDepth3(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam);
		void OnDepth4(float *pBuf, char*pGrayBuf, SG_DEPTHDATA_PARAM *pParam);
	};
}
