
// APK.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "APK.h"
#include "MainFrm.h"

#include "APKDoc.h"
#include "APKView.h"
#include "SystemConfig.h"

#include <XUtil/xUtils.h>
#include <XUtil/Comm/xSerialComm.h>
#include <CrashRpt.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CAPKApp

BEGIN_MESSAGE_MAP(CAPKApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAPKApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CAPKApp 생성

CAPKApp::CAPKApp() : m_LogSystem( _T("HITS@APK") ), m_hMutexOneInstance(NULL)
{
	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("HITS.APK.Ver1"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CAPKApp 개체입니다.

CAPKApp theApp;

CEvent g_eventForWatch;


// CAPKApp 초기화
long GetRoundLong( double dbScr )
{
	long lTemp;

	if(dbScr<=(-0.5))		lTemp = (long)(dbScr - 0.5f);
	else if(dbScr>=(0.5))	lTemp = (long)(dbScr + 0.5f);
	else if(dbScr>(-0.5) && dbScr<(0.5)) lTemp = 0;

	return lTemp;
}

short GetRoundShort( double dbScr )
{
	short sTemp;

	if(dbScr<=(-0.5))		sTemp = (short)(dbScr - 0.5f);
	else if(dbScr>=(0.5))	sTemp = (short)(dbScr + 0.5f);
	else if(dbScr>(-0.5) && dbScr<(0.5)) sTemp = 0;

	return sTemp;
}

float GetRoundFloat(double dbScr)
{
	float fTemp;

	if (dbScr <= (-0.5))		fTemp = (float)((long)(dbScr - 0.5f));
	else if (dbScr >= (0.5))	fTemp = (float)((long)(dbScr + 0.5f));
	else if (dbScr > (-0.5) && dbScr < (0.5)) fTemp = 0;

	return fTemp;
}

LPCTSTR GetImageViewerTitle(eCamType nCamType, UINT nIndex)
{
	if(nCamType == CamTypeAreaScan)
	{
		switch (nIndex)
		{
#ifdef RELEASE_1G
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: Tray Tab Ocr"				);
//		case IDX_AREA2:		return _T("[VIEWER2][CAM2] :: Tray Chip Cnt 1"			); // 유저가 못봄. Viewer Index 다음 번호로 미루자.
		case IDX_AREA3:		return _T("[VIEWER2][CAM2] :: Tray Chip Cnt"			);
		case IDX_AREA4:		return _T("[VIEWER3][CAM3] :: Tray Chip"				);
		case IDX_AREA5:		return _T("[VIEWER4][CAM4] :: Tray Mixing"				);
		case IDX_AREA6:		return _T("[VIEWER5][CAM5] :: Tray Lift Front"			);
		case IDX_AREA7:		return _T("[VIEWER6][CAM6] :: Tray Lift Rear"			);
		
#elif RELEASE_SG
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: Stacker Tray Tab Ocr"		);
		
#elif RELEASE_2G
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: Tray Banding"				);
		case IDX_AREA2:		return _T("[VIEWER2][CAM2] :: HIC"						);
		
#elif RELEASE_3G
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: Desiccant Position Left"	);
		case IDX_AREA2:		return _T("[VIEWER2][CAM2] :: Desiccant Position Right"	);
		case IDX_AREA3:		return _T("[VIEWER3][CAM1] :: Desiccant Cutting Left"	);
		case IDX_AREA4:		return _T("[VIEWER4][CAM2] :: Desiccant Cutting Right"	);
		case IDX_AREA5:		return _T("[VIEWER5][CAM3] :: Desiccant Material"		);
		
#elif RELEASE_4G
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: MBB Material"				);
		case IDX_AREA2:		return _T("[VIEWER2][CAM2] :: MBB Label"				);
		
#elif RELEASE_5G
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: Box Quality"				);
		case IDX_AREA2:		return _T("[VIEWER2][CAM2] :: Sealing Quality"			);
		
#elif RELEASE_6G
		case IDX_AREA1:		return _T("[VIEWER1][CAM1] :: Box Label"				);
		case IDX_AREA2:		return _T("[VIEWER2][CAM2] :: Box Tape"					);
		
#endif
		}
	}

	return _T("");
}

void MakeDirectory(LPCTSTR lpszDir)
{
	::SetLastError( 0 );
	CString strDir = lpszDir;
    if( strDir.Right(1) == _T("\\") )
		strDir = strDir.Left(strDir.GetLength()-1);
	if( ::GetFileAttributes(strDir) != -1 )
		return;

	DWORD dwErr = ::GetLastError();
	if( !((dwErr == ERROR_PATH_NOT_FOUND) || (dwErr == ERROR_FILE_NOT_FOUND)) )
	{
		::SetLastError( dwErr );
		return;
	}

    int nFound = strDir.ReverseFind( _T('\\') );
    MakeDirectory( strDir.Left(nFound) );
    ::CreateDirectory( strDir, NULL );
}

#ifndef _DEBUG
// Define the callback function that will be called on crash
BOOL WINAPI CrashCallback(LPVOID /*lpvState*/)
{  
	crAddScreenshot2(CR_AS_MAIN_WINDOW|CR_AS_USE_JPEG_FORMAT, 95);

	return TRUE;
}
#endif

BOOL CAPKApp::InitInstance()
{
	TCHAR szPath[MAX_PATH];
	SHGetFolderPath( NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, szPath );

	CString strPath = szPath;
	strPath += _T("\\APK\\Error" );
	MakeDirectory( strPath );

#ifndef _DEBUG

	// Define CrashRpt configuration parameters
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));  
	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = _T("APK");  
	info.pszAppVersion = _T("1.0");  
	info.pszEmailSubject = _T("APK 1.0 Error Report");  
	//info.pszEmailTo = _T("");
	info.pfnCrashCallback = CrashCallback;   
	info.uPriorities[CR_HTTP] = 3;  // First try send report over HTTP 
	info.uPriorities[CR_SMTP] = 2;  // Second try send report over SMTP  
	info.uPriorities[CR_SMAPI] = 1; // Third try send report over Simple MAPI    
	// Install all available exception handlers, use HTTP binary transfer encoding (recommended).
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING; 
	//info.dwFlags |= CR_INST_APP_RESTART;
	info.dwFlags |= CR_INST_STORE_ZIP_ARCHIVES;
	info.dwFlags |= CR_INST_DONT_SEND_REPORT;

	CString strErrorReportPath = strPath;
	info.pszErrorReportSaveDir = strErrorReportPath;

	// Install exception handlers
	int nResult = crInstall(&info);    
	if (nResult!=0)  
	{    
		// Something goes wrong. Get error message.
		TCHAR szErrorMsg[512];    
		szErrorMsg[0]=0;    
		crGetLastErrorMsg(szErrorMsg, 512);    
		TRACE("%s\n", szErrorMsg);
		::MessageBox( NULL, szErrorMsg, _T("APK"), MB_OK );
		return FALSE;
	} 
	
#endif

#ifndef _DEBUG
	m_hMutexOneInstance = ::CreateMutex( NULL, FALSE, _T("HITS@APK") ); 
	BOOL bFound = FALSE; 
	if ( ::GetLastError() == ERROR_ALREADY_EXISTS ) bFound = TRUE; 
	if (bFound) 
	{ 
		HWND hMDTWnd = ::FindWindow(NULL, _T("APK"));

		if ( hMDTWnd ) 
		{
			if ( ::IsWindowVisible( hMDTWnd ) )
			{
				::BringWindowToTop( hMDTWnd );
				::SetForegroundWindow( hMDTWnd );
				return FALSE;
			}
			else
			{
				DWORD dwProcessId;
				::GetWindowThreadProcessId( hMDTWnd, &dwProcessId );
				HANDLE hProcess = ::OpenProcess( PROCESS_TERMINATE, FALSE, dwProcessId );
				::TerminateProcess( hProcess, 0 );
				::CloseHandle( hProcess );
			}

		}
	} 
#endif

	XCommInitialize();

	CString strLogPath = GetExecuteDirectory() + _T("\\Log");
	MakeDirectory( (LPCTSTR)strLogPath );

	CString strSystemLogPath = GetExecuteDirectory() + _T("\\Log\\System");
	MakeDirectory( (LPCTSTR)strSystemLogPath );

	m_LogSystem.SetLogDirectory( strSystemLogPath );
	m_LogSystem.SetExpirePeriod( 1825 ); // 5년
	m_LogSystem.Start();

	//////////////////////////////////////////////////////////////////////////
	WRITE_LOG( WL_SYSTEM, _T("[Start System] ### [%s]"), GetExecuteFile() );

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//// 기본으로 생성되는 내용들
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	SetRegistryKey(_T("HITS"));
	LoadStdProfileSettings(0);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAPKDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CAPKView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CAPKApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	WRITE_LOG( WL_SYSTEM, _T("Exit System") );
	m_LogSystem.Stop();

	if ( m_hMutexOneInstance )
	{
		::ReleaseMutex( m_hMutexOneInstance );
		m_hMutexOneInstance = NULL;
	}

#ifndef _DEBUG
	crUninstall();
#endif

	return CWinApp::ExitInstance();
}

// CAPKApp 메시지 처리기
void CAPKApp::ExitProgram()
{
	OnAppExit();
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CAPKApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAPKApp 메시지 처리기

CxLogSystem& GetLogSystem()
{
	return theApp.GetLogSystem();
}

LPCTSTR ForLog( InspectType inspecttype )
{
	switch( inspecttype )
	{
#ifdef RELEASE_1G
	case InspectTypeTrayOcr:		return _T("InspectTrayOcr");			break;
	case InspectType3DChipCnt:		return _T("InspectType3DChip");			break;
	case InspectTypeChipOcr:		return _T("InspectTypeChipOcr");		break;
	case InspectTypeChip:			return _T("InspectTypeChip");			break;
	case InspectTypeMixing:			return _T("InspectTypeMixing");			break;
	case InspectTypeLiftFront:		return _T("InspectTypeLiftFront");		break;
	case InspectTypeLiftRear:		return _T("InspectTypeLiftRear"	);		break;
	
#elif RELEASE_SG
	case InspectTypeStackerOcr:		return _T("InspectTypeStackerOcr"); 	break;

#elif RELEASE_2G
	case InspectTypeBanding:		return _T("InspectTypeBanding");		break;
	case InspectTypeHIC:			return _T("InspectTypeHIC");			break;

#elif RELEASE_3G
	case InspectTypePositionLeft:		return _T("InspectTypePositionLeft");		break;
	case InspectTypePositionRight:		return _T("InspectTypePositionRight");		break;
	case InspectTypeDesiccantLeft:		return _T("InspectTypeDesiccantLeft");		break;
	case InspectTypeDesiccantRight:		return _T("InspectTypeDesiccantRight");		break;
	case InspectTypeMaterial:			return _T("InspectTypeMaterial");			break;

#elif RELEASE_4G
	case InspectTypeMBB:			return _T("InspectTypeMBB");			break;
	case InspectTypeLabel:			return _T("InspectTypeLabel");			break;

#elif RELEASE_5G
	case InspectTypeBox:			return _T("InspectTypeBox");			break;
	case InspectTypeSealing:		return _T("InspectTypeSealing");		break;

#elif  RELEASE_6G
	case InspectTypeLabel:			return _T("InspectTypeLabel");			break;
	case InspectTypeTape:			return _T("InspectTypeTape");			break;

#endif
	default:
		break;
	}

	return _T("");
}