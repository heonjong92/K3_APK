#pragma once

#include "Common/xSingletoneObject.h"

class CSystemConfig : public CxSingleton<CSystemConfig>
{
public:
	class INotifyChangeAccessRight
	{
	public:
		INotifyChangeAccessRight();
		virtual ~INotifyChangeAccessRight();
		virtual void OnChangeAccessRight() = 0;
	};

friend class INotifyChangeAccessRight;
public:
	struct LightControllerLFine
	{
		UINT nPort[LedCtrlTypeAreaMax];
	};

	struct CalibrationConfig
	{
		float fCaliMeasure[IDX_AREA_MAX][CalibrationDirMax]; // [Cam][X, Y]

		void Clear()
		{
			for( int i=0; i<IDX_AREA_MAX; ++i )
			{
				for( int j=0; j<CalibrationDirMax; ++j )
				{
					fCaliMeasure[i][j] = 0;
				}
			}
		}
	};

	struct CameraSerialNo
	{
		CString strSerialNo[CameraTypeMax];

		void Clear()
		{
			for( int i=0; i< CameraTypeMax; ++i )
			{
				strSerialNo[i] = _T("");
			}
		}
	};

	struct Cam3DScaleValue
	{
		float fScaleLowValues[4];
		float fScaleHighValues[4];
		
		float fScaleAreaLowValues;
		float fScaleAreaHighValues;

		void Clear()
		{
			for (int i = 0; i < 4; ++i)
			{
				fScaleLowValues[i] = 0.f;
				fScaleHighValues[i] = 255.f;
			}

			fScaleAreaLowValues = 0.f;
			fScaleAreaHighValues = 1024.f;
		}
	};

protected:
	CString		m_strCfgPathName;
	CString		m_strLogSvcMngrPathName;

	BOOL		m_bOriginImgSave;
	BOOL		m_bAutoDelete;
	BOOL		m_bOriginImgSend;

	CString		m_SaveLogPath;
	UINT		m_nExpiredDate;
	UINT		m_nDiskCapacity;
	CString		m_SendPath;
	
	CString		m_strTrayOcrImageSavePathName;

	UINT		m_nImageCut;
	//////////////////////////////////////////////////////////////////////////
	eAccessRight m_AccessRight;
	CString		m_strPasswordEngineer;
	CString		m_strPasswordProgrammer;
	BOOL		m_bIsAutoRunAtStartup;

	CalibrationConfig			m_CalibrationConfig;
	CameraSerialNo				m_CameraSerialNo;

	LightControllerLFine		m_LightCtrlAreaScanConfig;
	Cam3DScaleValue 		m_Cam3DScaleValue;

	CArray<INotifyChangeAccessRight*>	m_NotifyChangeAccessRightListeners;

	void AddChangeAccessRightListener(INotifyChangeAccessRight* pNotify);
	void RemoveChangeAccessRightListener(INotifyChangeAccessRight* pNotify);

public:
	void Load();
	void Save();

	eAccessRight GetAccessRight() const { return m_AccessRight; }
	void SetAccessRight( eAccessRight accessRight );

	BOOL IsValidPassword( eAccessRight access, LPCTSTR lpszPassword ) const;
	void ChangePassword( eAccessRight access, LPCTSTR lpszNewPassword );
	CString GetPassword( eAccessRight access ) const;

	BOOL IsAutoRunAtStartup() const { return m_bIsAutoRunAtStartup; }
	void SetAutoRunAtStartup(BOOL bCheck) { m_bIsAutoRunAtStartup = bCheck; }

	void SetOriginImgSave( BOOL bOn ) { m_bOriginImgSave = bOn; }
	BOOL GetOriginImgSave() const { return m_bOriginImgSave; }

	void SetAutoDelete( BOOL bOn ) { m_bAutoDelete = bOn; }
	BOOL GetAutoDelete() const { return m_bAutoDelete; }

	void SetOriginImgSend(BOOL bOn) { m_bOriginImgSend = bOn; }
	BOOL GetOriginImgSend() const { return m_bOriginImgSend; }

	CString GetSaveLogPath() const { return m_SaveLogPath; }
	void SetSaveLogPath( CString strSaveLogPath ) { m_SaveLogPath = strSaveLogPath;}

	UINT GetExpiredDate() const { return m_nExpiredDate; }
	void SetExpiredDate( int nExpiredDate )	{ m_nExpiredDate = nExpiredDate; }

	UINT GetDiskCapacity() const { return m_nDiskCapacity; }
	void SetDiskCapacity( int nDiskCapacity )	{ m_nDiskCapacity = nDiskCapacity; }

	CString GetSendPath() const { return m_SendPath; }
	void SetSendPath(CString strSendPath) { m_SendPath = strSendPath; }

	CString GetTrayOcrImageFile() const { return m_strTrayOcrImageSavePathName; }
	void SetTrayOcrImageFile(CString strFile);

	UINT GetImageCut() const { return m_nImageCut; }
	void SetImageCut(int nImageCut) { m_nImageCut = nImageCut; }

	void SetCalibrationConfig(CalibrationConfig Cali, int nViewer);
	CSystemConfig::CalibrationConfig& GetCalibrationConfig();
	CSystemConfig::CameraSerialNo& GetCameraSerialNo();
	CSystemConfig::LightControllerLFine& GetLightCtrlAreaScanConfig();
	CSystemConfig::Cam3DScaleValue& GetCam3DScaleValue();

public:
	CSystemConfig(void);
	virtual ~CSystemConfig(void);
};

