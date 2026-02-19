#pragma once


#include <XUtil/xUtils.h>
#include <XGraphic/xDataTypes.h>

#ifndef _CodeUsb

#define CORTEXDECODER_DLL
#include "crdDefs.h"

#pragma comment (lib , "CortexDecoderHasp.lib")

class CxGraphicObject;
class CxImageObject;

#define MAX_DATA_LENGTH		1000
#define MAX_NUM_BARCODES 	20
#define NUM_CORNERS			4

class CDecoderCortex
{
public:
	CDecoderCortex(void);
	~CDecoderCortex(void);

private:
	static CDecoderCortex* m_pThis;

public:
	static CDecoderCortex* GetPtr() {	return m_pThis;	}

public:
	int InitializeDecoder();
	int FreeDecoderHandle();
	int DecodeImage(CxImageObject& pMaskingImgObj);
	char* getSymbName(int nSymbolType);
	BOOL GetResultData(int nNum);
	void initResultData();

protected:
	int decoderHandle;
	int m_nDataLength[MAX_NUM_BARCODES];
	int m_nCorner[MAX_NUM_BARCODES][NUM_CORNERS * 2];				// each corner has two integers for x and y coordinates
	int m_nCenter[MAX_NUM_BARCODES][2];								// The center point has two integers for x and y coordinates
	char m_chOutBuf[MAX_NUM_BARCODES][MAX_DATA_LENGTH + 1];			// make sure this is large enough for your data

public:	// °á°ú
	int m_nNumDecoded;
	CString m_strType[MAX_NUM_BARCODES];
	CString m_strData[MAX_NUM_BARCODES];
	CRect m_rcResult[MAX_NUM_BARCODES];
};


#endif
