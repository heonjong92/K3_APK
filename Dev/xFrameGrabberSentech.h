#pragma once
#include "xFrameGrabber.h"
#include "../SystemConfig.h"

#include <StApi_TL.h>
#include <StApi_IP.h>
#include <StApi_GUI.h>

#include <opencv2/opencv.hpp>

using namespace StApi;
using namespace GenApi;

namespace VisionDevice
{
class CFrameGrabberSentech : public CxFrameGrabber
{
private:
	// No copies do not implement
	CFrameGrabberSentech(const CFrameGrabberSentech &rhs);
	//CFrameGrabberSentech &operator=(const CFrameGrabberSentech &rhs);

public:
	CFrameGrabberSentech(void);
	virtual ~CFrameGrabberSentech(void);

protected:
	BOOL m_bLive[CameraTypeMax];
	BOOL m_bOpen[CameraTypeMax];

	UINT m_nSizeX[CameraTypeMax];
	UINT m_nSizeY[CameraTypeMax];

	int m_nCameraIndex[CameraTypeMax];

	BOOL FindCameraIndex( CString strSerialNo, OUT UINT& nViewIndex );
	void SetCameraIndex( UINT nIndex, UINT nViewIndex )	{ m_nCameraIndex[nIndex] = nViewIndex; }
	int GetCameraIndex( UINT nIndex )	{	return m_nCameraIndex[nIndex];	}


	CIStDevicePtr m_pIStDevice[CameraTypeMax];
	StApi::CIStDataStreamPtr m_pIStDataStream[CameraTypeMax];
	IStDeviceReleasable *m_pIStDeviceReleasable;
	CStApiAutoInit	m_objStApiAutoInit;
	StApi::CIStSystemPtrArray m_objIStSystemPtrList;

public:
	BOOL Open(UINT nIndex, OUT UINT& nViewIndex );
	BOOL IsOpen( UINT nIndex );
	void Close( UINT nIndex );
	BOOL Grab( UINT nIndex );
	BOOL Live( UINT nIndex );
	BOOL Idle( UINT nIndex );
	BOOL IsLive( UINT nIndex );
	BOOL ExposureChange(int nIndex, double dExposure);
	BOOL GrabChange(BOOL bUseSoft,int nIndex,int nTriggerType =0);
	void SetEnumeration(INodeMap *pINodeMap, int nIndex, const char *szEnumerationName, const char *szValueName);
	UINT GetSizeX( UINT nIndex ){ return m_nSizeX[nIndex]; }
	UINT GetSizeY( UINT nIndex ){ return m_nSizeY[nIndex]; }

	BOOL m_bRotate;

	virtual void OnGrabFinish( UINT nWidth, UINT nHeight, int nChannel, void* pBuffer );

	static void CALLBACK OnStCallbackCFunction(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 0, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}
	static void CALLBACK OnStCallbackCFunction2(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
#if defined(RELEASE_2G) || defined(RELEASE_6G)
				CIStImageBufferPtr pIStImageBuffer = CreateIStImageBuffer();

				// Create a data converter object for pixel format conversion.
				CIStPixelFormatConverterPtr pIStPixelFormatConverter = CreateIStConverter(StConverterType_PixelFormat);

				// Convert the image data to StPFNC_BGR8 format
				pIStPixelFormatConverter->SetDestinationPixelFormat(StPFNC_BGR8);
				pIStPixelFormatConverter->Convert(pIStStreamBuffer->GetIStImage(), pIStImageBuffer);

				StApi::IStImage *pIStImage = pIStImageBuffer->GetIStImage();
#else
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
#endif
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 1, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}

	static void CALLBACK OnStCallbackCFunction3(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
#ifdef RELEASE_1G
				// cnt가 더 많이 찍고 이미 돌렸으니, Ocr을 조건으로 걸자.
				if( ((CFrameGrabberSentech*)pvContext)->m_bRotate)
				{
					((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 2, pIStImage->GetImageBuffer());
				}
				else
				{
					cv::Mat grayImage((int)pIStImage->GetImageHeight(), (int)pIStImage->GetImageWidth(), CV_8UC1, pIStImage->GetImageBuffer());
					cv::Mat rotateImage((int)pIStImage->GetImageWidth(), (int)pIStImage->GetImageHeight(), CV_8UC1);
					cv::rotate(grayImage, rotateImage, cv::ROTATE_180);

					((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 2, rotateImage.data);
				}
#else
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 2, pIStImage->GetImageBuffer());

#endif // RELEASE_1G
			}
			catch (...)
			{
				;
			}
		}
	}
	static void CALLBACK OnStCallbackCFunction4(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 3, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}
	static void CALLBACK OnStCallbackCFunction5(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 4, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}
	static void CALLBACK OnStCallbackCFunction6(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 5, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}
	static void CALLBACK OnStCallbackCFunction7(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 6, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}
	static void CALLBACK OnStCallbackCFunction8(StApi::IStCallbackParamBase *pIStCallbackParamBase, void * pvContext)
	{
		StApi::IStCallbackParamGenTLEventNewBuffer *pIStCallbackParamGenTLEventNewBuffer = dynamic_cast<StApi::IStCallbackParamGenTLEventNewBuffer*>(pIStCallbackParamBase);
		StApi::IStDataStream *pIStDataStream = pIStCallbackParamGenTLEventNewBuffer->GetIStDataStream();
		StApi::CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(0));
		if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
		{
			try
			{
				StApi::IStImage *pIStImage = pIStStreamBuffer->GetIStImage();
				((CFrameGrabberSentech*)pvContext)->OnGrabFinish((UINT)pIStImage->GetImageWidth(), (UINT)pIStImage->GetImageHeight(), 7, pIStImage->GetImageBuffer());
			}
			catch (...)
			{
				;
			}
		}
	}
};
}
