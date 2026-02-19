// DevTabSystem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "APK.h"
#include "DevTabSystem.h"
#include "afxdialogex.h"

#include "APKView.h"
#include "VisionSystem.h"
#include "VisionInterfaces.h"
#include "SystemConfig.h"
#include "LanguageInfo.h"

#include "UIExt/ResourceManager.h"
#include <XUtil/xUtils.h>

#include <XGraphic\xGraphicObject.h>
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDevTabSystem 대화 상자입니다.
IMPLEMENT_DYNAMIC(CDevTabSystem, CDialog)

CDevTabSystem::CDevTabSystem(CWnd* pParent /*=NULL*/)
	: CDialog(CDevTabSystem::IDD, pParent)
{
	m_pMainView = NULL;

	m_Calibration.Clear();

	m_nExpiredDate = 180;
	m_nDiskCapacity = 60;
	m_strSendPath = _T("D:\\HITS\\");
}

CDevTabSystem::~CDevTabSystem()
{
}

void CDevTabSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,	IDC_LABEL_TITLE_SYSTEM,			m_wndLabelTitleSystem);

	DDX_Control(pDX,	IDC_LABEL_ITEM_X2,				m_wndLabelTitleX2);
	DDX_Control(pDX,	IDC_LABEL_ITEM_Y2,				m_wndLabelTitleY2);
	DDX_Control(pDX,	IDC_LABEL_ITEM_CAM1,			m_wndLabelTitleCam1);
	DDX_Control(pDX,	IDC_LABEL_ITEM_CAM2,			m_wndLabelTitleCam2);
	DDX_Control(pDX,	IDC_LABEL_ITEM_CAM3,			m_wndLabelTitleCam3);
	DDX_Control(pDX,	IDC_LABEL_ITEM_CAM4,			m_wndLabelTitleCam4);
	DDX_Control(pDX,	IDC_LABEL_ITEM_CAM5,			m_wndLabelTitleCam5);
	DDX_Control(pDX,	IDC_LABEL_ITEM_CAM6,			m_wndLabelTitleCam6);
	DDX_Control(pDX,	IDC_LABEL_ITEM_SERIAL,			m_wndLabelTitleSerial);

