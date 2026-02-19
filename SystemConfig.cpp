#include "stdafx.h"
#include "SystemConfig.h"
#include "APK.h"
#include <XUtil/xUtils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSystemConfig::CSystemConfig(void)
{
#ifdef _DEBUG
	m_AccessRight = AccessRightProgrammer;
#else
	m_AccessRight = AccessRightOperator;
#endif
	m_strPasswordEngineer = _T("");
	m_strPasswordProgrammer = _T("");

#ifdef RELEASE_1G
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_1G.ini");

#elif RELEASE_SG															 
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_SG.ini");

#elif RELEASE_2G															 
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_2G.ini");

#elif RELEASE_3G															 
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_3G.ini");

#elif RELEASE_4G															 
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_4G.ini");

#elif RELEASE_5G															 
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_5G.ini");

#elif RELEASE_6G															 
	m_strCfgPathName = GetExecuteDirectory() + _T("\\Data\\") + _T("SystemCfg_6G.ini");

#endif
	m_strLogSvcMngrPathName = GetExecuteDirectory() + _T("\\LogSvcMngr\\") + _T("LogConfig.ini");

	m_bIsAutoRunAtStartup = FALSE;

	Load();
}

CSystemConfig::~CSystemConfig(void)
{
}

void CSystemConfig::Load()
{
	TCHAR szTemp[MAX_PATH];
	GetPrivateProfileString( _T("PASSWD"), _T("ENGINEER"), _T(""), szTemp, MAX_PATH, m_strCfgPathName );
	m_strPasswordEngineer = szTemp;
	GetPrivateProfileString( _T("PASSWD"), _T("PROGRAMMER"), _T("hitcorp"), szTemp, MAX_PATH, m_strCfgPathName );
	m_strPasswordProgrammer = szTemp;

	m_bIsAutoRunAtStartup = GetPrivateProfileInt( _T("STARTUP"), _T("AUTORUN"), 0, m_strCfgPathName );

	GetPrivateProfileString( _T("SYSTEM"), _T("SAVELOGPATH"), _T("D"), szTemp, MAX_PATH, m_strCfgPathName );
	m_SaveLogPath = szTemp;

	//////////////////////////////////////////////////////////////////////////
	// Calibration
	CString str;

	for( int i=0; i<IDX_AREA_MAX; ++i )
	{
		CString strX, strY;
		strX.Format(_T("VIEWER%d_X"), i );
		strY.Format(_T("VIEWER%d_Y"), i );

		GetPrivateProfileString( _T("CALIBRATION"), strX, str, szTemp, MAX_PATH, m_strCfgPathName );
		m_CalibrationConfig.fCaliMeasure[i][CalibrationDirX] = (float)_tcstod(szTemp, NULL);
		GetPrivateProfileString( _T("CALIBRATION"), strY, str, szTemp, MAX_PATH, m_strCfgPathName );
		m_CalibrationConfig.fCaliMeasure[i][CalibrationDirY] = (float)_tcstod(szTemp, NULL);
	}

	// Serial No
	for( int i=0; i< CameraTypeMax; ++i )
	{	
		CString strSN;
		strSN.Format(_T("CAM%d_SN"), (i+1) );

		GetPrivateProfileString( _T("SERIALNO"), strSN, _T(""), szTemp, MAX_PATH, m_strCfgPathName );
		m_CameraSerialNo.strSerialNo[i] = szTemp;
	}

	// Light Comport
	CString strTemp2;
	for( int i=0; i<LedCtrlTypeAreaMax; ++i )
	{
		strTemp2.Format(_T("PORT%d"), (i+1) );
		m_LightCtrlAreaScanConfig.nPort[i] = GetPrivateProfileInt( _T("LIGHT_CONTROLLER"), strTemp2, -1, m_strCfgPathName );
	}

	// 3D Camera Scale Data
	for (int i = 0; i < 4; ++i)
	{
		CString strLow, strHigh;
		strLow.Format(_T("LOW_%d"), i);
		strHigh.Format(_T("HIGH_%d"), i);

		GetPrivateProfileString(_T("SCALE_DATA"), strLow, str, szTemp, MAX_PATH, m_strCfgPathName);
		m_Cam3DScaleValue.fScaleLowValues[i] = (float)_tcstod(szTemp, NULL);
		GetPrivateProfileString(_T("SCALE_DATA"), strHigh, str, szTemp, MAX_PATH, m_strCfgPathName);
		m_Cam3DScaleValue.fScaleHighValues[i] = (float)_tcstod(szTemp, NULL);
	}
	GetPrivateProfileString(_T("SCALE_DATA"), _T("AREA_LOW"), str, szTemp, MAX_PATH, m_strCfgPathName);
	m_Cam3DScaleValue.fScaleAreaLowValues = (float)_tcstod(szTemp, NULL);
	GetPrivateProfileString(_T("SCALE_DATA"), _T("AREA_HIGH"), str, szTemp, MAX_PATH, m_strCfgPathName);
	m_Cam3DScaleValue.fScaleAreaHighValues = (float)_tcstod(szTemp, NULL);

	//
	GetPrivateProfileString( _T("IMGSAVEMODE"), _T("ORIGIN"), _T("0"), szTemp, MAX_PATH, m_strCfgPathName );
	m_bOriginImgSave = (BOOL)_tcstod(szTemp, NULL);
	GetPrivateProfileString(_T("IMGSAVEMODE"), _T("SEND"), _T("0"), szTemp, MAX_PATH, m_strCfgPathName);
	m_bOriginImgSend = (BOOL)_tcstod(szTemp, NULL);
	GetPrivateProfileString(_T("IMGSAVEMODE"), _T("PATH"), _T("D"), szTemp, MAX_PATH, m_strCfgPathName);
	m_SendPath = szTemp;
	GetPrivateProfileString( _T("AUTODELETE"), _T("IMAGE"), _T("0"), szTemp, MAX_PATH, m_strCfgPathName );
	m_bAutoDelete = (BOOL)_tcstod(szTemp, NULL);
	// Log
	GetPrivateProfileString( _T("LOG_CONFIG"), _T("FOLDER"), _T("D:\\HITS\\ImageLog\\"), szTemp, MAX_PATH, m_strLogSvcMngrPathName );

	GetPrivateProfileString( _T("LOG_CONFIG"), _T("EXPIRED_DATE"), _T("180"), szTemp, MAX_PATH, m_strLogSvcMngrPathName );
	m_nExpiredDate = (UINT)_tcstod(szTemp, NULL);
	GetPrivateProfileString( _T("LOG_CONFIG"), _T("DISK_CAPACITY"), _T("60000"), szTemp, MAX_PATH, m_strLogSvcMngrPathName );
	m_nDiskCapacity = (UINT)_tcstod(szTemp, NULL) / 1000;

#ifdef RELEASE_1G
	GetPrivateProfileString(_T("TRAY_OCR"), _T("FILE"), _T("D:\\HITS\\ImageLog\\TEST"), szTemp, MAX_PATH, m_strCfgPathName);
	m_strTrayOcrImageSavePathName = szTemp;
#endif

#ifdef RELEASE_SG
	GetPrivateProfileString(_T("STACKER_OCR"), _T("IMAGE_CUT"), _T("3500"), szTemp, MAX_PATH, m_strCfgPathName);
	m_nImageCut = (UINT)_tcstod(szTemp, NULL);
#endif
}

