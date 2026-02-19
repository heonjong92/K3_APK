#ifndef __I_DEFECT_INFO_H__
#define __I_DEFECT_INFO_H__

class CxImageObject;
typedef struct _LOGDEFECTINFO
{
	BYTE*			pImgObj;
	int				nImgWidth;
	int				nImgHieght;
	int				nViewIndex;
	BOOL			bIsCF;
	int				x, y;
	int				nOffsetY;
	int				nWidth;
	int				nHeight;
	int				nArea;
	float			fDefectPositionX;
	float			fDefectPositionY;
	LPCTSTR			lpszText;
	int				nGraphicIndex;
	int				nSurfaceIndex;
	BOOL			bStdErr;
	BYTE*			pbtImgBuffer;
} LOGDEFECTINFO;

class IDefectInfo
{
public:
	virtual void ClearDefectInfo( int nIndex ) = 0;
	virtual void AddDefectInfo( BYTE* pImgObj, int nImgWidth, int nImgHeight, int nIndex, BOOL bIsCF, int x, int y, int nOffsetY, 
								int nWidth, int nHeight, int nArea, float fDefectPositionX, float fDefectPositionY, 
								LPCTSTR lpszText, int nGraphicIndex, int nSurfaceIndex, BOOL bStdErr ) = 0;
	virtual void AddDefectInfoIndirect( LOGDEFECTINFO& lDefectInfo ) = 0;
};


#endif