#ifdef RELEASE_1G
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X2,	m_Calibration.fCaliMeasure[IDX_AREA2][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y2,	m_Calibration.fCaliMeasure[IDX_AREA2][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X3,	m_Calibration.fCaliMeasure[IDX_AREA3][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y3,	m_Calibration.fCaliMeasure[IDX_AREA3][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X4,	m_Calibration.fCaliMeasure[IDX_AREA4][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y4,	m_Calibration.fCaliMeasure[IDX_AREA4][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X5,	m_Calibration.fCaliMeasure[IDX_AREA5][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y5,	m_Calibration.fCaliMeasure[IDX_AREA5][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X6,	m_Calibration.fCaliMeasure[IDX_AREA6][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y6,	m_Calibration.fCaliMeasure[IDX_AREA6][CalibrationDirY]);

	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM1,		m_CameraSerialNo.strSerialNo[CameraTypeLine]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM2,		m_CameraSerialNo.strSerialNo[CameraType3DArea]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM3,		m_CameraSerialNo.strSerialNo[CameraTypeArea]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM4,		m_CameraSerialNo.strSerialNo[CameraTypeLine2]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM5,		m_CameraSerialNo.strSerialNo[CameraTypeLine3]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM6,		m_CameraSerialNo.strSerialNo[CameraTypeLine4]);
	
#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X2,	m_Calibration.fCaliMeasure[IDX_AREA2][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y2,	m_Calibration.fCaliMeasure[IDX_AREA2][CalibrationDirY]);

	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM1,		m_CameraSerialNo.strSerialNo[CameraTypeArea]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM2,		m_CameraSerialNo.strSerialNo[CameraTypeArea2]);

#elif RELEASE_3G
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X2,	m_Calibration.fCaliMeasure[IDX_AREA2][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y2,	m_Calibration.fCaliMeasure[IDX_AREA2][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X3,	m_Calibration.fCaliMeasure[IDX_AREA3][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y3,	m_Calibration.fCaliMeasure[IDX_AREA3][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X4,	m_Calibration.fCaliMeasure[IDX_AREA4][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y4,	m_Calibration.fCaliMeasure[IDX_AREA4][CalibrationDirY]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X5,	m_Calibration.fCaliMeasure[IDX_AREA5][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y5,	m_Calibration.fCaliMeasure[IDX_AREA5][CalibrationDirY]);

	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM1,		m_CameraSerialNo.strSerialNo[CameraTypeArea]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM2,		m_CameraSerialNo.strSerialNo[CameraTypeArea2]);
	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM3,		m_CameraSerialNo.strSerialNo[CameraTypeArea3]);

#elif RELEASE_SG
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_X,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirX]);
	DDX_Text(pDX,		IDC_EDIT_CALI_MEASURE_LT_Y,		m_Calibration.fCaliMeasure[IDX_AREA1][CalibrationDirY]);

	DDX_Text(pDX,		IDC_EDIT_SERIAL_NO_CAM1,		m_CameraSerialNo.strSerialNo[IDX_AREA1]);

#endif

	DDX_Control(pDX,	IDC_YIELD_CHART,				m_wndYieldChart);
	DDX_Control(pDX,	IDC_CB_SAVE_LOG_PATH,			m_wndSaveLogPath);
	DDX_Text(pDX,		IDC_EDIT_EXPIRED_DATE,			m_nExpiredDate);
	DDV_MinMaxUInt(pDX,	m_nExpiredDate,					1, 365);
	DDX_Text(pDX,		IDC_EDIT_DISK_CAPACITY,			m_nDiskCapacity);
	DDV_MinMaxUInt(pDX,	m_nDiskCapacity,				10, 1000);
	DDX_Text(pDX,		IDC_EDIT_SEND_IMAGE_PATH,		m_strSendPath);

	DDX_Control(pDX,	IDC_CHK_ORIGINIMGSAVE,			m_chkOriginImgeSave);
	DDX_Control(pDX,	IDC_CHK_AUTODELETE,				m_chkAutoDelete	);
	DDX_Control(pDX,	IDC_CHK_ORIGINIMGSAVE_SEND,		m_chkOriginImgeSend	);
}


BEGIN_MESSAGE_MAP(CDevTabSystem, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_CB_SAVE_LOG_PATH,		&CDevTabSystem::OnCbnSelchangeCbSaveLogPath)
	ON_BN_CLICKED(IDC_CHK_ORIGINIMGSAVE,		&CDevTabSystem::OnBnClickedChkOriginimgsave)
	ON_BN_CLICKED(IDC_CHK_AUTODELETE,			&CDevTabSystem::OnBnClickedChkAutodelete)
	ON_BN_CLICKED(IDC_CHK_ORIGINIMGSAVE_SEND,	&CDevTabSystem::OnBnClickedChkOriginimgsaveSend)
END_MESSAGE_MAP()


// CDevTabSystem 메시지 처리기입니다.
BOOL CDevTabSystem::OnInitDialog()
{
	CDialog::OnInitDialog();

	UIExt::CResourceManager* pRes = UIExt::CResourceManager::Instance();
	DWORD dwLabelBodyColor = pRes->GetSideBarTitleBackgroundColor();
	DWORD dwLabelTextColor = pRes->GetSideBarTitleForegroundColor();

	int nFontHeight = 12;
	m_wndLabelTitleSystem.SetColor( dwLabelBodyColor, dwLabelTextColor );
	m_wndLabelTitleSystem.SetFontHeight( nFontHeight );

	DWORD dwLabelItemBodyColor = pRes->GetSideBarItemBackgroundColor();
	DWORD dwLabelItemTextColor = pRes->GetSideBarItemForegroundColor();

	m_wndLabelAreaCam.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelAreaCam.SetFontHeight( nFontHeight );

	m_wndLabelTitleX2.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleX2.SetFontHeight( nFontHeight );
	m_wndLabelTitleY2.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleY2.SetFontHeight( nFontHeight );

	m_wndLabelTitleCam1.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleCam1.SetFontHeight( nFontHeight );
	m_wndLabelTitleCam2.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleCam2.SetFontHeight( nFontHeight );
	m_wndLabelTitleCam3.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleCam3.SetFontHeight( nFontHeight );
	m_wndLabelTitleCam4.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleCam4.SetFontHeight( nFontHeight );
	m_wndLabelTitleCam5.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleCam5.SetFontHeight( nFontHeight );
	m_wndLabelTitleCam6.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleCam6.SetFontHeight( nFontHeight );

	m_wndLabelTitleSerial.SetColor( dwLabelItemBodyColor, dwLabelItemTextColor );
	m_wndLabelTitleSerial.SetFontHeight( nFontHeight );

	//UpdateUI(TRUE);
	Refresh();

	UpdateLanguage();

	return TRUE;
}

void CDevTabSystem::UpdateLanguage()
{
#ifndef RELEASE_6G
	GetDlgItem(IDC_STATIC_SAVE_PATH)->SetWindowText(CLanguageInfo::Instance()->ReadString(4));
	GetDlgItem(IDC_STATIC_EXPIRED_DATE)->SetWindowText(CLanguageInfo::Instance()->ReadString(5));
	GetDlgItem(IDC_STATIC_DISK_CAPACITY)->SetWindowText(CLanguageInfo::Instance()->ReadString(6));
	GetDlgItem(IDC_CHK_ORIGINIMGSAVE)->SetWindowText(CLanguageInfo::Instance()->ReadString(7));
	GetDlgItem(IDC_CHK_AUTODELETE)->SetWindowText(CLanguageInfo::Instance()->ReadString(8));
	GetDlgItem(IDC_CHK_ORIGINIMGSAVE_SEND)->SetWindowText(CLanguageInfo::Instance()->ReadString(9));

#else
	GetDlgItem(IDC_STATIC_SAVE_PATH)->SetWindowText(CLanguageInfo::Instance()->ReadString(94));
	GetDlgItem(IDC_STATIC_EXPIRED_DATE)->SetWindowText(CLanguageInfo::Instance()->ReadString(95));
	GetDlgItem(IDC_STATIC_DISK_CAPACITY)->SetWindowText(CLanguageInfo::Instance()->ReadString(96));
	GetDlgItem(IDC_CHK_ORIGINIMGSAVE)->SetWindowText(CLanguageInfo::Instance()->ReadString(97));
	GetDlgItem(IDC_CHK_AUTODELETE)->SetWindowText(CLanguageInfo::Instance()->ReadString(98));
	GetDlgItem(IDC_CHK_ORIGINIMGSAVE_SEND)->SetWindowText(CLanguageInfo::Instance()->ReadString(99));

#endif // !RELEASE_6G
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CDevTabSystem::IsModified()
{
	UpdateData(TRUE);
	
	return TRUE;
}

BOOL CDevTabSystem::Save()
{
	UpdateData(TRUE);

	CSystemConfig::Instance()->SetExpiredDate( m_nExpiredDate );
	CSystemConfig::Instance()->SetDiskCapacity( m_nDiskCapacity );

	CString str;
	int nCurSel = m_wndSaveLogPath.GetCurSel();
	m_wndSaveLogPath.GetLBText(nCurSel, str);
	CSystemConfig::Instance()->SetSaveLogPath(str);
	CSystemConfig::Instance()->SetSendPath( m_strSendPath );

	CSystemConfig::Instance()->SetOriginImgSave( m_chkOriginImgeSave.GetCheck() );
	CSystemConfig::Instance()->SetAutoDelete( m_chkAutoDelete.GetCheck() );
	CSystemConfig::Instance()->SetOriginImgSend(m_chkOriginImgeSend.GetCheck());

	CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
	CalibrationConfig = m_Calibration;

	CSystemConfig::CameraSerialNo& CameraSerialNo = CSystemConfig::Instance()->GetCameraSerialNo();
	CameraSerialNo = m_CameraSerialNo;

	//////////////////////////////////////////////////////////////////////////
	CSystemConfig::Instance()->Save();

	CSystemConfig::Instance()->Load();

	m_pMainView->SetImageViewerResoultion();

	// LogSvcMngr
	CString strName = GetExecuteDirectory() + _T("\\LogSvcMngr\\") + _T("Stop.bat");
	if( m_chkAutoDelete.GetCheck() )
		strName = GetExecuteDirectory() + _T("\\LogSvcMngr\\") + _T("Service.bat");

	ShellExecute(NULL, _T("open"), strName, NULL, NULL, SW_SHOW);

	return TRUE;
}

void CDevTabSystem::UpdateUI(BOOL bEnableCtrl)
{
	// UI
#ifdef RELEASE_1G
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X3 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y3 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X4 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y4 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X5 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y5 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X6 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y6 )->EnableWindow(bEnableCtrl);

	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM1 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM3 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM4 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM5 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM6 )->EnableWindow(bEnableCtrl);

#elif RELEASE_2G || RELEASE_4G || RELEASE_5G || RELEASE_6G
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X3 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y3 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X6 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y6 )->EnableWindow(FALSE);

	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM1 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM3 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM6 )->EnableWindow(FALSE);

#elif RELEASE_3G
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X3 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y3 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X4 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y4 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X5 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y5 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X6 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y6 )->EnableWindow(FALSE);

	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM1 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM2 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM3 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM6 )->EnableWindow(FALSE);

#elif RELEASE_SG
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X2 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y2 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X3 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y3 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_X6 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_CALI_MEASURE_LT_Y6 )->EnableWindow(FALSE);

	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM1 )->EnableWindow(bEnableCtrl);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM2 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM3 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM4 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM5 )->EnableWindow(FALSE);
	GetDlgItem( IDC_EDIT_SERIAL_NO_CAM6 )->EnableWindow(FALSE);