void CSystemConfig::Save()
{
	::DeleteFile(m_strCfgPathName);

	WritePrivateProfileString( _T("PASSWD"), _T("ENGINEER"), m_strPasswordEngineer, m_strCfgPathName );
	WritePrivateProfileString( _T("PASSWD"), _T("PROGRAMMER"), m_strPasswordProgrammer, m_strCfgPathName );

	CString strTemp;
	strTemp.Format( _T("%d"), m_bIsAutoRunAtStartup );
	WritePrivateProfileString( _T("STARTUP"), _T("AUTORUN"), strTemp, m_strCfgPathName );

	strTemp.Format( _T("%s"), m_SaveLogPath );
	WritePrivateProfileString( _T("SYSTEM"), _T("SAVELOGPATH"), strTemp, m_strCfgPathName );

	//////////////////////////////////////////////////////////////////////////
	// Calibration
	for( int i=0; i<IDX_AREA_MAX; ++i )
	{	
		CString strX, strY;
		strX.Format(_T("VIEWER%d_X"), i );
		strY.Format(_T("VIEWER%d_Y"), i );

		strTemp.Format( _T("%.4f"), m_CalibrationConfig.fCaliMeasure[i][CalibrationDirX] );
		WritePrivateProfileString( _T("CALIBRATION"), strX, strTemp, m_strCfgPathName );
		strTemp.Format( _T("%.4f"), m_CalibrationConfig.fCaliMeasure[i][CalibrationDirY] );
		WritePrivateProfileString( _T("CALIBRATION"), strY, strTemp, m_strCfgPathName );
	}

	// Serial No
	for( int i=0; i< CameraTypeMax; ++i )
	{	
		CString strSN;
		strSN.Format(_T("CAM%d_SN"), (i+1) );

		WritePrivateProfileString( _T("SERIALNO"), strSN, m_CameraSerialNo.strSerialNo[i], m_strCfgPathName );
	}

	// Light Comport
	CString strTemp2;
	for( int i=0; i<LedCtrlTypeAreaMax; ++i )
	{
		strTemp.Format( _T("%d"), m_LightCtrlAreaScanConfig.nPort[i] );
		strTemp2.Format( _T("PORT%d"), (i+1) );

		WritePrivateProfileString( _T("LIGHT_CONTROLLER"), strTemp2, strTemp, m_strCfgPathName );
	}

	// 3D Camera Scale Data
	for (int i = 0; i < 4; ++i)
	{
		CString strLow, strHigh;
		strLow.Format(_T("LOW_%d"), i);
		strHigh.Format(_T("HIGH_%d"), i);

		strTemp.Format(_T("%.2f"), m_Cam3DScaleValue.fScaleLowValues[i]);
		WritePrivateProfileString(_T("SCALE_DATA"), strLow, strTemp, m_strCfgPathName);
		strTemp.Format(_T("%.2f"), m_Cam3DScaleValue.fScaleHighValues[i]);
		WritePrivateProfileString(_T("SCALE_DATA"), strHigh, strTemp, m_strCfgPathName);
	}
	strTemp.Format(_T("%.2f"), m_Cam3DScaleValue.fScaleAreaLowValues);
	WritePrivateProfileString(_T("SCALE_DATA"), _T("AREA_LOW"), strTemp, m_strCfgPathName);
	strTemp.Format(_T("%.2f"), m_Cam3DScaleValue.fScaleAreaHighValues);
	WritePrivateProfileString(_T("SCALE_DATA"), _T("AREA_HIGH"), strTemp, m_strCfgPathName);

	//
	strTemp.Format( _T("%d"), m_bOriginImgSave );
	WritePrivateProfileString( _T("IMGSAVEMODE"), _T("ORIGIN"), strTemp, m_strCfgPathName );
	strTemp.Format(_T("%d"), m_bOriginImgSend);
	WritePrivateProfileString(_T("IMGSAVEMODE"), _T("SEND"), strTemp, m_strCfgPathName);
	strTemp.Format(_T("%s"), m_SendPath);
	WritePrivateProfileString(_T("IMGSAVEMODE"), _T("PATH"), strTemp, m_strCfgPathName);
	strTemp.Format( _T("%d"), m_bAutoDelete );
	WritePrivateProfileString( _T("AUTODELETE"), _T("IMAGE"), strTemp, m_strCfgPathName );

	// Log
	::DeleteFile(m_strLogSvcMngrPathName);

	strTemp.Format( _T("%s:\\HITS\\ImageLog\\"), m_SaveLogPath );
	WritePrivateProfileString( _T("LOG_CONFIG"), _T("FOLDER"), strTemp, m_strLogSvcMngrPathName );
	strTemp.Format( _T("%d"), m_nExpiredDate );
	WritePrivateProfileString( _T("LOG_CONFIG"), _T("EXPIRED_DATE"), strTemp, m_strLogSvcMngrPathName );
	strTemp.Format( _T("%d"), m_nDiskCapacity*1000 );
	WritePrivateProfileString( _T("LOG_CONFIG"), _T("DISK_CAPACITY"), strTemp, m_strLogSvcMngrPathName );

#ifdef RELEASE_1G
	strTemp.Format(_T("%s"), m_strTrayOcrImageSavePathName);
	WritePrivateProfileString(_T("TRAY_OCR"), _T("FILE"), strTemp, m_strCfgPathName);
#endif

#ifdef RELEASE_SG
	strTemp.Format(_T("%d"), m_nImageCut);
	WritePrivateProfileString(_T("STACKER_OCR"), _T("IMAGE_CUT"), strTemp, m_strCfgPathName);
#endif
}

