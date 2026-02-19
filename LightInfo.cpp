#include "stdafx.h"
#include "LightInfo.h"
#include "APK.h"
#include "ModelInfo.h"

#include "SystemConfig.h"

#include <XUtil/xUtils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef MAX_PATH
#undef MAX_PATH
#define MAX_PATH	(4096)
#endif

CLightInfo::CLightInfo(void)
{
	m_LightCtrlAreaScanConfig.Clear();
}


CLightInfo::~CLightInfo(void)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void CLightInfo::New(LPCTSTR lpszModelName)
{
	m_strModelName = lpszModelName;
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME"), lpszModelName);
}

BOOL CLightInfo::Save()
{
	m_strModelName = CModelInfo::Instance()->GetModelName();
	CString strLightPath = CModelInfo::Instance()->GetRecipePath(m_strModelName);
	MakeDirectory((LPCTSTR)strLightPath);
	CString strLightInfoFile = strLightPath;
	strLightInfoFile += _T("LightInfo.ini");

	return TRUE;
}

BOOL CLightInfo::Load(LPCTSTR lpszModelName)
{
	CString strRecipePath    = CModelInfo::Instance()->GetRecipePath(lpszModelName);
	CString strLightInfoFile = strRecipePath;
	strLightInfoFile += _T("LightInfo.ini");

	if(!IsExistFile((LPCTSTR)strLightInfoFile))
		return FALSE;

	return TRUE;
}

void CLightInfo::SaveLightInfo(LPCTSTR lpszLightInfoFile)
{
	CString strLightInfoFile = lpszLightInfoFile;
	CString strTemp;

	::DeleteFile(strLightInfoFile);

	//////////////////////////////////////////////////////////////////////////
	CString strTemp2;
	for( int i=0; i<LedCtrlTypeAreaMax; ++i )
	{
		strTemp2.Format(_T("LIGHT CONTROLLER %d"), (i+1) );
		strTemp.Format( _T("%d"), m_LightCtrlAreaScanConfig.cLightValues[USE_LFINE_LCP100_CTRL_CH1][i] );
		WritePrivateProfileString( strTemp2, _T("CH1"), strTemp, strLightInfoFile );
		strTemp.Format( _T("%d"), m_LightCtrlAreaScanConfig.cLightValues[USE_LFINE_LCP100_CTRL_CH2][i] );
		WritePrivateProfileString( strTemp2, _T("CH2"), strTemp, strLightInfoFile );
	}
}

void CLightInfo::LoadLightInfo( LPCTSTR lpszLightInfoFile )
{
	CString strLightInfoFile = lpszLightInfoFile;
	TCHAR szTemp[MAX_PATH];
	CString strItem;

	//////////////////////////////////////////////////////////////////////////
	CString strTemp2;
	for( int i=0; i<LedCtrlTypeAreaMax; ++i )
	{
		strTemp2.Format(_T("LIGHT CONTROLLER %d"), (i+1) );
		::GetPrivateProfileString( strTemp2, _T("CH1"), NULL, szTemp, MAX_PATH, strLightInfoFile );
		m_LightCtrlAreaScanConfig.cLightValues[USE_LFINE_LCP100_CTRL_CH1][i] = (int)_tcstod( szTemp, NULL );
		::GetPrivateProfileString( strTemp2, _T("CH2"), NULL, szTemp, MAX_PATH, strLightInfoFile );
		m_LightCtrlAreaScanConfig.cLightValues[USE_LFINE_LCP100_CTRL_CH2][i] = (int)_tcstod( szTemp, NULL );
	}
}

void CLightInfo::Delete( LPCTSTR lpszModelName )
{
	CString strLightPath = CModelInfo::Instance()->GetRecipePath(m_strModelName);

	DeleteDirectory( (LPCTSTR)strLightPath );
	SetEmpty();
}

void CLightInfo::SetEmpty()
{
	m_strModelName = _T("");
	AfxGetApp()->WriteProfileString(_T("RECIPE"), _T("MODEL_NAME"), m_strModelName);
}

BOOL CLightInfo::IsValid()
{
	return m_strModelName.IsEmpty() ? FALSE : TRUE;
}

CLightInfo::LightControllerLFine& CLightInfo::GetLightCtrlAreaScanConfig() { return m_LightCtrlAreaScanConfig; } // read/write