#endif
}

void CDevTabSystem::Refresh()
{
	m_nExpiredDate = CSystemConfig::Instance()->GetExpiredDate();
	m_nDiskCapacity = CSystemConfig::Instance()->GetDiskCapacity();
	m_strSendPath = CSystemConfig::Instance()->GetSendPath();

	//
	m_wndSaveLogPath.ResetContent();

	for( int i=0; i<nIMAGELOG_DRIVE_MAX; ++i )
	{
		CString str;
		str.Format(_T("%c"), 'A'+i);

		if( CVisionSystem::Instance()->CheckExistDrive( str ) )
			m_wndSaveLogPath.AddString( str );
	}

	if( !CVisionSystem::Instance()->CheckExistDrive( CSystemConfig::Instance()->GetSaveLogPath() ) )
	{
		CString strMsg;
		strMsg.Format( _T("Please Check ImageLog Drive...") );

		::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), strMsg, _T("Failure"), MB_OK|MB_ICONSTOP);
	}
	else
	{
		m_wndSaveLogPath.SelectString(0, CSystemConfig::Instance()->GetSaveLogPath() );

		UpdateDiskSpace( CSystemConfig::Instance()->GetSaveLogPath() );
	}

	m_chkOriginImgeSave.SetCheck(CSystemConfig::Instance()->GetOriginImgSave());
	m_chkAutoDelete.SetCheck(CSystemConfig::Instance()->GetAutoDelete());
	m_chkOriginImgeSend.SetCheck(CSystemConfig::Instance()->GetOriginImgSend());

	CSystemConfig::CalibrationConfig& CalibrationConfig = CSystemConfig::Instance()->GetCalibrationConfig();
	m_Calibration = CalibrationConfig;

	CSystemConfig::CameraSerialNo& CameraSerialNo = CSystemConfig::Instance()->GetCameraSerialNo();
	m_CameraSerialNo = CameraSerialNo;

	if (CSystemConfig::Instance()->GetAccessRight() == AccessRightProgrammer)
		UpdateUI(TRUE);
	else
		UpdateUI(FALSE);

	UpdateData(FALSE);
}