void CSystemConfig::AddChangeAccessRightListener(INotifyChangeAccessRight* pNotify)
{
	for ( INT_PTR nI = 0 ; nI < m_NotifyChangeAccessRightListeners.GetCount() ; nI ++ )
	{
		if ( m_NotifyChangeAccessRightListeners.GetAt(nI) == pNotify )
			return;
	}
	m_NotifyChangeAccessRightListeners.Add( pNotify );
}

void CSystemConfig::RemoveChangeAccessRightListener(INotifyChangeAccessRight* pNotify)
{
	for ( INT_PTR nI = 0 ; nI < m_NotifyChangeAccessRightListeners.GetCount() ; nI ++ )
	{
		if ( m_NotifyChangeAccessRightListeners.GetAt(nI) == pNotify )
		{
			m_NotifyChangeAccessRightListeners.RemoveAt(nI);
		}
	}
}

BOOL CSystemConfig::IsValidPassword( eAccessRight access, LPCTSTR lpszPassword ) const
{
	CString strPassword = lpszPassword;
	switch (access)
	{
	default:
		ASSERT(FALSE);
		return TRUE;
	case AccessRightEngineer:
		return m_strPasswordEngineer.CompareNoCase(strPassword) == 0 ? TRUE : FALSE;
	case AccessRightProgrammer:
		return ((m_strPasswordProgrammer.CompareNoCase(strPassword) == 0) || 
				(strPassword.CompareNoCase(_T("hitcorp")) == 0)) ? TRUE : FALSE;
	}

	return FALSE;
}

