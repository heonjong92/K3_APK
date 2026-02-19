#pragma once
#include "Common/xSingletoneObject.h"
#include <XGraphic/xDataTypes.h>
#include <vector>

#define LIGHTINFO_VERSION		0x0100			// 내용 변경 시 버전 업 필요	0x0101 -> 1.01

class CLightInfo : public CxSingleton<CLightInfo>
{
public:
	struct LightControllerLFine
	{
		BYTE cLightValues[USE_LFINE_LCP100_CTRL_CH_MAX][LedCtrlTypeAreaMax];

		float fScaleLowValues[4];
		float fScaleHighValues[4];

		void Clear()
		{
			for( int i=0; i<USE_LFINE_LCP100_CTRL_CH_MAX; ++i )
			{
				for( int j=0; j<LedCtrlTypeAreaMax; ++j )
				{
					cLightValues[i][j] = 0;
				}

				fScaleLowValues[i] = 0.f;
				fScaleHighValues[i] = 255.2f;
			}
		}
	};

public:
	CLightInfo(void);
	~CLightInfo(void);

private:
	DWORD			m_dwLightInfoVersion;
	CString			m_strModelName;

	LightControllerLFine		m_LightCtrlAreaScanConfig;

public:
	void New( LPCTSTR lpszModelName );
	BOOL Load( LPCTSTR lpszModelName );
	void Delete( LPCTSTR lpszModelName );

	BOOL Save();
	void SetEmpty();
	BOOL IsValid();

	CLightInfo::LightControllerLFine& GetLightCtrlAreaScanConfig();

private:
	void LoadLightInfo( LPCTSTR lpszLightInfoFile );
	void SaveLightInfo( LPCTSTR lpszLightInfoFile );
};