void CDevTabSystem::Cleanup()
{
}

HBRUSH CDevTabSystem::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	short nCtrlId = (short)pWnd->GetDlgCtrlID();
	switch (nCtrlId)
	{
	case IDC_STATIC:
	case IDC_STATIC_UNIT:
	case IDC_STATIC_MEASURE_CALIBRATION:
	case IDC_STATIC_ALIGN_CALIBRATION:
	case IDC_YIELD_CHART:
	case IDC_STATIC_TOTGB:
	case IDC_STATIC_AVAILGB:
	case IDC_STATIC_USEDGB:
	case IDC_STATIC_SAVE_PATH:
	case IDC_STATIC_EXPIRED_DATE:
	case IDC_STATIC_DISK_CAPACITY:
	case IDC_CHK_ORIGINIMGSAVE:
	case IDC_CHK_AUTODELETE:
	case IDC_CHK_ORIGINIMGSAVE_SEND:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
	}

	return hbr;
}

BOOL CDevTabSystem::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect( rc, RGB(255, 255, 255) );
	return TRUE;
}

void CDevTabSystem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		m_nExpiredDate = CSystemConfig::Instance()->GetExpiredDate();
		m_nDiskCapacity = CSystemConfig::Instance()->GetDiskCapacity();
		m_strSendPath = CSystemConfig::Instance()->GetSendPath();
	}
	else
	{
		Cleanup();
	}
}