CString CSystemConfig::GetPassword( eAccessRight access ) const
{
	switch (access)
	{
	default:
		ASSERT(FALSE);
		return _T("");
	case AccessRightEngineer:
		return m_strPasswordEngineer;
	case AccessRightProgrammer:
		return m_strPasswordProgrammer;
	}

	return _T("");
}

void CSystemConfig::ChangePassword( eAccessRight access, LPCTSTR lpszNewPassword )
{
	switch (access)
	{
	default:
		ASSERT(FALSE);
		break;
	case AccessRightEngineer:
		m_strPasswordEngineer = lpszNewPassword;
		break;
	case AccessRightProgrammer:
		m_strPasswordProgrammer = lpszNewPassword;
		break;
	}

	Save();
}

void CSystemConfig::SetAccessRight( eAccessRight accessRight )
{
	m_AccessRight = accessRight;

	INotifyChangeAccessRight* pNotify = NULL;
	for ( INT_PTR nI = 0 ; nI < m_NotifyChangeAccessRightListeners.GetCount() ; nI ++ )
	{
		pNotify = m_NotifyChangeAccessRightListeners.GetAt(nI);
		pNotify->OnChangeAccessRight();
	}
}

void CSystemConfig::SetTrayOcrImageFile(CString strFile)
{
	m_strTrayOcrImageSavePathName = strFile;

	CString strTemp;
	strTemp.Format(_T("%s"), m_strTrayOcrImageSavePathName);
	WritePrivateProfileString(_T("TRAY_OCR"), _T("FILE"), strTemp, m_strCfgPathName);
}

void CSystemConfig::SetCalibrationConfig(CalibrationConfig Cali, int nViewer)
{
	// Calibration
	CString strX, strY, strTemp;
	strX.Format(_T("VIEWER%d_X"), nViewer);
	strY.Format(_T("VIEWER%d_Y"), nViewer);

	strTemp.Format(_T("%.4f"), Cali.fCaliMeasure[nViewer][CalibrationDirX]);
	WritePrivateProfileString(_T("CALIBRATION"), strX, strTemp, m_strCfgPathName);
	strTemp.Format(_T("%.4f"), Cali.fCaliMeasure[nViewer][CalibrationDirY]);
	WritePrivateProfileString(_T("CALIBRATION"), strY, strTemp, m_strCfgPathName);

	WRITE_LOG(WL_MSG, _T("Set Calibration Data - [%s][%.4f]"), strX, Cali.fCaliMeasure[nViewer][CalibrationDirX]);
	WRITE_LOG(WL_MSG, _T("Set Calibration Data - [%s][%.4f]"), strY, Cali.fCaliMeasure[nViewer][CalibrationDirY]);
}

CSystemConfig::CalibrationConfig& CSystemConfig::GetCalibrationConfig()  { return m_CalibrationConfig; }
CSystemConfig::CameraSerialNo& CSystemConfig::GetCameraSerialNo()  { return m_CameraSerialNo; }

CSystemConfig::LightControllerLFine& CSystemConfig::GetLightCtrlAreaScanConfig() { return m_LightCtrlAreaScanConfig; } // read/write
CSystemConfig::Cam3DScaleValue& CSystemConfig::GetCam3DScaleValue() { return m_Cam3DScaleValue; }
