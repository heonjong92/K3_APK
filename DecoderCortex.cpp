#include "stdafx.h"
#include "APK.h"
#include "DecoderCortex.h"

#include <XImage/xImageObject.h>
#include <XGraphic/xGraphicObject.h>
#include <XGraphic/xDataTypes.h>
#include <XUtil/xStopWatch.h>

#include "VisionSystem.h"
#include "APKView.h"

#ifndef _CodeUsb

#pragma warning(disable: 4312)
CDecoderCortex* CDecoderCortex::m_pThis = NULL;

CDecoderCortex::CDecoderCortex(void)
{
	m_pThis = this;

	decoderHandle = 0;
	initResultData();
}

CDecoderCortex::~CDecoderCortex(void)
{
}

int CDecoderCortex::InitializeDecoder(void)
{
    decoderHandle = CRD_Create();

    if (decoderHandle > 0 && decoderHandle < ERR_INVALID_HANDLE)
    {
		if (CRD_InitMultiEx(decoderHandle, MAX_NUM_BARCODES) < 0)
		{
			return -1;
		}
        return 0;
    }
    else
    {
        return -1;
    }
}

int CDecoderCortex::FreeDecoderHandle(void)
{
    if (decoderHandle > 0)
    {
        CRD_Destroy(decoderHandle);
        return 0;
    }
    else
    {
        return -1;
    }
}

int CDecoderCortex::DecodeImage(CxImageObject& pMaskingImgObj)
{
	initResultData();

	/* Set image properties */
	CRD_Set(decoderHandle,		P_IMAGE_ADDRESS,				pMaskingImgObj.GetImageBuffer());
	CRD_SetInt(decoderHandle,	P_IMAGE_WIDTH,					pMaskingImgObj.GetWidth());
	CRD_SetInt(decoderHandle,	P_IMAGE_HEIGHT,					pMaskingImgObj.GetHeight());
	CRD_SetInt(decoderHandle,	P_IMAGE_BUF_WIDTH,				pMaskingImgObj.GetWidthBytes());

	/* Set symbology properties */
	CRD_SetInt(decoderHandle,	P_ENABLE_C128,					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_C39,					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_I25,					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_CBAR,					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_C93, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_UPCA, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_UPCA, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_EAN13, 				1);
	CRD_SetInt(decoderHandle,	P_ENABLE_EAN8, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_UPCE, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_DB_14, 				1);
	CRD_SetInt(decoderHandle,	P_ENABLE_DB_14_STACKED, 		1);
	CRD_SetInt(decoderHandle,	P_ENABLE_DB_LIMITED, 			1);
	CRD_SetInt(decoderHandle,	P_ENABLE_DB_EXPANDED, 			1);
	CRD_SetInt(decoderHandle,	P_ENABLE_DB_EXPANDED_STACKED,	1);
	CRD_SetInt(decoderHandle,	P_ENABLE_DM, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_QR_2005, 				1);
	CRD_SetInt(decoderHandle,	P_ENABLE_MICRO_QR,				1);
	CRD_SetInt(decoderHandle,	P_ENABLE_QR_MODEL1,				1);
	CRD_SetInt(decoderHandle,	P_ENABLE_AZ, 					1);
	CRD_SetInt(decoderHandle,	P_ENABLE_PDF,					1);

	m_nNumDecoded = 0;
	m_nNumDecoded = CRD_DecodeMultiEx(decoderHandle, m_nNumDecoded);

	if (m_nNumDecoded > 0 && m_nNumDecoded < ERR_INVALID_HANDLE)
		return TRUE;
	else
		return FALSE;
}

char* CDecoderCortex::getSymbName(int nSymbolType)
{
	if (nSymbolType == V_SYMB_DM)
		return "DataMatrix";
	else if (nSymbolType == V_SYMB_QR_2005)
		return "QR-Code";
	else if (nSymbolType == V_SYMB_AZ)
		return "Aztec";
	else if (nSymbolType == V_SYMB_C128)
		return "Code-128";
	else if (nSymbolType == V_SYMB_C39)
		return "Code-39";
	else if (nSymbolType == V_SYMB_QR_MICRO)
		return "QR-Micro";
	else if (nSymbolType == V_SYMB_QR_MODEL1)
		return "QR-Model1";
	else if (nSymbolType == V_SYMB_QR_2005)
		return "QR-2005";
	else if (nSymbolType == V_SYMB_PDF)
		return "PDF417";
	else
		return "TBD";
}

BOOL CDecoderCortex::GetResultData(int nNum)
{
//	for (int nNum = 0; nNum < m_nNumDecoded; nNum++)
	{
		m_nDataLength[nNum] = CRD_GetIntMulti(decoderHandle, P_RESULT_LENGTH, nNum);

		CRD_GetIntsMulti(decoderHandle, P_RESULT_CORNERS, NUM_CORNERS * 2, nNum, &m_nCorner[nNum][0]);
		CRD_GetIntsMulti(decoderHandle, P_RESULT_CENTER, 2, nNum, &m_nCenter[nNum][0]);

		int nSymbolType = CRD_GetIntMulti(decoderHandle, P_RESULT_SYMBOL_TYPE, nNum);
		m_strType[nNum] = getSymbName(nSymbolType);

		int nDataLength = m_nDataLength[nNum];
		if ( (nDataLength <= MAX_DATA_LENGTH) && (nDataLength > 0) )
		{
			CRD_GetBytesMulti(decoderHandle, P_RESULT_STRING, nDataLength, nNum, &m_chOutBuf[nNum][0]);
			m_chOutBuf[nNum][nDataLength] = '\0'; // null termination

			for (int i = 0; i < nDataLength; i++)
			{
				m_strData[nNum] += m_chOutBuf[nNum][i];
			}

			m_rcResult[nNum].SetRectEmpty();
			m_rcResult[nNum].left = (m_nCorner[nNum][4] < m_nCorner[nNum][6]) ? m_nCorner[nNum][4] : m_nCorner[nNum][6];
			m_rcResult[nNum].right = (m_nCorner[nNum][0] > m_nCorner[nNum][2]) ? m_nCorner[nNum][0] : m_nCorner[nNum][2];
			m_rcResult[nNum].top = (m_nCorner[nNum][1] < m_nCorner[nNum][7]) ? m_nCorner[nNum][1] : m_nCorner[nNum][7];
			m_rcResult[nNum].bottom = (m_nCorner[nNum][3] > m_nCorner[nNum][5]) ? m_nCorner[nNum][3] : m_nCorner[nNum][5];
			m_rcResult[nNum].NormalizeRect();
		}
	}

	return TRUE;
}

void CDecoderCortex::initResultData()
{
	m_nNumDecoded = 0;

	for (int i = 0; i < MAX_NUM_BARCODES; i++)
	{
		m_nDataLength[i] = 0;
		m_strType[i] = _T("");
		m_strData[i] = _T("");
		m_rcResult[i].SetRectEmpty();

		for (int j = 0; j < NUM_CORNERS * 2; ++j)
		{
			m_nCorner[i][j] = 0;
		}

		for (int j = 0; j < 2; ++j)
		{
			m_nCenter[i][j] = 0;
		}

		for (int j = 0; j < MAX_DATA_LENGTH + 1; ++j)
		{
			m_chOutBuf[i][j] = '\0';
		}
	}
}

#endif