void CDevTabSystem::UpdateDiskSpace( CString strDrive )
{
	double TotalBytes, FreeBytes;
	float fTotal, fAvail, fUsed;
	CString chkDirName = strDrive + _T(":");
	CString str;
	ULARGE_INTEGER uliUserFree, uliTotal, uliRealFree;

	if(GetDiskFreeSpaceEx(chkDirName, &uliUserFree, &uliTotal, &uliRealFree)) 
	{
		TotalBytes = uliTotal.LowPart+(double)uliTotal.HighPart*(double)4294967296;
		fTotal = (float)(TotalBytes/1024/1024/1024); // Convert (GB)     
		FreeBytes = uliRealFree.LowPart+(double)uliRealFree.HighPart*(double)4294967296;
		fAvail = (float)(FreeBytes/1024/1024/1024); // Convert (GB)
		fUsed = fTotal - fAvail;
	}

	str.Format(_T("%s\\ : %4.1f"), chkDirName, fTotal);	// total gb
	SetDlgItemText(IDC_STATIC_TOTGB, str);

	str.Format(_T("Free : %4.1f"), fAvail);				// free gb
	SetDlgItemText(IDC_STATIC_AVAILGB, str);

	str.Format(_T("Used : %4.1f"), fUsed);				// used gb
	SetDlgItemText(IDC_STATIC_USEDGB, str);

	m_wndYieldChart.SetYield(fUsed/fTotal);
}

void CDevTabSystem::OnCbnSelchangeCbSaveLogPath()
{
	int nCurSel = m_wndSaveLogPath.GetCurSel();

	CString strDrive;
	m_wndSaveLogPath.GetLBText(nCurSel, strDrive);

	if (nCurSel < 0)
		return;

	if( !CVisionSystem::Instance()->CheckExistDrive( strDrive ) )
	{
		CString strMsg;
		strMsg.Format( _T("Please Check ImageLog Drive...") );

		::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), strMsg, _T("Failure"), MB_OK|MB_ICONSTOP);
	}
	else
	{
		UpdateDiskSpace( strDrive );
	}
}

void CDevTabSystem::OnBnClickedChkOriginimgsave()
{
	WRITE_LOG( WL_BTN, _T("CDevTabSystem::OnBnClickedChkOriginimgsave") );
}

void CDevTabSystem::OnBnClickedChkAutodelete()
{
	WRITE_LOG( WL_BTN, _T("CDevTabSystem::OnBnClickedChkAutodelete") );
}

void CDevTabSystem::OnBnClickedChkOriginimgsaveSend()
{
	WRITE_LOG(WL_BTN, _T("CDevTabSystem::OnBnClickedChkOriginimgsaveSend"));
}

BOOL CDevTabSystem::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(msg);
}